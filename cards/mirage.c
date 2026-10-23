#include "manalink.h"

// ----- GENERAL FUNCTIONS ----

static int check_unattach_from_phasing(int player, int card, int mode){
	card_instance_t* instance = get_card_instance(player, card);
	if (instance->damage_target_player < 0 || instance->damage_target_card < 0){
		return 0;	// not attached
	}
	card_instance_t* attached_to = get_card_instance(instance->damage_target_player, instance->damage_target_card);
	if (attached_to->internal_card_id < 0){
		return 1;	// attachment neither in play nor phased
	}
	if ((attached_to->state & STATE_INVISIBLE) || !(attached_to->state & STATE_IN_PLAY)){
		return 1;	// attachment neither in play nor phased
	}

	int attached_to_is_phased;
	if (mode == 0){	// phasing immediately, so either attachment is phasing after what it's attached to, or attachment is phasing alone
		attached_to_is_phased = attached_to->state & STATE_OUBLIETTED;
	} else {	// mode == 2 - second pass of two-pass phasing
		if (attached_to->state & STATE_OUBLIETTED){
			if (check_special_flags(instance->damage_target_player, instance->damage_target_card, SF_PHASED_OUT|SF_PHASED_OUT_INDIRECTLY) == (SF_PHASED_OUT|SF_PHASED_OUT_INDIRECTLY)){
				attached_to_is_phased = 0;	// attached_to will be phasing in, but hasn't yet
			} else {
				attached_to_is_phased = 1;	// attached_to has already phased out
			}
		} else {
			if (check_special_flags(instance->damage_target_player, instance->damage_target_card, SF_PHASED_OUT|SF_PHASED_OUT_INDIRECTLY) == 0){
				attached_to_is_phased = 0;	// attached_to has already phased in, or isn't phasing at all
			} else {
				attached_to_is_phased = 1;	// attached_to will be phasing out, but hasn't yet
			}
		}
	}

	if (attached_to_is_phased){
		int this_is_phased = instance->state & STATE_OUBLIETTED;
		return (is_token(instance->damage_target_player, instance->damage_target_card)
				|| !this_is_phased);
	} else {
		return 0;
	}
}

static void maybe_unattach_from_phasing(int player, int card, int mode){
	if (check_unattach_from_phasing(player, card, mode)){
		card_instance_t* instance = get_card_instance(player, card);
		if (has_subtype(player, card, SUBTYPE_EQUIPMENT)){
			instance->targets[8].player = instance->targets[8].card = -1;	// raw unattach
		}
		instance->damage_target_player = instance->damage_target_card = -1;
	}
}

static void phase_in_impl(int player, int card, int sf2, int mode){
	// mode = 0: phase in now with all attachments
	// mode = 1: set both SF_PHASED_OUT and SF_PHASED_OUT_INDIRECTLY on this and all phased-out attachments
	// mode = 2: phase this in now by itself
	card_instance_t *instance = get_card_instance(player, card);
	if (instance->internal_card_id > 0
		&& (instance->state & (STATE_OUBLIETTED|STATE_INVISIBLE|STATE_IN_PLAY)) == (STATE_OUBLIETTED|STATE_IN_PLAY)
		&& check_special_flags(player, card, sf2) == sf2){

		if (mode == 1){
			set_special_flags(player, card, SF_PHASED_OUT | SF_PHASED_OUT_INDIRECTLY);
		} else {
			play_sound_effect(WAV_DESTROY);	// the exile sound, despite its name
			instance->state &= ~STATE_OUBLIETTED;
			remove_special_flags(player, card, SF_PHASED_OUT | SF_PHASED_OUT_INDIRECTLY);
			if (get_id(player, card) == CARD_ID_TEFERIS_IMP){
				draw_cards(player, 1);
			}
			if (get_id(player, card) == CARD_ID_WARPING_WURM){
				add_1_1_counter(player, card);
			}
			if (get_id(player, card) == CARD_ID_SHIMMERING_EFREET){
				target_definition_t td;
				default_target_definition(player, card, &td, TYPE_CREATURE);
				td.allow_cancel = 0;

				if (can_target(&td) && pick_target(&td, "TARGET_CREATURE_PHASEOUT")){
					phase_out(instance->targets[0].player, instance->targets[0].card);
				}
			}
		}

		// attached auras, equipment, fortifications
		if (mode != 2){
			int p, c;
			for (p = 0; p < 2; ++p){
				for (c = 0; c < active_cards_count[p]; ++c){
					instance = get_card_instance(p, c);
					if (instance->internal_card_id > 0
						&& (instance->state & (STATE_OUBLIETTED|STATE_INVISIBLE|STATE_IN_PLAY)) == (STATE_OUBLIETTED|STATE_IN_PLAY)
						&& check_special_flags(p, c, SF_PHASED_OUT_INDIRECTLY)
						&& instance->damage_target_player == player && instance->damage_target_card == card){
						phase_in_impl(p, c, SF_PHASED_OUT_INDIRECTLY, mode);
					}
				}
			}
		}

		if (mode != 1){
			maybe_unattach_from_phasing(player, card, mode);
		}
	}
}

#if 0
void phase_in(int player, int card){
	phase_in_impl(player, card, SF_PHASED_OUT, 0);
}
#endif

static int phase_out_impl(int player, int card, int sf2, int mode){
	// mode = 0: phase out now with all attachments
	// mode = 1: set sf2 on this and SF_PHASED_OUT_INDIRECTLY on all phased-in attachments
	// mode = 2: phase this out now by itself, removing SF_PHASED_OUT_INDIRECTLY if both it and SF_PHASED_OUT are set
	if (in_play(player, card) && !check_special_flags(player, card, SF_CANNOT_PHASE_OUT)){
		card_instance_t* instance = get_card_instance(player, card);

		if (mode != 1){
			play_sound_effect(WAV_DESTROY);	// the exile sound, despite its name
			instance->state |= STATE_OUBLIETTED;
			if (get_id(player, card) == CARD_ID_TEFERIS_IMP){
				discard(player, 0, player);
			}
			if (get_id(player, card) == CARD_ID_ERTAIS_FAMILIAR){
				mill(player, 3);
			}
		}

		if ((sf2 & SF_PHASED_OUT) && (sf2 & SF_PHASED_OUT_INDIRECTLY)){
			sf2 &= ~SF_PHASED_OUT;
		}

		set_special_flags(player, card, sf2);

		if (mode != 2){
			// attached auras, equipment, fortifications
			int p, c;
			for (p = 0; p < 2; ++p){
				for (c = 0; c < active_cards_count[p]; ++c){
					if (in_play(p, c)){
						card_instance_t* attachment = get_card_instance(p, c);
						if (attachment->damage_target_player == player && attachment->damage_target_card == card){
							if (!(cards_data[attachment->internal_card_id].type & TYPE_EFFECT)){
								phase_out_impl(p, c, SF_PHASED_OUT_INDIRECTLY, mode);
							}
						}
					}
				}
			}
		}

		return 1;
	} else {
		return 0;
	}
}

int phase_out(int player, int card){
	return phase_out_impl(player, card, SF_PHASED_OUT, 0);
}

// There's some mild risk in checking results of new events; picking an unlikely number mitigates it some.
#define RESULT_PHASED_OUT	0x3a962ca
int phasing(int player, int card, event_t event){

	if (event == EVENT_PHASING){
		return phase_out_impl(player, card, SF_PHASED_OUT, 1) ? RESULT_PHASED_OUT : 0;
	}

	return 0;
}

void untap_phasing(int player, int shimmering_lands){
	/* This needs to be done in two passes to deal with the pathological case of a directly-phased-out equipment attached to a creature with phasing always
	 * phases in.  Otherwise it could phase in itself, and then immediately phase back out with the creature. */

	// First pass - just mark cards to phase in or out, so they really do happen simultaneously, and only once
	int c, any_phasing = 0;
	for (c = 0; c < active_cards_count[player]; ++c){
		card_instance_t* instance = get_card_instance(player, c);
		if (instance->internal_card_id >= 0){
			int state = instance->state & (STATE_OUBLIETTED | STATE_INVISIBLE | STATE_IN_PLAY);
			if (state == (STATE_OUBLIETTED | STATE_IN_PLAY)){
				phase_in_impl(player, c, SF_PHASED_OUT, 1);
				any_phasing = 1;
				continue;
			}

			if (shimmering_lands && is_what(player, c, TYPE_LAND)){
				static int shimmer_subtypes[] = {
					SUBTYPE_FOREST,
					SUBTYPE_ISLAND,
					SUBTYPE_MOUNTAIN,
					SUBTYPE_PLAINS,
					SUBTYPE_SWAMP,
					SUBTYPE_DESERT,
					SUBTYPE_GATE,
					SUBTYPE_LAIR,
					SUBTYPE_LOCUS,
					SUBTYPE_MINE,
					SUBTYPE_POWER_PLANT,
					SUBTYPE_TOWER,
					SUBTYPE_URZAS,
					-1,
				};
				int i;
				for (i = 0; shimmer_subtypes[i] >= 0; ++i){
					if ((shimmering_lands & (1<<i)) && has_subtype(player, c, shimmer_subtypes[i])){
						phase_out_impl(player, c, SF_PHASED_OUT, 1);
						any_phasing = 1;
						continue;
					}
				}
			}

			if (state == STATE_IN_PLAY	// the same as normal in_play(), when combined with the checks for internal_card_id, STATE_OUBLIETTED, STATE_INVISIBLE
				&& call_card_function(player, c, EVENT_PHASING) == RESULT_PHASED_OUT){
				any_phasing = 1;
			}
		}
	}

	if (any_phasing){
		// Second pass - phase all marked cards
		for (c = 0; c < active_cards_count[player]; ++c){
			card_instance_t* instance = get_card_instance(player, c);
			if (instance->internal_card_id >= 0){
				int state = instance->state & (STATE_OUBLIETTED | STATE_INVISIBLE | STATE_IN_PLAY);

				// Phase in only if phased out and marked both (from before)
				if (state == (STATE_OUBLIETTED | STATE_IN_PLAY)){
					phase_in_impl(player, c, SF_PHASED_OUT|SF_PHASED_OUT_INDIRECTLY, 2);
					continue;
				}

				// Phase out only if phased in and marked either or both (from before)
				if (state == STATE_IN_PLAY){
					int flags = check_special_flags(player, c, SF_PHASED_OUT|SF_PHASED_OUT_INDIRECTLY);
					if (flags){
						phase_out_impl(player, c, flags, 2);
					}
				}
			}
		}
	}
}

static int old_fetch(int player, int card, event_t event, subtype_t sub1, subtype_t sub2){
	comes_into_play_tapped(player, card, event);
	return fetchland(player, card, event, sub1, sub2, 0);
}

static int generic_mirage_knight(int player, int card, event_t event, int manacolor){

	flanking(player, card, event);

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_CAN_ACTIVATE && can_use_activated_abilities(player, card)  && has_mana(player, manacolor, 2) ){
		return 1;
	}
	else if( event == EVENT_ACTIVATE){
			charge_mana(player, manacolor, 2);
	}

	else if( event == EVENT_RESOLVE_ACTIVATION ){
			pump_ability_until_eot(player, card, player, instance->parent_card, 0, 0, KEYWORD_FIRST_STRIKE, 0);
	}

	return 0;
}

static int mirage_diamonds(int player, int card, event_t event){

	comes_into_play_tapped(player, card, event);

	return mana_producer(player, card, event);
}

int mirage_guildmage(int player, int card, event_t event, int color1, int color2){
/*Armorer Guildmage	|R
|B, |T: Target creature gets +1/+0 until end of turn.
|G, |T: Target creature gets +0/+1 until end of turn.
Civic Guildmage	|W
|G, |T: Target creature gets +0/+1 until end of turn.
|U, |T: Put target creature you control on top of its owner's library.
Granger Guildmage	|G	0x2003149
|R, |T: ~ deals 1 damage to any target and 1 damage to you.
|W, |T: Target creature gains first strike until end of turn.
Shadow Guildmage	|B	0x200314e
|U, |T: Put target creature you control on top of its owner's library.
|R, |T: ~ deals 1 damage to any target and 1 damage to you.
Shaper Guildmage	|U
|W, |T: Target creature gains first strike until end of turn.
|B, |T: Target creature gets +1/+0 until end of turn. */	 
	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_CREATURE);
	td1.preferred_controller = player;
	td1.allowed_controller = player;

	target_definition_t td3;
	default_target_definition(player, card, &td3, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	td3.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	enum{
		CHOICE_PUMP_P = 1,
		CHOICE_TOP_DECK,
		CHOICE_PUMP_T,
		CHOICE_DAMAGE,
		CHOICE_FIRST_STRIKE
	};

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		if( color1 == COLOR_BLACK || color2 == COLOR_BLACK ){
			if( generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_B(1), 0, &td, NULL) ){
				return 1;
			}
		}
		if( color1 == COLOR_BLUE || color2 == COLOR_BLUE ){
			if( generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_U(1), 0, &td1, NULL) ){
				return 1;
			}
		}
		if( color1 == COLOR_GREEN || color2 == COLOR_GREEN ){
			if( generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_G(1), 0, &td, NULL) ){
				return 1;
			}
		}
		if( color1 == COLOR_RED || color2 == COLOR_RED ){
			if( generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_R(1), 0, &td3, NULL) ){
				return 1;
			}
		}
		if( color1 == COLOR_WHITE || color2 == COLOR_WHITE ){
			if( generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_W(1), 0, &td, NULL) ){
				return 1;
			}
		}
	}

	if(event == EVENT_ACTIVATE ){
		int abilities[5] ={ (( color1 == COLOR_BLACK || color2 == COLOR_BLACK ) && generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_B(1), 0, &td, NULL)) ? 1 : 0,
							(( color1 == COLOR_BLUE || color2 == COLOR_BLUE ) && generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_U(1), 0, &td1, NULL)) ? 1 : 0,
							(( color1 == COLOR_GREEN || color2 == COLOR_GREEN ) && generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_G(1), 0, &td, NULL)) ? 1 : 0,
							(( color1 == COLOR_RED || color2 == COLOR_RED ) && generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_R(1), 0, &td3, NULL)) ? 1 : 0,
							(( color1 == COLOR_WHITE || color2 == COLOR_WHITE ) && generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_W(1), 0, &td, NULL)) ? 1 : 0,
		};
		int choice = DIALOG(player, card, event, DLG_RANDOM, DLG_NO_STORAGE, DLG_OMIT_ILLEGAL,
							"Pump power", abilities[0], current_phase == PHASE_AFTER_BLOCKING ? 8 : 1,
							"Put on top of deck", abilities[1], 5,
							"Pump toughness", abilities[2], 5,
							"Damage target & yourself", abilities[3], 10,
							"Give First Strike", abilities[4], current_phase == PHASE_AFTER_BLOCKING ? 8 : 1);
		if( ! choice ){
			spell_fizzled = 1;
			return 0;
		}
		instance->number_of_targets = instance->info_slot = 0;
		if( charge_mana_for_activated_ability(player, card, 0,
															(choice == CHOICE_PUMP_P ? 1 : 0),
															(choice == CHOICE_TOP_DECK ? 1 : 0),
															(choice == CHOICE_PUMP_T ? 1 : 0),
															(choice == CHOICE_DAMAGE ? 1 : 0),
															(choice == CHOICE_FIRST_STRIKE ? 1 : 0))
		  ){
			if( choice == CHOICE_PUMP_P ){
				if( pick_target(&td, "TARGET_CREATURE") ){
					add_state(player, card, STATE_TAPPED);
				}
			}
			if( choice == CHOICE_TOP_DECK ){
				if( new_pick_target(&td1, "Select target creature you control.", 0, 1 | GS_LITERAL_PROMPT) ){
					add_state(player, card, STATE_TAPPED);
				}
			}
			if( choice == CHOICE_PUMP_T ){
				if( pick_target(&td, "TARGET_CREATURE") ){
					add_state(player, card, STATE_TAPPED);
				}
			}
			if( choice == CHOICE_DAMAGE ){
				if( pick_target(&td3, "TARGET_ANY") ){
					add_state(player, card, STATE_TAPPED);
				}
			}
			if( choice == CHOICE_FIRST_STRIKE ){
				if( pick_target(&td, "TARGET_CREATURE") ){
					add_state(player, card, STATE_TAPPED);
				}
			}
			instance->info_slot = choice;
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( instance->info_slot == CHOICE_PUMP_P && valid_target(&td) ){
			pump_until_eot(instance->parent_controller, instance->parent_card, instance->targets[0].player, instance->targets[0].card, 1, 0);
		}
		if( instance->info_slot == CHOICE_TOP_DECK && valid_target(&td1) ){
			put_on_top_of_deck(instance->targets[0].player, instance->targets[0].card);
		}
		if( instance->info_slot == CHOICE_PUMP_T && valid_target(&td) ){
			pump_until_eot(instance->parent_controller, instance->parent_card, instance->targets[0].player, instance->targets[0].card, 0, 1);
		}
		if( instance->info_slot == CHOICE_DAMAGE && valid_target(&td3) ){
			damage_target0(player, card, 1);
			damage_player(instance->parent_controller, 1, instance->parent_controller, instance->parent_card);
		}
		if( instance->info_slot == CHOICE_FIRST_STRIKE && valid_target(&td) ){
			pump_ability_until_eot(instance->parent_controller, instance->parent_card, instance->targets[0].player, instance->targets[0].card,
									0, 0, KEYWORD_FIRST_STRIKE, 0);
		}
	}

	return 0;
}

// ---- CARDS ----
int card_abyssal_hunter(int player, int card, event_t event){
/* CARD_ID_ABYSSAL_HUNTER	2386
Abyssal Hunter	|3|B
Creature - Human Assassin 1/1
|B, |T: Tap target creature. ~ deals damage equal to ~'s power to that creature. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			tap_card(instance->targets[0].player, instance->targets[0].card);
			damage_creature_or_player(player, card, event, get_power(player, instance->parent_card));
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_B(1), 0, &td, "TARGET_CREATURE");
}

static int acidic_dagger_effect(int player, int card, event_t event){

	if( get_card_instance(player, card)->damage_target_player > -1 ){
		card_instance_t *instance = get_card_instance(player, card);

		int p = instance->damage_target_player;
		int c = instance->damage_target_card;

		if( event == EVENT_CHANGE_TYPE && affect_me(p, c) )
			get_card_instance(p, c)->destroys_if_blocked |= DIFB_DESTROYS_ALL;

		card_instance_t *damage= get_card_instance(affected_card_controller, affected_card);
		if( damage->internal_card_id == damage_card ){
			if( damage->damage_target_card != -1 && damage->damage_source_player == p && damage->damage_source_card == c &&
				damage->info_slot > 0 &&! has_subtype(damage->damage_target_player, damage->damage_target_card, SUBTYPE_WALL) )
			{
				if( instance->targets[1].player < 2 ){
					instance->targets[1].player = 2;
				}
				int pos = instance->targets[1].player;
				if( pos < 10 ){
					instance->targets[pos].player = damage->damage_target_player;
					instance->targets[pos].card = damage->damage_target_card;
					instance->targets[2].player++;
				}
			}
		}
		if( instance->targets[1].player > 2 && trigger_condition == TRIGGER_DEAL_DAMAGE && affect_me(player, card) &&
			reason_for_trigger_controller == player )
		{
			if(event == EVENT_TRIGGER ){
				event_result |= RESOLVE_TRIGGER_MANDATORY;
			}
			else if(event == EVENT_RESOLVE_TRIGGER){
					int i;
					for(i=2;i<instance->targets[1].player;i++){
						if( in_play(instance->targets[i].player, instance->targets[i].card) &&
							! has_subtype(instance->targets[i].player, instance->targets[i].card, SUBTYPE_WALL) )
						{
							kill_card(instance->targets[i].player, instance->targets[i].card, KILL_DESTROY);
						}
					}
					instance->targets[1].player = 2;
			}
		}

		if( other_leaves_play(player, card, p, c, event) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_SACRIFICE);
		}
	}

	if( event == EVENT_CLEANUP ){
		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

static void ai_helper_for_acidic_dagger(int player, int card, int target_pos){
	card_instance_t *instance = get_card_instance(player, card);
	int csvid = get_id(instance->targets[target_pos].player,  instance->targets[target_pos].card);
	switch( csvid ){
			// Creatures that allow you to draw a card when they damage a player
			case CARD_ID_THIEVING_MAGPIE:
			case CARD_ID_SHADOWMAGE_INFILTRATOR:
			case CARD_ID_HYSTRODON:
			case CARD_ID_JESKAI_ELDER:
			case CARD_ID_NINJA_OF_THE_DEEP_HOURS:
			case CARD_ID_OHRAN_VIPER:
			case CARD_ID_GLINT_EYE_NEPHILIM:
			case CARD_ID_JHESSIAN_THIEF:
			case CARD_ID_MARCHESAS_INFILTRATOR:
			case CARD_ID_SCROLL_THIEF:
			case CARD_ID_SILVERPELT_WEREWOLF:
			case CARD_ID_STEALER_OF_SECRETS:
			case CARD_ID_THASSAS_EMISSARY:
				ai_modifier += 48;
			default:
				break;
	}

	if( check_for_ability(instance->targets[target_pos].player, instance->targets[target_pos].card, KEYWORD_INFECT) ){
		ai_modifier += 48;
	}
}

int card_acidic_dagger(int player, int card, event_t event){
/*
Acidic Dagger |4
Artifact
{4}, {T}: Whenever target creature deals combat damage to a non-Wall creature this turn, destroy that non-Wall creature.
		When the targeted creature leaves the battlefield this turn, sacrifice Acidic Dagger.
		Activate this ability only before blockers are declared.
*/
	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		if( current_phase > PHASE_BEFORE_BLOCKING ){
			return 0;
		}
	}

	if( event == EVENT_ACTIVATE ){
		generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_X(4), 0, &td, "TARGET_CREATURE");
		if( spell_fizzled != 1 ){
			ai_helper_for_acidic_dagger(player, card, 0);
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			int legacy = create_targetted_legacy_effect(instance->parent_controller, instance->parent_card, &acidic_dagger_effect,
														instance->targets[0].player,  instance->targets[0].card);
			card_instance_t *leg = get_card_instance(instance->parent_controller, legacy);
			leg->targets[0].player = instance->parent_controller;
			leg->targets[0].card = instance->parent_card;
			leg->number_of_targets = 1;
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_X(4), 0, &td, "TARGET_CREATURE");
}

int card_afiya_grove(int player, int card, event_t event){
/*
Afiya Grove |1|G
Enchantment, 1G (2)
Afiya Grove enters the battlefield with three +1/+1 counters on it.
At the beginning of your upkeep, move a +1/+1 counter from Afiya Grove onto target creature.
When Afiya Grove has no +1/+1 counters on it, sacrifice it.
*/

	enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1, 3);

	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		if( count_1_1_counters(player, card) ){
			target_definition_t td;
			default_target_definition(player, card, &td, TYPE_CREATURE);
			td.preferred_controller = player;
			td.allow_cancel = 0;

			card_instance_t *instance = get_card_instance(player, card);

			if( pick_target(&td, "TARGET_CREATURE") ){
				move_counters(instance->targets[0].player,  instance->targets[0].card, player, card, COUNTER_P1_P1, 1);
			}
			instance->number_of_targets = 0;
		}
	}

	if( event == EVENT_STATIC_EFFECTS && in_play(player, card) && ! is_humiliated(player, card) && count_1_1_counters(player, card) < 1 ){
		kill_card(player, card, KILL_SACRIFICE);
	}

	return global_enchantment(player, card, event);
}

int card_aleatory(int player, int card, event_t event){
/* CARD_ID_ALEATORY	2392
Aleatory	|1|R
Instant
Cast ~ only during combat after blockers are declared.
Flip a coin. If you win the flip, target creature gets +1/+1 until end of turn.
Draw a card at the beginning of the next turn's upkeep. */

  // Cast ~ only during combat after blockers are declared.
  if (event == EVENT_CAN_CAST)
	return current_phase >= PHASE_AFTER_BLOCKING && current_phase < PHASE_MAIN2;

  // Flip a coin. If you win the flip, target creature gets +1/+1 until end of turn.
  // Draw a card at the beginning of the next turn's upkeep.
  target_definition_t td;
  default_target_definition(player, card, &td, TYPE_CREATURE);
  td.preferred_controller = player;

  if (event == EVENT_RESOLVE_SPELL)
	{
	  if (valid_target(&td))
		{
		  if (flip_a_coin(player, card))
			{
			  card_instance_t* instance = get_card_instance(player, card);
			  alternate_legacy_text(2, player, pump_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, 1,1));
			}
		  alternate_legacy_text(1, player, cantrip(player, card, 1));
		}
	  kill_card(player, card, KILL_DESTROY);
	}

  return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
}

int card_amber_prison(int player, int card, event_t event)
{
  /* Amber Prison	|4
   * Artifact
   * You may choose not to untap ~ during your untap step.
   * |4, |T: Tap target artifact, creature, or land. That permanent doesn't untap during its controller's untap step for as long
   * 		as ~ remains tapped. */

  choose_to_untap(player, card, event);

  if (!IS_GAA_EVENT(event))
	return 0;

  target_definition_t td;
  default_target_definition(player, card, &td, TYPE_ARTIFACT | TYPE_CREATURE | TYPE_LAND);

  if (event == EVENT_RESOLVE_ACTIVATION && valid_target(&td))
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  int pp = instance->parent_controller, pc = instance->parent_card;

	  tap_card(instance->targets[0].player, instance->targets[0].card);
	  does_not_untap_until_im_tapped(pp, pc, instance->targets[0].player, instance->targets[0].card);

	  card_instance_t* parent = get_card_instance(pp, pc);
	  parent->targets[1].player = instance->targets[0].player;
	  parent->targets[1].card = instance->targets[0].card;
	}

  return generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_TARGET, MANACOST_X(4), 0, &td, "ICY_MANIPULATOR");	//"Select target artifact, creature, or land."
}

int card_amulet_of_unmaking(int player, int card, event_t event){
/* CARD_ID_AMULET_OF_UNMAKING	2394
Amulet of Unmaking	|5
Artifact
|5, |T, Exile ~: Exile target artifact, creature, or land. Activate this ability only any time you could cast a sorcery. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_ARTIFACT | TYPE_CREATURE | TYPE_LAND);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_SORCERY_BE_PLAYED | GAA_CAN_TARGET | GAA_RFG_ME |
									GAA_LITERAL_PROMPT, MANACOST_X(5), 0, &td, "Select target artifact, creature or land.");
}

int card_afterlife(int player, int card, event_t event){
	/* Afterlife	|2|W
	 * Instant
	 * Destroy target creature. It can't be regenerated. Its controller puts a 1/1 |Swhite Spirit creature token with flying onto the battlefield. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);

			token_generation_t token;
			default_token_definition(player, card, CARD_ID_SPIRIT, &token);
			token.t_player = instance->targets[0].player;
			token.key_plus = KEYWORD_FLYING;
			token.color_forced = get_sleighted_color_test(player, card, COLOR_TEST_WHITE);
			generate_token(&token);
		}
		kill_card(player, card, KILL_DESTROY);
   }

   return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
}

int card_armorer_guildmage(int player, int card, event_t event){
/* CARD_ID_ARMORER_GUILDMAGE	2397
Armorer Guildmage	|R
Creature - Human Wizard 1/1
|B, |T: Target creature gets +1/+0 until end of turn.
|G, |T: Target creature gets +0/+1 until end of turn. */

	return mirage_guildmage(player, card, event, COLOR_BLACK, COLOR_GREEN);
}

int card_ashen_powder(int player, int card, event_t event){
/* CARD_ID_ASHEN_POWDER	2398
Ashen Powder	|2|B|B
Sorcery
Put target creature card from an opponent's graveyard onto the battlefield under your control. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	char msg[100] = "Select a creature card.";
	test_definition_t this_test;
	new_default_test_definition(&this_test, TYPE_CREATURE, msg);

	if( event == EVENT_RESOLVE_SPELL ){
		int selected = validate_target_from_grave_source(player, card, 1-player, 0);
		if( selected != -1 ){
			reanimate_permanent(player, card, 1-player, selected, REANIMATE_DEFAULT);
		}
		kill_card(player, card, KILL_SACRIFICE);
	}

	return generic_spell(player, card, event, GS_GRAVE_RECYCLER_OPP_GRAVE, NULL, NULL, 1, &this_test);
}

int card_asmira_holy_avenger(int player, int card, event_t event){
	// original code : 0x453CE0

	/* Asmira, Holy Avenger	|2|G|W
	 * Legendary Creature - Human Cleric 2/3
	 * Flying
	 * At the beginning of each end step, put a +1/+1 counter on ~ for each creature put into your graveyard from the battlefield this turn. */

	check_legend_rule(player, card, event);

	if( eot_trigger(player, card, event) ){
		add_1_1_counters(player, card, get_dead_count(player, TYPE_CREATURE));
	}

	return 0;
}

int card_auspicious_ancestor(int player, int card, event_t event){
/* CARD_ID_AUSPICIOUS_ANCESTOR	2399
Auspicious Ancestor	|3|W
Creature - Human Cleric 2/3
When ~ dies, you gain 3 life.
Whenever a player casts a |Swhite spell, you may pay |1. If you do, you gain 1 life. */

	if( trigger_condition == TRIGGER_SPELL_CAST && affect_me(player, card) ){
		test_definition_t test;
		default_test_definition(&test, TYPE_ANY);
		test.color = get_sleighted_color_test(player, card, COLOR_TEST_WHITE);
		if( new_specific_spell_played(player, card, event, player, RESOLVE_TRIGGER_AI(player), &test) ){
			if( has_mana(player, COLOR_ANY, 1) && charge_mana_while_resolving(player, card, EVENT_RESOLVE_TRIGGER, player, COLOR_ANY, 1) ){
				gain_life(player, 1);
			}
		}
	}

	if( this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY) ){
		gain_life(player, 3);
	}

	return 0;
}

int card_bad_river(int player, int card, event_t event){
	/* Bad River	""
	 * Land
	 * ~ enters the battlefield tapped.
	 * |T, Sacrifice ~: Search your library for |Han Island or |H2Swamp card and put it onto the battlefield. Then shuffle your library. */
	return old_fetch(player, card, event, SUBTYPE_ISLAND, SUBTYPE_SWAMP);
}

static int barbed_foliage_effect(int player, int card, event_t event){

	if( get_card_instance(player, card)->damage_target_player > -1 ){
		card_instance_t *instance = get_card_instance(player, card);

		int p = instance->damage_target_player;
		int c = instance->damage_target_card;

		if( event == EVENT_ABILITIES && affect_me(p, c) ){
			remove_special_ability(p, c, SP_KEYWORD_FLANKING);
		}
	}

	if( current_phase == PHASE_CLEANUP ){
		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

int card_barbed_foliage(int player, int card, event_t event)
{
/* CARD_ID_BARBED_FOLIAGE	2402
Barbed Foliage	|2|G|G
Enchantment
Whenever a creature attacks you, it loses flanking until end of turn.
Whenever a creature without flying attacks you, ~ deals 1 damage to it. */

	int amt;
	if ((amt = declare_attackers_trigger(player, card, event, DAT_TRACK | DAT_ATTACKS_PLAYER, 1-player, -1)))
	{
		card_instance_t* instance = get_card_instance(player, card);
		unsigned char* attackers = (unsigned char*)(&instance->targets[2].player);
		for (--amt; amt >= 0; --amt){
			if (in_play(current_turn, attackers[amt]) )
			{
				if (check_for_special_ability(current_turn, attackers[amt], SP_KEYWORD_FLANKING))
				{
					create_targetted_legacy_effect(player, card, &barbed_foliage_effect, current_turn, attackers[amt]);
				}

				if (!check_for_ability(current_turn, attackers[amt], KEYWORD_FLYING) )
				{
					damage_creature(current_turn, attackers[amt], 1, player, card);
				}
			}
		}
	}

  return global_enchantment(player, card, event);
}

// wall token --> vanilla

static int basalt_golem_legacy(int player, int card, event_t event){

	if( get_card_instance(player, card)->damage_target_player > -1 ){
		card_instance_t *instance = get_card_instance(player, card);

		int p = instance->damage_target_player;
		int c = instance->damage_target_card;

		if( end_of_combat_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY) ){
			if( can_cause_sacrifice(player, p) ){
				int enc_p = p;
				int enc_c = c;
				instance->damage_target_player = instance->damage_target_card = -1;
				kill_card(enc_p, enc_c, KILL_SACRIFICE);
				token_generation_t token;
				default_token_definition(player, card, CARD_ID_WALL, &token);
				token.t_player = enc_p;
				token.pow = 0;
				token.tou = 2;
				token.action = TOKEN_ACTION_CONVERT_INTO_ARTIFACT;
				generate_token(&token);
			}
			kill_card(player, card, KILL_EXILE);
		}
	}

	return 0;
}

static void stoning_at_end_of_combat(int player, int card, int t_player, int t_card)
{
	create_targetted_legacy_effect(player, card, &basalt_golem_legacy, t_player, t_card);
}

int card_basalt_golem(int player, int card, event_t event){
/* CARD_ID_BASALT_GOLEM	2405
Basalt Golem	|5
Artifact Creature - Golem 2/4
~ can't be blocked by artifact creatures.
Whenever ~ becomes blocked by a creature, that creature's controller sacrifices it at end of combat.
 If the player does, he or she creates a 0/2 colorless Wall artifact creature token with defender. */

	if( event == EVENT_CHANGE_TYPE && affect_me(player, card) && !is_humiliated(player, card) )
		get_card_instance(player, card)->destroys_if_blocked |= DIFB_DESTROYS_ALL;

	if( event == EVENT_BLOCK_LEGALITY && in_play(player, card) && ! is_humiliated(player, card) ){
		if( player == attacking_card_controller && card == attacking_card ){
			if( is_what(affected_card_controller, affected_card, TYPE_ARTIFACT) ){
				event_result = 1;
			}
		}
	}

	if (event == EVENT_DECLARE_BLOCKERS && !is_humiliated(player, card))
	{
		if( player == current_turn && is_attacking(player, card) )
			for_each_creature_blocking_me(player, card, stoning_at_end_of_combat, player, card);
	}

	return 0;
}

int card_bazaar_of_wonders(int player, int card, event_t event){
/*
Bazaar of Wonders |3|U|U
World Enchantment
When Bazaar of Wonders enters the battlefield, exile all cards from all graveyards.
Whenever a player casts a spell, counter it if a card with the same name is in a graveyard or a nontoken permanent with the same name
	is on the battlefield.
*/
	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		APNAP(p, { rfg_whole_graveyard(p); };);
	}

	if( new_specific_spell_played(player, card, event, ANYBODY, RESOLVE_TRIGGER_MANDATORY, NULL) ){
		int id = get_id(trigger_cause_controller, trigger_cause);
		int found = 0;
		int i;
		for(i=0; i<2 && ! found; i++){
			int count = count_graveyard(i)-1;
			while( count > -1 && ! found){
					if( cards_data[get_grave(i)[count]].id == id ){
						found = 1;
						break;
					}
					count--;
			}
		}
		if( ! found ){
			for(i=0; i<2 && ! found; i++){
				int count = active_cards_count[i]-1;
				while( count > -1 && ! found){
						if( in_play(i, count) && is_what(i, count, TYPE_PERMANENT) && ! is_token(i, count) ){
							if( get_card_name(i, count) == id ){
								found = 1;
								break;
							}
						}
						count--;
				}
			}
		}
		if( found == 1 ){
			real_counter_a_spell(player, card, trigger_cause_controller, trigger_cause);
		}
	}

	return enchant_world(player, card, event);
}

int card_benevolent_unicorn(int player, int card, event_t event){
	/* Benevolent Unicorn	|1|W	0x2003013
	 * Creature - Unicorn 1/2
	 * If a spell would deal damage to a permanent or player, it deals that much damage minus 1 to that permanent or player instead. */
	// Used externally
	card_instance_t* damage = damage_being_dealt(event);
	if( damage && (damage->targets[3].player & TYPE_SPELL) && in_play(player, card) && ! is_humiliated(player, card) && damage->info_slot > 0 ){
		damage->info_slot -= 1;
	}
	return 0;
}

int card_benthic_djinn(int player, int card, event_t event){
/* CARD_ID_BENTHIC_DJINN	2409
Benthic Djinn	|2|U|B
Creature - Djinn 5/3
|H2Islandwalk (This creature can't be blocked as long as defending player controls |Han Island.)
At the beginning of your upkeep, you lose 2 life. */

	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		lose_life(player, 2);
	}

	return 0;
}

int bone_harvest_impl(int player, int card, event_t event, int draw_else_slowtrip){

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	test_definition_t test;
	new_default_test_definition(&test, TYPE_CREATURE, "Select any target creature cards.");

	card_instance_t* instance = get_card_instance(player, card);

	if (event == EVENT_CAN_CAST){
		return generic_spell(player, card, event, GS_GRAVE_RECYCLER, NULL, NULL, 1, &test);
	}

	if (event == EVENT_CAST_SPELL && affect_me(player, card)){
		int max_targ = MIN(count_graveyard_by_type(player, TYPE_CREATURE), 18);
		instance->info_slot = select_multiple_cards_from_graveyard(player, player, 0, AI_MAX_VALUE, &test, max_targ, &instance->targets[0]);
	}

	if (event == EVENT_RESOLVE_SPELL){
		int i, num_validated = 0, selected, num_targets = 0;
		for (i = 0; i < instance->info_slot; ++i){
			++num_targets;
			if( (selected = validate_target_from_grave(player, card, player, i)) != -1){
				int card_added = add_card_to_hand(player, get_grave(player)[selected]);
				remove_card_from_grave(player, selected);
				put_on_top_of_deck(player, card_added);
				++num_validated;
			}
		}

		if (num_validated > 1){
			rearrange_top_x(player, player, num_validated);
		}
		if (num_validated == 0 && num_targets > 0){
			spell_fizzled = 1;
		}
		else{
			 if( draw_else_slowtrip ){
				 draw_a_card(player);
			 }
			 else{
				  cantrip(player, card, 1);
			 }
		}

		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_bone_harvest(int player, int card, event_t event)
{
  /* Bone Harvest	|2|B
   * Instant
   * Put any number of target creature cards from your graveyard on top of your library.
   * Draw a card at the beginning of the next turn's upkeep. */
  return bone_harvest_impl(player, card, event, 0);
}

int card_bone_mask(int player, int card, event_t event){
/*
Bone Mask |4
Artifact
{2}, {T}: The next time a source of your choice would deal damage to you this turn, prevent that damage.
		Exile cards from the top of your library equal to the damage prevented this way.
*/
	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.extra = damage_card;
	td.special = TARGET_SPECIAL_DAMAGE_PLAYER;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			card_instance_t *target = get_card_instance(instance->targets[0].player, instance->targets[0].card);
			int amount = target->info_slot;
			prevent_damage(instance->targets[0].player, instance->targets[0].card, -1);
			while( amount && deck_ptr[player][0] != -1 ){
					rfg_top_card_of_deck(player);
					amount--;
			}
		}
	}

	return generic_activated_ability(player, card, event, GAA_DAMAGE_PREVENTION | GAA_CAN_TARGET | GAA_UNTAPPED, MANACOST_X(2), 0,
									&td, "TARGET_DAMAGE");
}

int card_burning_palm_efreet(int player, int card, event_t event){
/* CARD_ID_BURNING_PALM_EFREET	2420
Burning Palm Efreet	|2|R|R
Creature - Efreet 2/2
|1|R|R: ~ deals 2 damage to target creature with flying and that creature loses flying until end of turn. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.required_abilities = KEYWORD_FLYING;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			damage_target0(player, card, 2);
			negate_ability_until_eot(instance->parent_controller, instance->parent_card,
									instance->targets[0].player, instance->targets[0].card, KEYWORD_FLYING);
		}
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_LITERAL_PROMPT, MANACOST_XR(1, 2), 0,
									&td, "Select target creature with flying.");
}

int card_burning_shield_askari(int player, int card, event_t event){
/* CARD_ID_BURNING_SHIELD_ASKARI	2421
Burning Shield Askari	|2|R
Creature - Human Knight 2/2
Flanking (Whenever a creature without flanking blocks this creature, the blocking creature gets -1/-1 until end of turn.)
|R|R: ~ gains first strike until end of turn. */

	return generic_mirage_knight(player, card, event, COLOR_RED);
}

int card_cadaverous_bloom(int player, int card, event_t event){
/* CARD_ID_CADAVEROUS_BLOOM	1586
Cadaverous Bloom	|3|B|G
Enchantment
Exile a card from your hand: Add |B|B or |G|G to your mana pool. */

	if( event == EVENT_CAN_ACTIVATE && hand_count[player] > 0){
		return can_produce_mana(player, card);
	}

	if( event == EVENT_ACTIVATE ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_ANY, "Select a card to exile.");
		int selected = new_select_a_card(player, player, TUTOR_FROM_HAND, 0, AI_MIN_VALUE, -1, &test);
		if( selected > -1 ){
			put_on_top_of_deck(player, selected);
			rfg_top_card_of_deck(player);

			produce_mana_all_one_color(player, COLOR_TEST_BLACK | COLOR_TEST_GREEN, 2);
			tapped_for_mana_color = -2;
		}
		else{
			spell_fizzled = 1;
		}
	}

	if( event == EVENT_COUNT_MANA && affect_me(player, card) && hand_count[player] > 0 && can_produce_mana(player, card) ){
		/* The usual problems with lots of calls to declare_mana_available_hex, detailed in card_axebane_guardian() in return_to_ravnica.c, apply here.  This is
		 * a slightly different case, since the mana is produced in packets of 2.  The approximation I'm going with here declares mana individually for the
		 * first three cards (up to 6 mana), then all in one packet for any remaining, so you don't run out of declaration slots after a 30-card Prosperity. */
		switch (hand_count[player]){
			default:declare_mana_available_hex(player, COLOR_TEST_BLACK | COLOR_TEST_GREEN, 2 * (hand_count[player] - 3));	// intentionally fall through
			case 3: declare_mana_available_hex(player, COLOR_TEST_BLACK | COLOR_TEST_GREEN, 2);	// intentionally fall through
			case 2: declare_mana_available_hex(player, COLOR_TEST_BLACK | COLOR_TEST_GREEN, 2);	// intentionally fall through
			case 1: declare_mana_available_hex(player, COLOR_TEST_BLACK | COLOR_TEST_GREEN, 2);	break;
		}
	}
	return global_enchantment(player, card, event);
}

int card_cadaverous_knight(int player, int card, event_t event){
/* CARD_ID_CADAVEROUS_KNIGHT	2422
Cadaverous Knight	|2|B
Creature - Zombie Knight 2/2
Flanking (Whenever a creature without flanking blocks this creature, the blocking creature gets -1/-1 until end of turn.)
|1|B|B: Regenerate ~. */

	flanking(player, card, event);

	return regeneration(player, card, event, MANACOST_XB(1, 2));
}

static void canopy_dragon_ai_helper(int player, int card){
	int def_power = 0;
	int def_toughness = 0;
	CYCLE_ALL_CARDS(p, c, 	{
								if( p == player && in_play(p, c) && is_what(p, c, TYPE_CREATURE) ){
									if( ! check_for_ability(p, c, KEYWORD_FLYING) ||
										(check_for_ability(p, c, KEYWORD_FLYING) && check_for_special_ability(p, c, SP_KEYWORD_CANNOT_BLOCK)))
									{
										def_power += get_power(p, c);
										def_toughness += get_toughness(p, c);
									}
								}
							}
	);
	if( def_power < get_toughness(player, card) && def_toughness < get_power(player, card) ){
		ai_modifier += 48;
	}
}

int card_canopy_dragon(int player, int card, event_t event){
/*
Canopy Dragon |4|G|G
Creature — Dragon 4/4
Trample
{1}{G}: Canopy Dragon gains flying and loses trample until end of turn.
*/
	if( event == EVENT_ABILITIES && affect_me(player, card) && get_card_instance( player, card )->info_slot == 66 ){
		if( event_result & KEYWORD_TRAMPLE ){
			event_result &= ~KEYWORD_TRAMPLE;
		}
		event_result |= KEYWORD_FLYING;
	}

	if( event == EVENT_ACTIVATE ){
		if( current_phase < PHASE_DECLARE_BLOCKERS && get_card_instance( player, card )->info_slot != 66 ){
			canopy_dragon_ai_helper(player, card);
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION){
		card_instance_t *instance = get_card_instance( player, card );
		card_instance_t *parent = get_card_instance( instance->parent_controller, instance->parent_card );
		parent->info_slot = 66;
	}

	if( event == EVENT_CLEANUP ){
		get_card_instance( player, card )->info_slot = 0;
	}

	return generic_activated_ability(player, card, event, 0, MANACOST_XG(1, 1), 0, NULL, NULL);
}

static int check_for_carrion_target_for_ai(int player, int card){
	if( land_can_be_played & LCBP_REGENERATION ){
		int p = player;
		int c;
		for(c=0; c<active_cards_count[c]; c++){
			if( in_play(p, c) && is_what(p, c, TYPE_CREATURE) && ! check_special_flags2(p, c, SF2_CANNOT_BE_SACRIFICED) ){
				int kill_code = get_card_instance(p, c)->kill_code;
				if( kill_code >= KILL_DESTROY && kill_code < KILL_EXILE ){
					return 99;
				}
			}
		}
	}
	return 0;
}

int card_carrion(int player, int card, event_t event){
/* Carrion	|1|B|B
 * Instant
 * As an additional cost to cast ~, sacrifice a creature.
 * Put X 0/1 |Sblack Insect creature tokens onto the battlefield, where X is the sacrificed creature's power. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	test_definition_t test;
	new_default_test_definition(&test, TYPE_CREATURE, "Select a creature to sacrifice.");

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		if( basic_spell(player, card, event) && new_can_sacrifice_as_cost(player, card, &test) ){
			if( ! IS_AI(player) ){
				return 1;
			}
			else{
				int result = check_for_carrion_target_for_ai(player, card);
				return result ? result : 1;
			}
		}
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		int sac = -1;
		int sac_destroyed = check_for_carrion_target_for_ai(player, card);
		if( ! IS_AI(player) || ! sac_destroyed ){
			sac = new_sacrifice(player, card, player, SAC_JUST_MARK|SAC_AS_COST|SAC_RETURN_CHOICE, &test);
			int pow = get_power(BYTE2(sac), BYTE3(sac));
			if( pow == 0 ){
				ai_modifier -=128;
			}
			// Some more AI, really narrow
			int upkeep_eater = 0;
			int booster = 0;
			CYCLE_ALL_CARDS(p, c, 	{
										if( p == player && in_play(p, c) && ! is_humiliated(p, c) ){
											int csvid = get_id(p, c);
											switch( csvid ){
													case CARD_ID_LORD_OF_THE_PIT:
													case CARD_ID_ABHORRENT_OVERLORD:
													case CARD_ID_DEVOURING_STROSSUS:
													case CARD_ID_DOOMGAPE:
													case CARD_ID_ELDRAZI_MONUMENT:
													case CARD_ID_LIEGE_OF_THE_PIT:
													case CARD_ID_SMOTHERING_ABOMINATION:
													case CARD_ID_XATHRID_DEMON:
														upkeep_eater = 1;
														goto end;
													case CARD_ID_ASCENDANT_EVINCAR:
													case CARD_ID_BAD_MOON:
													case CARD_ID_ASHENMOOR_LIEGE:
													case CARD_ID_CREAKWOOD_LIEGE:
													case CARD_ID_DEATHBRINGER_LIEGE:
													case CARD_ID_GLEN_ELENDRA_LIEGE:
													case CARD_ID_PARAGON_OF_OPEN_GRAVES:
													case CARD_ID_BEASTMASTER_ASCENSION:
													case CARD_ID_COLLECTIVE_BLESSING:
													case CARD_ID_DEATH_PIT_OFFERING:
													case CARD_ID_DICTATE_OF_HELIOD:
													case CARD_ID_GAEAS_ANTHEM:
													case CARD_ID_GLORIOUS_ANTHEM:
													case CARD_ID_GLORY_OF_WARFARE:
													case CARD_ID_GRUUL_WAR_CHANT:
													case CARD_ID_HOMURAS_ESSENCE:
													case CARD_ID_MARSHALS_ANTHEM:
													case CARD_ID_MIRARIS_WAKE:
													case CARD_ID_ORCISH_ORIFLAMME:
													case CARD_ID_SPEAR_OF_HELIOD:
														booster = 1;
														goto end;
													default:
														break;
											}
										}
									};
			);
			end:
			if( upkeep_eater || booster ){
				ai_modifier += (pow*8);
			}
		}
		else{
			target_definition_t td;
			default_target_definition(player, card, &td, TYPE_CREATURE);
			td.allowed_controller = td.preferred_controller = player;
			td.required_state = TARGET_STATE_DESTROYED;
			if( new_pick_target(&td, "TARGET_CREATURE", 0, 1) ){
				int pow = get_power(instance->targets[0].player, instance->targets[0].card);
				ai_modifier += (pow*8);
				SET_BYTE2(sac) = player;
				SET_BYTE3(sac) = instance->targets[0].card;
			}
		}
		if (!sac){
			cancel = 1;
			return 0;
		}
		instance->info_slot = get_power(BYTE2(sac), BYTE3(sac));
		kill_card(BYTE2(sac), BYTE3(sac), KILL_SACRIFICE);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		token_generation_t token;
		default_token_definition(player, card, CARD_ID_INSECT, &token);
		token.qty = instance->info_slot;
		token.pow = 0;
		token.color_forced = get_sleighted_color_test(player, card, COLOR_TEST_BLACK);
		generate_token(&token);
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

static void weaken_nondragon_nonartifact(int player, int card, int t_player, int t_card)
{
	if( ! has_subtype(t_player, t_card, SUBTYPE_DRAGON) && ! is_what(t_player, t_card, TYPE_ARTIFACT) ){
		int minus = round_down_value(get_power(t_player, t_card));
		pump_until_eot(player, card, t_player, t_card, -minus, 0);
	}
}

int card_catacomb_dragon(int player, int card, event_t event){
/* CARD_ID_CATACOMB_DRAGON	2425
Catacomb Dragon	|4|B|B
Creature - Dragon 4/4
Flying
Whenever ~ becomes blocked by a nonartifact, non-Dragon creature, that creature gets -X/-0 until end of turn, where X is half the creature's power, rounded down. */

	if (event == EVENT_DECLARE_BLOCKERS && !is_humiliated(player, card))
	{
		if (player == current_turn && is_attacking(player, card) )
			for_each_creature_blocking_me(player, card, weaken_nondragon_nonartifact, player, card);
	}

	return 0;
}

int card_celestial_dawn(int player, int card, event_t event){//UNUSEDCARD
/*
Celestial Dawn |1|W|W
Enchantment
Lands you control are Plains.
Nonland cards you own that aren't on the battlefield, spells you control, and nonland permanents you control are white.
You may spend white mana as though it were mana of any color. You may spend other mana only as though it were colorless mana.
*/
	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_SPELL ){
		int i;
		for(i=0; i<2; i++){
			int count = 0;
			while( count < active_cards_count[i] ){
					if( in_play(i, count) && !(i==player && count==card) && is_what(i, count, TYPE_PERMANENT) ){
						set_special_flags2(i, count, SF2_CELESTIAL_DAWN);
					}
					count++;
			}
		}
	}

	if( leaves_play(player, card, event) ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_ANY);
		this_test.id = get_id(player, card);
		this_test.not_me = 1;
		if( ! check_battlefield_for_special_card(player, card, 2, 0, &this_test) ){
			remove_special_flags2(2, -1, SF2_CELESTIAL_DAWN);
		}
	}

	if( event == EVENT_CAST_SPELL && ! affect_me(player, card) && is_what(affected_card_controller, affected_card, TYPE_PERMANENT) ){
		set_special_flags2(affected_card_controller, affected_card, SF2_CELESTIAL_DAWN);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		instance->targets[1].card = get_internal_card_id_from_csv_id(CARD_ID_PLAINS);
	}

	if( event == EVENT_CHANGE_TYPE && instance->targets[1].card > -1 ){
		if( affected_card_controller == player && is_what(affected_card_controller, affected_card, TYPE_LAND) ){
			event_result = instance->targets[1].card;
		}
	}

	if( event == EVENT_MODIFY_COST_GLOBAL && affected_card_controller == player ){
		card_ptr_t* c = cards_ptr[ get_id(affected_card_controller, affected_card)  ];
		int amount = 0;
		if( c->req_black < 16 ){
			amount+=c->req_black;
			COST_BLACK-=c->req_black;
		}
		if( c->req_blue < 16 ){
			amount+=c->req_blue;
			COST_BLUE-=c->req_blue;
		}
		if( c->req_green < 16 ){
			amount+=c->req_green;
			COST_GREEN-=c->req_green;
		}
		if( c->req_red < 16 ){
			amount+=c->req_red;
			COST_RED-=c->req_red;
		}
		COST_WHITE+=amount;
	}

	if( event == EVENT_SET_COLOR && affected_card_controller == player ){
		event_result = COLOR_TEST_WHITE;
	}

	return global_enchantment(player, card, event);
}

int card_chaosphere(int player, int card, event_t event){
/* CARD_ID_CHAOSPHERE	2428
Chaosphere	|2|R
World Enchantment
Creatures with flying can block only creatures with flying.
Creatures without flying have reach. (They can block creatures with flying.) */

	if( in_play(player, card) && ! is_humiliated(player, card) ){
		if( event == EVENT_ABILITIES && is_what(affected_card_controller, affected_card, TYPE_CREATURE) ){
			if( !(event_result & KEYWORD_FLYING) ){
				event_result |= KEYWORD_REACH;
			}
		}

		if( event == EVENT_BLOCK_LEGALITY && ! check_for_ability(attacking_card_controller, attacking_card, KEYWORD_FLYING) ){
			if( check_for_ability(affected_card_controller, affected_card, KEYWORD_FLYING) ){
				event_result = 1;
			}
		}
	}

	return enchant_world(player, card, event);
}

int card_charcoal_diamond(int player, int card, event_t event){
/* CARD_ID_CHARCOAL_DIAMOND	2429
Charcoal Diamond	|2
Artifact
~ enters the battlefield tapped.
|T: Add |B to your mana pool. */

	return mirage_diamonds(player, card, event);
}

int card_chariot_of_the_sun(int player, int card, event_t event){
/* CARD_ID_CHARIOT_OF_THE_SUN	2430
Chariot of the Sun	|3
Artifact
|2, |T: Until end of turn, target creature you control gains flying and has base toughness 1. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.allowed_controller = player;
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			set_pt_and_abilities_until_eot(instance->parent_controller, instance->parent_card,
											instance->targets[0].player,  instance->targets[0].card, -1, 1, KEYWORD_FLYING, 0, 0);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET | GAA_LITERAL_PROMPT, MANACOST_X(2), 0,
									&td, "Select target creature you control.");
}

int card_choking_sands(int player, int card, event_t event)
{
  /* Choking Sands	|1|B|B
   * Sorcery
   * Destroy target non-|H2Swamp land. If that land was nonbasic, ~ deals 2 damage to the land's controller. */

  if (!IS_GS_EVENT(player, card, event))
	return 0;

  target_definition_t td;
  default_target_definition(player, card, &td, TYPE_LAND);
  td.required_subtype = get_hacked_subtype(player, card, SUBTYPE_SWAMP);
  td.special = TARGET_SPECIAL_ILLEGAL_SUBTYPE;

  if (event == EVENT_RESOLVE_SPELL)
	{
	  if (valid_target(&td))
		{
		  card_instance_t* instance = get_card_instance(player, card);

		  int was_nonbasic = !is_basic_land(instance->targets[0].player, instance->targets[0].card);

		  kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);

		  if (was_nonbasic)
			damage_player(instance->targets[0].player, 2, player, card);
		}

	  kill_card(player, card, KILL_DESTROY);
	}

  return generic_spell(player, card, event, GS_CAN_TARGET|GS_LITERAL_PROMPT, &td,
					   get_hacked_land_text(player, card, "Select target non-%s land.", SUBTYPE_SWAMP), 1, NULL);
}

int card_circle_of_despair(int player, int card, event_t event){
	/* Circle of Despair	|1|W|B	0x200303b
	 * Enchantment
	 * |1, Sacrifice a creature: The next time a source of your choice would deal damage to any target this turn, prevent that damage. */
	return prevent_damage_sacrificing_a_creature(player, card, event, 1);
}

int card_civic_guildmage(int player, int card, event_t event){
/* CARD_ID_CIVIC_GUILDMAGE	2434
Civic Guildmage	|W
Creature - Human Wizard 1/1
|G, |T: Target creature gets +0/+1 until end of turn.
|U, |T: Put target creature you control on top of its owner's library. */

	return mirage_guildmage(player, card, event, COLOR_BLUE, COLOR_GREEN);
}

int card_crimson_hellkite(int player, int card, event_t event){
/* CARD_ID_CRIMSON_HELLKITE	2439
Crimson Hellkite	|6|R|R|R
Creature - Dragon 6/6
Flying
|X, |T: ~ deals X damage to target creature. Spend only |Sred mana on X. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION && valid_target(&td) ){
		damage_target0(player, card, instance->info_slot);
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_R(-1), 0, &td, "TARGET_CREATURE");
}

int card_crystal_vein(int player, int card, event_t event)
{
/* CARD_ID_CRYSTAL_VEIN	2443
Crystal Vein	""
Land
|T: Add |C to your mana pool.
|T, Sacrifice ~: Add |C|C to your mana pool. */

  return sac_land(player, card, event, COLOR_COLORLESS, COLOR_COLORLESS, COLOR_COLORLESS);
}

int card_crystal_golem(int player, int card, event_t event){
/* CARD_ID_CRYSTAL_GOLEM	2442
Crystal Golem	|4
Artifact Creature - Golem 3/3
At the beginning of your end step, ~ phases out.
(While it's phased out, it's treated as though it doesn't exist. It phases in before you untap during your next untap step.) */

	if( current_turn == player && eot_trigger(player, card, event) ){
		phase_out(player, card);
	}

	return 0;
}

int card_cursed_totem(int player, int card, event_t event){
/*
Cursed Totem |2
Artifact
Activated abilities of creatures can't be activated.
*/
	if( in_play(player, card) && ! is_humiliated(player, card) ){
		int disabling_mask = PB_ACTIVATE_ABILITIES_OF_CREATURES_DISABLED | PB_NONMANA_ACTIVATE_ABILITIES_OF_CREATURES_DISABLED;
		player_bits[player] |= disabling_mask;
		player_bits[1-player] |= disabling_mask;
	}

	return 0;
}

int card_daring_apprentice(int player, int card, event_t event){
	/*
	  Daring Apprentice |1|U|U
	  Creature - Human Wizard 1/1
	  {T}, Sacrifice Daring Apprentice: Counter target spell.
	*/

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance(player, card);
		if( counterspell_validate(player, card, NULL, 0) ){
			real_counter_a_spell(player, card, instance->targets[0].player, instance->targets[0].card);
		}
		kill_card(player, card, KILL_SACRIFICE);
	}

	return generic_activated_ability(player, card, event, GAA_SPELL_ON_STACK | GAA_UNTAPPED | GAA_SACRIFICE_ME, MANACOST0, 0, NULL, NULL);
}

int card_discordant_spirit(int player, int card, event_t event){
/*
Discordant Spirit |2|B|R
Creature — Spirit 2/2
At the beginning of each end step, if it's an opponent's turn, put a +1/+1 counter on Discordant Spirit for each 1 damage dealt
	to you this turn.
At the beginning of your end step, remove all +1/+1 counters from Discordant Spirit.
*/
	if( current_turn != player ){
		check_damage_test(ANYBODY, -1, event, DDBM_MUST_DAMAGE_TRIGGERING_PLAYER | DDBM_MUST_DAMAGE_PLAYER | DDBM_REPORT_DAMAGE_DEALT,
							player, card, 0, NULL);
		if( resolve_damage_trigger(player, card, event, DDBM_MUST_DAMAGE_TRIGGERING_PLAYER | DDBM_MUST_DAMAGE_PLAYER |
									DDBM_REPORT_DAMAGE_DEALT, player, card) )
		{
			int amount = get_card_instance(player, card)->targets[16].player;
			get_card_instance(player, card)->targets[16].player = 0;
			add_1_1_counters(player, card, amount);
		}
	}

	if( current_turn == player && eot_trigger(player, card, event) ){
		remove_all_counters(player, card, COUNTER_P1_P1);
	}

	return 0;
}

int card_dissipate(int player, int card, event_t event){
/* CARD_ID_DISSIPATE	2452
Dissipate	|1|U|U
Instant
Counter target spell. If that spell is countered this way, exile it instead of putting it into its owner's graveyard. */

	// Used externally
	if( event == EVENT_RESOLVE_SPELL ){
		if( counterspell_validate(player, card, NULL, 0) ){
			card_instance_t *instance = get_card_instance(player, card);
			if( manage_counterspell_linked_hacks(player, card, instance->targets[0].player, instance->targets[0].card) != KILL_EXILE ){
				put_on_top_of_deck(instance->targets[0].player, instance->targets[0].card);
				rfg_top_card_of_deck(instance->targets[0].player);
			}
			else{
				kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_COUNTERSPELL, NULL, NULL, 1, NULL);
}

int card_divine_retribution(int player, int card, event_t event){
/* CARD_ID_DIVINE_RETRIBUTION	2453
Divine Retribution	|1|W
Instant
~ deals damage to target attacking creature equal to the number of attacking creatures. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.required_state = TARGET_STATE_ATTACKING;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			damage_creature(instance->targets[0].player, instance->targets[0].card, count_attackers(current_turn), player, card);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET | GS_LITERAL_PROMPT, &td, "Select target attacking creature.", 1, NULL);
}

int card_dream_cache(int player, int card, event_t event){
/* CARD_ID_DREAM_CACHE	2455
Dream Cache	|2|U
Sorcery
Draw three cards, then put two cards from your hand both on top of your library or both on the bottom of your library. */

	if( event == EVENT_RESOLVE_SPELL ){
		draw_cards(player, 3);
		int max = MIN(2, hand_count[player]);
		if( max ){
			int choice = do_dialog(player, player, card, -1, -1, " Put 2 cards on top\n Put 2 cards on bottom", 1);
			test_definition_t this_test;
			if( choice == 0 ){
				new_default_test_definition(&this_test, TYPE_ANY, "Select a card to put on top.");
			}
			else{
				new_default_test_definition(&this_test, TYPE_ANY, "Select a card to put on bottom.");
			}
			int mode = choice == 0 ? TUTOR_DECK : TUTOR_BOTTOM_OF_DECK;
			while( max ){
					new_global_tutor(player, player, TUTOR_FROM_HAND, mode, 1, AI_MIN_VALUE, &this_test);
					max--;
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

static void phase_out_me_and_blocker(int player, int card, int t_player, int t_card)
{
	if( ! check_state(player, card, STATE_OUBLIETTED) ){
		phase_out(player, card);
	}
	if( ! check_state(t_player, t_card, STATE_OUBLIETTED) ){
		phase_out(t_player, t_card);
	}
}

int card_dream_fighter(int player, int card, event_t event){
/*
Dream Fighter |2|U
Creature — Human Soldier 1/1
Whenever Dream Fighter blocks or becomes blocked by a creature, Dream Fighter and that creature phase out.
	(While they're phased out, they're treated as though they don't exist. Each one phases in before its controller untaps during his or
	her next untap step.)
*/

	if (event == EVENT_CHANGE_TYPE && affect_me(player, card) && !is_humiliated(player, card))
		get_card_instance(player, card)->destroys_if_blocked |= DIFB_DESTROYS_ALL;

	if (event == EVENT_DECLARE_BLOCKERS && !is_humiliated(player, card))
	{
		if (player == current_turn && (get_card_instance(player, card)->state & STATE_ATTACKING))
			for_each_creature_blocking_me(player, card, phase_out_me_and_blocker, player, card);

		if (player == 1-current_turn && (get_card_instance(player, card)->state & STATE_BLOCKING))
			for_each_creature_blocked_by_me(player, card, phase_out_me_and_blocker, player, card);
	}

	return 0;
}

static void ai_helper_of_land_destroyer(int player, int card, int target_pos){
	if( count_subtype(player, TYPE_LAND, -1) <= 3 ){
		ai_modifier += 24;
	}
	card_instance_t *instance = get_card_instance(player, card);
	int csvid = get_id(instance->targets[target_pos].player, instance->targets[target_pos].card);
	switch( csvid ){
			case CARD_ID_MAZE_OF_ITH:
			case CARD_ID_LIBRARY_OF_ALEXANDRIA:
				ai_modifier += 128;
				break;
			default:
				break;
	}
}

int card_dwarven_miner(int player, int card, event_t event){
	/*
	  Dwarven Miner |1|R
	  Creature — Dwarf 1/2
	  {2}{R}, {T}: Destroy target nonbasic land.
	*/
	// Used externally
	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_LAND);
	td.required_subtype = SUBTYPE_BASIC;
	td.special = TARGET_SPECIAL_ILLEGAL_SUBTYPE;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_ACTIVATE ){
		generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET | GAA_LITERAL_PROMPT, MANACOST_XR(2, 1), 0,
									&td, "Select target nonbasic land.");
		if( spell_fizzled != 1 ){
			ai_helper_of_land_destroyer(player, card, 0);
		}
		return 0;
	}

	if( event == EVENT_RESOLVE_ACTIVATION && valid_target(&td) ){
		kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET | GAA_LITERAL_PROMPT, MANACOST_XR(2, 1), 0,
									&td, "Select target nonbasic land.");
}

int card_early_harvest(int player, int card, event_t event){
/* CARD_ID_EARLY_HARVEST	2459
Early Harvest	|1|G|G
Instant
Target player untaps all basic lands he or she controls. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.zone = TARGET_ZONE_PLAYERS;
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PLAYER", 1, NULL);
		if( spell_fizzled != 1 ){
			int p = instance->targets[0].player;
			if( p == player ){
				test_definition_t test;
				default_test_definition(&test, TYPE_LAND);
				test.subtype = SUBTYPE_LAND;
				test.state = STATE_TAPPED;
				ai_modifier += (check_battlefield_for_special_card(player, card, p, CBFSC_GET_COUNT, &test)*8);
			}
		}
		return 0;
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			test_definition_t this_test;
			default_test_definition(&this_test, TYPE_LAND);
			this_test.subtype = SUBTYPE_BASIC;
			new_manipulate_all(player, card, instance->targets[0].player, &this_test, ACT_UNTAP);
		}
		kill_card(player, card, KILL_SACRIFICE);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PLAYER", 1, NULL);
}

int card_elixir_of_vitality(int player, int card, event_t event){
/* CARD_ID_ELIXIR_OF_VITALITY	2462
Elixir of Vitality	|4
Artifact
~ enters the battlefield tapped.
|T, Sacrifice ~: You gain 4 life.
|8, |T, Sacrifice ~: You gain 8 life. */

	comes_into_play_tapped(player, card, event);

	if( event == EVENT_CAN_ACTIVATE ){
		return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_SACRIFICE_ME, MANACOST0, 0, NULL, NULL);
	}

	if(event == EVENT_ACTIVATE ){
		card_instance_t *instance = get_card_instance(player, card);
		int choice = 0;
		if( has_mana_for_activated_ability(player, card, MANACOST_X(8)) ){
			choice = do_dialog(player, player, card, -1, -1, " Gain 4 life\n Gain 8 life\n Cancel", 1);
		}
		if( choice == 2 ){
			spell_fizzled = 1;
			return 0;
		}
		charge_mana_for_activated_ability(player, card, MANACOST_X(8*choice));
		if( spell_fizzled != 1 ){
			tap_card(player, card);
			instance->info_slot = choice+1;
			kill_card(player, card, KILL_SACRIFICE);
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance(player, card);
		gain_life(player, 4+((instance->info_slot-1)*4));
	}

	return 0;
}

int card_emberwilde_djinn(int player, int card, event_t event){
/* CARD_ID_EMBERWILDE_DJINN	2464
Emberwilde Djinn	|2|R|R
Creature - Djinn 5/4
Flying
At the beginning of each player's upkeep, that player may pay |R|R or 2 life. If he or she does, the player gains control of ~. */

	if( current_turn != player && upkeep_trigger(player, card, event) ){
		int can_choose_life = can_pay_life(1-player, 2);
		int can_choose_mana = has_mana(1-player, COLOR_RED, 2);
		int choice;
		if (can_choose_life){
			if (can_choose_mana){
				choice = do_dialog(1-player, player, card, -1, -1, " Pay 2 life and gain control\n Pay RR and gain control\n Pass", 1);
			} else {
				choice = do_dialog(1-player, player, card, -1, -1, " Pay 2 life and gain control\n _Pay RR and gain control\n Pass", life[1-player] >= 3 ? 0 : 2);
			}
		} else if (can_choose_mana){
			choice = do_dialog(1-player, player, card, -1, -1, " _Pay 2 life and gain control\n Pay RR and gain control\n Pass", 1);
		} else {
			choice = 2;
		}

		if( choice != 2 ){
			if (choice == 0){
				lose_life(1-player, 2);
			} else {
				charge_mana(1-player, COLOR_RED, 2);
			}
			if( spell_fizzled != 1 ){
				give_control_of_self(player, card);
			}
		}
	}
	return 0;
}

int card_emberwilde_caliph(int player, int card, event_t event){
/* CARD_ID_EMBERWILDE_CALIPH	2463
Emberwilde Caliph	|2|U|R
Creature - Djinn 4/4
Flying, trample
~ attacks each turn if able.
Whenever ~ deals damage, you lose that much life. */

	attack_if_able(player, card, event);

	check_damage_test(player, card, event, DDBM_REPORT_DAMAGE_DEALT|DDBM_MUST_DAMAGE_PLANESWALKER, player, card, 0, NULL);

	if( resolve_damage_trigger(player, card, event, DDBM_REPORT_DAMAGE_DEALT|DDBM_MUST_DAMAGE_PLANESWALKER, player, card) ){
		int amount = get_card_instance(player, card)->targets[16].player;
		get_card_instance(player, card)->targets[16].player = 0;

		lose_life(player, amount);
	}

	return 0;
}

int card_energy_bolt(int player, int card, event_t event){
	/* Energy Bolt	|X|R|W	0x2003054
	 * Sorcery
	 * Choose one -
	 * * ~ deals X damage to target player or planeswalker.
	 * * Target player gains X life. */
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TARGET_TYPE_PLANESWALKER);
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;
	td.preferred_controller = ANYBODY;

	card_instance_t *instance = get_card_instance(player, card);

	enum{
		CHOICE_DAMAGE = 1,
		CHOICE_GAIN_LIFE,
	};

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_X_SPELL | GS_CAN_TARGET, &td, NULL, 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		generic_spell(player, card, event, GS_X_SPELL, &td, NULL, 1, NULL);
		if( spell_fizzled != 1 ){
			instance->number_of_targets = 0;
			if( ! check_special_flags2(player, card, SF2_COPIED_FROM_STACK) ){
				instance->targets[1].player = 0;
				int choice = DIALOG(player, card, event, DLG_RANDOM, DLG_NO_STORAGE,
									"Damage target", 1, 5,
									"Target gain life", 1, life[player] < 6 ? 10 : 1);
				if( ! choice ){
					spell_fizzled = 1;
					return 0;
				}
				instance->targets[1].player = choice;
			}
			if( pick_target(&td, instance->targets[1].player == CHOICE_DAMAGE ? "TARGET_PLAYER_OR_PLANESWALKER" : "TARGET_PLAYER") ){
				if( instance->targets[1].player == CHOICE_DAMAGE ){
					ai_modifier += (instance->targets[0].player != player ? 48 : -48);
				}
				if( instance->targets[1].player == CHOICE_GAIN_LIFE ){
					ai_modifier += (instance->targets[0].player == player ? 48 : -48);
				}
			}
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			if( instance->targets[1].player == CHOICE_DAMAGE ){
				damage_target0(player, card, instance->info_slot);
			}
			if( instance->targets[1].player == CHOICE_GAIN_LIFE ){
				gain_life(instance->targets[0].player, instance->info_slot);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_enlightened_tutor(int player, int card, event_t event){
/* CARD_ID_ENLIGHTENED_TUTOR	1525
Enlightened Tutor	|W
Instant
Search your library for an artifact or enchantment card and reveal that card. Shuffle your library, then put the card on top of it. */

	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_ARTIFACT | TYPE_ENCHANTMENT, "Select an artifact or enchantment card.");
		new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_DECK, 0, AI_MAX_VALUE, &test);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_femeref_archers(int player, int card, event_t event){
/* CARD_ID_FEMEREF_ARCHERS	2473
Femeref Archers	|2|G
Creature - Human Archer 2/2
|T: ~ deals 4 damage to target attacking creature with flying. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.required_abilities = KEYWORD_FLYING;
	td.required_state = TARGET_STATE_ATTACKING;

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			damage_target0(player, card, 4);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET | GAA_LITERAL_PROMPT, MANACOST0, 0,
									&td, "Select target attacking creature with flying.");
}

int card_femeref_knight(int player, int card, event_t event){
/* CARD_ID_FEMEREF_KNIGHT	2475
Femeref Knight	|2|W
Creature - Human Knight 2/2
Flanking (Whenever a creature without flanking blocks this creature, the blocking creature gets -1/-1 until end of turn.)
|W: ~ gains vigilance until end of turn. */

	flanking(player, card, event);
	return generic_shade(player, card, event, 0, MANACOST_W(1), 0, 0, 0, SP_KEYWORD_VIGILANCE);
}

int card_fire_diamond(int player, int card, event_t event){
/* CARD_ID_FIRE_DIAMOND	2480
Fire Diamond	|2
Artifact
~ enters the battlefield tapped.
|T: Add |R to your mana pool. */

	return mirage_diamonds(player, card, event);
}

int card_flash(int player, int card, event_t event){
/*
Flash |1|U
Instant
You may put a creature card from your hand onto the battlefield. If you do, sacrifice it unless you pay its mana cost reduced by up to {2}.
*/
	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t this_test;
		new_default_test_definition(&this_test, TYPE_CREATURE, "Select a creature card to put into play.");
		int result = new_global_tutor(player, player, TUTOR_FROM_HAND, TUTOR_PLAY, 0, AI_MAX_CMC, &this_test);
		if( result != -1 ){
			card_ptr_t* c = cards_ptr[ get_id( player, result ) ];
			int colorless = c->req_colorless-2;
			if(colorless < 0 ){ colorless = 0; }
			charge_mana_multi(player, colorless, c->req_black, c->req_blue, c->req_green, c->req_red, c->req_white );
			if( spell_fizzled == 1 ){
				kill_card(player, result, KILL_SACRIFICE);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_flood_plain(int player, int card, event_t event){
	/* Flood Plain	""
	 * Land
	 * ~ enters the battlefield tapped.
	 * |T, Sacrifice ~: Search your library for |Ha Plains or |H2Island card and put it onto the battlefield. Then shuffle your library. */
	return old_fetch(player, card, event, SUBTYPE_PLAINS, SUBTYPE_ISLAND);
}

int card_floodgate(int player, int card, event_t event){
/* CARD_ID_FLOODGATE	2484
Floodgate	|3|U
Creature - Wall 0/5
Defender (This creature can't attack.)
When ~ has flying, sacrifice it.
When ~ leaves the battlefield, it deals damage equal to half the number of |H1Islands you control, rounded down, to each non|Sblue creature without flying.*/

	if( event == EVENT_ABILITIES && affect_me(player, card) && in_play(player, card) && ! is_humiliated(player, card) ){
		if( event_result & KEYWORD_FLYING ){
			kill_card(player, card, KILL_SACRIFICE);
		}
	}

	if( leaves_play(player, card, event) ){
		int amount = count_subtype(player, TYPE_LAND, get_hacked_subtype(player, card, SUBTYPE_ISLAND))/2;
		if( amount ){
			test_definition_t test;
			default_test_definition(&test, TYPE_CREATURE);
			test.keyword = KEYWORD_FLYING;
			new_damage_all(player, card, ANYBODY, amount, 0, &test);
		}
	}

	return 0;
}

int card_foratog(int player, int card, event_t event){
	/*
	  Foratog |2|G
	  Creature - Atog 1/2
	  {G}, Sacrifice a Forest: Foratog gets +2/+2 until end of turn.
	*/
	if( event == EVENT_POW_BOOST || event == EVENT_TOU_BOOST )
	{
		test_definition_t test;
		default_test_definition(&test, TYPE_PERMANENT);
		test.subtype = get_hacked_subtype(player, card, SUBTYPE_FOREST);
		int num_can_sac = max_can_sacrifice_as_cost(player, card, &test);
		if (num_can_sac <= 0)
			return 0;

		return generic_shade_amt_can_pump(player, card, 2, 0, MANACOST_G(1), num_can_sac);
	}

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	test_definition_t test;
	default_test_definition(&test, TYPE_PERMANENT);
	test.subtype = get_hacked_subtype(player, card, SUBTYPE_FOREST);

	if( event == EVENT_CAN_ACTIVATE ){
		if( generic_activated_ability(player, card, event, 0, MANACOST_G(1), 0, NULL, NULL) ){
			return new_can_sacrifice_as_cost(player, card, &test);
		}
	}

	if( event == EVENT_ACTIVATE ){
		if( charge_mana_for_activated_ability(player, card, MANACOST_G(1)) ){
			scnprintf(test.message, 100, "%s", get_hacked_land_text(player, card, "Select a %s to sacrifice.", SUBTYPE_FOREST));
			if( ! new_sacrifice(player, card, player, SAC_AS_COST, &test) ){
				spell_fizzled = 1;
			}
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		return generic_shade(player, card, event, 0, MANACOST_G(1), 2, 2, 0, 0);
	}

	return 0;
}

int card_forbidden_crypt(int player, int card, event_t event)
{
  /* Forbidden Crypt	|3|B|B
   * Enchantment
   * If you would draw a card, return a card from your graveyard to your hand instead. If you can't, you lose the game.
   * If a card would be put into your graveyard from anywhere, exile that card instead. */

	if (trigger_condition == TRIGGER_REPLACE_CARD_DRAW && affect_me(player, card) && reason_for_trigger_controller == player &&
		!suppress_draw && ! is_humiliated(player, card) )
	{
		if (event == EVENT_TRIGGER)
			event_result |= RESOLVE_TRIGGER_MANDATORY;

		if (event == EVENT_RESOLVE_TRIGGER)
		{
			if (get_grave(player)[0] == -1)
				lose_the_game(player);
			else
			{
				test_definition_t this_test;
				new_default_test_definition(&this_test, 0, "Select a card to return to hand.");
				new_global_tutor(player, player, TUTOR_FROM_GRAVE, TUTOR_HAND, 1, AI_MAX_VALUE, &this_test);
				suppress_draw = 1;
			}
		}
	}

	if_a_card_would_be_put_into_graveyard_from_anywhere_exile_it_instead(player, card, event, player, NULL);

	if (event == EVENT_SHOULD_AI_PLAY)
	{
		ai_modifier += (player == AI ? 12 : -12) * count_graveyard(player);
		if (player == AI)
			ai_modifier -= count_deck(player) * 48;
	}

	return global_enchantment(player, card, event);
}

int card_forsaken_wastes(int player, int card, event_t event)
{
  /* Forsaken Wastes	|2|B
   * World Enchantment
   * Players can't gain life.
   * At the beginning of each player's upkeep, that player loses 1 life.
   * Whenever ~ becomes the target of a spell, that spell's controller loses 5 life. */

	enchant_world(player, card, event);

	const target_t* targeter = becomes_target_of_spell(player, card, event, player, card, ANYBODY, RESOLVE_TRIGGER_MANDATORY);
	if (targeter)
		lose_life(targeter->player, 5);

	upkeep_trigger_ability(player, card, event, 2);

	if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
		lose_life(current_turn, 1);

	if( in_play(player, card) && event == EVENT_STATIC_EFFECTS && affect_me(player, card) && ! is_humiliated(player, card) )
	{
		player_bits[0] |= PB_CANT_GAIN_LIFE;
		player_bits[1] |= PB_CANT_GAIN_LIFE;
	}

	return global_enchantment(player, card, event);
}

int card_frenetic_efreet(int player, int card, event_t event){
/* CARD_ID_FRENETIC_EFREET	2487
Frenetic Efreet	|1|U|R
Creature - Efreet 2/1
Flying
|0: Flip a coin. If you win the flip, ~ phases out. If you lose the flip, sacrifice ~.
(While it's phased out, it's treated as though it doesn't exist. It phases in before you untap during your next untap step.)*/

	if( event == EVENT_ACTIVATE ){
		ai_modifier += (current_turn != player ? 48 : -48);
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance( player, card );
		int flip = flip_a_coin(instance->parent_controller, instance->parent_card);
		if( flip == 1 ){
			phase_out(instance->parent_controller, instance->parent_card);
		}
		else{
			kill_card(instance->parent_controller, instance->parent_card, KILL_SACRIFICE);
		}
	}

	return generic_activated_ability(player, card, event, 0, MANACOST0, 0, NULL, NULL);
}

int card_goblin_elite_infantry(int player, int card, event_t event){
/* CARD_ID_GOBLIN_ELITE_INFANTRY	2490
Goblin Elite Infantry	|1|R
Creature - Goblin Warrior 2/2
Whenever ~ blocks or becomes blocked, it gets -1/-1 until end of turn. */

	if( event == EVENT_DECLARE_BLOCKERS && ! is_humiliated(player, card) ){
		if( current_turn == player ){
			if( is_attacking(player, card) && ! is_unblocked(player, card) ){
				pump_until_eot(player, card, player, card, -1, -1);
			}
		}
		else{
			card_instance_t *instance = get_card_instance(player, card);
			if( instance->blocking < 255 ){
				pump_until_eot(player, card, player, card, -1, -1);
			}
		}
	}

	return 0;
}

// goblin scout --> vanilla

int card_goblin_scouts(int player, int card, event_t event){
	/* Goblin Scouts	|3|R|R
	 * Sorcery
	 * Put three 1/1 |Sred Goblin Scout creature tokens with |H2mountainwalk onto the battlefield. */

	if(event == EVENT_RESOLVE_SPELL ){
		token_generation_t token;
		default_token_definition(player, card, CARD_ID_GOBLIN_SCOUT, &token);
		token.pow = token.tou = 1;
		token.qty = 3;
		token.color_forced = get_sleighted_color_test(player, card, COLOR_TEST_RED);
		generate_token(&token);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_goblin_soothsayer(int player, int card, event_t event){
/* CARD_ID_GOBLIN_SOOTHSAYER	2492
Goblin Soothsayer	|R
Creature - Goblin Shaman 1/1
|R, |T, Sacrifice a Goblin: |SRed creatures get +1/+1 until end of turn. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	test_definition_t test;
	new_default_test_definition(&test, TYPE_PERMANENT, "Select a Goblin to sacrifice.");
	test.subtype = SUBTYPE_GOBLIN;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_ACTIVATE){
		if( current_turn == player ){
			test_definition_t test2;
			default_test_definition(&test2, TYPE_CREATURE);
			test2.color = COLOR_TEST_RED;
			test2.state = STATE_ATTACKING;
			ai_modifier += (check_battlefield_for_special_card(player, card, player, CBFSC_GET_COUNT, &test2)*8);
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		int z;
		for(z=0; z < 2; z++){
		       int count = 0;
		       while(count < active_cards_count[z]){
			          if( in_play(z, count) && is_what(z, count, TYPE_CREATURE) &&
				         (get_color(z, count) & get_sleighted_color_test(player, card, COLOR_TEST_RED)) )
			          {
				          pump_until_eot(player, instance->parent_card, z, count, 1, 1);
			          }
			          count++;
		       }
		}			   
	}

	return generic_activated_ability_with_sacrifice_as_cost(player, card, event, GAA_UNTAPPED, MANACOST_R(1), 0, NULL, NULL, 0, &test, 0);
}

int card_goblin_tinkerer(int player, int card, event_t event){
/* CARD_ID_GOBLIN_TINKERER	2493
Goblin Tinkerer	|1|R
Creature - Goblin Artificer 1/2
|R, |T: Destroy target artifact. That artifact deals damage equal to its converted mana cost to ~. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_ARTIFACT);

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			int cmc = get_cmc(instance->targets[0].player, instance->targets[0].card);
			damage_creature(instance->parent_controller, instance->parent_card, cmc,
							instance->targets[0].player, instance->targets[0].card);
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_R(1), 0, &td, "TARGET_ARTIFACT");
}

int card_granger_guildmage(int player, int card, event_t event){
	/* Granger Guildmage	|G	0x2003149
	 * Creature - Human Wizard 1/1
	 * |R, |T: ~ deals 1 damage to any target and 1 damage to you.
	 * |W, |T: Target creature gains first strike until end of turn. */
	return mirage_guildmage(player, card, event, COLOR_RED, COLOR_WHITE);
}

int card_grasslands(int player, int card, event_t event){
	/* Grasslands	""
	 * Land
	 * ~ enters the battlefield tapped.
	 * |T, Sacrifice ~: Search your library for |Ha Forest or |H2Plains card and put it onto the battlefield. Then shuffle your library. */
	return old_fetch(player, card, event, SUBTYPE_FOREST, SUBTYPE_PLAINS);
}

static int grim_feast_effect(int player, int card, event_t event){

	//In case something is copying Pawn of Ulamog and then reverts to its original form.
	if( event == EVENT_STATIC_EFFECTS && affect_me(player, card) ){
		card_instance_t *instance = get_card_instance(player, card);
		if( instance->damage_target_player > -1 ){
			if( get_id(instance->damage_target_player, instance->damage_target_card) != CARD_ID_GRIM_FEAST ){
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
				if( get_owner(affected_card_controller, affected_card) == 1-player ){
					count_for_gfp_ability_and_store_values(player, card, event, ANYBODY, TYPE_CREATURE, NULL, GFPC_STORE_TOUGHNESS, 1);
				}
			}
		}
	}

	if( ! check_status(player, card, STATUS_INVISIBLE_FX) ){
		int result = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
		if( result ){
			card_instance_t *instance = get_card_instance(player, card);
			int amount = instance->targets[1].player;
			instance->targets[1].player = 0;
			gain_life(player, amount);
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


int card_grim_feast(int player, int card, event_t event){
/*
Grim Feast |1|B|G
Enchantment
At the beginning of your upkeep, Grim Feast deals 1 damage to you.
Whenever a creature is put into an opponent's graveyard from the battlefield, you gain life equal to its toughness.
*/
	if( event == EVENT_STATIC_EFFECTS && affect_me(player, card) ){
		int found = 0;
		int c;
		for(c=0; c<active_cards_count[player]; c++){
			if( in_play(player, c) && is_what(player, c, TYPE_EFFECT) ){
				card_instance_t *inst = get_card_instance(player, c);
				if( inst->info_slot == (int)grim_feast_effect ){
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
			int legacy = create_targetted_legacy_effect(player, card, &grim_feast_effect, player, card);
			card_instance_t *inst = get_card_instance(player, legacy);
			inst->targets[0].player = player;
			inst->targets[0].card = card;
			inst->number_of_targets = 1;
			add_status(player, legacy, STATUS_INVISIBLE_FX);
		}
	}

	if( event == EVENT_GRAVEYARD_FROM_PLAY ){
		if( get_owner(affected_card_controller, affected_card) == 1-player ){
			count_for_gfp_ability_and_store_values(player, card, event, ANYBODY, TYPE_CREATURE, NULL, GFPC_STORE_TOUGHNESS, 0);
		}
	}

	if( get_card_instance(player, card)->kill_code <= 0 ){
		int result = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
		if( result ){
			card_instance_t *instance = get_card_instance(player, card);
			int amount = instance->targets[0].player;
			instance->targets[0].player = 0;
			gain_life(player, amount);
		}
	}

	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		damage_player(player, 1, player, card);
	}

	return global_enchantment(player, card, event);
}

int card_hakim_loreweaver(int player, int card, event_t event){
/* Hakim, Loreweaver	|3|U|U
 * Legendary Creature - Human Wizard 2/4
 * Flying
 * |U|U: Return target Aura card from your graveyard to the battlefield attached to ~. Activate this ability only during your upkeep and
 * 		only if ~ isn't enchanted.
 * |U|U, |T: Destroy all Auras attached to ~. */

	check_legend_rule(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	test_definition_t test;
	new_default_test_definition(&test, TYPE_ENCHANTMENT, "Select target Aura card.");
	test.subtype = SUBTYPE_AURA_CREATURE;

	enum{
		CHOICE_REANIMATE_AURA = 1,
		CHOICE_KILL_AURAS
	};

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		if( generic_activated_ability_with_target_in_grave(player, card, event, 0, MANACOST_U(2), 0, NULL, NULL,
			0, &test, GS_GRAVE_RECYCLER) )
		{
			return 1;
		}
		return generic_activated_ability(player, card, event, GAA_UNTAPPED, MANACOST_U(2), 0, NULL, NULL);
	}

	if( event == EVENT_ACTIVATE ){
		int abils[3] = {
						0,
						generic_activated_ability_with_target_in_grave(player, card, EVENT_CAN_ACTIVATE, 0, MANACOST_U(2), 0, NULL, NULL,
																		0, &test, GS_GRAVE_RECYCLER),
						generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED, MANACOST_U(2), 0, NULL, NULL)
		};
		int priorities[3] = {0};
		CYCLE_ALL_CARDS(p, c, 	{
									if( in_play(p, c) && is_what(p, c, TYPE_ENCHANTMENT) && has_subtype(p, c, SUBTYPE_AURA) ){
										int cmc = get_cmc(p, c);
										priorities[CHOICE_KILL_AURAS] += (p == player ? -(cmc*8) : (cmc*8));
									}
								};
		);
		int i = 0;
		int max_cmc = -1;
		while( get_grave(player)[i] > -1 ){
				int iid = get_grave(player)[i];
				if( is_what(-1, iid, TYPE_ENCHANTMENT) && has_subtype_by_id(cards_data[iid].id, SUBTYPE_AURA_CREATURE) ){
					int cmc = get_cmc_by_internal_id(iid);
					if( cmc > max_cmc ){
						max_cmc = cmc;
						priorities[CHOICE_REANIMATE_AURA] = cmc *8;
					}
				}
				i++;
		}
		int choice = DIALOG(player, card, event, DLG_RANDOM,
							"Reanimate aura", 					abils[1], priorities[1],
							"Destroy aura attached to Hakim", 	abils[2], priorities[2]);
		if( ! choice ){
			spell_fizzled = 1;
			return 0;
		}
		if( choice == CHOICE_REANIMATE_AURA ){
			return generic_activated_ability_with_target_in_grave(player, card, EVENT_CAN_ACTIVATE, 0, MANACOST_U(2), 0, NULL, NULL,
																	0, &test, GS_GRAVE_RECYCLER);
		}
		if( choice == CHOICE_KILL_AURAS ){
			return generic_activated_ability(player, card, event, GAA_UNTAPPED, MANACOST_U(2), 0, NULL, NULL);
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( instance->info_slot == CHOICE_REANIMATE_AURA ){
			int selected = validate_target_from_grave_source(player, card, player, 0);
			if( selected != -1 ){
				reanimate_permanent(instance->parent_controller, instance->parent_card, instance->parent_controller, selected,
									REANIMATE_ATTACH_AS_AURA);
			}
		}
		if( instance->info_slot == CHOICE_KILL_AURAS ){
			int i;
			for(i=0; i<2; i++){
				int count = active_cards_count[i]-1;
				while(count > -1 ){
						if( in_play(i, count) && is_what(i, count, TYPE_ENCHANTMENT) && has_subtype(i, count, SUBTYPE_AURA) ){
							card_instance_t *aura = get_card_instance(i, count);
							if( aura->damage_target_player == instance->parent_controller &&
								aura->damage_target_card == instance->parent_card )
							{
								kill_card(i, count, KILL_DESTROY);
							}
						}
						count--;
				}
			}
		}
	}

	return 0;
}

int card_hammer_of_bogardan(int player, int card, event_t event){
	/* Hammer of Bogardan	|1|R|R	0x2001db2
	 * Sorcery
	 * ~ deals 3 damage to any target.
	 * |2|R|R|R: Return ~ from your graveyard to your hand. Activate this ability only during your upkeep. */
	if( event == EVENT_GRAVEYARD_ABILITY_UPKEEP ){
		if( has_mana_multi(player, MANACOST_XR(2, 3)) ){
			int choice = do_dialog(player, player, card, -1, -1," Return Hammer to hand\n Leave Hammer be\n", 0);
			if( choice == 0 ){
				charge_mana_multi(player, MANACOST_XR(2, 3));
				if( spell_fizzled != 1 ){
					card_instance_t* instance = get_card_instance(player, card);
					instance->state &= ~STATE_INVISIBLE;
					hand_count[player]++;
					return -1;
				}
			}
		}
		return -2;
	}

	return card_lightning_bolt(player, card, event);
}

int card_harbinger_of_night(int player, int card, event_t event){

	/* Harbinger of Night	|2|B|B
	 * Creature - Spirit 2/3
	 * At the beginning of your upkeep, put a -1/-1 counter on each creature. */

	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		manipulate_type(player, card, ANYBODY, TYPE_CREATURE, ACT_ADD_COUNTERS(COUNTER_M1_M1, 1));
	}

	return 0;
}

int card_harmattan_efreet(int player, int card, event_t event){
/* CARD_ID_HARMATTAN_EFREET	2504
Harmattan Efreet	|2|U|U
Creature - Efreet 2/2
Flying
|1|U|U: Target creature gains flying until end of turn. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}
	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;
	return vanilla_creature_pumper(player, card, event, MANACOST_XU(1, 2), 0, 0, 0, KEYWORD_FLYING, 0, &td);
}

int card_hivis_of_the_scale(int player, int card, event_t event){
/* CARD_ID_HIVIS_OF_THE_SCALE	2507
Hivis of the Scale	|3|R|R
Legendary Creature - Viashino Shaman 3/4
You may choose not to untap ~ during your untap step.
|T: Gain control of target Dragon for as long as you control ~ and ~ remains tapped. */

	check_legend_rule(player, card, event);

	choose_to_untap(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_PERMANENT);
	td1.required_subtype = SUBTYPE_DRAGON;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td1) ){
			gain_control(instance->parent_controller, instance->parent_card, instance->targets[0].player, instance->targets[0].card);
			card_instance_t *parent = get_card_instance(instance->parent_controller, instance->parent_card);
			parent->targets[1] = instance->targets[0];
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET | GAA_LITERAL_PROMPT, MANACOST0, 0,
									&td1, "Select target Dragon permanent.");
}

int card_illumination(int player, int card, event_t event){
/* CARD_ID_ILLUMINATION	2511
Illumination	|W|W
Instant
Counter target artifact or enchantment spell. Its controller gains life equal to its converted mana cost. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	counterspell_target_definition(player, card, &td, TYPE_ARTIFACT | TYPE_ENCHANTMENT);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		if( counterspell_validate(player, card, &td, 0) ){
			int amount = get_cmc(instance->targets[0].player, instance->targets[0].card);
			gain_life(instance->targets[0].player, amount);
			real_counter_a_spell(player, card, instance->targets[0].player, instance->targets[0].card);
		}
		kill_card(player, card, KILL_DESTROY);
	}

   return generic_spell(player, card, event, GS_COUNTERSPELL, &td, NULL, 1, NULL);
}

int card_infernal_contract(int player, int card, event_t event){
/* CARD_ID_INFERNAL_CONTRACT	1501
Infernal Contract	|B|B|B
Sorcery
Draw four cards. You lose half your life, rounded up. */

	if( event == EVENT_CAN_CAST && basic_spell(player, card, event) ){
		return can_pay_life(player, 1);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		int amount = (life[player]+1)/2;
		lose_life(player, amount);
		draw_cards(player, 4);
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_jolraels_centaur(int player, int card, event_t event){
/* CARD_ID_JOLRAELS_CENTAUR	2515
Jolrael's Centaur	|1|G|G
Creature - Centaur Archer 2/2
Shroud (This creature can't be the target of spells or abilities.)
Flanking (Whenever a creature without flanking blocks this creature, the blocking creature gets -1/-1 until end of turn.) */

	flanking(player, card, event);

	return 0;
}

int card_wood2(int player, int card, event_t event){

	if( event == EVENT_CAN_ACTIVATE && can_sacrifice_this_as_cost(player, card)  ){
		return can_produce_mana(player, card);
	}

	if( event == EVENT_ACTIVATE){
		produce_mana(player, COLOR_RED, 1);
		kill_card(player, card, KILL_SACRIFICE);
	}

	if( event == EVENT_COUNT_MANA && can_sacrifice_this_as_cost(player, card) && can_produce_mana(player, card) ){
		declare_mana_available(player, COLOR_RED, 1);
	}

	return 0;
}

int card_jungle_patrol(int player, int card, event_t event){
	/* Jungle Patrol	|3|G
	 * Creature - Human Soldier 3/2
	 * |1|G, |T: Put a 0/1 |Sgreen Wall creature token with defender named Wood onto the battlefield.
	 * Sacrifice a token named Wood: Add |R to your mana pool. */

	if( event == EVENT_COUNT_MANA && affect_me(player, card) ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_PERMANENT, "Select a Wood to sacrifice.");
		test.id = CARD_ID_WOOD;
		if( can_produce_mana(player, card) && new_can_sacrifice_as_cost(player, card, &test) ){
			declare_mana_available(player, COLOR_RED, 1);
		}
	}

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	test_definition_t test;
	new_default_test_definition(&test, TYPE_PERMANENT, "Select a Wood to sacrifice.");
	test.id = CARD_ID_WOOD;

	card_instance_t *instance = get_card_instance(player, card);

	enum{
		CHOICE_TOKEN = 1,
		CHOICE_MANA
	};

	if( event == EVENT_CAN_ACTIVATE ){
		if( can_produce_mana(player, card) && new_can_sacrifice_as_cost(player, card, &test) ){
			return 1;
		}
		return generic_activated_ability(player, card, event, GAA_UNTAPPED, MANACOST_XG(1, 1), 0, NULL, NULL);
	}

	if( event == EVENT_ACTIVATE ){
		int abils[3] = {
						0,
						generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED, MANACOST_XG(1, 1), 0, NULL, NULL),
						can_produce_mana(player, card) && new_can_sacrifice_as_cost(player, card, &test) ? 1 : 0
		};
		int choice = CHOICE_TOKEN;
		if( ! abils[1] || (paying_mana() && abils[2]) ){
			choice = CHOICE_MANA;
		}
		instance->info_slot = choice;
		if( ! IS_AI(player) ){
			choice = DIALOG(player, card, event, DLG_RANDOM,
							"Create Wood", 			abils[1], 1,
							"Get mana from Wood", 	abils[2], 1);
			if( ! choice ){
				spell_fizzled = 1;
				return 0;
			}
		}

		if( choice == CHOICE_TOKEN ){
			return generic_activated_ability(player, card, event, GAA_UNTAPPED, MANACOST_XG(1, 1), 0, NULL, NULL);
		}

		if( choice == CHOICE_MANA ){
			if( new_sacrifice(player, card, player, SAC_AS_COST, &test) ){
				produce_mana(player, COLOR_RED, 1);
			}
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( instance->info_slot == CHOICE_TOKEN ){
			generate_token_by_id(instance->parent_controller, instance->parent_card, CARD_ID_WOOD);
		}
	}


	return 0;
}

int card_jungle_wurm(int player, int card, event_t event){
/* CARD_ID_JUNGLE_WURM	2519
Jungle Wurm	|3|G|G
Creature - Wurm 5/5
Whenever ~ becomes blocked, it gets -1/-1 until end of turn for each creature blocking it beyond the first. */

	if( in_play(player, card) && current_turn == player && ! is_humiliated(player, card) && event == EVENT_DECLARE_BLOCKERS){
		int blockers = count_my_blockers(player, card);
		if (blockers > 1){
			int number = -(blockers - 1);
			pump_until_eot(player, card, player, card, number, number);
		}
	}

	return 0;
}

int card_kukemssa_pirates(int player, int card, event_t event){
/* CARD_ID_KUKEMSSA_PIRATES	2524
Kukemssa Pirates	|3|U
Creature - Human Pirate 2/2
Whenever ~ attacks and isn't blocked, you may gain control of target artifact defending player controls. If you do, ~ assigns no combat damage this turn.*/

	if( event == EVENT_DECLARE_BLOCKERS && is_attacking(player, card) && is_unblocked(player, card) ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_ARTIFACT);
		td.allowed_controller =  1-player;
		td.preferred_controller = 1-player;
		card_instance_t *instance = get_card_instance(player, card);
		if( can_target(&td) && pick_target(&td, "TARGET_ARTIFACT") ){
			gain_control(player, card, instance->targets[0].player, instance->targets[0].card);
			instance->info_slot = 66;
		}
	}

	card_instance_t* damage;
	if( get_card_instance(player, card)->info_slot == 66 &&
		(damage = combat_damage_being_prevented(event)) &&
		damage->damage_source_player == player && damage->damage_source_card == card )
	{
		prevent_damage(affected_card_controller, affected_card, -1);
	}

	if( event == EVENT_CLEANUP ){
		get_card_instance(player, card)->info_slot = 0;
	}

	return 0;
}

int card_lions_eye_diamond(int player, int card, event_t event){
	/* Lion's Eye Diamond	|0
	 * Artifact
	 * Sacrifice ~, Discard your hand: Add three mana of any one color to your mana pool. Activate this ability only any time you could cast an instant. */

	if (event == EVENT_ACTIVATE){
		get_card_instance(player, card)->mana_color = COLOR_TEST_ANY_COLORED;
		artifact_mana_all_one_color(player, card, event, 3, 1);
		if (cancel != 1){
			discard_all(player);
		}
		return 0;
	}
	else {
		return artifact_mana_all_one_color(player, card, event, 3, 1);
	}
}

int card_locust_swarm(int player, int card, event_t event){
/* CARD_ID_LOCUST_SWARM	2530
Locust Swarm	|3|G
Creature - Insect 1/1
Flying
|G: Regenerate ~.
|G: Untap ~. Activate this ability only once each turn. */

	if(event == EVENT_CLEANUP){
		get_card_instance(player, card)->targets[2].player = 0;
	}

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	enum{
		CHOICE_REGENERATE = 1,
		CHOICE_UNTAP
	};

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		if( generic_activated_ability(player, card, event, GAA_REGENERATION, MANACOST_G(1), 0, NULL, NULL) ){
			return 99;
		}
		if( generic_activated_ability(player, card, event, GAA_ONCE_PER_TURN, MANACOST_G(1), 0, NULL, NULL) ){
			return 1;
		}
	}

	if(event == EVENT_ACTIVATE ){
		instance->info_slot = 0;
		int choice = CHOICE_UNTAP;
		int result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_REGENERATION, MANACOST_G(1), 0, NULL, NULL);
		if( result ){
			if( generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_ONCE_PER_TURN, MANACOST_G(1), 0, NULL, NULL) ){
				choice = DIALOG(player, card, event, DLG_RANDOM,
								"Regenerate", result ? 1 : 0, 15,
								"Untap", 1, 5
								);
			}
			else{
				choice = CHOICE_REGENERATE;
			}
		}

		if( generic_activated_ability(player, card, event, choice == CHOICE_REGENERATE ? GAA_REGENERATION : GAA_ONCE_PER_TURN, MANACOST_G(1), 0, NULL, NULL) ){
			instance->info_slot = choice;
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( instance->info_slot == CHOICE_REGENERATE && can_regenerate(instance->parent_controller, instance->parent_card) ){
			regenerate_target(instance->parent_controller, instance->parent_card);
		}
		if( instance->info_slot == CHOICE_UNTAP ){
			untap_card(instance->parent_controller, instance->parent_card);
		}
	}

	return 0;
}

int card_lure_of_prey(int player, int card, event_t event){
/* CARD_ID_LURE_OF_PREY	2531
Lure of Prey	|2|G|G
Instant
Cast ~ only if an opponent cast a creature spell this turn.
You may put a |Sgreen creature card from your hand onto the battlefield. */

	if( event == EVENT_CAN_CAST && get_stormcreature_count(1-player) ){
		return basic_spell(player, card, event);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t this_test;
		new_default_test_definition(&this_test, TYPE_CREATURE,
									get_sleighted_color_text(player, card, "Select a %s creature card.", COLOR_GREEN));
		this_test.color = get_sleighted_color_test(player, card, COLOR_TEST_GREEN);
		this_test.zone = TARGET_ZONE_HAND;
		new_global_tutor(player, player, TUTOR_FROM_HAND, TUTOR_PLAY, 0, AI_MAX_CMC, &this_test);
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_malignant_growth(int player, int card, event_t event){
/* Malignant Growth	|3|G|U
 * Enchantment
 * Cumulative upkeep |1
 * At the beginning of your upkeep, put a growth counter on ~.
 * At the beginning of each opponent's draw step, that player draws an additional card for each growth counter on ~,
 *		then ~ deals damage to the player equal to the number of cards he or she drew this way. */

	cumulative_upkeep(player, card, event, MANACOST_X(1));

	if (event == EVENT_UPKEEP_TRIGGER_ABILITY){	// in addition to cumulative upkeep
		add_counter(player, card, COUNTER_GROWTH);
	}

	if( current_turn != player && event == EVENT_DRAW_PHASE ){
		int growth_counters = count_counters(player, card, COUNTER_GROWTH);
		event_result += growth_counters;
		damage_player(1-player, growth_counters, player, card);
	}

	return global_enchantment(player, card, event);
}

int card_marble_diamond(int player, int card, event_t event){
/* CARD_ID_MARBLE_DIAMOND	2537
Marble Diamond	|2
Artifact
~ enters the battlefield tapped.
|T: Add |W to your mana pool. */

	return mirage_diamonds(player, card, event);
}

int card_maro(int player, int card, event_t event)
{
  /* Maro	|2|G|G
   * Creature - Elemental 100/100
   * ~'s power and toughness are each equal to the number of cards in your hand. */

  if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) && affect_me(player, card) && !is_humiliated(player, card) && player != -1)
	event_result += hand_count[player];

  return 0;
}

int card_mind_bend(int player, int card, event_t event){
/* CARD_ID_MIND_BEND	1086
Mind Bend	|U
Instant
Change the text of target permanent by replacing all instances of one color word with another or one basic land type with another.
(For example, you may change "nonblack creature" to "nongreen creature" or "forestwalk" to "islandwalk." This effect lasts indefinitely.) */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_PERMANENT );

	card_instance_t *instance = get_card_instance( player, card);

	enum{
		CHOICE_SLEIGHT = 1,
		CHOICE_HACK
	};

	if( event == EVENT_CAN_CAST ){
		int result = counterspell(player, card, event, NULL, 0);
		if( result ){
			return result;
		}
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, NULL, 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		instance->info_slot = 0;
		if( counterspell(player, card, EVENT_CAN_CAST, NULL, 0) ){
			counterspell(player, card, event, NULL, 0);
			SET_BYTE0(instance->info_slot) = 1;
		}
		else{
			generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PERMANENT", 1, NULL);
		}
		int ai_mode[3] = {
							0,
							ai_can_sleight_card(instance->targets[0].player, instance->targets[0].card),
							ai_can_hack_card(instance->targets[0].player, instance->targets[0].card)
		};
		if( ! ai_mode[CHOICE_SLEIGHT] && ! ai_mode[CHOICE_HACK] )
		{
			ai_modifier -= 128;
		}
		int choice = DIALOG(player, card, event, DLG_RANDOM, DLG_NO_STORAGE,
							"Change color word", 	1, ai_mode[CHOICE_SLEIGHT] ? 10 : 1,
							"Change land word", 	1, ai_mode[CHOICE_HACK] ? 10 : 1);
		if( ! choice ){
			spell_fizzled = 1;
			return 0;
		}
		SET_BYTE1(instance->info_slot) = choice;
	}

	if( event == EVENT_RESOLVE_SPELL ){
		int t_player = -1;
		int t_card = -1;
		int mode = BYTE0(instance->info_slot);
		if( mode == 1 ){
			if( counterspell_validate(player, card, NULL, 0) ){
				t_player = instance->targets[0].player;
				t_card = instance->targets[0].card;
			}
		}
		else{
			if( valid_target(&td) ){
				t_player = instance->targets[0].player;
				t_card = instance->targets[0].card;
			}
		}
		if( t_player != -1 ){
			int change_mode = BYTE1(instance->info_slot);
			if( change_mode == CHOICE_SLEIGHT ){
				replace_all_instances_of_one_color_word_with_another(t_player, t_card);
			}
			if( change_mode == CHOICE_HACK ){
				replace_all_instances_of_one_basic_land_type_word_with_another(t_player, t_card);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

static void add_fungus_counters(int player, int card, int blocked_controller, int blocked_card)
{
  add_counters(blocked_controller, blocked_card, COUNTER_FUNGUS, 4);
  gains_doesnt_untap_while_has_a_counter_and_remove_a_counter_at_upkeep(player, card, blocked_controller, blocked_card, COUNTER_FUNGUS);
}
int card_mindbender_spores(int player, int card, event_t event)
{
  /* Mindbender Spores	|2|G
   * Creature - Fungus Wall 0/1
   * Defender
   * Flying
   * Whenever ~ blocks a creature, put four fungus counters on that creature. The creature gains "This creature doesn't untap during your untap step if it has a
   * fungus counter on it" and "At the beginning of your upkeep, remove a fungus counter from this creature." */

  if (event == EVENT_DECLARE_BLOCKERS && blocking(player, card, event))
	for_each_creature_blocked_by_me(player, card, add_fungus_counters, player, card);

  return 0;
}

int card_misers_cage(int player, int card, event_t event){
/* CARD_ID_MISERS_CAGE	2545
Misers' Cage	|3
Artifact
At the beginning of each opponent's upkeep, if that player has five or more cards in hand, ~ deals 2 damage to him or her. */

	upkeep_trigger_ability(player, card, event, 1-player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		if( hand_count[1-player] > 4 ){
			damage_player(1-player, 2, player, card);
		}
	}

	return 0;
}

int card_mist_dragon(int player, int card, event_t event){
/* CARD_ID_MIST_DRAGON	2546
Mist Dragon	|4|U|U
Creature - Dragon 4/4
|0: ~ gains flying. (This effect lasts indefinitely.)
|0: ~ loses flying. (This effect lasts indefinitely.)
|3|U|U: ~ phases out. (While it's phased out, it's treated as though it doesn't exist. It phases in before you untap during your next untap step.)*/

	if( get_card_instance(player, card)->targets[1].player == 66 && event == EVENT_ABILITIES && affect_me(player, card) &&
		! is_humiliated(player, card) )
	{
		event_result |= KEYWORD_FLYING;
	}


	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	card_instance_t* instance = get_card_instance(player, card);

	if (event == EVENT_CAN_ACTIVATE){
		return generic_activated_ability(player, card, event, 0, MANACOST0, 0, NULL, NULL);
	}

	if (event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION){
		int choice = DIALOG(player, card, event,
							"Gain flying", 1, 2,
							"Lose flying", 1, 1,
							"Phase out", has_mana_for_activated_ability(player, card, MANACOST_XU(3,2)), 3);

		if (event == EVENT_ACTIVATE){
			if (choice == 3)
				charge_mana_for_activated_ability(player, card, MANACOST_XU(3,2));
		} else {	// event == EVENT_RESOLVE_ACTIVATION
			if (!in_play(instance->parent_controller, instance->parent_card)){
				spell_fizzled = 1;
			} else {
				card_instance_t* parent = get_card_instance(player, instance->parent_card);
				switch (choice){
					case 1:	parent->targets[1].player = 66;	break;
					case 2:	parent->targets[1].player = 0;	break;
					case 3:	phase_out(instance->parent_controller, instance->parent_card);	break;
				}
			}
		}
	}

	return 0;
}

int card_moss_diamond(int player, int card, event_t event){
/* CARD_ID_MOSS_DIAMOND	2547
Moss Diamond	|2
Artifact
~ enters the battlefield tapped.
|T: Add |G to your mana pool. */

	return mirage_diamonds(player, card, event);
}

int card_mountain_valley(int player, int card, event_t event){
	/* Mountain Valley	""
	 * Land
	 * ~ enters the battlefield tapped.
	 * |T, Sacrifice ~: Search your library for |Ha Mountain or |H2Forest card and put it onto the battlefield. Then shuffle your library. */
	return old_fetch(player, card, event, SUBTYPE_MOUNTAIN, SUBTYPE_FOREST);
}

int card_mtenda_griffin(int player, int card, event_t event){
/*
Mtenda Griffin |3|W
Creature — Griffin 2/2
Flying
{W}, {T}: Return Mtenda Griffin to its owner's hand and return target Griffin card from your graveyard to your hand.
		Activate this ability only during your upkeep.
*/
	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	test_definition_t this_test;
	new_default_test_definition(&this_test, TYPE_PERMANENT, "Select a Griffin card.");
	this_test.subtype = SUBTYPE_GRIFFIN;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		int selected = validate_target_from_grave(player, card, player, 0);
		if( selected != -1 ){
			bounce_permanent(instance->parent_controller, instance->parent_card);
			from_grave_to_hand(player, selected, TUTOR_HAND);
		}
	}

	return generic_activated_ability_with_target_in_grave(player, card, event, GAA_UNTAPPED, MANACOST_W(1), 0, NULL, NULL,
															0, &this_test, GS_GRAVE_RECYCLER);
}

int card_mtenda_lion(int player, int card, event_t event)
{
/* CARD_ID_MTENDA_LION	2551
Mtenda Lion	|G
Creature - Cat 2/1
Whenever ~ attacks, defending player may pay |U. If that player does, prevent all combat damage that would be dealt by ~ this turn. */

  if (declare_attackers_trigger(player, card, event, 0, player, card)
	  && has_mana(1-current_turn, COLOR_BLUE, 1)
	  && do_dialog(1-current_turn, player, card, -1, -1, " Pay U\n Decline", 0) == 0
	  && charge_mana_while_resolving(player, card, EVENT_RESOLVE_TRIGGER, 1-current_turn, COLOR_BLUE, 1))
	negate_combat_damage_this_turn(player, card, player, card, 0);

  return 0;
}

int card_mystical_tutor(int player, int card, event_t event){
/* CARD_ID_MYSTICAL_TUTOR	1527
Mystical Tutor	|U
Instant
Search your library for an instant or sorcery card and reveal that card. Shuffle your library, then put the card on top of it. */

	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t this_test;
		new_default_test_definition(&this_test, TYPE_SPELL, "Select an instant or sorcery card.");
		new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_DECK, 0, AI_MAX_VALUE, &this_test);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_natural_balance(int player, int card, event_t event){
/* CARD_ID_NATURAL_BALANCE	2552
Natural Balance	|2|G|G
Sorcery
Each player who controls six or more lands chooses five lands he or she controls and sacrifices the rest.
Each player who controls four or fewer lands may search his or her library for up to X basic land cards and put them onto the battlefield,
where X is five minus the number of lands he or she controls. Then each player who searched his or her library this way shuffles it. */

	if( event == EVENT_RESOLVE_SPELL ){
		APNAP(p,{
					int amount = count_subtype(p, TYPE_LAND, -1);
					if( amount > 5 ){
						test_definition_t test;
						new_default_test_definition(&test, TYPE_LAND, "Select a land to sacrifice.");
						test.qty = amount-5;
						new_sacrifice(player, card, p, SAC_CAUSED | SAC_NO_CANCEL, &test);
					}
					else if( amount < 5 ){
							test_definition_t this_test;
							new_default_test_definition(&this_test, TYPE_LAND, "Select a basic land card.");
							this_test.subtype = SUBTYPE_BASIC;
							this_test.qty = 5-amount;
							new_global_tutor(p, p, TUTOR_FROM_DECK, TUTOR_PLAY, 0, AI_FIRST_FOUND, &this_test);
					}
				};
		);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_null_chamber(int player, int card, event_t event){
/*
Null Chamber |3|W
World Enchantment
As Null Chamber enters the battlefield, you and an opponent each name a card other than a basic land card.
The named cards can't be played.
*/
	if( event == EVENT_RESOLVE_SPELL ){
		card_instance_t *instance = get_card_instance( player, card);
		APNAP(p,{
					int csvid = -1;
					if( IS_AI(p) ){
						test_definition_t test;
						default_test_definition(&test, TYPE_LAND);
						test.subtype = SUBTYPE_BASIC;
						test.subtype_flag = DOESNT_MATCH;
						csvid = ai_name_a_random_csvid(player, card, 1-p, 0, &test);
					}
					else{
						csvid = card_from_list(p, 3, TYPE_ANY, 0, SUBTYPE_BASIC, 1, 0, 0, 0, 0, -1, 0);
					}
					instance->targets[p].card = csvid;
				}
		);
	}

	if( event == EVENT_MODIFY_COST_GLOBAL && ! is_humiliated(player, card) ){
		card_instance_t *instance = get_card_instance( player, card);
		if( get_id(affected_card_controller, affected_card) == instance->targets[affected_card_controller].card ){
			infinite_casting_cost();
		}
	}

	return global_enchantment(player, card, event);
}

int card_paupers_cage(int player, int card, event_t event){
/* CARD_ID_PAUPERS_CAGE	2557
Paupers' Cage	|3
Artifact
At the beginning of each opponent's upkeep, if that player has two or fewer cards in hand, ~ deals 2 damage to him or her. */

	upkeep_trigger_ability(player, card, event, 1-player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		if( hand_count[1-player] < 3 ){
			damage_player(1-player, 2, player, card);
		}
	}

	return 0;
}

int card_pacifism(int player, int card, event_t event)
{
/* CARD_ID_PACIFISM	968
Pacifism	|1|W
Enchantment - Aura
Enchant creature
Enchanted creature can't attack or block. */

	// Used externally
	if (event == EVENT_ABILITIES && in_play(player, card) && ! is_humiliated(player, card) )
	{
		card_instance_t* instance = get_card_instance(player, card);
		if (instance->damage_target_player >= 0 && instance->damage_target_card >= 0)
		{
			cannot_attack(instance->damage_target_player, instance->damage_target_card, event);
			cannot_block(instance->damage_target_player, instance->damage_target_card, event);
		}
	}

	return disabling_aura(player, card, event);
}

int card_patagia_golem(int player, int card, event_t event){
	/*
	  Patagia Golem |4
	  Artifact Creature - Golem 2/3
	  {3}: Patagia Golem gains flying until end of turn.
	*/

	return generic_shade(player, card, event, 0, MANACOST_X(3), 0, 0, KEYWORD_FLYING, 0);
}

int card_pearl_dragon(int player, int card, event_t event){
/* CARD_ID_PEARL_DRAGON	1670
Pearl Dragon	|4|W|W
Creature - Dragon 4/4
Flying
|1|W: ~ gets +0/+1 until end of turn. */

	return generic_shade_merge_pt(player, card, event, 0, MANACOST_XW(1, 1), 0,1);
}

int card_phyrexian_dreadnought(int player, int card, event_t event){
/*
Phyrexian Dreadnought |12
Artifact Creature — Dreadnought
Trample
When Phyrexian Dreadnought enters the battlefield, sacrifice it unless you sacrifice any number of creatures with total power 12 or greater.
*/
	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_CREATURE, "Select a creature to sacrifice.");

		state_untargettable(player, card, 1);

		int sac_power = 0;
		int sacced[151];
		int sc = 0;
		while( new_can_sacrifice(player, card, player, &test) ){
				int sac = new_sacrifice(player, card, player, SAC_JUST_MARK | SAC_RETURN_CHOICE, &test);
				if( ! sac ){
					break;
				}
				else{
					sacced[sc] = sac;
					sac_power += get_power(BYTE2(sac), BYTE3(sac));
					sc++;
					if( sac_power >= 12 && IS_AI(player) ){
						break;
					}
				}
		}
		int i;
		for(i=0; i<sc; i++){
			state_untargettable(BYTE2(sacced[i]), BYTE3(sacced[i]), 0);
			kill_card(BYTE2(sacced[i]), BYTE3(sacced[i]), KILL_SACRIFICE);
		}
		if( sac_power < 12 ){
			kill_card(player, card, KILL_SACRIFICE);
		}
	}

	return 0;
}

int card_phyrexian_purge(int player, int card, event_t event){
/*
Phyrexian Purge |2|B|R
Sorcery
Destroy any number of target creatures.
Phyrexian Purge costs 3 life more to cast for each target.
*/
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.allow_cancel = 2; //"Done" button.

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST && can_pay_life(player, 3) ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		int life_to_pay = 0;
		int t = 0;
		while( can_target(&td) && can_pay_life(player, life_to_pay) ){
				if( new_pick_target(&td, "TARGET_CREATURE", t, 0) ){
					state_untargettable(instance->targets[t].player, instance->targets[t].card, 1);
					t++;
					life_to_pay += 3;
					if( life[player] - life_to_pay < 6 && IS_AI(player) ){
						break;
					}
				}
				else{
					break;
				}
		}
		int i;
		for(i=0; i<t; i++){
			state_untargettable(instance->targets[i].player, instance->targets[i].card, 0);
		}
		lose_life(player, life_to_pay);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		int k;
		for(k=0; k<instance->number_of_targets; k++){
			if( validate_target(player, card, &td, k) ){
				kill_card( instance->targets[k].player, instance->targets[k].card, KILL_DESTROY);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_phyrexian_tribute(int player, int card, event_t event){
/*
Phyrexian Tribute |2|B
Sorcery
As an additional cost to cast Phyrexian Tribute, sacrifice two creatures.
Destroy target artifact.
*/

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_ARTIFACT);

	test_definition_t test;
	new_default_test_definition(&test, TYPE_CREATURE, "Select a creature to sacrifice.");

	card_instance_t* instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		test.qty = 2;
		if( new_can_sacrifice_as_cost(player, card, &test) ){
			return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_ARTIFACT", 1, NULL);
		}
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		instance->number_of_targets = 0;
		int sacced[2] = {-1, -1};
		int i;
		for(i=0; i<2; i++){
			sacced[i] = new_sacrifice(player, card, player, SAC_JUST_MARK|SAC_AS_COST|SAC_RETURN_CHOICE, &test);
		}
		if( sacced[0] == -1 || sacced[1] == -1 ){
			spell_fizzled = 1;
			return 0;
		}
		if( pick_target(&td, "TARGET_ARTIFACT") ){
			for(i=0; i<2; i++){
				state_untargettable(BYTE2(sacced[i]), BYTE3(sacced[i]), 0);
				kill_card(BYTE2(sacced[i]), BYTE3(sacced[i]), KILL_SACRIFICE);
			}
		}
		else{
			for(i=0; i<2; i++){
				if( sacced[i] > -1 ){
					state_untargettable(BYTE2(sacced[i]), BYTE3(sacced[i]), 0);
				}
			}
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}


int card_phyrexian_vault(int player, int card, event_t event){
/* CARD_ID_PHYREXIAN_VAULT	2561
Phyrexian Vault	|3
Artifact
|2, |T, Sacrifice a creature: Draw a card. */

	if( event == EVENT_RESOLVE_ACTIVATION ){
		draw_cards(player, 1);
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_SACRIFICE_CREATURE, MANACOST_X(2), 0, NULL, NULL);
}

int card_political_trickery(int player, int card, event_t event){
/* CARD_ID_POLITICAL_TRICKERY	2562
Political Trickery	|2|U
Sorcery
Exchange control of target land you control and target land an opponent controls. (This effect lasts indefinitely.) */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_LAND );
	td.preferred_controller = player;
	td.allowed_controller = player;

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_LAND );
	td1.preferred_controller = 1-player;
	td1.allowed_controller = 1-player;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST && can_target(&td1) ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, NULL, 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		instance->number_of_targets = 0;
		if( new_pick_target(&td, "Select target land you control.", 0, 1 | GS_LITERAL_PROMPT) ){
			new_pick_target(&td1, "Select target land opponent controls.", 1, 1 | GS_LITERAL_PROMPT);
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( validate_target(player, card, &td, 0) && validate_target(player, card, &td1, 1) ){
			exchange_control_of_target_permanents(player, card, instance->targets[0].player, instance->targets[0].card,
													instance->targets[1].player, instance->targets[1].card);
		}
		kill_card(player, card, KILL_DESTROY);
	}
	return 0;
}

int card_polymorph(int player, int card, event_t event){
/* CARD_ID_POLYMORPH	2563
Polymorph	|3|U
Sorcery
Destroy target creature. It can't be regenerated. Its controller reveals cards from the top of his or her library until he or she reveals a creature card.
The player puts that card onto the battlefield, then shuffles all other cards revealed this way into his or her library. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE );
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			metamorphosis(instance->targets[0].player, instance->targets[0].card, TYPE_CREATURE, KILL_BURY);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
}

int card_preferred_selection(int player, int card, event_t event){
/*
Preferred Selection |2|G|G
Enchantment, 2GG (4)
At the beginning of your upkeep, look at the top two cards of your library. You may sacrifice Preferred Selection and pay {2}{G}{G}.
If you do, put one of those cards into your hand. If you don't, put one of those cards on the bottom of your library.
*/
	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		int amount = MIN(2, count_deck(player));
		if( amount ){
			char msg[100];
			int pos = scnprintf(msg, 100, "Here's the first ");
			if( amount > 1 ){
				pos += scnprintf(msg + pos, 100-pos, "2 cards");
			}
			else{
				pos += scnprintf(msg + pos, 100-pos, "card");
			}
			scnprintf(msg + pos, 100-pos, " of your deck.");
			show_deck(player, deck_ptr[player], amount, msg, 0, 0x7375B0);

			int choice = 2;
			if( has_mana_multi(player, MANACOST_XG(2, 2)) && ! check_special_flags2(player, card, SF2_CANNOT_BE_SACRIFICED) ){
				choice = DIALOG(player, card, EVENT_ACTIVATE, DLG_RANDOM, DLG_NO_STORAGE, SAC_NO_CANCEL,
								"Sac Preferred Selection", 	1, hand_count[player] < 3 ? 10 : 1,
								"Decline", 					1, 5);
			}
			if( choice == 1 ){
				if( charge_mana_multi_while_resolving(player, card, EVENT_RESOLVE_TRIGGER, player, MANACOST_XG(2, 2)) ){
					test_definition_t test;
					new_default_test_definition(&test, TYPE_ANY, "Select a card to put into your hand.");
					test.create_minideck = amount;
					test.no_shuffle = 1;
					new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_HAND, 1, AI_MAX_VALUE, &test);
					kill_card(player, card, KILL_SACRIFICE);
				}
			}
			if( choice == 2 ){
				test_definition_t test;
				new_default_test_definition(&test, TYPE_ANY, "Select a card to put into your hand.");
				test.create_minideck = amount;
				test.no_shuffle = 1;
				new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_BOTTOM_OF_DECK, 1, AI_MAX_VALUE, &test);
			}
		}
	}

	return global_enchantment(player, card, event);
}

int card_prismatic_boon(int player, int card, event_t event){
/*
Prismatic Boon |X|W|U
Instant
Choose a color. X target creatures gain protection from the chosen color until end of turn.
*/
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_X_SPELL, NULL, NULL, 0, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		instance->number_of_targets = 0;
		int diff = true_get_updated_casting_cost(player, card, -1, event, -1);
		int max_targ = has_mana(player, COLOR_ANY, 1) - diff;
		int t = 0;
		while( t < max_targ && can_target(&td) ){
				if( new_pick_target(&td, "TARGET_CREATURE", t, 0) ){
					state_untargettable(instance->targets[t].player, instance->targets[t].card, 1);
					t++;
				}
				else{
					break;
				}
		}
		int i;
		for(i=0; i<t; i++){
			state_untargettable(instance->targets[i].player, instance->targets[i].card, 0);
		}
		charge_mana(player, COLOR_ANY, t + diff);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		int keyword = select_a_protection(player) | KEYWORD_RECALC_SET_COLOR;
		int i;
		for(i=0; i<instance->number_of_targets; i++){
			if( validate_target(player, card, &td, i) ){
				pump_ability_until_eot(player, card, instance->targets[i].player, instance->targets[i].card, 0, 0, keyword, 0);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_psychic_transfer(int player, int card, event_t event){
/*
Psychic Transfer |4|U
Sorcery
If the difference between your life total and target player's life total is 5 or less, exchange life totals with that player.
*/
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_LAND );
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			int min_life = life[player];
			int max_life = life[1-player];
			if( life[player] >  life[1-player] ){
				min_life = life[1-player];
				max_life = life[player];
			}
			if( max_life -  min_life <= 5 ){
				exchange_life_totals(player, instance->targets[0].player);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_ONLY_TARGET_OPPONENT, &td, NULL, 1, NULL);
}

int card_purgatory(int player, int card, event_t event){
/* CARD_ID_PURGATORY	2569
Purgatory	|2|W|B
Enchantment
Whenever a nontoken creature is put into your graveyard from the battlefield, exile that card.
At the beginning of your upkeep, you may pay |4 and 2 life. If you do, return a card exiled with ~ to the battlefield. */

	if(event == EVENT_GRAVEYARD_FROM_PLAY && in_play(affected_card_controller, affected_card) ){
		if( affected_card_controller == player && is_what(affected_card_controller, affected_card, TYPE_CREATURE) &&
			! is_token(affected_card_controller, affected_card) && ! is_stolen(affected_card_controller, affected_card) )
		{
			card_instance_t *instance = get_card_instance(player, card);
			card_instance_t *dead = get_card_instance(affected_card_controller, affected_card);
			if( dead->kill_code > 0 && dead->kill_code != KILL_EXILE ){
				if( instance->targets[0].card < 1 ){
					instance->targets[0].card = 1;
				}
				int souls = instance->targets[0].card;
				if( souls < 10 ){
					instance->targets[souls].card = get_original_id(affected_card_controller, affected_card);
					instance->targets[0].card++;
				}
				exiledby_remember(player, card, player, get_original_internal_card_id(affected_card_controller, affected_card), NULL, NULL);
				dead->kill_code = KILL_EXILE;
			}
		}
	}

	if( current_turn == player && trigger_condition == TRIGGER_UPKEEP ){
		if( has_mana(player, COLOR_COLORLESS, 4) && can_pay_life(player, 2) &&
			exiledby_choose(player, card, CARD_ID_PURGATORY, EXBY_FIRST_FOUND, 0, "creature", 1) )
		{
			upkeep_trigger_ability_mode(player, card, event, player, RESOLVE_TRIGGER_AI(player));
		}
	}

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		if( has_mana(player, COLOR_COLORLESS, 4) && can_pay_life(player, 2) &&
			exiledby_choose(player, card, CARD_ID_PURGATORY, EXBY_FIRST_FOUND, 0, "creature", 1) )
		{
			int rval = exiledby_choose(player, card, CARD_ID_PURGATORY, EXBY_CHOOSE, 0, "Creature", 1);
			charge_mana(player, COLOR_COLORLESS, 4);
			if( spell_fizzled != -1 ){
				int* loc = (int*)rval;
				if( loc ){
					int *loc2 = exiledby_find(player, card, *loc, NULL, NULL);
					int owner = *loc2 & 0x80000000 ? 1 : 0;
					int iid = *loc2 & ~0x80000000;
					*loc2 = -1;
					if( check_rfg(owner, cards_data[iid].id) ){
						int card_added = add_card_to_hand(player, iid);
						remove_card_from_rfg(owner, cards_data[iid].id);
						put_into_play(player, card_added);
						lose_life(player, 2);
					}
				}
			}
		}
	}

   return global_enchantment(player, card, event);
}

int card_purraj_of_urborg(int player, int card, event_t event){
/* CARD_ID_PURRAJ_OF_URBORG	2570
Purraj of Urborg	|3|B|B
Legendary Creature - Cat Warrior 2/3
~ has first strike as long as it's attacking.
Whenever a player casts a |Sblack spell, you may pay |B. If you do, put a +1/+1 counter on ~. */

	if( trigger_condition == TRIGGER_SPELL_CAST && affect_me(player, card) && player == reason_for_trigger_controller){
		test_definition_t test;
		default_test_definition(&test, TYPE_ANY);
		test.color = get_sleighted_color_test(player, card, COLOR_TEST_BLACK);
		if( new_specific_spell_played(player, card, event, ANYBODY, RESOLVE_TRIGGER_AI(player), &test) ){
			if( charge_mana_while_resolving(player, card, EVENT_RESOLVE_TRIGGER, player, COLOR_BLACK, 1) ){
				add_1_1_counter(player, card);
			}
		}
	}

	if( event == EVENT_ABILITIES && affect_me(player, card) && is_attacking(player, card) && ! is_humiliated(player, card) ){
		event_result |= KEYWORD_FIRST_STRIKE;
	}

	return 0;
}

int card_quirion_elves(int player, int card, event_t event){

	/* Quirion Elves	|1|G
	 * Creature - Elf Druid 1/1
	 * As ~ enters the battlefield, choose a color.
	 * |T: Add |G to your mana pool.
	 * |T: Add one mana of the chosen color to your mana pool. */

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		int clr = get_deck_color(player, player);	// best color in deck is non-green? use it.
		if (clr == COLOR_GREEN){
			clr = get_deck_color(-1, player);		// find two best colors and pick the non-green one
			clr &= ~COLOR_TEST_GREEN;
			clr = single_color_test_bit_to_color(clr);
		}
		instance->info_slot = COLOR_TEST_GREEN | (1 << choose_a_color(player, clr));
	}

	return mana_producing_creature_all_one_color(player, card, event, 24, instance->info_slot, 1);
}

int card_rampant_growth(int player, int card, event_t event){
	/*
	  Rampant Growth |1|G
	  Sorcery, 1G (2)
	  Search your library for a basic land card and put that card onto the battlefield tapped. Then shuffle your library.
	*/
	// Used externally
	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += (64 -(count_subtype(player, TYPE_LAND, -1)*8));
	}

	if( event == EVENT_RESOLVE_SPELL ){
		tutor_basic_land(player, 1, 1);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_rashida_scalebane(int player, int card, event_t event){
/* CARD_ID_RASHIDA_SCALEBANE	2575
Rashida Scalebane	|3|W|W
Legendary Creature - Human Soldier 3/4
|T: Destroy target attacking or blocking Dragon. It can't be regenerated. You gain life equal to its power. */

	check_legend_rule(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.required_subtype = SUBTYPE_DRAGON;
	td.required_state = TARGET_STATE_ATTACKING;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			gain_life(player, get_power(instance->targets[0].player, instance->targets[0].card));
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET | GAA_LITERAL_PROMPT, MANACOST0, 0,
									&td, "Select target attacking Dragon creature.");
}

int card_razor_pendulum(int player, int card, event_t event){
/* CARD_ID_RAZOR_PENDULUM	2577
Razor Pendulum	|4
Artifact
At the beginning of each player's end step, if that player has 5 or less life, ~ deals 2 damage to him or her. */

	if( eot_trigger(player, card, event) ){
		if( life[current_turn] <= 5 ){
			damage_player(current_turn, 2, player, card);
		}
	}

	return 0;
}

int card_reflect_damage(int player, int card, event_t event){
/* CARD_ID_REFLECT_DAMAGE	2580
Reflect Damage	|3|R|W
Instant
The next time a source of your choice would deal damage this turn, that damage is dealt to that source's controller instead. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.extra = damage_card;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			card_instance_t *target = get_card_instance(instance->targets[0].player, instance->targets[0].card);
			target->damage_target_player = target->damage_source_player;
			target->damage_target_card = -1;
		}
		kill_card(player, card, KILL_DESTROY);
	}
	return generic_spell(player, card, event, GS_CAN_TARGET | GS_DAMAGE_PREVENTION, &td, "TARGET_DAMAGE", 1, NULL);
}

int card_reign_of_chaos(int player, int card, event_t event)
{
  /* Reign of Chaos	|2|R|R
   * Sorcery
   * Choose one - Destroy target |H2Plains and target |Swhite creature; or destroy target |H2Island and target |Sblue creature. */

  if (!IS_CASTING(player, card, event))
	return 0;

  target_definition_t td_plains;
  default_target_definition(player, card, &td_plains, TYPE_LAND);
  td_plains.required_subtype = SUBTYPE_PLAINS;

  target_definition_t td_white_creature;
  default_target_definition(player, card, &td_white_creature, TYPE_CREATURE);
  td_white_creature.required_color = get_sleighted_color_test(player, card, COLOR_TEST_WHITE);

  target_definition_t td_island;
  default_target_definition(player, card, &td_island, TYPE_LAND);
  td_island.required_subtype = SUBTYPE_ISLAND;

  target_definition_t td_blue_creature;
  default_target_definition(player, card, &td_blue_creature, TYPE_CREATURE);
  td_blue_creature.required_color = get_sleighted_color_test(player, card, COLOR_TEST_BLUE);

  char prompt_w[200], prompt_u[200];
  if (ai_is_speculating == 1 || event != EVENT_CAST_SPELL)
	prompt_w[0] = prompt_u[0] = 0;
  else
	{
	  // Order is important here; get_sleighted_color_text can deal with the escaped %%, but get_hacked_land text can't.
	  strcpy(prompt_w, get_hacked_land_text(player, card, get_sleighted_color_text(player, card, "%%s and %s creature", COLOR_WHITE), SUBTYPE_PLAINS));
	  strcpy(prompt_u, get_hacked_land_text(player, card, get_sleighted_color_text(player, card, "%%s and %s creature", COLOR_BLUE), SUBTYPE_ISLAND));
	}

  enum
  {
	CHOICE_W = 1,
	CHOICE_U
  } choice = DIALOG(player, card, event,
					DLG_RANDOM, DLG_AUTOCHOOSE_IF_1,
					prompt_w, can_target(&td_plains) && can_target(&td_white_creature), 1,
					prompt_u, can_target(&td_island) && can_target(&td_blue_creature), 1);

  if (event == EVENT_CAN_CAST)
	return choice;
  else if (choice)
	{
	  target_definition_t *td_land, *td_creature;
	  if (choice == CHOICE_W)
		{
		  td_land = &td_plains;
		  td_creature = &td_white_creature;
		}
	  else
		{
		  td_land = &td_island;
		  td_creature = &td_blue_creature;
		}

	  card_instance_t* instance = get_card_instance(player, card);

	  if (event == EVENT_CAST_SPELL && affect_me(player, card))
		{
		  instance->number_of_targets = 0;
		  if (pick_next_target_noload(td_land, get_hacked_land_text(player, card, "Select target %s.",
																	choice == CHOICE_W ? SUBTYPE_PLAINS : SUBTYPE_ISLAND)))
			pick_next_target_noload(td_creature, get_sleighted_color_text(player, card, "Select target %s creature.",
																		  choice == CHOICE_W ? COLOR_WHITE : COLOR_BLUE));
		}

	  if (event == EVENT_RESOLVE_SPELL)
		{
		  if (validate_target(player, card, td_land, 0))
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);

		  if (validate_target(player, card, td_creature, 1))
			kill_card(instance->targets[1].player, instance->targets[1].card, KILL_DESTROY);

		  kill_card(player, card, KILL_DESTROY);
		}
	}

  return 0;
}

int card_reparations(int player, int card, event_t event){
/* CARD_ID_REPARATIONS	2583
Reparations	|1|W|U
Enchantment
Whenever an opponent casts a spell that targets you or a creature you control, you may draw a card. */

	if(trigger_condition == TRIGGER_SPELL_CAST && affect_me(player, card) && player == reason_for_trigger_controller &&
	   1-player == trigger_cause_controller )
	{
		int trig = 0;
		int i;
		card_instance_t *target = get_card_instance( trigger_cause_controller, trigger_cause);
		for(i=0; i<target->number_of_targets; i++ ){
			if( target->targets[i].player == player ){
				trig = 1;
				break;
			}
		}

		if( trig && new_specific_spell_played(player, card, event, 1-player, RESOLVE_TRIGGER_AI(player), NULL) ){
			draw_cards(player, 1);
		}
	}

	return global_enchantment(player, card, event);
}

int card_rocky_tar_pit(int player, int card, event_t event){
	/* Rocky Tar Pit	""
	 * Land
	 * ~ enters the battlefield tapped.
	 * |T, Sacrifice ~: Search your library for |Ha Swamp or |H2Mountain card and put it onto the battlefield. Then shuffle your library. */
	return old_fetch(player, card, event, SUBTYPE_SWAMP, SUBTYPE_MOUNTAIN);
}

int card_reality_ripple(int player, int card, event_t event){
/* CARD_ID_REALITY_RIPPLE	2578
Reality Ripple	|1|U
Instant
Target artifact, creature, or land phases out. (While it's phased out, it's treated as though it doesn't exist.
It phases in before its controller untaps during his or her next untap step.) */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE | TYPE_ARTIFACT | TYPE_LAND);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			phase_out(instance->targets[0].player, instance->targets[0].card);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET | GS_LITERAL_PROMPT, &td, "Select target creature, artifact or land", 1, NULL);
}

int card_sacred_mesa(int player, int card, event_t event){
	/* Sacred Mesa	|2|W
	 * Enchantment
	 * At the beginning of your upkeep, sacrifice ~ unless you sacrifice a Pegasus.
	 * |1|W: Put a 1/1 |Swhite Pegasus creature token with flying onto the battlefield. */

	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_PERMANENT, "Select a Pegasus to sacrifice.");
		test.subtype = SUBTYPE_PEGASUS;
		int kill = 1;
		if( new_can_sacrifice(player, card, player, &test) ){
			if( new_sacrifice(player, card, player, 0, &test) ){
				kill = 0;
			}
		}
		if( kill == 1 ){
			kill_card(player, card, KILL_SACRIFICE);
		}
	}

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance(player, card);
		generate_token_by_id(instance->parent_controller, instance->parent_card, CARD_ID_PEGASUS);
	}

	return generic_activated_ability(player, card, event, 0, MANACOST_XW(1, 1), 0, NULL, NULL);
}

int card_sandbar_crocodile(int player, int card, event_t event){
/* CARD_ID_SANDBAR_CROCODILE	2592
Sandbar Crocodile	|4|U
Creature - Crocodile 6/5
Phasing (This phases in or out before you untap during each of your untap steps. While it's phased out, it's treated as though it doesn't exist.) */

	return phasing(player, card, event);
}

int card_savage_twister(int player, int card, event_t event){
/* CARD_ID_SAVAGE_TWISTER	2594
Savage Twister	|X|R|G
Sorcery
~ deals X damage to each creature. */

	if( event == EVENT_RESOLVE_SPELL ){
		card_instance_t *instance = get_card_instance(player, card);
		new_damage_all(player, card, ANYBODY, instance->info_slot, NDA_ALL_CREATURES, NULL);
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_X_SPELL, NULL, NULL, 0, NULL);
}

int card_sealed_fate(int player, int card, event_t event){
/*
Sealed Fate |X|U|B
Sorcery
Look at the top X cards of target opponent's library. Exile one of those cards and put the rest back on top of that player's library
	in any order.
*/
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE );
	td.zone = TARGET_ZONE_PLAYERS;
	td.allowed_controller = 1-player;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		generic_spell(player, card, event, GS_X_SPELL | GS_CAN_ONLY_TARGET_OPPONENT, &td, "TARGET_OPPONENT", 1, NULL);
		if( spell_fizzled != 1 ){
			int mod = 0;
			if( instance->targets[0].player == 1-player ){
				mod = count_subtype(player, TYPE_LAND, -1) >= 6 ? 48 : -48;
			}
			ai_modifier += mod;
		}
		return 0;
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			int amount = get_card_instance(player, card)->info_slot;
			if( amount ){
				test_definition_t test;
				new_default_test_definition(&test, TYPE_ANY, "Select a card to exile.");
				test.qty = 1;
				test.no_shuffle = 1;
				reveal_top_cards_of_library_and_choose(player, card, 1-player, amount, 1, TUTOR_RFG, 1, TUTOR_DECK, 0, &test);
				amount--;
				if( amount ){
					rearrange_top_x(instance->targets[0].player, player, amount);
				}
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_X_SPELL | GS_CAN_ONLY_TARGET_OPPONENT, &td, "TARGET_OPPONENT", 1, NULL);
}

int card_seeds_of_innocence(int player, int card, event_t event){
/* CARD_ID_SEEDS_OF_INNOCENCE	2600
Seeds of Innocence	|1|G|G
Sorcery
Destroy all artifacts. They can't be regenerated. The controller of each of those artifacts gains life equal to its converted mana cost. */

	if( event == EVENT_RESOLVE_SPELL ){
		APNAP(p,{
					int c;
					int life_to_gain = 0;
					for(c = active_cards_count[p]-1; c > -1; c--){
						if( in_play(p, c) && is_what(p, c, TYPE_ARTIFACT) ){
							life_to_gain += get_cmc(p, c);
							kill_card(p, c, KILL_BURY);
						}
					}
					gain_life(p, life_to_gain);
				}
		);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_serene_heart(int player, int card, event_t event)
{
/* CARD_ID_SERENE_HEART	2601
Serene Heart	|1|G
Instant
Destroy all Auras. */

	if (event == EVENT_RESOLVE_SPELL)
	{
		test_definition_t test;
		new_default_test_definition(&test, TYPE_ENCHANTMENT, "");
		test.subtype = SUBTYPE_AURA;
		test.subtype_flag = MATCH;
		APNAP(p, { new_manipulate_all(player, card, p, &test, KILL_DESTROY); };);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_sewer_rats(int player, int card, event_t event){
/* CARD_ID_SEWER_RATS	2602
Sewer Rats	|B
Creature - Rat 1/1
|B, Pay 1 life: ~ gets +1/+0 until end of turn. Activate this ability no more than three times each turn. */

	if( event == EVENT_CAN_ACTIVATE ){
		if( get_card_instance(player, card)->info_slot == 3 ){
			return 0;
		}
	}

	if( event == EVENT_ACTIVATE ){
		generic_shade_merge_pt(player, card, event, 1, MANACOST_B(1), 1, 0);
		if( spell_fizzled != 1 ){
			get_card_instance(player, card)->info_slot++;
		}
		return 0;
	}

	if( event == EVENT_CLEANUP ){
		get_card_instance(player, card)->info_slot = 0;
	}

	return generic_shade_merge_pt(player, card, event, 1, MANACOST_B(1), 1, 0);
}

int card_shadow_guildmage(int player, int card, event_t event){
	/* Shadow Guildmage	|B	0x200314e
	 * Creature - Human Wizard 1/1
	 * |U, |T: Put target creature you control on top of its owner's library.
	 * |R, |T: ~ deals 1 damage to any target and 1 damage to you. */
	return mirage_guildmage(player, card, event, COLOR_BLUE, COLOR_RED);
}

int card_shallow_grave(int player, int card, event_t event){
/* CARD_ID_SHALLOW_GRAVE	2605
Shallow Grave	|1|B
Instant
Return the top creature card of your graveyard to the battlefield. That creature gains haste until end of turn. Exile it at the beginning of the next end step.*/

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += (current_turn == player ? (current_phase < PHASE_DECLARE_ATTACKERS ? 48 : -48) :
												(current_phase < PHASE_DECLARE_BLOCKERS ? 48 : -48));
	}

	if( event == EVENT_RESOLVE_SPELL ){
		int count = count_graveyard(player)-1;
		const int *grave = get_grave(player);
		while( count > -1 && grave[count] != -1 ){
				if( cards_data[grave[count]].type & TYPE_CREATURE ){
					reanimate_permanent(player, card, player, count, REANIMATE_HASTE_AND_EXILE_AT_EOT);
					break;
				}
				count--;
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_shaper_guildmage(int player, int card, event_t event){
/* CARD_ID_SHAPER_GUILDMAGE	2606
Shaper Guildmage	|U
Creature - Human Wizard 1/1
|W, |T: Target creature gains first strike until end of turn.
|B, |T: Target creature gets +1/+0 until end of turn. */

	return mirage_guildmage(player, card, event, COLOR_BLACK, COLOR_WHITE);
}

int card_shauku_endbringer(int player, int card, event_t event){
/* CARD_ID_SHAUKU_ENDBRINGER	2608
Shauku, Endbringer	|5|B|B
Legendary Creature - Vampire 5/5
Flying
~ can't attack if there's another creature on the battlefield.
At the beginning of your upkeep, you lose 3 life.
|T: Exile target creature and put a +1/+1 counter on ~. */

	check_legend_rule(player, card, event);

	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		lose_life(player, 3);
	}

	if( event == EVENT_ATTACK_LEGALITY && affect_me(player, card) && count_subtype(2, TYPE_CREATURE, -1) >= 2 &&
		! is_humiliated(player, card) )
	{
		event_result = 1;
	}

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
			add_1_1_counter(instance->parent_controller, instance->parent_card);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST0, 0, &td, "TARGET_CREATURE");
}

int card_shimmer(int player, int card, event_t event){
/* CARD_ID_SHIMMER	2609
Shimmer	|2|U|U
Enchantment
As ~ enters the battlefield, choose a land type.
Each land of the chosen type has phasing. (It phases in or out before its controller untaps during each of his or her untap steps.
While it's phased out, it's treated as though it doesn't exist.) */

	if( event == EVENT_RESOLVE_SPELL ){
		// These are all the current land subtypes from rule 205.3i; this card can choose non-basic land types
		int choice = do_dialog(player, player, card, -1, -1,
							   " Forest\n Island\n Mountain\n Plains\n Swamp\n Desert\n Gate\n Lair\n Locus\n Mine\n Power-Plant\n Tower\n Urza's", 0);

		card_instance_t* instance = get_card_instance( player, card );

		instance->info_slot = 1 << choice;
	}

	// Main effect handled directly in untap_phasing.

	return global_enchantment(player, card, event);
}

int card_sidar_jabari(int player, int card, event_t event){
/* CARD_ID_SIDAR_JABARI	2610
Sidar Jabari	|3|W
Legendary Creature - Human Knight 2/2
Flanking (Whenever a creature without flanking blocks this creature, the blocking creature gets -1/-1 until end of turn.)
Whenever ~ attacks, tap target creature defending player controls. */

	check_legend_rule(player, card, event);

	flanking(player, card, event);

	return card_master_of_diversion(player, card, event);
}

int card_skulking_ghost(int player, int card, event_t event){
/* CARD_ID_SKULKING_GHOST	2612
Skulking Ghost	|1|B
Creature - Spirit 2/1
Flying
When ~ becomes the target of a spell or ability, sacrifice it. */

	// Used externally
	if( in_play(player, card) && !is_humiliated(player, card) &&
		becomes_target_of_spell_or_effect(player, card, event, player, card, ANYBODY) )
		kill_card(player, card, KILL_SACRIFICE);

	return 0;
}

int card_sky_diamond(int player, int card, event_t event){
/* CARD_ID_SKY_DIAMOND	2613
Sky Diamond	|2
Artifact
~ enters the battlefield tapped.
|T: Add |U to your mana pool. */

	return mirage_diamonds(player, card, event);
}

int card_soulshriek(int player, int card, event_t event){
/* CARD_ID_SOULSHRIEK	2617
Soulshriek	|B
Instant
Target creature you control gets +X/+0 until end of turn, where X is the number of creature cards in your graveyard.
Sacrifice that creature at the beginning of the next end step. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;
	td.allowed_controller = player;

	card_instance_t *instance = get_card_instance(player, card);

	if(event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			int amount = count_graveyard_by_type(player, TYPE_CREATURE);
			pump_ability_t pump;
			default_pump_ability_definition(player, card, &pump, amount, 0, 0, 0);
			pump.paue_flags = PAUE_END_AT_EOT | PAUE_REMOVE_TARGET_AT_EOT;
			pump.eot_removal_method = KILL_SACRIFICE;
			pump_ability(player, card, instance->targets[0].player, instance->targets[0].card, &pump);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET | GS_LITERAL_PROMPT, &td, "Select target creature you control.", 1, NULL);
}

int spatial_binding_legacy(int player, int card, event_t event){

	if( get_card_instance(player, card)->damage_target_player > -1 ){
		card_instance_t *instance = get_card_instance(player, card);

		int p = instance->damage_target_player;
		int c = instance->damage_target_card;

		if( current_turn == p && upkeep_trigger(player, card, event) && count_upkeeps(p) > 0){
			remove_special_flags(p, c, SF_CANNOT_PHASE_OUT);
			kill_card(player, card, KILL_EXILE);
		}
	}

	return 0;
}

int card_spatial_binding(int player, int card, event_t event){
/* CARD_ID_SPATIAL_BINDING	2618
Spatial Binding	|U|B
Enchantment
Pay 1 life: Until your next upkeep, target permanent can't phase out. */

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_PERMANENT);
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			set_special_flags(instance->targets[0].player, instance->targets[0].card, SF_CANNOT_PHASE_OUT);
			create_targetted_legacy_effect(instance->parent_controller, instance->parent_card, &spatial_binding_legacy, instance->targets[0].player, instance->targets[0].card);
		}
	}
	return generic_activated_ability(player, card, event, GAA_CAN_TARGET, MANACOST0, 1, &td, "TARGET_PERMANENT");
}

int card_spectral_guardian(int player, int card, event_t event){
/* CARD_ID_SPECTRAL_GUARDIAN	2619
Spectral Guardian	|2|W|W
Creature - Spirit 2/3
As long as ~ is untapped, noncreature artifacts have shroud. (They can't be the targets of spells or abilities.) */

	if( event == EVENT_ABILITIES && in_play(player, card) && ! is_humiliated(player, card) ){
		if( is_what(affected_card_controller, affected_card, TYPE_ARTIFACT) &&
			! is_what(affected_card_controller, affected_card, TYPE_CREATURE) )
		{
			event_result |= KEYWORD_SHROUD;
		}
	}

	return 0;
}

int card_spirit_of_the_night(int player, int card, event_t event){
/* CARD_ID_SPIRIT_OF_THE_NIGHT	1299
Spirit of the Night	|6|B|B|B
Legendary Creature - Demon Spirit 6/5
Flying, trample, haste, protection from |Sblack
~ has first strike as long as it's attacking. */

	check_legend_rule(player, card, event);

	pro_black(player, card, event);

	haste(player, card, event);

	if( event == EVENT_ABILITIES && affect_me(player, card) && in_play(player, card) && ! is_humiliated(player, card) ){
		if( is_attacking(player, card) ){
			event_result |= KEYWORD_FIRST_STRIKE;
		}
	}

	return 0;
}

int card_stupor(int player, int card, event_t event)
{
  /* Stupor	|2|B
   * Sorcery
   * Target opponent discards a card at random, then discards a card. */

  if (!IS_GS_EVENT(player, card, event))
	return 0;

  target_definition_t td;
  default_target_definition(player, card, &td, 0);
  td.zone = TARGET_ZONE_PLAYERS;

  if (event == EVENT_RESOLVE_SPELL)
	{
	  if (valid_target(&td))
		{
		  int t_player = get_card_instance(player, card)->targets[0].player;
		  discard(t_player, DISC_RANDOM, player);
		  discard(t_player, 0, player);
		}
	  kill_card(player, card, KILL_DESTROY);
	}

  return generic_spell(player, card, event, GS_CAN_ONLY_TARGET_OPPONENT, &td, "TARGET_PLAYER", 1, NULL);
}

int card_subterranean_spirit(int player, int card, event_t event){
/* CARD_ID_SUBTERRANEAN_SPIRIT	2622
Subterranean Spirit	|3|R|R
Creature - Elemental Spirit 3/3
Protection from |Sred
|T: ~ deals 1 damage to each creature without flying. */

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance(player, card);

		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		this_test.keyword = KEYWORD_FLYING;
		this_test.keyword_flag = 1;
		new_damage_all(instance->parent_controller, instance->parent_card, ANYBODY, 1, 0, &this_test);
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED, MANACOST0, 0, NULL, NULL);
}

int card_sunweb(int player, int card, event_t event){
/* CARD_ID_SUNWEB	1165
Sunweb	|3|W
Creature - Wall 5/6
Defender (This creature can't attack.)
Flying
~ can't block creatures with power 2 or less. */

	if(event == EVENT_BLOCK_LEGALITY && affect_me(player, card) && ! is_humiliated(player, card) ){
		if( get_power(attacking_card_controller, attacking_card) < 3 ){
			event_result = 1;
		}
	}

	return 0;
}

int card_superior_numbers(int player, int card, event_t event){
/*
Superior Numbers |G|G
Sorcery
Superior Numbers deals damage to target creature equal to the number of creatures you control in excess of the number of creatures
	target opponent controls.
*/
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	if( event == EVENT_RESOLVE_SPELL ){
		int amount = count_subtype(player, TYPE_CREATURE, -1) - count_subtype(1-player, TYPE_CREATURE, -1);
		if( valid_target(&td) && amount > 0 ){
			damage_target0(player, card, amount);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
}

int card_tainted_specter(int player, int card, event_t event){
	/*
	  Tainted Specter English |3|B
	  Creature - Specter 2/2
	  Flying
	  {1}{B}{B}, {T}: Target player discards a card unless he or she puts a card from his or her hand on top of his or her library.
	  If that player discards a card this way, Tainted Specter deals 1 damage to each creature and each player.
	  Activate this ability only any time you could cast a sorcery.
	*/

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			if( hand_count[instance->targets[0].player] > 0 ){
				test_definition_t this_test;
				new_default_test_definition(&this_test, TYPE_ANY, "Select a card.");

				test_definition_t this_test2;
				default_test_definition(&this_test2, TYPE_CREATURE);
				this_test2.keyword = KEYWORD_PROT_BLACK;
				this_test2.keyword_flag = DOESNT_MATCH;
				this_test2.toughness = 1;

				int ai_choice = 0;
				if( check_battlefield_for_special_card(player, card, instance->targets[0].player, CBFSC_GET_COUNT, &this_test2) > 1 ){
					ai_choice = 1;
				}
				int selected = new_select_a_card(instance->targets[0].player, instance->targets[0].player, TUTOR_FROM_HAND, 1, AI_MIN_VALUE, -1, &this_test);
				int choice = do_dialog(instance->targets[0].player, instance->targets[0].player, selected, -1, -1,
										" Discard this card\n Put it on top of deck", ai_choice);
				if( choice == 0 ){
					new_discard_card(instance->targets[0].player, selected, player, 0);
					APNAP(p, {new_damage_all(instance->parent_controller, instance->parent_card, p, 1, NDA_ALL_CREATURES | NDA_PLAYER_TOO, NULL);};);
				}
				else{
					put_on_top_of_deck(instance->targets[0].player, selected);
				}
			}
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_SORCERY_BE_PLAYED | GAA_CAN_TARGET, MANACOST_XB(1, 2), 0, &td, "TARGET_PLAYER");
}

int card_taniwha(int player, int card, event_t event){
/* CARD_ID_TANIWHA	2626
Taniwha	|3|U|U
Legendary Creature - Serpent 7/7
Trample
Phasing (This phases in or out before you untap during each of your untap steps. While it's phased out, it's treated as though it doesn't exist.)
At the beginning of your upkeep, all lands you control phase out. (They phase in before you untap during your next untap step.) */

	int ph = phasing(player, card, event);
	if (ph){
		return ph;
	}

	check_legend_rule(player, card, event);

	if( current_turn == player && upkeep_trigger(player, card, event) ){
		int count = active_cards_count[player]-1;
		while( count > -1 ){
				if( in_play(player, count) && is_what(player, count, TYPE_LAND) ){
				   phase_out(player, count);
				}
				count--;
		}
	}

	return 0;
}

int card_teekas_dragon(int player, int card, event_t event){
/* CARD_ID_TEEKAS_DRAGON	2627
Teeka's Dragon	|9
Artifact Creature - Dragon 5/5
Flying; trample; rampage 4 (Whenever this creature becomes blocked, it gets +4/+4 until end of turn for each creature blocking it beyond the first.) */

	rampage(player, card, event, 4);

	return 0;
}

// teferi's drake --> sandbar crocodile

// teferi's imp --> sandbar crocodile

int card_teferis_isle(int player, int card, event_t event)
{
  /* Teferi's Isle	""
   * Legendary Land
   * Phasing
   * ~ enters the battlefield tapped.
   * |T: Add |U|U to your mana pool. */

  int ph = phasing(player, card, event);
  if (ph)
	return ph;

  check_legend_rule(player, card, event);

  comes_into_play_tapped(player, card, event);

  if (event == EVENT_ACTIVATE)
	produce_mana_tapped(player, card, COLOR_BLUE, 2);
  else if (event == EVENT_COUNT_MANA && affect_me(player, card) && CAN_TAP_FOR_MANA(player, card))
	declare_mana_available(player, COLOR_BLUE, 2);
  else
	return mana_producer(player, card, event);

  return 0;
}

int card_telim_tor(int player, int card, event_t event)
{
/* CARD_ID_TELIMTOR	2632
Telim'Tor	|4|R
Legendary Creature - Human Knight 2/2
Flanking (Whenever a creature without flanking blocks this creature, the blocking creature gets -1/-1 until end of turn.)
Whenever ~ attacks, all attacking creatures with flanking get +1/+1 until end of turn. */

  check_legend_rule(player, card, event);

  flanking(player, card, event);

  // Whenever ~ attacks, all attacking creatures with flanking get +1/+1 until end of turn.
  if (declare_attackers_trigger(player, card, event, 0, player, card))
	{
	  test_definition_t test;
	  new_default_test_definition(&test, TYPE_CREATURE, "");
	  test.keyword = SP_KEYWORD_FLANKING;
	  test.keyword_flag = F0_HAS_SP_KEYWORD;
	  test.state = STATE_ATTACKING;
	  pump_creatures_until_eot(player, card, current_turn, 2, 1,1, 0,0, &test);
	}

  return 0;
}

int card_telim_tors_darts(int player, int card, event_t event){
	/* Telim'Tor's Darts	|2	0x2009e9f
	 * Artifact
	 * |2, |T: ~ deals 1 damage to target player or planeswalker. */
	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TARGET_TYPE_PLANESWALKER);
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			damage_target0(player, card, 1);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_X(2), 0, &td, "TARGET_PLAYER_OR_PLANESWALKER");
}

int thirst_impl(int player, int card, event_t event, int x, int b, int u, int g, int r, int w)
{
  card_instance_t* instance = get_card_instance(player, card);
  int p = instance->damage_target_player, c = instance->damage_target_card;

  if (p >= 0 && c >= 0)
	{
	  // When ~ enters the battlefield, tap enchanted creature.
	  if (comes_into_play(player, card, event))
		tap_card(p, c);

	  // Enchanted creature doesn't untap during its controller's untap step.
	  does_not_untap(p, c, event);
	}

  // At the beginning of your upkeep, sacrifice ~ unless you pay ...
  basic_upkeep(player, card, event, x, b, u, g, r, w);

  // Enchant creature
  return disabling_aura(player, card, event);
}

int card_thirst(int player, int card, event_t event)
{
/* CARD_ID_THIRST	2635
Thirst	|2|U
Enchantment - Aura
Enchant creature
When ~ enters the battlefield, tap enchanted creature.
Enchanted creature doesn't untap during its controller's untap step.
At the beginning of your upkeep, sacrifice ~ unless you pay |U. */

  return thirst_impl(player, card, event, MANACOST_U(1));
}

int card_tombstone_stairwell(int player, int card, event_t event)
{
  /* Tombstone Stairwell	|2|B|B
   * World Enchantment
   * Cumulative upkeep |1|B
   * At the beginning of each upkeep, if ~ is on the battlefield, each player puts a 2/2 |Sblack Zombie creature token with haste named Tombspawn onto the
   * battlefield for each creature card in his or her graveyard.
   * At the beginning of each end step or when ~ leaves the battlefield, destroy all tokens put onto the battlefield with ~. They can't be regenerated. */

  enchant_world(player, card, event);

  if (eot_trigger(player, card, event) || leaves_play(player, card, event))
	{
	  card_instance_t* inst;
	  int p, c;
	  for (p = 0; p <= 1; ++p)
		for (c = 0; c < active_cards_count[p]; ++c)
		  if ((inst = in_play(p, c)) && cards_data[inst->internal_card_id].id == CARD_ID_ZOMBIE
			  && get_special_infos(p, c) == 67 && inst->damage_source_player == player && inst->damage_source_card == card)
			kill_card(p, c, KILL_BURY);
	}

  upkeep_trigger_ability(player, card, event, ANYBODY);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
	{
	  if (current_turn == player)
		cumulative_upkeep(player, card, event, MANACOST_XB(1,1));

	  if (!in_play(player, card))
		return 0;

	  int p;
	  for (p = 0; p <= 1; ++p)
		{
		  int dead = count_graveyard_by_type(p, TYPE_CREATURE);
		  token_generation_t token;
		  default_token_definition(player, card, CARD_ID_ZOMBIE, &token);
		  token.t_player = p;
		  token.qty = dead;
		  token.action = TOKEN_ACTION_DONT_COPY_TOKEN_SOURCE;
		  token.s_key_plus = SP_KEYWORD_HASTE;
		  token.special_infos = 67;
		  generate_token(&token);
		}
	}

  return global_enchantment(player, card, event);
}

int card_tranquil_domain(int player, int card, event_t event)
{
  /* Tranquil Domain	|1|G
   * Instant
   * Destroy all non-Aura enchantments. */

	if (event == EVENT_RESOLVE_SPELL){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_ENCHANTMENT, "");
		test.subtype = SUBTYPE_AURA;
		test.subtype_flag = DOESNT_MATCH;
		new_manipulate_all(player, card, ANYBODY, &test, KILL_DESTROY);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_uktabi_faeries(int player, int card, event_t event){
/* CARD_ID_UKTABI_FAERIE	2641
Uktabi Faerie	|1|G
Creature - Faerie 1/1
Flying
|3|G, Sacrifice ~: Destroy target artifact. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_ARTIFACT);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
		}
	}

	return generic_activated_ability(player, card, event, GAA_SACRIFICE_ME | GAA_CAN_TARGET, MANACOST_XG(3, 1), 0, &td, "TARGET_ARTIFACT");
}

int card_uktabi_wildcats(int player, int card, event_t event){
	/* Uktabi Wildcats	|4|G
	 * Creature - Cat 100/100
	 * ~'s power and toughness are each equal to the number of |H1Forests you control.
	 * |G, Sacrifice |Ha Forest: Regenerate ~. */

	if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) && affect_me(player, card) && player != -1 && ! is_humiliated(player, card) )
		event_result += count_subtype(player, TYPE_LAND, get_hacked_subtype(player, card, SUBTYPE_FOREST));

	if (card == -1){
		return 0;
	}

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	test_definition_t test;
	new_default_test_definition(&test, TYPE_PERMANENT, "Select a Forest to sacrifice.");
	test.subtype = SUBTYPE_FOREST;

	if( event == EVENT_RESOLVE_ACTIVATION ){
		return regeneration(player, card, event, MANACOST_G(1));
	}

	return generic_activated_ability_with_sacrifice_as_cost(player, card, event, GAA_REGENERATION, MANACOST_G(1), 0, NULL, NULL,
															0, &test, 0);
}

int card_unerring_sling(int player, int card, event_t event){
/*
Unerring Sling |3
Artifact
{3}, {T}, Tap an untapped creature you control: Unerring Sling deals damage equal to the tapped creature's power to target attacking or
		blocking creature with flying.
*/
	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.required_abilities = KEYWORD_FLYING;
	td.required_state = TARGET_STATE_IN_COMBAT;

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_CREATURE);
	td1.allowed_controller = player;
	td1.preferred_controller = player;
	td1.illegal_state = TARGET_STATE_TAPPED;
	td1.illegal_abilities = 0;
    td1.special = TARGET_SPECIAL_NOT_ME;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE && can_target(&td1) ){
		return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_X(3), 0, &td, NULL);
	}

	if( event == EVENT_ACTIVATE ){
		instance->number_of_targets = 0;
		if( charge_mana_for_activated_ability(player, card, MANACOST_X(3)) ){
			int to_tap = -1;
			if( new_pick_target(&td1, "Select an untapped creature you control.", 0, 1 | GS_LITERAL_PROMPT) ){
				to_tap = instance->targets[0].player;
				instance->number_of_targets = 0;
				if( new_pick_target(&td, "Select an attacking or blocking creature.", 0, 1 | GS_LITERAL_PROMPT) ){
					tap_card(player, to_tap);
					instance->info_slot = get_power(player, to_tap);
				}
			}
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			damage_target0(player, card, instance->info_slot);
		}
	}

	return 0;
}

int card_unfulfilled_desires(int player, int card, event_t event){
/* CARD_ID_UNFULFILLED_DESIRES	2644
Unfulfilled Desires	|1|U|B
Enchantment
|1, Pay 1 life: Draw a card, then discard a card. */

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		draw_cards(player, 1);
		discard(player, 0, player);
	}

	return generic_activated_ability(player, card, event, 0, MANACOST_X(1), 1, NULL, NULL);
}

int card_vaporous_djinn(int player, int card, event_t event){
/* CARD_ID_VAPOROUS_DJINN	2648
Vaporous Djinn	|2|U|U
Creature - Djinn 3/4
Flying
At the beginning of your upkeep, ~ phases out unless you pay |U|U. (While it's phased out, it's treated as though it doesn't exist.
It phases in before you untap during your next untap step.) */

	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		int go_out = 1;
		if( has_mana(player, COLOR_BLUE, 2) ){
			int choice = do_dialog(player, player, card, -1, -1, " Pay upkeep\n Pass", 0);
			if( choice == 0 ){
				charge_mana(player, COLOR_BLUE, 2);
				if( spell_fizzled != 1 ){
					go_out = 0;
				}
			}
		}
		if( go_out == 1 ){
			phase_out(player, card);
		}
	}

	return 0;
}

int card_ventifact_bottle(int player, int card, event_t event){
/* Ventifact Bottle	|3
* Artifact
* |X|1, |T: Put X charge counters on ~. Activate this ability only any time you could cast a sorcery.
* At the beginning of your precombat main phase, if ~ has a charge counter on it, tap it and remove all charge counters from it.
* 		Add |1 to your mana pool for each charge counter removed this way. */

	if( beginning_of_phase(player, card, event, player, card, PHASE_MAIN1) ){
		if ( count_counters(player, card, COUNTER_CHARGE) ){
			produce_mana(player, COLOR_COLORLESS, count_counters(player, card, COUNTER_CHARGE));
			remove_all_counters(player, card, COUNTER_CHARGE);
			tap_card(player, card);
		}
	}

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		return generic_activated_ability(player, card, event, GAA_UNTAPPED, MANACOST_X(1), 0, NULL, NULL);
	}

	if( event == EVENT_ACTIVATE ){
		if( charge_mana_for_activated_ability(player, card, MANACOST_X(1)) ){
			charge_mana(player, COLOR_COLORLESS, -1);
			if( spell_fizzled != 1 ){
				add_state(player, card, STATE_TAPPED);
				instance->targets[0].player = x_value;
				ai_modifier += (instance->info_slot * 8);
			}
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		add_counters(instance->parent_controller, instance->parent_card, COUNTER_CHARGE, instance->targets[0].player);
	}

	return 0;
}

static const char* targets_an_enchantment_in_play(int who_chooses, int player, int card)
{
	card_instance_t *instance = get_card_instance(player, card);
	int i;
	for(i=0; i<instance->number_of_targets; i++){
		if( instance->targets[i].card != -1 && in_play(instance->targets[i].player, instance->targets[i].card) &&
			is_what(instance->targets[i].player, instance->targets[i].card, TYPE_ENCHANTMENT)
		  ){
			return NULL;
		}
	}
	return "must target an enchantment in play.";
}

int card_vigilant_martyr(int player, int card, event_t event){
/* CARD_ID_VIGILANT_MARTYR	2651
Vigilant Martyr	|W
Creature - Human Cleric 1/1
Sacrifice ~: Regenerate target creature.
|W|W, |T, Sacrifice ~: Counter target spell that targets an enchantment. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;
	td.required_state = TARGET_STATE_DESTROYED;
	if( player == AI ){
		td.special = TARGET_SPECIAL_REGENERATION;
	}

	target_definition_t td2;
	counterspell_target_definition(player, card, &td2, TYPE_ANY);
	td2.special = TARGET_SPECIAL_EXTRA_FUNCTION;
	td2.extra = (int32_t)targets_an_enchantment_in_play;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		if( generic_activated_ability(player, card, event, GAA_SACRIFICE_ME | GAA_SPELL_ON_STACK | GAA_UNTAPPED, MANACOST_W(2), 0, &td2, NULL) ){
			return 99;
		}
		if( generic_activated_ability(player, card, event, GAA_SACRIFICE_ME | GAA_CAN_TARGET, MANACOST0, 0, &td, NULL) ){
			return 99;
		}
	}

	if(event == EVENT_ACTIVATE ){
		instance->number_of_targets = 0;
		if( generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_SACRIFICE_ME | GAA_SPELL_ON_STACK | GAA_UNTAPPED, MANACOST_W(2), 0, &td2, NULL) ){
			if( charge_mana_for_activated_ability(player, card, MANACOST_W(2)) ){
				instance->targets[0].player = card_on_stack_controller;
				instance->targets[0].card = card_on_stack;
				instance->number_of_targets = 1;
				tap_card(player, card);
				instance->info_slot = 67;
				kill_card(player, card, KILL_SACRIFICE);
			}
		}
		else{
			if( new_pick_target(&td, "Select target creature to regenerate.", 0, 1 | GS_LITERAL_PROMPT) ){
				instance->info_slot = 66;
				kill_card(player, card, KILL_SACRIFICE);
			}
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( instance->info_slot == 66 && valid_target(&td) && can_be_regenerated(instance->targets[0].player, instance->targets[0].card) ){
			regenerate_target(instance->targets[0].player, instance->targets[0].card);
		}
		if( instance->info_slot == 67 && counterspell_validate(player, card, &td2, 0) ){
			real_counter_a_spell(player, card, instance->targets[0].player, instance->targets[0].card);
		}
	}

	return 0;
}

int card_village_elder(int player, int card, event_t event){
/*
Village Elder |G
Creature � Human Druid 1/1
{G}, {T}, Sacrifice a Forest: Regenerate target creature.
*/
	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}
    card_instance_t *instance = get_card_instance(player, card);

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;
	td.required_state = TARGET_STATE_DESTROYED;
	if( player == AI ){
		td.special = TARGET_SPECIAL_REGENERATION;
	}

	test_definition_t test;
	new_default_test_definition(&test, TYPE_PERMANENT, get_hacked_land_text(player, card, "Select %a card.", SUBTYPE_FOREST));
	test.subtype = get_hacked_subtype(player, card, SUBTYPE_FOREST);

	if( event == EVENT_CAN_ACTIVATE ){
		if( land_can_be_played & LCBP_REGENERATION ){
			if( generic_activated_ability_with_sacrifice_as_cost(player, card, event, GAA_CAN_TARGET | GAA_UNTAPPED, MANACOST_G(1), 0,
																&td, "TARGET_CREATURE", 0, &test, 0) )
			{
				return 99;
			}
		}
		return 0;
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		return regenerate_target( instance->targets[0].player, instance->targets[0].card );
	}

	return generic_activated_ability_with_sacrifice_as_cost(player, card, event, GAA_CAN_TARGET | GAA_UNTAPPED, MANACOST_G(1), 0,
															&td, "TARGET_CREATURE", 0, &test, 0);
}

int card_vitalizing_cascade(int player, int card, event_t event)
{
  // 0x120282f

  /* Vitalizing Cascade	|X|G|W
   * Instant
   * You gain X plus 3 life. */

	if (event == EVENT_RESOLVE_SPELL)
	{
		gain_life(player, get_card_instance(player, card)->info_slot + 3);
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_X_SPELL, NULL, NULL, 0, NULL);
}

// cat token --> rhino token

int card_waiting_in_the_weeds(int player, int card, event_t event){
	/* Waiting in the Weeds	|1|G|G
	 * Sorcery
	 * Each player puts a 1/1 |Sgreen Cat creature token onto the battlefield for each untapped |H2Forest he or she controls. */

	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t this_test;
		new_default_test_definition(&this_test, TYPE_LAND, "");
		this_test.subtype = get_hacked_subtype(player, card, SUBTYPE_FOREST);
		this_test.state = STATE_TAPPED;
		this_test.state_flag = DOESNT_MATCH;

		APNAP(p,{
					token_generation_t token;
					default_token_definition(player, card, CARD_ID_CAT, &token);
					token.qty = check_battlefield_for_special_card(player, card, p, CBFSC_GET_COUNT, &this_test);
					token.t_player = p;
					token.pow = token.tou = 1;
					token.color_forced = COLOR_TEST_GREEN;
					generate_token(&token);
				}
		);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

static const char* woc_is_blocking_him(int who_chooses, int player, int card, int targeting_player, int targeting_card)
{
  if (is_blocking(targeting_card, card))
	return NULL;
  else
	return "must be blocked by Wall of Corpses";
}

int card_wall_of_corpses(int player, int card, event_t event){
/*
Wall of Corpses |1|B
Creature — Wall 0/2
Defender (This creature can't attack.)
{B}, Sacrifice Wall of Corpses: Destroy target creature Wall of Corpses is blocking.
*/
	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_CREATURE);
	td1.allowed_controller = td1.preferred_controller = 1-player;
	td1.special = TARGET_SPECIAL_EXTRA_FUNCTION;
	td1.extra = (int32_t)woc_is_blocking_him;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td1) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
		}
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_LITERAL_PROMPT | GAA_SACRIFICE_ME, MANACOST_B(1), 0,
									&td1, "Select target creature blocked by Wall of Corpses");
}

int card_wall_of_resistance(int player, int card, event_t event){

	/* Wall of Resistance	|1|W
	 * Creature - Wall 0/3
	 * Defender
	 * Flying
	 * At the beginning of each end step, if ~ was dealt damage this turn, put a +0/+1 counter on it. */

	if( get_card_instance(player, card)->info_slot != 66 && damage_dealt_to_me_arbitrary(player, card, event, 0, player, card) ){
		get_card_instance(player, card)->info_slot = 66;
	}

	if( get_card_instance(player, card)->info_slot == 66 && eot_trigger(player, card, event) ){
		add_counter(player, card, COUNTER_P0_P1);
		get_card_instance(player, card)->info_slot = 0;
	}

	return 0;
}

int card_wall_of_roots(int player, int card, event_t event){

	/* Wall of Roots	|1|G
	 * Creature - Plant Wall 0/5
	 * Defender
	 * Put a -0/-1 counter on ~: Add |G to your mana pool. Activate this ability only once each turn. */

	if( event == EVENT_CAN_ACTIVATE ){
		return get_card_instance(player, card)->info_slot != 1 && can_produce_mana(player, card);
	}

	if(event == EVENT_ACTIVATE ){
		get_card_instance(player, card)->info_slot = 1;
		add_counter(player, card, COUNTER_M0_M1);
		produce_mana(player, COLOR_GREEN, 1);
	}

	if( event == EVENT_COUNT_MANA && affect_me(player, card) && get_card_instance(player, card)->info_slot != 1 &&
		can_produce_mana(player, card) )
	{
		declare_mana_available(player, COLOR_GREEN, 1);
	}
	if( event == EVENT_CLEANUP ){
		get_card_instance(player, card)->info_slot = 0;
	}

	return 0;
}

int card_warping_wurm(int player, int card, event_t event){
/* CARD_ID_WARPING_WURM	2658
Warping Wurm	|2|G|U
Creature - Wurm 1/1
Phasing (This phases in or out before you untap during each of your untap steps. While it's phased out, it's treated as though it doesn't exist.)
At the beginning of your upkeep, ~ phases out unless you pay |2|G|U.
When ~ phases in, put a +1/+1 counter on it. */

	int ph = phasing(player, card, event);
	if (ph){
		return ph;
	}


	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		int go_out = 1;
		if( has_mana_multi(player, MANACOST_XUG(2, 1, 1)) ){
			int choice = do_dialog(player, player, card, -1, -1, " Pay upkeep\n Pass", 0);
			if( choice == 0 ){
				charge_mana_multi(player, MANACOST_XUG(2, 1, 1));
				if( spell_fizzled != 1 ){
					go_out = 0;
				}
			}
		}
		if( go_out == 1 ){
			phase_out(player, card);
		}
	}

	return 0;
}

int card_wildfire_emissary(int player, int card, event_t event)
{
  // 0x120240e

  /* Wildfire Emissary	|3|R
   * Creature - Efreet 2/4
   * Protection from |Swhite
   * |1|R: ~ gets +1/+0 until end of turn. */
  return generic_shade_merge_pt(player, card, event, 0, MANACOST_XR(1,1), 1,0);
}

int card_withering_boon(int player, int card, event_t event){
/* CARD_ID_WITHERING_BOON	2662
Withering Boon	|1|B
Instant
As an additional cost to cast ~, pay 3 life.
Counter target creature spell. */

	if( event == EVENT_CAN_CAST){
		int result = card_remove_soul(player, card, event);
		if( result ){
			if( can_pay_life_as_cost_for_spells_or_activated_abilities(player, 3) ){
				return result;
			}
		}
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		card_remove_soul(player, card, event);
		if( spell_fizzled != 1 ){
			lose_life(player, 3);
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		 return card_remove_soul(player, card, event);
	}

	return 0;
}

int card_yare(int player, int card, event_t event)
{
/* CARD_ID_YARE	2663
Yare	|2|W
Instant
Target creature defending player controls gets +3/+0 until end of turn. That creature can block up to two additional creatures this turn. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	card_instance_t* instance = get_card_instance(player, card);

  target_definition_t td;
  default_target_definition(player, card, &td, TYPE_CREATURE);
  td.allowed_controller = 1 - current_turn;
  td.preferred_controller = player;

  if (event == EVENT_CAN_CAST && current_phase > PHASE_MAIN1 && current_phase < PHASE_MAIN2)
	return can_target(&td);	// Unclear if "defending player controls" means this must be cast during combat.  I'm going to ignore it.

  if (event == EVENT_CAST_SPELL && affect_me(player, card))
	{
	  if (pick_target(&td, "TARGET_CREATURE")
		  && player == AI && instance->targets[0].player != AI)
		ai_modifier -= 48;
	}

  if (event == EVENT_RESOLVE_SPELL && valid_target(&td))
	{
	  int legacy = pump_ability_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, 3, 0, 0, 0);
	  get_card_instance(player, legacy)->targets[5].player = 2;

	  kill_card(player, card, KILL_DESTROY);
	}

  return 0;
}

int card_zhalfirin_commander(int player, int card, event_t event){
/* CARD_ID_ZHALFIRIN_COMMANDER	2665
Zhalfirin Commander	|2|W
Creature - Human Knight 2/2
Flanking (Whenever a creature without flanking blocks this creature, the blocking creature gets -1/-1 until end of turn.)
|1|W|W: Target Knight creature gets +1/+1 until end of turn. */

	flanking(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_CREATURE);
	td1.allowed_controller = player;
	td1.preferred_controller = player;
	td1.required_subtype = SUBTYPE_KNIGHT;
	td1.illegal_abilities = 0;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td1) ){
			pump_until_eot(instance->parent_controller, instance->parent_card, instance->targets[0].player, instance->targets[0].card, 1, 1);
		}
	}
	return generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_LITERAL_PROMPT, MANACOST_XW(1, 2), 0,
									&td1, "Select target Knight creature.");
}

int card_zhalfirin_knight(int player, int card, event_t event){
/* CARD_ID_ZHALFIRIN_KNIGHT	2666
Zhalfirin Knight	|2|W
Creature - Human Knight 2/2
Flanking (Whenever a creature without flanking blocks this creature, the blocking creature gets -1/-1 until end of turn.)
|W|W: ~ gains first strike until end of turn. */

	return generic_mirage_knight(player, card, event, COLOR_WHITE);
}

int card_zirilan_of_the_claw(int player, int card, event_t event){
/* CARD_ID_ZIRILAN_OF_THE_CLAW	2667
Zirilan of the Claw	|3|R|R
Legendary Creature - Viashino Shaman 3/4
|1|R|R, |T: Search your library for a Dragon permanent card and put that card onto the battlefield. Then shuffle your library.
That Dragon gains haste until end of turn. Exile it at the beginning of the next end step. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	test_definition_t this_test;
	new_default_test_definition(&this_test, TYPE_CREATURE, "Select a Dragon creature.");
	this_test.subtype = SUBTYPE_DRAGON;

	card_instance_t *instance = get_card_instance( player, card );

	check_legend_rule(player, card, event);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		int result = new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_PLAY, 0, AI_MAX_CMC, &this_test);
		card_instance_t *parent = get_card_instance(instance->parent_controller, instance->parent_card);
		parent->targets[2].card = 2+result;
		if( result > -1 ){
			create_targetted_legacy_effect(player, card, &haste_and_remove_eot, player, result);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_DECK_SEARCHER, MANACOST_XR(1, 2), 0, NULL, NULL);
}

int card_zuberi_golden_feather(int player, int card, event_t event){
/* CARD_ID_ZUBERI_GOLDEN_FEATHER	1677
Zuberi, Golden Feather	|4|W
Legendary Creature - Griffin 3/3
Flying
Other Griffin creatures get +1/+1. */

	check_legend_rule(player, card, event);
	return boost_creature_type(player, card, event, SUBTYPE_GRIFFIN, 1, 1, 0, 0);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static char *target_must_be_damage_targeting_this_ethereal_champion(int who_chooses, int player,
                                                                    int card, int targeting_player,
                                                                    int targeting_card);

/* ManalinkEh 2022: 0x237fef0; original cards/mirage.c:3002. */
int card_jolt(int player, int card, event_t event)
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
        td.illegal_abilities = 0;
        if (event != EVENT_RESOLVE_SPELL)
        {
          result = generic_spell(player, card, event, 1, &td, "TWIDDLE", 1, NULL);
          return result;
        }
        result = valid_target(&td);
        if (result != 0)
        {
          twiddle(player, card, 0);
          cantrip(player, card, 1);
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
  td.illegal_abilities = 0;
  result = generic_spell(player, card, event, 1, &td, "TWIDDLE", 1, NULL);
  if ((((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
       (affected_card_controller == player)) &&
      ((player == 1 && (spell_fizzled != 1))))
  {
    ai_modifier_twiddle(1, card, 0);
  }
  return result;
}

/* ManalinkEh 2022: 0x23868d0; original cards/mirage.c:5182. */
int card_telimtors_edict(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 0x47);
        td.allowed_controller = player | 4;
        td.illegal_abilities = 0;
        td.preferred_controller = td.allowed_controller;
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            instance = get_card_instance(player, card);
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
            cantrip(player, card, 1);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02386937;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x47);
  td.allowed_controller = player | 4;
  td.illegal_abilities = 0;
  td.preferred_controller = td.allowed_controller;
LAB_02386937:
  result = generic_spell(player, card, event, 0x81, &td,
                         "Select target permanent you own or control.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2388f70; original cards/mirage.c:5976. */
int card_agility(int player, int card, event_t event)
{
  int result;
  result = generic_aura_2022(player, card, event, player, 1, 1, 0, 0x2000, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x2388fd0; original cards/mirage.c:5986. */
int card_alarum(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  if (event == EVENT_CHECK_PUMP)
  {
    if ((current_turn == player) && (0x14 < current_phase))
    {
      return 0;
    }
    result = vanilla_instant_pump_2022(player, card, EVENT_CHECK_PUMP, 2, player, 1, 3, 0, 0);
    return result;
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
        default_target_definition(player, card, &td, 2);
        td.preferred_controller = player;
        td.illegal_state = 2;
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            instance = get_card_instance(player, card);
            untap_card(instance->targets[0].player, instance->targets[0].card);
            pump_until_eot_2022(player, card, instance->targets[0].player, instance->targets[0].card, 1,
                           3);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02389094;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.preferred_controller = player;
  td.illegal_state = 2;
LAB_02389094:
  result = generic_spell(player, card, event, 0x81, &td, "Select target non-attacking creature.", 1,
                         NULL);
  return result;
}

/* ManalinkEh 2022: 0x23891c0; original cards/mirage.c:6018. */
int card_ancestral_memories(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  int iVar3;
  int iStack_ec;
  test_definition_t this_test;
  if (event == EVENT_RESOLVE_SPELL)
  {
    iVar2 = count_deck(player);
    if (iVar2 != 0)
    {
      if (7 < iVar2)
      {
        iVar2 = 7;
      }
      iStack_ec = iVar2;
      if (2 < iVar2)
      {
        iStack_ec = 2;
      }
      iVar1 = iStack_ec;
      do
      {
        iStack_ec = iVar1;
        iVar3 = iVar2;
        new_default_test_definition(&this_test, 0xff, "Select a card to add to your hand");
        this_test.create_minideck = (unsigned short)iVar3;
        this_test.no_shuffle = 1;
        new_global_tutor(player, player, 1, 1, 1, 1, &this_test);
        if (iStack_ec + -1 == 0)
          break;
        iVar2 = iVar3 + -1;
        iVar1 = iStack_ec + -1;
      } while (iVar3 + -1 != 0);
      if (0 < iVar3 - iStack_ec)
      {
        mill(player, iVar3 - iStack_ec);
      }
    }
    kill_card(player, card, KILL_DESTROY);
  }
  iVar2 = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return iVar2;
}

/* ManalinkEh 2022: 0x2389640; original cards/mirage.c:6103. */
int card_azimaet_drake(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_POW_BOOST)
  {
    result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x2000, 0, 0, 1, 0, 0, 0,
                                       0, NULL, NULL);
    if (result != 0)
    {
      return 1;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    result = generic_shade(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0);
    return result;
  }
  result = generic_activated_ability(player, card, event, 0x2000, 0, 0, 1, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x238a540; original cards/mirage.c:6373. */
int card_blinding_light(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (event == EVENT_RESOLVE_SPELL)
  {
    default_test_definition(&this_test, 2);
    this_test.color = get_sleighted_color_test(player, card, 0x20);
    this_test.color_flag = 1;
    new_manipulate_all(player, card, 2, &this_test, ACT_TAP);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x238c500; original cards/mirage.c:6952. */
int card_dirtwater_wraith(int player, int card, event_t event)
{
  int result;
  if (1 < event - EVENT_POW_BOOST)
  {
    result = generic_shade_merge_pt(player, card, event, 0, 0, 1, 0, 0, 0, 0, 1, 0);
    return result;
  }
  result = generic_shade(player, card, event, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x238c5f0; original cards/mirage.c:6966. */
int card_disempower(int player, int card, event_t event)
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
            put_on_top_of_deck(instance->targets[0].player, instance->targets[0].card);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0238c656;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x44);
  get_card_instance(player, card);
LAB_0238c656:
  result = generic_spell(
      player, card, event, 0x81, &td,
      "Select target artifact or enchantment to put on top of its owner\'s library.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23760a0; original cards/mirage.c:7223. */
static char *target_must_be_damage_targeting_this_ethereal_champion(int who_chooses, int player,
                                                                    int card, int targeting_player,
                                                                    int targeting_card)
{
  card_instance_t *instance;
  char *pcVar2;

  instance = get_card_instance(player, card);
  pcVar2 = "must be damaging Ethereal Champion";
  if (((instance->internal_card_id == damage_card) &&
       (instance->damage_target_player == targeting_player)) &&
      (instance->damage_target_card == targeting_card))
  {
    pcVar2 = NULL;
  }
  return pcVar2;
}

/* ManalinkEh 2022: 0x238d340; original cards/mirage.c:7234. */
int card_ethereal_champion(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0);
    td.special = 0x8000;
    td.extra = (int32_t)target_must_be_damage_targeting_this_ethereal_champion;
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x1000004, 0, 0, 0, 0, 0,
                                         0, 1, &td, NULL);
      if (result == 0)
      {
        return 0;
      }
      return 99;
    }
    if (event == EVENT_ACTIVATE)
    {
      generic_activated_ability(player, card, EVENT_ACTIVATE, 0x1000006, 0, 0, 0, 0, 0, 0, 1, &td,
                                "Select damage made to Ethereal Champion.");
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0);
  }
  td.special = 0x8000;
  td.extra = (int32_t)target_must_be_damage_targeting_this_ethereal_champion;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    result = valid_target(&td);
    if (result != 0)
    {
      prevent_damage(instance->targets[0].player, instance->targets[0].card, 1);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x238d510; original cards/mirage.c:7269. */
int card_fallow_earth(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  default_target_definition(player, card, &td, 1);
  instance = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = valid_target(&td);
    if (result != 0)
    {
      put_on_top_of_deck(instance->targets[0].player, instance->targets[0].card);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0x81, &td,
                         "Select target land, put it on top of its owner\'s library.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x238d800; original cards/mirage.c:7322. */
int card_flame_elemental(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  default_target_definition(player, card, &td, 2);
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    result = valid_target(&td);
    if (result != 0)
    {
      result = get_power(player, card);
      damage_creature(instance->targets[0].player, instance->targets[0].card, result, player, card);
    }
  }
  result = generic_activated_ability(player, card, event, 0xd, 0, 0, 0, 0, 1, 0, 0, &td,
                                     "TARGET_CREATURE");
  return result;
}

/* ManalinkEh 2022: 0x238df40; original cards/mirage.c:7500. */
int card_horrible_hordes(int player, int card, event_t event)
{
  rampage(player, card, event, 1);
  return 0;
}

/* ManalinkEh 2022: 0x238df70; original cards/mirage.c:7510. */
int card_igneous_golem(int player, int card, event_t event)
{
  int result;
  result = generic_shade(player, card, event, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0x80, 0);
  return result;
}

/* ManalinkEh 2022: 0x238e5f0; original cards/mirage.c:7610. */
int card_kaerveks_hex(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  test_definition_t this_test2;
  if (event == EVENT_RESOLVE_SPELL)
  {
    default_test_definition(&this_test, 2);
    this_test.color = get_sleighted_color_test(player, card, 2);
    this_test.color_flag = 1;
    default_test_definition(&this_test2, 2);
    this_test2.color = get_sleighted_color_test(player, card, 8);
    new_damage_all(player, card, 2, 1, 0, &this_test2);
    new_damage_all(player, card, 2, 1, 0, &this_test);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2377350; original cards/mirage.c:7686. */
static int kukemssa_serpent_effect(int player, int card, event_t event)
{
  int basic_land_type_to_become;
  card_instance_t *instance;
  if (event == EVENT_CHANGE_TYPE)
  {
    instance = get_card_instance(player, card);
    if ((instance->damage_target_card == affected_card) &&
        (instance->damage_target_player == affected_card_controller))
    {
      basic_land_type_to_become = get_hacked_subtype(player, card, 0x5002);
      event_result = land_into_new_basic_land_type(event_result, basic_land_type_to_become);
    }
  }
  else if (event == EVENT_CLEANUP)
  {
    instance = get_card_instance(player, card);
    if (-1 < instance->targets[0].player)
    {
      kill_card(player, card, KILL_EXILE);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x238f0a0; original cards/mirage.c:7836. */
int card_mangaras_blessing(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    gain_life_2022(player, 5);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x238f100; original cards/mirage.c:7866. */
int card_merfolk_seer(int player, int card, event_t event)
{
  int result;
  result = duh_mode(player);
  result = this_dies_trigger(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (result != 0)
  {
    result = has_mana_multi(player, 1, 1, 0, 0, 0, 0);
    if (result != 0)
    {
      result = charge_mana_multi_while_resolving(player, card, EVENT_RESOLVE_TRIGGER, player, 1, 0,
                                                 1, 0, 0, 0);
      if (result != 0)
      {
        draw_a_card(player);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x238f6b0; original cards/mirage.c:7943. */
int card_nocturnal_raid(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = get_sleighted_color_test(player, card, 2);
    pump_color_until_eot_2022(player, card, 2, result, 2, 2, 0, 0);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x238f750; original cards/mirage.c:7957. */
int card_painful_memories(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  ec_definition_t ec;
  target_definition_t td;
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
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        default_target_definition(player, card, &td, 0);
        td.allowed_controller = 1 - player;
        td.zone = 0x1000;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            default_ec_definition(instance->targets[0].player, player, &ec);
            ec.effect = 0x20;
            default_test_definition(&this_test, 0xff);
            new_effect_coercion(&ec, &this_test);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0238f7da;
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
  get_card_instance(player, card);
LAB_0238f7da:
  result = generic_spell(player, card, event, 0x81, &td,
                         "Select target opponent, put a card from his hand on top of his library.",
                         1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2376ef0; original cards/mirage.c:8025. */
static int prismatic_lace_legacy(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  instance = get_card_instance(player, card);
  if (-1 < instance->damage_target_player)
  {
    instance = get_card_instance(player, card);
    result = instance->targets[1].card;
    if ((result < 0) || (event != EVENT_SET_COLOR))
    {
      if (event == EVENT_CLEANUP)
      {
        instance->targets[2].player = 0;
        return 0;
      }
    }
    else if ((instance->damage_target_card == affected_card) &&
             (instance->damage_target_player == affected_card_controller))
    {
      event_result = result;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2390090; original cards/mirage.c:8151. */
int card_raging_spirit(int player, int card, event_t event)
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
      change_color(instance->parent_controller, instance->parent_card, instance->parent_controller,
                   instance->parent_card, COLOR_TEST_COLORLESS,
                   CHANGE_COLOR_END_AT_EOT | CHANGE_COLOR_SET);
    }
  }
  result = generic_activated_ability(player, card, event, 0, 2, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2390340; original cards/mirage.c:8212. */
int card_reckless_embermage(int player, int card, event_t event)
{
  unsigned short uVar1;
  card_instance_t *instance;
  unsigned int uVar3;
  int iVar4;
  int iVar5;
  target_definition_t td;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 1, 0, 0, 0, 1, 0, 0,
                                        &td, NULL);
      return iVar4;
    }
    if (event == EVENT_ACTIVATE)
    {
      generic_activated_ability(player, card, EVENT_ACTIVATE, 4, 1, 0, 0, 0, 1, 0, 0, &td,
                                "TARGET_ANY");
      if (spell_fizzled == 1)
      {
        return 0;
      }
      if (((instance->targets[0].player == 1 - player) && (instance->targets[0].card == -1)) &&
          (life[1 - player] < 4))
      {
        ai_modifier = ai_modifier + 0x18;
      }
      iVar4 = get_toughness(player, card);
      uVar1 = instance->damage_on_card;
      iVar5 = 1;
      uVar3 = get_color(player, card);
      do
      {
        if (((1 << ((unsigned char)iVar5 & 0x1f) & uVar3) != 0) &&
            ((instance->regen_status & 1 << ((unsigned char)iVar5 + 10 & 0x1f)) != 0))
          goto LAB_023905cd;
        iVar5 = iVar5 + 1;
      } while (iVar5 != 6);
      if ((int)(iVar4 - (unsigned int)uVar1) < 2)
      {
        ai_modifier = ai_modifier + -0x30;
        return 0;
      }
    LAB_023905cd:
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
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    instance = get_card_instance(player, card);
  }
  if ((event == EVENT_RESOLVE_ACTIVATION) && (iVar4 = valid_target(&td), iVar4 != 0))
  {
    damage_target0(player, card, 1);
    damage_creature(instance->parent_controller, instance->parent_card, 1,
                    instance->parent_controller, instance->parent_card);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x23908b0; original cards/mirage.c:8327. */
int card_ritual_of_steel(int player, int card, event_t event)
{
  int result;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    cantrip(player, card, 1);
  }
  result = generic_aura_2022(player, card, event, player, 0, 2, 0, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x2391c00; original cards/mirage.c:8701. */
int card_searing_spear_askari(int player, int card, event_t event)
{
  int result;
  flanking(player, card, event);
  result = generic_shade(player, card, event, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0x10000);
  return result;
}

/* ManalinkEh 2022: 0x2392400; original cards/mirage.c:8828. */
int card_shadowbane(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  unsigned int uVar4;
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
        td.extra = damage_card;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            result = prevent_damage(instance->targets[0].player, instance->targets[0].card, -1);
            instance = get_card_instance(instance->targets[0].player, instance->targets[0].card);
            if (instance->internal_card_id == damage_card)
            {
              uVar3 = get_color((int)instance->damage_source_player, instance->damage_source_card);
              uVar4 = get_sleighted_color_test(player, card, 2);
              if ((uVar3 & uVar4) != 0)
              {
                gain_life_2022(player, result);
              }
            }
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0239246e;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0);
  td.extra = damage_card;
  get_card_instance(player, card);
LAB_0239246e:
  result = generic_spell(player, card, event, 0x101, &td, "TARGET_DAMAGE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2392e40; original cards/mirage.c:9083. */
int card_teferis_curse(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_02392e70;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_02392e70;
    if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         (((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)) &&
           (event != EVENT_CHANGE_TYPE)))) &&
        (((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)) &&
          (event != EVENT_STATIC_EFFECTS))))
    {
      if (event == EVENT_PHASING)
      {
        instance = get_card_instance(player, card);
        if (current_turn == instance->damage_target_player)
        {
          result =
              phasing((int)instance->damage_target_player, instance->damage_target_card, player);
          return result;
        }
      }
      return 0;
    }
  }
LAB_02392e70:
  default_target_definition(player, card, &td, 0x42);
  td.preferred_controller = 1 - player;
  result =
      disabling_targeted_aura(player, card, event, &td, "Select target artifact or creature.", 0);
  return result;
}

/* ManalinkEh 2022: 0x2393360; original cards/mirage.c:9237. */
int card_unyaro_griffin(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  if (event == EVENT_ACTIVATE)
  {
    counterspell_target_definition(player, card, &td, 0x38);
    td.required_color = get_sleighted_color_test(player, card, 0x10);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    counterspell_target_definition(player, card, &td, 0x38);
    td.required_color = get_sleighted_color_test(player, card, 0x10);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      instance = get_card_instance(player, card);
      real_counter_a_spell(instance->parent_controller, instance->parent_card,
                           instance->targets[0].player, instance->targets[0].card);
    }
  }
  result = generic_activated_ability(player, card, event, 0x408, 0, 0, 0, 0, 0, 0, 0, &td, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2376a80; original cards/mirage.c:9260. */
static char *is_blocking_urborg_panther(int who_chooses, int player, int card, int targeting_player,
                                        int targeting_card)
{
  int result;
  result = is_blocking(card, targeting_card);
  if (result == 0)
  {
    return "must be blocking Urborg Panther";
  }
  return NULL;
}

/* ManalinkEh 2022: 0x2377400; original cards/mirage.c:9268. */
static int check_for_feral_shadow_and_breathstealer(int player)
{
  int result;
  result = check_battlefield_for_id(player, 0x9ad);
  if (result == 0)
  {
    return 0;
  }
  result = check_battlefield_for_id(player, 0x971);
  return (unsigned int)(result != 0);
}

/* ManalinkEh 2022: 0x2393de0; original cards/mirage.c:9435. */
int card_wave_elemental(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_t td;

  default_target_definition(player, card, &td, 2);
  td.illegal_abilities = td.illegal_abilities | 0x20;
  td.allow_cancel = 3;
  instance = get_card_instance(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0xd, 0, 0, 1, 0, 0, 0, 0,
                                      &td, "TARGET_CREATURE");
    return iVar3;
  }
  if (event == EVENT_ACTIVATE)
  {
    instance->number_of_targets = '\0';
    iVar3 = can_sacrifice_this_as_cost(player, card);
    if ((iVar3 != 0) && (iVar3 = generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 0, 0, 1,
                                                           0, 0, 0, 0, NULL, NULL),
                         iVar3 != 0))
    {
      iVar3 = pick_up_to_n_targets_noload(&td, "Select target creature without flying to tap.", 3);
      if (iVar3 != 0)
      {
        kill_card(player, card, KILL_SACRIFICE);
        return 1;
      }
      remove_state(player, card, 0x10);
    }
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) && (instance->number_of_targets != '\0'))
  {
    iVar3 = 0;
    do
    {
      iVar2 = validate_target(player, card, &td, iVar3);
      if (iVar2 != 0)
      {
        tap_card(instance->targets[iVar3].player, instance->targets[iVar3].card);
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)(unsigned int)instance->number_of_targets);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2394010; original cards/mirage.c:9479. */
int card_wellspring(int player, int card, event_t event)
{
  char cVar1;
  int result;
  card_instance_t *instance;
  target_definition_t td;
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    instance = get_card_instance(player, card);
    gain_control_until_eot(player, card, (int)instance->damage_target_player,
                           instance->damage_target_card);
    upkeep_trigger_ability(player, card, EVENT_ETB, player);
  LAB_0239411f:
    if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
        (event == EVENT_CHANGE_TARGET))
      goto LAB_02394090;
  }
  else
  {
    upkeep_trigger_ability(player, card, event, player);
    if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    {
      instance = get_card_instance(player, card);
      cVar1 = instance->damage_target_player;
      result = instance->damage_target_card;
      untap_card((int)cVar1, result);
      gain_control_until_eot(player, card, (int)cVar1, result);
    }
    else
    {
      if (event == EVENT_CAN_CAST)
        goto LAB_02394090;
      if (event != EVENT_CAST_SPELL)
        goto LAB_0239411f;
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_02394090;
    }
  }
  if (((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
        ((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)))) &&
       (event != EVENT_CHANGE_TYPE)) &&
      (((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)) &&
        (event != EVENT_STATIC_EFFECTS))))
  {
    return 0;
  }
LAB_02394090:
  default_target_definition(player, card, &td, 1);
  td.preferred_controller = 1 - player;
  result = targeted_aura_custom_prompt(player, card, event, &td, "Select target land.");
  return result;
}

/* ManalinkEh 2022: 0x23941e0; original cards/mirage.c:9519. */
int card_zombie_mob(int player, int card, event_t event)
{
  int *piVar1;
  int iVar2;
  int iVar3;
  iVar2 = count_graveyard_by_type(player, 2);
  enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1, iVar2);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    piVar1 = graveyard_ptr_mutable[player];
    iVar2 = count_graveyard(player);
    iVar2 = iVar2 + -1;
    if (-1 < iVar2)
    {
      do
      {
        iVar3 = is_what(-1, piVar1[iVar2], 2);
        if (iVar3 != 0)
        {
          rfg_card_from_grave(player, iVar2);
        }
        iVar2 = iVar2 + -1;
      } while (iVar2 != -1);
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int effect_blind_fury(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2376b90; original cards/mirage.c:6343. */
static int effect_blind_fury(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = combat_damage_being_dealt(event);
  if ((instance != NULL) && (-1 < instance->damage_source_card))
  {
    result = is_what((int)instance->damage_source_player, instance->damage_source_card, 2);
    if (result != 0)
    {
      result = is_what((int)instance->damage_target_player, instance->damage_target_card, 2);
      if (result != 0)
      {
        instance->info_slot = instance->info_slot * 2;
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

/* ManalinkEh 2022: 0x238a460; original cards/mirage.c:6358. */
int card_blind_fury(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    result = negate_ability_until_eot(player, card, 2, -1, KEYWORD_TRAMPLE);
    alternate_legacy_text(1, player, result);
    result = create_legacy_effect(player, card, effect_blind_fury);
    alternate_legacy_text(2, player, result);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x238e9a0; original cards/mirage.c:7705. */
int card_kukemssa_serpent(int player, int card, event_t event)
{
  subtype_t subtype;
  const char *pcVar1;
  int result;
  card_instance_t *pcVar3;
  card_instance_t *pcVar4;
  target_definition_t td;
  test_definition_t test;

  subtype = get_hacked_subtype(player, card, 0x5002);
  landhome(player, card, event, subtype);
  if (((event != EVENT_ACTIVATE) && (1 < event - EVENT_RESOLVE_ACTIVATION)) &&
      (event != EVENT_CLEANUP))
  {
    return 0;
  }
  pcVar1 = get_hacked_land_text(player, card, "Select an %s to sacrifice.", 0x5002);
  new_default_test_definition(&test, 0x47, pcVar1);
  test.subtype = get_hacked_subtype(player, card, 0x5002);
  default_target_definition(player, card, &td, 1);
  if (ai_is_speculating == 1)
  {
    if (player != 1)
      goto LAB_0238ea81;
  }
  else if ((player != 1) || (((unsigned char)trace_mode & 2) != 0))
    goto LAB_0238ea81;
  td.required_subtype = get_hacked_subtype(1, card, 0x5002);
  td.special = 0x1000;
LAB_0238ea81:
  if (event == EVENT_CAN_ACTIVATE)
  {
    pcVar1 = get_hacked_land_text(player, card, "Select target land it becomes %s.", 0x5002);
    result = generic_activated_ability_with_sacrifice_as_cost(
        player, card, EVENT_CAN_ACTIVATE, GAA_CAN_TARGET | GAA_LITERAL_PROMPT, 0, 0, 1, 0, 0, 0, 0,
        &td, pcVar1, TYPE_NONE, &test, SARV_0);
    return result;
  }
  if (event != EVENT_ACTIVATE)
  {
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      pcVar3 = get_card_instance(player, card);
      result = create_targetted_legacy_effect(pcVar3->parent_controller, pcVar3->parent_card,
                                              kukemssa_serpent_effect, pcVar3->targets[0].player,
                                              pcVar3->targets[0].card);
      pcVar4 = get_card_instance(player, result);
      pcVar4->targets[0].player = pcVar3->parent_controller;
      pcVar4->targets[0].card = pcVar3->parent_card;
      pcVar4->number_of_targets = '\x01';
    }
    return 0;
  }
  pcVar1 = get_hacked_land_text(player, card, "Select target land it becomes %s.", 0x5002);
  result = generic_activated_ability_with_sacrifice_as_cost(
      player, card, EVENT_ACTIVATE, GAA_CAN_TARGET | GAA_LITERAL_PROMPT, 0, 0, 1, 0, 0, 0, 0, &td,
      pcVar1, TYPE_NONE, &test, SARV_0);
  return result;
}

/* ManalinkEh 2022: 0x238ef20; original cards/mirage.c:7796. */
int card_lightning_reflexes(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  bool bVar3;

  if (event == EVENT_MODIFY_COST)
  {
    result = can_sorcery_be_played(player, EVENT_MODIFY_COST);
    if (result == 0)
    {
      infinite_casting_cost();
    }
  }
  else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    result = can_sorcery_be_played(player, EVENT_CAST_SPELL);
    if (result == 0)
    {
      instance = get_card_instance(player, card);
      result = current_turn;
      bVar3 = current_phase == 0x1f;
      instance->targets[3].player = 0x8d;
      instance->targets[3].card = result;
      if (bVar3)
      {
        instance->targets[3].card = 1 - result;
      }
      ai_modifier = ai_modifier + -0x10;
    }
  }
  instance = get_card_instance(player, card);
  if (instance->targets[3].player == 0x8d)
  {
    instance = get_card_instance(player, card);
    if ((instance->targets[3].card == current_turn) && (current_phase == 0x22))
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
  }
  result = generic_aura_2022(player, card, event, player, 1, 0, 0x100, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x23925d0; original cards/mirage.c:8863. */
int card_shaukus_minion(int player, int card, event_t event)
{
  const char *prompt;
  int result;
  target_definition_t td;

  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 2);
    td.required_color = get_sleighted_color_test(player, card, 0x20);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.required_color = get_sleighted_color_test(player, card, 0x20);
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      damage_target0(player, card, 2);
    }
  }
  prompt = get_sleighted_color_text(player, card, "Select target %s creature.", 5);
  result = generic_activated_ability(player, card, event, 7, 0, 1, 0, 0, 1, 0, 0, &td, prompt);
  return result;
}

/* ManalinkEh 2022: 0x2392730; original cards/mirage.c:8886. */
int card_sirocco(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  const char *msg;
  int iStack_15c;
  ec_definition_t this_definition;
  target_definition_t td;
  test_definition_t this_test;

  default_target_definition(player, card, &td, 2);
  td.zone = 0x1000;
  td.preferred_controller = player;
  if (event == EVENT_RESOLVE_SPELL)
  {
    instance = get_card_instance(player, card);
    result = valid_target(&td);
    if (result != 0)
    {
      msg = get_sleighted_color_text(player, card, "Select a card to discard.", 2);
      new_default_test_definition(&this_test, 0x30, msg);
      this_test.color = get_sleighted_color_test(player, card, 4);
      result = instance->targets[0].player;
      this_test.zone = 0x100;
      default_ec_definition(result, result, &this_definition);
      this_definition.effect = 8;
      iStack_15c = new_effect_coercion(&this_definition, &this_test);
      if (0 < iStack_15c)
      {
        do
        {
          result = can_pay_life(instance->targets[0].player, 4);
          if (result == 0)
          {
            new_discard_card(instance->targets[0].player, 0, player, 0);
          }
          else
          {
            lose_life_2022(player, 4);
          }
          iStack_15c = iStack_15c + -1;
        } while (iStack_15c != 0);
      }
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 1, &td, "TARGET_PLAYER", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2392900; original cards/mirage.c:8948. */
int card_soar(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  bool bVar3;

  if (event == EVENT_MODIFY_COST)
  {
    result = can_sorcery_be_played(player, EVENT_MODIFY_COST);
    if (result == 0)
    {
      infinite_casting_cost();
    }
  }
  else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    result = can_sorcery_be_played(player, EVENT_CAST_SPELL);
    if (result == 0)
    {
      instance = get_card_instance(player, card);
      result = current_turn;
      bVar3 = current_phase == 0x1f;
      instance->targets[3].player = 0x8d;
      instance->targets[3].card = result;
      if (bVar3)
      {
        instance->targets[3].card = 1 - result;
      }
      ai_modifier = ai_modifier + -0x10;
    }
  }
  instance = get_card_instance(player, card);
  if (instance->targets[3].player == 0x8d)
  {
    instance = get_card_instance(player, card);
    if ((instance->targets[3].card == current_turn) && (current_phase == 0x22))
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
  }
  result = generic_aura_2022(player, card, event, player, 0, 1, 0x20, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x2392c80; original cards/mirage.c:9048. */
int card_soul_rend(int player, int card, event_t event)
{
  card_instance_t *instance;
  const char *prompt;
  int result;
  unsigned int uVar3;
  unsigned int uVar4;
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
            uVar3 = get_color(instance->targets[0].player, instance->targets[0].card);
            uVar4 = get_sleighted_color_test(player, card, 0x20);
            if ((uVar3 & uVar4) != 0)
            {
              kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);
            }
            cantrip(player, card, 1);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02392cea;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  get_card_instance(player, card);
LAB_02392cea:
  prompt = get_sleighted_color_text(player, card,
                                    "Select target creature if it is a %s creature destroy it.", 5);
  result = generic_spell(player, card, event, 0x81, &td, prompt, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2393020; original cards/mirage.c:9180. */
int card_tropical_storm(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t this_test;
  test_definition_t this_test2;

  if (event == EVENT_RESOLVE_SPELL)
  {
    default_test_definition(&this_test, 2);
    this_test.keyword = 0x20;
    instance = get_card_instance(player, card);
    new_damage_all(player, card, 2, instance->info_slot, 0, &this_test);
    default_test_definition(&this_test2, 2);
    this_test2.color = get_sleighted_color_test(player, card, 4);
    new_damage_all(player, card, 2, 1, 0, &this_test2);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 4, NULL, NULL, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2393150; original cards/mirage.c:9202. */
int card_unseen_walker(int player, int card, event_t event)
{
  card_instance_t *instance;
  const char *prompt;
  int result;
  target_definition_t td;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {
      if (((current_turn == player) && (current_phase < 0x17)) &&
          (result = can_target(&td), result != 0))
      {
        result = get_hacked_subtype(player, card, 0x5001);
        result = count_subtype(1 - player, 0x47, result);
        ai_modifier = (-(unsigned int)(result == 0) & 0xffffffa0) + 0x30;
      }
      goto LAB_023931be;
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
    result = get_hacked_walk(player, card, 4);
    pump_ability_until_eot_2022(instance->parent_controller, instance->parent_card,
                           instance->targets[0].player, instance->targets[0].card, 0, 0, result, 0);
  }
LAB_023931be:
  prompt = get_hacked_land_text(player, card, "Select target creature it gains %swalk.", 0x5001);
  result = generic_activated_ability(player, card, event, 6, 1, 0, 0, 2, 0, 0, 0, &td, prompt);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int fx_barreling_attack(int player, int card, event_t event);

/* ManalinkEh 2022: 0x237aca0; original cards/mirage.c:1307. */
int helper_for_carrion_like_effects(int p, int c, int mode, int modifier_for_instant_sorceries)
{
  card_instance_t *instance;
  int iVar2;
  int card;
  int iVar3;
  int iStack_18;
  int iStack_14;

  if (mode == 0)
  {
    iStack_14 = get_power(p, c);
  }
  else if (mode == 1)
  {
    iStack_14 = get_toughness(p, c);
  }
  else
  {
    iStack_14 = 0;
    if (mode == 2)
    {
      iStack_14 = get_cmc(p, c);
    }
  }
  if (modifier_for_instant_sorceries + 1 < iStack_14)
  {
    if (active_cards_count[p] < 1)
    {
      iVar3 = 0;
      iStack_18 = 0;
    }
    else
    {
      iVar3 = 0;
      iStack_18 = 0;
      card = 0;
      do
      {
        while (true)
        {
          instance = in_play(p, card);
          if ((instance == NULL) ||
              (iVar2 = is_humiliated_for_ability(p, card, AL_STATIC), iVar2 != 0))
            goto LAB_0237ad08;

          iVar2 = get_id(p, card);
          if (iVar2 == 0x2626)
            goto LAB_0237aea4;
          if (iVar2 < 0x2627)
            break;
          if (iVar2 == 0x3364)
            goto LAB_0237ad90;
          if (iVar2 < 0x3365)
          {
            if (iVar2 == 0x2923)
              goto LAB_0237aea4;
            if (iVar2 < 0x2924)
            {
              if (iVar2 == 0x28f8)
                goto LAB_0237aea4;
              if ((iVar2 == 0x2908) || (iVar2 == 0x2829))
                goto LAB_0237ad90;
            }
            else if ((iVar2 == 0x2a2e) || (iVar2 == 0x318e))
              goto LAB_0237ad90;
          }
          else
          {
            if (iVar2 == 0x3630)
              goto LAB_0237ad90;
            if (iVar2 < 0x3631)
            {
              if (iVar2 != 0x3436)
              {
                if (iVar2 != 0x34f4)
                  goto LAB_0237ad08;
                goto LAB_0237ad90;
              }
            }
            else
            {
              if (iVar2 == 0x3747)
                goto LAB_0237ad90;
              if (iVar2 != 0x3c57)
                goto LAB_0237ad08;
            }
          LAB_0237aea4:
            iStack_18 = iStack_18 + 1;
          }
        LAB_0237ad08:
          card = card + 1;
          if (active_cards_count[p] <= card)
            goto LAB_0237ada0;
        }
        if (iVar2 == 0x1373)
          goto LAB_0237aea4;
        if (iVar2 < 0x1374)
        {
          if (iVar2 != 0xb2)
          {
            if (iVar2 < 0xb3)
            {
              if (iVar2 != 8)
              {
                if (iVar2 == 0x97)
                  goto LAB_0237aea4;
                goto LAB_0237ad08;
              }
            }
            else if (iVar2 != 0x55b)
            {
              if (iVar2 < 0x55c)
              {
                if ((iVar2 == 0x3c6) || (iVar2 == 0x3f0))
                  goto LAB_0237ad90;
              }
              else if (iVar2 == 0x123a)
                goto LAB_0237ad90;
              goto LAB_0237ad08;
            }
          }
        }
        else
        {
          if (iVar2 == 0x2191)
            goto LAB_0237aea4;
          if (iVar2 < 0x2192)
          {
            if ((iVar2 != 0x1757) && (iVar2 != 0x1dea))
              goto LAB_0237ad08;
          }
          else if (iVar2 != 0x261f)
          {
            if (iVar2 < 0x2620)
            {
              if ((iVar2 == 0x24ff) || (iVar2 == 0x254a))
                goto LAB_0237ad90;
            }
            else if (iVar2 == 0x2621)
              goto LAB_0237ad90;
            goto LAB_0237ad08;
          }
        }
      LAB_0237ad90:
        iVar3 = iVar3 + 1;
        card = card + 1;
      } while (card < active_cards_count[p]);
    LAB_0237ada0:
      iStack_18 = iStack_18 << 3;
      iVar3 = iVar3 * 10;
    }
    iVar3 = iStack_18 + iStack_14 * 4 + iVar3;
  }
  else
  {
    iVar3 = -0x80;
  }
  return iVar3;
}

/* ManalinkEh 2022: 0x237d4e0; original cards/mirage.c:2156. */
int card_ersatz_gnomes(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_t *ptStack_bc;
  target_definition_t tds;
  target_definition_t tdp;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    counterspell_target_definition(player, card, &tds, 0xff);
    default_target_definition(player, card, &tdp, 0x47);
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {

      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x401, 0, 0, 0, 0, 0, 0,
                                        0, &tds, NULL);
      if (iVar2 == 0)
      {
        iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, 0, 0, 0, 0, 0, 0, 0,
                                          &tdp, NULL);
        return iVar2;
      }
      return iVar2;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->info_slot = 0;
      instance->number_of_targets = '\0';
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x401, 0, 0, 0, 0, 0, 0,
                                        0, &tds, NULL);
      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, 0, 0, 0, 0, 0, 0, 0,
                                        &tdp, NULL);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\004", "\001\016",
                          "Make a spell colorless", iVar2, 1, "Make a permanent colorless", iVar3,
                          1, 0, 0, 0, 0);
      if (iVar2 == 1)
      {
        iVar2 = generic_activated_ability(player, card, EVENT_ACTIVATE, 0x401, 0, 0, 0, 0, 0, 0, 0,
                                          &tds, NULL);
        return iVar2;
      }
      if (iVar2 != 2)
      {
        return 0;
      }
      iVar2 = generic_activated_ability(player, card, EVENT_ACTIVATE, 5, 0, 0, 0, 0, 0, 0, 0, &tdp,
                                        "TARGET_PERMANENT");
      return iVar2;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    counterspell_target_definition(player, card, &tds, 0xff);
    default_target_definition(player, card, &tdp, 0x47);
    instance = get_card_instance(player, card);
  }
  ptStack_bc = &tdp;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    iVar2 = instance->info_slot;
    if (iVar2 == 1)
    {
      iVar2 = counterspell_validate(player, card, &tds, 0);
      if (iVar2 != 0)
      {
        change_color(player, card, instance->targets[0].player, instance->targets[0].card,
                     COLOR_TEST_COLORLESS, CHANGE_COLOR_END_AT_EOT | CHANGE_COLOR_SET);
      }
      iVar2 = instance->info_slot;
    }
    if ((iVar2 == 2) && (iVar2 = validate_target(player, card, ptStack_bc, 0), iVar2 != 0))
    {
      change_color(instance->parent_controller, instance->parent_card, instance->targets[0].player,
                   instance->targets[0].card, COLOR_TEST_COLORLESS,
                   CHANGE_COLOR_END_AT_EOT | CHANGE_COLOR_SET);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2376ab0; original cards/mirage.c:6167. */
static int fx_barreling_attack(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  if (event == EVENT_DECLARE_BLOCKERS)
  {
    result = is_attacking((int)instance->damage_target_player, instance->damage_target_card);
    if (result != 0)
    {
      result = is_unblocked((int)instance->damage_target_player, instance->damage_target_card);
      if (result == 0)
      {
        result = is_humiliated_for_ability((int)instance->damage_target_player,
                                           instance->damage_target_card, AL_TRIG);
        if (result == 0)
        {
          pump_when_blocked((int)instance->damage_target_player, instance->damage_target_card,
                            EVENT_DECLARE_BLOCKERS, 1, 1, 0);
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

/* ManalinkEh 2022: 0x2389990; original cards/mirage.c:6183. */
int card_barreling_attack(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event == EVENT_RESOLVE_SPELL)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    result = valid_target(&td);
    if (result != 0)
    {

      instance = get_card_instance(player, card);
      create_targetted_legacy_effect(player, card, fx_barreling_attack, instance->targets[0].player,
                                     instance->targets[0].card);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = vanilla_instant_pump_2022(player, card, event, 2, player, 0, 0, 0x80, 0);
  return result;
}

/* ManalinkEh 2022: 0x2389cd0; original cards/mirage.c:6257. */
int card_blighted_shaman(int player, int card, event_t event)
{
  const char *pcVar1;
  card_instance_t *instance;
  int result;
  unsigned int uVar4;
  unsigned int player_00;
  target_definition_t *ptStack_218;
  target_definition_t td;
  test_definition_t this_test;
  test_definition_t test;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    pcVar1 = get_hacked_land_text(player, card, "Select a %s to sacrifice.", 0x5009);
    new_default_test_definition(&this_test, 1, pcVar1);
    this_test.subtype = get_hacked_subtype(player, card, 0x5009);
    new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = new_can_sacrifice_as_cost(player, card, &this_test);
      if (result == 0)
      {
        if (player == 0)
        {
          result = generic_activated_ability(0, card, EVENT_CAN_ACTIVATE, 0x205, 0, 0, 0, 0, 0, 0,
                                             0, &td, NULL);
          return result;
        }
        result = count_subtype(player, 2, -1);
        if (result < 2)
        {
          return 0;
        }
        result = 0x205;
      }
      else
      {
        result = 5;
      }
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, result, 0, 0, 0, 0, 0, 0,
                                         0, &td, NULL);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {
      result = charge_mana_for_activated_ability(player, card, 0, 0, 0, 0, 0, 0);
      if (result == 0)
      {
        return 0;
      }

      instance->info_slot = 0;
      instance->number_of_targets = '\0';
      result = new_can_sacrifice_as_cost(player, card, &this_test);
      uVar4 = 0;
      if (result != 0)
      {
        result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, 0, 0, 0, 0, 0, 0, 0,
                                           &td, NULL);
        uVar4 = (unsigned int)(result != 0);
      }
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x205, 0, 0, 0, 0, 0, 0,
                                         0, &td, NULL);
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\004", "\001\016",
                           "Sac Swamp target gets +1/+1", uVar4, 1, "Sac crit target gets +2/+2",
                           result, 1, 0, 0, 0, 0);
      instance->info_slot = result;
      if (result != 1)
      {
        if (result != 2)
        {
          return 0;
        }
        result = generic_activated_ability(player, card, EVENT_ACTIVATE, 0x205, 0, 0, 0, 0, 0, 0, 0,
                                           &td, "TARGET_CREATURE");
        return result;
      }

      uVar4 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                            &this_test);
      if (uVar4 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      instance->number_of_targets = '\0';
      result = generic_activated_ability(player, card, EVENT_ACTIVATE, 5, 0, 0, 0, 0, 0, 0, 0, &td,
                                         "TARGET_CREATURE");
      player_00 = (int)uVar4 >> 0x10 & 0xff;
      if (result != 0)
      {
        kill_card(player_00, uVar4 >> 0x18, KILL_SACRIFICE);
        instance->number_of_targets = '\x01';
        return 0;
      }
      state_untargettable(player_00, uVar4 >> 0x18, 0);
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
    pcVar1 = get_hacked_land_text(player, card, "Select a %s to sacrifice.", 0x5009);
    new_default_test_definition(&this_test, 1, pcVar1);
    this_test.subtype = get_hacked_subtype(player, card, 0x5009);
    new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
    instance = get_card_instance(player, card);
  }
  ptStack_218 = &td;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    result = instance->info_slot;
    if (result == 1)
    {
      result = valid_target(ptStack_218);
      if (result != 0)
      {
        pump_until_eot_2022(instance->parent_controller, instance->parent_card,
                       instance->targets[0].player, instance->targets[0].card, 1, 1);
      }
      result = instance->info_slot;
    }
    if ((result == 2) && (result = valid_target(ptStack_218), result != 0))
    {
      pump_until_eot_2022(instance->parent_controller, instance->parent_card,
                     instance->targets[0].player, instance->targets[0].card, 2, 2);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x238b890; original cards/mirage.c:6685. */
int card_cloak_of_invisibility(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_BLOCK_LEGALITY)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);

    if ((((result == 0) && (instance = get_card_instance(player, card),
                            instance->damage_target_player == attacking_card_controller)) &&
         (instance->damage_target_card == attacking_card)) &&
        ((result = has_subtype(affected_card_controller, affected_card, SUBTYPE_WALL),
          result == 0 && (instance = in_play(player, card), instance != NULL))))
    {
      event_result = 1;
    }
  }
  else
  {
    if (event == EVENT_CAN_CAST)
      goto LAB_0238b8c8;
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_0238b8c8;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_0238b8c8;
  }
  if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
       ((((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)) &&
          (event != EVENT_CHANGE_TYPE)) &&
         ((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)))))) &&
      (event != EVENT_STATIC_EFFECTS))
  {
    if (event == EVENT_PHASING)
    {

      instance = get_card_instance(player, card);
      if (current_turn == instance->damage_target_player)
      {
        result = phasing((int)instance->damage_target_player, instance->damage_target_card, player);
        return result;
      }
    }
    return 0;
  }
LAB_0238b8c8:
  result = vanilla_aura_2022(player, card, event, player);
  return result;
}

/* ManalinkEh 2022: 0x238bda0; original cards/mirage.c:6778. */
int card_crimson_roc(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if ((event == EVENT_DECLARE_BLOCKERS) && (current_turn != player))
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result == 0)
    {

      instance = get_card_instance(player, card);
      if (instance->blocking != 0xff)
      {
        result = check_for_ability(1 - player, (unsigned int)instance->blocking, 0x20);
        if (result == 0)
        {
          pump_ability_until_eot_2022(player, card, player, card, 1, 0, 0x100, 0);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x238c730; original cards/mirage.c:6993. */
int card_ebony_charm(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  target_definition_t td2;
  target_definition_t td3;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td2, 0);
    td2.allowed_controller = 1 - player;
    td2.zone = 0x1000;
    default_target_definition(player, card, &td3, 2);
    td3.preferred_controller = player;
    get_card_instance(player, card);
    iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 0x40, &td2, NULL, 1, NULL);
    if (((iVar2 != 0) ||
         (iVar2 = ability_decompose(player, card, EVENT_CAN_ACTIVATE), iVar2 != 0)) ||
        (iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td3, NULL, 1, NULL), iVar2 != 0))
    {
      return 1;
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td2, 0);
      td2.allowed_controller = 1 - player;
      td2.zone = 0x1000;
      default_target_definition(player, card, &td3, 2);
      td3.preferred_controller = player;
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        iVar2 = check_special_flags2_2022(player, card, -0x80000000);
        if (iVar2 == 0)
        {

          iVar3 = generic_spell(player, card, EVENT_CAN_CAST, 0x40, &td2, NULL, 1, NULL);
          iVar4 = ability_decompose(player, card, EVENT_CAN_ACTIVATE);
          iVar5 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td3, NULL, 1, NULL);
          iVar2 = 1;
          if (current_turn == player)
          {
            iVar2 = (unsigned int)(current_phase == 0x14) * 9 + 1;
          }
          iVar2 =
              dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "Suck 1 life.", iVar3, 5,
                          "Exile from grave.", iVar4, 10, "Give Fear.", iVar5, iVar2, 0, 0, 0, 0);
          if (iVar2 == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
        }
        iVar2 = instance->info_slot;
        if (iVar2 == 1)
        {
          iVar2 = generic_spell(player, card, EVENT_CAST_SPELL, 0xc0, &td2,
                                "Select target opponent.", 1, NULL);
        }
        else
        {
          if (iVar2 == 2)
          {
            ability_decompose(player, card, EVENT_ACTIVATE);
            iVar2 = instance->info_slot;
          }
          if (iVar2 != 3)
          {
            return 0;
          }
          iVar2 = generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td3,
                                "Select target creature to give fear.", 1, NULL);
        }
        return iVar2;
      }
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td2, 0);
    td2.allowed_controller = 1 - player;
    td2.zone = 0x1000;
    default_target_definition(player, card, &td3, 2);
    td3.preferred_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {

      iVar2 = instance->info_slot;
      if (iVar2 == 2)
      {
        ability_decompose(player, card, EVENT_RESOLVE_ACTIVATION);
      }
      else if (iVar2 == 3)
      {
        iVar2 = valid_target(&td3);
        if (iVar2 != 0)
        {
          pump_ability_until_eot(player, card, instance->targets[0].player,
                                 instance->targets[0].card, 0, 0, 0, 1);
        }
      }
      else if ((iVar2 == 1) && (iVar2 = valid_target(&td2), iVar2 != 0))
      {
        lose_life(instance->targets[0].player, 1);
        gain_life(player, 1);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x238ccf0; original cards/mirage.c:7095. */
int card_ekundu_cyclops(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;

  store_attackers(player, card, event, 0, player, -1, NULL);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  iVar1 = current_turn;

  if ((event == EVENT_DAT_ABILITY) && (0 < active_cards_count[current_turn]))
  {
    iVar4 = 0;
    do
    {

      instance = in_play(player, card);
      if (instance != NULL)
      {
        iVar3 = can_attack(player, card);
        if (iVar3 != 0)
        {
          if ((instance->state & 4) == 0)
          {
            instance->state = instance->state | 4;
          }
        }
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < active_cards_count[iVar1]);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x238d5d0; original cards/mirage.c:7289. */
int card_favorable_destiny(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  unsigned int uVar4;
  int player_00;

  instance = in_play(player, card);
  if (((instance == NULL) ||
       (result = is_humiliated_for_ability(player, card, AL_STATIC), result != 0)) ||
      (instance = get_card_instance(player, card), instance->damage_target_player == -1))
  {
  LAB_0238d60e:

    if (event == EVENT_CAN_CAST)
      goto LAB_0238d630;
    if (event != EVENT_CAST_SPELL)
      goto LAB_0238d618;
    if ((card == affected_card) && (player == affected_card_controller))
      goto LAB_0238d630;
  }
  else
  {
    instance = get_card_instance(player, card);
    player_00 = (int)instance->damage_target_player;
    result = instance->damage_target_card;
    uVar3 = get_color(player_00, result);
    uVar4 = get_sleighted_color_test(player_00, result, 0x20);
    if ((uVar3 & uVar4) == 0)
    {
    LAB_0238d768:
      if (event != EVENT_ABILITIES)
        goto LAB_0238d60e;
      if ((result == affected_card) && ((player_00 == affected_card_controller &&
                                         (result = count_subtype(player_00, 2, -1), 1 < result))))
      {
        event_result = event_result | 0x20000;
      }
    }
    else
    {
      if (event == EVENT_POWER)
      {
        if ((result == affected_card) && (player_00 == affected_card_controller))
        {
          event_result = event_result + 1;
        }
      }
      else
      {
        if (event != EVENT_TOUGHNESS)
          goto LAB_0238d768;
        if ((result == affected_card) && (player_00 == affected_card_controller))
        {
          event_result = event_result + 2;
        }
      }
    LAB_0238d618:
      if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
          (event == EVENT_CHANGE_TARGET))
        goto LAB_0238d630;
    }
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
LAB_0238d630:
  result = vanilla_aura_2022(player, card, event, player);
  return result;
}

/* ManalinkEh 2022: 0x238d900; original cards/mirage.c:7341. */
int card_gibbering_hyenas(int player, int card, event_t event)
{
  int result;
  unsigned int uVar2;

  if (event == EVENT_BLOCK_LEGALITY)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (((result == 0) && (card == affected_card)) && (player == affected_card_controller))
    {

      result = get_sleighted_color(attacking_card_controller, attacking_card, 1);
      uVar2 = get_color(attacking_card_controller, attacking_card);
      if ((1 << ((unsigned char)result & 0x1f) & uVar2) != 0)
      {
        event_result = 1;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x238deb0; original cards/mirage.c:7479. */
int card_haunting_apparition(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if ((((event == EVENT_POWER) && (card != -1)) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 2);
    test.color = get_sleighted_color_test(player, card, 8);
    result = new_special_count_grave(1 - player, &test);
    set_characteristic_pt(result + 1);
  }
  return 0;
}

/* ManalinkEh 2022: 0x238ecb0; original cards/mirage.c:7755. */
int card_lead_golem(int player, int card, event_t event)
{
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }
  does_not_untap_effect(player, card, player, card, 0, 1);
  return 0;
}

/* ManalinkEh 2022: 0x238faf0; original cards/mirage.c:8044. */
int card_prismatic_lace(int player, int card, event_t event)
{
  int result;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  unsigned int uVar4;
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
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {

            pcVar2 = get_card_instance(player, card);
            result = get_deck_color(player, 1 - player);
            if (player == 1)
            {
              uVar4 = 1 << ((unsigned char)result & 0x1f);
            }
            else
            {
              uVar4 = 0;
              while (result = dialog_impl(player, card, EVENT_RESOLVE_SPELL, "\001\002", "\001\014",
                                          "Black", (unsigned int)((uVar4 & 2) == 0), 1, "Blue",
                                          (unsigned int)((uVar4 & 4) == 0), 1, "Green",
                                          (unsigned int)((uVar4 & 8) == 0), 1, "Red",
                                          (unsigned int)((uVar4 & 0x10) == 0), 1, "White",
                                          (unsigned int)((uVar4 & 0x20) == 0), 1, 0, 0, 0, 0),
                     result != 0)
              {

                uVar4 = uVar4 | 1 << ((unsigned char)result & 0x1f);
              }
            }
            pcVar3 = get_card_instance(player, card);
            pcVar3->targets[1].card = uVar4;
            result =
                create_targetted_legacy_effect(player, card, prismatic_lace_legacy,
                                               pcVar2->targets[0].player, pcVar2->targets[0].card);
            pcVar2 = get_card_instance(player, result);
            pcVar2->targets[1].player = player;
            pcVar2->targets[1].card = uVar4;
            pcVar2->number_of_targets = '\x01';
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0238fb4a;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x47);
LAB_0238fb4a:
  result = generic_spell(player, card, event, 0x81, &td,
                         "Select target permanent, becomes the color or colors of your choice.", 1,
                         NULL);
  return result;
}

/* ManalinkEh 2022: 0x238ff90; original cards/mirage.c:8127. */
int card_radiant_essence(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  test_definition_t this_test;

  instance = in_play(player, card);
  if (instance != NULL)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      default_test_definition(&this_test, 0x47);
      this_test.color = get_sleighted_color_test(player, card, 2);
      result = check_battlefield_for_special_card(player, card, 1 - player, 0, &this_test);
      if (result != 0)
      {
        if (event == EVENT_POWER)
        {
          if ((card == affected_card) && (player == affected_card_controller))
          {
            event_result = event_result + 1;
          }
        }
        else if (((event == EVENT_TOUGHNESS) && (card == affected_card)) &&
                 (player == affected_card_controller))
        {
          event_result = event_result + 2;
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2391430; original cards/mirage.c:8557. */
int card_sawback_manticore(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_t td;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.required_state = 0x20;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 4, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      if (iVar3 != 0)
      {
        return 1;
      }
      iVar3 = is_attacking(player, card);
      if (iVar3 == 0)
      {
        if (current_phase != 0x18)
        {
          return 0;
        }
        if (current_turn == player)
        {
          return 0;
        }
        if (instance->blocking == '\0')
        {
          return 0;
        }
      }
      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x2004, 1, 0, 0, 0, 0, 0,
                                        0, &td, NULL);
      return iVar3;
    }
    if (event == EVENT_ACTIVATE)
    {

      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 4, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x2004, 1, 0, 0, 0, 0, 0,
                                        0, &td, NULL);
      iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Gains flying", iVar3, 5,
                          "Deal damage", iVar2, (current_phase != 0x18) - 1 & 10, 0, 0, 0, 0);
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
        iVar3 = generic_activated_ability(player, card, EVENT_ACTIVATE, 0x2006, 1, 0, 0, 0, 0, 0, 0,
                                          &td, "Select target attacking or blocking creature.");
        return iVar3;
      }
      iVar3 = generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 4, 0, 0, 0, 0, 0, 0, NULL,
                                        NULL);
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
    td.required_state = 0x20;
    instance = get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    iVar3 = instance->info_slot;
    if (iVar3 == 1)
    {
      pump_ability_until_eot_2022(player, card, instance->parent_controller, instance->parent_card, 0, 0,
                             0x20, 0);
      iVar3 = instance->info_slot;
    }
    if ((iVar3 == 2) && (iVar3 = valid_target(&td), iVar3 != 0))
    {
      damage_target0(player, card, 2);
    }
  }
  else if (event == EVENT_CLEANUP)
  {
    instance = get_card_instance(player, card);
    instance->targets[2].player = 0;
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x23934a0; original cards/mirage.c:9273. */
int card_urborg_panther(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  unsigned int uVar3;
  unsigned int uVar4;
  int iVar5;
  unsigned int player_00;
  unsigned int player_01;
  target_definition_t td2;
  test_definition_t test;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td2, 2);
    td2.special = 0x8000;
    td2.extra = (int32_t)is_blocking_urborg_panther;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar5 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0xe, 0, 1, 0, 0, 0, 0, 0,
                                        &td2, "Select target creature blocking Urborg Panther.");
      if (iVar5 == 0)
      {
        iVar5 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 8, 0, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        if (iVar5 == 0)
        {
          return 0;
        }
        iVar5 = check_for_feral_shadow_and_breathstealer(player);
        if (iVar5 == 0)
        {
          return 0;
        }
      }
      return 1;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->info_slot = 0;
      iVar5 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0xe, 0, 1, 0, 0, 0, 0, 0,
                                        &td2, "Select target creature blocking Urborg Panther.");
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 8, 0, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      uVar3 = 0;
      if (iVar2 != 0)
      {
        iVar2 = check_for_feral_shadow_and_breathstealer(player);
        uVar3 = (unsigned int)(iVar2 != 0);
      }
      iVar5 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Sac this destroy blocker",
                          iVar5, 5, "Sac to fetch", uVar3, 10, 0, 0, 0, 0);
      if (iVar5 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar5 == 1)
      {
        iVar5 = generic_activated_ability(player, card, EVENT_ACTIVATE, 0xe, 0, 1, 0, 0, 0, 0, 0,
                                          &td2, "Select target creature blocking Urborg Panther.");
        return iVar5;
      }
      if (iVar5 != 2)
      {
        return 0;
      }
      iVar5 = generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 0, 0, 0, 0, 0, 0, 0, NULL,
                                        NULL);
      if (iVar5 == 0)
      {
        return 0;
      }

      new_default_test_definition(&test, 0x47,
                                  "Select a Feral Shadow & a Breathstealer to sacrifice.");
      test.id = 0x9ad;
      uVar3 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                            &test);
      if (uVar3 != 0)
      {
        test.id = 0x971;
        uVar4 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                              &test);
        if (uVar4 != 0)
        {
          player_00 = (int)uVar3 >> 0x10 & 0xff;
          kill_card(player_00, uVar3 >> 0x18, KILL_SACRIFICE);
          player_01 = (int)uVar4 >> 0x10 & 0xff;
          kill_card(player_01, uVar4 >> 0x18, KILL_SACRIFICE);
          if (-1 < td2.who_chooses)
          {
            state_untargettable(td2.who_chooses >> 0x10 & 0xff,
                                (unsigned int)td2.who_chooses >> 0x18, 0);
          }
          if (spell_fizzled == 1)
          {
            return 0;
          }
          kill_card(player_00, uVar3 >> 0x18, KILL_SACRIFICE);
          kill_card(player_01, uVar4 >> 0x18, KILL_SACRIFICE);
          kill_card(player, card, KILL_SACRIFICE);
          return 0;
        }
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
    default_target_definition(player, card, &td2, 2);
    td2.special = 0x8000;
    td2.extra = (int32_t)is_blocking_urborg_panther;
    instance = get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    iVar5 = instance->info_slot;
    if (iVar5 == 1)
    {
      iVar5 = valid_target(&td2);
      if (iVar5 != 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      }
      iVar5 = instance->info_slot;
    }
    if (iVar5 == 2)
    {
      new_default_test_definition(&test, 0, "Select a card named Spirit of the Night.");
      test.id = 0x513;
      new_global_tutor(player, player, 1, 4, 0, 5, &test);
      return 0;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x238b020; original cards/mirage.c:6548. */
int card_chaos_charm(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_definition_t *td_00;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint32_t uVar6;
  const char *prompt;
  target_definition_t td;
  target_definition_t td2;
  target_definition_t td3;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x47);
    td.required_subtype = 0xd1;
    default_target_definition(player, card, &td2, 2);
    td2.preferred_controller = 2;
    default_target_definition(player, card, &td3, 2);
    td3.preferred_controller = player;
    get_card_instance(player, card);
    iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    if (((iVar2 != 0) ||
         (iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td2, NULL, 1, NULL),
          iVar2 != 0)) ||
        (iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td3, NULL, 1, NULL), iVar2 != 0))
    {
      return 1;
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 0x47);
      td.required_subtype = 0xd1;
      default_target_definition(player, card, &td2, 2);
      td2.preferred_controller = 2;
      default_target_definition(player, card, &td3, 2);
      td3.preferred_controller = player;
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        iVar2 = check_special_flags2_2022(player, card, -0x80000000);
        if (iVar2 == 0)
        {

          iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
          iVar3 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td2, NULL, 1, NULL);
          iVar4 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td3, NULL, 1, NULL);
          if ((current_turn == player) && (current_phase == 0x14))
          {
            uVar6 = 10;
            iVar5 = 1;
          }
          else
          {
            uVar6 = 1;
            iVar5 = (unsigned int)(0x17 < current_phase) * 9 + 1;
          }
          iVar2 =
              dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "Destroy Wall.", iVar2, 5,
                          "Deal 1 damage.", iVar3, iVar5, "Give Haste.", iVar4, uVar6, 0, 0, 0, 0);
          if (iVar2 == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
        }
        iVar2 = instance->info_slot;
        if (iVar2 == 1)
        {
          prompt = "Select target Wall to destroy.";
          td_00 = &td;
        }
        else
        {
          if (iVar2 == 2)
          {
            generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td2,
                          "Select target creature to deal 1 damage.", 1, NULL);
            iVar2 = instance->info_slot;
          }
          if (iVar2 != 3)
          {
            return 0;
          }
          prompt = "Select target creature to give haste.";
          td_00 = &td3;
        }
        iVar2 = generic_spell(player, card, EVENT_CAST_SPELL, 0x81, td_00, prompt, 1, NULL);
        return iVar2;
      }
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 0x47);
    td.required_subtype = 0xd1;
    default_target_definition(player, card, &td2, 2);
    td2.preferred_controller = 2;
    default_target_definition(player, card, &td3, 2);
    td3.preferred_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {

      iVar2 = instance->info_slot;
      if (iVar2 == 2)
      {
        iVar2 = valid_target(&td2);
        if (iVar2 != 0)
        {
          damage_creature(instance->targets[0].player, instance->targets[0].card, 1, player, card);
        }
      }
      else if (iVar2 == 3)
      {
        iVar2 = valid_target(&td3);
        if (iVar2 != 0)
        {
          pump_ability_until_eot(player, card, instance->targets[0].player,
                                 instance->targets[0].card, 0, 0, 0, 4);
        }
      }
      else if ((iVar2 == 1) && (iVar2 = valid_target(&td), iVar2 != 0))
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int cycle_of_life_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2377290; original cards/mirage.c:6809. */
static int cycle_of_life_legacy(int player, int card, event_t event)
{
  char cVar1;
  int t_card;
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  if (-1 < instance->damage_target_player)
  {

    instance = get_card_instance(player, card);
    if (player == current_turn)
    {
      cVar1 = instance->damage_target_player;
      t_card = instance->damage_target_card;
      result = upkeep_trigger(player, card, event);
      if (result != 0)
      {
        result = count_upkeeps(player);
        if (0 < result)
        {
          add_counters_from_source(player, card, (int)cVar1, t_card, COUNTER_P1_P1, 1);
          kill_card(player, card, KILL_EXILE);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x238be60; original cards/mirage.c:6830. */
int card_cycle_of_life(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.allowed_controller = player;
    td.required_state = 0x1000;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 0, 0, 0, 0, 0, 0, 0,
                                         &td, NULL);
      return result;
    }
    result = charge_mana_for_activated_ability_2022(player, card, 0, 0, 0, 0, 0, 0);
    if ((result != 0) && (result = pick_target(&td, "TARGET_CREATURE"), result != 0))
    {
      bounce_permanent(player, card);
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_0238be8e;

    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.allowed_controller = player;
    td.required_state = 0x1000;
    instance = get_card_instance(player, card);
  }
  if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
  {
    real_set_pt(player, card, instance->targets[0].player, instance->targets[0].card, 0, 1, 4);
    create_targetted_legacy_effect(player, card, cycle_of_life_legacy, instance->targets[0].player,
                                   instance->targets[0].card);
  }
LAB_0238be8e:
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x238dff0; original cards/mirage.c:7525. */
int card_jabaris_influence(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  const char *prompt;
  target_definition_t td1;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td1, 2);
    td1.illegal_color = get_sleighted_color_test(player, card, 2);
    td1.illegal_type = 0x40;
    td1.required_state = 4;
    get_card_instance(player, card);
    if ((0x1d < current_phase) && (1 - player == current_turn))
    {
      result = can_target(&td1);
      return result;
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
        default_target_definition(player, card, &td1, 2);
        td1.illegal_color = get_sleighted_color_test(player, card, 2);
        td1.illegal_type = 0x40;
        td1.required_state = 4;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td1);
          if (result != 0)
          {
            gain_control_permanently(player, instance->targets[0].player,
                                     instance->targets[0].card);
            add_counters_from_source(player, card, instance->targets[0].player,
                                     instance->targets[0].card, COUNTER_M1_M0, 1);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0238e109;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
    default_target_definition(player, card, &td1, 2);
    td1.illegal_color = get_sleighted_color_test(player, card, 2);
    td1.illegal_type = 0x40;
    td1.required_state = 4;
    get_card_instance(player, card);
  }
LAB_0238e109:
  prompt = get_sleighted_color_text(player, card, "Select target nonartifact, non%s creature.", 1);
  result = generic_spell(player, card, event, 0x81, &td1, prompt, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x238f510; original cards/mirage.c:7921. */
int card_mire_shade(int player, int card, event_t event)
{
  const char *pcVar1;
  card_instance_t *instance;
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

      instance = get_card_instance(player, card);
      add_counters_from_source(player, card, instance->parent_controller, instance->parent_card,
                               COUNTER_P1_P1, 1);
    }
  }
  result = generic_activated_ability_with_sacrifice_as_cost(
      player, card, event, GAA_CAN_SORCERY_BE_PLAYED, 0, 1, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE,
      &this_test, SARV_0);
  return result;
}

/* ManalinkEh 2022: 0x2390180; original cards/mirage.c:8171. */
int card_ravenous_vampire(int player, int card, event_t event)
{
  unsigned int uVar1;
  int result;
  card_instance_t *instance;
  target_definition_t td;

  upkeep_trigger_ability(player, card, event, player);
  if (event != EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    return 0;
  }

  default_target_definition(player, card, &td, 2);
  td.allowed_controller = player;
  td.preferred_controller = player;
  td.illegal_abilities = 0;
  td.illegal_type = 0x40;
  if ((player == 1) || (uVar1 = 0, ai_is_speculating == 1))
  {
    td.special = 0x4000;
    result = can_target(&td);
    uVar1 = -(unsigned int)(result == 0) | 3;
  }
  result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\014", "\001\003", "\001\002",
                       "Sacrifice for Crovax", 1, uVar1, "Pass", 1, 1, 0, 0, 0, 0);
  if (result == 1)
  {
    result = can_target(&td);
    if (result != 0)
    {
      result = controller_sacrifices_a_permanent(player, card, 2, 0);
      if (result != 0)
      {
        instance = in_play(player, card);
        if (instance != NULL)
        {
          add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
        }
        goto LAB_0239028e;
      }
    }
  }
  tap_card(player, card);
LAB_0239028e:
  instance = get_card_instance(player, card);
  instance->number_of_targets = '\0';
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int sabertooth_cobra_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2389a70; original cards/mirage.c:6206. */
int card_binding_agony(int player, int card, event_t event)
{
  char cVar1;
  card_instance_t *pcVar2;
  int result;
  card_instance_t *pcVar4;

  pcVar2 = in_play(player, card);
  if (((pcVar2 == NULL) ||
       (pcVar2 = get_card_instance(player, card), pcVar2->damage_target_card == -1)) ||
      (result = is_humiliated_for_ability(player, card, AL_TRIG), result != 0))
    goto LAB_02389aa4;

  pcVar2 = get_card_instance(player, card);
  if (event == EVENT_DEAL_DAMAGE)
  {
    cVar1 = pcVar2->damage_target_player;
    result = pcVar2->damage_target_card;

    pcVar4 = get_card_instance(affected_card_controller, affected_card);
    if ((pcVar4->internal_card_id == damage_card) &&
        ((pcVar4->damage_target_card == result && (cVar1 == pcVar4->damage_target_player))))
    {

      result = pcVar4->info_slot;
      if (result < 1)
      {

        pcVar4 = get_card_instance((int)pcVar4->damage_source_player, pcVar4->damage_source_card);
        result = pcVar4->targets[0x10].player;
        if (result < 0)
        {
          result = 0;
        }
      }
      pcVar2->info_slot_fn = pcVar2->info_slot_fn + result;
    }
    result = pcVar2->info_slot;
    if (result < 1)
      goto LAB_02389b98;
    if (trigger_condition == 0xd7)
      goto LAB_02389b3a;
  LAB_02389ab2:
    if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
        (event == EVENT_CHANGE_TARGET))
      goto LAB_02389ac8;
  }
  else
  {
    result = pcVar2->info_slot;
    if ((result < 1) || (trigger_condition != 0xd7))
    {
    LAB_02389aa4:
      if (event == EVENT_CAN_CAST)
        goto LAB_02389ac8;
      if (event != EVENT_CAST_SPELL)
        goto LAB_02389ab2;
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_02389ac8;
    }
    else
    {
    LAB_02389b3a:
      if (((card != affected_card) || (player != affected_card_controller)) ||
          (reason_for_trigger_controller != player))
        goto LAB_02389aa4;
      if (event == EVENT_TRIGGER)
      {
        event_result = event_result | 2;
      }
      else
      {
        if (event != EVENT_RESOLVE_TRIGGER)
          goto LAB_02389aa4;
        damage_player(trigger_cause_controller, result, player, card);
        pcVar2->info_slot = 0;
      }
    }
  }
LAB_02389b98:
  if ((((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         (2 < event - EVENT_POWER)) &&
        (event != EVENT_RESOLVE_MOVING_AURA)) &&
       ((event != EVENT_CHANGE_TYPE && (event != EVENT_SET_COLOR)))) &&
      ((event != EVENT_GET_SELECTED_CARD && (event != EVENT_STATIC_EFFECTS))))
  {
    return 0;
  }
LAB_02389ac8:
  result = vanilla_aura_2022(player, card, event, 1 - player);
  return result;
}

/* ManalinkEh 2022: 0x238a620; original cards/mirage.c:6392. */
int card_breathstealer(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_POW_BOOST)
  {
    result = has_mana(player, 1, 1);
    if (result != 0)
    {
      event_result = event_result + 1;
    }
  LAB_0238a6f1:
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_0238a64e;
    }
  }
  else
  {
    if (event == EVENT_TOU_BOOST)
    {
      result = has_mana(player, 1, 1);
      if (result == 0)
      {
        return 0;
      }
      event_result = event_result + -1;
      return 0;
    }
    if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
      goto LAB_0238a6f1;
  LAB_0238a64e:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_0238a657;
  }

  instance = get_card_instance(player, card);
  pump_until_eot_merge_previous_2022(instance->parent_controller, instance->parent_card,
                                instance->parent_controller, instance->parent_card, 1, -1);
LAB_0238a657:
  result = generic_activated_ability(player, card, event, 0, 0, 1, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x238b6c0; original cards/mirage.c:6654. */
int card_cinder_cloud(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int result;
  card_instance_t *pcVar3;
  unsigned int uVar4;
  unsigned int uVar5;
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
        pcVar1 = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {

            kill_card(pcVar1->targets[0].player, pcVar1->targets[0].card, KILL_DESTROY);
            pcVar3 = get_card_instance(pcVar1->targets[0].player, pcVar1->targets[0].card);
            if (pcVar3->kill_code != '\0')
            {
              uVar4 = get_color(pcVar1->targets[0].player, pcVar1->targets[0].card);
              uVar5 = get_sleighted_color_test(player, card, 0x20);
              if ((uVar4 & uVar5) != 0)
              {
                result = get_power(pcVar1->targets[0].player, pcVar1->targets[0].card);
                damage_player(pcVar1->targets[0].player, result, player, card);
              }
            }
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0238b726;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  get_card_instance(player, card);
LAB_0238b726:
  result =
      generic_spell(player, card, event, 0x81, &td, "Select target creature to destroy.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x238bc60; original cards/mirage.c:6757. */
int card_coral_fighters(int player, int card, event_t event)
{
  int result;
  unsigned int argument7;

  if (event == EVENT_DECLARE_BLOCKERS)
  {
    result = is_unblocked(player, card);
    if (result != 0)
    {
      result = is_humiliated_for_ability(player, card, AL_TRIG);
      if (result == 0)
      {

        show_deck(player, deck_ptr[1 - current_turn], 1, "Coral Fighters Reveals...", 0,
                  ENGINE_DONE_LABEL);
        result = is_what(-1, *deck_ptr[1 - current_turn], 1);
        argument7 = 1;
        if (result != 0)
        {
          result = count_subtype(1 - player, 1, -1);
          argument7 = (unsigned int)(result < 5);
        }
        result = do_dialog(player, player, card, -1, -1, " Top\n Bottom", argument7);
        if (result == 1)
        {
          put_top_card_of_deck_to_bottom(1 - current_turn);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x238c370; original cards/mirage.c:6914. */
int card_delirium(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  int argument2;
  target_definition_t td;

  default_target_definition(player, card, &td, 2);
  td.allowed_controller = 1 - player;
  td.preferred_controller = td.allowed_controller;
  if (event == EVENT_CAN_CAST)
  {
    if (td.allowed_controller == current_turn)
    {
      result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
      return result;
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      new_pick_target(&td, "Select target creature your opponent controls, tap it.", 0, 0x80);
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    result = validate_target(player, card, &td, 0);
    if (result != 0)
    {

      instance = get_card_instance(player, card);
      tap_card(instance->targets[0].player, instance->targets[0].card);
      argument2 = get_power(instance->targets[0].player, instance->targets[0].card);
      result = instance->targets[0].player;
      damage_player(result, argument2, result, instance->targets[0].card);
      maze_of_ith_effect(player, card, instance->targets[0].player, instance->targets[0].card);
    }
    kill_card(player, card, KILL_DESTROY);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x238d0c0; original cards/mirage.c:7175. */
int card_ether_well(int player, int card, event_t event)
{
  int card_00;
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  unsigned int uVar4;
  unsigned int uVar5;
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
      default_target_definition(player, card, &td, 2);
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        iVar1 = get_base_value(instance->targets[0].player, instance->targets[0].card);
        ai_modifier = ai_modifier + iVar1;
      }
      goto LAB_0238d12d;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 2);
      instance = get_card_instance(player, card);
      if (event != EVENT_RESOLVE_SPELL)
        goto LAB_0238d12d;
      iVar1 = valid_target(&td);
      if (iVar1 != 0)
      {

        iVar1 = instance->targets[0].player;
        card_00 = instance->targets[0].card;
        iVar3 = is_what(iVar1, card_00, 2);
        if (iVar3 != 0)
        {
          uVar4 = get_color(iVar1, card_00);
          uVar5 = get_sleighted_color_test(iVar1, card_00, 0x10);
          if ((uVar4 & uVar5) != 0)
          {

            iVar3 = get_base_value(instance->targets[0].player, instance->targets[0].card);
            iVar3 = do_dialog(player, player, card, -1, -1,
                              " Put creature on top\n Put creature on bottom",
                              (unsigned int)(0x31 < iVar3));
            if (iVar3 != 0)
            {
              if (iVar3 == 1)
              {
                put_on_bottom_of_deck(iVar1, card_00);
              }
              goto LAB_0238d228;
            }
          }
        }
        put_on_top_of_deck(iVar1, card_00);
      }
    LAB_0238d228:
      kill_card(player, card, KILL_DESTROY);
      goto LAB_0238d12d;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  default_target_definition(player, card, &td, 2);
  get_card_instance(player, card);
LAB_0238d12d:
  iVar1 = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x238d9b0; original cards/mirage.c:7358. */
int card_grave_servitude(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  bool bVar4;
  target_definition_t td;

  if (event == EVENT_MODIFY_COST)
  {
    iVar2 = can_sorcery_be_played(player, EVENT_MODIFY_COST);
    if (iVar2 == 0)
    {
      infinite_casting_cost();
    }
  }
  else
  {
    if (event == EVENT_SHOULD_AI_PLAY)
    {
      iVar2 = should_ai_play(player, card);
      return iVar2;
    }
    if (event == EVENT_CAN_CAST)
    {
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = 2;
      iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
      return iVar2;
    }
    if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
        (player == affected_card_controller))
    {

      iVar2 = can_sorcery_be_played(player, EVENT_CAST_SPELL);
      if (iVar2 == 0)
      {

        instance = get_card_instance(player, card);
        iVar2 = current_turn;
        bVar4 = current_phase == 0x1f;
        instance->targets[3].player = 0x8d;
        instance->targets[3].card = iVar2;
        if (bVar4)
        {
          instance->targets[3].card = 1 - iVar2;
        }
      }
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = 2;
      instance = get_card_instance(player, card);
      generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_CREATURE", 1, NULL);
      if (spell_fizzled != 1)
      {

        iVar3 = get_toughness(instance->targets[0].player, instance->targets[0].card);
        iVar2 = instance->targets[0].player;
        if (iVar2 == player)
        {
          if (iVar3 == 1)
          {
            ai_modifier = ai_modifier + -0x30;
            return 0;
          }

          ai_modifier = ai_modifier + 0xc;
          iVar2 = check_for_ability(player, instance->targets[0].card, 0x40000120);
          if (iVar2 != 0)
          {
            ai_modifier = ai_modifier + 0x18;
          }
          iVar2 = check_for_special_ability(instance->targets[0].player, instance->targets[0].card,
                                            0x110);
          if (iVar2 != 0)
          {
            ai_modifier = ai_modifier + 0x18;
          }
          iVar2 = instance->targets[0].player;
          if (1 - player != iVar2)
          {
            return 0;
          }
        }
        else
        {
          if (iVar2 != 1 - player)
          {
            return 0;
          }
          if (iVar3 == 1)
          {
            ai_modifier = ai_modifier + 0x30;
            return 0;
          }
        }

        ai_modifier = ai_modifier + -0xc;
        iVar2 = check_for_ability(iVar2, instance->targets[0].card, 0x40000120);
        if (iVar2 != 0)
        {
          ai_modifier = ai_modifier + -0x18;
        }
        iVar2 = check_for_special_ability(instance->targets[0].player, instance->targets[0].card,
                                          0x110);
        if (iVar2 != 0)
        {
          ai_modifier = ai_modifier + -0x18;
          return 0;
        }
      }
      return 0;
    }
  }
  instance = get_card_instance(player, card);
  if (((instance->targets[3].player == 0x8d) &&
       (instance = get_card_instance(player, card), instance->targets[3].card == current_turn)) &&
      (current_phase == 0x22))
  {
    kill_card(player, card, KILL_SACRIFICE);
  }
  instance = in_play(player, card);

  if (((instance != NULL) &&
       (iVar2 = is_humiliated_for_ability(player, card, AL_STATIC), iVar2 == 0)) &&
      ((instance = get_card_instance(player, card),
        instance->damage_target_player != -1 &&
            (((instance = get_card_instance(player, card),
               event == EVENT_SET_COLOR && (instance->damage_target_card == affected_card)) &&
              (instance->damage_target_player == affected_card_controller))))))
  {
    event_result = 2;
  }
  iVar2 = generic_aura_2022(player, card, event, player, 3, -1, 0, 0, 0, 0, 0);
  return iVar2;
}

/* ManalinkEh 2022: 0x238de20; original cards/mirage.c:7456. */
int card_gravebane_zombie(int player, int card, event_t event)
{
  int result;
  int owner;
  int position;

  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    result = find_in_owners_graveyard(player, card, &owner, &position);
    if (result != 0)
    {

      result = graveyard_ptr_mutable[owner][position];
      remove_card_from_grave(owner, position);
      raw_put_iid_on_top_of_deck(owner, result);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2390610; original cards/mirage.c:8268. */
int card_reign_of_terror(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  const char *pcVar4;
  int iVar5;
  char buffer[100];

  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      iVar1 = get_sleighted_color_test(player, card, 8);
      count_permanents_by_color(1 - player, 2, iVar1);
      iVar1 = get_sleighted_color_test(player, card, 8);
      count_permanents_by_color(player, 2, iVar1);
      iVar1 = get_sleighted_color_test(player, card, 0x20);
      iVar1 = count_permanents_by_color(1 - player, 2, iVar1);
      iVar3 = get_sleighted_color_test(player, card, 0x20);
      iVar3 = count_permanents_by_color(player, 2, iVar3);
      pcVar4 = get_sleighted_color_text(player, card, " Destroy %s creatures\n", 3);
      iVar5 = scnprintf(buffer, 100, "%s", pcVar4);
      pcVar4 = get_sleighted_color_text(player, card, "Destroy %s creatures", 5);
      scnprintf(buffer + iVar5, 100 - iVar5, "%s", pcVar4);
      iVar1 = do_dialog(player, player, card, -1, -1, buffer, (iVar1 <= iVar3) + 1);
      instance->info_slot = iVar1;
      if (iVar1 == 2)
      {
        spell_fizzled = 1;
        return 0;
      }
    }
    goto LAB_0239062e;
  }
  if (event != EVENT_RESOLVE_SPELL)
    goto LAB_0239062e;

  default_test_definition((test_definition_t *)buffer, 2);
  instance = get_card_instance(player, card);
  if (instance->info_slot == 0)
  {

    iVar1 = 8;
  LAB_02390860:

    get_sleighted_color_test(player, card, iVar1);
    iVar1 = new_manipulate_all(player, card, 2, (test_definition_t *)buffer, ACT_KILL_BURY);
    lose_life_2022(player, iVar1);
  }
  else if (instance->info_slot == 1)
  {
    iVar1 = 0x20;
    goto LAB_02390860;
  }
  kill_card(player, card, KILL_DESTROY);
LAB_0239062e:
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x2390950; original cards/mirage.c:8343. */
int card_roots_of_life(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  const char *pcVar3;
  int iVar4;
  int blt[2];
  char buffer[100];

  if (event == EVENT_RESOLVE_SPELL)
  {

    blt[0] = get_hacked_subtype(player, card, 0x5002);
    blt[1] = get_hacked_subtype(player, card, 0x5009);
    pcVar3 = get_hacked_land_text(player, card, " Select %s land\n", 0x5002);
    iVar2 = scnprintf(buffer, 100, "%s", pcVar3);
    pcVar3 = get_hacked_land_text(player, card, " Select %s land.", 0x5009);
    scnprintf(buffer + iVar2, 100 - iVar2, "%s", pcVar3);
    iVar2 = get_hacked_subtype(player, card, 0x5002);
    iVar2 = count_subtype(1 - player, 1, iVar2);
    iVar4 = get_hacked_subtype(player, card, 0x5009);
    iVar4 = count_subtype(1 - player, 1, iVar4);
    iVar2 = do_dialog(player, player, card, -1, -1, buffer, (unsigned int)(iVar2 < iVar4));
    instance = get_card_instance(player, card);
    instance->targets[1].player = blt[iVar2];
    get_card_instance(player, card);
  }
  else
  {
    instance = get_card_instance(player, card);
    if (((-1 < instance->targets[1].player) && (event == EVENT_TAP_CARD)) &&
        (1 - player == affected_card_controller))
    {
      iVar2 = is_humiliated_for_ability(player, card, AL_TRIG);
      if (iVar2 == 0)
      {
        instance = get_card_instance(player, card);
        iVar2 = has_subtype(affected_card_controller, affected_card, instance->targets[1].player);
        if (iVar2 != 0)
        {
          gain_life_2022(player, 1);
        }
      }
    }
  }
  iVar2 = global_enchantment(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x2376c30; original cards/mirage.c:8371. */
int sabertooth_cobra_legacy(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  upkeep_trigger_ability(player, card, event, player);
  if (event != EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      instance = get_card_instance(player, card);
      kill_card(instance->parent_controller, instance->parent_card, KILL_EXILE);
    }
    goto LAB_02376c67;
  }
  if ((player == 1) && ((ai_is_speculating == 1 || (((unsigned char)trace_mode & 2) == 0))))
  {
    result = has_mana(1, 0, 2);
    if (result != 0)
    {
      charge_mana_while_resolving(1, card, EVENT_UPKEEP_TRIGGER_ABILITY, 1, COLOR_COLORLESS, 2);
      if (spell_fizzled != 1)
      {
        spell_fizzled = 1;
        goto LAB_02376d98;
      }
    }
    poison(1, 1);
  }
  else
  {
    poison(player, 1);
  }
LAB_02376d98:
  kill_card(player, card, KILL_EXILE);
LAB_02376c67:
  result = generic_activated_ability(player, card, event, 0, 2, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2390b90; original cards/mirage.c:8402. */
int card_sabertooth_cobra(int player, int card, event_t event)
{
  unsigned int uVar1;
  int result;
  card_instance_t *instance;
  unsigned int uVar4;
  unsigned int *puStack_20;
  int iStack_1c;
  int times_damaged[2];

  result =
      damage_dealt_by_me(player, card, event, DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER);
  if (result != 0)
  {

    instance = get_card_instance(player, card);
    iStack_1c = 0;
    uVar1 = instance->targets[1].player;
    instance->targets[1].player = 0;
    uVar4 = uVar1 & 0xff;
    puStack_20 = (unsigned int *)times_damaged;
    times_damaged[1] = uVar1 >> 8 & 0xff;
    times_damaged[0] = uVar4;
    while (true)
    {
      while (uVar4 != 0)
      {
        poison(iStack_1c, 1);
        if (player == iStack_1c)
        {
          create_legacy_activate(player, card, sabertooth_cobra_legacy);
          uVar4 = uVar4 - 1;
          *puStack_20 = uVar4;
        }
        else
        {
          create_legacy_activate_for_opponent(player, card, sabertooth_cobra_legacy, player, card);
          uVar4 = uVar4 - 1;
          *puStack_20 = uVar4;
        }
      }
      iStack_1c = iStack_1c + 1;
      puStack_20 = puStack_20 + 1;
      if (iStack_1c == 2)
        break;
      uVar4 = *puStack_20;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2391900; original cards/mirage.c:8632. */
int card_sea_scryer(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  color_t color;
  int iVar3;
  unsigned int uVar4;

  if (event == EVENT_COUNT_MANA)
  {
    if ((((card == affected_card) && (player == affected_card_controller)) &&
         (iVar1 = is_tapped(player, card), iVar1 == 0)) &&
        ((iVar1 = is_sick(player, card),
          iVar1 == 0 && (iVar1 = can_produce_mana(player, card), iVar1 != 0))))
    {
      declare_mana_available(player, 0, 1);
      iVar1 = has_mana(player, 7, 2);
      if (iVar1 != 0)
      {
        declare_mana_available(player, 2, 1);
        return 0;
      }
    }
  }
  else if (((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) &&
           (iVar1 = can_produce_mana(player, card), iVar1 != 0))
  {
    uVar4 = 0;
    iVar1 = is_tapped(player, card);
    if (iVar1 == 0)
    {
      iVar1 = is_sick(player, card);
      uVar4 = (unsigned int)(iVar1 == 0);
    }
    if (player == 1)
    {
      if (event != EVENT_CAN_ACTIVATE)
      {
        produce_mana_tapped_2022(1, card, COLOR_COLORLESS, 1);
        return 0;
      }
      return uVar4;
    }
    instance = get_card_instance(player, card);
    iVar1 = has_mana(player, 7, 1);
    if (((iVar1 == 0) || (iVar1 = is_tapped(player, card), iVar1 != 0)) ||
        (iVar1 = is_sick(player, card), iVar1 != 0))
    {
      if (event != EVENT_CAN_ACTIVATE)
        goto LAB_02391a94;
      iVar1 = 0;
    }
    else
    {
      if (event != EVENT_CAN_ACTIVATE)
      {
        color = choose_a_color_exe(player, "What kind of mana?", 1, 0, uVar4 + 4);
        if (color == ~COLOR_COLORLESS)
        {
          spell_fizzled = 1;
          return 0;
        }
        if (color != COLOR_COLORLESS)
        {
          instance->info_slot = 1;
          charge_mana(player, COLOR_COLORLESS, 1);
          instance->info_slot = 0;
          if (spell_fizzled == 1)
          {
            return 0;
          }
          produce_mana_tapped_2022(player, card, color, 1);
          tapped_for_mana_color = -2;
          return 0;
        }
      LAB_02391a94:
        produce_mana_tapped_2022(player, card, COLOR_COLORLESS, 1);
        return 0;
      }
      iVar1 = 1;
    }
    iVar3 = has_mana(player, 7, 2);
    if (iVar3 != 0)
    {
      return iVar1;
    }
    return uVar4;
  }

  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x238fda0; original cards/mirage.c:8095. */
int card_pyric_salamander(int player, int card, event_t event)
{
  CardFunction p_Var1;
  int result;
  card_instance_t *pcVar3;
  card_instance_t *pcVar4;

  if (event != EVENT_ACTIVATE)
  {
    if (((trigger_condition == 0xcd) && (card == affected_card)) &&
        (player == affected_card_controller))
    {

      pcVar3 = get_card_instance(player, card);
      result = eot_trigger(player, card, event);
      if (result != 0)
      {
        pcVar4 = get_card_instance(player, card);
        if (0 < pcVar4->info_slot)
        {
          result = check_state(player, card, STATE_OUBLIETTED);
          if (result == 0)
          {
            result = is_humiliated_for_ability(player, card, AL_STATIC);
            if (result == 0)
            {
              kill_card(player, card, KILL_SACRIFICE);
            }
          }
        }
        pcVar3->info_slot = 0;
      }
    }
    result = generic_shade_merge_pt(player, card, event, 0, 0, 0, 0, 0, 1, 0, 1, 0);
    return result;
  }

  pcVar3 = get_card_instance(player, card);
  generic_shade(player, card, EVENT_ACTIVATE, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0);
  if ((spell_fizzled != 1) && (p_Var1 = pcVar3->info_slot_fn,
                               pcVar3->info_slot = (int32_t)(p_Var1 + 1), (int)(p_Var1 + 1) < 3))
  {
    ai_modifier = ai_modifier + -0x30;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int effect_soul_echo(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2376dd0; original cards/mirage.c:8982. */
static int effect_soul_echo(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int iVar3;
  int iVar4;
  int num;

  pcVar1 = get_card_instance(player, card);
  iVar4 = pcVar1->targets[0].player;
  card_00 = pcVar1->targets[0].card;
  pcVar2 = damage_being_dealt(event);
  if (((pcVar2 != NULL) && (pcVar2->damage_target_player == iVar4)) &&
      (pcVar2->damage_target_card == -1))
  {

    iVar3 = count_counters(iVar4, card_00, COUNTER_ECHO);

    num = pcVar2->info_slot;
    if (iVar3 < pcVar2->info_slot)
    {
      num = iVar3;
    }
    if (0 < num)
    {
      remove_counters_from_source(iVar4, card_00, iVar4, card_00, COUNTER_ECHO, num);
      pcVar2->info_slot_fn = pcVar2->info_slot_fn + -num;
    }
  }
  if (current_turn == player)
  {
    iVar4 = upkeep_trigger(player, card, event);
    if ((iVar4 != 0) && (pcVar1->targets[1].player == 1))
    {
      kill_card(player, card, KILL_EXILE);
    }
  }
  if (event == EVENT_CLEANUP)
  {
    pcVar1->targets[1].player = 1;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2392a80; original cards/mirage.c:9007. */
int card_soul_echo(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      if ((player == 1) && (x_value < 3))
      {
        ai_modifier = ai_modifier + -0x24;
      }
      instance = get_card_instance(player, card);
      instance->info_slot = x_value;
    }
    instance = get_card_instance(player, card);
    enters_the_battlefield_with_counters(player, card, EVENT_CAST_SPELL, COUNTER_ECHO,
                                         instance->info_slot);
    cannot_lose_the_game_for_having_less_than_0_life_2022(player, card, EVENT_CAST_SPELL, player);
    upkeep_trigger_ability(player, card, EVENT_CAST_SPELL, player);
  }
  else
  {
    instance = get_card_instance(player, card);
    enters_the_battlefield_with_counters(player, card, event, COUNTER_ECHO, instance->info_slot);
    cannot_lose_the_game_for_having_less_than_0_life_2022(player, card, event, player);
    upkeep_trigger_ability(player, card, event, player);
    if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    {
      result = count_counters(player, card, COUNTER_ECHO);
      if (result < 1)
      {
        kill_card(player, card, KILL_SACRIFICE);
      }
      else
      {

        result =
            do_dialog(1 - player, player, card, -1, -1, " Remove a counter per 1 damage\n Pass",
                      (unsigned int)(1 < life[player]));
        if (result == 0)
        {

          result = create_legacy_effect(player, card, effect_soul_echo);
          instance = get_card_instance(player, result);
          instance->targets[0].player = player;
          instance->targets[0].card = card;
          instance->number_of_targets = '\x01';
        }
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x238f1f0; original cards/mirage.c:7882. */
int card_mind_harness(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  unsigned int uVar4;
  const char *prompt;
  target_definition_t td;

  cumulative_upkeep(player, card, event, 1, 0, 0, 0, 0, 0);
  instance = in_play(player, card);
  if (instance == NULL)
  {
  LAB_0238f274:
    leaves_play_2022(player, card, event, 0, 0);
    if (event != EVENT_CAN_CAST)
    {
      if (event == EVENT_CAST_SPELL)
      {
        if ((card != affected_card) || (player != affected_card_controller))
          goto LAB_0238f318;
        default_target_definition(player, card, &td, 2);
        td.required_color = get_sleighted_color_test(player, card, 0x18);
        goto LAB_0238f471;
      }
      if ((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET))
        goto LAB_0238f2ab;
    }
  }
  else
  {

    instance = get_card_instance(player, card);
    if ((event != EVENT_STATIC_EFFECTS) || (instance->damage_target_player < '\0'))
      goto LAB_0238f274;
    uVar3 = get_color((int)instance->damage_target_player, instance->damage_target_card);
    uVar4 = get_sleighted_color_test((int)instance->damage_target_player,
                                     instance->damage_target_card, 0x18);
    if ((uVar3 & uVar4) == 0)
    {
      kill_card(player, card, KILL_STATE_BASED_ACTION);
    }
    leaves_play_2022(player, card, EVENT_STATIC_EFFECTS, 0, 0);
  LAB_0238f2ab:
    if (event != EVENT_CHANGE_TARGET)
    {
    LAB_0238f318:
      if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) == EVENT_CAN_MOVE_AURA) ||
           (event - EVENT_POWER < 3)) ||
          (event == EVENT_RESOLVE_MOVING_AURA))
      {
      LAB_0238f430:
        default_target_definition(player, card, &td, 2);
        td.required_color = get_sleighted_color_test(player, card, 0x18);
        if (event != EVENT_CAST_SPELL)
          goto LAB_0238f2eb;
      LAB_0238f471:
        if ((card == affected_card) && (player == affected_card_controller))
        {
          prompt =
              get_sleighted_color_text2(player, card, "Select target %s or %s creature.", 4, 3);
          result = generic_spell(player, card, EVENT_CAST_SPELL, 0x281, &td, prompt, 1, NULL);
          return result;
        }
        goto LAB_0238f2eb;
      }
      if ((event != EVENT_CHANGE_TYPE) && (event != EVENT_SET_COLOR))
      {
        if (((event & ~EVENT_UPKEEP) == EVENT_GET_SELECTED_CARD) || (event == EVENT_STATIC_EFFECTS))
          goto LAB_0238f430;
        if (event != EVENT_LEAVES_PLAY_ABILITY)
        {
          return 0;
        }
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.required_color = get_sleighted_color_test(player, card, 0x18);
LAB_0238f2eb:
  result = generic_stealing_aura(player, card, event, &td, "TARGET_CREATURE");
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2386090; original cards/mirage.c:4993. */
int card_suqata_firewalker(int player, int card, event_t event)
{
  int result;
  unsigned int uVar2;
  unsigned int uVar3;

  special_abilities2(player, card, event, SP_KEYWORD2_PROT_MISC);
  if ((((event == EVENT_PROT_MISC) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (((unsigned int)event_result >> 24 & 0xff) == '\x05'))
  {
    uVar2 = get_color(attacking_card_controller, attacking_card);
    uVar3 = get_sleighted_color_test(player, card, 0x10);
    if ((uVar2 & uVar3) != 0)
    {
      event_result = event_result | 1;
    }
  }
  result = card_prodigal_sorcerer(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x238c0a0; original cards/mirage.c:6878. */
int card_decomposition(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  const char *prompt;
  target_definition_t td;

  instance = get_card_instance(player, card);
  if (((instance->damage_target_player < '\0') ||
       (instance = in_play(player, card), instance == NULL)) ||
      (result = is_humiliated_for_ability(player, card, AL_TRIG), result != 0))
  {
  LAB_0238c0c5:
    default_target_definition(player, card, &td, 2);
    td.required_color = get_sleighted_color_test(player, card, 2);
    td.preferred_controller = 1 - player;
    result = attached_creature_dies_trigger_for_controller(player, card, event,
                                                           RESOLVE_TRIGGER_MANDATORY);
    if (result != 0)
    {
    LAB_0238c2d0:
      instance = get_card_instance(player, card);
      lose_life_2022((int)instance->damage_target_player, 2);
    }
    if (event == EVENT_CAN_CAST)
      goto LAB_0238c148;
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_0238c148;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_0238c148;
  }
  else
  {
    instance = get_card_instance(player, card);
    upkeep_trigger_ability(player, card, event, (int)instance->damage_target_player);
    if (event != EVENT_UPKEEP_TRIGGER_ABILITY)
      goto LAB_0238c0c5;
    instance = get_card_instance(player, card);
    if (instance->damage_target_player == current_turn)
    {

      instance = get_card_instance(player, card);
      cumulative_upkeep_general((int)instance->damage_target_player, instance->damage_target_card,
                                EVENT_UPKEEP_TRIGGER_ABILITY, can_pay_1_life_thunk,
                                lose_1_life_thunk);
    }
    default_target_definition(player, card, &td, 2);
    td.required_color = get_sleighted_color_test(player, card, 2);
    td.preferred_controller = 1 - player;
    result = attached_creature_dies_trigger_for_controller(
        player, card, EVENT_UPKEEP_TRIGGER_ABILITY, RESOLVE_TRIGGER_MANDATORY);
    if (result != 0)
      goto LAB_0238c2d0;
  }
  if (((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
      (((((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)) &&
          (event != EVENT_CHANGE_TYPE)) &&
         ((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)))) &&
        (event != EVENT_STATIC_EFFECTS))))
  {
    return 0;
  }
LAB_0238c148:
  prompt = get_sleighted_color_text(player, card, "Select target %s creature.", 1);
  result = targeted_aura_custom_prompt(player, card, event, &td, prompt);
  return result;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2389300; original cards/mirage.c:6048. */
int card_armor_of_thorns(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int uVar2;
  int result;
  bool bVar4;
  target_definition_2022_t td;

  if (event == EVENT_MODIFY_COST)
  {
    result = can_sorcery_be_played(player, EVENT_MODIFY_COST);
    if (result == 0)
    {
      infinite_casting_cost();
    }
    instance = get_card_instance(player, card);
    if (instance->targets[3].player == 0x8d)
      goto LAB_023894ac;
  }
  else
  {
    if (event == EVENT_SHOULD_AI_PLAY)
    {
      result = should_ai_play(player, card);
      return result;
    }
    if (event == EVENT_CAN_CAST)
    {
      default_target_definition_2022(player, card, &td, 2);
      td.illegal_color = get_sleighted_color_test(player, card, 2);
      result = basic_spell(player, card, EVENT_CAN_CAST);
      if (result == 0)
      {
        return 0;
      }
      result = can_target_2022(&td);
      return (unsigned int)(result != 0);
    }
    if (event == EVENT_CAST_SPELL)
    {
      if (((card == affected_card) && (player == affected_card_controller)) &&
          (result = can_sorcery_be_played(player, EVENT_CAST_SPELL), result == 0))
      {

        instance = get_card_instance(player, card);
        result = current_turn;
        bVar4 = current_phase == 0x1f;
        instance->targets[3].player = 0x8d;
        instance->targets[3].card = result;
        if (bVar4)
        {
          instance->targets[3].card = 1 - result;
        }
      }
      instance = get_card_instance(player, card);
      if (instance->targets[3].player == 0x8d)
        goto LAB_023894ac;
    LAB_02389410:
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_02389368;
    }
    else
    {
      instance = get_card_instance(player, card);
      if (instance->targets[3].player == 0x8d)
      {
      LAB_023894ac:
        instance = get_card_instance(player, card);
        if ((instance->targets[3].card == current_turn) && (current_phase == 0x22))
        {
          kill_card(player, card, KILL_SACRIFICE);
        }
        if (event == EVENT_CAST_SPELL)
          goto LAB_02389410;
      }
      if ((event == EVENT_RESOLVE_SPELL) ||
          ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
        goto LAB_02389368;
    }
  }
  if (((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
      (((((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)) &&
          (event != EVENT_CHANGE_TYPE)) &&
         ((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)))) &&
        (event != EVENT_STATIC_EFFECTS))))
  {
    return 0;
  }
LAB_02389368:
  attached_gets_pt(player, card, event, 2, 2);
  default_target_definition_2022(player, card, &td, 2);
  uVar2 = get_sleighted_color_test(player, card, 2);
  td.illegal_color = td.illegal_color | uVar2;
  result = aura(player, card, event, &td, "CALL_TO_SERVE");
  return result;
}

/* ManalinkEh 2022: 0x238ba20; original cards/mirage.c:6717. */
int card_consuming_ferocity(int player, int card, event_t event)
{
  char cVar1;
  card_instance_t *instance;
  int iVar3;
  int t_player;
  int iVar4;
  target_definition_2022_t td;

  instance = in_play(player, card);
  if ((instance == NULL) ||
      (instance = get_card_instance(player, card), instance->damage_target_player == -1))
  {
  LAB_0238ba58:
    if (event == EVENT_CAN_CAST)
      goto LAB_0238ba78;
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_0238ba78;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_0238ba78;
  }
  else
  {

    instance = get_card_instance(player, card);
    cVar1 = instance->damage_target_player;
    iVar3 = instance->damage_target_card;
    upkeep_trigger_ability(player, card, event, player);
    if (event != EVENT_UPKEEP_TRIGGER_ABILITY)
      goto LAB_0238ba58;
    t_player = (int)cVar1;
    add_counters_from_source(player, card, t_player, iVar3, COUNTER_P1_P0, 1);
    iVar4 = count_counters(t_player, iVar3, COUNTER_P1_P0);
    if (2 < iVar4)
    {
      iVar4 = get_power(t_player, iVar3);
      damage_player(t_player, iVar4, t_player, iVar3);
      kill_card(t_player, iVar3, KILL_BURY);
    }
  }
  if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
       (((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)) &&
         (event != EVENT_CHANGE_TYPE)))) &&
      (((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)) &&
        (event != EVENT_STATIC_EFFECTS))))
  {
    return 0;
  }
LAB_0238ba78:
  attached_gets_pt(player, card, event, 1, 0);
  default_target_definition_2022(player, card, &td, 2);
  td.preferred_controller = player;
  td.required_subtype = 0xd1;
  td.special = 0x1000;
  iVar3 = aura(player, card, event, &td, "CONSUMING_FEROCITY");
  return iVar3;
}
#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2376a50; original cards/mirage.c:6129. */
static char *is_blocking_kod(int who_chooses, int player, int card, int targeting_player,
                             int targeting_card)
{
  int result;

  result = is_blocking(card, targeting_card);
  if (result == 0)
  {
    return "must be blocking Barbed-Back Wurm";
  }
  return NULL;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2393b20; original cards/mirage.c:9377. */
int card_ward_of_lights(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  bool bVar3;

  if (event == EVENT_MODIFY_COST)
  {
    result = can_sorcery_be_played(player, EVENT_MODIFY_COST);
    if (result == 0)
    {
      infinite_casting_cost();
    }
  }
  else if ((((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
            (player == affected_card_controller)) &&
           (result = can_sorcery_be_played(player, EVENT_CAST_SPELL), result == 0))
  {

    instance = get_card_instance(player, card);
    result = current_turn;
    bVar3 = current_phase == 0x1f;
    instance->targets[3].player = 0x8d;
    instance->targets[3].card = result;
    if (bVar3)
    {
      instance->targets[3].card = 1 - result;
    }
    ai_modifier = ai_modifier + -0x10;
  }
  instance = get_card_instance(player, card);
  if (((instance->targets[3].player == 0x8d) &&
       (instance = get_card_instance(player, card), instance->targets[3].card == current_turn)) &&
      (current_phase == 0x22))
  {
    kill_card(player, card, KILL_SACRIFICE);
  }
  result = this_effect_doesnt_remove_this(player, card, event);
  if (result != 0)
  {
    return 0;
  }
  if (event == EVENT_ABILITIES)
  {

    instance = in_play(player, card);

    if (((instance != NULL) && (-1 < instance->damage_target_card)) &&
        ((result = is_humiliated_for_ability(player, card, AL_STATIC),
          result == 0 && ((instance->damage_target_card == affected_card &&
                           (instance->damage_target_player == affected_card_controller))))))
    {
      event_result = event_result | instance->targets[1].card;
    }
  LAB_02393ce8:
    if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
        (event == EVENT_CHANGE_TARGET))
      goto LAB_02393d01;
  }
  else
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      instance = get_card_instance(player, card);
      result = select_a_protection(player);
      instance->targets[1].card = result;
      goto LAB_02393d01;
    }
    if (event == EVENT_CAN_CAST)
      goto LAB_02393d01;
    if (event != EVENT_CAST_SPELL)
      goto LAB_02393ce8;
    if ((card == affected_card) && (player == affected_card_controller))
      goto LAB_02393d01;
  }
  if (((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
        ((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)))) &&
       (event != EVENT_CHANGE_TYPE)) &&
      (((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)) &&
        (event != EVENT_STATIC_EFFECTS))))
  {
    return 0;
  }
LAB_02393d01:
  result = aura(player, card, event, NULL, NULL);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2377110; original cards/mirage.c:9110. */
static int effect_sacrifice_at_next_eot(int player, int card, event_t event)
{
  char cVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  instance = get_card_instance(player, card);
  if (-1 < instance->damage_target_player)
  {
    instance = get_card_instance(player, card);
    if ((((trigger_condition == TRIGGER_EOT) && (card == affected_card)) &&
         (player == affected_card_controller)) &&
        (player == reason_for_trigger_controller))
    {
      iVar2 = instance->targets[1].player;
      if (iVar2 < 1)
      {
        if (event == EVENT_TRIGGER)
        {
          event_result = event_result | 2;
        }
        else if (event == EVENT_RESOLVE_TRIGGER)
        {
          cVar1 = instance->damage_target_player;
          iVar2 = instance->damage_target_card;
          iVar4 = check_state((int)cVar1, iVar2, STATE_OUBLIETTED);
          if (iVar4 == 0)
          {
            kill_card((int)cVar1, iVar2, KILL_SACRIFICE);
          }
          else
          {
            kill_card(player, card, KILL_EXILE);
          }
        }
      }
      else
      {
        instance->targets[1].player = iVar2 + -1;
      }
    }
  }
  return 0;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2390c80; original cards/mirage.c:8440. */
int card_sapphire_charm(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int iVar2;
  target_definition_2022_t *td_00;
  int iVar3;
  card_instance_t *pcVar4;
  int iVar5;
  int iVar6;
  uint32_t uVar7;
  bool bVar8;
  const char *prompt;
  unsigned int uStack_118;
  target_definition_2022_t td;
  target_definition_2022_t td2;
  target_definition_2022_t td3;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition_2022(player, card, &td, 0);
    td.zone = 0x1000;
    td.preferred_controller = player;
    default_target_definition_2022(player, card, &td2, 2);
    td2.preferred_controller = 2;
    default_target_definition_2022(player, card, &td3, 2);
    td3.allowed_controller = 1 - player;
    td3.preferred_controller = td3.allowed_controller;
    get_card_instance(player, card);
    iVar3 = generic_spell_2022(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    if (((iVar3 == 0) &&
         (iVar3 = generic_spell_2022(player, card, EVENT_CAN_CAST, 1, &td2, NULL, 1, NULL),
          iVar3 == 0)) &&
        (iVar3 = generic_spell_2022(player, card, EVENT_CAN_CAST, 1, &td3, NULL, 1, NULL),
         iVar3 == 0))
    {
      return 0;
    }
    return 1;
  }
  if (event != EVENT_CAST_SPELL)
  {
    if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
        (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
    default_target_definition_2022(player, card, &td, 0);
    td.zone = 0x1000;
    td.preferred_controller = player;
    default_target_definition_2022(player, card, &td2, 2);
    td2.preferred_controller = 2;
    default_target_definition_2022(player, card, &td3, 2);
    td3.allowed_controller = 1 - player;
    td3.preferred_controller = td3.allowed_controller;
    pcVar1 = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {

      iVar3 = pcVar1->info_slot;
      if (iVar3 == 2)
      {
        iVar3 = valid_target_2022(&td2);
        if (iVar3 != 0)
        {
          iVar3 = pump_ability_until_eot_2022(player, card, pcVar1->targets[0].player,
                                              pcVar1->targets[0].card, 0, 0, 0x20, 0);
          alternate_legacy_text(2, player, iVar3);
        }
      }
      else if (iVar3 == 3)
      {
        iVar3 = valid_target_2022(&td3);
        if (iVar3 != 0)
        {
          phase_out_impl(pcVar1->targets[0].player, pcVar1->targets[0].card, 0x4000000, 0);
        }
      }
      else if ((iVar3 == 1) && (iVar3 = valid_target_2022(&td), iVar3 != 0))
      {

        iVar3 = create_legacy_effect(player, card, cantrip_legacy);
        iVar3 = alternate_legacy_text(1, player, iVar3);
        pcVar4 = get_card_instance(player, iVar3);
        bVar8 = current_phase == 4;
        pcVar4->targets[1].player = pcVar1->targets[0].player;
        pcVar4->targets[1].card = 1;
        if (bVar8)
        {
          remove_state(player, iVar3, 0x100);
        }
      }
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
  default_target_definition_2022(player, card, &td, 0);
  td.zone = 0x1000;
  td.preferred_controller = player;
  default_target_definition_2022(player, card, &td2, 2);
  td2.preferred_controller = 2;
  default_target_definition_2022(player, card, &td3, 2);
  iVar3 = 1 - player;
  td3.allowed_controller = iVar3;
  td3.preferred_controller = iVar3;
  pcVar1 = get_card_instance(player, card);
  if (card != affected_card)
  {
    return 0;
  }
  if (player != affected_card_controller)
  {
    return 0;
  }
  pcVar1->number_of_targets = '\0';
  iVar2 = check_special_flags2_2022(player, card, -0x80000000);
  if (iVar2 != 0)
    goto LAB_02390ebe;

  iVar2 = generic_spell_2022(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
  iVar5 = generic_spell_2022(player, card, EVENT_CAN_CAST, 1, &td2, NULL, 1, NULL);
  iVar6 = generic_spell_2022(player, card, EVENT_CAN_CAST, 1, &td3, NULL, 1, NULL);
  uStack_118 = 0;
  if (current_turn == iVar3)
  {
    if (current_phase == 0x14)
    {
      uVar7 = 10;
      if (current_turn != player)
        goto LAB_023913b8;
      uStack_118 = 1;
    }
    else
    {
    LAB_023913ab:
      uVar7 = 5;
      if (current_turn == iVar3)
      {
      LAB_023913b8:
        uStack_118 = (unsigned int)(0x15 < current_phase);
      }
    }
  }
  else if (current_turn == player)
  {
    if (current_phase != 0x14)
      goto LAB_023913ab;
    uVar7 = 5;
    uStack_118 = 1;
  }
  else
  {
    uVar7 = 5;
  }
  iVar3 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "Draw a card.", iVar2, 5,
                      "Give flying.", iVar5, uStack_118, "Target crit phase out.", iVar6, uVar7, 0,
                      0, 0, 0);
  if (iVar3 == 0)
  {
    spell_fizzled = 1;
    return 0;
  }
LAB_02390ebe:
  iVar3 = pcVar1->info_slot;
  if (iVar3 == 1)
  {
    prompt = "Target player draws a card at the beginning of the next turn\'s upkeep.";
    td_00 = &td;
  }
  else
  {
    if (iVar3 == 2)
    {
      generic_spell_2022(player, card, EVENT_CAST_SPELL, 0x81, &td2,
                         "Select target creature to give flying.", 1, NULL);
      iVar3 = pcVar1->info_slot;
    }
    if (iVar3 != 3)
    {
      return 0;
    }
    prompt = "Select target creature an opponent controls it phases out.";
    td_00 = &td3;
  }
  iVar3 = generic_spell_2022(player, card, EVENT_CAST_SPELL, 0x81, td_00, prompt, 1, NULL);
  return iVar3;
}

/* ManalinkEh 2022: 0x23761b0; original cards/mirage.c:9143. */
static void sacrifice_token_at_next_eot(token_generation_2022_t *token, int blue_wall, int number)
{
  int card;
  card_instance_t *instance;

  card = create_targetted_legacy_effect(token->s_player, token->s_card,
                                        effect_sacrifice_at_next_eot, token->t_player, blue_wall);
  if ((current_phase == 0x1f) && (-1 < card))
  {
    instance = get_card_instance(token->s_player, card);
    instance->targets[1].player = 1;
    return;
  }
  return;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x23897c0; original cards/mirage.c:6137. */
int card_barbed_back_wurm(int player, int card, event_t event)
{
  card_instance_t *instance;
  const char *prompt;
  int result;
  target_definition_2022_t td2;

  if (event == EVENT_ACTIVATE)
  {
    default_target_definition_2022(player, card, &td2, 2);
    td2.allowed_controller = 1 - player;
    td2.preferred_controller = td2.allowed_controller;
    td2.required_color = get_sleighted_color_test(player, card, 8);
    td2.special = 0x8000;
    td2.extra = (int32_t)is_blocking_kod;
    get_card_instance(player, card);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition_2022(player, card, &td2, 2);
    td2.allowed_controller = 1 - player;
    td2.preferred_controller = td2.allowed_controller;
    td2.required_color = get_sleighted_color_test(player, card, 8);
    td2.special = 0x8000;
    td2.extra = (int32_t)is_blocking_kod;
    instance = get_card_instance(player, card);
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target_2022(&td2), result != 0))
    {
      pump_until_eot_2022(instance->parent_controller, instance->parent_card,
                          instance->targets[0].player, instance->targets[0].card, -1, -1);
    }
  }
  prompt = get_sleighted_color_text(player, card,
                                    "Select target %s creature blocking Barbed-Back Wurm.", 3);
  result = generic_activated_ability_2022(player, card, event, 6, 0, 1, 0, 0, 0, 0, 0, &td2,
                                          (char *)prompt);
  return result;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 native scalar-word handlers. */
int card_jungle_troll(int player, int card, event_t event);
void phase_in_impl_2022(int player, int card, int sf2, int mode);
void phase_in_2022(int player, int card);
int phase_out_impl_2022(int player, int card, int sf2, int mode);
int phase_out_2022(int player, int card);

/* ManalinkEh 2022: 0x238e250; original cards/mirage.c:7559. */
int card_jungle_troll(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  if (event == EVENT_CAN_ACTIVATE)
  {
    iVar2 = has_mana_for_activated_ability_2022(player, card, 0, 0, 0, 0, 1, 0);
    if ((iVar2 != 0) ||
        (iVar2 = has_mana_for_activated_ability_2022(player, card, 0, 0, 0, 1, 0, 0), iVar2 != 0))
    {
      iVar2 = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, GAA_REGENERATION, 0,
                                             0, 0, 0, 0, 0, 0, NULL, NULL);
      return iVar2;
    }
  }
  else if (event == EVENT_ACTIVATE)
  {
    iVar2 = generic_activated_ability_2022(player, card, EVENT_ACTIVATE, GAA_REGENERATION, 0, 0, 0,
                                           0, 0, 0, 0, NULL, NULL);
    if (iVar2 != 0)
    {
      iVar2 = has_mana_for_activated_ability_2022(player, card, 0, 0, 0, 1, 0, 0);
      iVar3 = has_mana_for_activated_ability_2022(player, card, 0, 0, 0, 0, 1, 0);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\004", "\001\016",
                          "Pay R to regenerate.", iVar3, 1, "Pay G to regenerate.", iVar2, 1, 0, 0,
                          0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 == 1)
      {
        iVar3 = 1;
        iVar2 = 0;
      }
      else
      {
        if (iVar2 != 2)
        {
          return 0;
        }
        iVar3 = 0;
        iVar2 = 1;
      }
      charge_mana_for_activated_ability_2022(player, card, 0, 0, 0, iVar2, iVar3, 0);
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    iVar2 = can_regenerate(instance->parent_controller, instance->parent_card);
    if (iVar2 != 0)
    {
      regenerate_target(instance->parent_controller, instance->parent_card);
      return 0;
    }
  }
  return 0;
}


/* ManalinkEh 2022: 0x2377620; original cards/mirage.c:53. */
void phase_in_impl_2022(int player, int card, int sf2, int mode)
{
  card_instance_t *pcVar1;
  int iVar2;
  card_instance_t *pcVar3;
  int iVar4;
  unsigned int uVar5;
  int card_00;
  target_definition_2022_t td;

  pcVar1 = get_card_instance(player, card);
  if (pcVar1->internal_card_id < 1)
  {
    return;
  }
  if ((pcVar1->state & (STATE_OUBLIETTED | STATE_INVISIBLE | STATE_IN_PLAY)) !=
      (STATE_OUBLIETTED | STATE_IN_PLAY))
  {
    return;
  }
  iVar2 = check_special_flags_2022(player, card, sf2);
  if (iVar2 != sf2)
  {
    return;
  }
  iVar2 = check_special_flags3_2022(player, card, SF3_CANNOT_PHASE_IN);
  if (iVar2 != 0)
  {
    return;
  }
  if (mode == 1)
  {
    set_special_flags_2022(player, card, (SF_PHASED_OUT | SF_PHASED_OUT_INDIRECTLY));
  LAB_023777ca:
    iVar2 = 0;
    do
    {
      card_00 = 0;
      if (0 < active_cards_count[iVar2])
      {
        do
        {
          pcVar1 = get_card_instance(iVar2, card_00);
          if ((((0 < pcVar1->internal_card_id) &&
                ((pcVar1->state & (STATE_OUBLIETTED | STATE_INVISIBLE | STATE_IN_PLAY)) ==
                 (STATE_OUBLIETTED | STATE_IN_PLAY))) &&
               (iVar4 = check_special_flags_2022(iVar2, card_00, SF_PHASED_OUT_INDIRECTLY),
                iVar4 != 0)) &&
              ((pcVar1->damage_target_player == player && (pcVar1->damage_target_card == card))))
          {
            phase_in_impl_2022(iVar2, card_00, SF_PHASED_OUT_INDIRECTLY, mode);
          }
          card_00 = card_00 + 1;
        } while (card_00 < active_cards_count[iVar2]);
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 != 2);
    if (mode == 1)
    {
      return;
    }
  }
  else
  {
    play_sound_effect(WAV_DESTROY);
    pcVar1->state = pcVar1->state & 0xff7fffff;
    remove_special_flags_2022(player, card, (SF_PHASED_OUT | SF_PHASED_OUT_INDIRECTLY));
    iVar2 = get_id(player, card);
    if (iVar2 == 0xa46)
    {
      draw_cards(player, 1);
    }
    iVar2 = get_id(player, card);
    if (iVar2 == 0xa62)
    {
      add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
    }
    iVar2 = get_id(player, card);
    if (iVar2 == 0xad6)
    {
      default_target_definition_2022(player, card, &td, 2);
      td.allow_cancel = 0;
      iVar2 = can_target_2022(&td);
      if ((iVar2 != 0) && (iVar2 = pick_target_2022(&td, "TARGET_CREATURE_PHASEOUT"), iVar2 != 0))
      {
        phase_out_impl_2022(pcVar1->targets[0].player, pcVar1->targets[0].card, 0x4000000, 0);
      }
    }
    if (mode != 2)
      goto LAB_023777ca;
  }
  pcVar1 = get_card_instance(player, card);
  if (pcVar1->damage_target_player < '\0')
  {
    return;
  }
  if (pcVar1->damage_target_card < 0)
  {
    return;
  }
  pcVar3 = get_card_instance((int)pcVar1->damage_target_player, pcVar1->damage_target_card);
  if ((pcVar3->internal_card_id < 0) || (uVar5 = pcVar3->state, (uVar5 & 0x22) != 2))
    goto LAB_0237779a;
  if (mode == 0)
  {
    uVar5 = uVar5 & STATE_OUBLIETTED;
  joined_r0x02377964:
    if (uVar5 == 0)
    {
      return;
    }
  }
  else
  {
    if ((uVar5 & STATE_OUBLIETTED) == 0)
    {
      uVar5 =
          check_special_flags_2022((int)pcVar1->damage_target_player, pcVar1->damage_target_card,
                                   (SF_PHASED_OUT | SF_PHASED_OUT_INDIRECTLY));
      goto joined_r0x02377964;
    }
    iVar2 = check_special_flags_2022((int)pcVar1->damage_target_player, pcVar1->damage_target_card,
                                     (SF_PHASED_OUT | SF_PHASED_OUT_INDIRECTLY));
    if (iVar2 == (SF_PHASED_OUT | SF_PHASED_OUT_INDIRECTLY))
    {
      return;
    }
  }
  uVar5 = pcVar1->state;
  iVar2 = is_token((int)pcVar1->damage_target_player, pcVar1->damage_target_card);
  if ((iVar2 == 0) && ((uVar5 & STATE_OUBLIETTED) != 0))
  {
    return;
  }
LAB_0237779a:
  pcVar1 = get_card_instance(player, card);
  pcVar1->damage_target_card = -1;
  pcVar1->damage_target_player = -1;
  return;
}


/* ManalinkEh 2022: 0x2377970; original cards/mirage.c:109. */
void phase_in_2022(int player, int card)
{
  phase_in_impl_2022(player, card, SF_PHASED_OUT, 0);
  return;
}


/* ManalinkEh 2022: 0x2377450; original cards/mirage.c:113. */
int phase_out_impl_2022(int player, int card, int sf2, int mode)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  instance = in_play(player, card);
  if ((instance != NULL) && (iVar2 = check_special_flags_2022(player, card, 0x400), iVar2 == 0))
  {
    instance = get_card_instance(player, card);
    if (mode != 1)
    {
      play_sound_effect(WAV_DESTROY);
      instance->state = instance->state | STATE_OUBLIETTED;
      iVar2 = get_id(player, card);
      if (iVar2 == 0xa46)
      {
        discard(player, 0, player);
      }
      iVar2 = get_id(player, card);
      if (iVar2 == 0xb98)
      {
        mill(player, 3);
      }
    }
    if ((sf2 & (uint32_t)(SF_PHASED_OUT | SF_PHASED_OUT_INDIRECTLY)) ==
        (SF_PHASED_OUT | SF_PHASED_OUT_INDIRECTLY))
    {
      sf2 = sf2 & 0xfbffffff;
    }
    iVar2 = 0;
    set_special_flags_2022(player, card, sf2);
    iVar3 = mode;
    while (iVar3 != 2)
    {
      iVar3 = 0;
      if (0 < active_cards_count[iVar2])
      {
        do
        {
          instance = in_play(iVar2, iVar3);
          if ((((instance != NULL) && (instance = get_card_instance(iVar2, iVar3),
                                       player == instance->damage_target_player)) &&
               (card == instance->damage_target_card)) &&
              (-1 < (char)cards_data[instance->internal_card_id].type))
          {
            phase_out_impl_2022(iVar2, iVar3, SF_PHASED_OUT_INDIRECTLY, mode);
          }
          iVar3 = iVar3 + 1;
        } while (iVar3 < active_cards_count[iVar2]);
      }
      iVar2 = iVar2 + 1;
      iVar3 = iVar2;
    }
    return 1;
  }
  return 0;
}


/* ManalinkEh 2022: 0x2377a70; original cards/mirage.c:160. */
int phase_out_2022(int player, int card)
{
  int result;
  result = phase_out_impl_2022(player, card, SF_PHASED_OUT, 0);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2392f60; original cards/mirage.c:9152. */
int card_tidal_wave(int player, int card, event_t event)
{
  int result;
  token_generation_2022_t token;
  if (event == EVENT_RESOLVE_SPELL)
  {
    default_token_definition_2022(player, card, 0x49a3, &token);
    token.pow = 5;
    token.tou = 5;
    token.color_forced = '\x04';
    token.key_plus = KEYWORD_DEFENDER;
    token.special_code_on_generation = sacrifice_token_at_next_eot;
    generate_token_2022(&token);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell_2022(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x238f920; original cards/mirage.c:7991. */
int card_prismatic_circle(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_2022_t td;

  cumulative_upkeep(player, card, event, 1, 0, 0, 0, 0, 0);
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = choose_a_color_and_show_legacy(player, card, 1 - player, -1);
    instance = get_card_instance(player, card);
    instance->info_slot = 1U << ((unsigned char)result & 0x1f);
  }
  else if ((((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
            (event == EVENT_RESOLVE_ACTIVATION)) ||
           (event == EVENT_CLEANUP))
  {
    default_target_definition_2022(player, card, &td, 0x80);
    td.special = 0x8000;
    td.extra = (int32_t)colored_damage_source_that_damages_me_2022;
    instance = get_card_instance(player, card);
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target_2022(&td), result != 0))
    {
      prevent_damage(instance->targets[0].player, instance->targets[0].card, -1);
    }
    result = generic_activated_ability_2022(player, card, event, 0x1000004, 1, 0, 0, 0, 0, 0, 0,
                                            &td, "TARGET_DAMAGE");
    return result;
  }
  result = global_enchantment(player, card, event);
  return result;
}

#endif
