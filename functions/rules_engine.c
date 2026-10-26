#include <ctype.h>
#include <limits.h>

#include "manalink.h"

static void increase_trap_condition_impl(int player, int trap, int val, int add);

/* Canonical recovered helper declarations. */
int get_stormcolor_count(int player, color_t clr);
void increment_storm_count(int player, int mode);
int get_specific_storm_count(int player);
int get_storm_count(void);
int get_stormcreature_count(int k);

/* October 2022 independent game-mode flags. */
other_game_mode_t other_game_mode;

extern int mulligans_complete[2];

int hack_allow_sorcery_if_rules_engine_is_only_effect = 0;

static int vanguard_avatar[2][10];
static int vcount[2] = {0, 0};
#ifdef CARDS_IN_HAND_THIS_TURN	// unused
static int cards_in_hand_this_turn[2][100];
#endif
static int settings[SETTING_MAX_PLUS_1] = { 0 };
static event_t actual_event = -1;
static int challenge = 0;
static int rules_engine_card[2] = {-1,-1};
static int deadbox_card[2] = {-1,-1};
static int starting_life_total[2] = {20,20};
static int force_init_upkeep = 0;

/* Current targets usage
 * [0], [1], [2], [4]:	activate_cards_in_hand().  No need to exhaustively sort them out just yet.
 * [3]:					Unused, but currently set to -1 in activate_cards_in_hand()'s EVENT_ACTIVATE handler.
 * [5]-[17]:			Assigned in rules_engine_globals_t.
 * [18]:				Counter storage. */

// Storage is contiguous with Rules Engine's full targets array.
typedef struct
{
  target_t targets[5];	// For use in activate_cards_in_hand().
  uint16_t shorts_per_turn[TRAP_NUMSHORTS_SHORTS_PER_TURN];
  uint8_t unassigned1[20];	// Unassigned storage is spaced out so that values of any given size can be added without invalidating savegames.
  uint16_t shorts_persistent[TRAP_NUMSHORTS_SHORTS_PERSISTENT];
  uint8_t unassigned2[16];
  uint8_t bytes_per_turn[TRAP_NUMBYTES_BYTES_PER_TURN];
  uint8_t unassigned3[10];
  uint8_t bytes_persistent[TRAP_NUMBYTES_BYTES_PERSISTENT];
  uint8_t unassigned4[6];
  uint8_t nybbles_per_turn[TRAP_NUMBYTES_NYBBLES_PER_TURN];
  uint8_t unassigned5[5];
  uint8_t bits_per_turn[TRAP_NUMBYTES_BITS_PER_TURN];
  uint8_t unassigned6[3];
  uint8_t bits_persistent[TRAP_NUMBYTES_BITS_PERSISTENT];
  uint8_t unassigned7[3];
  target_t counters;
} PACKED rules_engine_globals_t;

STATIC_ASSERT(sizeof(rules_engine_globals_t) == sizeof(((card_instance_t*)(NULL))->targets), sizeof_rules_engine_globals_t_must_match_target_array);

int get_setting(int thing){
	return settings[thing];
}


// exe interface - no longer used
void set_draft(void){
}
void set_momir(void){
}
void set_testing(void){
}
void set_none(void){
}

int get_momir(void){
	return singleplayer_mode == SPM_MOMIR;
}

static int get_challenge(void){
	return challenge;
}

void set_challenge(int m){
	challenge = m;
	set_challenge_round_to_0();
	ante = 0;
}

int unlimited_allowed_in_deck(int csvid)
{
  static int unlimited_list[] =
	{
	  CARD_ID_FOREST, CARD_ID_ISLAND, CARD_ID_MOUNTAIN, CARD_ID_PLAINS, CARD_ID_SWAMP,
	  CARD_ID_SNOW_COVERED_FOREST, CARD_ID_SNOW_COVERED_ISLAND, CARD_ID_SNOW_COVERED_MOUNTAIN, CARD_ID_SNOW_COVERED_PLAINS, CARD_ID_SNOW_COVERED_SWAMP,
	  CARD_ID_PERSISTENT_PETITIONERS,
	  CARD_ID_RAT_COLONY,
	  CARD_ID_RELENTLESS_RATS,
	  CARD_ID_SHADOWBORN_APOSTLE,
	  CARD_ID_WASTES
	};

  unsigned int i;
  for (i = 0; i < sizeof(unlimited_list) / sizeof(int); ++i)
	if (unlimited_list[i] == csvid)
	  return 1;

  return 0;
}

static int check_deck_legality(void){
	int vintage_legal = 1;
	int legacy_legal = 1;
	int illegal_card = -1;
	static int restricted_cards_vintage[] =
		{
		 CARD_ID_ANCESTRAL_RECALL, CARD_ID_BALANCE, CARD_ID_BLACK_LOTUS, CARD_ID_BRAINSTORM,
		 CARD_ID_CHALICE_OF_THE_VOID, CARD_ID_CHANNEL, CARD_ID_DEMONIC_CONSULTATION, CARD_ID_DEMONIC_TUTOR,
		 CARD_ID_DIG_THROUGH_TIME, CARD_ID_FASTBOND, CARD_ID_FLASH, CARD_ID_GITAXIAN_PROBE, CARD_ID_GUSH,
		 CARD_ID_IMPERIAL_SEAL, CARD_ID_LIBRARY_OF_ALEXANDRIA, CARD_ID_LIONS_EYE_DIAMOND, CARD_ID_LODESTONE_GOLEM,
		 CARD_ID_LOTUS_PETAL, CARD_ID_MANA_CRYPT, CARD_ID_MANA_VAULT, CARD_ID_MEMORY_JAR, CARD_ID_MERCHANT_SCROLL,
		 CARD_ID_MINDS_DESIRE, CARD_ID_MOX_EMERALD, CARD_ID_MOX_JET, CARD_ID_MOX_PEARL, CARD_ID_MOX_RUBY,
		 CARD_ID_MOX_SAPPHIRE, CARD_ID_MYSTICAL_TUTOR, CARD_ID_NECROPOTENCE, CARD_ID_PONDER, CARD_ID_SOL_RING,
		 CARD_ID_STRIP_MINE, CARD_ID_THORN_OF_AMETHYST, CARD_ID_TIME_VAULT, CARD_ID_TIME_WALK, CARD_ID_TIMETWISTER,
		 CARD_ID_TINKER, CARD_ID_TOLARIAN_ACADEMY, CARD_ID_TREASURE_CRUISE, CARD_ID_TRINISPHERE,
		 CARD_ID_VAMPIRIC_TUTOR, CARD_ID_WHEEL_OF_FORTUNE, CARD_ID_WINDFALL, CARD_ID_YAWGMOTHS_WILL
		};

	static int banned_cards_vintage[] =
		{
		 // Miscellaneous
		 CARD_ID_CHAOS_ORB, CARD_ID_FALLING_STAR, CARD_ID_SHAHRAZAD,

		 // Ante
		 CARD_ID_AMULET_OF_QUOZ, CARD_ID_BRONZE_TABLET, CARD_ID_CONTRACT_FROM_BELOW, CARD_ID_DARKPACT,
		 CARD_ID_DEMONIC_ATTORNEY, CARD_ID_JEWELED_BIRD, CARD_ID_REBIRTH, CARD_ID_TEMPEST_EFREET,
		 CARD_ID_TIMMERIAN_FIENDS,

		 // Un-sets.  Only these three are currently implemented in Manalink.
		 CARD_ID_DUH, CARD_ID_GOBLIN_TUTOR, CARD_ID_JACK_IN_THE_MOX
		};

	static int banned_cards_legacy[] =
		{
		 // Ante
		 CARD_ID_AMULET_OF_QUOZ, CARD_ID_BRONZE_TABLET, CARD_ID_CONTRACT_FROM_BELOW, CARD_ID_DARKPACT,
		 CARD_ID_DEMONIC_ATTORNEY, CARD_ID_JEWELED_BIRD, CARD_ID_REBIRTH, CARD_ID_TEMPEST_EFREET,
		 CARD_ID_TIMMERIAN_FIENDS,

		 CARD_ID_ANCESTRAL_RECALL, CARD_ID_BALANCE, CARD_ID_BAZAAR_OF_BAGHDAD, CARD_ID_BLACK_LOTUS,
		 CARD_ID_CHANNEL, CARD_ID_CHAOS_ORB, CARD_ID_DEATHRITE_SHAMAN, CARD_ID_DEMONIC_CONSULTATION,
		 CARD_ID_DEMONIC_TUTOR, CARD_ID_DIG_THROUGH_TIME, CARD_ID_EARTHCRAFT, CARD_ID_FALLING_STAR, CARD_ID_FASTBOND,
		 CARD_ID_FLASH, CARD_ID_FRANTIC_SEARCH, CARD_ID_GITAXIAN_PROBE, CARD_ID_GOBLIN_RECRUITER, CARD_ID_GUSH,
		 CARD_ID_HERMIT_DRUID, CARD_ID_IMPERIAL_SEAL, CARD_ID_LIBRARY_OF_ALEXANDRIA, CARD_ID_MANA_CRYPT,
		 CARD_ID_MANA_DRAIN, CARD_ID_MANA_VAULT, CARD_ID_MEMORY_JAR, CARD_ID_MENTAL_MISSTEP, CARD_ID_MIND_TWIST,
		 CARD_ID_MINDS_DESIRE, CARD_ID_MISHRAS_WORKSHOP, CARD_ID_MOX_EMERALD, CARD_ID_MOX_JET, CARD_ID_MOX_PEARL,
		 CARD_ID_MOX_RUBY, CARD_ID_MOX_SAPPHIRE, CARD_ID_MYSTICAL_TUTOR, CARD_ID_NECROPOTENCE, CARD_ID_OATH_OF_DRUIDS,
		 CARD_ID_SENSEIS_DIVINING_TOP, CARD_ID_SHAHRAZAD, CARD_ID_SKULLCLAMP, CARD_ID_SOL_RING, CARD_ID_STRIP_MINE,
		 CARD_ID_SURVIVAL_OF_THE_FITTEST, CARD_ID_TIME_VAULT, CARD_ID_TIME_WALK, CARD_ID_TIMETWISTER, CARD_ID_TINKER,
		 CARD_ID_TOLARIAN_ACADEMY, CARD_ID_TREASURE_CRUISE, CARD_ID_VAMPIRIC_TUTOR, CARD_ID_WHEEL_OF_FORTUNE,
		 CARD_ID_WINDFALL, CARD_ID_YAWGMOTHS_BARGAIN, CARD_ID_YAWGMOTHS_WILL,

		 // Un-sets.  Only these three are currently implemented in Manalink.
		 CARD_ID_DUH, CARD_ID_GOBLIN_TUTOR, CARD_ID_JACK_IN_THE_MOX
		};

	int *deck = deck_ptr[HUMAN];
	int new_deck[500];
	int i;
	unsigned int j=0;
	for(i = 0; i < 500; i++){
		new_deck[i] = -1;
	}
	for(i = 0; i < 500; i++){
		if( deck[i] > -1 ){
			new_deck[i] = cards_data[ deck[i] ].id;
		}
		else{
			new_deck[i] = -1;
			j = i;
			i = 500;
		}
	}
	int count = 0;
	/*
	while( count < active_cards_count[HUMAN]){
		if(! in_play(HUMAN, count) ){
			new_deck[j++] = get_id(HUMAN, count);
		}
		count++;
	}
	*/

	// deck too small
	if( count_deck(HUMAN) < 52 && gauntlet_round == 0 ){
		return 0;
	}

	// This counts the number of every single card in existence (!) in the deck, rather than just the cards in the deck.
	for(i=0;i<available_slots;i++){
		count = 0;
		for(j = 0;j<500;j++){
			if( new_deck[j] == i ){ count++; }
		}

		if( count > 4 ){
			if (!unlimited_allowed_in_deck(i)){
				int card_added =  add_card_to_hand(HUMAN, get_internal_card_id_from_csv_id(i)  );
				card_instance_t *target = get_card_instance(HUMAN, card_added);
				target->state |= STATE_INVISIBLE;
				--hand_count[HUMAN];
				return card_added;
			}
		}

		if( vintage_legal && count > 1 ){
			for(j = 0;j<sizeof(restricted_cards_vintage)/sizeof(int);j++){
				if( restricted_cards_vintage[j] == i ){
					vintage_legal = 0;
					illegal_card = i;
				}
			}
		}

		if( vintage_legal && count > 0 ){
			for(j = 0;j<sizeof(banned_cards_vintage)/sizeof(int);j++){
				if( banned_cards_vintage[j] == i ){
					vintage_legal = 0;
					illegal_card = i;
				}
			}
		}

		if( legacy_legal && count > 0 ){
			for(j = 0;j<sizeof(banned_cards_legacy)/sizeof(int);j++){
				if( banned_cards_legacy[j] == i ){
					legacy_legal = 0;
					illegal_card = i;
				}
			}
		}
	}

	if( legacy_legal || vintage_legal ){
		return -1;
	}
	int card_added =  add_card_to_hand(HUMAN, get_internal_card_id_from_csv_id(illegal_card)  );
	card_instance_t *target = get_card_instance(HUMAN, card_added);
	target->state |= STATE_INVISIBLE;
	--hand_count[HUMAN];
	return card_added;
}

void read_settings(void){
	// Only read config once
	if (settings[SETTING_SETTING_FILE_READ] == 0){
		settings[SETTING_SETTING_FILE_READ] = 1;

		// Default values
		settings[SETTING_ABILITY_ICON_FILE] = (int)("Abilities.pic");
		settings[SETTING_ABILITY_ICON_MAX] = 26;
		settings[SETTING_HAND_SIZE] = 8;
		settings[SETTING_MIN_PICS_TO_RANDOMIZE] = 15;
		settings[SETTING_RULES_ENGINE] = 1;

		// if the config file exists, then read in the config settings
		char buffer[200];
		FILE* file = fopen("config.txt", "r");
		if( file != NULL ){
			settings[SETTING_SETTING_FILE_READ] = 2;
			while( fgets(buffer, 200, file) ){
				const char* p;
				// skip whitespace
				for (p = buffer; *p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'); ++p) {}

				switch (*p){
#define CFG(str, settingnum)													\
						if (strncasecmp(p, str ":", strlen(str ":")) == 0){		\
							settings[settingnum] = atoi(p + strlen(str ":"));	\
							continue;											\
						}
#define CFGSTR(str, settingnum)													\
						if (strncasecmp(p, str ":", strlen(str ":")) == 0){		\
							const char* q = p + strlen(str ":");				\
							while (isspace(*q))									\
								++q;											\
							char* dup = strdup(q);								\
							char* r = dup + strlen(dup) - 1;					\
							while (r > dup && isspace(*r)){						\
								*r = 0;											\
								--r;											\
							}													\
							if (*dup)											\
								settings[settingnum] = (int)dup;				\
							continue;											\
						}
					case '/': case 0:
						// Ignore blank lines and lines starting with /
						continue;
					case 'a': case 'A':
						CFGSTR("AbilityIconFile", SETTING_ABILITY_ICON_FILE);
						CFG("AbilityIconMax", SETTING_ABILITY_ICON_MAX);
						CFG("AbilityIconSize", SETTING_ABILITY_ICON_SIZE);
						CFG("AiDecisionTime", SETTING_AI_DECISION_TIME);
						CFG("AiUsesRulesEngine", SETTING_AI_USES_RULES_ENGINE);
						CFG("AlphabetizeDeck", SETTING_ALPHABETIZE_DECK);
						CFG("Archenemy", SETTING_ARCHENEMY);
						CFG("AttachEquipment", SETTING_ATTACH_EQUIPMENT);
						CFG("AutoHaste", SETTING_AUTO_HASTE);
						CFG("AutoMill", SETTING_AUTO_MILL);
						CFG("AutotapArtifacts", SETTING_AUTOTAP_ARTIFACTS);
						CFG("AutotapCreatures", SETTING_AUTOTAP_CREATURES);
						break;
					case 'c': case 'C':
						CFG("ChallengeMode", SETTING_CHALLENGE_MODE);
						CFG("CheckDeckLegality", SETTING_CHECK_DECK_LEGALITY);
						CFG("CrippleAI", SETTING_CRIPPLE_AI);
						break;
					case 'd': case 'D':
						CFG("DuhMode", SETTING_DUH_MODE);
						CFG("Debug", SETTING_DEBUG);
						break;
					case 'h': case 'H':
						CFG("HandSize", SETTING_HAND_SIZE);
						break;
					case 'm': case 'M':
						CFG("ManaBurn", SETTING_MANA_BURN);
						CFG("MinPicsToRandomize", SETTING_MIN_PICS_TO_RANDOMIZE);
						break;
					case 'q': case 'Q':
						CFG("QuickStart", SETTING_QUICK_START);
						break;
					case 'r': case 'R':
						CFG("RulesEngine", SETTING_RULES_ENGINE);
						break;
					case 's': case 'S':
						CFG("SmallCardSize", SETTING_SMALLCARD_SIZE);
						CFG("SmartTarget", SETTING_SMART_TARGET);
						break;
					case 't': case 'T':
						CFG("TournamentMode", SETTING_TOURNAMENT_MODE);
						break;
					case 'u': case 'U':
						CFG("UnlimitedMana", SETTING_UNLIMITED_MANA);
						break;
					case 'v': case 'V':
						CFG("VerifyHandCount", SETTING_VERIFY_HAND_COUNT);
						break;
#undef CFG
				}
				popup("config.txt", "Unknown line in config.txt: \"%s\"", buffer);
			}
			fclose(file);
		}
	}
}

static void read_and_adjust_settings(void){

	read_settings();

	// if the challenge file exists, then add the challenge flag and remove debug modes
	if (settings[SETTING_CHALLENGE_MODE] || get_challenge()){
		settings[SETTING_CHALLENGE_MODE] = 1;
		debug_mode = 0;
		settings[SETTING_AUTO_MILL] = 0;
		settings[SETTING_UNLIMITED_MANA] = 0;
		settings[SETTING_AUTO_HASTE] = 0;
		settings[SETTING_QUICK_START] = 0;
		settings[SETTING_CRIPPLE_AI] = 0;
		settings[SETTING_DEBUG] = 0;
	} else {
		// Set exe debug mode
		if (settings[SETTING_DEBUG]){
			debug_mode = 1;
			trace_mode = 0;
		}
		// if we are testing, turn on crippleAI, autohaste, unlimited mana, and
		// quickstart and automill
		if (singleplayer_mode == SPM_TESTING){
			force_init_upkeep = 1;
			settings[SETTING_AUTO_MILL] = 1;
			settings[SETTING_UNLIMITED_MANA] = 1;
			settings[SETTING_AUTO_HASTE] = 1;
			settings[SETTING_QUICK_START] = 1;
			settings[SETTING_CRIPPLE_AI] = 1;
		}
	}

	/* Make sure rules_engine_card and deadbox_card globals are correct.  They need to be reset at each new game and each loaded game, since their placement
	 * will vary on how many times you mulligan and whether they were added midgame by a Wish card or such. */
	int p, c;
	for (p = 0; p <= 1; ++p){
		rules_engine_card[p] = -1;
		deadbox_card[p] = -1;
		for (c = 0; c < 150; ++c){
			if (in_play(p, c)){
				int csvid = get_id(p, c);
				if (csvid == CARD_ID_RULES_ENGINE){
					rules_engine_card[p] = c;
				}
				if (csvid == CARD_ID_DEADBOX){
					deadbox_card[p] = c;
				}
			}
		}
	}
}

static UnorderedMapIntInt card_image_numbers = NULL;

int get_card_image_number(int csvid, int player, int card)
{
  // 0x468160

  // This surely belongs somewhere else.  But so do settings.

  if (__builtin_expect(csvid < 0 || player < 0 || card < 0, 0))
	return 0;

  int num_pics = cards_ptr[csvid]->num_pics;
  if (__builtin_expect(num_pics <= 1, 1))
	return 0;

  if (__builtin_expect(num_pics < settings[SETTING_MIN_PICS_TO_RANDOMIZE], 1))
	return (card + player) % num_pics;

  int key = (LOWORD(csvid) << 16) | (BYTE0(player) << 8) | BYTE0(card);
  int* prev_value = UnorderedMapIntInt_fetch(&card_image_numbers, key);
  if (__builtin_expect(prev_value != NULL, 1))
	return *prev_value;

  return *(UnorderedMapIntInt_set(&card_image_numbers, key)) = internal_rand(num_pics);

  /* card_image_numbers should likely be cleared when loading a save game or when starting a new one, but the earliest existing injection at game start happens
   * after cards are drawn. */
}

void pregame(void){

	/* This called after *every card drawn* during the initial draw.
	 * There isn't really much of a better place to put it; any earlier and libraries won't have been initialized. */

	hack_allow_sorcery_if_rules_engine_is_only_effect = 0;

	if (game_type != GAMETYPE_SOLO_DUEL){
		singleplayer_mode = 0;	// Seems to be handled by the dialog code already, but I can't find it, and no reason to risk not clearing it.
		/* Recovered 2022 game-mode fragment: reset outside solo duel @ 0x26c7a80. */
		other_game_mode = OGM_NONE;
		/* End recovered 2022 game-mode fragment: reset outside solo duel. */
	}

	int i;
	for (i = 0; i < 50; ++i){
		is_unlocked(0, 0, 0, i);
	}
	// Check for vanguard avatars in the deck or already drawn
	int p, c;
	for (p = 0; p < 2; ++p){
		if( vcount[p] == 0 ){
			mulligans_complete[p] = 0;
			int *deck = deck_ptr[p];
			for (i = 0; i < 500 && deck[i] != -1; i++){
				if (cards_data[deck[i]].cc[2] == 3){
					int csvid = cards_data[deck[i]].id;
					if (csvid == CARD_ID_AVATAR_MOMIR_VIG){
						singleplayer_mode = SPM_MOMIR;
					}
					/* Recovered 2022 game-mode fragment: EDH avatar in deck @ 0x26c7a80. */
					if (csvid == CARD_ID_ELDER_DRAGON_HIGHLANDER)
					{
					  other_game_mode |= OGM_EDH;
					}
					/* End recovered 2022 game-mode fragment: EDH avatar in deck. */
					if( vcount[p] < 10 ){
						vanguard_avatar[p][vcount[p]] = csvid;
						vcount[p]++;
					}
				}
			}
			for (c = 0; c < active_cards_count[p]; ++c){
				card_instance_t* instance = get_card_instance(p, c);
				if (instance->internal_card_id >= 0 && cards_data[instance->internal_card_id].cc[2] == 3){
					int csvid = cards_data[instance->internal_card_id].id;
					if (csvid == CARD_ID_AVATAR_MOMIR_VIG){
						singleplayer_mode = SPM_MOMIR;
					}
					/* Recovered 2022 game-mode fragment: EDH avatar already drawn @ 0x26c7a80. */
					if (csvid == CARD_ID_ELDER_DRAGON_HIGHLANDER)
					{
					  other_game_mode |= OGM_EDH;
					}
					/* End recovered 2022 game-mode fragment: EDH avatar already drawn. */
					if( vcount[p] < 10 ){
						vanguard_avatar[p][vcount[p]] = csvid;
						vcount[p]++;
					}
				}
			}
		}
	}


	// if the draft file exists, then add the draft avatar
	if (singleplayer_mode == SPM_DRAFT){
		vanguard_avatar[HUMAN][0] = CARD_ID_DRAFT;
		vcount[HUMAN] = 1;
		vcount[AI] = 0;
	}
	else if (singleplayer_mode == SPM_MOMIR){
		vanguard_avatar[HUMAN][0] = CARD_ID_AVATAR_MOMIR_VIG;
		vcount[HUMAN] = 1;
		vanguard_avatar[AI][0] = CARD_ID_AVATAR_MOMIR_VIG;
		vcount[AI] = 1;
	}

	settings[SETTING_GAMESTART_SETTINGS_APPLIED] = 0;
	read_and_adjust_settings();

	if( vcount[AI] == 0 && get_setting(SETTING_ARCHENEMY) && !get_setting(SETTING_CHALLENGE_MODE)){
		int draft_found = 0;
		int k;
		for(k=0; k<10; k++){
			if( vanguard_avatar[HUMAN][k] == CARD_ID_DRAFT ){
				draft_found++;
				break;
			}
		}
		if( ! draft_found ){
			vanguard_avatar[AI][0] = CARD_ID_ARCHENEMY;
			vcount[AI] = 1;
		}
	}
}

void after_load_game(void)
{
	hack_allow_sorcery_if_rules_engine_is_only_effect = 0;

	// Not strictly correct if the game was saved in turn one, but wouldn't have worked anyway since the other settings were off, too.
	// ...I think.  Challenge setting might get forced on, in which case, challenge_mode_upkeep() should still be called (once).
	// On yet a third hand, if that's true, it was *always* getting called the first turn after a load, which was even worse.
	settings[SETTING_GAMESTART_SETTINGS_APPLIED] = 1;

	read_and_adjust_settings();
}

int quick_start(void){

	if( vcount[HUMAN] ){
		return 1;
	}

	return get_setting(SETTING_QUICK_START);
}

// Has a code pointer assigned, but if the exe calls it, I can't find it.
int fake_random(int num){
	return 0;
}

int is_hidden_agenda(int csvid){
	switch( csvid ){
			case CARD_ID_CONSPIRACY_BRAGOS_FAVOR:
			case CARD_ID_CONSPIRACY_DOUBLE_STROKE:
			case CARD_ID_CONSPIRACY_IMMEDIATE_ACTION:
			case CARD_ID_CONSPIRACY_ITERATIVE_ANALYSIS:
			case CARD_ID_CONSPIRACY_MUZZIOS_PREPARATIONS:
			case CARD_ID_CONSPIRACY_SECRET_SUMMONING:
			case CARD_ID_CONSPIRACY_SECRETS_OF_PARADISE:
			case CARD_ID_CONSPIRACY_UNEXPECTED_POTENTIAL:
			case CARD_ID_CONSPIRACY_ADRIANAS_VALOR:
			case CARD_ID_CONSPIRACY_ASSEMBLE_THE_RANK_AND_VILE:
			case CARD_ID_CONSPIRACY_ECHOING_BOON:
			case CARD_ID_CONSPIRACY_HIRED_HEIST:
			case CARD_ID_CONSPIRACY_INCENDIARY_DISSENT:
			case CARD_ID_CONSPIRACY_NATURAL_UNITY:
			case CARD_ID_CONSPIRACY_SUMMONERS_BOND:
				return 1;
			default:
				return 0;
	}
	return 0;
}

static void vanguard_check(void){
	// put the vanguard avatars into play
	int p;
	for(p=0;p<2;p++){
		if( ! get_setting(SETTING_CHALLENGE_MODE) ){
			int k;
			for(k=0; k<vcount[p]; k++){
				if( vanguard_avatar[p][k] != -1 ){
					if( ! is_hidden_agenda(vanguard_avatar[p][k]) ){
						int card_added = add_card_to_hand(p, get_internal_card_id_from_csv_id(vanguard_avatar[p][k]));
						put_into_play(p, card_added);
						vanguard_avatar[p][k] = -1;
					}
					else{
						int card_added = add_card_to_hand(p, get_internal_card_id_from_csv_id(CARD_ID_CONSPIRACY_HIDDEN_AGENDA));
						card_instance_t *instance= get_card_instance(p, card_added);
						instance->targets[13].player = CARD_ID_CONSPIRACY_HIDDEN_AGENDA ;
						instance->targets[13].card = vanguard_avatar[p][k];
						put_into_play(p, card_added);
						vanguard_avatar[p][k] = -1;
					}
				}
			}
			vcount[p] = 0;
		}
		else{
			if( p == 0 ){
				apply_challenge_vanguard();
			}
		}
	}
}

// Perform any upkeep triggers for cards in the graveyard
static void resolve_graveyard_upkeep_triggers(int player, int card, event_t event ){
	card_instance_t *instance= get_card_instance(player, card);
	if( current_turn == player ){

		// if any abilities were found, do the upkeep trigger
		if(trigger_condition == TRIGGER_UPKEEP && affect_me(player, card) && reason_for_trigger_controller == player){
			int grave_count = count_graveyard(player);
			const int *graveyard = get_grave(player);
			int i = 0;
			int ability_found = 0;
			int c1 = count_upkeeps(player);
			while(i<grave_count && ability_found != 1){
					card_data_t* card_d = &cards_data[ graveyard[i] ];
					if( card_d->id == CARD_ID_DEATH_SPARK ){
						if( graveyard[i+1] != -1 && (cards_data[ graveyard[i+1] ].type & TYPE_CREATURE) ){
							ability_found = 1;
						}
					}
					else if( card_d->cc[2] == 5 || card_d->id == CARD_ID_ETERNAL_DRAGON || ( player == AI && card_d->id == CARD_ID_NIM_DEVOURER) ||
							( player == AI && card_d->id == CARD_ID_NECROSAVANT) || ( player == AI && card_d->id == CARD_ID_UNDEAD_GLADIATOR)
						 ){
							ability_found = 1;
					}
					i++;
			}
			if( ability_found && c1 > 0){
				if(event == EVENT_TRIGGER){
					event_result |= 2;
				}
				else if(event == EVENT_RESOLVE_TRIGGER){
						while( c1 > 0 ){
							i = grave_count-1;
							while( i > -1 ){
								card_data_t* card_d = &cards_data[ graveyard[i] ];
								if( card_d->id != CARD_ID_DEATH_SPARK ){
									if( card_d->cc[2] == 5 || card_d->id == CARD_ID_ETERNAL_DRAGON ||
										( player == AI && card_d->id == CARD_ID_NIM_DEVOURER) ||
										( player == AI && card_d->id == CARD_ID_NECROSAVANT)
									  ){
										// put a copy of the card into your hand
										int card_added =  add_card_to_hand(player, graveyard[i] );

										instance->number_of_targets = 1;
										card_instance_t *target = get_card_instance(player, card_added);
										target->state |= STATE_INVISIBLE;
										--hand_count[player];

										CardFunction ptFunction = card_d->code_pointer;
										int result = ptFunction(player, card_added, EVENT_GRAVEYARD_ABILITY_UPKEEP);
										if( result == -1 ){ // this card is removed
											remove_card_from_grave(player, i);
										}
										else if( result == -2 ){ // -2 means no card is removed
											obliterate_card_and_recycle(player, card_added);
										}
										else{  // another card was removed
											obliterate_card_and_recycle(player, card_added);
											remove_card_from_grave(player, result);
										}
									}
								}

								else{
										if( graveyard[i+1] != -1 && (cards_data[ graveyard[i+1] ].type & TYPE_CREATURE) ){
											int card_added =  add_card_to_hand(player, graveyard[i] );
											card_instance_t *target = get_card_instance(player, card_added);
											target->state |= STATE_INVISIBLE;
											--hand_count[player];

											CardFunction ptFunction = card_d->code_pointer;
											int result = ptFunction(player, card_added, EVENT_GRAVEYARD_ABILITY_UPKEEP);
											if( result == -1 ){ // this card is removed
												remove_card_from_grave(player, i);
											}
											else if( result == -2 ){ // -2 means no card is removed
													obliterate_card_and_recycle(player, card_added);
											}
											else{  // another card was removed
												obliterate_card_and_recycle(player, card_added);
												remove_card_from_grave(player, result);
											}
										}
								}
								i--;
							}
							c1--;
						}
				}
			}
		}
	}
}

static void resolve_dredge_triggers(int player, int card, event_t event ){
	int i;

	// Check for dredge
	if(trigger_condition == TRIGGER_REPLACE_CARD_DRAW && affect_me(player, card) && reason_for_trigger_controller == player && !suppress_draw){
		// see if there are any cards to dredge
		int grave_count = count_graveyard(player);
		const int *graveyard = get_grave(player);
		int ability_found = 0;
		char buffer[500];
		int pos = scnprintf(buffer, 500, "Dredge:\n Pass\n" );
		for(i=0;i<grave_count;i++){
			card_data_t* card_d = &cards_data[ graveyard[i] ];
			if( card_d->cc[2] == 6 ){
				CardFunction ptFunction = card_d->code_pointer;
				int result = ptFunction(player, 0, EVENT_DREDGE);
				if( count_deck(player) >= result ){
					ability_found = 1;
					card_ptr_t* c = cards_ptr[ card_d->id ];
					pos += scnprintf(buffer + pos, 500-pos, " %s (%d)\n", c->name, result );
				}
			}
		}

		if( ability_found ){
			if(event == EVENT_TRIGGER){
				event_result |= 2;
			}
			else if(event == EVENT_RESOLVE_TRIGGER){
				int choice = do_dialog(player, player, card, -1, -1, buffer, 1);
				if( choice != 0 ){
				   int valid = 0;
				   for(i=0;i<grave_count;i++){
						card_data_t* card_d = &cards_data[ graveyard[i] ];
						if( card_d->cc[2] == 6 ){
							CardFunction ptFunction = card_d->code_pointer;
							int result = ptFunction(player, 0, EVENT_DREDGE);
							if( count_deck(player) >= result && ++valid == choice ){
								from_grave_to_hand(player, i, TUTOR_HAND);
								mill( player, result );
								suppress_draw = 1;
							}
						}
					}
				}
			}
		}
	}
}
/*
static int is_unearth(const card_data_t* cd)
{
	* Unearth.  (cc[2] == 9 or Viscera Dragger), is type creature, and is not any of:
   * Carrionette
   * Champion of Stray Souls
   * Chronosavant
   * Firewing Phoenix
   * Gangrenous Goliath
   * Glory
   * Gravecrawler
   * Jarad, Golgari Lich Lord
   * Masked Admirers
   * Necrosavant
   * Rot Farm Skeleton
   * Salvage Titan
   * Sekki, Seasons' Guide
   * Shard Phoenix
   * Skaab Ruinator
   * Tymaret, the Murder King
   * Worldheart Phoenix
   *
   * Why in the bloody blue blazes are all of those *and* unearth *and* flashback *and* planeswalkers all sharing cc[2]==9?
   * There's 256 possible values and we've used only 20 of them.
   */
/*
  if (cd->cc[2] != 9 && cd->id != CARD_ID_VISCERA_DRAGGER)
	return 0;

  if (!(cd->type & TYPE_CREATURE))
	return 0;

  switch (cd->id)
	{
	  case CARD_ID_CHRONOSAVANT:
	  case CARD_ID_FIREWING_PHOENIX:
	  case CARD_ID_GANGRENOUS_GOLIATH:
	  case CARD_ID_GLORY:
	  case CARD_ID_GRAVECRAWLER:
	  case CARD_ID_JARAD_GOLGARI_LICH_LORD:
	  case CARD_ID_MASKED_ADMIRERS:
	  case CARD_ID_NECROSAVANT:
	  case CARD_ID_ROT_FARM_SKELETON:
	  case CARD_ID_SALVAGE_TITAN:
	  case CARD_ID_SEKKI_SEASONS_GUIDE:
	  case CARD_ID_SHARD_PHOENIX:
	  case CARD_ID_SKAAB_RUINATOR:
	  case CARD_ID_TYMARET_THE_MURDER_KING:
	  case CARD_ID_WORLDHEART_PHOENIX:
		return 0;
	}

  return 1;
}
*/

static int activate_cards_in_hand(int player, int card, event_t event){
#pragma message "All of the special cases here are *odious*.  Should be migrated to their card functions, checking the return value from the events to see what to do."
	if (IS_AI(player) && !get_setting(SETTING_AI_USES_RULES_ENGINE)){
		return 0;
	}
	card_instance_t *instance= get_card_instance(player, card);
	int homing_sliver_flag = get_trap_condition(player, TRAP_HOMING_SLIVER);

	// look for cards in hand that have abilities
	int i;
	if( event == EVENT_CAN_ACTIVATE ){
		//return 1;
		if( hand_count[player] > 0 ){
			for(i=0;i<active_cards_count[player]; i++){
				card_instance_t *hand_instance = get_card_instance(player, i);
				if( ! ( hand_instance->state & STATE_INVISIBLE ) && ! ( hand_instance->state & STATE_IN_PLAY )  ){
					int id = hand_instance->internal_card_id;
					if( id > -1 ){
						if( cards_data[id].cc[2] == 4 || cards_data[id].cc[2] == 16 || cards_data[id].cc[2] == 23 ||
						    get_id(player, i) == CARD_ID_DRAGON_WINGS ||
							(homing_sliver_flag && has_subtype(player, i, SUBTYPE_SLIVER))
						  ){
							CardFunction ptFunction = cards_data[id].code_pointer;
							if( ptFunction(player, i, EVENT_CAN_ACTIVATE_FROM_HAND ) ){
								return 1;
							}
						}
					}
				}
			}
		}

		// also check for cards in the graveyard
		const int *graveyard = get_grave(player);
		if( graveyard[0] != -1 ){
			int graveyard_count = count_graveyard(player);
			for (i = 0; i < graveyard_count; ++i){
				card_data_t* card_d = &cards_data[ graveyard[i] ];
				if( card_d->cc[2] == 9 || card_d->id == CARD_ID_VISCERA_DRAGGER || card_d->cc[2] == 10 || card_d->cc[2] == 17 ||
					card_d->id == CARD_ID_HAAKON_STROMGALD_SCOURGE || card_d->id == CARD_ID_SKARRGAN_FIREBIRD ||
					card_d->id == CARD_ID_MARSHALING_CRY || card_d->id == CARD_ID_UNDEAD_GLADIATOR || card_d->id == CARD_ID_VINEWEFT ||
					card_d->id == CARD_ID_GRYFFS_BOON || card_d->cc[2] == 21 || card_d->cc[2] == 23
				  ){
					int fake = add_card_to_hand(player, graveyard[i]);
					add_state(player, fake, STATE_INVISIBLE);
					--hand_count[player];

					CardFunction ptFunction = card_d->code_pointer;
					int result = ptFunction(player, fake, EVENT_GRAVEYARD_ABILITY);
					obliterate_card_and_recycle(player, fake);
					if( result ){
						if( result & GA_INTERRUPT_ABILITY){
							return 99;
						}
						return 1;
					}
				}
			}
		}

		if( check_rfg(player, CARD_ID_TORRENT_ELEMENTAL) && can_sorcery_be_played(player, event) && has_mana_hybrid(player, 2, COLOR_BLACK, COLOR_GREEN, 3)){
			return 1;
		}
		if( check_rfg(player, CARD_ID_ETERNAL_SCOURGE) && can_sorcery_be_played(player, event) ){
			if( has_mana_to_cast_iid(player, event, CARD_ID_ETERNAL_SCOURGE) ){
				return 1;
			}
		}
	}
	else if( event == EVENT_ACTIVATE ){
		hack_allow_sorcery_if_rules_engine_is_only_effect = 1;	// must not return from this function without clearing this
		unsigned int cb = 3000;
		char buffer[cb];
		int opts[100][2];
		int opts_count = 1;
		int pos = scnprintf(buffer, cb, " Cancel\n" );
		for (i = 0; i < active_cards_count[player]; i++){
			card_instance_t *hand_instance = get_card_instance(player, i);
			if( ! ( hand_instance->state & STATE_INVISIBLE ) && ! ( hand_instance->state & STATE_IN_PLAY )  ){
				int id = hand_instance->internal_card_id;
				if( id > -1 ){
					int can_be_activated = 0;
					int csvid = cards_data[id].id;
					switch( csvid ){
							case CARD_ID_MARSHALING_CRY:
							case CARD_ID_SILENT_BLADE_ONI:
							case CARD_ID_VISCERA_DRAGGER:
							case CARD_ID_DRAGON_WINGS:
								can_be_activated = 1;
								break;
							default:
								break;
					}
			        // Cycling / Basic landcycling / other activation from hand
					if( cards_data[id].cc[2] == 4 || cards_data[id].cc[2] == 23 ){
						can_be_activated = 1;
					}
					// Forecast: could be used only during your upkeeps, only on your turn and only once per turn
					if( cards_data[id].cc[2] == 16 && current_turn == player && current_phase == PHASE_UPKEEP && count_upkeeps(player) &&
						hand_instance->info_slot == 0 )
					{
						can_be_activated = 1;
					}
					// Homing Sliver gives "Slivercycling" to all Slivers in your hand
					if( homing_sliver_flag && has_subtype(player, i, SUBTYPE_SLIVER)  ){
						can_be_activated = 1;
					}
					if( can_be_activated ){
						CardFunction ptFunction = cards_data[id].code_pointer;
						if( ptFunction(player, i, EVENT_CAN_ACTIVATE_FROM_HAND ) ){
							card_ptr_t* c = cards_ptr[ get_id(player, i) ];
							pos += scnprintf(buffer + pos, cb-pos, " Activate %s\n", c->name );
							opts[opts_count][0] = i;
							opts[opts_count][1] = GA_EXTRA_ACTIVATE_FROM_HAND;
							opts_count++;
						}
					}
				}
			}
		}

		// also check for cards in the graveyard
		const int *graveyard = get_grave(player);
		int cage_flag = check_battlefield_for_id(2, CARD_ID_GRAFDIGGERS_CAGE);
		int graveyard_count = count_graveyard(player);
		for (i = 0; i < graveyard_count; ++i){
				card_data_t* card_d = &cards_data[ graveyard[i] ];
				if( card_d->cc[2] == 9 || card_d->id == CARD_ID_VISCERA_DRAGGER || card_d->cc[2] == 10 || card_d->cc[2] == 17 ||
					card_d->id == CARD_ID_HAAKON_STROMGALD_SCOURGE || card_d->id == CARD_ID_SKARRGAN_FIREBIRD ||
					card_d->id == CARD_ID_MARSHALING_CRY || card_d->id == CARD_ID_UNDEAD_GLADIATOR || card_d->id == CARD_ID_VINEWEFT ||
					card_d->id == CARD_ID_GRYFFS_BOON || card_d->cc[2] == 21 || card_d->cc[2] == 23
				  ){
					int fake = add_card_to_hand(player, graveyard[i]);
					add_state(player, fake, STATE_INVISIBLE);
					--hand_count[player];

					// CardFunction ptFunction = card_d->code_pointer;
					// int result = ptFunction(player, fake, EVENT_GRAVEYARD_ABILITY );
					int result = call_card_function(player, fake, EVENT_GRAVEYARD_ABILITY);
					if( result ){
						card_ptr_t* c = cards_ptr[ card_d->id ];
						if( (card_d->type & (TYPE_CREATURE | TYPE_LAND)) ){//Drownyard Temple case.
							if( ! cage_flag ){
								if( result & GA_PLAYABLE_FROM_GRAVE ){
									pos += scnprintf(buffer + pos, cb-pos, " Play %s\n", c->name );
									opts[opts_count][0] = i;
									opts[opts_count][1] = result;
									opts_count++;
								}
								if( result & GA_UNEARTH ){
									pos += scnprintf(buffer + pos, cb-pos, " Unearth %s\n", c->name );
									opts[opts_count][0] = i;
									opts[opts_count][1] = result;
									opts_count++;
								}
								if( result & GA_RETURN_TO_PLAY ){
									pos += scnprintf(buffer + pos, cb-pos, " Return %s to play\n", c->name );
									opts[opts_count][0] = i;
									opts[opts_count][1] = result;
									opts_count++;
								}
							}
							if( result & GA_BASIC ){
								if( result & GA_EMBALM ){
									pos += scnprintf(buffer + pos, cb-pos, " Embalm %s\n", c->name );
									opts[opts_count][0] = i;
									opts[opts_count][1] = result;
									opts_count++;
								}
								// GA_SCAVENGE is two bits
								else if( (result & GA_SCAVENGE) == GA_SCAVENGE ){
									pos += scnprintf(buffer + pos, cb-pos, " Scavenge %s\n", c->name );
									opts[opts_count][0] = i;
									opts[opts_count][1] = result;
									opts_count++;
								}
								else{
									pos += scnprintf(buffer + pos, cb-pos, " Activate %s\n", c->name );
									opts[opts_count][0] = i;
									opts[opts_count][1] = result;
									opts_count++;
								}
							}
							if( result & GA_RETURN_TO_HAND ){
								pos += scnprintf(buffer + pos, cb-pos, " Return %s to hand\n", c->name );
								opts[opts_count][0] = i;
								opts[opts_count][1] = result;
								opts_count++;
							}
							if( result & GA_PUT_ON_TOP_OF_DECK ){
								pos += scnprintf(buffer + pos, cb-pos, " Put %s on top of deck\n", c->name );
								opts[opts_count][0] = i;
								opts[opts_count][1] = result;
								opts_count++;
							}
						}
						else{
							if( result & GA_RETURN_TO_HAND ){
								pos += scnprintf(buffer + pos, cb-pos, " Return %s to hand\n", c->name );
								opts[opts_count][0] = i;
								opts[opts_count][1] = result;
								opts_count++;
							}
							else{
								if( ! cage_flag && can_legally_play_iid(player, graveyard[i]) ){
									if(card_d->cc[2] == 10 ){
										pos += scnprintf(buffer + pos, cb-pos, " Retrace %s\n", c->name );
										opts[opts_count][0] = i;
										opts[opts_count][1] = result;
										opts_count++;
									}
									else{
										pos += scnprintf(buffer + pos, cb-pos, " Flashback %s\n", c->name );
										opts[opts_count][0] = i;
										opts[opts_count][1] = result;
										opts_count++;
									}
								}
							}
						}
					}
					obliterate_card_and_recycle(player, fake);
				}
		}
		if( check_rfg(player, CARD_ID_TORRENT_ELEMENTAL) && can_sorcery_be_played(player, event) && has_mana_hybrid(player, 2, COLOR_BLACK, COLOR_GREEN, 3)){
			pos += scnprintf(buffer + pos, cb-pos, " Return Torrent Elemental to play\n");
			opts[opts_count][0] = CARD_ID_TORRENT_ELEMENTAL;
			opts[opts_count][1] = 0;	// in particular, don't set any bits in GA_GRAVE_ABILITY_MASK
			opts_count++;
		}
		if( check_rfg(player, CARD_ID_ETERNAL_SCOURGE) && can_sorcery_be_played(player, event) ){
			if( has_mana_to_cast_iid(player, event, CARD_ID_ETERNAL_SCOURGE) ){
				pos += scnprintf(buffer + pos, cb-pos, " Cast Eternal Scourge\n");
				opts[opts_count][0] = CARD_ID_ETERNAL_SCOURGE;
				opts[opts_count][1] = 0;	// in particular, don't set any bits in GA_GRAVE_ABILITY_MASK
				opts_count++;
			}
		}
		for (i = 0; i < 5; i++){
			instance->targets[i].player = -1;
			instance->targets[i].card = -1;
		}

		int choice = do_dialog(player, player, card, -1, -1, buffer, 1);
		if( choice == 0 ){
			spell_fizzled = 1;
			hack_allow_sorcery_if_rules_engine_is_only_effect = 0;
			return 0;
		}

		// pay the costs for the selected card
		if( opts[choice][1] == 0 ){ //Actually, only Torrent Elemental
			if( charge_mana_hybrid(player, card, 2, COLOR_BLACK, COLOR_GREEN, 3) ){
				remove_card_from_rfg(player, CARD_ID_TORRENT_ELEMENTAL);
				instance->targets[2].player = 0;
				instance->targets[2].card = CARD_ID_TORRENT_ELEMENTAL;
			}
		}
		if( opts[choice][1] == GA_EXTRA_ACTIVATE_FROM_HAND ){
			card_instance_t *hand_instance = get_card_instance(player, opts[choice][0]);
			int id = hand_instance->internal_card_id;
			CardFunction ptFunction = cards_data[id].code_pointer;
			if( !ptFunction(player, opts[choice][0], EVENT_CAN_ACTIVATE_FROM_HAND) ){
				spell_fizzled = 1;
				hack_allow_sorcery_if_rules_engine_is_only_effect = 0;
				return 0;
			}

			int result = ptFunction(player, opts[choice][0], EVENT_ACTIVATE_FROM_HAND);
			if( ! result ){
				spell_fizzled = 1;
				hack_allow_sorcery_if_rules_engine_is_only_effect = 0;
				return 0;
			}

			// result = 2 when a card was cycled
			if( result == 2 ){
				activate_astral_slide();
			}

			// Forecast: could be used only during your upkeeps, only on your turn and only once per turn
			if( cards_data[id].cc[2] == 16 ){
				get_card_instance(player, opts[choice][0])->info_slot = 1;
			}

			hand_instance->number_of_targets = 0;

			instance->targets[0].card = hand_instance->targets[0].card;
			instance->targets[0].player = hand_instance->targets[0].player;
			instance->targets[2].player = GA_EXTRA_ACTIVATE_FROM_HAND;
			instance->targets[2].card = opts[choice][0];
		}

		if( opts[choice][1] & GA_GRAVE_ABILITY_MASK ){
			int fake = add_card_to_hand(player, graveyard[opts[choice][0]]);
			add_state(player, fake, STATE_INVISIBLE);
			--hand_count[player];

			int result = call_card_function(player, fake, EVENT_PAY_FLASHBACK_COSTS );

			if ( ! result || spell_fizzled == 1){
				obliterate_card(player, fake);
				spell_fizzled = 1;
				hack_allow_sorcery_if_rules_engine_is_only_effect = 0;
				return 0;
			}
			if( opts[choice][1] & GA_PLAYABLE_FROM_GRAVE ){
				if( result == GAPAID_REMOVE ){
					play_card_in_grave_for_free(player, player, opts[choice][0]);
				}
				if( result == GAPAID_EXILE ){
					flashback_spell(player, opts[choice][0]);
				}
				cant_be_responded_to = 1; //This fake activation can't be responded, but you could interact with the spell
			}
			else{
				instance->targets[2].player = opts[choice][1];
				instance->targets[2].card = graveyard[opts[choice][0]];
				if( result == GAPAID_REMOVE ){
					remove_card_from_grave(player, opts[choice][0]);
				}
				if( result == GAPAID_EXILE ){
					rfg_card_from_grave(player, opts[choice][0]);
				}
				// GAPAID_REMAIN_IN_GRAVE --> just leave the card in grave
			}
			obliterate_card(player, fake);
		}
		hack_allow_sorcery_if_rules_engine_is_only_effect = 0;
	}
	else if( event == EVENT_RESOLVE_ACTIVATION ){
			/*
			char buffer2[500];
			card_ptr_t* c1 = cards_ptr[ instance->targets[0].card ];
			scnprintf(buffer2, 500, " %d ", instance->targets[1].player);
			do_dialog(player, player, instance->parent_card, -1, -1, buffer2, 0);
			*/
			if( instance->targets[2].card > -1 ){
				if( instance->targets[2].card == CARD_ID_TORRENT_ELEMENTAL ){
					int card_added = add_card_to_hand(player, get_internal_card_id_from_csv_id(CARD_ID_TORRENT_ELEMENTAL));
					add_state(player, card_added, STATE_TAPPED);
					put_into_play(player, card_added);
					return 0;
				}

				if( instance->targets[2].card == CARD_ID_ETERNAL_SCOURGE ){
					play_card_in_exile_for_free(player, player, CARD_ID_ETERNAL_SCOURGE);
					return 0;
				}

				int mode = instance->targets[2].player;
				if( mode & GA_EXTRA_ACTIVATE_FROM_HAND ){
					card_instance_t* actv = get_card_instance(player, instance->targets[2].card);
					card_data_t* card_d = get_card_data(player, instance->targets[2].card);	// will be valid even if the card no longer exists
					call_card_fn(card_d->code_pointer, actv, player, instance->targets[2].card,
								 EVENT_RESOLVE_ACTIVATION_FROM_HAND);	// not just call_card_function(), since the card may have been discarded since activation
					if( card_d->cc[2] == 16 ){ // Forecast, it could be used only once per turn
						actv->info_slot = 1;
					}
					return 0;
				}
				/*
				if( mode & GA_PLAYABLE_FROM_GRAVE ){
					flashback = 1;
					int internal_id = instance->targets[2].card;
					int pos = raw_put_iid_on_top_of_graveyard(player, internal_id);
					increase_trap_condition(player, TRAP_CARDS_TO_GY_FROM_ANYWHERE, -1);    // Since it's not being newly put into the graveyard
					if (cards_data[internal_id].cc[2] == 10){	// retrace - goes back to graveyard
						play_card_in_grave_for_free(player, player, pos);
					}
					else {
						play_card_in_grave_for_free_and_exile_it(player, player, pos);
					}
					flashback = 0;
					return 0;
				}
				*/

				int card_added = add_card_to_hand(player, instance->targets[2].card);
				add_state(player, card_added, STATE_INVISIBLE);	--hand_count[player];

				if( mode & GA_BASIC ){
					call_card_function(player, card_added, EVENT_RESOLVE_ACTIVATED_GRAVEYARD_ABILITY);
					obliterate_card(player, card_added);
				}
				if( mode & GA_RETURN_TO_HAND ){
					remove_state(player, card_added, STATE_INVISIBLE);	++hand_count[player];
				}
				if( mode & GA_PUT_ON_TOP_OF_DECK ){
					remove_state(player, card_added, STATE_INVISIBLE);	++hand_count[player];
					put_on_top_of_deck(player, card_added);
				}

				// GA_RETURN_TO_PLAY_MODIFIED is two bits
				if( (mode & GA_RETURN_TO_PLAY_MODIFIED) == GA_RETURN_TO_PLAY_MODIFIED ){
					call_card_function(player, card_added, EVENT_RETURN_TO_PLAY_FROM_GRAVE_MODIFIED);
				}

				if( mode & GA_UNEARTH ){
					convert_to_token(player, card_added);
					set_special_flags(player, card_added, SF_UNEARTH);
				}

				if( mode & (GA_RETURN_TO_PLAY | GA_UNEARTH) ){
					remove_state(player, card_added, STATE_INVISIBLE);	++hand_count[player];
					put_into_play(player, card_added);
				}

				if( mode & GA_UNEARTH ){
					create_targetted_legacy_effect(player, card_added, &effect_unearth, player, card_added);
				}

				// GA_RETURN_TO_PLAY_WITH_EFFECT is two bits
				if( (mode & GA_RETURN_TO_PLAY_WITH_EFFECT) == GA_RETURN_TO_PLAY_WITH_EFFECT ){
					call_card_function(player, card_added, EVENT_RESOLVE_ACTIVATED_GRAVEYARD_ABILITY);
				}
			}
	}
	// reset card with Forecast
	if( event == EVENT_CLEANUP && (get_rules_engine_infos(player) & REF_FORECAST) ){
		int count;
		for (count = 0; count < active_cards_count[player]; ++count){
			if( in_hand(player, count) ){
				card_data_t* card_d = get_card_data(player, count);
				if( card_d->cc[2] == 16 ){
					card_instance_t *crd = get_card_instance(player, count);
					crd->info_slot = 0;
				}
			}
		}
	}
	return 0;
}

int get_flashback(int player, int card){
	if( check_special_flags(player, card, SF_FLASHBACK) ){
		return 1;
	}
	return 0;
}

#ifdef CARDS_IN_HAND_THIS_TURN	// unused
static int was_card_in_hand(int player, int card){
	return 1; // comment out this code since track_hand is so slow
	int i=0;
	while( cards_in_hand_this_turn[player][i] != -1){
		if( cards_in_hand_this_turn[player][i++] == card ){
			return 1;
		}
	}
	return 0;
}
#endif

#ifdef CARDS_IN_HAND_THIS_TURN	// unused
static void track_hand(int player, int card, event_t event ){
	int i;
	if( comes_into_play(player, card, event) == 1 && player == HUMAN){
		for(i=0;i<100;i++){
			cards_in_hand_this_turn[HUMAN][i] = -1;
			cards_in_hand_this_turn[AI][i] = -1;
		}
	}

	int p;
	for (p=0;p<2;p++){
		for(i=0;i<active_cards_count[p]; i++){
			card_instance_t *card_instance = get_card_instance(p, i);
			if( ! ( card_instance->state & STATE_INVISIBLE ) && ! ( card_instance->state & STATE_IN_PLAY )  ){
				int j=0;
				while( cards_in_hand_this_turn[p][j] != -1 && j < 100 ){
					if( cards_in_hand_this_turn[p][j] == i ){
						break;
					}
					j++;
				}
				cards_in_hand_this_turn[p][j] = i;
			}
		}
	}

	if( event == EVENT_CLEANUP ){
		for(i=0;i<100;i++){
			cards_in_hand_this_turn[HUMAN][i] = -1;
			cards_in_hand_this_turn[AI][i] = -1;
		}
	}
}
#endif

static int get_rules_engine_card(int player){
	return rules_engine_card[player];
}



static void reset_storm_count(int player, int card)
{
  target_definition_t td;

  if (player != HUMAN)
    return;

  base_target_definition(HUMAN, card, &td, TYPE_CREATURE);
  td.required_subtype = SUBTYPE_WEREWOLF;
  if (!can_target(&td))
  {
    for (int p = 0; p < 2; ++p)
    {
      int rec = rules_engine_card[p];
      if (rec >= 0)
      {
        increase_trap_condition_impl(p, 0x403, 0, 0);
        increase_trap_condition_impl(p, 0x404, 0, 0);
        increase_trap_condition_impl(p, 0x405, 0, 0);
        increase_trap_condition_impl(p, 0x406, 0, 0);
        increase_trap_condition_impl(p, 0x407, 0, 0);
        remove_all_counters(HUMAN, card, p, rec, COUNTER_VORTEX);
        remove_all_counters(HUMAN, card, p, rec, COUNTER_BLOOD);
        remove_all_counters(HUMAN, card, p, rec, COUNTER_CHARGE);
        remove_all_counters(HUMAN, card, p, rec, COUNTER_DEVOTION);
      }
    }
  }
  else
  {
    int human_count = rules_engine_card[HUMAN] == -1
                          ? 0
                          : count_counters(HUMAN, rules_engine_card[HUMAN], COUNTER_VORTEX);
    int ai_count =
        rules_engine_card[AI] == -1 ? 0 : count_counters(AI, rules_engine_card[AI], COUNTER_VORTEX);
    for (int p = 0; p < 2; ++p)
    {
      for (int c = 0; c < active_cards_count[p]; ++c)
      {
        if (in_play(p, c))
        {
          card_instance_t *instance = get_card_instance(p, c);
          if (has_subtype(p, c, SUBTYPE_WEREWOLF) && instance->targets[9].card != 0x42)
          {
            instance->targets[9].card = 0x42;
            instance->targets[8].player = human_count;
            instance->targets[8].card = ai_count;
          }
          if (get_id(p, c) == CARD_ID_RULES_ENGINE)
          {
            increase_trap_condition_impl(p, 0x403, 0, 0);
            increase_trap_condition_impl(p, 0x404, 0, 0);
            increase_trap_condition_impl(p, 0x405, 0, 0);
            increase_trap_condition_impl(p, 0x406, 0, 0);
            increase_trap_condition_impl(p, 0x407, 0, 0);
            remove_all_counters(HUMAN, card, p, c, COUNTER_VORTEX);
            remove_all_counters(HUMAN, card, p, c, COUNTER_BLOOD);
            remove_all_counters(HUMAN, card, p, c, COUNTER_CHARGE);
            remove_all_counters(HUMAN, card, p, c, COUNTER_DEVOTION);
          }
        }
      }
    }
  }
}

static void keep_storm_count(int player, int card, event_t event)
{
  enum
  {
    TRAP_NONCREATURES_PLAYED = 0x408
  };
  int not_cast;
  if (player != HUMAN)
    return;

  if (trigger_condition == TRIGGER_SPELL_CAST && card == affected_card &&
      affected_card_controller == HUMAN && reason_for_trigger_controller == HUMAN &&
      (not_cast = check_special_flags(trigger_cause_controller, trigger_cause, SF_NOT_CAST),
       event == EVENT_END_TRIGGER && !not_cast))
  {
    card_instance_t *played = get_card_instance(trigger_cause_controller, trigger_cause);
    uint8_t types = cards_data[played->internal_card_id].type;
    if (!(types & TYPE_LAND))
    {
      int mode;
      if (types & TYPE_CREATURE)
        mode = STORM_CREATURE;
      else
      {
        increase_trap_condition_impl(trigger_cause_controller, TRAP_NONCREATURES_PLAYED, 1, 1);
        mode = 0;
      }
      if (types & TYPE_ENCHANTMENT)
        mode |= STORM_ENCHANTMENT;
      if ((types & TYPE_SPELL) && !(types & TYPE_CREATURE))
        mode |= STORM_INSTANT_SORCERY;
      if (get_id(trigger_cause_controller, trigger_cause) == CARD_ID_PEER_THROUGH_DEPTHS)
        increase_trap_condition_impl(trigger_cause_controller, TRAP_PEER_THROUGH_DEPTHS, 1, 1);
      if (get_id(trigger_cause_controller, trigger_cause) == CARD_ID_REACH_THROUGH_MISTS)
        increase_trap_condition_impl(trigger_cause_controller, TRAP_REACH_THROUGH_MISTS, 1, 1);
      unsigned int colors = get_color(trigger_cause_controller, trigger_cause);
      increment_storm_count(trigger_cause_controller, mode | (colors & COLOR_TEST_ANY_COLORED));
    }
  }
  else if (event == EVENT_CAN_SKIP_TURN)
    reset_storm_count(player, card);
}









#if 0	// unused
int get_turn_count(void)
{
  return get_trap_condition(HUMAN, TRAP_TURN_COUNT) + get_trap_condition(AI, TRAP_TURN_COUNT);
}
#endif

int get_specific_turn_count(int player){
  return get_trap_condition(player, TRAP_TURN_COUNT);
}

event_t get_actual_event(void){
	return actual_event;
}

static void parse_grave_for_centaur_vinecrasher(int player, int card){
	const int *grave = get_grave(player);
	int count = count_graveyard(player)-1;
	while( count > -1 ){
			if( has_mana(player, COLOR_GREEN, 2) && cards_data[grave[count]].id == CARD_ID_CENTAUR_VINECRASHER  ){
				int choice = do_dialog(player, player, card, -1, -1, " Return Centaur Vinecrasher to your hand\n Do not return", 0);
				if( choice == 0 ){
					charge_mana(player, COLOR_GREEN, 2);
					if( spell_fizzled != 1 ){
						add_card_to_hand(player, grave[count]);
						remove_card_from_grave(player, count);
					}
				}
			}
			count--;
	}
}

static int prized_amalgam_legacy(int player, int card, event_t event){

	if( eot_trigger(player, card, event) ){
		seek_grave_for_id_to_reanimate(player, card, player, CARD_ID_PRIZED_AMALGAM, REANIMATE_TAP);
		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

	enum{
		GC_BLOODGHAST = 		1<<0,
		GC_VENGEVINE =	 		1<<1,
		GC_WONDER =		 		1<<2,
		GC_SWORD_OF_THE_MEEK =	1<<3,
		GC_DEARLY_DEPARTED =	1<<4,
		GC_CHANDRAS_PHOENIX =	1<<5,
		GC_BLOOD_SPEAKER =	 	1<<6,
		GC_SOSUKES_SUMMONS =	1<<7,
		GC_VEILBORN_GHOUL =	 	1<<8,
		GC_NETHER_TRAITOR =	 	1<<9,
		GC_MASKED_ADMIRERS =	1<<10,
		GC_AUNTIES_SNITCH =		1<<11,
		GC_REACH_OF_BRANCHES =	1<<12,
		GC_BRAWN =				1<<13,
		GC_FILTH =				1<<14,
		GC_VALOR =				1<<15,
		GC_BLADEWINGS_THRALL =	1<<16,
		GC_DRAGON_ENCHANTMENTS =1<<17,
		GC_RECOVER =			1<<18,
		GC_PYREWILD_SHAMAN =	1<<19,
		GC_ANGER =				1<<20,
		GC_AKOUM_FIREBIRD =		1<<21,
		GC_CENTAUR_VINECRASHER =1<<22,
		GC_KOZILEKS_RETURN =	1<<23,
		GC_PRIZED_AMALGAM =		1<<24,
		GC_OVALCHASE_DAREDEVIL= 1<<25,
	};

static int get_graveyard_counts(int player){

	int ids[] = {   CARD_ID_BLOODGHAST, //0
					CARD_ID_VENGEVINE, //1
					CARD_ID_WONDER, //2
					CARD_ID_SWORD_OF_THE_MEEK, //3
					CARD_ID_DEARLY_DEPARTED, //4
					CARD_ID_CHANDRAS_PHOENIX, //5
					CARD_ID_BLOOD_SPEAKER, //6
					CARD_ID_SOSUKES_SUMMONS, //7
					CARD_ID_VEILBORN_GHOUL, //8
					CARD_ID_NETHER_TRAITOR,//9
					CARD_ID_MASKED_ADMIRERS,//10
					CARD_ID_AUNTIES_SNITCH,//11
					CARD_ID_REACH_OF_BRANCHES,//12
					CARD_ID_BRAWN,//13
					CARD_ID_FILTH, //14
					CARD_ID_VALOR, //15
					CARD_ID_BLADEWINGS_THRALL, //16
					-1, //17
					-1, //18
					CARD_ID_PYREWILD_SHAMAN, // 19
					CARD_ID_ANGER,	// 20
					CARD_ID_AKOUM_FIREBIRD,	// 21
					CARD_ID_CENTAUR_VINECRASHER, //22
					CARD_ID_KOZILEKS_RETURN, //23
					CARD_ID_PRIZED_AMALGAM, //24
					CARD_ID_OVALCHASE_DAREDEVIL //25
	};

	int result = 0;
	const int *grave = get_grave(player);
	int count = 0;
	while( grave[count] != -1 ){
			card_data_t* card_d = &cards_data[ grave[count] ];
			if( card_d->cc[2] == 18 ){
				result |= GC_DRAGON_ENCHANTMENTS;
			}
			if( card_d->cc[2] == 20 ){
				result |= GC_RECOVER;
			}
			unsigned int k;
			for(k=0; k<(sizeof(ids)/sizeof(ids[0])); k++){
				if( card_d->id == ids[k] ){
					result |= 1<<k;
					break;
				}
			}
			count++;
	}
	return result;
}

static void static_graveyard_abilities(int player, int card, event_t event)
{
  uint16_t uVar1;
  uint16_t uVar2;
  uint16_t uVar3;
  uint16_t uVar4;
  uint16_t uVar5;
  int *piVar6;
  xtrigger_t xVar7;
  bool bVar8;
  bool bVar9;
  card_instance_t *instance;
  int iVar11;
  unsigned int uVar12;
  int iVar13;
  int iVar14;
  unsigned int uVar15;
  int iVar16;
  enable_xtrigger_flags_t eVar17;
  xtrigger_t xVar18;
  const char *pcVar19;
  int iVar20;
  unsigned int uStack_f0;
  char buffer[100];
  if ((((unsigned int)player_bits[player] >> 24 & 0xff) & 2) != 0)
  {
    return;
  }
  if (trigger_condition == 0xdb)
  {
    if (card == affected_card)
    {
      if (player != affected_card_controller)
      {
      LAB_026c3940:
        if (event != EVENT_ABILITIES)
          goto LAB_026c394d;
        goto LAB_026c3c99;
      }
      if ((reason_for_trigger_controller == player) && (trigger_cause_controller == player))
      {
        uVar12 = get_graveyard_counts(player);
        uStack_f0 = uVar12 & 1;
        if (uStack_f0 != 0)
        {
          iVar11 = is_what(player, trigger_cause, 1);
          uStack_f0 = (unsigned int)(iVar11 != 0);
        }
        if ((((uVar12 & 8) != 0) &&
             (iVar11 = get_power(trigger_cause_controller, trigger_cause), iVar11 == 1)) &&
            (iVar11 = get_toughness(trigger_cause_controller, trigger_cause), iVar11 == 1))
        {
          uStack_f0 = uStack_f0 | 2;
        }
        if ((((uVar12 & 0x10) != 0) &&
             (iVar11 = is_what(trigger_cause_controller, trigger_cause, 2), iVar11 != 0)) &&
            (iVar11 = has_subtype(trigger_cause_controller, trigger_cause, SUBTYPE_HUMAN),
             iVar11 != 0))
        {
          uStack_f0 = uStack_f0 | 4;
        }
        if ((((uVar12 & 0x40) != 0) &&
             (iVar11 = is_what(trigger_cause_controller, trigger_cause, 0x47), iVar11 != 0)) &&
            (iVar11 = has_subtype(trigger_cause_controller, trigger_cause, SUBTYPE_DEMON),
             iVar11 != 0))
        {
          uStack_f0 = uStack_f0 | 8;
        }
        if (((((uVar12 & 0x80) != 0) &&
              (iVar11 = is_what(trigger_cause_controller, trigger_cause, 2), iVar11 != 0)) &&
             (iVar11 = has_subtype(trigger_cause_controller, trigger_cause, SUBTYPE_SNAKE),
              iVar11 != 0)) &&
            (iVar11 = is_token(trigger_cause_controller, trigger_cause), iVar11 == 0))
        {
          uStack_f0 = uStack_f0 | 0x10;
        }
        if ((((uVar12 & 0x100) != 0) &&
             (iVar11 = is_what(trigger_cause_controller, trigger_cause, 1), iVar11 != 0)) &&
            (iVar11 = has_subtype(trigger_cause_controller, trigger_cause, SUBTYPE_SWAMP),
             iVar11 != 0))
        {
          uStack_f0 = uStack_f0 | 0x20;
        }
        if ((((uVar12 & 0x1000) != 0) &&
             (iVar11 = is_what(trigger_cause_controller, trigger_cause, 1), iVar11 != 0)) &&
            (iVar11 = has_subtype(trigger_cause_controller, trigger_cause, SUBTYPE_FOREST),
             iVar11 != 0))
        {
          uStack_f0 = uStack_f0 | 0x40;
        }
        if ((((uVar12 & 0x10000) != 0) &&
             (iVar11 = is_what(trigger_cause_controller, trigger_cause, 0x47), iVar11 != 0)) &&
            (iVar11 = has_subtype(trigger_cause_controller, trigger_cause, SUBTYPE_DRAGON),
             iVar11 != 0))
        {
          uStack_f0 = uStack_f0 | 0x80;
        }
        if ((((uVar12 & 0x20000) != 0) &&
             (iVar11 = is_what(trigger_cause_controller, trigger_cause, 2), iVar11 != 0)) &&
            (iVar11 = get_cmc(trigger_cause_controller, trigger_cause), 5 < iVar11))
        {
          uStack_f0 = uStack_f0 | 0x100;
        }
        if ((((uVar12 & 0x200000) != 0) &&
             (iVar11 = is_what(trigger_cause_controller, trigger_cause, 1), iVar11 != 0)) &&
            (iVar11 = has_mana_multi(player, 4, 0, 0, 0, 2, 0), iVar11 != 0))
        {
          uStack_f0 = uStack_f0 | 0x200;
        }
        if (((((uVar12 & 0x1000000) != 0) &&
              (iVar11 = is_what(trigger_cause_controller, trigger_cause, 2), iVar11 != 0)) &&
             (iVar11 = check_special_flags3(trigger_cause_controller, trigger_cause, 1),
              iVar11 != 0)) &&
            (iVar11 = get_owner(trigger_cause_controller, trigger_cause), iVar11 == player))
        {
          uStack_f0 = uStack_f0 | 0x400;
        }
        if (((uVar12 & 0x2000000) == 0) ||
            (iVar11 = is_what(trigger_cause_controller, trigger_cause, 0x40), iVar11 == 0))
        {
          if (((uVar12 & 0x10000000) != 0) &&
              (iVar11 = has_subtype(trigger_cause_controller, trigger_cause, SUBTYPE_GATE),
               iVar11 != 0))
            goto LAB_026c47a3;
          if (uStack_f0 == 0)
            goto LAB_026c3824;
        }
        else
        {
          uStack_f0 = uStack_f0 | 0x800;
          if (((uVar12 & 0x10000000) != 0) &&
              (iVar11 = has_subtype(trigger_cause_controller, trigger_cause, SUBTYPE_GATE),
               iVar11 != 0))
          {
          LAB_026c47a3:
            uStack_f0 = uStack_f0 | 0x1000;
          }
        }
        iVar13 = trigger_cause;
        iVar11 = trigger_cause_controller;
        if (event == EVENT_TRIGGER)
        {
          event_result = event_result | 2;
        }
        else
        {
          if (event != EVENT_RESOLVE_TRIGGER)
            goto LAB_026c3824;
          piVar6 = graveyard_ptr_mutable[player];
          iVar14 = count_graveyard(player);
          iVar14 = iVar14 + -1;
          if (-1 < iVar14)
          {
            do
            {
              iVar20 = piVar6[iVar14];
              uVar4 = cards_data[iVar20].id;
              if (uVar4 == 0x2e94)
              {
                if ((uStack_f0 & 4) != 0)
                {
                  add_counters(iVar11, iVar13, iVar11, iVar13, COUNTER_P1_P1, 1);
                }
              }
              else if ((unsigned short)uVar4 < 0x2e95)
              {
                if (uVar4 == 0x234c)
                {
                  if (((uStack_f0 & 2) != 0) &&
                      ((iVar20 = duh_mode(player),
                        iVar20 != 0 ||
                            (iVar20 = do_dialog(player, player, card, -1, -1,
                                                " Return Sword of the Meek\tto play\n Pass", 0),
                             iVar20 == 0))))
                  {
                    iVar20 = reanimate_permanent(player, card, player, iVar14, REANIMATE_DEFAULT);
                    equip_target_creature(player, iVar20, iVar11, iVar13);
                  }
                }
                else if ((unsigned short)uVar4 < 0x234d)
                {
                  if (uVar4 == 0x1bff)
                  {
                    if ((uStack_f0 & 8) != 0)
                      goto LAB_026c5cd4;
                  }
                  else if (uVar4 == 0x1d8d)
                  {
                    if ((uStack_f0 & 0x10) != 0)
                    {
                      iVar20 = duh_mode(player);
                      if (iVar20 == 0)
                      {
                        pcVar19 = " Return Sosuke\'s Summons to hand\n Do not return";
                        goto LAB_026c5c9f;
                      }
                      goto LAB_026c5cc7;
                    }
                  }
                  else if ((uVar4 == 0x1952) && ((uStack_f0 & 0x80) != 0))
                  {
                    iVar20 = duh_mode(player);
                    if (iVar20 == 0)
                    {
                      pcVar19 = " Return Bladewing\'s Thrall to play\n Do not return";
                      goto LAB_026c4a9b;
                    }
                    goto LAB_026c4ac3;
                  }
                }
                else if (uVar4 == 0x24c4)
                {
                  if ((uStack_f0 & 0x40) != 0)
                  {
                    iVar20 = duh_mode(player);
                    if (iVar20 == 0)
                    {
                      pcVar19 = " Return Reach of Branches to hand\n Do not return";
                    LAB_026c5c9f:
                      iVar20 = do_dialog(player, player, card, -1, -1, pcVar19, 0);
                      if (iVar20 != 0)
                        goto LAB_026c483a;
                    }
                  LAB_026c5cc7:
                    iVar20 = piVar6[iVar14];
                  LAB_026c5cd4:
                    add_card_to_hand(player, iVar20);
                    remove_card_from_grave(player, iVar14);
                  }
                }
                else
                {
                  if (((uVar4 == 0x290f) && ((uStack_f0 & 1) != 0)) &&
                      ((iVar20 = duh_mode(player),
                        iVar20 != 0 ||
                            (iVar20 = do_dialog(player, player, card, -1, -1,
                                                " Return Bloodghast to play\n Do not return", 0),
                             iVar20 == 0))))
                  {
                  LAB_026c48f1:
                    reanimate_permanent(player, card, player, iVar14, REANIMATE_DEFAULT);
                  }
                }
              }
              else if (uVar4 == 0x3e33)
              {
                if ((uStack_f0 & 0x400) != 0)
                {
                  create_legacy_effect(player, card, prized_amalgam_legacy);
                }
              }
              else if ((unsigned short)uVar4 < 0x3e34)
              {
                if (uVar4 == 0x315a)
                {
                  if ((uStack_f0 & 0x20) != 0)
                  {
                    iVar20 = duh_mode(player);
                    if (iVar20 == 0)
                    {
                      pcVar19 = " Return Veilborn Ghoul to hand\n Do not return";
                      goto LAB_026c5c9f;
                    }
                    goto LAB_026c5cc7;
                  }
                }
                else if ((((uVar4 == 0x3b9b) && ((uStack_f0 & 0x200) != 0)) &&
                          (iVar20 = do_dialog(player, player, card, -1, -1,
                                              " Return Akoum Firebird to bf\n Decline", 0),
                           iVar20 == 0)) &&
                         (iVar20 = charge_mana_multi_while_resolving_csvid(
                              0x3b9b, EVENT_RESOLVE_TRIGGER, player, 4, 0, 0, 0, 2, 0),
                          iVar20 != 0))
                  goto LAB_026c48f1;
              }
              else if (uVar4 == 0x4054)
              {
                if ((uStack_f0 & 0x800) != 0)
                {
                  iVar20 = duh_mode(player);
                  if (iVar20 == 0)
                  {
                    pcVar19 = " Return Ovalchase Daredevil to play\n Do not return";
                  LAB_026c4a9b:
                    iVar20 = do_dialog(player, player, card, -1, -1, pcVar19, 0);
                    if (iVar20 != 0)
                      goto LAB_026c483a;
                  }
                LAB_026c4ac3:
                  iVar20 = add_card_to_hand(player, piVar6[iVar14]);
                  remove_card_from_grave(player, iVar14);
                  put_into_play(player, iVar20);
                }
              }
              else
              {
                if (((uVar4 == 0x4caf) && ((uStack_f0 & 0x1000) != 0)) &&
                    ((iVar20 = duh_mode(player),
                      iVar20 != 0 ||
                          (iVar20 = do_dialog(
                               player, player, card, -1, -1,
                               " Put Gate Colossus on top of your library\n Leave in grave", 0),
                           iVar20 == 0))))
                {
                  from_graveyard_to_deck(player, iVar14, 1);
                }
              }
            LAB_026c483a:
              if ((cards_data[piVar6[iVar14]].cc[2] == '\x12') && ((uStack_f0 & 0x100) != 0))
              {
                scnprintf(buffer, 100, " Return %s to bf\n Decline",
                          cards_ptr[cards_data[piVar6[iVar14]].id]->name);
                iVar20 = duh_mode(player);
                if ((iVar20 != 0) ||
                    (iVar20 = do_dialog(player, player, card, -1, -1, buffer, 0), iVar20 == 0))
                {
                  iVar20 = add_card_to_hand(player, piVar6[iVar14]);
                  remove_card_from_grave(player, iVar14);
                  put_into_play_aura_attached_to_target(player, iVar20, trigger_cause_controller,
                                                        trigger_cause);
                }
              }
              iVar14 = iVar14 + -1;
            } while (iVar14 != -1);
          }
        }
        if (trigger_condition == 0xd3)
          goto LAB_026c382f;
      }
      else if (event == EVENT_ABILITIES)
        goto LAB_026c41bf;
    LAB_026c394d:
      if (trigger_condition == 0xd7)
      {
      LAB_026c395d:
        if (card == affected_card)
          goto LAB_026c3969;
      }
    }
    else
    {
    LAB_026c383d:
      if (event == EVENT_ABILITIES)
      {
      LAB_026c3c8d:
        if (affected_card_controller == player)
        {
        LAB_026c41bf:
          instance = in_play(player, affected_card);
          if ((instance != NULL) &&
              (iVar11 = is_what(affected_card_controller, affected_card, 2), iVar11 != 0))
          {
            uVar12 = get_graveyard_counts(player);
            if (((uVar12 & 4) != 0) &&
                (iVar11 = check_battlefield_for_subtype(player, 1, 0x5002), iVar11 != 0))
            {
              event_result = event_result | 0x20;
            }
            if (((uVar12 & 0x2000) != 0) &&
                (iVar11 = check_battlefield_for_subtype(player, 1, 0x5001), iVar11 != 0))
            {
              event_result = event_result | 0x80;
            }
            if (((uVar12 & 0x4000) != 0) &&
                (iVar11 = check_battlefield_for_subtype(player, 1, 0x5009), iVar11 != 0))
            {
              event_result = event_result | 1;
            }
            if (((uVar12 & 0x8000) != 0) &&
                (iVar11 = check_battlefield_for_subtype(player, 1, 0x5007), iVar11 != 0))
            {
              event_result = event_result | 0x100;
            }
            if (((uVar12 & 0x100000) != 0) &&
                (iVar11 = check_battlefield_for_subtype(player, 1, 0x5006), iVar11 != 0))
            {
              haste(affected_card_controller, affected_card, EVENT_ABILITIES);
            }
          }
        }
      LAB_026c3c99:
        if (trigger_condition == 0xd7)
          goto LAB_026c395d;
        goto LAB_026c3857;
      }
    }
  LAB_026c384a:
    if ((((event == EVENT_GRAVEYARD_FROM_PLAY) &&
          (iVar11 = check_for_death_trigger_removal(affected_card_controller, affected_card),
           iVar11 == 0)) &&
         (instance = get_card_instance(affected_card_controller, affected_card),
          (unsigned char)(instance->kill_code - 1) < 3)) &&
        (iVar11 = check_special_flags2(affected_card_controller, affected_card, 0x10000000),
         iVar11 == 0))
    {
      uVar12 = get_graveyard_counts(player);
      if ((uVar12 & 0x40200) != 0)
      {
        if (affected_card_controller != player)
          goto LAB_026c3857;
        instance = in_play(player, affected_card);
        if ((instance != NULL) &&
            (iVar11 = is_what(affected_card_controller, affected_card, 2), iVar11 != 0))
        {
          instance = get_card_instance(player, card);
          if (instance->targets[1].player < 0)
          {
            instance = get_card_instance(player, card);
            instance->targets[1].player = 0;
          }
          instance = get_card_instance(player, card);
          instance->targets[1].player = instance->targets[1].player | 1;
        }
      }
      if ((((uVar12 & 0x8000000) != 0) && (affected_card_controller == player)) &&
          ((instance = in_play(player, affected_card),
            instance != NULL &&
                ((iVar11 = is_what(affected_card_controller, affected_card, 2),
                  iVar11 != 0 && (iVar11 = has_subtype(affected_card_controller, affected_card,
                                                       SUBTYPE_DRAGON),
                                  iVar11 != 0))))))
      {
        instance = get_card_instance(player, card);
        if (instance->targets[1].player < 0)
        {
          instance = get_card_instance(player, card);
          instance->targets[1].player = 0;
        }
        instance = get_card_instance(player, card);
        instance->targets[1].player = instance->targets[1].player | 1;
      }
    }
  }
  else
  {
  LAB_026c3824:
    if (trigger_condition != 0xd3)
      goto LAB_026c3c80;
  LAB_026c382f:
    iVar11 = affected_card;
    if (card != affected_card)
      goto LAB_026c383d;
    if (player != affected_card_controller)
      goto LAB_026c3940;
    if (reason_for_trigger_controller == player)
    {
      uVar12 = get_graveyard_counts(player);
      iVar13 = is_what(trigger_cause_controller, trigger_cause, 1);
      if (iVar13 != 0)
      {
      LAB_026c3c80:
        if (event != EVENT_ABILITIES)
          goto LAB_026c394d;
        goto LAB_026c3c8d;
      }
      uVar15 = uVar12 & 0x4000000;
      if ((trigger_cause_controller != player) ||
          (iVar13 = is_what(player, trigger_cause, 2), iVar13 == 0))
      {
        if ((uVar15 != 0) &&
            (iVar13 = has_subtype(trigger_cause_controller, trigger_cause, SUBTYPE_BOLAS),
             iVar13 != 0))
        {
          iVar13 = is_what(trigger_cause_controller, trigger_cause, 0x100);
          uStack_f0 = 0;
          if (iVar13 != 0)
          {
          LAB_026c446b:
            uStack_f0 = uStack_f0 | 0x10;
            goto LAB_026c4472;
          }
        }
        goto LAB_026c3c80;
      }
      uStack_f0 = uVar12 & 2;
      if (uStack_f0 != 0)
      {
        if (rules_engine_card[player] == -1)
        {
          uStack_f0 = 0;
        }
        else
        {
          iVar13 = count_counters(player, rules_engine_card[player], COUNTER_BLOOD);
          uStack_f0 = (unsigned int)(iVar13 == 1);
        }
      }
      if (((uVar12 & 0x400) != 0) && (iVar13 = has_mana(player, 3, 2), iVar13 != 0))
      {
        uStack_f0 = uStack_f0 | 2;
      }
      if (((uVar12 & 0x1000000) != 0) &&
          (iVar13 = check_special_flags(trigger_cause_controller, trigger_cause, 0x10000000),
           iVar13 != 0))
      {
        uStack_f0 = uStack_f0 | 8;
      }
      if ((((uVar12 & 0x800000) == 0) ||
           (iVar13 = has_subtype(trigger_cause_controller, trigger_cause, SUBTYPE_ELDRAZI),
            iVar13 == 0)) ||
          (iVar13 = get_cmc(trigger_cause_controller, trigger_cause), iVar13 < 7))
      {
        if ((uVar15 != 0) &&
            (iVar13 = has_subtype(trigger_cause_controller, trigger_cause, SUBTYPE_BOLAS),
             iVar13 != 0))
          goto LAB_026c5e22;
      LAB_026c5884:
        if (uStack_f0 == 0)
          goto LAB_026c3c80;
      }
      else
      {
        uStack_f0 = uStack_f0 | 4;
        if ((uVar15 != 0) &&
            (iVar13 = has_subtype(trigger_cause_controller, trigger_cause, SUBTYPE_BOLAS),
             iVar13 != 0))
        {
        LAB_026c5e22:
          iVar13 = is_what(trigger_cause_controller, trigger_cause, 0x100);
          if (iVar13 != 0)
            goto LAB_026c446b;
          goto LAB_026c5884;
        }
      }
    LAB_026c4472:
      if (event == EVENT_TRIGGER)
      {
        event_result = event_result | 2;
      }
      else
      {
        if (event != EVENT_RESOLVE_TRIGGER)
          goto LAB_026c3c80;
        piVar6 = graveyard_ptr_mutable[player];
        iVar13 = count_graveyard(player);
        iVar13 = iVar13 + -1;
        if (-1 < iVar13)
        {
          do
          {
            iVar14 = piVar6[iVar13];
            uVar3 = cards_data[iVar14].id;
            if (uVar3 == 0x3d0c)
            {
              if ((uStack_f0 & 4) != 0)
              {
                iVar14 = add_card_to_hand(player, iVar14);
                add_state(player, iVar14, 0x20);
                default_test_definition((test_definition_t *)buffer, 2);
                iVar20 = check_battlefield_for_special_card(player, iVar11, player, 4,
                                                            (test_definition_t *)buffer);
                iVar16 = check_battlefield_for_special_card(player, iVar11, 1 - player, 4,
                                                            (test_definition_t *)buffer);
                iVar20 =
                    do_dialog(player, player, iVar11, -1, -1, " Exile Kozilek\'s Return\n Decline",
                              (unsigned int)(iVar16 < iVar20));
                if (iVar20 == 0)
                {
                  remove_card_from_grave(player, iVar13);
                  new_damage_all(player, iVar14, 2, 5, 1, NULL);
                  rfg_card_in_hand(player, iVar14);
                }
                else
                {
                  obliterate_card(player, iVar14);
                  hand_count[player] = hand_count[player] + -1;
                }
              }
            }
            else if ((unsigned short)uVar3 < 0x3d0d)
            {
              if (uVar3 == 0x23f8)
              {
                if (((((uStack_f0 & 2) != 0) && (iVar14 = has_mana(player, 3, 2), iVar14 != 0)) &&
                     ((iVar14 = duh_mode(player),
                       iVar14 != 0 || (iVar14 = do_dialog(
                                           player, player, iVar11, -1, -1,
                                           " Return Masked Admirers to hand\n Do not return", 0),
                                       iVar14 == 0)))) &&
                    (charge_mana(player, COLOR_GREEN, 2), spell_fizzled != 1))
                {
                  add_card_to_hand(player, piVar6[iVar13]);
                  remove_card_from_grave(player, iVar13);
                }
              }
              else
              {
                if (((uVar3 == 0x2b43) && ((uStack_f0 & 1) != 0)) &&
                    ((iVar14 = duh_mode(player),
                      iVar14 != 0 ||
                          (iVar14 = do_dialog(player, player, iVar11, -1, -1,
                                              " Return Vengevine to play\n Do not return", 0),
                           iVar14 == 0))))
                {
                  reanimate_permanent(player, iVar11, player, iVar13, REANIMATE_DEFAULT);
                }
              }
            }
            else if (uVar3 == 0x3e33)
            {
              if ((uStack_f0 & 8) != 0)
              {
                create_legacy_effect(player, iVar11, prized_amalgam_legacy);
              }
            }
            else if ((uVar3 == 0x412d) && ((uStack_f0 & 0x10) != 0))
            {
              add_counters(player, iVar11, trigger_cause_controller, trigger_cause,
                                       COUNTER_LOYALTY, 1);
              rfg_card_from_grave(player, iVar13);
            }
            iVar13 = iVar13 + -1;
          } while (iVar13 != -1);
        }
      }
      goto LAB_026c3c99;
    }
    if (event == EVENT_ABILITIES)
      goto LAB_026c41bf;
    if (trigger_condition != 0xd7)
      goto LAB_026c384a;
  LAB_026c3969:
    if ((player != affected_card_controller) || (reason_for_trigger_controller != player))
      goto LAB_026c384a;
    uVar12 = get_graveyard_counts(player);
    if (rules_engine_card[player] < 0)
    {
      uStack_f0 = 0;
    LAB_026c46cf:
      iVar11 = 1 - player;
      iVar13 = rules_engine_card[iVar11];
    LAB_026c3a2b:
      if ((-1 < iVar13) && (instance = get_card_instance(iVar11, iVar13),
                            (instance->targets[0x10].card & 0x2000) != 0))
      {
      LAB_026c45c0:
        iVar11 = has_mana(player, 0, 3);
        if ((iVar11 != 0) && ((uVar12 & 0x80000) != 0))
        {
          uStack_f0 = uStack_f0 | 4;
          goto LAB_026c3a56;
        }
      }
      if (uStack_f0 == 0)
        goto LAB_026c384a;
    }
    else
    {
      instance = get_card_instance(player, rules_engine_card[player]);
      uStack_f0 =
          (int)(unsigned int)*(unsigned char *)((int)&instance->targets[0x10].card + 1) >> 3 & 1;
      if (uStack_f0 != 0)
      {
        uStack_f0 = uVar12 >> 5 & 1;
      }
      iVar11 = 1 - player;
      if (rules_engine_card[player] < 0)
        goto LAB_026c46cf;
      instance = get_card_instance(player, rules_engine_card[player]);
      if (((instance->targets[0x10].card & 0x200) == 0) &&
          ((rules_engine_card[player] < 0 ||
            (instance = get_card_instance(player, rules_engine_card[player]),
             (instance->targets[0x10].card & 0x400) == 0))))
      {
        iVar13 = rules_engine_card[iVar11];
        goto LAB_026c3a2b;
      }
      iVar13 = rules_engine_card[iVar11];
      if ((uVar12 & 0x800) == 0)
        goto LAB_026c3a2b;
      uStack_f0 = uStack_f0 | 2;
      if ((-1 < iVar13) && (instance = get_card_instance(iVar11, iVar13),
                            (instance->targets[0x10].card & 0x2000) != 0))
        goto LAB_026c45c0;
    }
  LAB_026c3a56:
    if (event == EVENT_TRIGGER)
    {
      event_result = event_result | 2;
    }
    else
    {
      if (event != EVENT_RESOLVE_TRIGGER)
        goto LAB_026c384a;
      piVar6 = graveyard_ptr_mutable[player];
      iVar11 = count_graveyard(player);
      iVar11 = iVar11 + -1;
      if (-1 < iVar11)
      {
        do
        {
          iVar13 = piVar6[iVar11];
          uVar1 = cards_data[iVar13].id;
          if (uVar1 == 0x2e0d)
          {
            if ((uStack_f0 & 1) == 0)
              goto LAB_026c3ac0;
          LAB_026c3b8e:
            add_card_to_hand(player, iVar13);
            remove_card_from_grave(player, iVar11);
          }
          else if (uVar1 == 0x338e)
          {
            if ((((uStack_f0 & 4) != 0) && (iVar13 = has_mana(player, 0, 3), iVar13 != 0)) &&
                (((iVar13 = duh_mode(player),
                   iVar13 != 0 || (iVar13 = do_dialog(player, player, card, -1, -1,
                                                      " Return Pyrewild Shaman\n Pass", 0),
                                   iVar13 == 0)) &&
                  (iVar13 = charge_mana_while_resolving_csvid(0x338e, EVENT_RESOLVE_TRIGGER, player,
                                                              COLOR_COLORLESS, 3),
                   iVar13 != 0))))
            {
            LAB_026c3b87:
              iVar13 = piVar6[iVar11];
              goto LAB_026c3b8e;
            }
          }
          else
          {
            if (((uVar1 == 0x246a) && ((uStack_f0 & 2) != 0)) &&
                ((iVar13 = duh_mode(player),
                  iVar13 != 0 || (iVar13 = do_dialog(player, player, card, -1, -1,
                                                     " Return Auntie\'s Snitch\n Pass", 0),
                                  iVar13 == 0))))
              goto LAB_026c3b87;
          }
        LAB_026c3ac0:
          iVar11 = iVar11 + -1;
        } while (iVar11 != -1);
      }
      increase_trap_condition_impl(player, 0x603, 0, 0);
    }
  }
LAB_026c3857:
  instance = get_card_instance(player, card);
  if ((instance->targets[1].player < 1) || (trigger_condition != 0xd5))
  {
  LAB_026c387c:
    xVar18 = trigger_condition;
    eVar17 = enable_xtrigger_flags | ENABLE_XTRIGGER_MILLED;
    if (trigger_condition != 0xe5)
    {
      if (trigger_condition == XTRIGGER_MILLED)
        goto LAB_026c3bb8;
      goto LAB_026c38a4;
    }
    if (xtrigger_impl_value_dont_use_directly == XTRIGGER_MILLED)
    {
    LAB_026c3bb8:
      if (((card == affected_card) && (player == affected_card_controller)) &&
          ((reason_for_trigger_controller == player &&
            (enable_xtrigger_flags = eVar17, uVar12 = get_graveyard_counts(player),
             (uVar12 & 0x400000) != 0))))
      {
        if (1 < event - EVENT_TRIGGER)
        {
          return;
        }
        if (0 < num_cards_milled)
        {
          iVar11 = 0;
          uStack_f0 = 0;
          do
          {
            iVar13 = is_what(-1, cards_milled[iVar11].internal_card_id, 1);
            if (iVar13 != 0)
            {
              if (event == EVENT_TRIGGER)
                goto LAB_026c46e2;
              uStack_f0 = uStack_f0 + 1;
            }
            iVar11 = iVar11 + 1;
          } while (iVar11 < num_cards_milled);
          eVar17 = enable_xtrigger_flags;
          xVar18 = trigger_condition;
          if (uStack_f0 != 0)
          {
            parse_grave_for_centaur_vinecrasher(player, card);
            eVar17 = enable_xtrigger_flags;
            xVar18 = trigger_condition;
          }
        }
      }
      enable_xtrigger_flags = eVar17 | ENABLE_XTRIGGER_CARD_TO_GY_FROM_ANYWHERE_BUT_LIBRARY;
      xVar7 = xtrigger_impl_value_dont_use_directly;
      if (xVar18 == 0xe5)
        goto joined_r0x026c3be5;
      goto LAB_026c38ad;
    }
    enable_xtrigger_flags =
        enable_xtrigger_flags |
        (ENABLE_XTRIGGER_CARD_TO_GY_FROM_ANYWHERE_BUT_LIBRARY | ENABLE_XTRIGGER_MILLED);
    xVar7 = xtrigger_impl_value_dont_use_directly;
  }
  else
  {
    if ((card == affected_card) &&
        ((player == affected_card_controller && (reason_for_trigger_controller == player))))
    {
      instance = get_card_instance(player, card);
      if (event == EVENT_TRIGGER)
      {
        event_result = event_result | 2;
      }
      else
      {
        if (event == EVENT_RESOLVE_TRIGGER)
        {
          uVar12 = instance->targets[1].player;
          piVar6 = graveyard_ptr_mutable[player];
          iVar11 = count_graveyard(player);
          uVar12 = uVar12 & 1;
          iVar11 = iVar11 + -1;
          if (-1 < iVar11)
          {
            do
            {
              uVar5 = cards_data[piVar6[iVar11]].id;
              if (uVar5 == 0x20d1)
              {
                iVar13 = has_mana_multi(player, 1, 2, 0, 0, 0, 0);
                if ((iVar13 != 0) && (uVar12 != 0))
                {
                  iVar13 = do_dialog(player, player, card, -1, -1,
                                     " Return Krovikan Rot to your hand\n Do not return", 0);
                  if (iVar13 != 0)
                    goto LAB_026c5470;
                  iVar20 = 0;
                  iVar14 = 0;
                  iVar13 = 2;
                LAB_026c5453:
                  charge_mana_multi(player, 1, iVar13, 0, iVar14, 0, iVar20);
                  if (spell_fizzled == 1)
                    goto LAB_026c5470;
                LAB_026c536f:
                  add_card_to_hand(player, piVar6[iVar11]);
                  remove_card_from_grave(player, iVar11);
                }
              }
              else if ((unsigned short)uVar5 < 0x20d2)
              {
                if (uVar5 == 0x20ba)
                {
                  iVar13 = has_mana_multi(player, 2, 1, 0, 0, 0, 0);
                  if ((iVar13 != 0) && (uVar12 != 0))
                  {
                    iVar13 = do_dialog(player, player, card, -1, -1,
                                       " Return Grim Harvest to your hand\n Do not return", 0);
                    if (iVar13 == 0)
                    {
                      iVar14 = 0;
                      iVar13 = 1;
                    LAB_026c55c9:
                      charge_mana_multi(player, 2, iVar13, iVar14, 0, 0, 0);
                    joined_r0x026c55e0:
                      if (spell_fizzled != 1)
                        goto LAB_026c536f;
                    }
                    goto LAB_026c5470;
                  }
                }
                else if ((unsigned short)uVar5 < 0x20bb)
                {
                  if (uVar5 == 0x209f)
                  {
                    iVar13 = has_mana_multi(player, 2, 0, 2, 0, 0, 0);
                    if ((iVar13 != 0) && (uVar12 != 0))
                    {
                      iVar13 = do_dialog(player, player, card, -1, -1,
                                         " Return Controvert to your hand\n Do not return", 0);
                      if (iVar13 == 0)
                      {
                        iVar14 = 2;
                        iVar13 = 0;
                        goto LAB_026c55c9;
                      }
                      goto LAB_026c5470;
                    }
                  }
                  else if (((uVar5 == 0x20b4) &&
                            (iVar13 = can_pay_life(player, (life[player] + 1) / 2), iVar13 != 0)) &&
                           (uVar12 != 0))
                  {
                    iVar13 = do_dialog(player, player, card, -1, -1,
                                       " Return Garza\'s Assassin to your hand\n Do not return", 0);
                    if (iVar13 == 0)
                    {
                      lose_life(player, (life[player] + 1) / 2);
                      goto LAB_026c536f;
                    }
                    goto LAB_026c5470;
                  }
                }
                else if (((uVar5 == 0x20c2) &&
                          (iVar13 = has_mana_multi(player, 0, 0, 0, 0, 2, 0), iVar13 != 0)) &&
                         (uVar12 != 0))
                {
                  iVar13 = do_dialog(player, player, card, -1, -1,
                                     " Return Icefall to your hand\n Do not return", 0);
                  if (iVar13 == 0)
                  {
                    charge_mana_multi(player, 0, 0, 0, 0, 2, 0);
                    goto joined_r0x026c55e0;
                  }
                  goto LAB_026c5470;
                }
              }
              else if (uVar5 == 0x21a8)
              {
                if (((uVar12 != 0) && (iVar13 = has_mana(player, 1, 1), iVar13 != 0)) &&
                    (iVar13 = do_dialog(player, player, card, -1, -1,
                                        " Return Nether Traitor to play\n Do not return", 0),
                     iVar13 == 0))
                {
                  charge_mana(player, COLOR_BLACK, 1);
                joined_r0x026c550b:
                  if (spell_fizzled != 1)
                  {
                    reanimate_permanent(player, -1, player, iVar11, REANIMATE_DEFAULT);
                  }
                }
              }
              else if ((unsigned short)uVar5 < 0x21a9)
              {
                if (uVar5 == 0x20ea)
                {
                  iVar13 = has_mana_multi(player, 1, 0, 0, 1, 0, 0);
                  if ((iVar13 != 0) && (uVar12 != 0))
                  {
                    iVar13 = do_dialog(player, player, card, -1, -1,
                                       " Return Resize to your hand\n Do not return", 0);
                    if (iVar13 == 0)
                    {
                      iVar20 = 0;
                      iVar14 = 1;
                      goto LAB_026c5443;
                    }
                    goto LAB_026c5470;
                  }
                }
                else if (((uVar5 == 0x2102) &&
                          (iVar13 = has_mana_multi(player, 1, 0, 0, 0, 0, 1), iVar13 != 0)) &&
                         (uVar12 != 0))
                {
                  iVar13 = do_dialog(player, player, card, -1, -1,
                                     " Return Sun\'s Bounty to your hand\n Do not return", 0);
                  if (iVar13 == 0)
                  {
                    iVar20 = 1;
                    iVar14 = 0;
                  LAB_026c5443:
                    iVar13 = 0;
                    goto LAB_026c5453;
                  }
                LAB_026c5470:
                  rfg_card_from_grave(player, iVar11);
                }
              }
              else
              {
                if (((uVar5 == 0x4387) && (uVar12 != 0)) &&
                    ((iVar13 = has_mana_multi(player, 1, 1, 0, 0, 0, 0),
                      iVar13 != 0 && (iVar13 = do_dialog(
                                          player, player, card, -1, -1,
                                          " Return Boneyard Scourge to play\n Do not return", 0),
                                      iVar13 == 0))))
                {
                  charge_mana_multi(player, 1, 1, 0, 0, 0, 0);
                  goto joined_r0x026c550b;
                }
              }
              iVar11 = iVar11 + -1;
            } while (iVar11 != -1);
          }
        }
        else if (event != EVENT_END_TRIGGER)
          goto LAB_026c387c;
        instance = get_card_instance(player, card);
        instance->targets[1].player = 0;
      }
      goto LAB_026c387c;
    }
    eVar17 = enable_xtrigger_flags | ENABLE_XTRIGGER_MILLED;
  LAB_026c38a4:
    enable_xtrigger_flags = eVar17 | ENABLE_XTRIGGER_CARD_TO_GY_FROM_ANYWHERE_BUT_LIBRARY;
    xVar18 = trigger_condition;
  LAB_026c38ad:
    xVar7 = xVar18;
  }
joined_r0x026c3be5:
  if ((xVar7 == XTRIGGER_CARD_TO_GY_FROM_ANYWHERE_BUT_LIBRARY) &&
      (uVar12 = get_graveyard_counts(player), (uVar12 & 0x400000) != 0))
  {
    default_test_definition((test_definition_t *)buffer, 1);
    iVar11 = when_a_card_is_put_into_graveyard_from_anywhere_but_library_do_something(
        player, card, event, player, RESOLVE_TRIGGER_MANDATORY, (test_definition_t *)buffer);
    if (iVar11 != 0)
    {
      parse_grave_for_centaur_vinecrasher(player, card);
    }
  }
  iVar11 = beginning_of_combat(player, card, event, player);

  if ((((iVar11 != 0) && (rules_engine_card[player] != -1)) &&
       (iVar11 = count_counters(player, rules_engine_card[player], COUNTER_CHARGE), 2 < iVar11)) &&
      (uVar12 = get_graveyard_counts(player), (uVar12 & 0x20000000) != 0))
  {
    piVar6 = graveyard_ptr_mutable[player];
    iVar11 = count_graveyard(player);
    iVar11 = iVar11 + -1;
    if (-1 < iVar11)
    {
      do
      {
        if (cards_data[piVar6[iVar11]].id == 0x4b50)
        {
          reanimate_permanent(player, card, player, iVar11, REANIMATE_DEFAULT);
        }
        iVar11 = iVar11 + -1;
      } while (iVar11 != -1);
    }
  }
  if (trigger_condition != 0xe0)
  {
    return;
  }
  if (card != affected_card)
  {
    return;
  }
  if (player != affected_card_controller)
  {
    return;
  }
  if (reason_for_trigger_controller != player)
  {
    return;
  }
  uVar12 = get_graveyard_counts(player);
  if ((uVar12 & 0x40000000) == 0)
  {
  LAB_026c4560:
    if (-1 < (int)uVar12)
    {
      return;
    }
    if (trigger_cause_controller == player)
    {
      return;
    }
  LAB_026c3eb8:
    iVar11 = has_mana(player, 4, 1);
    if (iVar11 == 0)
    {
      return;
    }
    bVar8 = false;
  }
  else
  {
    if (trigger_cause_controller != player)
    {
      if (-1 < (int)uVar12)
      {
        return;
      }
      goto LAB_026c3eb8;
    }
    if (rules_engine_card[player] < 0)
    {
    LAB_026c4723:
      bVar9 = false;
      bVar8 = true;
      goto LAB_026c3edf;
    }
    instance = get_card_instance(player, rules_engine_card[player]);
    if (*(short *)((int)&instance->targets[5].player + 2) != 0)
      goto LAB_026c4560;
    if (((-1 < (int)uVar12) || (trigger_cause_controller == player)) ||
        (iVar11 = has_mana(player, 4, 1), iVar11 == 0))
      goto LAB_026c4723;
    bVar8 = true;
  }
  bVar9 = true;
LAB_026c3edf:
  if (event == EVENT_TRIGGER)
  {
  LAB_026c46e2:
    event_result = event_result | 2;
  }
  else if (event == EVENT_RESOLVE_TRIGGER)
  {
    piVar6 = graveyard_ptr_mutable[player];
    iVar11 = count_graveyard(player);
    iVar11 = iVar11 + -1;
    if (-1 < iVar11)
    {
      do
      {
        uVar2 = cards_data[piVar6[iVar11]].id;
        if (uVar2 == 0x298d)
        {
          if ((bVar9) && (iVar13 = has_mana(player, 4, 1), iVar13 != 0))
          {
            iVar13 = add_card_to_hand(player, piVar6[iVar11]);
            add_state(player, iVar13, 0x20);
            hand_count[player] = hand_count[player] + -1;
            iVar14 =
                do_dialog(player, player, iVar13, -1, -1, " Get back Punishing Fire\n Pass", 0);
            if ((iVar14 == 0) && (charge_mana(player, COLOR_RED, 1), spell_fizzled != 1))
            {
              from_grave_to_hand(player, iVar11, -1);
            }
            obliterate_card(player, iVar13);
          }
        }
        else if ((uVar2 == 0x5051) && (bVar8))
        {
          from_grave_to_hand(player, iVar11, -1);
        }
        iVar11 = iVar11 + -1;
      } while (iVar11 != -1);
    }
  }
  return;
}

static void keep_turn_count(int player, int card, event_t event)
{
  if (event == EVENT_BEGIN_TURN && current_turn == player)
	increase_trap_condition(player, TRAP_TURN_COUNT, 1);
}

int card_coa_special_effect(int player, int card, event_t event);

static int initial_upkeep_triggers(int player, int card, event_t event ){

	if( player == HUMAN && ! get_trap_condition(HUMAN, TRAP_FIRST_TURN_UPKEEP) && upkeep_trigger(player, card, event) ){
		int i;

		// deal with serum powder
		if( player == HUMAN ){

			int p = HUMAN;
			int choice = 1;
			int serum_found = 1;
			if( ! is_unlocked(player, card, event, 7) ){ serum_found = 0; }
			while( serum_found == 1 ){
				serum_found = 0;
				for(i=0;i<active_cards_count[p];i++){
					if( in_hand(p, i) && get_id(p, i ) == CARD_ID_SERUM_POWDER ){
						choice = do_dialog(p, p, i, -1, -1, " Take Serum Powder Mulligan\n Keep Hand", 0);
						serum_found = 1;
						break;
					}
				}
				if( serum_found == 0 || choice == 1 ){
					break;
				}

				int hand_size = 0;
				for(i=0;i<active_cards_count[player];i++){
					if( in_hand(player, i) ){
						hand_size++;
						kill_card(player, i, KILL_EXILE);
					}
				}

				// proceed with normal mulliganning
				int cards = hand_size;
				while(cards > 0 ){
					// draw the appropriate number of cards
					shuffle_vanguard(player, 1);
					draw_cards(player, cards);

					// allow the choice to mulligan
					choice = do_dialog(player, player, card, -1, -1, " Keep\n Mulligan", 0);
					if( choice == 1 ){
						cards--;
					}
					else{
						break;
					}

					// put your hand back into the deck
					for (i = 0; i < active_cards_count[p]; ++i){
						if (in_hand(player, i)){
							put_on_top_of_deck(player, i);
						}
					}
				}
			}


		}
		for(i=0; i<2; i++){
			// leylines and chancellors
			int count = active_cards_count[i]-1;
			while( count > -1 ){
					if( in_hand(i, count) ){
						int id = get_id(i, count );
						if( id == CARD_ID_LEYLINE_OF_THE_VOID ||
						   id == CARD_ID_LEYLINE_OF_SANCTITY ||
						   id == CARD_ID_LEYLINE_OF_PUNISHMENT ||
						   id == CARD_ID_LEYLINE_OF_VITALITY ||
						   id == CARD_ID_LEYLINE_OF_SINGULARITY ||
						   id == CARD_ID_LEYLINE_OF_LIGHTNING ||
						   id == CARD_ID_LEYLINE_OF_THE_MEEK ||
						   id == CARD_ID_LEYLINE_OF_LIFEFORCE
						 ){
							card_ptr_t* c_me = cards_ptr[ id ];
							char buffer[100];
							snprintf(buffer, 100, " Put %s into play\n Leave it in hand", c_me->name);
							int choice2 = do_dialog(i, i, count, -1, -1, buffer, 0);
							if( choice2 == 0 ){
							   put_into_play(i, count);
							}
						}
						if( id == CARD_ID_CHANCELLOR_OF_THE_ANNEX ){
							int choice2 = do_dialog(i, i, count, -1, -1, " Reveal Chancellor of the Annex\n Pass", 0);
							if( choice2 == 0 ){
								int leg = create_legacy_effect(i, count, card_coa_special_effect);
								set_special_infos(i, leg, 66);
							}
						}
						if( id == CARD_ID_CHANCELLOR_OF_THE_DROSS ){
							int choice2 = do_dialog(i, i, count, -1, -1, " Reveal Chancellor of the Dross\n Pass", 0);
							if( choice2 == 0 ){
								lose_life(1-i, 3);
								gain_life(i, 3);
							}
						}
						if( id == CARD_ID_CHANCELLOR_OF_THE_FORGE ){
							int choice2 = do_dialog(i, i, count, -1, -1, " Reveal Chancellor of the Forge\n Pass", 0);
							if( choice2 == 0 ){
								token_generation_t token;
								default_token_definition(player, card, CARD_ID_GOBLIN, &token);
								token.t_player = i;
								token.s_key_plus = SP_KEYWORD_HASTE;
								generate_token(&token);
							}
						}
						if( id == CARD_ID_CHANCELLOR_OF_THE_SPIRES ){
							int choice2 = do_dialog(i, i, count, -1, -1, " Reveal Chancellor of the Spires\n Pass", 0);
							if( choice2 == 0 ){
								mill(1-i, 7);
							}
						}
						if( id == CARD_ID_CHANCELLOR_OF_THE_TANGLE ){
							int choice2 = do_dialog(i, i, count, -1, -1, " Reveal Chancellor of the Tangle\n Pass", 0);
							if( choice2 == 0 ){
								increase_trap_condition(i, TRAP_COT_FLAG, 1);
								/*
								int legacy_card = create_legacy_effect(i, count, &effect_mana_drain );
								card_instance_t *legacy = get_card_instance(i, legacy_card);
								legacy->targets[0].player = PHASE_MAIN1;
								legacy->targets[1].card = CARD_ID_CHANCELLOR_OF_THE_TANGLE;
								*/
							}
						}
						if( id == CARD_ID_GEMSTONE_CAVERNS && hand_count[player] > 1 ){
							int choice2 = do_dialog(i, i, count, -1, -1, " Reveal Gemstone Caverns\n Pass", 0);
							if( choice2 == 0 ){
								target_definition_t td;
								default_target_definition(i, count, &td, TYPE_ANY);
								td.zone = TARGET_ZONE_HAND;
								td.allowed_controller = i;
								td.preferred_controller = i;
								td.who_chooses = i;
								td.illegal_abilities = 0;

								card_instance_t *instance2 = get_card_instance(i, count);

								state_untargettable(i, count, 1);

								if( pick_target(&td, "TARGET_CARD") ){
									instance2->number_of_targets = 1;
									rfg_card_in_hand(instance2->targets[0].player, instance2->targets[0].card);
									put_into_play(i, count);
									add_counters(i, count, i, count, COUNTER_LUCK, 1);
								}

								state_untargettable(i, count, 0);
							}
						}
						if( id == CARD_ID_PROVIDENCE ){
							int choice2 = do_dialog(i, i, count, -1, -1, " Reveal Providence\n Pass", 0);
							if( choice2 == 0 ){
								set_life_total(i, 26);
							}
						}
					}
					count--;
			}
		}
		increase_trap_condition(HUMAN, TRAP_FIRST_TURN_UPKEEP, 1);
	}

	// deal with challenges
	if( get_setting(SETTING_CHALLENGE_MODE) ){
		if( (get_challenge3() == 5 || get_challenge4() == 0 || get_challenge4() == 1 ||(gauntlet_round >= 29 && gauntlet_round < 40) || (gauntlet_round >= 59)) && current_turn == HUMAN ){
			if( player == HUMAN && upkeep_trigger(player, card, event) ){
				if( get_challenge4() == 0 || get_challenge4() == 1 || (gauntlet_round >= 29 && gauntlet_round < 40) ||
					(gauntlet_round >= 59 && gauntlet_round < 70) || get_challenge3() == 5
				  ){
					apply_challenge_onslaught();
				}
				else if(gauntlet_round >= 70){
						apply_challenge_puzzle_upkeep();
				}
			}
		}
	}


	if( get_setting(SETTING_GAMESTART_SETTINGS_APPLIED) == 0 && ( get_setting(SETTING_AUTO_MILL) || get_setting(SETTING_CRIPPLE_AI) || get_setting(SETTING_CHECK_DECK_LEGALITY) || get_setting(SETTING_CHALLENGE_MODE)) ){
		if( player == HUMAN && trigger_condition == TRIGGER_UPKEEP && affect_me(player, card) ){
			if(event == EVENT_TRIGGER){
				event_result |= RESOLVE_TRIGGER_MANDATORY;
			}
			else if(event == EVENT_RESOLVE_TRIGGER){
				if (ai_is_speculating != 1){
					settings[SETTING_GAMESTART_SETTINGS_APPLIED] = 1;
					force_init_upkeep = 0;
				}
				if(get_setting(SETTING_AUTO_MILL) ){
					if( count_deck(HUMAN) > 30 ){
						mill(HUMAN, 30);
						mill(AI, 30);
					}
				}
				if( get_setting(SETTING_CRIPPLE_AI) ){
					// put every card back into the deck
					int count = 0;
					while( count < active_cards_count[AI]){
						if(! in_play(AI, count) ){
							put_on_top_of_deck(AI, count);
						}
						count++;
					}
				}
				if(( get_setting(SETTING_CHECK_DECK_LEGALITY) || get_setting(SETTING_CHALLENGE_MODE) ) && get_challenge_round() != 19){
					int illegal_card = check_deck_legality();
					if ( illegal_card > -1 && POISON_COUNTERS(HUMAN) == 0){
						//gain_life(HUMAN, get_challenge_round());
						if( get_setting(SETTING_CHALLENGE_MODE) ){
							lose_the_game(HUMAN);
						}
						else{
							do_dialog(HUMAN, HUMAN, illegal_card, -1, -1, "Your deck is not legal.", 0);
							lose_the_game(HUMAN);
						}

					}
				}
				if( get_setting(SETTING_CHALLENGE_MODE) ){
					debug_mode = 0;
					challenge_mode_upkeep(0);
				}
			}
		}
	}

	if( event == EVENT_PHASE_CHANGED && current_turn == player && current_phase == PHASE_MAIN1 && get_trap_condition(player, TRAP_COT_FLAG) == 1 ){
		produce_mana(player, COLOR_GREEN, 1);
		set_trap_condition(player, TRAP_COT_FLAG, 0);
	}

	return 0;
}

static int count_cards_actually_in_hand(int player){
	int c, total = 0;
	for (c = 0; c < active_cards_count[player]; ++c){
		if (in_hand(player, c)
			&& !(cards_data[get_card_instance(player, c)->internal_card_id].type & TYPE_EFFECT)){	// e.g. "draw a card"
			++total;
		}
	}
	return total;
}

static void debug_settings(int player, int card, event_t event){
	// if we are in challenge mode, make sure all the debug settings are off
	if( get_setting(SETTING_CHALLENGE_MODE) ){
		if( debug_mode == 1 || get_setting(SETTING_AUTO_MILL) || get_setting(SETTING_UNLIMITED_MANA) || get_setting(SETTING_AUTO_HASTE) || get_setting(SETTING_CRIPPLE_AI) || game_number > 0 ){
			do_dialog(HUMAN, player, card, -1, -1, "Invalid Settings", 0);
			lose_the_game(HUMAN);
		}

		if( gauntlet_round < 30 && (( gauntlet_round - get_challenge_round() > 1)  || ( get_challenge_round() - gauntlet_round > 1))  ){
			char buffer[100];
			snprintf(buffer, 100, " gauntlet: %d, challenge: %d", gauntlet_round , get_challenge_round());
			do_dialog(HUMAN, player, card, -1, -1, buffer, 0);
			lose_the_game(HUMAN);
		}

		/*
		if(  count_cards_by_id(HUMAN, CARD_ID_RULES_ENGINE) == 0 && current_phase > PHASE_UPKEEP){
			do_dialog(HUMAN, player, card, -1, -1, "No rules engine", 0);
			lose_the_game(HUMAN);
		}
		*/
	}

	// give each play unlimited mana
	if( get_setting(SETTING_UNLIMITED_MANA) == 1 ){
		int p, c;
		for(p=0;p<2;p++){
			for(c=0;c<6;c++){
				if( ! has_mana(p, c, 10) ) {
					produce_mana(p, c, 10);
				}
			}
		}
	}

	// give all creatures haste
	if( get_setting(SETTING_AUTO_HASTE) ){
		haste(affected_card_controller, affected_card, event);
	}

	if( get_setting(SETTING_VERIFY_HAND_COUNT)
		&& event == EVENT_STATIC_EFFECTS
		&& current_phase != PHASE_DRAW){	// the draw-a-card legacy is sometimes considered to be in hand by the exe, sometimes not - just avoid the issue
		get_card_instance(player, card)->info_slot |= REF_LEYLINES;	// so TRAP_FIRST_TURN_UPKEEP is updated
		if (get_trap_condition(HUMAN, TRAP_FIRST_TURN_UPKEEP)){	// to avoid the false positive at start of game
			int actually_in_hand = count_cards_actually_in_hand(player);
			if (actually_in_hand != hand_count[player]){
				if (ai_is_speculating != 1){
					popup("Rules Engine", "Rules Engine detected hand count inconsistency for %s\nhand_count[%d]=%d\ncount_cards_actually_in_hand(%d)=%d",
						  player == 0 ? "human" : "ai", player, hand_count[player], player, actually_in_hand);
				}
				hand_count[player] = actually_in_hand;
			}
		}
	}
}

static int effect_challenge3(int player, int card, event_t event ){
	if( eot_trigger(player, card, event) ){
		life[HUMAN]=0;
	}
	return 0;
}

static int gauntlet_settings(int player, int card, event_t event ){
	if( ! get_setting(SETTING_CHALLENGE_MODE) ){
		return 0;
	}

	// during the pit challenge, legends have shroud
	if( get_challenge4() == 1 && event == EVENT_ABILITIES ){
		if( has_creature_type(affected_card_controller, affected_card, SUBTYPE_LEGEND ) ){
			event_result |= KEYWORD_SHROUD;
		}
	}

	// during the enchanted challenge, enchantments are indestructible
	if( get_challenge4() == 6 ){
		int c=0;
		for(c=0;c< active_cards_count[AI];c++){
			card_data_t* card_d = get_card_data(AI, c);
			if((card_d->type & TYPE_ENCHANTMENT) && in_play(AI, c)){
				indestructible(AI, c, event);
			}
		}
	}

	// if we won the enchanted challenge, your enchantments are indestructible
	if( event == EVENT_GRAVEYARD_FROM_PLAY && is_unlocked(HUMAN, card, event, 36) ){
		int c=0;
		for(c=0;c< active_cards_count[HUMAN];c++){
			card_data_t* card_d = get_card_data(HUMAN, c);
			if((card_d->type & TYPE_ENCHANTMENT) && in_play(HUMAN, c)){
				indestructible(HUMAN, c, event);
			}
		}
	}

	if( (( gauntlet_round >= 71 && gauntlet_round <= 80 ) || get_challenge2() == 1 ) && player == HUMAN){
		skip_your_draw_step(player, event);
	}

	if( get_challenge2() == 3 ){
		if(event == EVENT_MODIFY_COST_GLOBAL && player == HUMAN && affected_card_controller == player
		   && get_trap_condition(player, TRAP_CHALLENGE_EPIC) && !is_what(affected_card_controller, affected_card, TYPE_LAND)){
			infinite_casting_cost();
		}

		// keep track of spells played
		if(trigger_condition == TRIGGER_SPELL_CAST && event == EVENT_END_TRIGGER && affect_me(player, card)
		   && player == reason_for_trigger_controller && player == HUMAN && player == trigger_cause_controller
		   && !is_what(trigger_cause_controller, trigger_cause, TYPE_LAND)){
			set_trap_condition(player, TRAP_CHALLENGE_EPIC, 1);
		}
	}

	if( get_challenge3() == 8 ){
		if(trigger_condition == TRIGGER_SPELL_CAST && event == EVENT_END_TRIGGER && affect_me(player, card)
		   && player == reason_for_trigger_controller && trigger_cause_controller == HUMAN && player == HUMAN)
		{
			card_instance_t *played= get_card_instance(trigger_cause_controller, trigger_cause);
			if( ! ( cards_data[ played->internal_card_id].type & TYPE_LAND ) ){
				create_legacy_effect(player, card, &effect_challenge3 );
			}
		}
	}
	else if( get_challenge4() == 3 ){
		if(trigger_condition == TRIGGER_SPELL_CAST && affect_me(player, card)
		   && player == reason_for_trigger_controller && trigger_cause_controller == AI && player == AI)
		{
			card_instance_t *played= get_card_instance(trigger_cause_controller, trigger_cause);
			if( ! ( cards_data[ played->internal_card_id].type & TYPE_LAND ) ){
				if(event == EVENT_TRIGGER){
					event_result |= RESOLVE_TRIGGER_MANDATORY;
				}
				else if(event == EVENT_RESOLVE_TRIGGER){
					do_cascade(trigger_cause_controller, trigger_cause, -1);
				}
			}
		}
	}

	int round = get_challenge_round() - 1;

	// end the game if the time limit for puzzle challenge is over
	if( ( gauntlet_round == 72 || gauntlet_round == 73 ) && current_turn == HUMAN && current_phase == PHASE_MAIN1 ){
		life[HUMAN]=0;
		raw_set_poison(HUMAN, 10);
	}
	else if( gauntlet_round > 70 && gauntlet_round < 81 && current_turn == HUMAN && eot_trigger(player, card, event) ){
		life[HUMAN]=0;
		raw_set_poison(HUMAN, 10);
	}

	if( round < 49 || gauntlet_round > 59 ){ return 0; }
	/*
	if(event == EVENT_MODIFY_COST_GLOBAL ){
		card_instance_t *instance = get_card_instance( affected_card_controller, affected_card);
		int type = cards_data[ instance->internal_card_id ].type;

		if( round == 49 && ( instance->card_color & COLOR_TEST_BLACK ) && ! ( type & TYPE_LAND ) && ! ( type & TYPE_ARTIFACT ) ){
			infinite_casting_cost();
		}
		else if( round == 50 && ! ( type & TYPE_LAND ) && ! ( type & TYPE_ARTIFACT )&&instance->card_color & COLOR_TEST_BLUE ){
			infinite_casting_cost();
		}
		else if( round == 51 && ! ( type & TYPE_LAND ) && ! ( type & TYPE_ARTIFACT )&&instance->card_color & COLOR_TEST_GREEN ){
			infinite_casting_cost();
		}
		else if( round == 52 && ! ( type & TYPE_LAND ) && ! ( type & TYPE_ARTIFACT )&&instance->card_color & COLOR_TEST_RED ){
			infinite_casting_cost();
		}
		else if( round == 53 && ! ( type & TYPE_LAND ) && ! ( type & TYPE_ARTIFACT )&& instance->card_color & COLOR_TEST_WHITE ){
			infinite_casting_cost();
		}
		else if( round == 54 && type & TYPE_ARTIFACT ){
			infinite_casting_cost();
		}
		else if( round == 55 && is_what(affected_card_controller, affected_card, TYPE_ENCHANTMENT | TARGET_TYPE_PLANESWALKER)){
			infinite_casting_cost();
		}
		else if( round == 56 && type & TYPE_CREATURE ){
			infinite_casting_cost();
		}
		else if( round == 57 && is_what(affected_card_controller, affected_card, TYPE_SPELL)){
			infinite_casting_cost();
		}
		else if( round == 58 && type & TYPE_LAND ){
			infinite_casting_cost();
		}
	}
	*/
	return 0;
}

static void vengeful_pharaoh_effect(int player, int card, event_t event ){

	if( trigger_condition == TRIGGER_DEAL_DAMAGE && affect_me(player, card) &&
		reason_for_trigger_controller == player && current_turn == 1-player
	  ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);
		td.required_state = TARGET_STATE_ATTACKING;
		td.allowed_controller = 1-player;
		td.preferred_controller = 1-player;
		td.illegal_abilities |= KEYWORD_PROT_BLACK | KEYWORD_PROT_CREATURES;
		td.allow_cancel = 0;

		if( can_target(&td) && (current_phase == PHASE_FIRST_STRIKE_DAMAGE || current_phase == PHASE_NORMAL_COMBAT_DAMAGE)
		  ){
			if(event == EVENT_TRIGGER){
				event_result |= 2;
			}
			else if( event == EVENT_RESOLVE_TRIGGER ){
				const int *grave = get_grave(player);
				int count = count_graveyard(player)-1;
				while( count > -1 ){
					if( cards_data[grave[count]].id == CARD_ID_VENGEFUL_PHARAOH && can_target(&td) ){
						pick_target(&td, "TARGET_CREATURE");
						card_instance_t *instance = get_card_instance(player, card);
						kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
						int card_added = add_card_to_hand(player, grave[count]);
						remove_card_from_grave(player, count);
						put_on_top_of_deck(player, card_added);
					}
					count--;
				}
			}
		}
	}

	return;
}

int check_card_for_rules_engine(int card)
{
  uint16_t uVar1;
  uint16_t uVar2;
  unsigned int uVar3;
  int result;
  unsigned char bVar5;
  bool bVar6;
  unsigned int uStack_14;

  if (card < 0)
  {
    return 0;
  }
  uVar3 = (uint16_t)cards_data[card].id | ((uint32_t)(uint16_t)cards_data[card].reserved2 << 16);
  uStack_14 = (unsigned int)((((((((((short)uVar3 == 0x1b29 || (short)uVar3 == 0x2d39) ||
                                    (short)uVar3 == 0x2170) ||
                                   (short)uVar3 == 0x21d7) ||
                                  (short)uVar3 == 0x3f2c) ||
                                 (short)uVar3 == 0x4d27) ||
                                (unsigned short)((short)uVar3 + 0xe06fU) < 5) ||
                               (unsigned short)((short)uVar3 + 0xb074U) < 2) ||
                              (unsigned short)((short)uVar3 + 0xd47dU) < 4) ||
                             (unsigned short)((short)uVar3 + 0xd2c5U) < 4);
  bVar5 = cards_data[card].cc[2];
  if (bVar5 == 0xd)
  {
    bVar6 = (short)uVar3 == 0x5051;
    uStack_14 = uStack_14 | 2;
  LAB_026c84c9:
    if (((((short)uVar3 == 0x1cd2 ||
           ((short)uVar3 == 0x2971 ||
            (((short)uVar3 == 0x1998 ||
              ((short)uVar3 == 0xc87 || ((short)uVar3 == 0x9e3 || (short)uVar3 == 0x2844))) ||
             (short)uVar3 == 0x19ca))) ||
          (short)uVar3 == 0x4067) ||
         (short)uVar3 == 0x40fe) ||
        ((short)uVar3 == 0x56d1))
      goto LAB_026c8528;
  }
  else
  {
    if (((short)uVar3 == 0x2971) || ((short)uVar3 == 0x299b))
    {
    LAB_026c83f8:
      bVar6 = (short)uVar3 == 0x5051;
      if ((((short)uVar3 == 0x54db ||
            ((((short)uVar3 == 0x3ff4 ||
               (((short)uVar3 == 0x3b2f ||
                 ((short)uVar3 == 0x2033 ||
                  (((short)uVar3 == 0x37d2 ||
                    ((short)uVar3 == 0x3799 ||
                     (((short)uVar3 == 0x2e0d ||
                       ((short)uVar3 == 0x338e ||
                        ((short)uVar3 == 0x290e || (short)uVar3 == 0x2a4a))) ||
                      (short)uVar3 == 0x3951))) ||
                   (short)uVar3 == 0x2985))) ||
                (short)uVar3 == 0x40fd)) ||
              (short)uVar3 == 0x42fe) ||
             bVar6)) ||
           (short)uVar3 == 0x5901) ||
          (short)uVar3 == 0x56c2)
        goto LAB_026c84a7;
      bVar6 = false;
    }
    else
    {
      result = has_subtype_by_id(-1, uVar3 & 0xffff, SUBTYPE_TRAP);
      if (result == 0)
      {
        uVar2 = cards_data[card].id;
        bVar5 = cards_data[card].cc[2];
        uVar3 = (uVar3 & 0xffff0000U) | (uint16_t)uVar2;
        goto LAB_026c83f8;
      }
      uVar1 = cards_data[card].id;
      bVar5 = cards_data[card].cc[2];
      bVar6 = uVar1 == 0x5051;
      uVar3 = (uVar3 & 0xffff0000U) | (uint16_t)uVar1;
    LAB_026c84a7:
      uStack_14 = uStack_14 | 2;
    }
    if (bVar5 != 0xc)
      goto LAB_026c84c9;
  LAB_026c8528:
    uStack_14 = uStack_14 | 4;
  }
  if (bVar5 == 6)
  {
    uStack_14 = uStack_14 | 0x10;
    if (((((short)uVar3 == 0x8de || (short)uVar3 == 0x197a) || (short)uVar3 == 0x1a58) ||
         (short)uVar3 == 0xabe) ||
        ((short)uVar3 == 0x18a2))
      goto LAB_026c8570;
  LAB_026c8790:
    if (((((short)uVar3 == 0x338e ||
           ((short)uVar3 == 0x246a ||
            (((short)uVar3 == 0x2c9b ||
              ((short)uVar3 == 0x1ad7 ||
               (((short)uVar3 == 0x2ade || ((short)uVar3 == 0x2b43 || (short)uVar3 == 0x2b3e)) ||
                (short)uVar3 == 0x2aa8))) ||
             (short)uVar3 == 0x27cb))) ||
          (short)uVar3 == 0x4a18) ||
         (short)uVar3 == 0x4b50) ||
        ((short)uVar3 == 0x4705))
      goto LAB_026c858c;
  }
  else
  {
    if ((bVar5 == 5) ||
        (((((short)uVar3 == 0x8de || (short)uVar3 == 0x197a) || (short)uVar3 == 0x1a58) ||
          (short)uVar3 == 0xabe || ((short)uVar3 == 0x18a2))))
    {
    LAB_026c8570:
      uStack_14 = uStack_14 | 0x20;
    }
    if (((bVar5 & 0xef) != 2) && (bVar5 != 0x14))
      goto LAB_026c8790;
  LAB_026c858c:
    uStack_14 = uStack_14 | 0x40;
  }
  if (((short)uVar3 == 0x298d) || (bVar6))
  {
    uStack_14 = uStack_14 | 0x140;
    if ((short)uVar3 == 0x22c4)
      goto LAB_026c8726;
  LAB_026c85b3:
    if ((short)uVar3 == 0x903)
    {
      uStack_14 = uStack_14 | 0x200;
    }
  }
  else
  {
    if ((short)uVar3 != 0x22c4)
      goto LAB_026c85b3;
  LAB_026c8726:
    uStack_14 = uStack_14 | 0x80;
  }
  if (((((((((short)uVar3 == 0x2b93 || bVar5 == 0xb) || (short)uVar3 == 0x25f8) ||
           (short)uVar3 == 0xef2) ||
          (short)uVar3 == 0x31e0) ||
         (short)uVar3 == 0x2324) ||
        (short)uVar3 == 0x14ef) ||
       (short)uVar3 == 0x22f2) ||
      ((short)uVar3 == 0x4bf6))
  {
    uStack_14 = uStack_14 | 0x400;
  }
  if ((((bVar5 == 9) && ((cards_data[card].type & 4) == 0)) ||
       (((((((((((((((bVar5 == 4 || bVar5 == 10) || bVar5 == 0x1b) || (short)uVar3 == 0x2771) ||
                   (short)uVar3 == 0x20bd) ||
                  (short)uVar3 == 0x30f6) ||
                 (short)uVar3 == 0x230c) ||
                (short)uVar3 == 0x18a2) ||
               (short)uVar3 == 0x1977) ||
              (short)uVar3 == 0x3774) ||
             (short)uVar3 == 0x3de0) ||
            (short)uVar3 == 0x308a) ||
           (short)uVar3 == 0x4ea3) ||
          (short)uVar3 == 0x520b) ||
         (short)uVar3 == 0x59df) ||
        (bVar5 & 0xfb) == 0x11)) ||
      ((unsigned char)(bVar5 - 0x17) < 3))
  {
    uStack_14 = uStack_14 | 0x1000;
  }
  if (bVar5 == 0xe)
  {
    uStack_14 = uStack_14 | 0x2000;
    if ((short)uVar3 != 0x2e58)
    {
      return uStack_14;
    }
  }
  else if ((short)uVar3 != 0x2e58)
    goto LAB_026c86e1;
  uStack_14 = uStack_14 | 0x4000;
LAB_026c86e1:
  if (bVar5 == 0xf)
  {
    return uStack_14 | 0x8000;
  }
  if (bVar5 == 0x10)
  {
    uStack_14 = uStack_14 | 0x11000;
  }
  else
  {
    if (bVar5 == 0x13)
    {
      return uStack_14 | 0x80000;
    }
    if (bVar5 == 0x16)
    {
      return uStack_14 | 0x400000;
    }
    if (bVar5 == 0x1c)
    {
      uStack_14 = uStack_14 | 0x200000;
    }
  }
  return uStack_14;
}

static void shape_deadbox(void){
	deadbox_card[AI] = generate_reserved_token_by_id(AI, CARD_ID_DEADBOX);
	deadbox_card[HUMAN] = generate_reserved_token_by_id(HUMAN, CARD_ID_DEADBOX);
}

static void shape_rules_engine(int infos){
		int z;
		for (z = 0; z < 2; z++){
			rules_engine_card[z] = generate_reserved_token_by_id(z, CARD_ID_RULES_ENGINE); // rules engine
			card_instance_t *instance = get_card_instance(z, rules_engine_card[z]);
			instance->info_slot = infos;
			// Initialize everything from targets[5] up to end of targets array to 0
			memset(&instance->targets[5], 0, sizeof(target_t) * 14);
		}
}

static int check_decks_for_rules_engine(void){

	int i;
	int z;
	int result = 0;
	for(z=0; z < 2; z++){
		int *deck = deck_ptr[z];
		for(i=0; i < count_deck(z); i++ ){
			result |= check_card_for_rules_engine(deck[i]);
		}
		for (i = 0; i < active_cards_count[z]; ++i){
			result |= check_card_for_rules_engine(get_card_instance(z, i)->internal_card_id);
		}
	}
	return result;
}

void recalculate_rules_engine_and_deadbox(void)
{
  int flags = check_decks_for_rules_engine();

  if (flags & REF_DEADBOX)
	{
	  if (get_deadbox_card(HUMAN) == -1)
		shape_deadbox();
	}
  else
	{
	  int db = get_deadbox_card(HUMAN);
	  if (db != -1)
		kill_card(HUMAN, db, KILL_SACRIFICE);
	  db = get_deadbox_card(AI);
	  if (db != -1)
		kill_card(AI, db, KILL_SACRIFICE);
	  deadbox_card[HUMAN] = deadbox_card[AI] = -1;
	}

  if (flags & ~REF_DEADBOX)
	{
	  int re = get_rules_engine_card(HUMAN);
	  if (re == -1)
		shape_rules_engine(flags);
	  else
		{
		  get_card_instance(HUMAN, re)->info_slot = flags;
		  re = get_rules_engine_card(AI);
		  if (re != -1)
			get_card_instance(AI, re)->info_slot = flags;
		}
	}
  else
	{
	  int re = get_rules_engine_card(HUMAN);
	  if (re != -1)
		kill_card(HUMAN, re, KILL_SACRIFICE);
	  re = get_rules_engine_card(AI);
	  if (re != -1)
		kill_card(AI, re, KILL_SACRIFICE);
	  rules_engine_card[HUMAN] = rules_engine_card[AI] = -1;
	}
}

void update_rules_engine(int infos){
	if( infos < 1 ){
		return;
	}
	int re = get_rules_engine_card(HUMAN);
	if( re < 0 ){
		if( infos != 8192 ){
			shape_rules_engine(infos);
		}
	}
	else{
		int i;
		for(i=0; i<2; i++){
			int re_card = get_rules_engine_card(i);
			card_instance_t *instance = get_card_instance( i, re_card );
			instance->info_slot |= infos;
		}
	}
	if( infos & 8192 ){
		if( get_deadbox_card(HUMAN) == -1 ){
			shape_deadbox();
		}
	}
}

static void clear_temporary_traps(card_instance_t* instance);
static void check_trap_conditions(int player, int card, event_t event);

int get_rules_engine_infos(int player){
	int re = get_rules_engine_card(player);
	if( re > -1 ){
		card_instance_t *instance = get_card_instance(player,re);
		return instance->info_slot;
	}
	return 0;
}

int get_deadbox_card(int player){
	return deadbox_card[player];
}

void game_startup(void){

	memset(graveyard_source, -1, sizeof(graveyard_source));
	next_graveyard_source_id = 1;

	int re_infos = 0;

	re_infos = check_decks_for_rules_engine();

	hack_allow_sorcery_if_rules_engine_is_only_effect = 0;

	if( get_setting(SETTING_CHALLENGE_MODE) ){
		re_infos |= REF_LEYLINES; // upkeep triggers
		re_infos |= REF_GAUNTLET; // gauntlet settings
		set_challenge_round();
	}
	if( get_setting(SETTING_ARCHENEMY) ){	// Also set by the Archenemy card, if it's in a deck instead of playing via setting.
		re_infos |= REF_TRAP_CONDITIONS;
	}
	vanguard_check();

	if (get_setting(SETTING_RULES_ENGINE) == 0){
		return;
	}

	if( re_infos & REF_DEADBOX ){
		shape_deadbox();
	}

	if( re_infos > 0 && re_infos != REF_DEADBOX ){
		shape_rules_engine(re_infos);
	}

	return;
}

void set_starting_life_total(int player, int amount ){
	if( amount > 20 ){
		gain_life(player, amount - 20);
	}
	else if( amount < 20 ){
			life[player]-=(20-amount);
	}
	starting_life_total[player] = amount;
}

int get_starting_life_total(int player){
	return starting_life_total[player];
}

static void increase_trap_condition_impl(int player, int trap, int val, int add /*otherwise set*/)
{
  if (add && val == 0)
	return;

  int rec = get_rules_engine_card(player);
  if (rec < 0)
	return;

  card_instance_t* instance = get_card_instance(player, rec);

  // Strictly speaking, this is undefined behavior, but should work on everything Manalink can.
  rules_engine_globals_t* globals = (rules_engine_globals_t*)(&instance->targets[0].player);

  int bit, idx = trap & 0xFF;

  // Uses gcc case ranges extension.  Could be done (more verbosely) with a series of if/elses, of course.
  switch (trap)
	{
	  case TRAP_MIN_SHORTS_PER_TURN ... TRAP_MAX_SHORTS_PER_TURN:
		if (add)
		  val += globals->shorts_per_turn[idx];

		if (val > USHRT_MAX)
		  val = USHRT_MAX;
		else if (val < 0)
		  val = 0;

		globals->shorts_per_turn[idx] = val;
		break;

	  case TRAP_MIN_SHORTS_PERSISTENT ... TRAP_MAX_SHORTS_PERSISTENT:
		if (add)
		  val += globals->shorts_persistent[idx];

		if (val > USHRT_MAX)
		  val = USHRT_MAX;
		else if (val < 0)
		  val = 0;

		globals->shorts_persistent[idx] = val;
		break;

	  case TRAP_MIN_BYTES_PER_TURN ... TRAP_MAX_BYTES_PER_TURN:
		if (add)
		  val += globals->bytes_per_turn[idx];

		if (val > UCHAR_MAX)
		  val = UCHAR_MAX;
		else if (val < 0)
		  val = 0;

		globals->bytes_per_turn[idx] = val;
		break;

	  case TRAP_MIN_BYTES_PERSISTENT ... TRAP_MAX_BYTES_PERSISTENT:
		if (add)
		  val += globals->bytes_persistent[idx];

		if (val > UCHAR_MAX)
		  val = UCHAR_MAX;
		else if (val < 0)
		  val = 0;

		globals->bytes_persistent[idx] = val;
		break;

	  case TRAP_MIN_NYBBLES_PER_TURN ... TRAP_MAX_NYBBLES_PER_TURN:
		if (idx % 2 == 0)
		  {
			// low nybble
			idx /= 2;

			if (add)
			  val += globals->nybbles_per_turn[idx] & 0x0F;

			if (val > 0x0F)
			  val = 0x0F;
			else if (val < 0)
			  val = 0;

			globals->nybbles_per_turn[idx] = (globals->nybbles_per_turn[idx] & ~0x0F) | val;
		  }
		else
		  {
			// high nybble
			idx /= 2;

			if (add)
			  val += (globals->nybbles_per_turn[idx] & 0xF0) >> 4;

			if (val > 0x0F)
			  val = 0x0F;
			else if (val < 0)
			  val = 0;

			globals->nybbles_per_turn[idx] = (globals->nybbles_per_turn[idx] & ~0xF0) | (val << 4);
		  }
		break;

	  case TRAP_MIN_BITS_PER_TURN ... TRAP_MAX_BITS_PER_TURN:
		bit = idx % 8;
		idx /= 8;

		if (val > 0)
		  globals->bits_per_turn[idx] |= 1 << bit;
		else
		  globals->bits_per_turn[idx] &= ~(1 << bit);
		break;

	  case TRAP_MIN_BITS_PERSISTENT ... TRAP_MAX_BITS_PERSISTENT:
		bit = idx % 8;
		idx /= 8;

		if (val > 0)
		  globals->bits_persistent[idx] |= 1 << bit;
		else
		  globals->bits_persistent[idx] &= ~(1 << bit);
		break;
	}
}

int get_trap_condition(int player, int trap)
{
  int rec = get_rules_engine_card(player);
  if (rec < 0)
	return 0;

  card_instance_t* instance = get_card_instance(player, rec);

  // Strictly speaking, this is undefined behavior, but should work on everything Manalink can.
  rules_engine_globals_t* globals = (rules_engine_globals_t*)(&instance->targets[0].player);

  int bit, idx = trap & 0xFF;

  // Uses gcc case ranges extension.  Could be done (more verbosely) with a series of if/elses, of course.
  switch (trap)
	{
	  case TRAP_MIN_SHORTS_PER_TURN ... TRAP_MAX_SHORTS_PER_TURN:
		return globals->shorts_per_turn[idx];

	  case TRAP_MIN_SHORTS_PERSISTENT ... TRAP_MAX_SHORTS_PERSISTENT:
		return globals->shorts_persistent[idx];

	  case TRAP_MIN_BYTES_PER_TURN ... TRAP_MAX_BYTES_PER_TURN:
		return globals->bytes_per_turn[idx];

	  case TRAP_MIN_BYTES_PERSISTENT ... TRAP_MAX_BYTES_PERSISTENT:
		return globals->bytes_persistent[idx];

	  case TRAP_MIN_NYBBLES_PER_TURN ... TRAP_MAX_NYBBLES_PER_TURN:
		if (idx % 2 == 0)
		  {
			// low nybble
			idx /= 2;

			return globals->nybbles_per_turn[idx] & 0x0F;
		  }
		else
		  {
			// high nybble
			idx /= 2;

			return (globals->nybbles_per_turn[idx] & 0xF0) >> 4;
		  }

	  case TRAP_MIN_BITS_PER_TURN ... TRAP_MAX_BITS_PER_TURN:
		bit = idx % 8;
		idx /= 8;

		return (globals->bits_per_turn[idx] & (1 << bit)) ? 1 : 0;

	  case TRAP_MIN_BITS_PERSISTENT ... TRAP_MAX_BITS_PERSISTENT:
		bit = idx % 8;
		idx /= 8;

		return (globals->bits_persistent[idx] & (1 << bit)) ? 1 : 0;
	}

  return 0;
}

// Clear the per-turn trap conditions out at start of turn
static void clear_temporary_traps(card_instance_t* instance)
{
  // Strictly speaking, this is undefined behavior, but should work on everything Manalink can.
  rules_engine_globals_t* globals = (rules_engine_globals_t*)(&instance->targets[0].player);

  memset(globals->shorts_per_turn, 0, sizeof(globals->shorts_per_turn));
  memset(globals->bytes_per_turn, 0, sizeof(globals->bytes_per_turn));
  memset(globals->nybbles_per_turn, 0, sizeof(globals->nybbles_per_turn));
  memset(globals->bits_per_turn, 0, sizeof(globals->bits_per_turn));
}

static void check_trap_conditions(int player, int card, event_t event){

	if( player != HUMAN ){ return; }

	// Oran Rief the Vastwood + Novijen, Heart of Progress + Hixus, Prison Warden
	if( event == EVENT_CAST_SPELL && is_what(affected_card_controller, affected_card, TYPE_CREATURE) ){
		set_special_flags(affected_card_controller, affected_card, SF_JUST_CAME_INTO_PLAY);
	}

	// how many unblocked creatures
	int i, p = current_turn;
	if( event == EVENT_DECLARE_BLOCKERS ){
		for(i=0;i<active_cards_count[p];i++){
			if( is_unblocked(p, i) ){
				increase_trap_condition(current_turn, TRAP_UNBLOCKED_CREATURES, 1);
			}
		}
	}

	// was life gained?
	// handled in "gain_life" in "functions.c"

	// how many Zubera died this turn?
	if( event == EVENT_GRAVEYARD_FROM_PLAY && in_play(affected_card_controller, affected_card) ){
		int kill_code = get_card_instance(affected_card_controller, affected_card)->kill_code;
		if( kill_code >= KILL_DESTROY && kill_code < KILL_EXILE ){
			if( has_subtype(affected_card_controller, affected_card, SUBTYPE_ZUBERA) ){
				increase_trap_condition(affected_card_controller, TRAP_DEAD_ZUBERA_COUNT, 1);
			}
			if( get_id(affected_card_controller, affected_card) == CARD_ID_CLUE && kill_code == KILL_SACRIFICE ){
				increase_trap_condition(affected_card_controller, TRAP_CLUES_SACRIFICED, 1);
			}
		}
	}

	if(event == EVENT_DECLARE_BLOCKERS){
		int count = 0;
		while(count < active_cards_count[current_turn]){
			if( is_what(current_turn, count, TYPE_CREATURE) && is_attacking(current_turn, count)  ){
				set_trap_condition(current_turn, TRAP_RAID, 1);
				break;
			}
			count++;
		}
	}

#if 0
	/* Nothing currently checks TRAP_NUMBER_OF_ATTACKING_CREATURES (byte), and TRAP_NEMESIS_TRAP (bit) is inaccurate - the card's mana cost decreases if a white
	 * creature is currently attacking, not if one attacked this turn; and something may well have already destroyed it. */

	// how many creatures are attacking?
	if(event == EVENT_DECLARE_BLOCKERS){
		int count = 0;
		while(count < active_cards_count[current_turn]){
			if( is_what(current_turn, count, TYPE_CREATURE) && is_attacking(current_turn, count)  ){
				increase_trap_condition(current_turn, TRAP_NUMBER_OF_ATTACKING_CREATURES, 1);
				if( get_color(current_turn, count) & COLOR_TEST_WHITE ){
					increase_trap_condition(player, TRAP_NEMESIS_TRAP, 1);
					increase_trap_condition(1-player, TRAP_NEMESIS_TRAP, 1);
				}
			}
			count++;
		}
	}
	else{
		set_trap_condition(current_turn, TRAP_NUMBER_OF_ATTACKING_CREATURES, 0);
		set_trap_condition(1-current_turn, TRAP_NUMBER_OF_ATTACKING_CREATURES, 0);
	}
#endif

	// did an artifact / creature / land come into play?
	if(event == EVENT_END_TRIGGER && trigger_condition == TRIGGER_COMES_INTO_PLAY && reason_for_trigger_controller == affected_card_controller){
		if( is_what(trigger_cause_controller, trigger_cause, TYPE_ARTIFACT) ){
			increase_trap_condition(trigger_cause_controller, TRAP_ARTIFACTS_PLAYED, 1);
		}
		if( is_what(trigger_cause_controller, trigger_cause, TYPE_LAND) ){
			increase_trap_condition(trigger_cause_controller, TRAP_LANDS_PLAYED, 1);
		}
		if( is_what(trigger_cause_controller, trigger_cause, TYPE_CREATURE) ){
			increase_trap_condition(trigger_cause_controller, TRAP_CREATURES_PLAYED, 1);
			if( get_color(trigger_cause_controller, trigger_cause) & COLOR_TEST_GREEN ){
				increase_trap_condition(trigger_cause_controller, TRAP_PERMAFROST_TRAP, 1);
			}
		}
		if( is_planeswalker(trigger_cause_controller, trigger_cause) ){
			increase_trap_condition(trigger_cause_controller, TRAP_PLANESWALKERS_PLAYED, 1);
		}
	}

	if (event == EVENT_END_TRIGGER && trigger_condition == TRIGGER_DISCARD && reason_for_trigger_controller == affected_card_controller){
		increase_trap_condition(trigger_cause_controller, TRAP_DISCARDED_CARDS, 1);
	}

	if( trigger_condition == TRIGGER_LEAVE_PLAY && affect_me(player, card) &&
		reason_for_trigger_controller == affected_card_controller )
	{
		if( is_what(trigger_cause_controller, trigger_cause, TYPE_PERMANENT) ){
			set_trap_condition(trigger_cause_controller, TRAP_REVOLT, 1);
		}
	}

	if( event == EVENT_CLEANUP ){
		remove_special_flags(2, -1, SF_JUST_CAME_INTO_PLAY);
	}
}

void increase_trap_condition(int player, int trap, int amount)
{
  increase_trap_condition_impl(player, trap, amount, 1);
}

void set_trap_condition(int player, int trap, int value)
{
  increase_trap_condition_impl(player, trap, value, 0);
}

static void beginning_of_combat_effects(int player, int card, event_t event){
	card_instance_t *instance = get_card_instance(player, card);

	if (event == EVENT_PHASE_CHANGED && current_phase == PHASE_DECLARE_ATTACKERS && instance->targets[1].player != 1 && in_play(player, card)){
		instance->targets[1].player = 1;
		if( current_turn == player && ferocious(player, card) ){
			const int *grave = get_grave(player);
			int count = count_graveyard(player)-1;
			while( count > -1 ){
					if( cards_data[grave[count]].id == CARD_ID_FLAMEWAKE_PHOENIX && has_mana(player, COLOR_RED, 1) ){
						if( do_dialog(player, player, card, -1, -1, " Return Flamewake Phoenix to play\n Pass", 0) == 0 ){
							charge_mana(player, COLOR_RED, 1);
							if( spell_fizzled != 1 ){
								reanimate_permanent(player, card, player, count, REANIMATE_DEFAULT);
							}
						}
					}
					count--;
			}
		}
	}

	if(event == EVENT_PHASE_CHANGED && current_phase != PHASE_DECLARE_ATTACKERS){
		instance->targets[1].player = 0;
	}
}

int card_rules_engine(int player, int card, event_t event ){
	card_instance_t *instance = get_card_instance(player,card);

	if( event == EVENT_CLEANUP ){
		int i;
		for(i=0; i<2; i++){
			remove_special_flags2(i, -1, SF2_HAS_DAMAGED_PLAYER0);
			remove_special_flags2(i, -1, SF2_HAS_DAMAGED_PLAYER1);
		}
	}

	// (I'd think this should be during EVENT_CLEANUP, but it's harmless to preserve the original behavior and delay until EVENT_CAN_SKIP_TURN of next turn)
	if (event == EVENT_CAN_SKIP_TURN){
		clear_temporary_traps(instance);
	}

	debug_settings(player, card, event);
	if ((instance->info_slot & REF_LEYLINES) || force_init_upkeep){
		initial_upkeep_triggers(player, card, event);            // 1
		keep_turn_count(player, card, event);                    // same as above
	}
	if( instance->info_slot & REF_TRAP_CONDITIONS ){
		check_trap_conditions(player, card, event);                   // 2
	}
	if( instance->info_slot & REF_STORM_COUNT ){
		keep_storm_count(player, card, event);                   // 4
	}
	if( instance->info_slot & REF_GAUNTLET ){
		gauntlet_settings(player, card, event);                  // 8
		//apply_challenge_10_to_1(player, card, event);
	}
	if( instance->info_slot & REF_DREDGE ){
		resolve_dredge_triggers(player, card, event);            // 16
	}
	if( instance->info_slot & REF_UPKEEP_ABILITY_IN_GRAVE ){
		resolve_graveyard_upkeep_triggers(player, card, event);  // 32
	}
	if( instance->info_slot & REF_CONT_AND_TRIG_ABILITY_IN_GRAVE ){
		static_graveyard_abilities(player, card, event);         // 64
	}
	if( instance->info_slot & REF_BRIDGE_FROM_BELOW ){
		bridge_from_below(player, card, event);                  // 128
	}
	// 1<<8 unused
	if( instance->info_slot & REF_KROVIKAN_HORROR ){
		krovikan_horror(player, card, event);                    // 512
	}

	#if 0
	if( instance->info_slot & REF_DISCARD_TRIGGER_MADNESS ){
		discard_triggers(player, card, event);                   // 1024
	}
	#endif

	// (1<<13) --> Deabox
	if( instance->info_slot & REF_VENGEFUL_PHARAOH ){
		vengeful_pharaoh_effect(player, card, event);                 // 16384
	}
	if( instance->info_slot & REF_MIRACLES ){
		miracle(player, card, event);
	}
	// (1<<16) --> Forecast
	// (1<<17) --> Scavenge
	// (1<<18) --> Onslaught's auras auto-returning from grave
	if( instance->info_slot & REF_CONSPIRE ){
		conspire(player, card, event);
	}
	// (1<<20) --> Recover
	if( instance->info_slot & REF_BEGINNING_OF_COMBAT_EFFECTS ){
		beginning_of_combat_effects(player, card, event);
	}
	if( instance->info_slot & REF_ACTIVATED_ABILITIES_HAND_GRAVE ){
		return activate_cards_in_hand(player, card, event);           // 4096
	}

	return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x26c7f10; original functions/rules_engine.c:1534. */
int get_aftermath(int player, int card)
{
  int result;
  result = check_special_flags(player, card, 0x200000);
  return (unsigned int)(result != 0);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x26c5ff0; original functions/rules_engine.c:439. */
int companion_effect(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = can_sorcery_be_played(player, EVENT_CAN_ACTIVATE);
    if (result != 0)
    {
      result = has_mana(player, 0, 3);
      return (unsigned int)(result != 0);
    }
  }
  else if (((event == EVENT_ACTIVATE) &&
            (result = charge_mana(player, COLOR_COLORLESS, 3), result != 0)) &&
           (spell_fizzled != 1))
  {
    instance = get_card_instance(player, card);
    result = get_internal_card_id_from_csv_id((unsigned int)instance->display_pic_csv_id);
    add_card_to_hand(player, result);
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}


/* October 2022 storm backing-state layout checks. */
#ifndef SHANDALAR
STATIC_ASSERT(sizeof(rules_engine_card) == 8, storm_native_card_slots);
STATIC_ASSERT(sizeof(rules_engine_globals_t) == 152, storm_native_globals_bytes);
STATIC_ASSERT(offsetof(rules_engine_globals_t, targets) == 0, storm_native_offset_targets);
STATIC_ASSERT(offsetof(rules_engine_globals_t, shorts_per_turn) == 40, storm_native_offset_shorts_per_turn);
STATIC_ASSERT(offsetof(rules_engine_globals_t, unassigned1) == 48, storm_native_offset_unassigned1);
STATIC_ASSERT(offsetof(rules_engine_globals_t, shorts_persistent) == 68, storm_native_offset_shorts_persistent);
STATIC_ASSERT(offsetof(rules_engine_globals_t, unassigned2) == 70, storm_native_offset_unassigned2);
STATIC_ASSERT(offsetof(rules_engine_globals_t, bytes_per_turn) == 86, storm_native_offset_bytes_per_turn);
STATIC_ASSERT(offsetof(rules_engine_globals_t, unassigned3) == 102, storm_native_offset_unassigned3);
STATIC_ASSERT(offsetof(rules_engine_globals_t, bytes_persistent) == 112, storm_native_offset_bytes_persistent);
STATIC_ASSERT(offsetof(rules_engine_globals_t, unassigned4) == 114, storm_native_offset_unassigned4);
STATIC_ASSERT(offsetof(rules_engine_globals_t, nybbles_per_turn) == 120, storm_native_offset_nybbles_per_turn);
STATIC_ASSERT(offsetof(rules_engine_globals_t, unassigned5) == 128, storm_native_offset_unassigned5);
STATIC_ASSERT(offsetof(rules_engine_globals_t, bits_per_turn) == 133, storm_native_offset_bits_per_turn);
STATIC_ASSERT(offsetof(rules_engine_globals_t, unassigned6) == 137, storm_native_offset_unassigned6);
STATIC_ASSERT(offsetof(rules_engine_globals_t, bits_persistent) == 140, storm_native_offset_bits_persistent);
STATIC_ASSERT(offsetof(rules_engine_globals_t, unassigned7) == 141, storm_native_offset_unassigned7);
STATIC_ASSERT(offsetof(rules_engine_globals_t, counters) == 144, storm_native_offset_counters);
STATIC_ASSERT(COUNTER_BLOOD == 19, storm_native_COUNTER_BLOOD);
STATIC_ASSERT(COUNTER_DEVOTION == 32, storm_native_COUNTER_DEVOTION);
STATIC_ASSERT(COUNTER_VORTEX == 38, storm_native_COUNTER_VORTEX);
STATIC_ASSERT(COUNTER_CHARGE == 123, storm_native_COUNTER_CHARGE);
STATIC_ASSERT(sizeof(storm_mode_t) == 4, storm_native_mode_bytes);
STATIC_ASSERT(STORM_CREATURE == 64, storm_canonical_creature_mode);
#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26c7f70; original functions/rules_engine.c:1599. */
void increment_storm_count(int player, int mode)
{
  int card;

  card = rules_engine_card[player];
  if (card != -1)
  {
    hack_silent_counters = hack_silent_counters + 1;
    add_counters(player, card, player, card, COUNTER_VORTEX, 1);
    increase_trap_condition_impl(player, 0x301, 1, 1);
    if ((mode & STORM_CREATURE) != 0)
    {
      add_counters(player, card, player, card, COUNTER_BLOOD, 1);
    }
    if ((mode & STORM_ENCHANTMENT) != 0)
    {
      add_counters(player, card, player, card, COUNTER_DEVOTION, 1);
    }
    if ((mode & STORM_INSTANT_SORCERY) != 0)
    {
      add_counters(player, card, player, card, COUNTER_CHARGE, 1);
    }
    hack_silent_counters = hack_silent_counters + -1;
    if ((mode & 2U) != 0)
    {
      increase_trap_condition_impl(player, 0x403, 1, 1);
    }
    if ((mode & 4U) != 0)
    {
      increase_trap_condition_impl(player, 0x404, 1, 1);
    }
    if ((mode & 8U) != 0)
    {
      increase_trap_condition_impl(player, 0x405, 1, 1);
    }
    if ((mode & 0x10U) != 0)
    {
      increase_trap_condition_impl(player, 0x406, 1, 1);
    }
    if ((mode & 0x20U) != 0)
    {
      increase_trap_condition_impl(player, 0x407, 1, 1);
      return;
    }
  }
  return;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26c81a0; original functions/rules_engine.c:1735. */
int get_specific_storm_count(int player)
{
  int result;

  if (rules_engine_card[player] != -1)
  {
    result = count_counters(player, rules_engine_card[player], COUNTER_VORTEX);
    return result;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26c81e0; original functions/rules_engine.c:1744. */
int get_storm_count(void)
{
  int iVar1;
  int iVar2;

  if (rules_engine_card[1] == -1)
  {
    iVar1 = 0;
  }
  else
  {
    iVar1 = count_counters(1, rules_engine_card[1], COUNTER_VORTEX);
  }
  if (rules_engine_card[0] != -1)
  {
    iVar2 = count_counters(0, rules_engine_card[0], COUNTER_VORTEX);
    iVar1 = iVar1 + iVar2;
  }
  return iVar1;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26c8240; original functions/rules_engine.c:1749. */
int get_stormcreature_count(int k)
{
  int result;

  if (rules_engine_card[k] != -1)
  {
    result = count_counters(k, rules_engine_card[k], COUNTER_BLOOD);
    return result;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26c8280; original functions/rules_engine.c:1757. */
int get_storminstantsorcery_count(int k)
{
  int result;

  if (rules_engine_card[k] != -1)
  {
    result = count_counters(k, rules_engine_card[k], COUNTER_CHARGE);
    return result;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26c82c0; original functions/rules_engine.c:1765. */
int get_stormenchantment_count(int k)
{
  int result;

  if (rules_engine_card[k] != -1)
  {
    result = count_counters(k, rules_engine_card[k], COUNTER_DEVOTION);
    return result;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26c90c0; original functions/rules_engine.c:1773. */
int get_stormcolor_count(int player, color_t clr)
{
  int result;
  if (4U < (unsigned int)(clr - COLOR_BLACK))
  {
    return 0;
  }
  result = get_trap_condition(player, clr + 0x402);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022 inline reset_storm_count: owner 0x26c9140; DIE 0x68be08e. */

/* End ManalinkEh 2022 inline reset_storm_count. */
#endif


#ifndef SHANDALAR
/* ManalinkEh 2022 inline keep_storm_count: owner 0x26c9140; DIE 0x68be050. */

/* End ManalinkEh 2022 inline keep_storm_count. */
#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26c7890; original functions/rules_engine.c:84. */
int limit_allowed_in_deck(int csvid)
{
  static const int unlimited_list[17] = {91,   126,   164,   188,   239,   2100,  2101, 2102, 2103,
                                         2104, 20819, 15719, 22636, 19685, 18489, 7107, 13335};
  for (unsigned int i = 0; i < 17; ++i)
    if (csvid == unlimited_list[i])
      return 5000;
  return csvid == 0x5100 ? 7 : 4;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26c78f0; original functions/rules_engine.c:365. */
int random_rogue(void)
{
  typedef struct
  {
    char name[200];
    char quote[300];
    char imgname[268];
    int difficulty;
    char padding[4004];
  } rogue_t;
  rogue_t *rogues = (rogue_t *)0x62c3c0;
  int num_rogues = EXE_DWORD(0x7370b8);
  int ok_rogues[num_rogues];
  read_and_adjust_settings();
  int num_ok_rogues = 0;
  for (int i = 0; i < num_rogues; ++i)
    if (rogues[i].name[0] && rogues[i].imgname[0] && rogues[i].difficulty == opponent_skill)
      ok_rogues[num_ok_rogues++] = i;
  if (!num_ok_rogues)
  {
    for (int i = 0; i < num_rogues; ++i)
      if (rogues[i].name[0] && rogues[i].imgname[0])
        ok_rogues[num_ok_rogues++] = i;
  }
  if (num_ok_rogues)
    return ok_rogues[internal_rand(num_ok_rogues)];
  popup("Fatal error", "No valid opponents defined in Rogues.csv.");
  abort();
}

#endif
