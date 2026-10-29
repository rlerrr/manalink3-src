#include <limits.h>

#include "manalink.h"
static void add_or_remove_counters_impl(int player,int card,int t_player,int t_card,counter_t type,int num);

/* Canonical recovered helper declarations. */
static int get_updated_counters_number_as_cost(int player, int card, counter_t type, int number);
int get_updated_counters_number(int player, int card, counter_t type, int number);
static void add_or_remove_counters_impl(int player, int card, int t_player, int t_card,
                                                    counter_t type, int num);
void add_counters(int player, int card, int t_player, int t_card, counter_t type,
                              int num);
void remove_counters(int player, int card, int t_player, int t_card, counter_t type,
                                 int num);
void remove_all_counters(int player, int card, int t_player, int t_card, counter_t type);
void copy_counters(int player, int card, int t_player, int t_card, int src_player,
                               int src_card, int raw);
void add_counters_predoubled(int player, int card, int t_player, int t_card,
                                         counter_t type, int num);
void move_counters(int player, int card, int t_player, int t_card, int src_player,
                               int src_card, counter_t counter_type, int number);

int hack_silent_counters = 0;
int counters_added = 0;	// backed up and saved

/* Relevant data in card_instance_t:
 * special_counters: number of "special" counters (preferred type determined by card csvid; stored in special_counter_type)
 * counters2: drawn in upper right of lower section.  Originally -0/-2 counters for Spirit Shackle; now arbitrary.
 * counters3: drawn in upper center of lower section.  Originally -1/-1 counters for Unstable Mutation; now arbitrary.
 * counters4: drawn in upper left of lower section.  Originally -0/-1 counters for Orcish Catapult; now arbitrary.
 * counters: drawn lower left of lower section.  Always +1/+1 counters (originally using text from Dwarven Weaponsmith).
 * counters5: drawn in lower right of lower section.  Originally +1/+1 counters from Ashnod's Transmogrant; now arbitray.
 * counters_m1m1: drawn in lower left of lower section with +1/+1 counters.  Originally unused.  Always -1/-1 counters.
 * targets[18].player: byte0: type of counters2.  byte1: type of counters3.  byte2: type of counters4.  byte3: type of counters5.
 * targets[18].card: byte0: non-displayed counters slot A.  byte1: non-displayed counters slot B.  byte2: type of byte0.  byte3: type of byte1.
 */

static void play_counters_sound(wav_t wav){
	if (hack_silent_counters <= 0){
		play_sound_effect(wav);
	}
}

int get_counter_type_by_id(int csvid)
{
  switch (csvid)
  {
  case CARD_ID_ARNJLOTS_ASCENT:
  case CARD_ID_BALDUVIAN_SHAMAN:
  case CARD_ID_BLIZZARD:
  case CARD_ID_BRAND_OF_ILL_OMEN:
  case CARD_ID_BREATH_OF_DREAMS:
  case CARD_ID_COLD_SNAP:
  case CARD_ID_ENERGY_STORM:
  case CARD_ID_FLOW_OF_MAGGOTS:
  case CARD_ID_FYNDHORN_POLLEN:
  case CARD_ID_GLACIAL_CHASM:
  case CARD_ID_HALLS_OF_MIST:
  case CARD_ID_ILLUSIONARY_FORCES:
  case CARD_ID_ILLUSIONARY_PRESENCE:
  case CARD_ID_ILLUSIONARY_TERRAIN:
  case CARD_ID_ILLUSIONARY_WALL:
  case CARD_ID_ILLUSIONS_OF_GRANDEUR:
  case CARD_ID_INFERNAL_DARKNESS:
  case CARD_ID_MADDENING_WIND:
  case CARD_ID_MESMERIC_TRANCE:
  case CARD_ID_MUSICIAN:
  case CARD_ID_MYSTIC_MIGHT:
  case CARD_ID_MYSTIC_REMORA:
  case CARD_ID_NAKED_SINGULARITY:
  case CARD_ID_POLAR_KRAKEN:
  case CARD_ID_REALITY_TWIST:
  case CARD_ID_RITUAL_OF_SUBDUAL:
  case CARD_ID_SNOWFALL:
  case CARD_ID_SOLDEVI_SIMULACRUM:
  case CARD_ID_DYSTOPIA:
  case CARD_ID_ROYAL_DECREE:
  case CARD_ID_SUSTAINING_SPIRIT:
  case CARD_ID_THOUGHT_LASH:
  case CARD_ID_TIDAL_CONTROL:
  case CARD_ID_TORNADO:
  case CARD_ID_VARCHILDS_WAR_RIDERS:
  case CARD_ID_YAVIMAYA_ANTS:
  case CARD_ID_DECOMPOSITION:
  case CARD_ID_MALIGNANT_GROWTH:
  case CARD_ID_MIND_HARNESS:
  case CARD_ID_PRISMATIC_CIRCLE:
  case CARD_ID_TOMBSTONE_STAIRWELL:
  case CARD_ID_CORROSION:
  case CARD_ID_ELEPHANT_GRASS:
  case CARD_ID_FIRESTORM_HELLKITE:
  case CARD_ID_HEAT_WAVE:
  case CARD_ID_JUJU_BUBBLE:
  case CARD_ID_ABOROTH:
  case CARD_ID_ANCESTRAL_KNOWLEDGE:
  case CARD_ID_ARCTIC_WOLVES:
  case CARD_ID_GALLOWBRAID:
  case CARD_ID_HEART_OF_BOGARDAN:
  case CARD_ID_INNER_SANCTUM:
  case CARD_ID_MANA_CHAINS:
  case CARD_ID_MORINFEN:
  case CARD_ID_MWONVULI_OOZE:
  case CARD_ID_PSYCHIC_VORTEX:
  case CARD_ID_REVERED_UNICORN:
  case CARD_ID_UKTABI_EFREET:
  case CARD_ID_VOLUNTEER_RESERVES:
  case CARD_ID_WAVE_OF_TERROR:
  case CARD_ID_ARCTIC_NISHOBA:
  case CARD_ID_BALDUVIAN_FALLEN:
  case CARD_ID_BRAID_OF_FIRE:
  case CARD_ID_COVER_OF_WINTER:
  case CARD_ID_EARTHEN_GOO:
  case CARD_ID_FREYALISES_RADIANCE:
  case CARD_ID_GLACIAL_PLATING:
  case CARD_ID_HERALD_OF_LESHRAC:
  case CARD_ID_HIBERNATIONS_END:
  case CARD_ID_JOTUN_GRUNT:
  case CARD_ID_JOTUN_OWL_KEEPER:
  case CARD_ID_KARPLUSAN_MINOTAUR:
  case CARD_ID_KJELDORAN_JAVELINEER:
  case CARD_ID_KROVIKAN_WHISPERS:
  case CARD_ID_MAGMATIC_CORE:
  case CARD_ID_PHOBIAN_PHANTASM:
  case CARD_ID_PHYREXIAN_ETCHINGS:
  case CARD_ID_PHYREXIAN_SOULGORGER:
  case CARD_ID_RONOM_HULK:
  case CARD_ID_SHELTERING_ANCIENT:
  case CARD_ID_SURVIVOR_OF_THE_UNSEEN:
  case CARD_ID_VEXING_SPHINX:
  case CARD_ID_WALL_OF_SHARDS:
  case CARD_ID_SPLINTER_TOKEN:
    return COUNTER_AGE;
  case CARD_ID_HANKYU:
    return COUNTER_AIM;
  case CARD_ID_ARCHERY_TRAINING:
    return COUNTER_ARROW;
  case CARD_ID_SERRATED_ARROWS:
    return COUNTER_ARROWHEAD;
  case CARD_ID_FIVE_ALARM_FIRE:
    return COUNTER_BLAZE;
  case CARD_ID_BLOODLETTER_QUILL:
  case CARD_ID_RAKDOS_RITEKNIFE:
    return COUNTER_BLOOD;
  case CARD_ID_BOUNTY_HUNTER:
    return COUNTER_BOUNTY;
  case CARD_ID_GWAFA_HAZID_PROFITEER:
    return COUNTER_BRIBERY;
  case CARD_ID_OSAI_VULTURES:
    return COUNTER_CARRION;
  case CARD_ID_SCAVENGING_GHOUL:
    return COUNTER_CORPSE;
  case CARD_ID_ICATIAN_MONEYCHANGER:
    return COUNTER_CREDIT;
  case CARD_ID_DELIFS_CUBE:
    return COUNTER_CUBE;
  case CARD_ID_TRADE_CARAVAN:
    return COUNTER_CURRENCY;
  case CARD_ID_BOGARDAN_PHOENIX:
  case CARD_ID_DEADBOX:
    return COUNTER_DEATH;
  case CARD_ID_ERTAIS_MEDDLING:
  case CARD_ID_DELAYING_SHIELD:
    return COUNTER_DELAY;
  case CARD_ID_LAND_CAP:
  case CARD_ID_LAVA_TUBES:
  case CARD_ID_RIVER_DELTA:
  case CARD_ID_TIMBERLINE_RIDGE:
  case CARD_ID_VELDT:
  case CARD_ID_HICKORY_WOODLOT:
  case CARD_ID_PEAT_BOG:
  case CARD_ID_REMOTE_FARM:
  case CARD_ID_SANDSTONE_NEEDLE:
  case CARD_ID_SAPRAZZAN_SKERRY:
  case CARD_ID_DECREE_OF_SILENCE:
  case CARD_ID_FORCE_BUBBLE:
    return COUNTER_DEPLETION;
  case CARD_ID_DESCENT_INTO_MADNESS:
    return COUNTER_DESPAIR;
  case CARD_ID_BLOODTHIRSTY_OGRE:
  case CARD_ID_PIOUS_KITSUNE:
    return COUNTER_DEVOTION;
  case CARD_ID_MYOJIN_OF_CLEANSING_FIRE:
  case CARD_ID_MYOJIN_OF_INFINITE_RAGE:
  case CARD_ID_MYOJIN_OF_LIFES_WEB:
  case CARD_ID_MYOJIN_OF_NIGHTS_REACH:
  case CARD_ID_MYOJIN_OF_SEEING_WINDS:
    return COUNTER_DIVINITY;
  case CARD_ID_ARMAGEDDON_CLOCK:
  case CARD_ID_YOUR_INESCAPABLE_DOOM:
  case CARD_ID_IMMINENT_DOOM:
    return COUNTER_DOOM;
  case CARD_ID_RASPUTIN_DREAMWEAVER:
    return COUNTER_DREAM;
  case CARD_ID_SOUL_ECHO:
    return COUNTER_ECHO;
  case CARD_ID_ESSENCE_BOTTLE:
    return COUNTER_ELIXIR;
  case CARD_ID_ENERGY_VORTEX:
  case CARD_ID_ELDER_DRAGON_HIGHLANDER:
  case CARD_ID_PLANECHASE:
  case CARD_ID_SPAT:
  case CARD_ID_RULES_ENGINE:
    return COUNTER_VORTEX;
  case CARD_ID_MAGOSI_THE_WATERVEIL:
    return COUNTER_EON;
  case CARD_ID_JAR_OF_EYEBALLS:
    return COUNTER_EYEBALL;
  case CARD_ID_ANCIENT_HYDRA:
  case CARD_ID_BLASTODERM:
  case CARD_ID_CLOUDSKATE:
  case CARD_ID_DEFENDER_EN_VEC:
  case CARD_ID_JOLTING_MERFOLK:
  case CARD_ID_PARALLAX_DEMENTIA:
  case CARD_ID_PARALLAX_NEXUS:
  case CARD_ID_PARALLAX_TIDE:
  case CARD_ID_PARALLAX_WAVE:
  case CARD_ID_PHYREXIAN_PROWLER:
  case CARD_ID_REJUVENATION_CHAMBER:
  case CARD_ID_RUSTING_GOLEM:
  case CARD_ID_SAPROLING_BURST:
  case CARD_ID_SKYSHROUD_BEHEMOTH:
  case CARD_ID_SKYSHROUD_RIDGEBACK:
  case CARD_ID_TANGLE_WIRE:
  case CARD_ID_WOODRIPPER:
    return COUNTER_FADE;
  case CARD_ID_KANGEE_AERIE_KEEPER:
  case CARD_ID_SOULCATCHERS_AERIE:
  case CARD_ID_AVEN_MIMEOMANCER:
    return COUNTER_FEATHER;
  case CARD_ID_AZORS_ELOCUTORS:
    return COUNTER_FILIBUSTER;
  case CARD_ID_NAAR_ISLE:
    return COUNTER_FLAME;
  case CARD_ID_BOUNTY_OF_THE_LUXA:
    return COUNTER_FLOOD;
  case CARD_ID_SPOROGENESIS:
    return COUNTER_FUNGUS;
  case CARD_ID_TEMP_OF_THE_DAMNED:
    return COUNTER_FUNK;
  case CARD_ID_GOBLIN_BOMB:
  case CARD_ID_INCENDIARY:
  case CARD_ID_POWDER_KEG:
    return COUNTER_FUSE;
  case CARD_ID_DRAGONS_HOARD:
    return COUNTER_GOLD;
  case CARD_ID_MOMENTUM:
  case CARD_ID_SIMIC_ASCENDANCY:
    return COUNTER_GROWTH;
  case CARD_ID_TRIASSIC_EGG:
  case CARD_ID_LUDEVICS_TEST_SUBJECT:
    return COUNTER_HATCHLING;
  case CARD_ID_FYLGJA:
  case CARD_ID_URSINE_FYLGJA:
    return COUNTER_HEALING;
  case CARD_ID_HOOFPRINTS_OF_THE_STAG:
    return COUNTER_HOOFPRINT;
  case CARD_ID_TEMPORAL_DISTORTION:
    return COUNTER_HOURGLASS;
  case CARD_ID_FASTING:
    return COUNTER_HUNGER;
  case CARD_ID_NECROPOLIS_OF_AZAR:
    return COUNTER_HUSK;
  case CARD_ID_ICEBERG:
  case CARD_ID_DARK_DEPTHS:
  case CARD_ID_WOOLLY_RAZORBACK:
    return COUNTER_ICE;
  case CARD_ID_DISEASED_VERMIN:
  case CARD_ID_FESTERING_WOUND:
    return COUNTER_INFECTION;
  case CARD_ID_DIVINE_INTERVENTION:
    return COUNTER_INTERVENTION;
  case CARD_ID_ICATIAN_JAVELINEERS:
    return COUNTER_JAVELIN;
  case CARD_ID_BAKU_ALTAR:
  case CARD_ID_BLADEMANE_BAKU:
  case CARD_ID_BUDOKA_PUPIL:
  case CARD_ID_ICHIGA_WHO_TOPPLES_OAKS:
  case CARD_ID_CALLOW_JUSHI:
  case CARD_ID_JARAKU_THE_INTERLOPER:
  case CARD_ID_CUNNING_BANDIT:
  case CARD_ID_AZAMUKI_TREACHERY_INCARNATE:
  case CARD_ID_FAITHFUL_SQUIRE:
  case CARD_ID_KAISO_MEMORY_OF_LOYALTY:
  case CARD_ID_HIRED_MUSCLE:
  case CARD_ID_SCARMAKER:
  case CARD_ID_PETALMANE_BAKU:
  case CARD_ID_QUILLMANE_BAKU:
  case CARD_ID_SKULLMANE_BAKU:
  case CARD_ID_WAXMANE_BAKU:
    return COUNTER_KI;
  case CARD_ID_BEASTBREAKER_OF_BALA_GED:
  case CARD_ID_BRIMSTONE_MAGE:
  case CARD_ID_CARAVAN_ESCORT:
  case CARD_ID_CORALHELM_COMMANDER:
  case CARD_ID_ECHO_MAGE:
  case CARD_ID_ENCLAVE_CRYPTOLOGIST:
  case CARD_ID_GUUL_DRAZ_ASSASSIN:
  case CARD_ID_HADA_SPY_PATROL:
  case CARD_ID_HALIMAR_WAVEWATCH:
  case CARD_ID_HEDRON_FIELD_PURISTS:
  case CARD_ID_IKIRAL_OUTRIDER:
  case CARD_ID_JORAGA_TREESPEAKER:
  case CARD_ID_KABIRA_VINDICATOR:
  case CARD_ID_KARGAN_DRAGONLORD:
  case CARD_ID_KAZANDU_TUSKCALLER:
  case CARD_ID_KNIGHT_OF_CLIFFHAVEN:
  case CARD_ID_LIGHTHOUSE_CHRONOLOGIST:
  case CARD_ID_LORD_OF_SHATTERSKULL_PASS:
  case CARD_ID_NIRKANA_CUTTHROAT:
  case CARD_ID_NULL_CHAMPION:
  case CARD_ID_SKYWATCHER_ADEPT:
  case CARD_ID_STUDENT_OF_WARFARE:
  case CARD_ID_TRANSCENDENT_MASTER:
  case CARD_ID_ZULAPORT_ENFORCER:
    return COUNTER_LEVEL;
  case CARD_ID_MIND_UNBOUND:
  case CARD_ID_SCROLL_OF_THE_MASTERS:
  case CARD_ID_MYTH_REALIZED:
  case CARD_ID_CHAINERS_TORMENT:
  case CARD_ID_FALL_OF_THE_THRAN:
  case CARD_ID_HISTORY_OF_BENALIA:
  case CARD_ID_PHYREXIAN_SCRIPTURES:
  case CARD_ID_RITE_OF_BELZENLOK:
  case CARD_ID_SONG_OF_FREYALISE:
  case CARD_ID_THE_ANTIQUITIES_WAR:
  case CARD_ID_THE_ELDEST_REBORN:
  case CARD_ID_THE_FIRST_ERUPTION:
  case CARD_ID_THE_FLAME_OF_KELD:
  case CARD_ID_THE_MENDING_OF_DOMINARIA:
  case CARD_ID_THE_MIRARI_CONJECTURE:
  case CARD_ID_TIME_OF_ICE:
  case CARD_ID_TRIUMPH_OF_GERRARD:
  case 0x5208:
  case 0x520a:
  case 0x5239:
  case 0x5244:
  case 0x52a6:
  case 0x52a7:
  case 0x52a8:
  case 0x52a9:
  case 0x52aa:
  case 0x52b5:
    return COUNTER_LORE;
  case CARD_ID_AJANI_GOLDMANE:
  case CARD_ID_CHANDRA_NALAAR:
  case CARD_ID_GARRUK_WILDSPEAKER:
  case CARD_ID_JACE_BELEREN:
  case CARD_ID_LILIANA_VESS:
  case CARD_ID_AJANI_VENGEANT:
  case CARD_ID_ELSPETH_KNIGHT_ERRANT:
  case CARD_ID_SARKHAN_VOL:
  case CARD_ID_TEZZERET_THE_SEEKER:
  case CARD_ID_NICOL_BOLAS_PLANESWALKER:
  case CARD_ID_CHANDRA_ABLAZE:
  case CARD_ID_NISSA_REVANE:
  case CARD_ID_SORIN_MARKOV:
  case CARD_ID_JACE_THE_MIND_SCULPTOR:
  case CARD_ID_GIDEON_JURA:
  case CARD_ID_GIDEON_JURA_ANIMATED:
  case CARD_ID_SARKHAN_THE_MAD:
  case CARD_ID_ELSPETH_TIREL:
  case CARD_ID_KOTH_OF_THE_HAMMER:
  case CARD_ID_VENSER_THE_SOJOURNER:
  case CARD_ID_TEZZERET_AGENT_OF_BOLAS:
  case CARD_ID_KARN_LIBERATED:
  case CARD_ID_CHANDRA_THE_FIREBRAND:
  case CARD_ID_GARRUK_PRIMAL_HUNTER:
  case CARD_ID_JACE_MEMORY_ADEPT:
  case CARD_ID_GARRUK_RELENTLESS:
  case CARD_ID_GARRUK_THE_VEIL_CURSED:
  case CARD_ID_LILIANA_OF_THE_VEIL:
  case CARD_ID_SORIN_LORD_OF_INNISTRAD:
  case CARD_ID_TAMIYO_THE_MOON_SAGE:
  case CARD_ID_TIBALT_THE_FIEND_BLOODED:
  case CARD_ID_AJANI_CALLER_OF_THE_PRIDE:
  case CARD_ID_LILIANA_OF_THE_DARK_REALMS:
  case CARD_ID_JACE_ARCHITECT_OF_THOUGHT:
  case CARD_ID_VRASKA_THE_UNSEEN:
  case CARD_ID_DOMRI_RADE:
  case CARD_ID_GIDEON_CHAMPION_OF_JUSTICE:
  case CARD_ID_GIDEON_CHAMPION_ANIMATED:
  case CARD_ID_RAL_ZAREK:
  case CARD_ID_CHANDRA_PYROMASTER:
  case CARD_ID_GARRUK_CALLER_OF_BEASTS:
  case CARD_ID_ASHIOK_NIGHTMARE_WEAVER:
  case CARD_ID_ELSPETH_SUNS_CHAMPION:
  case CARD_ID_XENAGOS_THE_REVELER:
  case CARD_ID_KIORA_THE_CRASHING_WAVE:
  case CARD_ID_AJANI_MENTOR_OF_HEROES:
  case CARD_ID_DACK_FAYDEN:
  case CARD_ID_AJANI_STEADFAST:
  case CARD_ID_GARRUK_APEX_PREDATOR:
  case CARD_ID_JACE_THE_LIVING_GUILDPACT:
  case CARD_ID_NISSA_WORLDWAKER:
  case CARD_ID_SARKHAN_THE_DRAGONSPEAKER:
  case CARD_ID_SORIN_SOLEMN_VISITOR:
  case CARD_ID_DARETTI_SCRAP_SAVANT:
  case CARD_ID_FREYALISE_LLANOWARS_FURY:
  case CARD_ID_NAHIRI_THE_LITHOMANCER:
  case CARD_ID_OB_NIXILIS_OF_THE_BLACK_OATH:
  case CARD_ID_TEFERI_TEMPORAL_ARCHMAGE:
  case CARD_ID_UGIN_THE_SPIRIT_DRAGON:
  case CARD_ID_NARSET_TRANSCENDENT:
  case CARD_ID_SARKHAN_UNBROKEN:
  case CARD_ID_CHANDRA_ROARING_FLAME:
  case CARD_ID_JACE_TELEPATH_UNBOUND:
  case CARD_ID_GIDEON_BATTLE_FORGED:
  case CARD_ID_LILIANA_DEFIANT_NECROMANCER:
  case CARD_ID_NISSA_SAGE_ANIMIST:
  case CARD_ID_GIDEON_ALLY_OF_ZENDIKAR:
  case CARD_ID_KIORA_MASTER_OF_THE_DEPTHS:
  case CARD_ID_OB_NIXILIS_REIGNITED:
  case CARD_ID_CHANDRA_FLAMECALLER:
  case CARD_ID_NISSA_VOICE_OF_ZENDIKAR:
  case CARD_ID_ARLINN_KORD:
  case CARD_ID_ARLINN_EMBRACED_BY_THE_MOON:
  case CARD_ID_JACE_UNRAVELER_OF_SECRETS:
  case CARD_ID_NAHIRI_THE_HARBINGER:
  case CARD_ID_SORIN_GRIM_NEMESIS:
  case CARD_ID_LILIANA_THE_LAST_HOPE:
  case CARD_ID_TAMIYO_FIELD_RESEARCHER:
  case CARD_ID_CHANDRA_PYROGENIUS:
  case CARD_ID_CHANDRA_TORCH_OF_DEFIANCE:
  case CARD_ID_DARETTI_INGENIOUS_ICONOCLAST:
  case CARD_ID_DOVIN_BAAN:
  case CARD_ID_KAYA_GHOST_ASSASSIN:
  case CARD_ID_NISSA_NATURES_ARTISAN:
  case CARD_ID_NISSA_VITAL_FORCE:
  case CARD_ID_SAHEELI_RAI:
  case CARD_ID_AJANI_UNYIELDING:
  case CARD_ID_AJANI_VALIANT_PROTECTOR:
  case CARD_ID_TEZZERET_THE_SCHEMER:
  case CARD_ID_TEZZERET_MASTER_OF_METAL:
  case CARD_ID_GIDEON_OF_THE_TRIALS:
  case CARD_ID_GIDEON_MARTIAL_PARAGON:
  case CARD_ID_LILIANA_DEATH_WIELDER:
  case CARD_ID_LILIANA_DEATHS_MAJESTY:
  case CARD_ID_NISSA_STEWARD_OF_ELEMENTS:
  case CARD_ID_NICOL_BOLAS_GOD_PHARAOH:
  case CARD_ID_SAMUT_THE_TESTED:
  case CARD_ID_HUATLI_WARRIOR_POET:
  case CARD_ID_VRASKA_RELIC_SEEKER:
  case CARD_ID_ANGRATH_MINOTAUR_PIRATE:
  case CARD_ID_ANGRATH_THE_FLAME_CHAINED:
  case CARD_ID_HUATLI_RADIANT_CHAMPION:
  case CARD_ID_KARN_SCION_OF_URZA:
  case CARD_ID_TEFERI_HERO_OF_DOMINARIA:
  case CARD_ID_TEFERI_TIMEBENDER:
  case CARD_ID_AJANI_ADVERSARY_OF_TYRANTS:
  case CARD_ID_LILIANA_UNTOUCHED_BY_DEATH:
  case CARD_ID_NICOL_BOLAS_THE_ARISEN:
  case CARD_ID_TEZZERET_ARTIFICE_MASTER:
  case CARD_ID_VIVIEN_REID:
  case CARD_ID_AMINATOU_THE_FATESHIFTER:
  case CARD_ID_ESTRID_THE_MASKED:
  case CARD_ID_LORD_WINDGRACE:
  case CARD_ID_SAHEELI_THE_GIFTED:
  case CARD_ID_RAL_IZZET_VICEROY:
  case CARD_ID_VRASKA_GOLGARI_QUEEN:
  case CARD_ID_DOVIN_GRAND_ARBITER:
  case 0x4d5b:
  case 0x4d5d:
  case 0x4d60:
  case 0x4d62:
  case 0x4d7c:
  case 0x4d87:
  case 0x4d90:
  case 0x4d92:
  case 0x4dab:
  case 0x4dc2:
  case 0x4dd0:
  case 0x4dd2:
  case 0x4dd3:
  case 0x4dd5:
  case 0x4dd9:
  case 0x4ddb:
  case 0x4de5:
  case 0x4df4:
  case 0x4df6:
  case 0x4df9:
  case 0x4dfb:
  case 0x4e01:
  case 0x4e0d:
  case 0x4e18:
  case 0x4e1a:
  case 0x4e1b:
  case 0x4e23:
  case 0x4e30:
  case 0x4e32:
  case 0x4e35:
  case 0x4e36:
  case 0x4e38:
  case 0x4e3c:
  case 0x4e47:
  case 0x4e4d:
  case 0x4e51:
  case 0x4ee8:
  case 0x4f29:
  case 0x4f32:
  case 0x4f51:
  case 0x4f52:
  case 0x4f55:
  case 0x4f98:
  case 0x4fbc:
  case 0x4fd0:
  case 0x5084:
  case 0x50d2:
  case 0x51e0:
  case 0x51eb:
  case 0x520b:
  case 0x5356:
  case 0x53b8:
  case 0x5568:
  case 0x55a2:
  case 0x55a9:
  case 0x567e:
  case 0x56c4:
  case 0x58ae:
  case 0x58cc:
  case 0x58e9:
  case 0x5901:
  case 0x5902:
  case 0x59d8:
  case 0x5a06:
  case 0x5a13:
    return COUNTER_LOYALTY;
  case CARD_ID_CHANCE_ENCOUNTER:
  case CARD_ID_GEMSTONE_CAVERNS:
    return COUNTER_LUCK;
  case CARD_ID_MAGNETIC_WEB:
    return COUNTER_MAGNET;
  case CARD_ID_MINE_LAYER:
    return COUNTER_MINE;
  case CARD_ID_GEMSTONE_MINE:
    return COUNTER_MINING;
  case CARD_ID_CYCLOPEAN_TOMB:
    return COUNTER_MIRE;
  case CARD_ID_ASSEMBLE_THE_LEGION:
    return COUNTER_MUSTER;
  case CARD_ID_MERSEINE:
    return COUNTER_NET;
  case CARD_ID_CELESTIAL_CONVERGENCE:
    return COUNTER_OMEN;
  case CARD_ID_ORCISH_MINE:
    return COUNTER_ORE;
  case CARD_ID_BARRINS_CODEX:
  case CARD_ID_PRIVATE_RESEARCH:
    return COUNTER_PAGE;
  case CARD_ID_TORTURE_CHAMBER:
    return COUNTER_PAIN;
  case CARD_ID_DREAD_WIGHT:
    return COUNTER_PARALYZATION;
  case CARD_ID_LOTUS_BLOSSOM:
    return COUNTER_PETAL;
  case CARD_ID_VOODOO_DOLL:
    return COUNTER_PIN;
  case CARD_ID_TRAVELING_PLAGUE:
  case CARD_ID_WITHERING_HEX:
  case CARD_ID_PLAGUE_BOILER:
    return COUNTER_PLAGUE;
  case CARD_ID_CORAL_REEF:
    return COUNTER_POLYP;
  case CARD_ID_MAGMA_MINE:
  case CARD_ID_HELLION_CRUCIBLE:
  case CARD_ID_MOUNT_KERALIA:
    return COUNTER_PRESSURE;
  case CARD_ID_COCOON:
    return COUNTER_PUPA;
  case CARD_ID_ARCHMAGE_ASCENSION:
  case CARD_ID_BEASTMASTER_ASCENSION:
  case CARD_ID_BLOODCHIEF_ASCENSION:
  case CARD_ID_IOR_RUIN_EXPEDITION:
  case CARD_ID_KHALNI_HEART_EXPEDITION:
  case CARD_ID_LUMINARCH_ASCENSION:
  case CARD_ID_PYROMANCER_ASCENSION:
  case CARD_ID_QUEST_FOR_ANCIENT_SECRETS:
  case CARD_ID_QUEST_FOR_PURE_FLAME:
  case CARD_ID_QUEST_FOR_THE_GEMBLADES:
  case CARD_ID_QUEST_FOR_THE_GRAVELORD:
  case CARD_ID_QUEST_FOR_THE_HOLY_RELIC:
  case CARD_ID_SOUL_STAIR_EXPEDITION:
  case CARD_ID_SUNSPRING_EXPEDITION:
  case CARD_ID_ZEKTAR_SHRINE_EXPEDITION:
  case CARD_ID_QUEST_FOR_RENEWAL:
  case CARD_ID_QUEST_FOR_ULAS_TEMPLE:
  case CARD_ID_QUEST_FOR_THE_GOBLIN_LORD:
  case CARD_ID_QUEST_FOR_THE_NIHIL_STONE:
    return COUNTER_QUEST;
  case CARD_ID_ALL_HALLOWS_EVE:
  case CARD_ID_ENDLESS_SCREAM:
    return COUNTER_SCREAM;
  case CARD_ID_ARETOPOLIS:
    return COUNTER_SCROLL;
  case CARD_ID_ROC_HATCHLING:
    return COUNTER_SHELL;
  case CARD_ID_PALLIATION_ACCORD:
    return COUNTER_PALLIATION;
  case CARD_ID_CEPHALID_VANDAL:
    return COUNTER_SHRED;
  case CARD_ID_CHROMATIC_ARMOR:
    return COUNTER_SLEIGHT;
  case CARD_ID_GUTTER_GRIME:
    return COUNTER_SLIME;
  case CARD_ID_SMOKESTACK:
    return COUNTER_SOOT;
  case CARD_ID_ELVISH_FARMER:
  case CARD_ID_FERAL_THALLID:
  case CARD_ID_SPORE_FLOWER:
  case CARD_ID_THALLID:
  case CARD_ID_THALLID_DEVOURER:
  case CARD_ID_THORN_THALLID:
  case CARD_ID_DEATHSPORE_THALLID:
  case CARD_ID_SAVAGE_THALLID:
  case CARD_ID_SPORESOWER_THALLID:
  case CARD_ID_THALLID_GERMINATOR:
  case CARD_ID_THALLID_SHELL_DWELLER:
  case CARD_ID_MYCOLOGIST:
  case CARD_ID_PALLID_MYCODERM:
  case CARD_ID_PSYCHOTROPE_THALLID:
  case CARD_ID_VITASPORE_THALLID:
  case CARD_ID_SPOROLOTH_ANCIENT:
  case CARD_ID_UTOPIA_MYCON:
    return COUNTER_SPORE;
  case CARD_ID_CITY_OF_SHADOWS:
  case CARD_ID_BOTTOMLESS_VAULT:
  case CARD_ID_DWARVEN_HOLD:
  case CARD_ID_HOLLOW_TREES:
  case CARD_ID_ICATIAN_STORE:
  case CARD_ID_SAND_SILOS:
  case CARD_ID_FOUNTAIN_OF_CHO:
  case CARD_ID_MERCADIAN_BAZAAR:
  case CARD_ID_RUSHWOOD_GROVE:
  case CARD_ID_SAPRAZZAN_COVE:
  case CARD_ID_SUBTERRANEAN_HANGAR:
  case CARD_ID_CALCIFORM_POOLS:
  case CARD_ID_DREADSHIP_REEF:
  case CARD_ID_FUNGAL_REACHES:
  case CARD_ID_MOLTEN_SLAGHEAP:
  case CARD_ID_SALTCRUSTED_STEPPE:
  case CARD_ID_CRUCIBLE_OF_THE_SPIRIT_DRAGON:
  case CARD_ID_MAGE_RING_NETWORK:
    return COUNTER_STORAGE;
  case CARD_ID_CRESCENDO_OF_WAR:
    return COUNTER_STRIFE;
  case CARD_ID_PURSUIT_OF_KNOWLEDGE:
  case CARD_ID_GRIMOIRE_OF_THE_DEAD:
    return COUNTER_STUDY;
  case CARD_ID_NIGHT_DEALINGS:
    return COUNTER_THEFT;
  case CARD_ID_HOMARID:
  case CARD_ID_TIDAL_INFLUENCE:
    return COUNTER_TIDE;
  case CARD_ID_TIME_VAULT:
  case CARD_ID_TOURACHS_GATE:
  case CARD_ID_INFINITE_HOURGLASS:
  case CARD_ID_TIME_BOMB:
  case CARD_ID_ANCESTRAL_VISION:
  case CARD_ID_CORPULENT_CORPSE:
  case CARD_ID_CURSE_OF_THE_CABAL:
  case CARD_ID_DEEP_SEA_KRAKEN:
  case CARD_ID_DIVINE_CONGREGATION:
  case CARD_ID_DURKWOOD_BALOTH:
  case CARD_ID_DUSKRIDER_PEREGRINE:
  case CARD_ID_ERRANT_EPHEMERON:
  case CARD_ID_GREATER_GARGADON:
  case CARD_ID_GREATER_GARGADON_SUSPENDED:
  case CARD_ID_HYPERGENESIS:
  case CARD_ID_ITH_HIGH_ARCANIST:
  case CARD_ID_IVORY_GIANT:
  case CARD_ID_KELDON_HALBERDIER:
  case CARD_ID_LIVING_END:
  case CARD_ID_LOTUS_BLOOM:
  case CARD_ID_MINDSTAB:
  case CARD_ID_NANTUKO_SHAMAN:
  case CARD_ID_PARDIC_DRAGON:
  case CARD_ID_PHTHISIS:
  case CARD_ID_PLUNDER:
  case CARD_ID_RESTORE_BALANCE:
  case CARD_ID_RIFT_BOLT:
  case CARD_ID_RIFTWING_CLOUDSKATE:
  case CARD_ID_SEARCH_FOR_TOMORROW:
  case CARD_ID_VISCERID_DEEPWALKER:
  case CARD_ID_WHEEL_OF_FATE:
  case CARD_ID_AEON_CHRONICLER:
  case CARD_ID_AVEN_RIFTWATCHER:
  case CARD_ID_BENALISH_COMMANDER:
  case CARD_ID_CALCIDERM:
  case CARD_ID_CHRONOZOA:
  case CARD_ID_DEADLY_GRUB:
  case CARD_ID_DEADWOOD_TREEFOLK:
  case CARD_ID_DETRITIVORE:
  case CARD_ID_DICHOTOMANCY:
  case CARD_ID_FUNGAL_BEHEMOTH:
  case CARD_ID_GIANT_DUSTWASP:
  case CARD_ID_HEROES_REMEMBERED:
  case CARD_ID_KELDON_MARAUDERS:
  case CARD_ID_LAVACORE_ELEMENTAL:
  case CARD_ID_REALITY_ACID:
  case CARD_ID_RIFTMARKED_KNIGHT:
  case CARD_ID_ROILING_HORROR:
  case CARD_ID_SHADE_OF_TROKAIR:
  case CARD_ID_SHIVAN_METEOR:
  case CARD_ID_TIDEWALKER:
  case CARD_ID_TIMEBENDER:
  case CARD_ID_TIMECRAFTING:
  case CARD_ID_VEILING_ODDITY:
  case CARD_ID_WANING_WURM:
  case CARD_ID_ARC_BLADE:
  case CARD_ID_CHRONOMANTIC_ESCAPE:
  case CARD_ID_CYCLICAL_EVOLUTION:
  case CARD_ID_DUST_OF_MOMENTS:
  case CARD_ID_EPOCHRASITE:
  case CARD_ID_FESTERING_MARCH:
  case CARD_ID_INFILTRATOR_IL_KOR:
  case CARD_ID_KNIGHT_OF_SURSI:
  case CARD_ID_LOST_AURAMANCERS:
  case CARD_ID_MAELSTROM_DJINN:
  case CARD_ID_NIHILITH:
  case CARD_ID_PETRIFIED_PLATING:
  case CARD_ID_RAVAGING_RIFTWURM:
  case CARD_ID_REALITY_STROBE:
  case CARD_ID_SHIVAN_SAND_MAGE:
  case CARD_ID_SOULTETHER_GOLEM:
  case CARD_ID_AS_FORETOLD:
  case 0x4ebe:
    return COUNTER_TIME;
  case CARD_ID_HELIX_PINNACLE:
    return COUNTER_TOWER;
  case CARD_ID_TRAP_DIGGER:
    return COUNTER_TRAP;
  case CARD_ID_LEGACYS_ALLURE:
    return COUNTER_TREASURE;
  case CARD_ID_DISCORDANT_DIRGE:
  case CARD_ID_LILTING_REFRAIN:
  case CARD_ID_MIDSUMMER_REVEL:
  case CARD_ID_RECANTATION:
  case CARD_ID_RUMBLING_CRESCENDO:
  case CARD_ID_SERRAS_HYMN:
  case CARD_ID_SERRAS_LITURGY:
  case CARD_ID_TORCH_SONG:
  case CARD_ID_VILE_REQUIEM:
  case CARD_ID_WAR_DANCE:
  case CARD_ID_YISAN_THE_WANDERER_BARD:
  case 0x4e60:
    return COUNTER_VERSE;
  case CARD_ID_LIVING_ARTIFACT:
    return COUNTER_VITALITY;
  case CARD_ID_ROGUE_SKYCAPTAIN:
    return COUNTER_WAGE;
  case CARD_ID_MERCADIAN_LIFT:
    return COUNTER_WINCH;
  case CARD_ID_CYCLONE:
  case CARD_ID_FREYALISES_WINDS:
    return COUNTER_WIND;
  case CARD_ID_DJINN_OF_WISHES:
  case CARD_ID_RING_OF_THREE_WISHES:
  case 0x5142:
    return COUNTER_WISH;
  case CARD_ID_BLACK_MANA_BATTERY:
  case CARD_ID_BLUE_MANA_BATTERY:
  case CARD_ID_GREEN_MANA_BATTERY:
  case CARD_ID_RED_MANA_BATTERY:
  case CARD_ID_WHITE_MANA_BATTERY:
  case CARD_ID_ICE_CAULDRON:
  case CARD_ID_JEWELED_AMULET:
  case CARD_ID_VENTIFACT_BOTTLE:
  case CARD_ID_BLACK_MARKET:
  case CARD_ID_KYREN_TOY:
  case CARD_ID_MAGISTRATES_SCEPTER:
  case CARD_ID_MANA_CACHE:
  case CARD_ID_RIPTIDE_REPLICATOR:
  case CARD_ID_ALTAR_OF_SHADOWS:
  case CARD_ID_BANSHEES_BLADE:
  case CARD_ID_CHALICE_OF_THE_VOID:
  case CARD_ID_JINXED_CHOKER:
  case CARD_ID_LIGHTNING_COILS:
  case CARD_ID_SERUM_TANK:
  case CARD_ID_SUN_DROPLET:
  case CARD_ID_ARCANE_SPYGLASS:
  case CARD_ID_AETHER_VIAL:
  case CARD_ID_CHIMERIC_EGG:
  case CARD_ID_DARKSTEEL_REACTOR:
  case CARD_ID_MIRRODINS_CORE:
  case CARD_ID_SPAWNING_PIT:
  case CARD_ID_TALON_OF_PAIN:
  case CARD_ID_BATON_OF_COURAGE:
  case CARD_ID_CLEARWATER_GOBLET:
  case CARD_ID_ENGINEERED_EXPLOSIVES:
  case CARD_ID_GEMSTONE_ARRAY:
  case CARD_ID_HELIOPHIAL:
  case CARD_ID_INFUSED_ARROWS:
  case CARD_ID_OPALINE_BRACERS:
  case CARD_ID_PENTAD_PRISM:
  case CARD_ID_OROCHI_HATCHERY:
  case CARD_ID_TENDO_ICE_BRIDGE:
  case CARD_ID_UMEZAWAS_JITTE:
  case CARD_ID_LIGHTNING_STORM:
  case CARD_ID_COALITION_RELIC:
  case CARD_ID_VIVID_CRAG:
  case CARD_ID_VIVID_CREEK:
  case CARD_ID_VIVID_GROVE:
  case CARD_ID_VIVID_MARSH:
  case CARD_ID_VIVID_MEADOW:
  case CARD_ID_DOOR_OF_DESTINIES:
  case CARD_ID_SIGIL_OF_DISTINCTION:
  case CARD_ID_LIGHTNING_REAVER:
  case CARD_ID_ETERNITY_VESSEL:
  case CARD_ID_EVERFLOWING_CHALICE:
  case CARD_ID_ANGELHEART_VIAL:
  case CARD_ID_SPHINX_BONE_WAND:
  case CARD_ID_SURRAKAR_SPELLBLADE:
  case CARD_ID_CHIMERIC_MASS:
  case CARD_ID_CULLING_DAIS:
  case CARD_ID_GOLDEN_URN:
  case CARD_ID_GOLEM_FOUNDRY:
  case CARD_ID_GRINDCLOCK:
  case CARD_ID_LUX_CANNON:
  case CARD_ID_NECROGEN_CENSER:
  case CARD_ID_RATCHET_BOMB:
  case CARD_ID_TRIGON_OF_CORRUPTION:
  case CARD_ID_TRIGON_OF_INFESTATION:
  case CARD_ID_TRIGON_OF_MENDING:
  case CARD_ID_TRIGON_OF_RAGE:
  case CARD_ID_TRIGON_OF_THOUGHT:
  case CARD_ID_TUMBLE_MAGNET:
  case CARD_ID_SHRIEKHORN:
  case CARD_ID_SPHERE_OF_THE_SUNS:
  case CARD_ID_TITAN_FORGE:
  case CARD_ID_CONVERSION_CHAMBER:
  case CARD_ID_SHRINE_OF_BOUNDLESS_GROWTH:
  case CARD_ID_SHRINE_OF_BURNING_RAGE:
  case CARD_ID_SHRINE_OF_LIMITLESS_POWER:
  case CARD_ID_SHRINE_OF_LOYAL_LEGIONS:
  case CARD_ID_SHRINE_OF_PIERCING_VISION:
  case CARD_ID_SURGE_NODE:
  case CARD_ID_DRUIDS_REPOSITORY:
  case CARD_ID_OTHERWORLD_ATLAS:
  case CARD_ID_MANA_BLOOM:
  case CARD_ID_ASTRAL_CORNUCOPIA:
  case CARD_ID_KILNSPIRE_DISTRICT:
  case CARD_ID_PRIMAL_AMULET:
  case CARD_ID_TRANSMOGRIFYING_WAND:
  case CARD_ID_FIREMINDS_RESEARCH:
    return COUNTER_CHARGE;
  case CARD_ID_PRISM_ARRAY:
    return COUNTER_CRYSTAL;
  case CARD_ID_BRIBERS_PURSE:
    return COUNTER_GEM;
  case CARD_ID_QUARANTINE_FIELD:
    return COUNTER_ISOLATION;
  case CARD_ID_CALL_FOR_UNITY:
    return COUNTER_UNITY;
  case CARD_ID_EDIFICE_OF_AUTHORITY:
  case CARD_ID_LUXA_RIVER_SHRINE:
  case CARD_ID_ORACLES_VAULT:
  case CARD_ID_PYRAMID_OF_THE_PANTHEON:
  case CARD_ID_SUNSET_PYRAMID:
    return COUNTER_BRICK;
  case CARD_ID_TREASURE_MAP:
    return COUNTER_LANDMARK;
  case CARD_ID_TETZIMOC_PRIMAL_DEATH:
    return COUNTER_PREY;
  case CARD_ID_ARIXMETHES_SLUMBERING_ISLE:
    return COUNTER_SLUMBER;
  case 0x5127:
    return COUNTER_KNOWLEDGE;
  case 0x5226:
    return COUNTER_TASK;
  case 0x5374:
    return COUNTER_FORESHADOW;
  case 0x5481:
    return COUNTER_INCARNATION;
  case 0x53f5:
  case 0x5478:
    return COUNTER_SOUL;
  default:
    return -1;
  }
}

int get_counter_type_by_instance(card_instance_t* instance)
{
  int iid = instance->internal_card_id;

  if (iid == -1)
	iid = instance->backup_internal_card_id;

  if (iid == activation_card)
	iid = instance->original_internal_card_id;

  int csvid;
  if (iid == LEGACY_EFFECT_CUSTOM)
	{
	  if (instance->info_slot == (int)generic_suspend_legacy)
		return COUNTER_TIME;
	  else
		csvid = instance->display_pic_csv_id;
	}
  else
	csvid = cards_data[iid].id;

  return get_counter_type_by_id(csvid);
}

void get_counter_name_by_type(char *dest, counter_t counter_type, int num)
{
  if (num <= 0)
    return;
  if ((unsigned int)counter_type > COUNTER_end)
    counter_type = COUNTER_end;
  if (num == 256)
    scnprintf(dest, 100, "%s", counter_names[counter_type]);
  else
    scnprintf(dest, 100, "%s: %d", counter_names[counter_type], num);
}

void get_special_counters_name(char *dest, int csvid, int num)
{
  if (num > 0)
    get_counter_name_by_type(dest, get_counter_type_by_id(csvid), num);
}







// num must be > 0, or no effect


/* num must be > 0, or no effect.  Assumes caller has already called get_updated_counters_number().  This still accounts for putting counters on
 * Melira's Keepers, but not onto effect cards.  Do not call for the latter. */


// unaffected by doubling effects; num may be positive or negative
void add_counters_as_cost(int player, int card, counter_t type, int num)
{
  if (0 < num)
  {
    num = get_updated_counters_number_as_cost(player, card, type, num);
  }
  if (num == 0)
  {
    return;
  }
  add_or_remove_counters_impl(player, card, player, card, type, num);
  return;
}

// num must be > 0, or no effect


/* For each counter on {src_player, src_card}, add one of that type to {t_player, t_card}.  If raw is set, add the exact amount (even for Melira's Keepers or
 * Melira, Sylvok Outcast, or if Doubling Season etc. are present). */


/* Moves number of counter_type from {src_player, src_card} to {t_player, t_card}.  Moves all counters of all types if counter_type is -1; moves all of
 * counter_type if number is -1. */


static int count_counters_impl(card_instance_t* instance, counter_t type)
{
  if (type == COUNTER_P1_P1)
	return instance->counters;	// Dedicated slot for +1/+1 counters

  if (type == COUNTER_M1_M1)
	return instance->counters_m1m1;	// Dedicated slot for -1/-1 counters

  uint8_t* t18 = (void*)(&instance->targets[18].player);

  int tot;
  if ((unsigned char)type == COUNTER_invalid)	// Total count of all counters
	{
	  tot = (instance->special_counters		// dedicated to a type determined by card's csvid
			 + instance->counters			// dedicated to +1/+1 counters
			 + instance->counters_m1m1);	// dedicated to -1/-1 counters

	  // Dynamically-assigned counter types.
	  // Four visible
	  if (t18[0] != COUNTER_invalid)
		tot += instance->counters2;
	  if (t18[1] != COUNTER_invalid)
		tot += instance->counters3;
	  if (t18[2] != COUNTER_invalid)
		tot += instance->counters4;
	  if (t18[3] != COUNTER_invalid)
		tot += instance->counters5;
	  // Two more invisible
	  if (t18[6] != COUNTER_invalid)
		tot += t18[4];
	  if (t18[7] != COUNTER_invalid)
		tot += t18[5];

	  return tot;
	}

  /* General case.  Be a little extra paranoid in case a card has gotten the same counter type both in special_counters (type determined by csvid) and an
   * assignable counter slot, presumeably by an uncontrolled copy effect. */

  tot = 0;

  if (instance->special_counter_type == type)
	tot += instance->special_counters;

  // Four visible
  if (t18[0] == type)
	tot += instance->counters2;
  if (t18[1] == type)
	tot += instance->counters3;
  if (t18[2] == type)
	tot += instance->counters4;
  if (t18[3] == type)
	tot += instance->counters5;
  // Two more invisible
  if (t18[6] == type)
	tot += t18[4];
  if (t18[7] == type)
	tot += t18[5];

  return tot;
}

// use -1 to get total count
int count_counters(int player, int card, counter_t type)
{
  card_instance_t* instance = get_card_instance(player, card);
  if (instance->internal_card_id == activation_card)
	instance = get_card_instance(instance->parent_controller, instance->parent_card);

  return count_counters_impl(instance, type);
}

/* Just like count_counters(), but if called for an activation_card, return the counters that were on th parent card as it was activated, not how many are on it
 * now. */
int count_counters_no_parent(int player, int card, counter_t type)
{
  return count_counters_impl(get_card_instance(player, card), type);
}

// use -1 to remove all of all types


// May use ANYBODY for player and/or -1 for counter_type to indicate any player's cards or any kind of counter.  Returns a total count.
int count_counters_by_counter_and_card_type(int player, counter_t counter_type, type_t type)
{
  int p, c, result = 0;
  for (p = 0; p <= 1; ++p)
	if (p == player || player == ANYBODY)
	  for (c = 0; c < active_cards_count[p]; ++c)
		if (in_play(p, c) && is_what(p, c, type))
			result += count_counters(p, c, counter_type);

  return result;
}

// May use ANYBODY for player and/or -1 for counter_type to indicate any player's cards or any kind of counter.  Stops looking after the first match.
int has_any_counters(int player, counter_t counter_type, type_t type)
{
  int p, c;
  for (p = 0; p <= 1; ++p)
	if (p == player || player == ANYBODY)
	  for (c = 0; c <= active_cards_count[p]; ++c)
		if (in_play(p, c) && is_what(p, c, type) && count_counters(p, c, counter_type))
		  return 1;

  return 0;
}

/* The return values here, which should all be between -10000 and 10000, indicate how much the AI values a counter type on a card it controls.  They're inverted
 * if the opponent controls the card.
 *
 * If adding is nonzero, the AI is considering adding another counter to the card (there will always be at least one counter of that type already there);
 * otherwise, it's considering removing a counter of that type, possibly the last.
 *
 * So, for example, COUNTER_EON returns 0 for a non-native card, else 4 if adding, else 9001: this means, the AI doesn't care one way or the other about Eon
 * counters on a Grizzly Bears; is very slightly in favor of adding a second (or third, etc.) Eon counter to a Magosi, the Waterveil it controls; is very
 * slightly against adding a second (or third, etc.) Eon counter to a Magosi, the Waterveil the human player controls; is very, very much against removing an
 * Eon counter from its own Magosi, the Waterveil; and is very, very much in favor of removing an Eon counter from the human player's Magosi, the Waterveil. */
static int raw_counter_value(int player, int t_player, int t_card, counter_t counter_type,
                             int adding)
{
  card_instance_t *instance;
  int result;
  int count;
  int limit;
  unsigned int csvid;
  int value;
  int iid;

  instance = get_card_instance(t_player, t_card);
  iid = instance->internal_card_id;
  if (iid < 0)
    iid = instance->backup_internal_card_id;
  csvid = cards_data[iid].id;
  if (adding == 2)
  {
    result = count_counters(t_player, t_card, counter_type);
    adding = (int)(0 < result);
  }
  switch (counter_type)
  {
  case COUNTER_P1_P1:
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    if (csvid == 0x2367)
    {
      return 7300;
    }
    if (csvid < 0x2368)
    {
      if (csvid == 0x18cb)
      {
        return 7300;
      }
      if (csvid < 0x18cc)
      {
        if (csvid == 0xdf2)
        {
          return 7300;
        }
        if (csvid < 0xdf3)
        {
          if (csvid == 0xd66)
          {
            return 7300;
          }
          if ((csvid - 0x216 & 0xfffb) == 0)
          {
            return 7300;
          }
        }
        else
        {
          if (csvid == 0xe02)
          {
            return 7300;
          }
          if (csvid == 0xfb5)
          {
            return 7300;
          }
        }
      }
      else
      {
        if (csvid == 0x1f39)
        {
          return 7300;
        }
        if (csvid < 0x1f3a)
        {
          if (csvid == 0x1a63)
          {
            return 7300;
          }
          if (csvid == 0x1b89)
          {
            return 7300;
          }
        }
        else
        {
          if (csvid == 0x1fd9)
          {
            return 7300;
          }
          if (csvid == 0x2206)
          {
            return 7300;
          }
        }
      }
    }
    else
    {
      if (csvid == 0x2b36)
      {
        return 7300;
      }
      if (csvid < 0x2b37)
      {
        if (csvid == 0x277c)
        {
          return 7300;
        }
        if (csvid < 0x277d)
        {
          if (csvid == 0x24d7)
          {
            return 7300;
          }
          if (csvid < 0x24d8)
          {
            if ((uint16_t)(csvid + 0xdb77) < 2)
            {
              return 7300;
            }
          }
          else if (csvid == 0x2722)
          {
            return 7300;
          }
        }
        else
        {
          if (csvid == 0x2810)
          {
            return 7300;
          }
          if (csvid == 0x2a1f)
          {
            return 7300;
          }
        }
      }
      else
      {
        if (csvid == 0x2ef6)
        {
          return 7300;
        }
        if (csvid < 0x2ef7)
        {
          if (csvid == 0x2de7)
          {
            return 7300;
          }
          if (csvid == 0x2df5)
          {
            return 7300;
          }
        }
        else
        {
          if (csvid == 0x306c)
          {
            return 7300;
          }
          if (csvid == 0x32aa)
          {
            return 7300;
          }
        }
      }
    }
    result = check_special_flags(t_player, t_card, 0x8000);
    value = (-(unsigned int)(result == 0) & 0xffffe82c) + 0x1c84;
    break;
  case COUNTER_M1_M1:
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = -3200;
    break;
  case COUNTER_P0_P1:
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = 150;
    break;
  case COUNTER_P0_P2:
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = 300;
    break;
  case COUNTER_P1_P0:
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = 600;
    break;
  case COUNTER_P1_P2:
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = 1800;
    break;
  case COUNTER_P2_P0:
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = 2400;
    break;
  case COUNTER_P2_P2:
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = 4800;
    break;
  case COUNTER_M0_M1:
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = -1600;
    break;
  case COUNTER_M0_M2:
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = -7200;
    break;
  case COUNTER_M1_M0:
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = -1563;
    break;
  case COUNTER_M2_M1:
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = -4800;
    break;
  case COUNTER_M2_M2:
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = -9600;
    break;
  case COUNTER_AGE:
    result = get_counter_type_by_instance(instance);
    if (result != 0xd)
    {
      return 0;
    }
    value = -1000;
    break;
  case COUNTER_AIM:
    result = get_counter_type_by_instance(instance);
    if (result != 0xe)
    {
      return 0;
    }
    value = 125;
    break;
  case COUNTER_ARROW:
    result = get_counter_type_by_instance(instance);
    if (result != 0xf)
    {
      return 0;
    }
    value = 440;
    break;
  case COUNTER_ARROWHEAD:
    result = get_counter_type_by_instance(instance);
    if (result != 0x10)
    {
      return 0;
    }
    value = 490;
    break;
  case COUNTER_AWAKENING:
    result = is_what(t_player, t_card, 1);
    if (result == 0)
    {
      return 0;
    }
    value = 6;
    break;
  case COUNTER_BLAZE:
    result = get_counter_type_by_instance(instance);
    if (result == 0x12)
    {
      value = 52;
    }
    else
    {
      result = is_what(t_player, t_card, 1);
      if (result == 0)
      {
        return 0;
      }
      value = (-(unsigned int)(adding == 0) & 0xfffffe73) - 4;
    }
    break;
  case COUNTER_BLOOD:
    if (csvid == 0x1e60)
    {
      value = -160;
    }
    else
    {
      value = (csvid != 0x204f) - 1 & 0x2260;
    }
    break;
  case COUNTER_BOUNTY:
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    return (-(unsigned int)(adding == 0) & 0xfffffdad) - 6;
  case COUNTER_BRIBERY:
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = (-(unsigned int)(adding == 0) & 0xfffffd4a) - 7;
    break;
  case COUNTER_CARRION:
    result = get_counter_type_by_instance(instance);
    if (result != 0x16)
    {
      return 0;
    }
    value = 40;
    break;
  case COUNTER_CHIP:
    result = get_counter_type_by_instance(instance);
    if (result != 0x17)
    {
      return 0;
    }
    value = 36;
    break;
  case COUNTER_CORPSE:
    result = get_counter_type_by_instance(instance);
    if (result != 0x18)
    {
      return 0;
    }
    value = 430;
    break;
  case COUNTER_CREDIT:
    result = get_counter_type_by_instance(instance);
    if (result != 0x19)
    {
      return 0;
    }
    value = 38;
    break;
  case COUNTER_CUBE:
    result = get_counter_type_by_instance(instance);
    if (result != 0x1a)
    {
      return 0;
    }
    value = 47;
    break;
  case COUNTER_CURRENCY:
    result = get_counter_type_by_instance(instance);
    if (result != 0x1b)
    {
      return 0;
    }
    value = 41;
    break;
  case COUNTER_DEATH:
    result = get_counter_type_by_instance(instance);
    if (result != 0x1c)
    {
      return 0;
    }
    value = (-(unsigned int)(adding == 0) & 0xffffec7c) - 5;
    break;
  case COUNTER_DELAY:
    if (csvid == 0x159f)
    {
      value = -850;
    }
    else
    {
      if (LEGACY_EFFECT_CUSTOM != iid)
      {
        return 0;
      }
      if (instance->display_pic_csv_id != 0xc41)
      {
        return 0;
      }
      value = -850;
    }
    break;
  case COUNTER_DEPLETION:
    result = get_counter_type_by_instance(instance);
    if (result != 0x1e)
    {
      return 0;
    }
    if (csvid == 0x196a)
    {
      value = -9999;
    }
    else if (csvid < 0x196b)
    {
      if (csvid < 0x11f7)
      {
        if (0x11d7 < csvid)
        {
          return (-(unsigned int)((1U << ((char)csvid + 0x28U & 0x1f) & 0x40400001U) == 0) &
                  0xfffffcc2) +
                 0x1e0;
        }
        if (csvid == 0x1188)
        {
          value = 480;
        }
        else
        {
          value = ((csvid == 0x11c6) - 1 & 0xfffffcc2) + 0x1e0;
        }
      }
      else
      {
        value = -350;
      }
    }
    else
    {
      value = ((csvid != 0x1981) - 1 & 0xffffff42) - 0x15e;
    }
    break;
  case COUNTER_DESPAIR:
    result = get_counter_type_by_instance(instance);
    if (result != 0x1f)
    {
      return 0;
    }
    value = 16;
    break;
  case COUNTER_DEVOTION:
    result = get_counter_type_by_instance(instance);
    if (result != 0x20)
    {
      return 0;
    }
    value = 420;
    break;
  case COUNTER_DIVINITY:
    result = get_counter_type_by_instance(instance);
    if (result == 0x21)
    {
      value = 9600;
    }
    else
    {
      value = (-(unsigned int)(adding == 0) & 0x1f3c) + 5;
    }
    break;
  case COUNTER_DOOM:
    if ((csvid == 0x1d7) && (life[1 - player] < life[player]))
    {
      value = 13;
    }
    else
    {
      value = (-(unsigned int)(adding == 0) & 0xfffffce7) - 8;
    }
    break;
  case COUNTER_DREAM:
    result = get_counter_type_by_instance(instance);
    if (result != 0x23)
    {
      return 0;
    }
    if (((csvid == 0x2fa) && (adding != 0)) &&
        (result = count_counters(t_player, t_card, COUNTER_DREAM), 6 < result))
    {
      return 0;
    }
    value = 410;
    break;
  case COUNTER_ECHO:
    result = get_counter_type_by_instance(instance);
    if (result != 0x24)
    {
      return 0;
    }
    value = 65;
    break;
  case COUNTER_ELIXIR:
    result = get_counter_type_by_instance(instance);
    if (result != 0x25)
    {
      return 0;
    }
    value = 44;
    break;
  case COUNTER_VORTEX:
    result = get_counter_type_by_instance(instance);
    if (result != 0x26)
    {
      return 0;
    }
    value = 380;
    break;
  case COUNTER_EON:
    result = get_counter_type_by_instance(instance);
    if (result != 0x27)
    {
      return 0;
    }
    value = (-(unsigned int)(adding == 0) & 0x2325) + 4;
    break;
  case COUNTER_EYEBALL:
    result = get_counter_type_by_instance(instance);
    if (result != 0x28)
    {
      return 0;
    }
    value = 70;
    break;
  case COUNTER_FADE:
    result = get_counter_type_by_instance(instance);
    if (result != 0x29)
    {
      return 0;
    }
    if (csvid == 0x127a)
    {
      value = 9200;
      if ((int)instance->damage_target_player == 1 - player)
      {
        value = ((t_player != player) - 1 & 0xffffb1e6) + 0x270d;
      }
    }
    else
    {
      value = 9200;
    }
    break;
  case COUNTER_FATE:
    value = 801;
    goto joined_r0x026d233a;
  case COUNTER_FEATHER:
    result = get_counter_type_by_instance(instance);
    if (result == 0x2b)
    {
      value = 8600;
    }
    else
    {
      value = (-(unsigned int)(adding == 0) & 0x2b6) + 7;
    }
    break;
  case COUNTER_FILIBUSTER:
    result = get_counter_type_by_instance(instance);
    if (result != 0x2c)
    {
      return 0;
    }
    value = 10000;
    break;
  case COUNTER_FLAME:
    result = get_counter_type_by_instance(instance);
    if (result != 0x2d)
    {
      return 0;
    }
    value = ((t_player != player) - 1 & 0xfffffaec) + 0x28a;
    break;
  case COUNTER_FLOOD:
    if (adding != 0)
    {
      return 0;
    }
    value = -301;
    break;
  case COUNTER_FUNGUS:
    value = -190;
    break;
  case COUNTER_FUNK:
    result = get_counter_type_by_instance(instance);
    if (result != 0x30)
    {
      return 0;
    }
    value = 9000;
    break;
  case COUNTER_FUSE:
    result = get_counter_type_by_instance(instance);
    if (result == 0x31)
    {
      value = 400;
    }
    else
    {
      result = is_what(t_player, t_card, 2);
      if (result == 0)
      {
        return 0;
      }
      value = -800;
    }
    break;
  case COUNTER_GLYPH:
    value = -210;
    break;
  case COUNTER_GOLD:
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = (-(unsigned int)(adding == 0) & 0xffffe892) - 3;
    break;
  case COUNTER_GROWTH:
    result = get_counter_type_by_instance(instance);
    if (result == 0x34)
    {
      return 0;
    }
    value = ((csvid == 0x1042) - 1 & 0xfffffe2f) + 0x1e0;
    break;
  case COUNTER_HATCHLING:
    result = get_counter_type_by_instance(instance);
    if (result != 0x35)
    {
      return 0;
    }
    value = 7900;
    break;
  case COUNTER_HEALING:
    result = get_counter_type_by_instance(instance);
    if (result != 0x36)
    {
      return 0;
    }
    value = 39;
    break;
  case COUNTER_HOOFPRINT:
    result = get_counter_type_by_instance(instance);
    if (result != 0x37)
    {
      return 0;
    }
    value = 8000;
    break;
  case COUNTER_HOURGLASS:
    if (adding == 0)
    {
      result = is_tapped(t_player, t_card);
      value = (-(unsigned int)(result == 0) & 0x7c8) - 0x7d1;
    }
    else
    {
      value = -9;
    }
    break;
  case COUNTER_HUNGER:
    result = get_counter_type_by_instance(instance);
    if (result != 0x39)
    {
      return 0;
    }
    value = 510;
    break;
  case COUNTER_HUSK:
    result = get_counter_type_by_instance(instance);
    if (result != 0x3a)
    {
      return 0;
    }
    value = 8100;
    break;
  case COUNTER_ICE:
    if (csvid == 0x20a4)
    {
      value = -330;
    }
    else if (csvid == 0x2117)
    {
      value = -230;
    }
    else
    {
      value = ((csvid == 0x7b7) - 1 & 0xffffffae) + 0x50;
    }
    break;
  case COUNTER_INFECTION:
    result = get_counter_type_by_instance(instance);
    if (result != 0x3c)
    {
      return 0;
    }
    value = 7800;
    break;
  case COUNTER_INTERVENTION:
    result = get_counter_type_by_instance(instance);
    if (result != 0x3d)
    {
      return 0;
    }
    value = 24;
    break;
  case COUNTER_JAVELIN:
    result = get_counter_type_by_instance(instance);
    if (result != 0x3e)
    {
      return 0;
    }
    value = 701;
    break;
  case COUNTER_KI:
    result = get_counter_type_by_instance(instance);
    if (result != 0x3f)
    {
      return 0;
    }
    value = 511;
    break;
  case COUNTER_LEVEL:
    result = get_counter_type_by_instance(instance);
    if (result != 0x40)
    {
      return 0;
    }
    if (0x2aaa < csvid)
    {
      switch (csvid - 0x2ac1 & 0xffff)
      {
      case 0:
      code_r0x026d22a5:
        result = 500;
        limit = 4;
        break;
      case 1:
      case 0x3a:
      case 0x60:
      case 0x8b:
      code_r0x026d2299:
        result = 0x1d4c;
        limit = 3;
        break;
      case 2:
      case 0x11:
      code_r0x026d228d:
        result = 500;
        limit = 5;
        break;
      case 7:
      case 0x13:
        result = 0x1d4c;
        limit = 5;
        break;
      case 0xb:
      case 0x19:
      case 0x3e:
      code_r0x026d223f:
        result = 0x1d4c;
        limit = 4;
        break;
      case 0x14:
        result = 500;
        limit = 8;
        break;
      case 0x15:
      case 0x28:
        result = 500;
        limit = 6;
        break;
      case 0x23:
      case 0x6f:
        result = 500;
        limit = 7;
        break;
      case 0x7a:
        result = 500;
        limit = 0xc;
        break;
      default:
        return 0;
      }
    code_r0x026d2249:
      if (adding == 0)
      {
        return result;
      }
      count = count_counters(t_player, t_card, COUNTER_LEVEL);
      if (limit <= count)
      {
        return 0;
      }
      return result;
    }
    if (csvid < 0x2a7e)
    {
      return 0;
    }
    switch (csvid - 0x2a7e & 0xffff)
    {
    case 0:
      goto code_r0x026d223f;
    default:
      return 0;
    case 5:
      goto code_r0x026d2299;
    case 9:
      goto code_r0x026d228d;
    case 0x10:
    case 0x23:
      goto code_r0x026d22a5;
    case 0x2c:
      result = 500;
      limit = 3;
      goto code_r0x026d2249;
    }
  case COUNTER_LORE:
    result = get_counter_type_by_instance(instance);
    if (result != 0x41)
    {
      return 0;
    }
    value = 8300;
    break;
  case COUNTER_LOYALTY:
    result = is_what(t_player, t_card, 0x100);
    if (result == 0)
    {
      return 0;
    }
    value = 8500;
    break;
  case COUNTER_LUCK:
    result = get_counter_type_by_instance(instance);
    if (result != 0x43)
    {
      return 0;
    }
    if (csvid == 0x158b)
    {
      value = 9998;
    }
    else
    {
      value = (-(unsigned int)(adding == 0) & 0x12a) + 3;
    }
    break;
  case COUNTER_MAGNET:
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = (-(unsigned int)(adding == 0) & 0xfffffc21) - 10;
    break;
  default:
    value = 0;
    break;
  case COUNTER_MANNEQUIN:
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = (-(unsigned int)(adding == 0) & 0xffffe4b2) - 0xb;
    break;
  case COUNTER_MATRIX:
    value = 46;
    break;
  case COUNTER_MINE:
    result = is_what(t_player, t_card, 1);
    if (result == 0)
    {
      return 0;
    }
    value = (-(unsigned int)(adding == 0) & 0xffffe4b2) - 0xc;
    break;
  case COUNTER_MINING:
    result = get_counter_type_by_instance(instance);
    if (result != 0x49)
    {
      return 0;
    }
    value = 470;
    break;
  case COUNTER_MIRE:
    result = is_what(t_player, t_card, 1);
    if (result == 0)
    {
      return 0;
    }
    value = (-(unsigned int)(adding == 0) & 0xffffff9c) - 1;
    break;
  case COUNTER_MUSIC:
    value = -700;
    break;
  case COUNTER_MUSTER:
    result = get_counter_type_by_instance(instance);
    if (result != 0x4c)
    {
      return 0;
    }
    value = 130;
    break;
  case COUNTER_NET:
    if (csvid != 0x70f)
    {
      return 0;
    }
    if ((int)instance->damage_target_player == 1 - player)
    {
      value = ((t_player == player) - 1 & 0xfffffff0) + 8;
    }
    else
    {
      if (instance->damage_target_player != player)
      {
        return 0;
      }
      value = ((t_player != player) - 1 & 0xfffffce0) + 400;
    }
    break;
  case COUNTER_OMEN:
    if (csvid != 0x12c7)
    {
      return 0;
    }
    if (life[1 - player] < life[player])
    {
      value = ((t_player == player) - 1 & 0xffffb1e6) + 0x270d;
    }
    else
    {
      if (life[1 - player] <= life[player])
      {
        return 0;
      }
      value = ((t_player != player) - 1 & 0xffffb1e0) + 10000;
    }
    break;
  case COUNTER_ORE:
    result = get_counter_type_by_instance(instance);
    if (result != 0x4f)
    {
      return 0;
    }
    value = -530;
    break;
  case COUNTER_PAGE:
    result = get_counter_type_by_instance(instance);
    if (result != 0x50)
    {
      return 0;
    }
    value = 370;
    break;
  case COUNTER_PAIN:
    result = get_counter_type_by_instance(instance);
    if (result != 0x51)
    {
      return 0;
    }
    value = 60;
    break;
  case COUNTER_PARALYZATION:
    value = -250;
    break;
  case COUNTER_PETAL:
    result = get_counter_type_by_instance(instance);
    if (result != 0x53)
    {
      return 0;
    }
    value = 81;
    break;
  case COUNTER_PHYLACTERY:
    value = (-(unsigned int)(adding == 0) & 7999) + 2;
    break;
  case COUNTER_PIN:
    result = get_counter_type_by_instance(instance);
    if (result != 0x56)
    {
      return 0;
    }
    value = -50;
    break;
  case COUNTER_PLAGUE:
    result = get_counter_type_by_instance(instance);
    if (result != 0x57)
    {
      return 0;
    }
    if (instance->damage_target_player == player)
    {
      value = ((t_player != player) - 1 & 0xffffff38) + 100;
    }
    else if ((1 - player != (int)instance->damage_target_player) ||
             (value = 0xffffff9c, t_player == player))
    {
      value = 100;
    }
    break;
  case COUNTER_POLYP:
    result = get_counter_type_by_instance(instance);
    if (result != 0x58)
    {
      return 0;
    }
    value = 110;
    break;
  case COUNTER_POP:
    result = get_counter_type_by_instance(instance);
    if (result != 0x59)
    {
      return 0;
    }
    value = 35;
    break;
  case COUNTER_PRESSURE:
    if (csvid == 0x39b2)
    {
      value = ((t_player == player) - 1 & 0xfffffd30) + 0x168;
    }
    else
    {
      result = get_counter_type_by_instance(instance);
      if (result != 0x5a)
      {
        return 0;
      }
      value = 360;
    }
    break;
  case COUNTER_PUPA:
    result = get_counter_type_by_instance(instance);
    if (result != 0x5b)
    {
      return 0;
    }
    value = -260;
    break;
  case COUNTER_QUEST:
    result = get_counter_type_by_instance(instance);
    if (result != 0x5c)
    {
      return 0;
    }
    if (csvid < 0x2994)
    {
      if (0x294b < csvid)
      {
        switch (csvid - 0x294c & 0xffff)
        {
        case 0:
        case 0xb:
        case 0x46:
        code_r0x026d32d2:
          result = 0x15e;
          limit = 3;
          break;
        default:
          return 0;
        case 0x1b:
          result = 6000;
          limit = 4;
          break;
        case 0x42:
          result = 6000;
          limit = 2;
          break;
        case 0x43:
          result = 10;
          limit = 5;
          break;
        case 0x44:
          goto code_r0x026d32b4;
        case 0x45:
          result = 0x15e;
          limit = 0;
          break;
        case 0x47:
          result = 0x15e;
          limit = 5;
        }
        goto code_r0x026d1b47;
      }
      if (csvid == 0x2908)
      {
        result = 6000;
        limit = 7;
        goto code_r0x026d1b47;
      }
      if (csvid != 0x290e)
      {
        if (csvid != 0x28ff)
        {
          return 0;
        }
        result = 6000;
        limit = 6;
        goto code_r0x026d1b47;
      }
    }
    else
    {
      if (csvid == 0x2a3b)
      {
      code_r0x026d32b4:
        result = 0x15e;
        limit = 4;
        goto code_r0x026d1b47;
      }
      if (csvid < 0x2a3c)
      {
        if (((csvid != 0x29b8) && (csvid != 0x29da)) && (csvid != 0x29ad))
        {
          return 0;
        }
        goto code_r0x026d32d2;
      }
      if (csvid == 0x2a3d)
      {
        result = 6000;
        limit = 5;
        goto code_r0x026d1b47;
      }
      if (csvid == 0x2a3e)
      {
        result = 0x15e;
        limit = 2;
        goto code_r0x026d1b47;
      }
      if (csvid != 0x2a3c)
      {
        return 0;
      }
    }
    result = 6000;
    limit = 3;
  code_r0x026d1b47:
    if (adding == 0)
    {
      return result;
    }
    count = count_counters(t_player, t_card, COUNTER_LEVEL);
    if (limit <= count)
    {
      return 0;
    }
    return result;
  case COUNTER_RUST:
    result = is_what(t_player, t_card, 0x40);
    if (result == 0)
    {
      return 0;
    }
    value = -600;
    break;
  case COUNTER_SCREAM:
    if ((LEGACY_EFFECT_CUSTOM == iid) && (instance->display_pic_csv_id == 0x232))
    {
      value = -500;
    }
    else
    {
      if (csvid != 0xc3e)
      {
        return 0;
      }
      if (instance->damage_target_player == player)
      {
        value = ((t_player == player) - 1 & 0xffffff10) + 0x78;
      }
      else
      {
        if ((int)instance->damage_target_player != 1 - player)
        {
          return 0;
        }
        value = ((t_player != player) - 1 & 0xffffff10) + 0x78;
      }
    }
    break;
  case COUNTER_SCROLL:
    if (csvid != 0x39a3)
    {
      return 0;
    }
    value = ((t_player == player) - 1 & 0xffffffbc) + 0x22;
    break;
  case COUNTER_SHELL:
    result = get_counter_type_by_instance(instance);
    if (result != 0x60)
    {
      return 0;
    }
    value = -280;
    break;
  case COUNTER_PALLIATION:
    result = get_counter_type_by_instance(instance);
    if (result != 0x61)
    {
      return 0;
    }
    value = 50;
    break;
  case COUNTER_SHOE:
    result = get_counter_type_by_instance(instance);
    if (result != 0x62)
    {
      return 0;
    }
    value = 1400;
    break;
  case COUNTER_SHRED:
    result = get_counter_type_by_instance(instance);
    if (result != 99)
    {
      return 0;
    }
    value = 7400;
    break;
  case COUNTER_SLEEP:
    value = -261;
    break;
  case COUNTER_SLEIGHT:
    result = get_counter_type_by_instance(instance);
    if (result != 0x65)
    {
      return 0;
    }
    value = -290;
    break;
  case COUNTER_SLIME:
    result = get_counter_type_by_instance(instance);
    if (result != 0x66)
    {
      return 0;
    }
    value = 7700;
    break;
  case COUNTER_SOOT:
    result = get_counter_type_by_instance(instance);
    if (result != 0x67)
    {
      return 0;
    }
    value = 14;
    break;
  case COUNTER_SPORE:
    result = get_counter_type_by_instance(instance);
    if (result != 0x68)
    {
      return 0;
    }
    value = 550;
    break;
  case COUNTER_STORAGE:
    result = get_counter_type_by_instance(instance);
    if (result != 0x69)
    {
      return 0;
    }
    value = 560;
    break;
  case COUNTER_STRIFE:
    result = get_counter_type_by_instance(instance);
    if (result != 0x6a)
    {
      return 0;
    }
    value = 320;
    break;
  case COUNTER_STUDY:
    result = get_counter_type_by_instance(instance);
    if (result != 0x6b)
    {
      return 0;
    }
    value = 7600;
    break;
  case COUNTER_THEFT:
    result = get_counter_type_by_instance(instance);
    if (result != 0x6c)
    {
      return 0;
    }
    value = 140;
    break;
  case COUNTER_THIRD_DEGREE_BURN:
    if (adding == 0)
    {
      return -0xf;
    }
    goto code_r0x026d24f0;
  case COUNTER_TIDE:
    result = get_counter_type_by_instance(instance);
    if (result == 0x6e)
    {
      result = count_counters(t_player, t_card, COUNTER_P1_P1);
      return (-(unsigned int)(result == 0) & 0x148) - 300;
    }
    goto code_r0x026d24f0;
  case COUNTER_TIME:
    if ((LEGACY_EFFECT_CUSTOM == iid) && (instance->info_slot == (int)generic_suspend_legacy))
    {
      return -0x385;
    }
    result = get_counter_type_by_instance(instance);
    if (result == 0x6f)
    {
      if (csvid == 0x227e)
      {
        return -0x96;
      }
      if (csvid < 0x227f)
      {
        if (csvid != 0x2233)
        {
          return ((csvid != 0x223c) - 1 & 0xffffdbde) + 0x238c;
        }
        return -0x96;
      }
      return ((csvid != 0x2304) - 1 & 0xffffdbde) + 0x238c;
    }
    goto code_r0x026d24f0;
  case COUNTER_TOWER:
    result = get_counter_type_by_instance(instance);
    if (result != 0x70)
    {
      return 0;
    }
    value = 30;
    break;
  case COUNTER_TRAP:
    result = is_what(t_player, t_card, 1);
    if (result != 0)
    {
      return (-(unsigned int)(adding == 0) & 100) + 1;
    }
    goto code_r0x026d24f0;
  case COUNTER_TREASURE:
    result = get_counter_type_by_instance(instance);
    if (result != 0x73)
    {
      return 0;
    }
    value = 5000;
    break;
  case COUNTER_VELOCITY:
    if (csvid != 0x943)
    {
      return 0;
    }
    value = -550;
    break;
  case COUNTER_VERSE:
    result = get_counter_type_by_instance(instance);
    if (result == 0x75)
    {
      if (csvid == 0xef3)
      {
        return 7000;
      }
      if (csvid < 0xef4)
      {
        if (csvid != 0xea3)
        {
          return 530;
        }
        result = hand_count[1 - t_player];
        limit = count_counters(t_player, t_card, COUNTER_VERSE);
        return ((result < (int)(limit + (unsigned int)(adding == 0))) - 1 & 0xffffe6ba) + 7000;
      }
      if (csvid == 0xf35)
      {
        return 7000;
      }
      if (csvid == 0xf6e)
      {
        return 7000;
      }
      return ((csvid == 0xf1a) - 1 & 0xffffe6ba) + 7000;
    }
    goto code_r0x026d24f0;
  case COUNTER_VITALITY:
    result = get_counter_type_by_instance(instance);
    if (result != 0x76)
    {
      return 0;
    }
    value = 37;
    break;
  case COUNTER_WAGE:
    result = get_counter_type_by_instance(instance);
    if (result != 0x77)
    {
      return 0;
    }
    value = -520;
    break;
  case COUNTER_WINCH:
    result = get_counter_type_by_instance(instance);
    if (result != 0x78)
    {
      return 0;
    }
    value = 310;
    break;
  case COUNTER_WIND:
    if (csvid == 0x195)
    {
      if (life[1 - player] < life[player])
      {
        value = ((t_player == player) - 1 & 0xffffffe8) + 0xc;
      }
      else
      {
        value = ((t_player != player) - 1 & 0xffffffe6) + 0xd;
      }
    }
    else
    {
      value = -13;
    }
    break;
  case COUNTER_WISH:
    result = get_counter_type_by_instance(instance);
    if (result != 0x7a)
    {
      return 0;
    }
    value = 540;
    break;
  case COUNTER_CHARGE:
    if (csvid == 0x2be0)
    {
      value = 6500;
    }
    else
    {
      if (csvid < 0x2be1)
      {
        if (csvid == 0x1adb)
        {
          return 9999;
        }
        if (csvid < 0x1adc)
        {
          if (csvid == 0x11af)
          {
            return 6500;
          }
          if (csvid == 0x1860)
          {
            return 6500;
          }
        }
        else
        {
          if (csvid == 0x2483)
          {
            return 6500;
          }
          if (csvid < 0x2484)
          {
            if (csvid == 0x1ca8)
            {
              return 6500;
            }
            if (csvid == 0x1da3)
            {
              return 6500;
            }
          }
          else
          {
            if (csvid == 0x2840)
            {
              return 6500;
            }
            if (csvid == 0x2b29)
            {
              return 6500;
            }
          }
        }
      }
      else
      {
        if (csvid == 0x39b0)
        {
          return ((t_player == player) - 1 & 0xfffffc7c) + 0x1c2;
        }
        if (csvid < 0x39b1)
        {
          if (csvid == 0x2d3f)
          {
            return 6500;
          }
          if (csvid < 0x2d40)
          {
            if (csvid == 0x2c25)
            {
              return 6500;
            }
            if (csvid == 0x2d17)
            {
              return 6500;
            }
            if (csvid == 0x2c06)
            {
              return 6500;
            }
          }
          else
          {
            if (csvid == 0x2da2)
            {
              return 6500;
            }
            if (csvid == 0x3095)
            {
              return 6500;
            }
          }
        }
      }
      result = get_counter_type_by_instance(instance);
      if (result != 0x7b)
      {
        return 0;
      }
      value = 450;
    }
    break;
  case COUNTER_CRYSTAL:
    result = get_counter_type_by_instance(instance);
    if (result != 0x7c)
    {
      return 0;
    }
    value = 441;
    break;
  case COUNTER_GEM:
    result = get_counter_type_by_instance(instance);
    if (result != 0x7d)
    {
      return 0;
    }
    value = 442;
    break;
  case COUNTER_PLOT:
    result = get_counter_type_by_instance(instance);
    if (result != 0x7f)
    {
      return 0;
    }
    value = 5002;
    break;
  case COUNTER_HOUR:
    result = get_counter_type_by_instance(instance);
    if (result != 0x80)
    {
      return 0;
    }
    value = 5003;
    break;
  case COUNTER_FURY:
    result = get_counter_type_by_instance(instance);
    if (result != 0x81)
    {
      return 0;
    }
    value = 1201;
    break;
  case COUNTER_SPITE:
    result = get_counter_type_by_instance(instance);
    if (result != 0x82)
    {
      return 0;
    }
    value = 402;
    break;
  case COUNTER_UNITY:
    result = get_counter_type_by_instance(instance);
    if (result != 0x83)
    {
      return 0;
    }
    value = 3101;
    break;
  case COUNTER_BRICK:
    result = get_counter_type_by_instance(instance);
    if (result != 0x84)
    {
      return 0;
    }
    value = 316;
    break;
  case COUNTER_GLOBE:
    value = -281;
    break;
  case COUNTER_LANDMARK:
    result = get_counter_type_by_instance(instance);
    if (result != 0x87)
    {
      return 0;
    }
    value = 1500;
    break;
  case COUNTER_PREY:
    if (adding != 0)
    {
      return 0;
    }
    value = -3001;
    break;
  case COUNTER_SLUMBER:
    result = get_counter_type_by_instance(instance);
    if (result != 0x89)
    {
      return 0;
    }
    value = -331;
    break;
  case COUNTER_KNOWLEDGE:
    result = get_counter_type_by_instance(instance);
    if (result != 0x8d)
    {
      return 0;
    }
    value = 7701;
    break;
  case COUNTER_COIN:
    if (adding != 0)
    {
      return 0;
    }
    if (t_player == player)
    {
      result = is_what(t_player, t_card, 2);
      if (result == 0)
      {
        return 0;
      }
      value = 4300;
    }
    else
    {
      result = is_what(t_player, t_card, 2);
      if (result == 0)
      {
        return 0;
      }
      value = -4300;
    }
    break;
  case COUNTER_TASK:
    result = get_counter_type_by_instance(instance);
    if (result != 0x8f)
    {
      return 0;
    }
    value = 1301;
    break;
  case COUNTER_DEATHTOUCH:
    value = 4107;
    goto joined_r0x026d233a;
  case COUNTER_DOUBLE_STRIKE:
    if (adding != 0)
    {
      return 0;
    }
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = 4108;
    break;
  case COUNTER_FIRST_STRIKE:
    if (adding != 0)
    {
      return 0;
    }
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = 2109;
    break;
  case COUNTER_FLYING:
    if (adding != 0)
    {
      return 0;
    }
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = 1808;
    break;
  case COUNTER_HEXPROOF:
    value = 4110;
    goto joined_r0x026d233a;
  case COUNTER_INDESTRUCTIBLE:
    value = 6110;
  joined_r0x026d233a:
    if (adding != 0)
    {
      return 0;
    }
    break;
  case COUNTER_LIFELINK:
    result = 0x83e;
    goto joined_r0x026d230c;
  case COUNTER_MENACE:
    result = 0x758;
  joined_r0x026d230c:
    if (adding != 0)
    {
      return 0;
    }
    return result;
  case COUNTER_REACH:
    if (adding != 0)
    {
      return 0;
    }
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return 0;
    }
    value = 908;
    break;
  case COUNTER_TRAMPLE:
    if (adding == 0)
    {
      result = is_what(t_player, t_card, 2);
      if (result != 0)
      {
        return 2107;
      }
      return 0;
    }
    goto code_r0x026d24f0;
  case COUNTER_VIGILANCE:
    if (adding == 0)
    {
      result = is_what(t_player, t_card, 2);
      if (result != 0)
      {
        return 2111;
      }
      return 0;
    }
  code_r0x026d24f0:
    value = 0;
    break;
  case COUNTER_FORESHADOW:
    result = get_counter_type_by_instance(instance);
    if (result != 0x9d)
    {
      return 0;
    }
    value = 1338;
    break;
  case COUNTER_INCARNATION:
    result = get_counter_type_by_instance(instance);
    if (result != 0x9e)
    {
      return 0;
    }
    value = -3338;
    break;
  case COUNTER_SOUL:
    if (csvid == 0x53f5)
    {
      value = -3707;
    }
    else
    {
      value = (csvid != 0x5478) - 1 & 0x517;
    }
    break;
  case COUNTER_ACORN:
    result = get_counter_type_by_instance(instance);
    if (result != 0xa0)
    {
      return 0;
    }
    value = 7300;
    break;
  case COUNTER_BOOK:
  case COUNTER_COMPONENT:
    value = 5000;
    break;
  case COUNTER_CORRUPTION:
  case COUNTER_HONE:
  case COUNTER_NIGHT:
  case COUNTER_VOW:
    value = -5000;
    break;
  case COUNTER_ENLIGHTENED:
  case COUNTER_HARMONY:
  case COUNTER_VALOR:
  case COUNTER_SHIELD:
    value = 7500;
    break;
  case COUNTER_GHOSTFORM:
    value = 500;
    break;
  case COUNTER_POINT:
    value = 9000;
    break;
  case COUNTER_VOID:
    value = 2000;
    break;
  case COUNTER_VOYAGE:
  case COUNTER_EMBER:
  case COUNTER_SUSPECT:
    value = 4500;
    break;
  case COUNTER_AEGIS:
    value = 1000;
    break;
  case COUNTER_RITUAL:
    value = 3500;
    break;
  case COUNTER_BLOODLINE:
    value = -4500;
    break;
  case COUNTER_CROAK:
  case COUNTER_HATCHING:
    value = 1000;
    break;
  case COUNTER_INVITATION:
    value = -2500;
    break;
  case COUNTER_JUDGMENT:
    value = 2500;
    break;
  case COUNTER_COLLECTION:
    value = 5000;
    break;
  case COUNTER_CONTESTED:
  case COUNTER_STASH:
    value = 3000;
  }
  return value;
}

// Returns COUNTER_invalid if cancelled, or if there are no counters.
counter_t choose_existing_counter_type(int who_chooses, int src_player, int src_card, int t_player, int t_card, cect_t mode, int smallcard_player, int smallcard_card)
{
  STATIC_ASSERT(sizeof(counter_t) >= 2, counter_t_must_be_at_least_two_bytes_wide);

  char choices[10][100];
  int i;
  for (i = 0; i < 10; ++i)
	choices[i][0] = 0;
  counter_t counter_types[10];
  int priorities[10];

  int num_types = 0;

  card_instance_t* instance = get_card_instance(t_player, t_card);
  uint8_t* t18 = (void*)(&instance->targets[18].player);

  struct
  {
	uint8_t num;
	uint8_t typ;
  } counter_data[9] =
	  {
		{ instance->special_counters, instance->special_counter_type },	// Top
		{ instance->counters2, t18[0] },	// Upper right
		{ instance->counters3, t18[1] },	// Upper center
		{ instance->counters4, t18[2] },	// Upper left
		{ instance->counters5, t18[3] },	// Lower right
		{ instance->counters, COUNTER_P1_P1 },		// Lower left
		{ instance->counters_m1m1, COUNTER_M1_M1 },	// Also lower left
		{ t18[4], t18[6] },	// undisplayed 1
		{ t18[5], t18[7] }	// undisplayed 2
	  };

  int add_remove_mode = mode & (CECT_REMOVE | CECT_ADD_OR_REMOVE | CECT_MOVE);
  ASSERT(num_bits_set(add_remove_mode) <= 1);
#define CECT_ADD ((cect_t)(1<<8))
  if (!add_remove_mode)
	add_remove_mode |= CECT_ADD;

  for (i = 0; i < 9; ++i)
	if (counter_data[i].num > 0 && counter_data[i].typ != COUNTER_invalid)
	  {
		counter_types[num_types] = counter_data[i].typ;
		scnprintf(choices[num_types], 100, "%s: %d", counter_names[counter_data[i].typ], counter_data[i].num);

		int pri_add = INT_MIN, pri_remove = INT_MIN;
		if (add_remove_mode & (CECT_ADD | CECT_ADD_OR_REMOVE))	// consider adding a counter to {t_player,t_card}
		  {
			pri_add = raw_counter_value(who_chooses, t_player, t_card, counter_data[i].typ, 1);
			if (who_chooses != t_player)
			  pri_add = -pri_add;
		  }
		if (add_remove_mode & CECT_MOVE)	// consider adding a counter to {smallcard_player,smallcard_card}
		  {
			pri_add = raw_counter_value(who_chooses, smallcard_player, smallcard_card, counter_data[i].typ, 2);
			if (who_chooses != smallcard_player)
			  pri_add = -pri_add;
		  }
		if (add_remove_mode & (CECT_REMOVE | CECT_ADD_OR_REMOVE | CECT_MOVE))	// consider removing a counter from {t_player,t_card}
		  {
			pri_remove = -raw_counter_value(who_chooses, t_player, t_card, counter_data[i].typ, 0);
			if (who_chooses != t_player)
			  pri_remove = -pri_remove;
		  }

		switch (add_remove_mode)
		  {
			case CECT_REMOVE:
			  priorities[num_types] = pri_remove;
			  break;

			case CECT_ADD_OR_REMOVE:
			  if (pri_remove > pri_add)
				{
				  priorities[num_types] = pri_remove;
				  counter_types[num_types] |= (1<<9);
				}
			  else
				priorities[num_types] = pri_add;
			  break;

			case CECT_MOVE:
			  priorities[num_types] = (pri_add - pri_remove) / 2;
			  break;

			default:	// add
			  priorities[num_types] = pri_add;
			  break;
		  }

		if (mode & CECT_CONSIDER_ALL)
		  {
			int pri = priorities[num_types];

			if (pri < 0)
			  pri = (pri / 3) + 3334;	// map raw values of [-10,000..-1] to [1..3,334]
			else if (pri > 0)
			  pri = (99 * pri) + 9999;	// map raw values of [1..10,000] to [10098..999999] (the maximum priority DIALOG() can take and still treat normally)
			else
			  pri = 5775;				// map raw value of 0 to 5,775 == sqrt(3) times as likely as raw value of -1, and 1/sqrt(3) times as likely as raw value of 1

			priorities[num_types] = pri;
		  }

		++num_types;
	  }

  if (num_types == 0)
	{
	  if (!(mode & CECT_AI_CAN_CANCEL))
		cancel = 1;
	  return COUNTER_invalid;
	}

  if (mode & CECT_AUTOCHOOSE_BEST)
	{
	  int best_pri = INT_MIN;
	  counter_t best = COUNTER_invalid;
	  for (i = 0; i < num_types; ++i)
		if (priorities[i] > best_pri)
		  {
			best_pri = priorities[i];
			best = i;
		  }

	  if (best_pri <= 0 && (mode & (IS_AI(who_chooses) ? CECT_HUMAN_CAN_CANCEL : CECT_AI_CAN_CANCEL)))
		{
		  if (!(mode & CECT_AI_CAN_CANCEL))
			cancel = 1;
		  return COUNTER_invalid;
		}
	  else
		return counter_types[best];
	}

  if (mode & CECT_AI_CAN_CANCEL)
	{
	  mode &= ~CECT_HUMAN_CAN_CANCEL;	// We'll be displaying a cancel option with this, so suppress the automatic one
	  strcpy(choices[num_types], EXE_STR(0x7281A4));	// localized Cancel
	  counter_types[num_types] = COUNTER_invalid;

	  if (mode & CECT_CONSIDER_ALL)
		priorities[num_types] = 5775;	// same as a raw value of 0 would get
	  else
		priorities[num_types] = 1;
	}

#undef ADD_TYPE

  const char* prompt;
  if (mode & CECT_MOVE)
	prompt = "Move a counter";
  else if (mode & CECT_ADD_OR_REMOVE)
	prompt = "Add or remove a counter";
  else if (mode & CECT_REMOVE)
	prompt = "Remove a counter";
  else
	prompt = "Add a counter";

  int choice;

#define ADD_CHOICE(x)	choices[x][0] ? choices[x] : NULL, 1, priorities[x]
#define ADD_CHOICES		ADD_CHOICE(0), ADD_CHOICE(1), ADD_CHOICE(2), ADD_CHOICE(3), ADD_CHOICE(4), ADD_CHOICE(5), ADD_CHOICE(6), ADD_CHOICE(7), ADD_CHOICE(8), ADD_CHOICE(9)

  // We avoid DLG_OMIT_ILLEGAL by making sure we pass NULL when we run out of choices, and not having any valid choices after invalid ones.
  choice = DIALOG(src_player, src_card, EVENT_ACTIVATE,
				  DLG_FULLCARD(t_player, t_card), DLG_SMALLCARD(smallcard_player, smallcard_card),
				  DLG_HEADER(prompt), DLG_WHO_CHOOSES(who_chooses), DLG_NO_STORAGE, DLG_RANDOM,
				  (mode & CECT_HUMAN_CAN_CANCEL ? DLG_NO_OP : DLG_NO_CANCEL),
				  (mode & CECT_AI_NO_DIALOG ? DLG_NO_DISPLAY_FOR_AI : DLG_NO_OP),
				  ADD_CHOICES);

#undef ADD_CHOICE
#undef ADD_CHOICES

  if (choice == 0 || counter_types[choice - 1] == COUNTER_invalid)
	{
	  if (!(mode & CECT_AI_CAN_CANCEL))
		cancel = 1;
	  return COUNTER_invalid;
	}
  else
	return counter_types[choice - 1];
}

void poison(int player, int number_of_counters)
{
  if (number_of_counters <= 0)
    return;
  for (int p = 0; p < 2; ++p)
    for (int c = 0; c < active_cards_count[p]; ++c)
    {
      card_instance_t *instance = in_play(p, c);
      if (!instance)
        continue;
      int csvid = cards_data[instance->internal_card_id].id;
      if (csvid == CARD_ID_WINDING_CONSTRICTOR)
        number_of_counters += player == p;
      else if (csvid == CARD_ID_SOLEMNITY)
      {
        return;
      }
    }
  int total = player_counters[player].poison_ + number_of_counters;
  if (total > 255)
    total = 255;
  player_counters[player].poison_ = total;
  if (number_of_counters > 0)
    play_sound_effect(WAV_POISON);
}

void raw_set_poison(int player, uint8_t new_number_of_counters)
{
  //if (ai_is_speculating == 1 && new_number_of_counters > player_counters[player].poison_)
  //  {
  //    player_counters[player].poison_ = new_number_of_counters;
  //    check_loss_of_game(false);
  //  }
  //else
  player_counters[player].poison_ = new_number_of_counters;
}

void add_experience(int player, int number_of_counters)
{
  if (number_of_counters <= 0)
    return;
  int total;
  for (int p = 0; p < 2; ++p)
    for (int c = 0; c < active_cards_count[p]; ++c)
    {
      card_instance_t *instance = in_play(p, c);
      if (!instance)
        continue;
      int csvid = cards_data[instance->internal_card_id].id;
      if (csvid == CARD_ID_WINDING_CONSTRICTOR)
        number_of_counters += player == p;
      else if (csvid == CARD_ID_SOLEMNITY)
      {
        number_of_counters = 0;
        goto AFTER_SCAN;
      }
    }
AFTER_SCAN:
  total = player_counters[player].experience_ + number_of_counters;
  if (total > 255)
    total = 255;
  player_counters[player].experience_ = total;
  if (ai_is_speculating != 1)
    play_sound_effect(WAV_COUNTER);
}

void raw_set_experience(int player, uint8_t new_number_of_counters)
{
  player_counters[player].experience_ = new_number_of_counters;
}

void add_energy(int player, int number_of_counters)
{
  if (number_of_counters <= 0)
    return;
  for (int p = 0; p < 2; ++p)
    for (int c = 0; c < active_cards_count[p]; ++c)
    {
      card_instance_t *instance = in_play(p, c);
      if (!instance)
        continue;
      int csvid = cards_data[instance->internal_card_id].id;
      if (csvid == CARD_ID_WINDING_CONSTRICTOR)
        number_of_counters += player == p;
      else if (csvid == CARD_ID_SOLEMNITY)
      {
        return;
      }
    }
  int total = player_counters[player].energy_ + number_of_counters;
  if (total > 255)
    total = 255;
  player_counters[player].energy_ = total;
  if (number_of_counters > 0)
  {
    play_sound_effect(WAV_ENERGY_PLUS);
    if (enable_xtrigger_flags & XTRIGGER_COUNTER_ENERGY)
    {
      int old_counters_added = counters_added;
      counters_added = number_of_counters;
      dispatch_xtrigger2(player, XTRIGGER_COUNTER_ENERGY,
                         number_of_counters > 1 ? "energy counters" : "energy counter", 0, player,
                         -1);
      counters_added = old_counters_added;
    }
  }
}

void remove_energy(int player, int number_of_counters)
{
  if (number_of_counters <= 0 || player_counters[player].energy_ == 0)
    return;
  int total = player_counters[player].energy_ - number_of_counters;
  if (total < 0)
    total = 0;
  player_counters[player].energy_ = total;
  play_sound_effect(WAV_ENERGY_MINUS);
}

int count_player_counters(int player, player_counter_names_t type){
	switch( type ){
			case PCOUNTER_ENERGY:
				return ENERGY_COUNTERS(player);
			case PCOUNTER_POISON:
				return POISON_COUNTERS(player);
			case PCOUNTER_EXPERIENCE:
				return EXPERIENCE_COUNTERS(player);
			case PCOUNTER_invalid:
				return ENERGY_COUNTERS(player)+POISON_COUNTERS(player)+EXPERIENCE_COUNTERS(player);
	}
	return 0;
}

void add_player_counters(int player, player_counter_names_t type, int amount){
	switch( type ){
			case PCOUNTER_ENERGY:
				add_energy(player, amount);
				break;
			case PCOUNTER_POISON:
				poison(player, amount);
				break;
			case PCOUNTER_EXPERIENCE:
				add_experience(player, amount);
				break;
			case PCOUNTER_invalid:
				ASSERT( 0 && "PCOUNTER_invalid not handled by 'add_player_counters'.");
				break;
	}
}

void remove_player_counters(int player, player_counter_names_t type, int amount){
	switch( type ){
			case PCOUNTER_ENERGY:
				remove_energy(player, amount);
				break;
			case PCOUNTER_POISON:
				ASSERT( 0 && "you can't remove poison counters via 'remove_player_counters'");
				break;
			case PCOUNTER_EXPERIENCE:
				ASSERT( 0 && "you can't remove experience counters via 'remove_player_counters'");
				break;
			case PCOUNTER_invalid:
				ASSERT( 0 && "PCOUNTER_invalid not handled by 'remove_player_counters'.");
				break;
	}
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int can_have_counters_placed_on_it(int player, int card);

/* ManalinkEh 2022: 0x26d44d0; original functions/counters.c:2691. */
int can_have_counters_placed_on_it(int player, int card)
{
  int result;

  if (-1 < card)
  {
    result = get_id(player, card);
    if ((result == 0x25db) || (result == 0x2cd6))
    {
      result = is_humiliated(player, card, AL_STATIC);
      return (unsigned int)(result != 0);
    }
  }
  return 1;
}

/* Counter operations carry source and recipient pairs and use the shared
 * native trigger IDs and enable masks. */
int counters_added_type = -1; /* Target .data 0x02721d68, counters.c:7. */


/* ManalinkEh 2022: 0x26d4540; original functions/counters.c:919. */
static int get_updated_counters_number_as_cost(int player, int card, counter_t type, int number)
{
  int result;
  result = can_have_counters_placed_on_it(player, card);
  if (result != 0)
  {
    if (type == COUNTER_M1_M1)
    {
      if ((-1 < card) && (result = is_what(player, card, 2), result == 0))
      {
        return number;
      }
      result = check_battlefield_for_id(player, 0x2d73);
    }
    else
    {
      if (type != COUNTER_P1_P1)
      {
        return number;
      }
      if ((-1 < card) && (result = is_what(player, card, 2), result == 0))
      {
        return number;
      }
      result = check_battlefield_for_id(1 - player, 0x4f3c);
    }
    if (result == 0)
    {
      return number;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x26d45f0; original functions/counters.c:933. */
static int get_updated_counters_number_impl(int player, int card, counter_t type, int number)
{
  uint16_t uVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  int card_00;
  int iVar5;

  iVar2 = get_updated_counters_number_as_cost(player, card, type, number);
  if ((iVar2 != 0) && ((card < 0 || (iVar5 = is_what(player, card, 0x80), iVar5 == 0))))
  {
    iVar5 = 0;
    do
    {
      card_00 = 0;
      if (0 < active_cards_count[iVar5])
      {
        do
        {
          instance = in_play(iVar5, card_00);
          if (instance != NULL)
          {
            uVar1 = cards_data[instance->internal_card_id].id;
            if (uVar1 == 0x41bc)
            {
              if (player == iVar5)
              {
                iVar4 = is_what(player, card, 0x42);
                iVar2 = (iVar2 + 1) - (unsigned int)(iVar4 == 0);
              }
            }
            else if ((unsigned short)uVar1 < 0x41bd)
            {
              if (uVar1 == 0x3541)
              {
              LAB_026d4710:
                if (type == COUNTER_P1_P1)
                {
                LAB_026d4671:
                  iVar2 = iVar2 * 2;
                }
              }
              else if ((unsigned short)uVar1 < 0x3542)
              {
                if (uVar1 == 0x1e90)
                {
                  if (player == iVar5)
                    goto LAB_026d4671;
                }
                else if (uVar1 == 0x3192)
                {
                LAB_026d46e6:
                  if ((player == iVar5) && (iVar4 = is_what(player, card, 2), iVar4 != 0))
                    goto LAB_026d4710;
                }
              }
              else if (uVar1 == 0x37c7)
              {
              LAB_026d46c8:
                if (player == iVar5)
                {
                  iVar2 = (iVar2 + 1) - (unsigned int)(type != COUNTER_P1_P1);
                }
              }
              else if (uVar1 == 0x39ba)
                goto LAB_026d4671;
            }
            else if (uVar1 == 0x48bd)
            {
              iVar2 = iVar2 + (unsigned int)(player == iVar5);
            }
            else if ((unsigned short)uVar1 < 0x48be)
            {
              if (uVar1 == 0x42a5)
              {
                if ((player == iVar5) && (type == COUNTER_M1_M1))
                {
                  iVar4 = is_what(player, card, 2);
                  iVar2 = iVar2 + -1 + (unsigned int)(iVar4 == 0);
                }
              }
              else if ((uVar1 == 0x4348) && (iVar4 = is_what(iVar5, card, 0x47), iVar4 != 0))
              {
                iVar2 = iVar2 + -99;
              }
            }
            else if (uVar1 == 0x4df1)
            {
              if (card == card_00)
                goto LAB_026d46c8;
            }
            else if (uVar1 == 0x54d2)
              goto LAB_026d46e6;
          }
          card_00 = card_00 + 1;
        } while (card_00 < active_cards_count[iVar5]);
      }
      iVar5 = iVar5 + 1;
    } while (iVar5 != 2);
  }
  return iVar2;
}

/* ManalinkEh 2022: 0x26d4810; original functions/counters.c:933. */
int get_updated_counters_number(int player, int card, counter_t type, int number)
{
  int result;

  if (0 < number)
  {
    result = get_updated_counters_number_impl(player, card, type, number);
    return result;
  }
  return number;
}

/* ManalinkEh 2022: 0x26d0e90; original functions/counters.c:1012. */
static void add_or_remove_counters_impl(int player, int card, int t_player, int t_card,
                                                    counter_t type, int num)
{
  uint16_t uVar1;
  bool bVar2;
  int iVar3;
  unsigned char bVar4;
  uint8_t uVar5;
  card_instance_t *instance;
  counter_t cVar6;
  const char *pcVar7;
  int iVar8;
  uint8_t *label;
  int iStack_1c;
  uint8_t *position;
  iVar3 = counters_added;
  iVar8 = counters_added_type;
  if ((0 < num) && (((uint8_t)enable_xtrigger_flags & ENABLE_XTRIGGER_COUNTERS) !=
                    ENABLE_XTRIGGER_0))
  {
    counters_added = num;
    counters_added_type = type;
    pcVar7 = "counters";
    if (num < 2)
    {
      pcVar7 = "counter";
    }
    dispatch_xtrigger2(t_player, XTRIGGER_REPLACE_COUNTERS, pcVar7, 0, t_player, t_card);
  }
  counters_added_type = iVar8;
  counters_added = iVar3;
  instance = get_card_instance(t_player, t_card);
  label = (uint8_t *)(instance->targets + 0x12);
  switch (type)
  {
  case COUNTER_P1_P1:
    position = (uint8_t *)&instance->counters;
    iStack_1c = 1;
    goto code_r0x026d1118;
  case COUNTER_M1_M1:
    position = (uint8_t *)&instance->counters_m1m1;
    iStack_1c = -1;
  code_r0x026d1118:
    label = NULL;
    bVar4 = *position;
    iVar8 = iStack_1c;
    goto LAB_026d1122;
  case COUNTER_P0_P1:
    iVar8 = 1;
    iStack_1c = 0;
    break;
  case COUNTER_P0_P2:
    iVar8 = 2;
    iStack_1c = 0;
    break;
  case COUNTER_P1_P0:
    iVar8 = 0;
    iStack_1c = 1;
    break;
  case COUNTER_P1_P2:
    iVar8 = 2;
    iStack_1c = 1;
    break;
  case COUNTER_P2_P0:
    iVar8 = 0;
    iStack_1c = 2;
    break;
  case COUNTER_P2_P2:
    iVar8 = 2;
    iStack_1c = 2;
    break;
  case COUNTER_M0_M1:
    iVar8 = -1;
    iStack_1c = 0;
    break;
  case COUNTER_M0_M2:
    iVar8 = -2;
    iStack_1c = 0;
    break;
  case COUNTER_M1_M0:
    iVar8 = 0;
    iStack_1c = -1;
    break;
  case COUNTER_M2_M1:
    iVar8 = -1;
    iStack_1c = -2;
    break;
  case COUNTER_M2_M2:
    iVar8 = -2;
    iStack_1c = -2;
    break;
  default:
    iVar8 = 0;
    iStack_1c = 0;
  }
  bVar4 = instance->special_counter_type;
  position = (uint8_t *)instance;
  if (type == bVar4)
  {
    if (instance->special_counters == '\0')
    {
      instance->special_counter_type = 0xff;
      goto LAB_026d0f68;
    }
    label = (uint8_t *)&instance->special_counter_type;
  LAB_026d1018:
    if (bVar4 != 0xff)
    {
      bVar4 = *position;
      if (num < 1)
        goto LAB_026d102d;
    LAB_026d112a:
      if ((int)((unsigned int)bVar4 + num) < 0x100)
      {
        *position = bVar4 + (char)num;
        if (label == NULL)
          goto LAB_026d1340;
      }
      else
      {
        num = 0xff - (unsigned int)bVar4;
        *position = bVar4 + (char)num;
        if (label == NULL)
          goto LAB_026d115d;
      }
      *(uint8_t *)&*label = (uint8_t)type;
      goto LAB_026d115d;
    }
  LAB_026d1308:
    *position = '\0';
    bVar4 = 0;
  LAB_026d1122:
    if (0 < num)
      goto LAB_026d112a;
  LAB_026d102d:
    if (-1 < num)
    {
    LAB_026d1033:
      uVar5 = bVar4 + (char)num;
      *position = uVar5;
      if (label == NULL)
        goto LAB_026d115d;
    LAB_026d1042:
      if ((uVar5 == '\0') &&
          ((*(uint8_t *)&*label = 0xff,
            label == (uint8_t *)&instance->special_counter_type ||
                (label <= (uint8_t *)((int)&instance->targets[0x12].player + 3U)))))
      {
        uVar5 = (uint8_t)instance->targets[0x12].card;
        if ((uVar5 == '\0') || (*(char *)((int)&instance->targets[0x12].card + 2) == -1))
        {
          uVar5 = *(uint8_t *)((int)&instance->targets[0x12].card + 1);
          if ((uVar5 != '\0') && (*(char *)((int)&instance->targets[0x12].card + 3) != -1))
          {
            *position = uVar5;
            *(uint8_t *)&*label = *(uint8_t *)((int)&instance->targets[0x12].card + 3);
            *(uint8_t *)((int)&instance->targets[0x12].card + 1) = 0;
            *(uint8_t *)((int)&instance->targets[0x12].card + 3) = 0xff;
          }
        }
        else
        {
          *position = uVar5;
          *(uint8_t *)&*label = *(uint8_t *)((int)&instance->targets[0x12].card + 2);
          *(uint8_t *)&instance->targets[0x12].card = 0;
          *(uint8_t *)((int)&instance->targets[0x12].card + 2) = 0xff;
        }
      }
      goto LAB_026d115d;
    }
    if ((int)((unsigned int)bVar4 + num) < 0)
    {
      num = -(unsigned int)bVar4;
      goto LAB_026d1033;
    }
    uVar5 = bVar4 + (char)num;
    *position = uVar5;
    if (label != NULL)
      goto LAB_026d1042;
  LAB_026d1340:
    if (iStack_1c != 0)
    {
      instance->counter_power = instance->counter_power + (short)iStack_1c * (short)num;
      instance->regen_status = instance->regen_status | 0x4000000;
    }
    if (iVar8 != 0)
    {
      uVar1 = instance->counter_toughness;
      instance->regen_status = instance->regen_status | 0x2000000;
      instance->counter_toughness = uVar1 + (short)iVar8 * (short)num;
    }
    if (0 < num)
    {
      if (hack_silent_counters < 1)
      {
        play_sound_effect(WAV_COUNTER);
      }
      iVar3 = counters_added;
      iVar8 = counters_added_type;
      counters_added_type = type;
      counters_added = num;
      dispatch_event_with_attacker(t_player, t_card, EVENT_COUNTERS, player, card);
      counters_added_type = iVar8;
      counters_added = iVar3;
      if (type == COUNTER_P1_P1)
      {
        iVar8 = is_what(t_player, t_card, 0x47);
        if (iVar8 != 0)
        {
          set_trap_condition(t_player, 0x607, 1);
        }
        iVar8 = counters_added;
        if (((uint8_t)enable_xtrigger_flags & ENABLE_XTRIGGER_1_1_COUNTERS) ==
            ENABLE_XTRIGGER_0)
        {
          return;
        }
        pcVar7 = "+1/+1 counters";
        if (num < 2)
        {
          pcVar7 = "+1/+1 counter";
        }
        counters_added = num;
        dispatch_xtrigger2(t_player, XTRIGGER_1_1_COUNTERS, pcVar7, 0, t_player, t_card);
        counters_added = iVar8;
        return;
      }
      bVar2 = true;
      goto LAB_026d11b8;
    }
  }
  else
  {
  LAB_026d0f68:
    bVar4 = (unsigned char)instance->targets[0x12].player;
    if (type == bVar4)
    {
      if (instance->counters2 != '\0')
      {
        position = (uint8_t *)&instance->counters2;
        goto LAB_026d1018;
      }
      *(uint8_t *)&instance->targets[0x12].player = 0xff;
    }
    bVar4 = *(unsigned char *)((int)&instance->targets[0x12].player + 1);
    if (type == bVar4)
    {
      if (instance->counters3 != '\0')
      {
        label = (uint8_t *)((int)&instance->targets[0x12].player + 1);
        position = (uint8_t *)&instance->counters3;
        goto LAB_026d1018;
      }
      *(uint8_t *)((int)&instance->targets[0x12].player + 1) = 0xff;
    }
    bVar4 = *(unsigned char *)((int)&instance->targets[0x12].player + 2);
    if (type == bVar4)
    {
      if (instance->counters4 != '\0')
      {
        label = (uint8_t *)((int)&instance->targets[0x12].player + 2);
        position = (uint8_t *)&instance->counters4;
        goto LAB_026d1018;
      }
      *(uint8_t *)((int)&instance->targets[0x12].player + 2) = 0xff;
    }
    bVar4 = *(unsigned char *)((int)&instance->targets[0x12].player + 3);
    if (type == bVar4)
    {
      if (instance->counters5 != '\0')
      {
        label = (uint8_t *)((int)&instance->targets[0x12].player + 3);
        position = (uint8_t *)&instance->counters5;
        goto LAB_026d1018;
      }
      *(uint8_t *)((int)&instance->targets[0x12].player + 3) = 0xff;
    }
    bVar4 = *(unsigned char *)((int)&instance->targets[0x12].card + 2);
    if (type == bVar4)
    {
      if ((char)instance->targets[0x12].card != '\0')
      {
        label = (uint8_t *)((int)&instance->targets[0x12].card + 2);
        position = (uint8_t *)&instance->targets[0x12].card;
        goto LAB_026d1018;
      }
      *(uint8_t *)((int)&instance->targets[0x12].card + 2) = 0xff;
    }
    bVar4 = *(unsigned char *)((int)&instance->targets[0x12].card + 3);
    if (type == bVar4)
    {
      if (*(char *)((int)&instance->targets[0x12].card + 1) != '\0')
      {
        label = (uint8_t *)((int)&instance->targets[0x12].card + 3);
        position = (uint8_t *)((int)&instance->targets[0x12].card + 1);
        goto LAB_026d1018;
      }
      *(uint8_t *)((int)&instance->targets[0x12].card + 3) = 0xff;
    }
    if (num < 0)
    {
      return;
    }
    uVar5 = instance->special_counters;
    if ((uVar5 == '\0') && (cVar6 = get_counter_type_by_instance(instance), type == cVar6))
    {
    LAB_026d1009:
      label = (uint8_t *)&instance->special_counter_type;
      bVar4 = instance->special_counter_type;
      goto LAB_026d1018;
    }
    if (instance->counters2 == '\0')
    {
      bVar4 = (unsigned char)instance->targets[0x12].player;
      position = (uint8_t *)&instance->counters2;
      goto LAB_026d1018;
    }
    if (instance->counters3 == '\0')
    {
      label = (uint8_t *)((int)&instance->targets[0x12].player + 1);
      bVar4 = *(unsigned char *)((int)&instance->targets[0x12].player + 1);
      position = (uint8_t *)&instance->counters3;
      goto LAB_026d1018;
    }
    if (instance->counters4 == '\0')
    {
      label = (uint8_t *)((int)&instance->targets[0x12].player + 2);
      bVar4 = *(unsigned char *)((int)&instance->targets[0x12].player + 2);
      position = (uint8_t *)&instance->counters4;
      goto LAB_026d1018;
    }
    if (instance->counters5 == '\0')
    {
      label = (uint8_t *)((int)&instance->targets[0x12].player + 3);
      bVar4 = *(unsigned char *)((int)&instance->targets[0x12].player + 3);
      position = (uint8_t *)&instance->counters5;
      goto LAB_026d1018;
    }
    if (uVar5 == '\0')
      goto LAB_026d1009;
    if (*(char *)((int)&instance->targets[0x12].card + 2) == -1)
    {
      position = (uint8_t *)&instance->targets[0x12].card;
      label = (uint8_t *)((int)&instance->targets[0x12].card + 2);
      goto LAB_026d1308;
    }
    if (*(char *)((int)&instance->targets[0x12].card + 3) == -1)
    {
      position = (uint8_t *)((int)&instance->targets[0x12].card + 1);
      label = (uint8_t *)((int)&instance->targets[0x12].card + 3);
      goto LAB_026d1308;
    }
  LAB_026d115d:
    if (num != 0)
      goto LAB_026d1340;
  }
  iVar3 = counters_added;
  iVar8 = counters_added_type;
  counters_added_type = type;
  counters_added = num;
  dispatch_event_with_attacker(t_player, t_card, EVENT_COUNTERS, player, card);
  bVar2 = 0 < num;
  counters_added_type = iVar8;
  counters_added = iVar3;
LAB_026d11b8:
  iVar8 = counters_added;
  if (((type == COUNTER_M1_M1) && (bVar2)) &&
      (((uint8_t)enable_xtrigger_flags & ENABLE_XTRIGGER_M1_M1_COUNTERS) != ENABLE_XTRIGGER_0))
  {
    pcVar7 = "-1/-1 counters";
    if (num < 2)
    {
      pcVar7 = "-1/-1 counter";
    }
    counters_added = num;
    dispatch_xtrigger2(t_player, XTRIGGER_M1_M1_COUNTERS, pcVar7, 0, t_player, t_card);
  }
  counters_added = iVar8;
  return;
}

/* ManalinkEh 2022: 0x26d4840; original functions/counters.c:1266. */
void add_counters(int player, int card, int t_player, int t_card, counter_t type,
                              int num)
{
  card_instance_t *instance;
  int num_00;

  instance = get_card_instance(t_player, t_card);
  if (instance->internal_card_id == activation_card)
  {
    t_player = instance->parent_controller;
    t_card = instance->parent_card;
  }
  if (0 < num)
  {
    num_00 = get_updated_counters_number(t_player, t_card, type, num);
    if (0 < num_00)
    {
      add_or_remove_counters_impl(player, card, t_player, t_card, type, num_00);
      return;
    }
  }
  return;
}

/* Recovered October 2022 source-aware counter removal. */

/* ManalinkEh 2022: 0x26ceef0; original functions/counters.c:1630. */
static void clear_all_counters(int t_player, int t_card)
{
  unsigned int uVar1;
  card_instance_t *instance;

  instance = get_card_instance(t_player, t_card);
  uVar1 = instance->regen_status;
  instance->special_counter_type = 0xff;
  instance->special_counters = '\0';
  instance->counters2 = '\0';
  instance->counters3 = '\0';
  instance->counters4 = '\0';
  instance->counters = '\0';
  instance->counters5 = '\0';
  instance->counters_m1m1 = '\0';
  instance->targets[0x12].player = -1;
  instance->targets[0x12].card = -0x10000;
  instance->counter_power = 0;
  instance->counter_toughness = 0;
  instance->regen_status = uVar1 | 0x6000000;
  return;
}

/* ManalinkEh 2022: 0x26d48d0; original functions/counters.c:1304. */
static void remove_counters_impl(int player, int card, int t_player, int t_card,
                                                  counter_t type, int num)
{
  card_instance_t *instance;
  int iVar2;
  int32_t iVar3;
  int iVar4;
  int iStack_70;
  target_definition_t td;
  token_generation_t token;

  instance = get_card_instance(t_player, t_card);
  if (instance->internal_card_id == activation_card)
  {
    t_player = instance->parent_controller;
    t_card = instance->parent_card;
  }
  iStack_70 = count_counters(t_player, t_card, type);
  add_or_remove_counters_impl(player, card, t_player, t_card, type, -num);
  if ((type == COUNTER_TIME) && (iVar2 = is_suspend_legacy(t_player, t_card), iVar2 != 0))
  {
    if (num < iStack_70)
    {
      iStack_70 = num;
    }
    iVar3 = instance->targets[9].card;
    if (iVar3 == 0x2221)
    {
      draw_cards(t_player, iStack_70);
      iVar3 = instance->targets[9].card;
    }
    if (iVar3 == 0x2228)
    {
      default_token_definition(t_player, t_card, 0x4986, &token);
      token.qty = (uint8_t)iStack_70;
      generate_token(&token);
      iVar3 = instance->targets[9].card;
    }
    if (iVar3 == 0x223e)
    {
      default_target_definition(t_player, t_card, &td, 1);
      td.required_subtype = 0x2000;
      td.special = 0x1000;
      td.allow_cancel = 0;
      if (0 < iStack_70)
      {
        iVar2 = 0;
        do
        {
          instance->number_of_targets = '\0';
          iVar4 = can_target(&td);
          if ((iVar4 != 0) &&
              (iVar4 = new_pick_target(&td, "Select target nonbasic land.", 0, 0x80), iVar4 != 0))
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
          }
          iVar2 = iVar2 + 1;
        } while (iStack_70 != iVar2);
      }
      iVar3 = instance->targets[9].card;
    }
    if (iVar3 == 0x224f)
    {
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = player;
      if (0 < iStack_70)
      {
        iVar2 = 0;
        do
        {
          instance->number_of_targets = '\0';
          iVar4 = can_target(&td);
          if ((iVar4 != 0) && (iVar4 = pick_target(&td, "TARGET_CREATURE"), iVar4 != 0))
          {
            add_counters(t_player, t_card, instance->targets[0].player,
                                     instance->targets[0].card, COUNTER_P1_P1, 1);
          }
          iVar2 = iVar2 + 1;
        } while (iStack_70 != iVar2);
      }
      iVar3 = instance->targets[9].card;
    }
    if (iVar3 == 0x2287)
    {
      iVar2 = 0;
      default_target_definition(player, card, &td, 2);
      td.zone = 0x1000;
      td.allow_cancel = 0;
      if (0 < iStack_70)
      {
        do
        {
          instance->number_of_targets = '\0';
          iVar4 = can_target(&td);
          if ((iVar4 != 0) && (iVar4 = pick_target(&td, "TARGET_PLAYER"), iVar4 != 0))
          {
            lose_life(instance->targets[0].player, 1);
            gain_life(player, 1);
          }
          iVar2 = iVar2 + 1;
        } while (iStack_70 != iVar2);
      }
    }
  }
  return;
}

/* ManalinkEh 2022: 0x26d4c10; original functions/counters.c:1304. */
void remove_counters(int player, int card, int t_player, int t_card, counter_t type,
                                 int num)
{
  if (num < 1)
  {
    return;
  }
  remove_counters_impl(player, card, t_player, t_card, type, num);
  return;
}

/* ManalinkEh 2022: 0x26d4c50; original functions/counters.c:1630. */
void remove_all_counters(int player, int card, int t_player, int t_card, counter_t type)
{
  if ((char)type != -1)
  {
    remove_counters_impl(player, card, t_player, t_card, type, 0xff);
    return;
  }
  clear_all_counters(t_player, t_card);
  return;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
void quarry_hauler_helper(int player, int card, int t_player, int t_card);

/* ManalinkEh 2022: 0x26d51e0; original functions/counters.c:2412. */
void quarry_hauler_helper(int player, int card, int t_player, int t_card)
{
  char cVar1;
  card_instance_t *instance;
  uint32_t uVar3;
  int iVar4;
  int iVar5;
  counter_t cVar6;
  int iVar7;
  uint32_t uVar8;
  int iVar9;
  unsigned int uVar10;
  unsigned int uVar11;
  uint32_t uVar12;
  int *piVar13;
  char (*pacVar14)[100];
  unsigned char bVar15;
  counter_t cVar16;
  uint32_t uVar17;
  char (*pacVar18)[100];
  uint32_t uVar19;
  char (*pacVar20)[100];
  uint32_t uVar21;
  char (*pacVar22)[100];
  uint32_t uVar23;
  char (*pacVar24)[100];
  uint32_t uVar25;
  uint32_t uVar26;
  uint32_t uVar27;
  uint32_t uVar28;
  uint32_t uVar29;
  uint32_t uVar30;
  unsigned int uStack_3fc;
  uint32_t uStack_3f8;
  uint32_t uStack_3f4;
  uint32_t uStack_3f0;
  uint32_t uStack_3ec;
  int iStack_3e8;
  unsigned int uStack_3e4;
  counter_t counter_types[9];
  int choices[9];
  char ctr_names[9][100];

  bVar15 = 0;
  cVar16 = t_player;
  iVar4 = t_card;
  instance = get_card_instance(t_player, t_card);
  if (instance->internal_card_id == activation_card)
  {
    iVar4 = instance->parent_card;
    cVar16 = instance->parent_controller;
    instance = get_card_instance(cVar16, iVar4);
  }
  uVar10 = 0;
  if (instance->special_counters != '\0')
  {
    counter_types[0] = (counter_t)instance->special_counter_type;
    uVar10 = (unsigned int)(counter_types[0] != COUNTER_invalid);
  }
  if (instance->counters2 != '\0')
  {
    cVar6 = (counter_t)(unsigned char)instance->targets[0x12].player;
    counter_types[uVar10] = cVar6;
    uVar10 = uVar10 + (cVar6 != COUNTER_invalid);
  }
  if (instance->counters3 != '\0')
  {
    cVar6 = (counter_t) * (unsigned char *)((int)&instance->targets[0x12].player + 1);
    counter_types[uVar10] = cVar6;
    uVar10 = uVar10 + (cVar6 != COUNTER_invalid);
  }
  if (instance->counters4 != '\0')
  {
    cVar6 = (counter_t) * (unsigned char *)((int)&instance->targets[0x12].player + 2);
    counter_types[uVar10] = cVar6;
    uVar10 = uVar10 + (cVar6 != COUNTER_invalid);
  }
  if (instance->counters5 != '\0')
  {
    cVar6 = (counter_t) * (unsigned char *)((int)&instance->targets[0x12].player + 3);
    counter_types[uVar10] = cVar6;
    uVar10 = uVar10 + (cVar6 != COUNTER_invalid);
  }
  if (instance->counters != '\0')
  {
    counter_types[uVar10] = COUNTER_P1_P1;
    uVar10 = uVar10 + 1;
  }
  if (instance->counters_m1m1 != '\0')
  {
    counter_types[uVar10] = COUNTER_M1_M1;
    uVar10 = uVar10 + 1;
  }
  cVar1 = *(char *)((int)&instance->targets[0x12].card + 1);
  if (((char)instance->targets[0x12].card == '\0') ||
      (cVar6 = (counter_t) * (unsigned char *)((int)&instance->targets[0x12].card + 2),
       counter_types[uVar10] = cVar6, cVar6 == COUNTER_invalid))
  {
    if ((cVar1 == '\0') ||
        (cVar6 = (counter_t) * (unsigned char *)((int)&instance->targets[0x12].card + 3),
         counter_types[uVar10] = cVar6, cVar6 == COUNTER_invalid))
    {
      uVar11 = uVar10;
      if (uVar10 == 0)
      {
        return;
      }
    }
    else
    {
      uVar11 = uVar10 + 1;
    }
  }
  else
  {
    uVar11 = uVar10 + 1;
    if ((cVar1 != '\0') &&
        (cVar6 = (counter_t) * (unsigned char *)((int)&instance->targets[0x12].card + 3),
         counter_types[uVar11] = cVar6, cVar6 != COUNTER_invalid))
    {
      uVar11 = uVar10 + 2;
    }
  }
  iVar9 = 0;
  piVar13 = choices;
  for (iVar7 = 9; iVar7 != 0; iVar7 = iVar7 + -1)
  {
    *piVar13 = 0;
    piVar13 = piVar13 + (unsigned int)bVar15 * -2 + 1;
  }
  pacVar14 = ctr_names;
  do
  {
    iVar9 = iVar9 + 1;
    get_counter_name_by_type(*pacVar14, cVar16, iVar4);
    pacVar14 = pacVar14 + 1;
  } while (iVar9 < (int)uVar11);
  uStack_3fc = uVar11;
  if (uVar11 == 0)
  {
    uStack_3fc = 1;
  }

  iStack_3e8 = 0;
  uStack_3e4 = uVar11;
  if (uVar11 < 9)
    goto LAB_026d5692;
LAB_026d53b8:
  uVar10 = (unsigned int)(choices[8] == 0);
LAB_026d53c5:
  if (choices[7] != 0)
    goto LAB_026d56a1;
  uStack_3ec = 1;
LAB_026d53dd:
  if (choices[6] != 0)
    goto LAB_026d56b8;
  uStack_3f0 = 1;
LAB_026d53f5:
  if (choices[5] != 0)
    goto LAB_026d56cf;
  uStack_3f4 = 1;
LAB_026d540d:
  if (choices[4] != 0)
    goto LAB_026d56e6;
  uVar12 = 1;
LAB_026d5420:
  if (choices[3] != 0)
    goto LAB_026d56f5;
  uVar8 = 1;
LAB_026d5433:
  if (choices[2] != 0)
    goto LAB_026d5704;
  uStack_3f8 = 1;
  do
  {
    uVar3 = 1;
    if (choices[1] != 0)
      goto LAB_026d571b;
    while (true)
    {
      pacVar14 = ctr_names + 8;
      pacVar24 = ctr_names + 7;
      pacVar22 = ctr_names + 6;
      pacVar20 = ctr_names + 5;
      pacVar18 = ctr_names + 4;
      uVar30 = 0;
      uVar29 = 0;
      uVar28 = 0;
      uVar27 = 0;
      uVar26 = 1;
      uVar25 = 1;
      uVar23 = 1;
      uVar21 = 1;
      uVar19 = 1;
      uVar17 = 1;
      iVar9 = dialog_impl(player, card, EVENT_RESOLVE_ACTIVATION, "\001\002", "\001\003",
                          "\001\014", "\001\004", "\001\006", t_player, t_card, "\001\026",
                          ctr_names, (unsigned int)(iStack_3e8 == 0), 1, ctr_names + 1, uVar3, 1,
                          ctr_names + 2, uStack_3f8, 1, ctr_names + 3, uVar8, 1, pacVar18, uVar12,
                          1, pacVar20, uStack_3f4, 1, pacVar22, uStack_3f0, 1, pacVar24, uStack_3ec,
                          1, pacVar14, uVar10, 1, 0, 0, 0, 0);
      iVar7 = 1;
      iVar4 = 1;
      if (player == 1)
      {

        cVar16 = counter_types[uStack_3fc];
        iVar4 = raw_counter_value(1, t_player, t_card, cVar16, 1);
        iVar5 = raw_counter_value(1, t_player, t_card, cVar16, 0);
        if (iVar4 < 0)
        {
          iVar7 = iVar4 / 3 + 0xd06;
        }
        else if (iVar4 < 1)
        {
          iVar7 = 0x168f;
        }
        else
        {
          iVar7 = iVar4 * 99 + 9999;
        }
        if (iVar5 < 0)
        {
          iVar4 = iVar5 / 3 + 0xd06;
        }
        else
        {
          iVar4 = 0x168f;
          if (0 < iVar5)
          {
            iVar4 = iVar5 * 99 + 9999;
          }
        }
        uStack_3fc = 2;
      }
      iVar4 =
          dialog_impl(player, card, EVENT_RESOLVE_ACTIVATION, "\001\002", "\001\003", "\001\014",
                      "\001\006", t_player, t_card, "\001\011", ctr_names + iVar9 + -1, "\001\026",
                      "Add", 1, iVar7, "Remove", 1, iVar4, 0, 0, 0, 0, uVar17, pacVar18, uVar12,
                      uVar19, pacVar20, uStack_3f4, uVar21, pacVar22, uStack_3f0, uVar23, pacVar24,
                      uStack_3ec, uVar25, pacVar14, uVar10, uVar26, uVar27, uVar28, uVar29, uVar30);
      choices[iVar9 + -1] = iVar4;
      uStack_3e4 = uStack_3e4 - 1;
      if ((int)uStack_3e4 < 1)
      {
        iVar4 = 0;
        do
        {
          if (choices[iVar4] == 1)
          {
            add_counters(player, card, t_player, t_card, counter_types[iVar4], 1);
          }
          else if (choices[iVar4] == 2)
          {
            remove_counters_impl(player, card, t_player, t_card,
                                                  counter_types[iVar4], 1);
          }
          iVar4 = iVar4 + 1;
        } while (iVar4 < (int)uVar11);
        return;
      }
      iStack_3e8 = choices[0];
      if (8 < uVar11)
        goto LAB_026d53b8;
    LAB_026d5692:
      uVar10 = 0;
      if (7 < uVar11)
        goto LAB_026d53c5;
    LAB_026d56a1:
      uStack_3ec = 0;
      if (6 < uVar11)
        goto LAB_026d53dd;
    LAB_026d56b8:
      uStack_3f0 = 0;
      if (5 < uVar11)
        goto LAB_026d53f5;
    LAB_026d56cf:
      uStack_3f4 = 0;
      if (4 < uVar11)
        goto LAB_026d540d;
    LAB_026d56e6:
      uVar12 = 0;
      if (3 < uVar11)
        goto LAB_026d5420;
    LAB_026d56f5:
      uVar8 = 0;
      if (2 < uVar11)
        goto LAB_026d5433;
    LAB_026d5704:
      uStack_3f8 = 0;
      if (1 < uVar11)
        break;
    LAB_026d571b:
      uVar3 = 0;
    }
  } while (true);
}

void copy_counters(int player, int card, int t_player, int t_card, int src_player,
                               int src_card, int raw);


/* ManalinkEh 2022: 0x26d4ca0; original functions/counters.c:1385. */
void copy_counters(int player, int card, int t_player, int t_card, int src_player,
                               int src_card, int raw)
{
  typedef struct
  {
    counter_t typ;
    int num;
  } counter_copy_entry;

  unsigned char bVar1;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  counter_t cVar4;
  int iVar5;
  counter_copy_entry *paVar6;
  counter_copy_entry *paVar7;
  int iVar8;
  counter_copy_entry counters[9];

  pcVar2 = get_card_instance(src_player, src_card);
  if (pcVar2->internal_card_id == activation_card)
  {
    pcVar2 = get_card_instance(pcVar2->parent_controller, pcVar2->parent_card);
  }
  pcVar3 = get_card_instance(t_player, t_card);
  if (pcVar3->internal_card_id == activation_card)
  {
    t_player = pcVar3->parent_controller;
    t_card = pcVar3->parent_card;
  }
  if ((pcVar2->special_counters == 0) ||
      (counters[0].typ = (unsigned int)pcVar2->special_counter_type,
       pcVar2->special_counter_type == COUNTER_invalid))
  {
    bVar1 = pcVar2->counters2;
    iVar8 = 0;
    if (bVar1 != 0)
      goto LAB_026d4e85;
  }
  else
  {
    iVar8 = 1;
    counters[0].num = (unsigned int)pcVar2->special_counters;
    bVar1 = pcVar2->counters2;
    if (bVar1 == 0)
    {
      iVar8 = 1;
    }
    else
    {
    LAB_026d4e85:
      cVar4 = (counter_t)(unsigned char)pcVar2->targets[0x12].player;
      counters[iVar8].typ = cVar4;
      if (cVar4 != COUNTER_invalid)
      {
        counters[iVar8].num = (unsigned int)bVar1;
        iVar8 = iVar8 + 1;
      }
    }
  }
  bVar1 = pcVar2->counters3;
  if ((bVar1 != 0) &&
      (cVar4 = (counter_t) * (unsigned char *)((int)&pcVar2->targets[0x12].player + 1),
       counters[iVar8].typ = cVar4, cVar4 != COUNTER_invalid))
  {
    counters[iVar8].num = (unsigned int)bVar1;
    iVar8 = iVar8 + 1;
  }
  bVar1 = pcVar2->counters4;
  if ((bVar1 != 0) &&
      (cVar4 = (counter_t) * (unsigned char *)((int)&pcVar2->targets[0x12].player + 2),
       counters[iVar8].typ = cVar4, cVar4 != COUNTER_invalid))
  {
    counters[iVar8].num = (unsigned int)bVar1;
    iVar8 = iVar8 + 1;
  }
  bVar1 = pcVar2->counters5;
  if ((bVar1 != 0) &&
      (cVar4 = (counter_t) * (unsigned char *)((int)&pcVar2->targets[0x12].player + 3),
       counters[iVar8].typ = cVar4, cVar4 != COUNTER_invalid))
  {
    counters[iVar8].num = (unsigned int)bVar1;
    iVar8 = iVar8 + 1;
  }
  bVar1 = pcVar2->counters;
  if (bVar1 != 0)
  {
    counters[iVar8].typ = COUNTER_P1_P1;
    counters[iVar8].num = (unsigned int)bVar1;
    iVar8 = iVar8 + 1;
  }
  bVar1 = pcVar2->counters_m1m1;
  if (bVar1 != 0)
  {
    counters[iVar8].typ = COUNTER_M1_M1;
    counters[iVar8].num = (unsigned int)bVar1;
    iVar8 = iVar8 + 1;
  }
  bVar1 = (unsigned char)pcVar2->targets[0x12].card;
  if ((bVar1 != 0) &&
      (cVar4 = (counter_t) * (unsigned char *)((int)&pcVar2->targets[0x12].card + 2),
       counters[iVar8].typ = cVar4, cVar4 != COUNTER_invalid))
  {
    counters[iVar8].num = (unsigned int)bVar1;
    iVar8 = iVar8 + 1;
  }
  bVar1 = *(unsigned char *)((int)&pcVar2->targets[0x12].card + 1);
  if ((bVar1 == 0) ||
      (cVar4 = (counter_t) * (unsigned char *)((int)&pcVar2->targets[0x12].card + 3),
       counters[iVar8].typ = cVar4, cVar4 == COUNTER_invalid))
  {
    if (raw == 0)
    {
      if (iVar8 == 0)
      {
        return;
      }
      goto LAB_026d4ebb;
    }
    if (iVar8 == 0)
    {
      return;
    }
  }
  else
  {
    counters[iVar8].num = (unsigned int)bVar1;
    iVar8 = iVar8 + 1;
    if (raw == 0)
    {
    LAB_026d4ebb:
      paVar6 = counters;
      do
      {
        paVar7 = paVar6 + 1;
        add_counters(player, card, t_player, t_card, paVar6->typ, paVar6->num);
        paVar6 = paVar7;
      } while (paVar7 != counters + iVar8);
      return;
    }
  }
  iVar5 = 0;
  do
  {
    if (counters[iVar5].num != 0)
    {
      add_or_remove_counters_impl(player, card, t_player, t_card, counters[iVar5].typ,
                                              counters[iVar5].num);
    }
    iVar5 = iVar5 + 1;
  } while (iVar5 < iVar8);
  return;
}

/* ManalinkEh 2022: 0x26d5970; original functions/counters.c:1281. */
void add_counters_predoubled(int player, int card, int t_player, int t_card,
                                         counter_t type, int num)
{
  int result;
  result = is_what(t_player, t_card, 0x80);
  if (result != 0)
  {
    backtrace_and_abort("functions/counters.c:1283\n!is_what(t_player, t_card, TYPE_EFFECT)");
  }
  result = can_have_counters_placed_on_it(t_player, t_card);
  if ((result != 0) && (num != 0))
  {
    add_or_remove_counters_impl(player, card, t_player, t_card, type, num);
    return;
  }
  return;
}


/* ManalinkEh 2022: 0x26d5a00; original functions/counters.c:1437. */
void move_counters(int player, int card, int t_player, int t_card, int src_player,
                               int src_card, counter_t counter_type, int number)
{
  uint8_t uVar1;
  int result;
  card_instance_t *instance;
  char cStack_14;
  if ((t_card != src_card) || (t_player != src_player))
  {
    result = can_have_counters_placed_on_it(t_player, t_card);
    if (result != 0)
    {
      cStack_14 = (char)counter_type;
      if (cStack_14 == -1)
      {
        result = check_battlefield_for_id(t_player, 0x2d73);
        if (result != 0)
        {
          instance = get_card_instance(src_player, src_card);
          uVar1 = instance->counters_m1m1;
          instance->counters_m1m1 = '\0';
          hack_silent_counters = hack_silent_counters + 1;
          copy_counters(player, card, t_player, t_card, src_player, src_card, 1);
          clear_all_counters(src_player, src_card);
          hack_silent_counters = hack_silent_counters + -1;
          instance->counters_m1m1 = uVar1;
        }
        result = check_battlefield_for_id(1 - t_player, 0x4f3c);
        if (result == 0)
        {
          hack_silent_counters = hack_silent_counters + 1;
          copy_counters(player, card, t_player, t_card, src_player, src_card, 1);
          clear_all_counters(src_player, src_card);
          hack_silent_counters = hack_silent_counters + -1;
        }
        else
        {
          result = count_counters(src_player, src_card, COUNTER_P1_P1);
          remove_counters_impl(player, card, src_player, src_card, COUNTER_P1_P1,
                                                0xff);
          hack_silent_counters = hack_silent_counters + 1;
          copy_counters(player, card, t_player, t_card, src_player, src_card, 1);
          clear_all_counters(src_player, src_card);
          add_counters(player, card, src_player, src_card, COUNTER_P1_P1, result);
          hack_silent_counters = hack_silent_counters + -1;
        }
      }
      else
      {
        result = check_battlefield_for_id(t_player, 0x2d73);
        if ((counter_type != COUNTER_M1_M1) || (result == 0))
        {
          result = check_battlefield_for_id(1 - t_player, 0x4f3c);
          if ((counter_type != COUNTER_P1_P1) || (result == 0))
          {
            if (number < 0)
            {
              number = 0xff;
            }
            result = count_counters(src_player, src_card, counter_type);
            if (result < number)
            {
              number = result;
            }
            if (0 < number)
            {
              remove_counters_impl(player, card, src_player, src_card,
                                                    counter_type, number);
            }
            add_counters_predoubled(player, card, t_player, t_card, counter_type,
                                                number);
            return;
          }
        }
      }
    }
  }
  return;
}


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26d4f30; original functions/counters.c:1506. */
void increment_all_counters(int src_player, int src_card, int t_player, int t_card)
{
  counter_t *pcVar1;
  char cVar2;
  card_instance_t *instance;
  counter_t cVar4;
  int result;
  unsigned int uStack_38;
  counter_t counter_types[9];
  if ((1 < (unsigned int)t_player) || (0x96 < t_card))
  {
    return;
  }
  if (t_card < 0)
  {
    if (ENERGY_COUNTERS(t_player) != '\0')
    {
      add_energy(t_player, 1);
    }
    if (POISON_COUNTERS(t_player) != '\0')
    {
      poison(t_player, 1);
    }
    if (EXPERIENCE_COUNTERS(t_player) == '\0')
    {
      return;
    }
    add_experience(t_player, 1);
    return;
  }
  instance = get_card_instance(t_player, t_card);
  if (instance->internal_card_id == activation_card)
  {
    instance = get_card_instance(instance->parent_controller, instance->parent_card);
  }
  uStack_38 = 0;
  if (instance->special_counters != '\0')
  {
    counter_types[0] = (unsigned int)instance->special_counter_type;
    uStack_38 = (unsigned int)(instance->special_counter_type != COUNTER_invalid);
  }
  if (instance->counters2 != '\0')
  {
    cVar4 = (counter_t)(unsigned char)instance->targets[0x12].player;
    counter_types[uStack_38] = cVar4;
    uStack_38 = uStack_38 + (cVar4 != COUNTER_invalid);
  }
  if (instance->counters3 != '\0')
  {
    cVar4 = (counter_t) * (unsigned char *)((int)&instance->targets[0x12].player + 1);
    counter_types[uStack_38] = cVar4;
    uStack_38 = uStack_38 + (cVar4 != COUNTER_invalid);
  }
  if (instance->counters4 != '\0')
  {
    cVar4 = (counter_t) * (unsigned char *)((int)&instance->targets[0x12].player + 2);
    counter_types[uStack_38] = cVar4;
    uStack_38 = uStack_38 + (cVar4 != COUNTER_invalid);
  }
  if (instance->counters5 != '\0')
  {
    cVar4 = (counter_t) * (unsigned char *)((int)&instance->targets[0x12].player + 3);
    counter_types[uStack_38] = cVar4;
    uStack_38 = uStack_38 + (cVar4 != COUNTER_invalid);
  }
  if (instance->counters != '\0')
  {
    counter_types[uStack_38] = COUNTER_P1_P1;
    uStack_38 = uStack_38 + 1;
  }
  if (instance->counters_m1m1 != '\0')
  {
    counter_types[uStack_38] = COUNTER_M1_M1;
    uStack_38 = uStack_38 + 1;
  }
  cVar2 = *(char *)((int)&instance->targets[0x12].card + 1);
  if (((char)instance->targets[0x12].card == '\0') ||
      (cVar4 = (counter_t) * (unsigned char *)((int)&instance->targets[0x12].card + 2),
       counter_types[uStack_38] = cVar4, cVar4 == COUNTER_invalid))
  {
    if ((cVar2 == '\0') ||
        (cVar4 = (counter_t) * (unsigned char *)((int)&instance->targets[0x12].card + 3),
         counter_types[uStack_38] = cVar4, cVar4 == COUNTER_invalid))
    {
      if (uStack_38 == 0)
      {
        return;
      }
      goto LAB_026d50de;
    }
  }
  else
  {
    uStack_38 = uStack_38 + 1;
    if ((cVar2 == '\0') ||
        (cVar4 = (counter_t) * (unsigned char *)((int)&instance->targets[0x12].card + 3),
         counter_types[uStack_38] = cVar4, cVar4 == COUNTER_invalid))
      goto LAB_026d50de;
  }
  uStack_38 = uStack_38 + 1;
LAB_026d50de:
  result = 0;
  do
  {
    pcVar1 = counter_types + result;
    result = result + 1;
    add_counters(src_player, src_card, t_player, t_card, *pcVar1, 1);
  } while (result < (int)uStack_38);
  return;
}

#endif
