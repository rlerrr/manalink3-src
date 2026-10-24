#include "manalink.h"

void default_token_definition(int player, int card, int id, token_generation_t *token)
{
  token->id = id;
  token->eff_player = player;
  token->eff_card = card;
  token->t_player = player;

  if (player >= 0 && card >= 0)	// Find the real source if this was given either an activation card or an effect card
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  if (instance->internal_card_id == activation_card)
		{
		  player = instance->parent_controller;
		  card = instance->parent_card;
		}
	  else if (instance->internal_card_id >= 0)	// No need to check backup_internal_card_id; if it's tokens from an effect card that's already left play, it's always a bug
		{
		  int csvid = cards_data[instance->internal_card_id].id;
		  if (csvid >= 901 && csvid <= 908)
			{
			  player = instance->damage_source_player;
			  card = instance->damage_source_card;
			}
		}
	}
  if (player >= 0 && card >= 0)
	{
	  token->s_player = player;
	  token->s_card = card;
	  token->no_sleight = 0;
	}
  else
	{
	  token->s_player = token->s_card = -1;
	  token->no_sleight = 1;
	}

  token->qty = 1;
  token->pow = -1;
  token->tou = -1;
  token->key_plus = 0;
  token->s_key_plus = 0;
  token->color_forced = 0;
  token->special_infos = 0;
  token->action = 0;
  token->action_argument = 0;
  token->legacy = 0;
  token->special_code_for_legacy = &empty;
  token->special_code_on_generation = NULL;
  token->keep_track_of_tokens_generated = 0;
  token->special_flags2 = 0;
}

static int get_copyable_special_infos(int t_player, int t_card);

// Create a token definition that's a copy of (t_player,t_card).
void copy_token_definition(int player, int card, token_generation_t* token, int t_player, int t_card)
{
  int csvid = get_id(t_player, t_card);
  default_token_definition(player, card, csvid, token);
  token->no_sleight = 1;

  if (cards_ptr[csvid]->card_type == 8)	// A token defined in manalink.csv
	{
	  card_instance_t* instance = get_card_instance(t_player, t_card);
	  if (check_special_flags(t_player, t_card, SF_DONT_COPY_TOKEN_SOURCE))
		token->s_player = token->s_card = -1;
	  else
		{
		  token->s_player = instance->damage_source_player;
		  token->s_card = instance->damage_source_card;
		}
	  token->pow = instance->targets[5].player;
	  token->tou = instance->targets[5].card;
	  token->key_plus = instance->targets[6].player;
	  token->s_key_plus = instance->targets[7].player;
	  token->color_forced = instance->targets[7].card;
	  token->special_flags2 = instance->targets[8].card;
	  token->special_infos = get_copyable_special_infos(t_player, t_card);
		if( check_special_flags2(t_player, t_card, SF2_ENCHANTED_EVENING) ){
			token->special_flags2 = SF2_ENCHANTED_EVENING;
		}
	}
}

int copy_token_characteristics_for_clone(int player, int card, int t_player, int t_card)
{
  int csvid = get_id(t_player, t_card);
  if (cards_ptr[csvid]->card_type == 8)	// A token defined in manalink.csv
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  card_instance_t* token = get_card_instance(t_player, t_card);

	  int i;
	  for (i = 4; i <= 8; ++i)
		instance->targets[i] = token->targets[i];	// struct copy

	  if (instance->targets[7].card > 0)
		instance->initial_color = instance->targets[7].card & COLOR_TEST_ANY_COLORED;

	  instance->color = instance->initial_color & COLOR_TEST_ANY_COLORED;
	  instance->regen_status |= KEYWORD_RECALC_SET_COLOR;

	  set_special_infos(player, card, get_copyable_special_infos(t_player, t_card));

		if( check_special_flags2(t_player, t_card, SF2_ENCHANTED_EVENING) ){
			set_special_flags2(player, card, SF2_ENCHANTED_EVENING);
		}

	  return 1;
	}
  else
	return 0;
}

/* Just like generic_token(), but supports storing the values on an effect card to set them on a different permanent.  The idea is that changing a permanent to
 * temporarily be a copy of a token shouldn't have to overwrite all of its targets. */
int token_characteristic_setting_effects(int player, int card, event_t event, int t_player, int t_card){
	if (card == -1){
		return 0;
	}
	if (check_special_flags2(player, card, SF2_TEMPORARY_COPY_OF_TOKEN)){
		return 0;
	}
	card_instance_t* instance = get_card_instance(player, card);
	if( event == EVENT_POWER && affect_me(t_player, t_card) && instance->targets[5].player >= 0 ){
		event_result += instance->targets[5].player - get_base_power(player, card);
	}
	if( event == EVENT_TOUGHNESS && affect_me(t_player, t_card) && instance->targets[5].card >= 0 ){
		event_result += instance->targets[5].card - get_base_toughness(player, card);
	}
	if( event == EVENT_ABILITIES && affect_me(t_player, t_card) && instance->targets[6].player > 0 ){
		event_result |= instance->targets[6].player;
	}
	if( event == EVENT_SET_COLOR && affect_me(t_player, t_card) ){
		if (!(card == t_card && player == t_player)){
			if (instance->targets[7].card > 0){
				event_result = instance->targets[7].card & COLOR_TEST_ANY_COLORED;
			}
		}	// otherwise handled by setting initial_color in set_token()
	}
	if( instance->targets[7].player > 0 && instance->targets[7].player > 0 ){
		// Deliberately use the attached-to card as the effect source, not the attachment
		special_abilities(t_player, t_card, event, instance->targets[7].player, t_player, t_card);
	}
	if (event == EVENT_CHANGE_TYPE && affect_me(t_player, t_card) && instance->targets[8].card > 0){
		if (!(land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS)){
			// So there's a chance to set SF2_TEMPORARY_COPY_OF_TOKEN.
			land_can_be_played |= LCBP_NEED_EVENT_CHANGE_TYPE_SECOND_PASS;
		} else {
			set_special_flags2(t_player, t_card, instance->targets[8].card);
		}
	}
	return 0;
}

int generic_token(int player, int card, event_t event){
	return token_characteristic_setting_effects(player, card, event, player, card);
}

static void set_token(int player, int card, token_generation_t *token ){
	card_instance_t *instance = get_card_instance(player, card);
	instance->damage_source_player = token->s_player;
	instance->damage_source_card = token->s_card;
	instance->targets[5].player = token->pow;
	instance->targets[5].card = token->tou;
	instance->targets[6].player = token->key_plus;
	// instance->targets[6].card unused
	instance->targets[7].player = token->s_key_plus;
	// instance->targets[7].card set below
	instance->targets[8].card = token->special_flags2;

	color_test_t forced = token->color_forced;
	int sleight = token->s_player >= 0 && token->s_card >= 0 && !token->no_sleight && !(forced > 0 && (forced & 0x80));
	if (forced <= 0){
		forced = get_card_instance(player, card)->color;
	}
	if (sleight){
		forced = get_sleighted_color_test(token->s_player, token->s_card, forced & COLOR_TEST_ANY_COLORED);
	} else {
		forced &= COLOR_TEST_ANY_COLORED;
	}

	instance->initial_color = forced;
	instance->color = forced;
	instance->regen_status |= KEYWORD_RECALC_SET_COLOR;

	instance->targets[7].card = forced | 0x80;

	set_special_flags2(player, card, token->special_flags2);

	convert_to_token(player, card);
	set_special_infos(player, card, token->special_infos);
}

void convert_to_token(int player, int card){
	card_instance_t *instance = get_card_instance(player, card);
	instance->token_status |= STATUS_TOKEN;
}

static int get_updated_tokens_number(int player, int number)
{
  card_instance_t* instance;
  int p, c;
  for (p = 0; p <= 1; ++p)
	for (c = 0; c < active_cards_count[p]; ++c)
	  if ((instance = in_play(p, c)))
		switch (cards_data[instance->internal_card_id].id)
		  {
			case CARD_ID_DOUBLING_SEASON:		// self only: doubles
			case CARD_ID_PARALLEL_LIVES:
			case CARD_ID_ANOINTED_PROCESSION:
			  if (p != player)
				continue;
			  // else fall through
			case CARD_ID_SELESNYA_LOFT_GARDENS:	// everyone: doubles
			case CARD_ID_PRIMAL_VIGOR:
			  number *= 2;
			  break;
		  }
  return number;
}

static int is_reserved_id(int csvid){
	int iid = get_internal_card_id_from_csv_id(csvid);
	return (cards_data[iid].cc[2] == 3	// vanguard card, edh, etc.
			|| cards_data[iid].type == TYPE_EFFECT);
}

void set_special_infos(int player, int card, int infos){
	card_instance_t *instance = get_card_instance(player, card);
	instance->targets[14].player = infos;
}

int get_special_infos(int player, int card){
	card_instance_t *instance = get_card_instance(player, card);
	if( instance->targets[14].player > -1 ){
		return instance->targets[14].player;
	}
	return 0;
}

static int get_copyable_special_infos(int t_player, int t_card)
{
  /* Augh.  Most special_infos are characteristic of the token, and so should be copied; some are added by the effect putting it into play, and so shouldn't.
   * But many of the latter are on token function that are shared among multiple ids and so are prohibitively difficult to test for; so only allow special_infos
   * that are whitelisted here. */
  int special_infos = get_special_infos(t_player, t_card);
  if (special_infos)
	switch (get_backup_id(t_player, t_card))
	  {
		case CARD_ID_ASSASSIN:
		  if (special_infos != 66)	// Vraska the Unseen assassin: has "Whenever this creature deals combat damage to a player, that player loses the game."
			special_infos = 0;
		  break;
		case CARD_ID_ASSEMBLY_WORKER:
		  if (special_infos != 66)	// Urza's Factory assembly-worker: does *not* have Mishra's Factory assembly-worker abilities
			special_infos = 0;
		  break;
		case CARD_ID_BAT:
		  if (special_infos != 66)	// Sengir Nosferatu bat: has "1B, Sacrifice this creature: Return an exiled card named Sengir Nosferatu to the battlefield under its owner's control."
			special_infos = 0;
		  break;
		case CARD_ID_SPIRIT:
		  if (special_infos != 66)	// Daxos the Returned bird : has "This creature's power and toughness are each equal to the number of experience counters you have."
			special_infos = 0;
		  break;
		case CARD_ID_CLERIC:
		  if (special_infos != 66)	// Deathpact Angel cleric: has "3WBB, T, Sacrifice this creature: Return a card named Deathpact Angel from your graveyard to the battlefield."
			special_infos = 0;
		  break;
		case CARD_ID_DRAGON:
		  if (special_infos != 66 // Dragon Broodmother dragon: has Devour 2
			  && special_infos != 67)	// Dragon Egg dragon: has "R: This creature gets +1/+0 until end of turn."
			special_infos = 0;
		  break;
		case CARD_ID_ELDRAZI_SPAWN:
		  if (special_infos != 66)	// Hedron Fields of Agadeem eldrazi: has annihilator 1; does *not* have eldrazi spawn mana generation.
			special_infos = 0;
		  break;
		case CARD_ID_ELEMENTAL:
		  if (special_infos != 67)	// Voice of Resurgence elemental: has "This creature's power and toughness are each equal to the number of creatures you control."
			special_infos = 0;
		  break;
		case CARD_ID_GOBLIN:
		  if (special_infos != 67)	// Goblin Assault goblin: has haste
			special_infos = 0;
		  break;
		case CARD_ID_ILLUSION_TOKEN:
		  if (special_infos != 66)	// Summoner's Bane illusion: does *not* have flying.
			special_infos = 0;
		  break;
		case CARD_ID_INSECT:
		  if (special_infos != 67)	// Carrion Call/Phyrexian Swarmlord/Trigon of Infestation insect: has infect
			special_infos = 0;
		  break;
		case CARD_ID_OOZE:
		  if (special_infos != 66	// Gutter Grime ooze: has "This creature's power and toughness are each equal to the number of slime counters on Gutter Grime."
			  && special_infos != 67)	// Mitotic Slime ooze: has "When this creature dies, put two 1/1 green Ooze creature tokens onto the battlefield."
			special_infos = 0;
		  break;
		case CARD_ID_SAPROLING:
		  if (special_infos != 66)	// Saproling Burst saproling: has "This creature's power and toughness are each equal to the number of fade counters on Saproling Burst."
			special_infos = 0;
		  break;
		case CARD_ID_WOLF:
		  if (special_infos != 66)	// Sound the Call wolf: has "This creature gets +1/+1 for each card named Sound the Call in each graveyard."
			special_infos = 0;
		  break;
		default:
		  special_infos = 0;
	  }
  return special_infos;
}

static int real_generate_token(int player, int int_id, token_generation_t *token, int delay_put_into_play){
	int card_added = add_card_to_hand( player, int_id );
	set_token(player, card_added, token);
	if (delay_put_into_play){
		get_card_instance(player, card_added)->state |= STATE_INVISIBLE;
		--hand_count[player];
	} else {
		put_into_play(player, card_added);
	}
	return card_added;
}

void generate_token(token_generation_t *token){
	int int_id = get_internal_card_id_from_csv_id(token->id);
	if(!(cards_data[int_id].type & TYPE_ARTIFACT) && (token->action & TOKEN_ACTION_CONVERT_INTO_ARTIFACT)){// not is_what() - deliberately ignore SF2_MYCOSYNTH_LATTICE
		int_id = create_a_card_type(int_id);
		cards_data[int_id].type |= TYPE_ARTIFACT;
	}
	int number = token->qty;
	if( ! is_what(-1, int_id, TYPE_EFFECT) ){
		number = get_updated_tokens_number(token->t_player, number);
	}

	int i;
	int card_added = -1;

	ASSERT(token->eff_card != -1 || !(token->keep_track_of_tokens_generated > 0 || token->legacy == 1 ||
									  (token->action & (TOKEN_ACTION_EQUIP | TOKEN_ACTION_HASTE | TOKEN_ACTION_PUMP_POWER | TOKEN_ACTION_PUMP_TOUGHNESS))));

	card_instance_t *source = token->keep_track_of_tokens_generated ? get_card_instance(token->eff_player, token->eff_card) : NULL;
	for( i = 0; i < number; i++){
		card_added = real_generate_token(token->t_player, int_id, token, 1);

		card_instance_t* instance = get_card_instance(token->t_player, card_added);

		if (token->legacy == 1){
			create_targetted_legacy_effect(token->eff_player, token->eff_card, token->special_code_for_legacy, token->t_player, card_added);
		}

		if (token->action & TOKEN_ACTION_TAPPED){
			instance->state |= STATE_TAPPED;	// Not tap_card() to avoid an EVENT_TAP_CARD
		}
		if (token->action & TOKEN_ACTION_ATTACKING_UNTAPPED){
			instance->state |= STATE_ATTACKING;
			choose_who_attack(token->t_player, card_added);
		}
		if (token->action & TOKEN_ACTION_BLOCKING){
			card_instance_t* to_block = get_card_instance(1 - token->t_player, token->action_argument);
			instance->blocking = to_block->blocking == 255 ? token->action_argument : to_block->blocking;
			instance->state |= STATE_UNKNOWN8000|STATE_BLOCKING;
		}
		if ((token->action & TOKEN_ACTION_EQUIP) && i == 0){
			equip_target_creature(token->eff_player, token->eff_card, token->t_player, card_added);
		}
		if (token->action & TOKEN_ACTION_DONT_COPY_TOKEN_SOURCE){
			set_special_flags(token->t_player, card_added, SF_DONT_COPY_TOKEN_SOURCE);
		}
		if (token->action & (TOKEN_ACTION_HASTE | TOKEN_ACTION_PUMP_POWER | TOKEN_ACTION_PUMP_TOUGHNESS)){
			pump_ability_until_eot(token->eff_player, token->eff_card, token->t_player, card_added,
								   (token->action & TOKEN_ACTION_PUMP_POWER) ? token->action_argument : 0,
								   (token->action & TOKEN_ACTION_PUMP_TOUGHNESS) ? token->action_argument : 0,
								   0,
								   (token->action & TOKEN_ACTION_HASTE) ? SP_KEYWORD_HASTE : 0);
		}
		if (token->action & TOKEN_ACTION_ADD_SUBTYPE){
			add_a_subtype(token->t_player, card_added, token->action_argument);
		}

		if (token->special_code_on_generation){
			(*token->special_code_on_generation)(token, card_added, i);
		}

		if( (int)token->keep_track_of_tokens_generated > i ){
			source->targets[i].player = token->t_player;
			source->targets[i].card = card_added;
		}

		put_into_play(token->t_player, card_added);

		/* Recovered 2022 token counter action (0x26659d0); after put_into_play. */
		if (token->action & TOKEN_ACTION_ADD_COUNTERS){
			++hack_silent_counters;
			add_counters_from_source(token->s_player, token->s_card, token->t_player, card_added,
								 (unsigned int)token->action_argument >> 16, token->action_argument & 0xffff);
			--hack_silent_counters;
		}
		/* End recovered 2022 token counter action. */

		if ((token->action & TOKEN_ACTION_BLOCKING) && in_play(token->t_player, card_added)){
			play_sound_effect(WAV_BLOCK2);
			if (event_flags & EA_SELECT_BLOCK){
				dispatch_trigger2(current_turn, TRIGGER_BLOCKER_CHOSEN, EXE_STR(0x790074)/*PROMPT_BLOCKERSELECTION[0]*/, 0, token->t_player, card_added);
			}
		}

		// legendary tokens
		if( token->special_infos == 99 ){
			// I'm almost certain this is no longer used.
			int p;
			int kill_me = 0;
			int leg_id = get_id(token->t_player, card_added);
			for(p=0;p<2;p++){
				int c = active_cards_count[p]-1;
				while( c > -1 ){
						if( in_play(p, c) && ! (p==token->t_player && c==card_added) ){
							if( get_id(p, c) == leg_id && get_special_infos(p, c) == 99 ){
								kill_card(p, c, KILL_SACRIFICE);
								kill_me = 1;
							}
						}
						c--;
				}
			}
			if( kill_me == 1 ){
				kill_card(token->t_player, card_added, KILL_SACRIFICE);
			}
		}
	}
	if( token->keep_track_of_tokens_generated > 0  ){
		token->keep_track_of_tokens_generated = MIN(token->keep_track_of_tokens_generated, (unsigned int)number);
	}
}

void copy_token(int player, int card, int tok_player, int tok_card ){
	if( tok_player < 0 || tok_card < 0 ){
		return;
	}

	token_generation_t token;
	copy_token_definition(player, card, &token, tok_player, tok_card);
	generate_token(&token);
}

static int generate_token_by_id_impl(int player, int card, int csvid, int number, int reserved){
	if (number <= 0){
		return 0;
	}

	token_generation_t token;
	default_token_definition(player, card, csvid, &token);
	int int_id = get_internal_card_id_from_csv_id(csvid);
	if (reserved){
		token.no_sleight = 1;
		number = 1;
	} else {
		number = get_updated_tokens_number(player, number);
		if (number < 1){
			number = 1;
		}
	}

	int i;
	int card_added = -1;
	for( i = 0; i < number; i++){
		card_added = real_generate_token(player, int_id, &token, 0);
	}

	return card_added;
}

void generate_token_by_id(int player, int card, int csvid){
	generate_token_by_id_impl(player, card, csvid, 1, is_reserved_id(csvid));
}

void generate_tokens_by_id(int player, int card, int csvid, int howmany){
	generate_token_by_id_impl(player, card, csvid, howmany, is_reserved_id(csvid));
}

int generate_reserved_token_by_id(int player, int csvid){
	ASSERT(is_reserved_id(csvid));
	// It's only safe to use -1 for card because we're sending 1 for reserved, and therefore will be setting no_sleight=1
	return generate_token_by_id_impl(player, -1, csvid, 1, 1);
}

void copy_all_tokens(int player, int card, int type, test_definition_t *test){
	int c, marked[151] = {0};
	for (c = 0; c < active_cards_count[player]; ++c){
		if (in_play(player, c) && is_token(player, c)){
			if( (type && is_what(player, c, type)) || (test != NULL && new_make_test_in_play(player, c, -1, test)) ){
				marked[c] = 1;
			}
		}
	}

	for (c = active_cards_count[player]-1; c >-1 ; c--){
		if (marked[c]){
			copy_token(player, card, player, c);
		}
	}
}

static int from_subtype_to_token_id(int subtype){
	switch( subtype ){
			case	SUBTYPE_ADVISOR: return 0;
			case	SUBTYPE_ALLY: return 0;
			case	SUBTYPE_ANGEL: return CARD_ID_ANGEL;
			case	SUBTYPE_ANTEATER: return 0;
			case	SUBTYPE_ANTELOPE: return 0;
			case	SUBTYPE_APE: return CARD_ID_APE;
			case	SUBTYPE_ARCHER: return 0;
			case	SUBTYPE_ARCHON: return 0;
			case	SUBTYPE_ARTIFICER: return 0;
			case	SUBTYPE_ASSASSIN: return CARD_ID_ASSASSIN;
			case	SUBTYPE_ASSEMBLY_WORKER: return 0;
			case	SUBTYPE_ATOG: return 0;
			case	SUBTYPE_AUROCHS: return 0;
			case	SUBTYPE_AVATAR: return CARD_ID_AVATAR;
			case	SUBTYPE_BADGER: return 0;
			case	SUBTYPE_BARBARIAN: return 0;
			case	SUBTYPE_BASILISK: return 0;
			case	SUBTYPE_BAT: return CARD_ID_BAT;
			case	SUBTYPE_BEAR: return CARD_ID_BEAR;
			case	SUBTYPE_BEAST: return CARD_ID_BEAST;
			case	SUBTYPE_BEEBLE: return 0;
			case	SUBTYPE_BERSERKER: return 0;
			case	SUBTYPE_BIRD: return CARD_ID_BIRD;
			case	SUBTYPE_BLINKMOTH: return 0;
			case	SUBTYPE_BOAR: return CARD_ID_BOAR;
			case	SUBTYPE_BRINGER: return 0;
			case	SUBTYPE_BRUSHWAGG: return 0;
			case	SUBTYPE_CAMARID: return CARD_ID_CAMARID;
			case	SUBTYPE_CAMEL: return 0;
			case	SUBTYPE_CARIBOU: return CARD_ID_CARIBOU;
			case	SUBTYPE_CARRIER: return 0;
			case	SUBTYPE_CAT: return CARD_ID_CAT;
			case	SUBTYPE_CENTAUR: return CARD_ID_CENTAUR;
			case	SUBTYPE_CEPHALID: return 0;
			case	SUBTYPE_CHIMERA: return 0;
			case	SUBTYPE_CITIZEN: return CARD_ID_CITIZEN;
			case	SUBTYPE_CLERIC: return CARD_ID_CLERIC;
			case	SUBTYPE_COCKATRICE: return 0;
			case	SUBTYPE_CONSTRUCT: return CARD_ID_CONSTRUCT;
			case	SUBTYPE_COWARD: return 0;
			case	SUBTYPE_CRAB: return 0;
			case	SUBTYPE_CROCODILE: return 0;
			case	SUBTYPE_CYCLOPS: return 0;
			case	SUBTYPE_DAUTHI: return 0;
			case	SUBTYPE_DEMON: return CARD_ID_DEMON;
			case	SUBTYPE_DESERTER: return CARD_ID_DESERTER;
			case	SUBTYPE_DEVIL: return CARD_ID_DEVIL;
			case	SUBTYPE_DJINN: return 0;
			case	SUBTYPE_DRAGON: return CARD_ID_DRAGON;
			case	SUBTYPE_DRAKE: return CARD_ID_DRAKE;
			case	SUBTYPE_DREADNOUGHT: return 0;
			case	SUBTYPE_DRONE: return 0;
			case	SUBTYPE_DRUID: return 0;
			case	SUBTYPE_DRYAD: return 0;
			case	SUBTYPE_DWARF: return 0;
			case	SUBTYPE_EFREET: return 0;
			case	SUBTYPE_ELDER: return 0;
			case	SUBTYPE_ELDRAZI: return CARD_ID_ELDRAZI;
			case	SUBTYPE_ELEMENTAL: return CARD_ID_ELEMENTAL;
			case	SUBTYPE_ELEPHANT: return CARD_ID_ELEPHANT;
			case	SUBTYPE_ELF: return 0;
			case	SUBTYPE_ELK: return 0;
			case	SUBTYPE_EYE: return 0;
			case	SUBTYPE_FAERIE: return CARD_ID_FAERIE;
			case	SUBTYPE_FERRET: return 0;
			case	SUBTYPE_FISH: return CARD_ID_FISH;
			case	SUBTYPE_FLAGBEARER: return 0;
			case	SUBTYPE_FOX: return 0;
			case	SUBTYPE_FROG: return 0;
			case	SUBTYPE_FUNGUS: return 0;
			case	SUBTYPE_GARGOYLE: return CARD_ID_GARGOYLE;
			case	SUBTYPE_GERM: return CARD_ID_GERM;
			case	SUBTYPE_GIANT: return CARD_ID_GIANT;
			case	SUBTYPE_GNOME: return CARD_ID_GNOME;
			case	SUBTYPE_GOAT: return CARD_ID_GOAT;
			case	SUBTYPE_GOBLIN: return CARD_ID_GOBLIN;
			case	SUBTYPE_GOD: return 0;
			case	SUBTYPE_GOLEM: return CARD_ID_GOLEM;
			case	SUBTYPE_GORGON: return 0;
			case	SUBTYPE_GRAVEBORN: return CARD_ID_GRAVEBORN;
			case	SUBTYPE_GREMLIN: return 0;
			case	SUBTYPE_GRIFFIN: return CARD_ID_GRIFFIN;
			case	SUBTYPE_HAG: return 0;
			case	SUBTYPE_HARPY: return CARD_ID_HARPY;
			case	SUBTYPE_HEAD: return 0;
			case	SUBTYPE_HELLION: return CARD_ID_HELLION;
			case	SUBTYPE_HIPPO: return CARD_ID_HIPPO;
			case	SUBTYPE_HIPPOGRIFF: return 0;
			case	SUBTYPE_HOMARID: return 0;
			case	SUBTYPE_HOMUNCULUS: return CARD_ID_HOMUNCULUS;
			case	SUBTYPE_HORROR: return CARD_ID_HORROR;
			case	SUBTYPE_HORSE: return 0;
			case	SUBTYPE_HOUND: return CARD_ID_HOUND;
			case	SUBTYPE_HUMAN: return CARD_ID_HUMAN;
			case	SUBTYPE_HYDRA: return CARD_ID_HYDRA;
			case	SUBTYPE_HYENA: return 0;
			case	SUBTYPE_ILLUSION: return CARD_ID_ILLUSION_TOKEN;
			case	SUBTYPE_IMP: return 0;
			case	SUBTYPE_INCARNATION: return 0;
			case	SUBTYPE_INSECT: return CARD_ID_INSECT;
			case	SUBTYPE_JELLYFISH: return 0;
			case	SUBTYPE_JUGGERNAUT: return 0;
			case	SUBTYPE_KAVU: return CARD_ID_KAVU;
			case	SUBTYPE_KIRIN: return 0;
			case	SUBTYPE_KITHKIN: return 0;
			case	SUBTYPE_KNIGHT: return CARD_ID_KNIGHT;
			case	SUBTYPE_KOBOLD: return 0;
			case	SUBTYPE_KOR: return 0;
			case	SUBTYPE_KRAKEN: return CARD_ID_KRAKEN;
			case	SUBTYPE_LAMIA: return 0;
			case	SUBTYPE_LAMMASU: return 0;
			case	SUBTYPE_LEECH: return 0;
			case	SUBTYPE_LEVIATHAN: return 0;
			case	SUBTYPE_LHURGOYF: return 0;
			case	SUBTYPE_LICID: return 0;
			case	SUBTYPE_LIZARD: return CARD_ID_LIZARD;
			case	SUBTYPE_MANTICORE: return 0;
			case	SUBTYPE_MASTICORE: return 0;
			case	SUBTYPE_MERCENARY: return 0;
			case	SUBTYPE_MERFOLK: return CARD_ID_MERFOLK;
			case	SUBTYPE_METATHRAN: return 0;
			case	SUBTYPE_MINION: return CARD_ID_MINION;
			case	SUBTYPE_MINOTAUR: return CARD_ID_MINOTAUR;
			case	SUBTYPE_MOLE: return 0;
			case	SUBTYPE_MONGER: return 0;
			case	SUBTYPE_MONGOOSE: return 0;
			case	SUBTYPE_MONK: return CARD_ID_MONK;
			case	SUBTYPE_MOONFOLK: return 0;
			case	SUBTYPE_MUTANT: return 0;
			case	SUBTYPE_MYR: return CARD_ID_MYR;
			case	SUBTYPE_MYSTIC: return 0;
			case	SUBTYPE_NAGA: return 0;
			case	SUBTYPE_NAUTILUS: return 0;
			case	SUBTYPE_NEPHILIM: return 0;
			case	SUBTYPE_NIGHTMARE: return 0;
			case	SUBTYPE_NIGHTSTALKER: return 0;
			case	SUBTYPE_NINJA: return 0;
			case	SUBTYPE_NOGGLE: return 0;
			case	SUBTYPE_NOMAD: return 0;
			case	SUBTYPE_NYMPH: return 0;
			case	SUBTYPE_OCTOPUS: return CARD_ID_OCTOPUS;
			case	SUBTYPE_OGRE: return CARD_ID_OGRE;
			case	SUBTYPE_OOZE: return CARD_ID_OOZE;
			case	SUBTYPE_ORB: return CARD_ID_ORB;
			case	SUBTYPE_ORC: return 0;
			case	SUBTYPE_ORGG: return 0;
			case	SUBTYPE_OUPHE: return 0;
			case	SUBTYPE_OX: return 0;
			case	SUBTYPE_OYSTER: return 0;
			case	SUBTYPE_PEGASUS: return CARD_ID_PEGASUS;
			case	SUBTYPE_PENTAVITE: return CARD_ID_PENTAVITE;
			case	SUBTYPE_PEST: return CARD_ID_PEST;
			case	SUBTYPE_PHELDDAGRIF: return 0;
			case	SUBTYPE_PHOENIX: return 0;
			case	SUBTYPE_PINCHER: return CARD_ID_PINCHER;
			case	SUBTYPE_PIRATE: return 0;
			case	SUBTYPE_PLANT: return CARD_ID_PLANT;
			case	SUBTYPE_PRAETOR: return 0;
			case	SUBTYPE_PRISM: return CARD_ID_PRISM;
			case	SUBTYPE_PROCESSOR: return 0;
			case	SUBTYPE_RABBIT: return 0;
			case	SUBTYPE_RAT: return CARD_ID_RAT;
			case	SUBTYPE_REBEL: return 0;
			case	SUBTYPE_REFLECTION: return CARD_ID_REFLECTION;
			case	SUBTYPE_REVELER: return 0;
			case	SUBTYPE_RHINO: return CARD_ID_RHINO;
			case	SUBTYPE_RIGGER: return 0;
			case	SUBTYPE_ROGUE: return 0;
			case	SUBTYPE_SABLE: return 0;
			case	SUBTYPE_SALAMANDER: return 0;
			case	SUBTYPE_SAMURAI: return 0;
			case	SUBTYPE_SAND: return 0;
			case	SUBTYPE_SAPROLING: return CARD_ID_SAPROLING;
			case	SUBTYPE_SATYR: return CARD_ID_SATYR;
			case	SUBTYPE_SCARECROW: return 0;
			case	SUBTYPE_SCION: return 0;
			case	SUBTYPE_SCORPION: return 0;
			case	SUBTYPE_SCOUT: return 0;
			case	SUBTYPE_SERF: return CARD_ID_SERF;
			case	SUBTYPE_SERPENT: return 0;
			case	SUBTYPE_SHADE: return 0;
			case	SUBTYPE_SHAMAN: return 0;
			case	SUBTYPE_SHAPESHIFTER: return CARD_ID_SHAPESHIFTER_TOKEN;
			case	SUBTYPE_SHEEP: return CARD_ID_SHEEP;
			case	SUBTYPE_SIREN: return 0;
			case	SUBTYPE_SKELETON: return CARD_ID_SKELETON;
			case	SUBTYPE_SLITH: return 0;
			case	SUBTYPE_SLIVER: return CARD_ID_SLIVER;
			case	SUBTYPE_SLUG: return 0;
			case	SUBTYPE_SNAKE: return CARD_ID_SNAKE;
			case	SUBTYPE_SOLDIER: return CARD_ID_SOLDIER;
			case	SUBTYPE_SOLTARI: return 0;
			case	SUBTYPE_SPAWN: return CARD_ID_SPAWN;
			case	SUBTYPE_SPECTER: return 0;
			case	SUBTYPE_SPELLSHAPER: return 0;
			case	SUBTYPE_SPHINX: return CARD_ID_SPHINX;
			case	SUBTYPE_SPIDER: return CARD_ID_SPIDER;
			case	SUBTYPE_SPIKE: return CARD_ID_SPIKE;
			case	SUBTYPE_SPIRIT: return CARD_ID_SPIRIT;
			case	SUBTYPE_SPLINTER: return CARD_ID_SPLINTER_TOKEN;
			case	SUBTYPE_SPONGE: return 0;
			case	SUBTYPE_SQUID: return CARD_ID_SQUID;
			case	SUBTYPE_SQUIRREL: return CARD_ID_SQUIRREL;
			case	SUBTYPE_STARFISH: return CARD_ID_STARFISH;
			case	SUBTYPE_SURRAKAR: return CARD_ID_SURVIVOR;
			case	SUBTYPE_SURVIVOR: return 0;
			case	SUBTYPE_TETRAVITE: return 0;
			case	SUBTYPE_THALAKOS: return 0;
			case	SUBTYPE_THOPTER: return CARD_ID_THOPTER;
			case	SUBTYPE_THRULL: return CARD_ID_THRULL;
			case	SUBTYPE_TREEFOLK: return CARD_ID_TREEFOLK;
			case	SUBTYPE_TRISKELAVITE: return CARD_ID_TRISKELAVITE;
			case	SUBTYPE_TROLL: return 0;
			case	SUBTYPE_TURTLE: return 0;
			case	SUBTYPE_UNICORN: return 0;
			case	SUBTYPE_VAMPIRE: return CARD_ID_VAMPIRE;
			case	SUBTYPE_VEDALKEN: return 0;
			case	SUBTYPE_VIASHINO: return 0;
			case	SUBTYPE_VOLVER: return 0;
			case	SUBTYPE_WALL: return 0;
			case	SUBTYPE_WARRIOR: return CARD_ID_WARRIOR;
			case	SUBTYPE_WEIRD: return CARD_ID_WEIRD;
			case	SUBTYPE_WEREWOLF: return 0;
			case	SUBTYPE_WHALE: return CARD_ID_WHALE;
			case	SUBTYPE_WIZARD: return 0;
			case	SUBTYPE_WOLF: return CARD_ID_WOLF;
			case	SUBTYPE_WOLVERINE: return 0;
			case	SUBTYPE_WOMBAT: return 0;
			case	SUBTYPE_WORM: return CARD_ID_WORM;
			case	SUBTYPE_WRAITH: return 0;
			case	SUBTYPE_WURM: return 0;
			case	SUBTYPE_YETI: return 0;
			case	SUBTYPE_ZOMBIE: return CARD_ID_ZOMBIE;
			case	SUBTYPE_ZUBERA: return 0;
			default: return 0;
	}
}

static void store_selected_subtype(token_generation_t* token, int beast, int unused)
{
	get_card_instance(token->t_player, beast)->targets[1].card = token->action_argument;
}

void generate_custom_token_from_subtype(int player, int card, int subtype, int color, int pow, int tou, int amount){
	// See Riptide Replicator and Volrath's Laboratory

	int csvid = from_subtype_to_token_id(subtype);

	token_generation_t token;
	if( csvid ){
		default_token_definition(player, card, csvid, &token);
	}
	else{
		default_token_definition(player, card, CARD_ID_RIPTIDE_REPLICATOR_TOKEN, &token);
	}
	token.qty = amount;
	token.pow = pow;
	token.tou = tou;
	token.color_forced = color;
	token.no_sleight = 1;
	if( ! csvid ){
		token.special_code_on_generation = &store_selected_subtype;
		token.action_argument = subtype;
	}
	generate_token(&token);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2664e30; original functions/token_generation.c:945. */
static int legacy_negate_static_abilities(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = get_card_instance(player, card);
  if (event == EVENT_ABILITIES)
  {
    if ((instance->damage_target_card == affected_card) &&
        (instance->damage_target_player == affected_card_controller))
    {
      event_result = event_result & 0xffffffdf;
    }
  }
  else if (((event == EVENT_CHANGE_TYPE) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    instance->regen_status = instance->regen_status | 0x1f000000;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x26668f0; original functions/token_generation.c:680. */
void copy_all_tokens_both_players(int player, int card, int type, test_definition_t *test)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  unsigned char bVar6;
  int marked[151];

  iVar3 = current_turn;

  bVar6 = 0;
  iVar2 = active_cards_count[current_turn];
  if (0 < iVar2)
  {
    piVar5 = marked;
    for (iVar4 = 0x97; iVar4 != 0; iVar4 = iVar4 + -1)
    {
      *piVar5 = 0;
      piVar5 = piVar5 + 1;
    }
    if (0 < iVar2)
    {
      iVar4 = 0;
      do
      {
        instance = in_play(iVar3, iVar4);
        if (((instance != NULL) && (iVar2 = is_token(iVar3, iVar4), iVar2 != 0)) &&
            (((type != 0 && (iVar2 = is_what(iVar3, iVar4, type), iVar2 != 0)) ||
              ((test != NULL &&
                (iVar2 = new_make_test_in_play(iVar3, iVar4, -1, test), iVar2 != 0))))))
        {
          marked[iVar4] = 1;
        }
        iVar4 = iVar4 + 1;
        iVar2 = active_cards_count[iVar3];
      } while (iVar4 < iVar2);
    }
    iVar2 = iVar2 + -1;
    if (-1 < iVar2)
    {
      do
      {

        if ((marked[iVar2] != 0) && (-1 < iVar3))
        {
          copy_token(iVar3, iVar2, iVar3, iVar2);
        }
        iVar2 = iVar2 + -1;
      } while (iVar2 != -1);
    }
  }
  iVar3 = 1 - current_turn;
  iVar2 = active_cards_count[iVar3];
  if (0 < iVar2)
  {
    piVar5 = marked;
    for (iVar4 = 0x97; iVar4 != 0; iVar4 = iVar4 + -1)
    {
      *piVar5 = 0;
      piVar5 = piVar5 + (unsigned int)bVar6 * -2 + 1;
    }
    if (0 < iVar2)
    {
      iVar4 = 0;
      do
      {
        instance = in_play(iVar3, iVar4);
        if (((instance != NULL) && (iVar2 = is_token(iVar3, iVar4), iVar2 != 0)) &&
            (((type != 0 && (iVar2 = is_what(iVar3, iVar4, type), iVar2 != 0)) ||
              ((test != NULL &&
                (iVar2 = new_make_test_in_play(iVar3, iVar4, -1, test), iVar2 != 0))))))
        {
          marked[iVar4] = 1;
        }
        iVar4 = iVar4 + 1;
        iVar2 = active_cards_count[iVar3];
      } while (iVar4 < iVar2);
    }
    iVar2 = iVar2 + -1;
    if (-1 < iVar2)
    {
      do
      {

        if ((marked[iVar2] != 0) && (-1 < iVar3))
        {
          copy_token(iVar3, iVar2, iVar3, iVar2);
        }
        iVar2 = iVar2 + -1;
      } while (iVar2 != -1);
    }
  }
  return;
}


/* October 2022 token creation component. */
void generate_token_2022(token_generation_2022_t *token);
static void set_token_2022(int player, int card, token_generation_2022_t *token);
void default_token_definition_2022(int player, int card, int id, token_generation_2022_t *token);
static int resolve_token_iid_2022(iid_t int_id, token_generation_2022_t *token);
static int get_copyable_special_infos_part_2022(int t_player, int t_card);
void copy_token_definition_2022(int player, int card, token_generation_2022_t *token, int t_player,
                                int t_card);
int token_characteristic_setting_effects_2022(int player, int card, event_t event, int t_player,
                                              int t_card);
int generic_token_2022(int player, int card, event_t event);

/* ManalinkEh 2022: 0x26659d0; original functions/token_generation.c:503. */
void generate_token_2022(token_generation_2022_t *token)
{
  uint16_t uVar1;
  uint16_t uVar2;
  int32_t iVar3;
  int32_t iVar4;
  bool bVar5;
  bool bVar6;
  uint8_t uVar7;
  int iVar8;
  card_instance_t *pcVar9;
  token_action_2022_t tVar10;
  int iVar11;
  card_instance_t *pcVar12;
  int iVar13;
  int card;
  int iVar14;
  unsigned int uVar15;
  unsigned int uStack_ac;
  int iStack_9c;
  card_instance_t *pcStack_98;
  int iStack_94;
  int iStack_90;
  iid_t iStack_8c;
  int iStack_88;
  int aiStack_84[8];
  target_definition_t td;

  iVar8 = get_internal_card_id_from_csv_id(token->id);
  uStack_ac = (unsigned int)token->qty;
  iVar8 = is_what(-1, iVar8, 0x80);
  if (iVar8 == 0)
  {
    iVar8 = token->t_player;
    iVar14 = 0;
    do
    {
      iVar13 = 0;
      if (0 < active_cards_count[iVar14])
      {
      LAB_026664f0:
        do
        {
          pcVar9 = in_play(iVar14, iVar13);
          if (pcVar9 != NULL)
          {
            uVar2 = cards_data[pcVar9->internal_card_id].id;
            if (uVar2 != 0x3541)
            {
              if ((unsigned short)uVar2 < 0x3542)
              {
                if ((uVar2 != 0x1e90) && (uVar2 != 0x2f09))
                {
                  iVar13 = iVar13 + 1;
                  if (active_cards_count[iVar14] <= iVar13)
                    break;
                  goto LAB_026664f0;
                }
              }
              else
              {
                if (uVar2 == 0x39ba)
                  goto LAB_02666529;
                if (uVar2 != 0x41c4)
                  goto LAB_0266652b;
              }
              if (iVar8 != iVar14)
                goto LAB_0266652b;
            }
          LAB_02666529:
            uStack_ac = uStack_ac * 2;
          }
        LAB_0266652b:
          iVar13 = iVar13 + 1;
        } while (iVar13 < active_cards_count[iVar14]);
      }
      iVar14 = iVar14 + 1;
    } while (iVar14 != 2);
  }
  if ((int)uStack_ac < 1)
  {
    return;
  }
  iStack_94 = token->id;
  aiStack_84[2] = 0xffffffff;
  aiStack_84[0] = iStack_94;
  aiStack_84[3] = 0;
  aiStack_84[1] = uStack_ac;
  iVar8 = token->t_player;
  aiStack_84[4] = 0xffffffff;
  aiStack_84[5] = 0;
  aiStack_84[6] = 0xffffffff;
  aiStack_84[7] = 0;
  if (active_cards_count[iVar8] < 1)
  {
    bVar5 = false;
  }
  else
  {
    iVar13 = 0;
    bVar5 = false;
    iStack_88 = 0;
    iVar14 = 0;
  LAB_02665aa9:
    do
    {
      pcVar9 = in_play(iVar8, iVar13);
      if ((pcVar9 != NULL) &&
          (iVar8 = is_humiliated_for_ability(token->t_player, iVar13, AL_STATIC), iVar8 == 0))
      {
        iVar8 = get_id(token->t_player, iVar13);
        iVar14 = iVar14 + (unsigned int)(iVar8 == 0x59a8);
        iVar8 = get_id(token->t_player, iVar13);
        iStack_88 = iStack_88 + (unsigned int)(iVar8 == 0x59ce);
        iVar8 = get_id(token->t_player, iVar13);
        if (iVar8 == 0x4b8f)
        {
          iVar8 = token->t_player;
          iVar13 = iVar13 + 1;
          bVar5 = true;
          if (active_cards_count[iVar8] <= iVar13)
            break;
          goto LAB_02665aa9;
        }
      }
      iVar8 = token->t_player;
      iVar13 = iVar13 + 1;
    } while (iVar13 < active_cards_count[iVar8]);
    for (; iVar14 != 0; iVar14 = iVar14 + -1)
    {
      while (iStack_94 != 0x4917)
      {
        if (iStack_94 == 0x5077)
        {
          if (1 < iVar14)
          {
            aiStack_84[1] = aiStack_84[1] + uStack_ac;
          }
          aiStack_84[2] = 0x4917;
          aiStack_84[3] = aiStack_84[3] + uStack_ac;
          aiStack_84[4] = 0x4998;
          aiStack_84[5] = aiStack_84[5] + uStack_ac;
        }
        else if (iStack_94 == 0x4998)
        {
          if (1 < iVar14)
          {
            aiStack_84[1] = aiStack_84[1] + uStack_ac;
          }
          aiStack_84[2] = 0x5077;
          aiStack_84[3] = aiStack_84[3] + uStack_ac;
          aiStack_84[4] = 0x4917;
          aiStack_84[5] = aiStack_84[5] + uStack_ac;
        }
        iVar14 = iVar14 + -1;
        if (iVar14 == 0)
          goto LAB_02665be0;
      }
      if (1 < iVar14)
      {
        aiStack_84[1] = aiStack_84[1] + uStack_ac;
      }
      aiStack_84[2] = 0x5077;
      aiStack_84[3] = aiStack_84[3] + uStack_ac;
      aiStack_84[4] = 0x4998;
      aiStack_84[5] = aiStack_84[5] + uStack_ac;
    }
  LAB_02665be0:
    if (iStack_88 != 0)
    {
      aiStack_84[6] = 0x4991;
      aiStack_84[7] = (iStack_88 + -1) * uStack_ac + uStack_ac;
    }
  }
  iStack_90 = 0;
  do
  {
    if (-1 < iStack_94)
    {
      if (((bVar5) && (iVar8 = get_internal_card_id_from_csv_id(iStack_94),
                       cards_data[iVar8].cc[2] != '\x03')) &&
          (cards_data[iVar8].type != 0x80))
      {
        iStack_94 = 0x48ff;
        aiStack_84[iStack_90 * 2] = 0x48ff;
      }
      iStack_8c = get_internal_card_id_from_csv_id(iStack_94);
      iStack_88 = 0;
      iVar8 = aiStack_84[iStack_90 * 2 + 1];
      if (0 < iVar8)
      {
        do
        {
          if ((0 < iStack_90) &&
              (default_token_definition_2022(token->s_player, token->s_card, iStack_94, token),
               iStack_94 == 0x4991))
          {
            token->tou = 1;
            token->pow = 1;
            token->color_forced = '\b';
          }
          if (((bVar5) && (iVar14 = get_internal_card_id_from_csv_id(iStack_94),
                           cards_data[iVar14].cc[2] != '\x03')) &&
              (cards_data[iVar14].type != 0x80))
          {
            token->key_plus = 0x20;
            token->s_key_plus = 8;
            token->tou = 4;
            token->pow = 4;
            token->color_forced = ' ';
            token->special_infos = 0;
          }
          iStack_8c = resolve_token_iid_2022(iStack_8c, token);
          if (token->eff_card == -1)
          {
            if (((token->keep_track_of_tokens_generated != 0) || (token->legacy == '\x01')) ||
                ((token->action & (TOKEN2022_ACTION_PUMP_TOUGHNESS | TOKEN2022_ACTION_PUMP_POWER |
                                   TOKEN2022_ACTION_HASTE | TOKEN2022_ACTION_EQUIP)) != 0))
            {
              backtrace_and_abort("functions/token_generation.c:392\ntoken->eff_card != -1 || "
                                  "!(token->keep_track_of_tokens_generated > 0 || token->legacy == "
                                  "1 || (token->action & (TOKEN_ACTION_EQUIP | TOKEN_ACTION_HASTE "
                                  "| TOKEN_ACTION_PUMP_POWER | TOKEN_ACTION_PUMP_TOUGHNESS)))");
            }
          LAB_02665cab:
            pcStack_98 = NULL;
          }
          else
          {
            if (token->keep_track_of_tokens_generated == 0)
              goto LAB_02665cab;
            pcStack_98 = get_card_instance(token->eff_player, token->eff_card);
          }
          iVar14 = token->t_player;
          iVar13 = add_card_to_hand(iVar14, iStack_8c);
          set_token_2022(iVar14, iVar13, token);
          pcVar9 = get_card_instance(iVar14, iVar13);
          pcVar9->state = pcVar9->state | 0x20;
          hand_count[iVar14] = hand_count[iVar14] + -1;
          pcVar9 = get_card_instance(token->t_player, iVar13);
          if (token->legacy == '\x01')
          {
            iVar14 = create_targetted_legacy_effect(token->eff_player, token->eff_card,
                                                    token->special_code_for_legacy, token->t_player,
                                                    iVar13);
            pcVar12 = get_card_instance(token->eff_player, iVar14);
            pcVar12->eot_toughness = 0x300;
            uVar1 = cards_data[iStack_8c].id;
            pcVar12->display_pic_csv_id = uVar1;
          }
          tVar10 = token->action;
          if ((tVar10 & TOKEN2022_ACTION_TAPPED) != 0)
          {
            pcVar9->state = pcVar9->state | 0x10;
          }
          if (((tVar10 & TOKEN2022_ACTION_ATTACKING_UNTAPPED) != 0) &&
              (pcVar9->state = pcVar9->state | 4,
               (tVar10 & TOKEN2022_ACTION_SKIP_ATTACK_CHECK) == 0))
          {
            choose_who_attack(token->t_player, iVar13);
            tVar10 = token->action;
          }
          if ((tVar10 & TOKEN2022_ACTION_BLOCKING) != 0)
          {
            pcVar12 = get_card_instance(1 - token->t_player, token->action_argument);
            uVar7 = pcVar12->blocking;
            if (uVar7 == 0xff)
            {
              uVar7 = (uint8_t)token->action_argument;
            }
            pcVar9->blocking = uVar7;
            tVar10 = token->action;
            pcVar9->state = pcVar9->state | 0x8008;
          }
          if (((tVar10 & TOKEN2022_ACTION_EQUIP) != 0) && (iStack_88 == 0))
          {
            equip_target_creature(token->eff_player, token->eff_card, token->t_player, iVar13);
            tVar10 = token->action;
          }
          if (((tVar10 & TOKEN2022_ACTION_EQUIP_ANY) != 0) && (iStack_88 == 0))
          {
            default_target_definition(token->eff_player, token->eff_card, &td, 0xff);
            td.required_subtype = 0x3001;
            td.illegal_abilities = 0;
            pcVar12 = get_card_instance(token->eff_player, token->eff_card);
            iVar3 = pcVar12->targets[0].player;
            iVar4 = pcVar12->targets[0].card;
            iVar14 = can_target(&td);
            if ((iVar14 != 0) &&
                (iVar14 = new_pick_target(&td, "Select an equipment you control", 0, 0x80),
                 iVar14 != 0))
            {
              equip_target_creature(pcVar12->targets[0].player, pcVar12->targets[0].card,
                                    token->t_player, iVar13);
            }
            pcVar12->targets[0].player = iVar3;
            pcVar12->targets[0].card = iVar4;
            tVar10 = token->action;
          }
          if ((tVar10 & TOKEN2022_ACTION_DONT_COPY_TOKEN_SOURCE) != 0)
          {
            set_special_flags_2022(token->t_player, iVar13, 0x40000000);
            tVar10 = token->action;
          }
          if ((tVar10 & (TOKEN2022_ACTION_PUMP_TOUGHNESS | TOKEN2022_ACTION_PUMP_POWER |
                         TOKEN2022_ACTION_HASTE)) != 0)
          {
            iStack_9c = 0;
            if ((tVar10 & TOKEN2022_ACTION_PUMP_TOUGHNESS) != 0)
            {
              iStack_9c = token->action_argument;
            }
            iVar14 = 0;
            if ((tVar10 & TOKEN2022_ACTION_PUMP_POWER) != 0)
            {
              iVar14 = token->action_argument;
            }
            pump_ability_until_eot(token->eff_player, token->eff_card, token->t_player, iVar13,
                                   iVar14, iStack_9c, 0, tVar10 >> 2 & 4);
            tVar10 = token->action;
          }
          if ((tVar10 & TOKEN2022_ACTION_ADD_SUBTYPE) != 0)
          {
            add_a_subtype_permanently(token->eff_player, token->eff_card, token->t_player, iVar13,
                                      token->action_argument);
            tVar10 = token->action;
          }
          if ((tVar10 & TOKEN2022_ACTION_RECORD_SOURCE_ID) != 0)
          {
            iVar14 = get_id(token->s_player, token->s_card);
            pcVar9->damage_source_card = iVar14;
          }
          if (token->special_code_on_generation != NULL)
          {
            (*token->special_code_on_generation)(token, iVar13, iStack_88);
          }
          iVar14 = token->t_player;
          if (iStack_88 < (int)token->keep_track_of_tokens_generated)
          {
            pcStack_98->targets[iStack_88].player = iVar14;
            pcStack_98->targets[iStack_88].card = iVar13;
          }
          put_into_play(iVar14, iVar13);
          tVar10 = token->action;
          if ((tVar10 & TOKEN2022_ACTION_ADD_COUNTERS) != 0)
          {
            hack_silent_counters = hack_silent_counters + 1;
            add_counters_from_source(token->s_player, token->s_card, token->t_player, iVar13,
                                     (unsigned int)token->action_argument >> 0x10,
                                     token->action_argument & 0xffff);
            hack_silent_counters = hack_silent_counters + -1;
            tVar10 = token->action;
          }
          if ((((tVar10 & TOKEN2022_ACTION_BLOCKING) == 0) ||
               (pcVar9 = in_play(token->t_player, iVar13), pcVar9 == NULL)) ||
              (play_sound_effect(WAV_BLOCK2),
               (((unsigned int)event_flags >> 16 & 0xff) & 0x10) == 0))
          {
            uVar15 = token->special_infos;
          }
          else
          {
            dispatch_trigger2(current_turn, TRIGGER_BLOCKER_CHOSEN, EXE_STR(0x790074), 0,
                              token->t_player, iVar13);
            uVar15 = token->special_infos;
          }
          if (uVar15 == 99)
          {
            uVar15 = 0;
            iVar14 = get_id(token->t_player, iVar13);
            bVar6 = false;
            do
            {
              card = active_cards_count[uVar15] + -1;
              if (-1 < card)
              {
                do
                {
                  pcVar9 = in_play(uVar15, card);
                  if ((((pcVar9 != NULL) && ((token->t_player != uVar15 || (iVar13 != card)))) &&
                       (iVar11 = get_id(uVar15, card), iVar14 == iVar11)) &&
                      (((int)(uVar15 ^ 0xffffffff) < 0 && (pcVar9 = get_card_instance(uVar15, card),
                                                           pcVar9->targets[0xe].player == 99))))
                  {
                    kill_card(uVar15, card, KILL_SACRIFICE);
                    bVar6 = true;
                  }
                  card = card + -1;
                } while (card != -1);
              }
              uVar15 = uVar15 + 1;
            } while (uVar15 != 2);
            if (bVar6)
            {
              kill_card(token->t_player, iVar13, KILL_SACRIFICE);
            }
          }
          if (iStack_88 < (int)token->keep_track_of_tokens_generated)
          {
            pcVar9 = get_card_instance(token->eff_player, token->eff_card);
            pcVar9->targets[iStack_88].player = token->t_player;
            pcVar9->targets[iStack_88].card = iVar13;
          }
          iStack_88 = iStack_88 + 1;
        } while (iStack_88 != iVar8);
      }
      increase_trap_condition(token->t_player, 0x206, iVar8);
    }
    iStack_90 = iStack_90 + 1;
    if (iStack_90 == 4)
    {
      uVar15 = token->keep_track_of_tokens_generated;
      if (uVar15 == 0)
      {
        return;
      }
      if (uVar15 < uStack_ac)
      {
        uStack_ac = uVar15;
      }
      token->keep_track_of_tokens_generated = uStack_ac;
      return;
    }
    iStack_94 = aiStack_84[iStack_90 * 2];
  } while (true);
}

/* ManalinkEh 2022: 0x2664eb0; original functions/token_generation.c:150. */
static void set_token_2022(int player, int card, token_generation_2022_t *token)
{
  uint32_t uVar1;
  unsigned int uVar2;
  card_instance_t *instance;

  instance = get_card_instance(player, card);
  instance->damage_source_player = (int8_t)token->s_player;
  instance->damage_source_card = token->s_card;
  uVar1 = token->s_key_plus;
  instance->ext->sp_keywords = uVar1;
  instance->token_special_keywords_2022 = uVar1;
  set_special_flags2_2022(player, card, token->special_flags2);
  instance = get_card_instance(player, card);
  uVar2 = token->special_infos;
  instance->token_status = instance->token_status | 0x10;
  instance = get_card_instance(player, card);
  instance->targets[0xe].player = uVar2;
  return;
}

/* ManalinkEh 2022: 0x2665470; original functions/token_generation.c:5. */
void default_token_definition_2022(int player, int card, int id, token_generation_2022_t *token)
{
  int result;
  card_instance_t *instance;
  token->id = id;
  token->eff_player = player;
  token->eff_card = card;
  token->t_player = player;
  if ((player < 0) || (card < 0))
  {
  LAB_02665498:
    token->s_card = -1;
    token->s_player = -1;
    token->no_sleight = '\x01';
  }
  else
  {
    instance = get_card_instance(player, card);
    result = instance->internal_card_id;
    if (result == activation_card)
    {
      player = instance->parent_controller;
      card = instance->parent_card;
    LAB_0266554c:
      if ((player < 0) || (card < 0))
        goto LAB_02665498;
    }
    else
    {
      if ((-1 < result) && (cards_data[result].id - 0x385 < 8))
      {
        player = (int)instance->damage_source_player;
        card = instance->damage_source_card;
        goto LAB_0266554c;
      }
    }
    token->s_player = player;
    token->s_card = card;
    token->no_sleight = '\0';
  }
  token->pow = -1;
  token->tou = -1;
  token->key_plus = 0;
  token->s_key_plus = 0;
  token->color_forced = '\0';
  token->special_code_for_legacy = empty;
  token->special_code_on_generation = NULL;
  token->legacy = '\0';
  token->qty = '\x01';
  token->special_infos = 0;
  token->action = 0;
  token->action_argument = 0;
  token->keep_track_of_tokens_generated = 0;
  token->special_flags2 = 0;
  token->special_flags3 = 0;
  return;
}

/* ManalinkEh 2022: 0x2664f30; original functions/token_generation.c:295. */
static int resolve_token_iid_2022(iid_t int_id, token_generation_2022_t *token)
{
  unsigned char bVar1;
  unsigned int uVar2;
  int iVar3;
  token_action_2022_t tVar4;
  unsigned char bVar5;
  char cVar6;
  char cVar7;
  char cVar8;
  int iVar9;
  card_instance_t *instance;
  int iVar11;
  int iVar12;
  uint32_t uVar13;
  card_data_t *pcVar14;
  int *piVar15;
  card_data_t *pcVar16;
  bool bVar17;
  unsigned char bVar18;
  unsigned int uStack_6c;
  unsigned int uStack_64;
  unsigned int uStack_58;
  int dynused[16];

  bVar18 = 0;
  iVar9 = get_internal_card_id_from_csv_id((unsigned int)cards_data[int_id].id);
  iVar11 = engine_dynamic_iid_start_2022;
  if ((cards_data[iVar9].cc[2] != '\x03') && (cards_data[iVar9].type != 0x80))
  {
    iVar9 = engine_dynamic_iid_start_2022 + 0x10;
    iVar3 = engine_dynamic_iid_start_2022;
    do
    {
      iVar12 = iVar3;
      uVar2 = (uint32_t)cards_data[iVar12].id | ((uint32_t)cards_data[iVar12].reserved2 << 16);
      if (((((uint16_t)uVar2 == cards_data[int_id].id) && (token->id == (uVar2 & 0xffff))) &&
           ((token->pow < 0 || (token->pow == (int)cards_data[iVar12].power)))) &&
          (((token->tou < 0 || (token->tou == (int)cards_data[iVar12].toughness)) &&
            ((cards_data[iVar12].static_ability & token->key_plus) != 0))))
      {
        bVar1 = 1;
        if ((token->action & TOKEN2022_ACTION_CONVERT_INTO_ARTIFACT) != 0)
        {
          bVar1 = cards_data[iVar12].type >> 6 & 1;
        }
        uStack_58 = (unsigned int)cards_data[iVar12].color;
        bVar5 = token->color_forced;
        if (token->color_forced == 0)
        {
          bVar5 = cards_data[iVar12].color;
        }
        uStack_6c = (unsigned int)bVar5;
        if (((-1 < token->s_player) && (-1 < token->s_card)) && (token->no_sleight == '\0'))
        {
          uStack_6c = get_sleighted_color_test(token->s_player, token->s_card, bVar5 & 0x3e);
          uStack_58 = (unsigned int)cards_data[iVar12].color;
        }
        if ((uStack_58 == uStack_6c) && (bVar1 != 0))
        {
          return iVar12;
        }
      }
      iVar3 = iVar12 + 1;
    } while (iVar12 + 1 != iVar9);
    bVar1 = cards_data[int_id].color;
    if (token->color_forced != 0)
    {
      bVar1 = token->color_forced;
    }
    uStack_64 = (unsigned int)bVar1;
    if (((-1 < token->s_player) && (-1 < token->s_card)) && (token->no_sleight == '\0'))
    {
      uStack_64 = get_sleighted_color_test(token->s_player, token->s_card, uStack_64 & 0x3e);
    }
    if (((((-1 < token->pow) && (token->pow != (int)cards_data[int_id].power)) ||
          ((-1 < token->tou && (token->tou != (int)cards_data[int_id].toughness)))) ||
         ((token->key_plus != 0 && (token->key_plus != cards_data[int_id].static_ability)))) ||
        ((cards_data[int_id].color != uStack_64 ||
          ((((cards_data[int_id].type & 0x40) == 0 &&
             ((token->action & TOKEN2022_ACTION_CONVERT_INTO_ARTIFACT) != 0)) ||
            ((token->action & TOKEN2022_ACTION_ISNT_LEGENDARY) != 0))))))
    {
      uStack_58 = 0;
      piVar15 = dynused;
      for (iVar9 = 0x10; iVar9 != 0; iVar9 = iVar9 + -1)
      {
        *piVar15 = 0;
        piVar15 = piVar15 + (unsigned int)bVar18 * -2 + 1;
      }
      do
      {
        iVar9 = 0;
        do
        {
          while (true)
          {
            instance = get_card_instance(uStack_58, iVar9);
            uVar13 = instance->original_internal_card_id;
            if (uVar13 == 0xffffffff)
            {
              uVar13 = (uint32_t)instance->backup_internal_card_id;
            }
            if ((iVar11 <= (int)uVar13) && ((int)uVar13 <= iVar11 + 0xf))
            {
              dynused[uVar13 - iVar11] = 1;
            }
            iVar3 = instance->dummy3;
            if ((iVar11 + 0xf < iVar3 || iVar3 < iVar11) || (uVar13 == 0))
              break;
            iVar9 = iVar9 + 1;
            dynused[iVar3 - iVar11] = 1;
            if (iVar9 == 0x96)
              goto LAB_0266519c;
          }
          iVar9 = iVar9 + 1;
        } while (iVar9 != 0x96);
      LAB_0266519c:
        uStack_58 = uStack_58 + 1;
        iVar9 = iVar11;
        if (uStack_58 == 2)
        {
          do
          {
            if (((dynused[iVar9 - iVar11] == 0) && (engine_dynamic_iid_start_2022 <= iVar9)) &&
                (iVar9 <= engine_dynamic_iid_start_2022 + 0xf))
            {
              cards_data[iVar9].id = 0xffff;
            }
            bVar17 = iVar12 != iVar9;
            iVar9 = iVar9 + 1;
          } while (bVar17);
          iVar11 = create_a_card_type(int_id);
          cards_data[iVar11] = cards_data[int_id];
          iVar9 = token->pow;
          if ((-1 < iVar9) && (iVar9 != cards_data[int_id].power))
          {
            cards_data[iVar11].power = (int16_t)iVar9;
          }
          iVar9 = token->tou;
          if ((-1 < iVar9) && (iVar9 != cards_data[int_id].toughness))
          {
            cards_data[iVar11].toughness = (int16_t)iVar9;
          }
          if (token->key_plus != 0)
          {
            cards_data[iVar11].static_ability = cards_data[iVar11].static_ability | token->key_plus;
          }
          tVar4 = token->action;
          if (((cards_data[int_id].type & 0x40) == 0) &&
              ((tVar4 & TOKEN2022_ACTION_CONVERT_INTO_ARTIFACT) != 0))
          {
            cards_data[iVar11].type = cards_data[iVar11].type | 0x40;
          }
          if ((tVar4 & TOKEN2022_ACTION_ISNT_LEGENDARY) != 0)
          {
            cards_data[iVar11].subtype = '\0';
          }
          cards_data[iVar11].color = (uint8_t)uStack_64;
          return iVar11;
        }
      } while (true);
    }
  }
  return int_id;
}

/* ManalinkEh 2022: 0x2665380; original functions/token_generation.c:218. */
static int get_copyable_special_infos_part_2022(int t_player, int t_card)
{
  unsigned int uVar1;
  card_instance_t *instance;
  int result;
  unsigned int uVar4;

  instance = get_card_instance(t_player, t_card);
  uVar1 = instance->targets[0xe].player;
  uVar4 = (int)(uVar1 ^ 0xffffffff) >> 0x1f & uVar1;
  if ((int)uVar1 < 1)
  {
    return uVar4;
  }
  result = get_backup_id(t_player, t_card);
  if (result < 0x493b)
  {
    if (0x4902 < result)
    {
      switch (result)
      {
      case 0x4903:
      case 0x4905:
      case 0x4916:
      case 0x4925:
        goto LAB_02665409;
      default:
        goto LAB_026653e3;
      case 0x491f:
        goto LAB_0266545b;
      case 0x4926:
      case 0x493a:
      code_r0x0266544e:
        if (uVar1 != 0x43)
        {
          return 0;
        }
        return uVar4;
      }
    }
    if (result != 0x38e)
    {
    LAB_026653e3:
      return 0;
    }
  }
  else
  {
    if (result == 0x496c)
    {
    LAB_0266545b:
      if (1 < uVar4 - 0x42)
      {
        return 0;
      }
      return uVar4;
    }
    if (result < 0x496d)
    {
      if (result != 0x4951)
      {
        if (result != 0x4952)
        {
          return 0;
        }
        goto code_r0x0266544e;
      }
    }
    else
    {
      if (result == 0x49a8)
        goto LAB_0266545b;
      if (result < 0x49a9)
      {
        if ((result - 0x497dU & 0xffffffef) != 0)
        {
          return 0;
        }
      }
      else if (result != 0x500b)
      {
        return 0;
      }
    }
  }
LAB_02665409:
  if (uVar1 == 0x42)
  {
    return uVar4;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2665580; original functions/token_generation.c:62. */
void copy_token_definition_2022(int player, int card, token_generation_2022_t *token, int t_player,
                                int t_card)
{
  special_flags2_t sVar1;
  int result;
  card_instance_t *instance;
  unsigned int uVar4;

  result = get_id(t_player, t_card);
  default_token_definition_2022(player, card, result, token);
  token->no_sleight = '\x01';
  if (cards_ptr[result]->card_type == 8)
  {
    instance = get_card_instance(t_player, t_card);
    result = check_special_flags_2022(t_player, t_card, 0x40000000);
    if (result == 0)
    {
      token->s_player = (int)instance->damage_source_player;
      token->s_card = instance->damage_source_card;
    }
    else
    {
      token->s_card = -1;
      token->s_player = -1;
    }
    sVar1 = instance->ext->special_flags2;
    token->s_key_plus = instance->ext->sp_keywords;
    token->special_flags2 = sVar1;
    if ((t_player < 0) || (t_card < 0))
    {
      uVar4 = 0;
    }
    else
    {
      uVar4 = get_copyable_special_infos_part_2022(t_player, t_card);
    }
    token->special_infos = uVar4;
    result = check_special_flags2_2022(t_player, t_card, 0x80);
    if (result != 0)
    {
      token->special_flags2 = 0x80;
      return;
    }
  }
  return;
}

/* ManalinkEh 2022: 0x2665820; original functions/token_generation.c:124. */
int token_characteristic_setting_effects_2022(int player, int card, event_t event, int t_player,
                                              int t_card)
{
  special_flags2_t value;
  int result;
  card_instance_t *instance;

  if (card != -1)
  {
    result = check_special_flags2_2022(player, card, 0x400);
    if (result == 0)
    {
      instance = get_card_instance(player, card);
      special_abilities(t_player, t_card, event, instance->token_special_keywords_2022, t_player,
                        t_card);
      if ((((event == EVENT_CHANGE_TYPE) && (t_card == affected_card)) &&
           (t_player == affected_card_controller)) &&
          (value = instance->ext->special_flags2, 0 < value))
      {
        if ((land_can_be_played & 0x20000U) == 0)
        {
          land_can_be_played = land_can_be_played | 0x10000;
        }
        else
        {
          set_special_flags2_2022(t_player, t_card, value);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2665900; original functions/token_generation.c:146. */
int generic_token_2022(int player, int card, event_t event)
{
  int result;
  result = token_characteristic_setting_effects_2022(player, card, event, player, card);
  return result;
}


/* October 2022 token ID/copy entry points. */
static int generate_token_by_id_part_2022(int player, int card, int csvid, int number, int reserved);
static void copy_token_part_2022(int player, int card, int tok_player, int tok_card);
void copy_token_2022(int player, int card, int tok_player, int tok_card);
int generate_token_by_id_2022(int player, int card, int csvid);
void generate_tokens_by_id_2022(int player, int card, int csvid, int howmany);
int generate_reserved_token_by_id_2022(int player, int csvid);

/* ManalinkEh 2022: 0x2666580; original functions/token_generation.c:624. */
static int generate_token_by_id_part_2022(int player, int card, int csvid, int number, int reserved)
{
  int iVar1;
  int iid;
  int iVar2;
  token_generation_2022_t token;

  default_token_definition_2022(player, card, csvid, &token);
  if (reserved == 0)
  {
    iVar1 = -1;
    token.qty = (unsigned char)number;
    generate_token_2022(&token);
  }
  else
  {
    token.no_sleight = '\x01';
    iVar1 = get_internal_card_id_from_csv_id(csvid);
    iid = resolve_token_iid_2022(iVar1, &token);
    if (number < 1)
    {
      return -1;
    }
    iVar2 = 0;
    do
    {
      iVar2 = iVar2 + 1;
      iVar1 = add_card_to_hand(player, iid);
      set_token_2022(player, iVar1, &token);
      put_into_play(player, iVar1);
    } while (number != iVar2);
  }
  return iVar1;
}

/* ManalinkEh 2022: 0x2666640; original functions/token_generation.c:614. */
static void copy_token_part_2022(int player, int card, int tok_player, int tok_card)
{
  token_generation_2022_t token;
  copy_token_definition_2022(player, card, &token, tok_player, tok_card);
  generate_token_2022(&token);
  return;
}

/* ManalinkEh 2022: 0x2666680; original functions/token_generation.c:614. */
void copy_token_2022(int player, int card, int tok_player, int tok_card)
{
  if ((-1 < tok_player) && (-1 < tok_card))
  {
    copy_token_part_2022(player, card, tok_player, tok_card);
    return;
  }
  return;
}

/* ManalinkEh 2022: 0x26666b0; original functions/token_generation.c:649. */
int generate_token_by_id_2022(int player, int card, int csvid)
{
  int result;
  unsigned int reserved;

  result = get_internal_card_id_from_csv_id(csvid);
  if (cards_data[result].cc[2] == '\x03')
  {
    reserved = 1;
  }
  else
  {
    reserved = (unsigned int)(cards_data[result].type == 0x80);
  }
  result = generate_token_by_id_part_2022(player, card, csvid, 1, reserved);
  return result;
}

/* ManalinkEh 2022: 0x2666720; original functions/token_generation.c:653. */
void generate_tokens_by_id_2022(int player, int card, int csvid, int howmany)
{
  int result;
  unsigned int reserved;

  result = get_internal_card_id_from_csv_id(csvid);
  if (cards_data[result].cc[2] == '\x03')
  {
    reserved = 1;
  }
  else
  {
    reserved = (unsigned int)(cards_data[result].type == 0x80);
  }
  if (0 < howmany)
  {
    generate_token_by_id_part_2022(player, card, csvid, howmany, reserved);
    return;
  }
  return;
}

/* ManalinkEh 2022: 0x2666790; original functions/token_generation.c:657. */
int generate_reserved_token_by_id_2022(int player, int csvid)
{
  int result;

  result = get_internal_card_id_from_csv_id(csvid);
  if ((cards_data[result].cc[2] != '\x03') && (cards_data[result].type != 0x80))
  {
    backtrace_and_abort("functions/token_generation.c:658\nis_reserved_id(csvid)");
  }
  result = generate_token_by_id_part_2022(player, -1, csvid, 1, 1);
  return result;
}


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2665690; original functions/token_generation.c:87. */
int copy_token_characteristics_for_clone_2022(int player, int card, int t_player, int t_card)
{
  uint8_t uVar1;
  int32_t iVar2;
  unsigned char bVar3;
  int iVar4;
  card_instance_t *pcVar5;
  card_instance_t *pcVar6;
  uint32_t uStack_14;

  iVar4 = get_id(t_player, t_card);
  uStack_14 = 0;
  if (cards_ptr[iVar4]->card_type == 8)
  {
    pcVar5 = get_card_instance(player, card);
    pcVar6 = get_card_instance(t_player, t_card);
    uStack_14 = pcVar6->original_internal_card_id;
    if (uStack_14 == 0xffffffff)
    {
      uStack_14 = (uint32_t)pcVar6->backup_internal_card_id;
    }
    iVar2 = pcVar6->targets[7].player;
    pcVar5->targets[7].card = pcVar6->targets[7].card;
    pcVar5->targets[7].player = iVar2;
    iVar2 = pcVar6->targets[8].card;
    pcVar5->targets[8].player = pcVar6->targets[8].player;
    pcVar5->targets[8].card = iVar2;
    uVar1 = cards_data[uStack_14].color;
    iVar4 = get_internal_card_id_from_csv_id(iVar4);
    if (uVar1 == cards_data[iVar4].color)
    {
      bVar3 = pcVar5->initial_color & 0x3e;
    }
    else
    {
      bVar3 = cards_data[uStack_14].color & 0x3e;
      pcVar5->initial_color = bVar3;
    }
    pcVar5->color = bVar3;
    pcVar5->regen_status = pcVar5->regen_status | 0x10000000;
    if ((t_player < 0) || (t_card < 0))
    {
      iVar4 = 0;
    }
    else
    {
      iVar4 = get_copyable_special_infos_part_2022(t_player, t_card);
    }
    pcVar5 = get_card_instance(player, card);
    pcVar5->targets[0xe].player = iVar4;
    iVar4 = check_special_flags2_2022(t_player, t_card, 0x80);
    uStack_14 = 1;
    if (iVar4 != 0)
    {
      uStack_14 = 1;
      set_special_flags2_2022(player, card, 0x80);
    }
  }
  return uStack_14;
}

#endif
