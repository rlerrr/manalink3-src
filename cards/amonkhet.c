#include "manalink.h"

static void
cb_embalm(token_generation_t* token, int c, int toknum)
{
  set_special_flags3(token->t_player, c, SF3_EMBALMED_OR_ETERNALIZED);
}
static int
embalm(int player, int card, event_t event, card_id_t cid, int x, int b, int u, int g, int r, int w)
{
  /* 702.127. Embalm
   *
   * 702.127a Embalm is an activated ability that functions while the card with embalm is in a graveyard. "Embalm
   * [cost]" means "[Cost], Exile this card from your graveyard: Create a token that's a copy of this card, except
   * it's white, it has no mana cost, and it's a Zombie in addition to its other types. Activate this ability only any
   * time you could cast a sorcery."
   *
   * 702.127b A token is "embalmed" if it's created by a resolving embalm ability. */

  if (event == EVENT_GRAVEYARD_ABILITY
	  && has_mana_multi(player, x, b, u, g, r, w) && can_sorcery_be_played(player, event))
	return GA_EMBALM | GA_BASIC;

  if (event == EVENT_PAY_FLASHBACK_COSTS)
	{
	  charge_mana_multi(player, x, b, u, g, r, w);
	  if (cancel != 1)
		return GAPAID_EXILE;
	}

  if (event == EVENT_RESOLVE_ACTIVATED_GRAVEYARD_ABILITY)
	{
	  token_generation_t token;
	  default_token_definition(player, card, cid, &token);
	  token.color_forced = COLOR_TEST_WHITE;
	  token.no_sleight = 1;
	  token.special_code_on_generation = cb_embalm;
	  generate_token(&token);
	}

  return event == EVENT_CAN_CAST;
}

static void
vigilant_white_warriors(int player, int card, int num)
{
  token_generation_t token;
  default_token_definition(player, card, CARD_ID_WARRIOR, &token);
  token.qty = num;
  token.s_key_plus = SP_KEYWORD_VIGILANCE;
  token.color_forced = COLOR_TEST_WHITE;
  generate_token(&token);
}

int card_angel_of_sanctions(int player, int card, event_t event)
{
  /* Angel of Sanctions	|3|W|W
   * Creature - Angel 3/4
   * Flying
   * When ~ enters the battlefield, you may exile target nonland permanent an opponent controls until ~ leaves the battlefield.
   * Embalm |5|W */

  return_from_oblivion(player, card, event);

  etb_ability(player, card, event, RESOLVE_TRIGGER_AI(player));

  if (event == EVENT_ETB_ABILITY)
	{
	  target_definition_t td;
	  default_target_definition(player, card, &td, TYPE_PERMANENT);
	  td.illegal_type = TYPE_LAND;
	  td.allowed_controller = 1-player;

	  if (can_target(&td)
		  && new_pick_target(&td, "Select target nonland permanent an opponent controls.", 0, GS_LITERAL_PROMPT))
		{
		  card_instance_t* inst = get_card_instance(player, card);
		  obliviation(player, card, inst->targets[0].player, inst->targets[0].card);
		  inst->number_of_targets = 0;
		}
	}

  return embalm(player, card, event, CARD_ID_ANGEL_OF_SANCTIONS, MANACOST_XW(5, 1));
}

/* Anointed Procession	|3|W	=>innistrad.c:card_parallel_lives 0x2003f31
 * Enchantment
 * If an effect would create one or more tokens under your control, it creates twice that many of those tokens instead. */

int card_anointer_priest(int player, int card, event_t event)
{
  /* Anointer Priest	|1|W
   * Creature - Human Cleric 1/3
   * Whenever a creature token enters the battlefield under your control, you gain 1 life.
   * Embalm |3|W */

  if (specific_cip(player, card, event, player, RESOLVE_TRIGGER_MANDATORY, TYPE_CREATURE,F1_IS_TOKEN, 0,0, 0,0, 0,0,
				   -1,0))
	gain_life(player, 1);

  return embalm(player, card, event, CARD_ID_ANOINTER_PRIEST, MANACOST_XW(3, 1));
}

/* Approach of the Second Sun	|6|W
 * Sorcery
 * If this spell was cast from your hand and you've cast another spell named ~ this game, you win the game. Otherwise, put ~ into its owner's library seventh from the top and you gain 7 life. */

// Aven Mindcensor => future_sight.c

int card_binding_mummy(int player, int card, event_t event)
{
  /* Binding Mummy	|1|W
   * Creature - Zombie 2/2
   * Whenever another Zombie enters the battlefield under your control, you may tap target artifact or creature. */

  if (trigger_condition == TRIGGER_COMES_INTO_PLAY && affect_me(player, card)
	  && player == reason_for_trigger_controller)
	{
	  test_definition_t test;
	  new_default_test_definition(&test, TYPE_PERMANENT, "");
	  test.subtype = SUBTYPE_ZOMBIE;
	  test.not_me = 1;

	  etb_ability_global(player, event, RESOLVE_TRIGGER_AI(player), 0, &test, player, card);
	}

  if (event == EVENT_ETB_ABILITY)
	{
	  target_definition_t td;
	  default_target_definition(player, card, &td, TYPE_ARTIFACT | TYPE_CREATURE);
	  td.preferred_controller = 1-player;

	  card_instance_t* inst = get_card_instance(player, card);

	  if (can_target(&td) && pick_target(&td, "PUTREFY"))
		tap_card(inst->targets[0].player, inst->targets[0].card);
	  inst->number_of_targets = 0;
	  cancel = 0;
	}
  return 0;
}

int card_cartouche_of_solidarity(int player, int card, event_t event)
{
  /* Cartouche of Solidarity	|W
   * Enchantment - Aura Cartouche
   * Enchant creature you control
   * When ~ enters the battlefield, create a 1/1 |Swhite Warrior creature token with vigilance.
   * Enchanted creature gets +1/+1 and has first strike. */

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

  if (event == EVENT_ETB_ABILITY)
	vigilant_white_warriors(player, card, 1);

  return generic_aura(player, card, event, player, 1,1, KEYWORD_FIRST_STRIKE,0, 0,0,0);
}

int card_cast_out(int player, int card, event_t event)
{
  /* Cast Out	|3|W
   * Enchantment
   * Flash
   * When ~ enters the battlefield, exile target nonland permanent an opponent controls until ~ leaves the battlefield.
   * Cycling |W */

  if (IS_ACTIVATING_FROM_HAND(event))
	return cycling(player, card, event, MANACOST_W(1));

  return_from_oblivion(player, card, event);

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

  if (event == EVENT_ETB_ABILITY)
	{
	  target_definition_t td;
	  default_target_definition(player, card, &td, TYPE_PERMANENT);
	  td.illegal_type = TYPE_LAND;
	  td.allowed_controller = 1-player;
	  td.allow_cancel = 0;

	  if (can_target(&td)
		  && new_pick_target(&td, "Select target nonland permanent an opponent controls.", 0, GS_LITERAL_PROMPT))
		{
		  card_instance_t* inst = get_card_instance(player, card);
		  obliviation(player, card, inst->targets[0].player, inst->targets[0].card);
		  inst->number_of_targets = 0;
		}
	}

  return global_enchantment(player, card, event);
}

/* Compulsory Rest	|1|W
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature can't attack or block.
 * Enchanted creature has "|2, Sacrifice this creature: You gain 2 life." */

/* Devoted Crop-Mate	|2|W
 * Creature - Human Warrior 3/2
 * You may exert ~ as it attacks. When you do, return target creature card with converted mana cost 2 or less from your graveyard to the battlefield. */

int card_djerus_resolve(int player, int card, event_t event)
{
  /* Djeru's Resolve	|W
   * Instant
   * Untap target creature. Prevent all damage that would be dealt to it this turn.
   * Cycling |2 */

  if (IS_ACTIVATING_FROM_HAND(event))
	return cycling(player, card, event, MANACOST_X(2));

  if (!IS_GS_EVENT(player, card, event))
	return 0;

  target_definition_t td;
  default_target_definition(player, card, &td, TYPE_CREATURE);
  td.preferred_controller = player;

  if (event == EVENT_RESOLVE_SPELL)
	{
	  if (valid_target(&td))
		{
		  card_instance_t* inst = get_card_instance(player, card);
		  untap_card(inst->targets[0].player, inst->targets[0].card);
		  prevent_all_damage_to_target(player, card, inst->targets[0].player, inst->targets[0].card, 1);
		}
	  kill_card(player, card, KILL_DESTROY);
	}

  return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
}

/* Fan Bearer	|W	=>shadowmoor.c:card_trip_noose 0x2007ebf
 * Creature - Zombie 1/2
 * |2, |T: Tap target creature. */

int card_revoke_existence(int player, int card, event_t event);

int card_forsake_the_worldly(int player, int card, event_t event)
{
  /* Forsake the Worldly	|2|W
   * Instant
   * Exile target artifact or enchantment.
   * Cycling |2 */

  if (IS_ACTIVATING_FROM_HAND(event))
	return cycling(player, card, event, MANACOST_X(2));

  return card_revoke_existence(player, card, event);
}

/* Gideon of the Trials	|1|W|W
 * Legendary Planeswalker - Gideon (3)
 * +1: Until your next turn, prevent all damage target permanent would deal.
 * 0: Until end of turn, ~ becomes a 4/4 Human Soldier creature with indestructible that's still a planeswalker. Prevent all damage that would be dealt to him this turn.
 * 0: You get an emblem with "As long as you control a ~ planeswalker, you can't lose the game and your opponents can't win the game." */

/* Gideon of the Trials Emblem	""
 * Emblem - Gideon
 * As long as you control a Gideon planeswalker, you can't lose the game and your opponents can't win the game. */

/* Gideon's Intervention	|2|W|W
 * Enchantment
 * As ~ enters the battlefield, choose a card name.
 * Your opponents can't cast spells with the chosen name.
 * Prevent all damage that would be dealt to you and permanents you control by sources with the chosen name. */

/* Glory-Bound Initiate	|1|W
 * Creature - Human Warrior 3/1
 * You may exert ~ as it attacks. When you do, it gets +1/+3 and gains lifelink until end of turn. */

/* Gust Walker	|1|W
 * Creature - Human Wizard 2/2
 * You may exert ~ as it attacks. When you do, it gets +1/+1 and gains flying until end of turn. */

// Impeccable Timing => kaladesh.c

int card_in_oketras_name(int player, int card, event_t event)
{
  /* In Oketra's Name	|1|W
   * Instant
   * Zombies you control get +2/+1 until end of turn. Other creatures you control get +1/+1 until end of turn. */

  if (event == EVENT_RESOLVE_SPELL)
	{
	  test_definition_t test;
	  new_default_test_definition(&test, TYPE_CREATURE, "");
	  test.subtype = SUBTYPE_ZOMBIE;
	  pump_creatures_until_eot(player, card, player, 1, 2,1, 0,0, &test);

	  test.subtype_flag = DOESNT_MATCH;
	  pump_creatures_until_eot(player, card, player, 2, 1,1, 0,0, &test);

	  kill_card(player, card, KILL_DESTROY);
	}

  return basic_spell(player, card, event);
}

// Mighty Leap => m11.c

int card_oketra_the_true(int player, int card, event_t event)
{
  /* Oketra the True	|3|W
   * Legendary Creature - God 3/6
   * Double strike, indestructible
   * ~ can't attack or block unless you control at least three other creatures.
   * |3|W: Create a 1/1 |Swhite Warrior creature token with vigilance. */

  check_legend_rule(player, card, event);

  indestructible(player, card, event);

  if (event == EVENT_ATTACK_LEGALITY && affect_me(player, card)
	  && !is_humiliated(player, card)
	  && count_permanents_by_type(player, TYPE_CREATURE) <= 3)
	event_result = 1;

  if (event == EVENT_ABILITIES && affect_me(player, card)
	  && !is_humiliated(player, card)
	  && count_permanents_by_type(player, TYPE_CREATURE) <= 3)
	cannot_block(player, card, event);

  if (event == EVENT_RESOLVE_ACTIVATION)
	vigilant_white_warriors(player, card, 1);

  return generic_activated_ability(player, card, event, GAA_NONE, MANACOST_XW(3, 1), 0, NULL, NULL);
}

int card_oketras_attendant(int player, int card, event_t event)
{
  /* Oketra's Attendant	|3|W|W
   * Creature - Bird Soldier 3/3
   * Flying
   * Cycling |2
   * Embalm |3|W|W */

  if (IS_ACTIVATING_FROM_HAND(event))
	return cycling(player, card, event, MANACOST_X(2));

  return embalm(player, card, event, CARD_ID_OKETRAS_ATTENDANT, MANACOST_XW(3, 2));
}

int card_protection_of_the_hekma(int player, int card, event_t event)
{
  /* Protection of the Hekma	|4|W
   * Enchantment
   * If a source an opponent controls would deal damage to you, prevent 1 of that damage. */

  card_instance_t* damage = damage_being_prevented(event);
  if (damage && damage->damage_target_player == player && damage->damage_target_card == -1
	  && damage->damage_source_player == 1-player
	  && !is_humiliated(player, card))
	prevent_damage(affected_card_controller, affected_card, 1);

  return 0;
}

int card_regal_caracal(int player, int card, event_t event)
{
  /* Regal Caracal	|3|W|W
   * Creature - Cat 3/3
   * Other Cats you control get +1/+1 and have lifelink.
   * When ~ enters the battlefield, create two 1/1 |Swhite Cat creature tokens with lifelink. */

  boost_subtype(player, card, event, SUBTYPE_KNIGHT, 1,1, 0,SP_KEYWORD_LIFELINK, BCT_CONTROLLER_ONLY);

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

  if (event == EVENT_ETB_ABILITY)
	{
	  token_generation_t token;
	  default_token_definition(player, card, CARD_ID_CAT, &token);
	  token.qty = 2;
	  token.pow = 1;
	  token.tou = 1;
	  token.s_key_plus = SP_KEYWORD_LIFELINK;
	  generate_token(&token);
	}
  return 0;
}

// Renewed Faith => onslaught.c

/* Rhet-Crop Spearmaster	|2|W
 * Creature - Human Warrior 3/1
 * You may exert ~ as it attacks. When you do, it gets +1/+0 and gains first strike until end of turn. */

int card_sacred_cat(int player, int card, event_t event)
{
  /* Sacred Cat	|W
   * Creature - Cat 1/1
   * Lifelink
   * Embalm |W */

  lifelink(player, card, event);

  return embalm(player, card, event, CARD_ID_SACRED_CAT, MANACOST_W(1));
}

/* Seraph of the Suns	|5|W|W	=>scars_of_mirrodin.c:card_darksteel_myr 0x200422e
 * Creature - Angel 4/4
 * Flying
 * Indestructible */

int card_sparring_mummy(int player, int card, event_t event)
{
  /* Sparring Mummy	|3|W
   * Creature - Zombie 3/3
   * When ~ enters the battlefield, untap target creature. */

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

  if (event == EVENT_ETB_ABILITY)
	{
	  target_definition_t td;
	  default_target_definition(player, card, &td, TYPE_CREATURE);
	  td.preferred_controller = player;
	  td.allow_cancel = 0;

	  card_instance_t* inst = get_card_instance(player, card);

	  if (can_target(&td) && pick_target(&td, "TARGET_CREATURE"))
		untap_card(inst->targets[0].player, inst->targets[0].card);

	  cancel = 0;
	  inst->number_of_targets = 0;
	}

  return 0;
}

/* Supply Caravan	|4|W
 * Creature - Camel 3/5
 * When ~ enters the battlefield, if you control a tapped creature, create a 1/1 |Swhite Warrior creature token with vigilance. */

/* Tah-Crop Elite	|3|W
 * Creature - Bird Warrior 2/2
 * Flying
 * You may exert ~ as it attacks. When you do, creatures you control get +1/+1 until end of turn. */

/* Those Who Serve	|2|W	0x401000
 * Creature - Zombie 2/4 */

/* Time to Reflect	|W
 * Instant
 * Exile target creature that blocked or was blocked by a Zombie this turn. */

/* Trial of Solidarity	|2|W
 * Enchantment
 * When ~ enters the battlefield, creatures you control get +2/+1 and gain vigilance until end of turn.
 * When a Cartouche enters the battlefield under your control, return ~ to its owner's hand. */

/* Trueheart Duelist	|1|W
 * Creature - Human Warrior 2/2
 * ~ can block an additional creature each combat.
 * Embalm |2|W */

/* Unwavering Initiate	|2|W
 * Creature - Human Warrior 3/2
 * Vigilance
 * Embalm |4|W */

/* Vizier of Deferment	|2|W
 * Creature - Human Cleric 2/2
 * Flash
 * When ~ enters the battlefield, you may exile target creature if it attacked or blocked this turn. Return that card to the battlefield under its owner's control at the beginning of the next end step. */

/* Vizier of Remedies	|1|W
 * Creature - Human Cleric 2/1
 * If one or more -1/-1 counters would be put on a creature you control, that many -1/-1 counters minus one are put on it instead. */

/* Winged Shepherd	|5|W
 * Creature - Angel 3/3
 * Flying, vigilance
 * Cycling |W */

// Ancient Crab => vanilla, first printed in OGW

/* Angler Drake	|4|U|U	=>return_to_ravnica.c:card_voidwielder 0x2007596
 * Creature - Drake 4/4
 * Flying
 * When ~ enters the battlefield, you may return target creature to its owner's hand. */

/* As Foretold	|2|U
 * Enchantment
 * At the beginning of your upkeep, put a time counter on ~.
 * Once each turn, you may pay |0 rather than pay the mana cost for a spell you cast with converted mana cost X or less, where X is the number of time counters on ~. */

/* Aven Initiate	|3|U
 * Creature - Bird Warrior 3/2
 * Flying
 * Embalm |6|U */

// Cancel => unlimited.c:card_counterspell(), first printed in TSP

/* Cartouche of Knowledge	|1|U
 * Enchantment - Aura Cartouche
 * Enchant creature you control
 * When ~ enters the battlefield, draw a card.
 * Enchanted creature gets +1/+1 and has flying. */

/* Censor	|1|U
 * Instant
 * Counter target spell unless its controller pays |1.
 * Cycling |U */

/* Compelling Argument	|1|U
 * Sorcery
 * Target player puts the top five cards of their library into their graveyard.
 * Cycling |U */

/* Cryptic Serpent	|5|U|U
 * Creature - Serpent 6/5
 * This spell costs |1 less to cast for each instant and sorcery card in your graveyard. */

/* Curator of Mysteries	|2|U|U
 * Creature - Sphinx 4/4
 * Flying
 * Whenever you cycle or discard another card, scry 1.
 * Cycling |U */

/* Decision Paralysis	|3|U	=>m12.c:card_frost_breath 0x2004035
 * Instant
 * Tap up to two target creatures. Those creatures don't untap during their controller's next untap step. */

/* Drake Haven	|2|U
 * Enchantment
 * Whenever you cycle or discard a card, you may pay |1. If you do, create a 2/2 |Sblue Drake creature token with flying. */

// Essence Scatter => legends.c:card_remove_soul(), first printed in M10

/* Floodwaters	|4|U|U
 * Sorcery
 * Return up to two target creatures to their owners' hands.
 * Cycling |2 */

/* Galestrike	|2|U
 * Instant
 * Return target tapped creature to its owner's hand.
 * Draw a card. */

/* Glyph Keeper	|3|U|U
 * Creature - Sphinx 5/3
 * Flying
 * Whenever ~ becomes the target of a spell or ability for the first time each turn, counter that spell or ability.
 * Embalm |5|U|U */

/* Hekma Sentinels	|2|U
 * Creature - Human Cleric 2/3
 * Whenever you cycle or discard a card, ~ gets +1/+1 until end of turn. */

/* Hieroglyphic Illumination	|3|U
 * Instant
 * Draw two cards.
 * Cycling |U */

/* Illusory Wrappings	|2|U
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature has base power and toughness 0/2. */

/* Kefnet the Mindful	|2|U
 * Legendary Creature - God 5/5
 * Flying, indestructible
 * ~ can't attack or block unless you have seven or more cards in hand.
 * |3|U: Draw a card, then you may return a land you control to its owner's hand. */

/* Labyrinth Guardian	|1|U
 * Creature - Illusion Warrior 2/3
 * When ~ becomes the target of a spell, sacrifice it.
 * Embalm |3|U */

/* Lay Claim	|5|U|U
 * Enchantment - Aura
 * Enchant permanent
 * You control enchanted permanent.
 * Cycling |2 */

/* Naga Oracle	|3|U
 * Creature - Naga Cleric 2/4
 * When ~ enters the battlefield, look at the top three cards of your library. Put any number of them into your graveyard and the rest back on top of your library in any order. */

/* New Perspectives	|5|U
 * Enchantment
 * When ~ enters the battlefield, draw three cards.
 * As long as you have seven or more cards in hand, you may pay |0 rather than pay cycling costs. */

/* Open into Wonder	|X|U|U
 * Sorcery
 * X target creatures can't be blocked this turn. Until end of turn, those creatures gain "Whenever this creature deals combat damage to a player, draw a card." */

/* Pull from Tomorrow	|X|U|U
 * Instant
 * Draw X cards, then discard a card. */

/* River Serpent	|5|U
 * Creature - Serpent 5/5
 * ~ can't attack unless there are five or more cards in your graveyard.
 * Cycling |U */

/* Sacred Excavation	|3|U
 * Sorcery
 * Return up to two target cards with cycling from your graveyard to your hand. */

/* Scribe of the Mindful	|2|U
 * Creature - Human Cleric 2/2
 * |1, |T, Sacrifice ~: Return target instant or sorcery card from your graveyard to your hand. */

/* Seeker of Insight	|1|U
 * Creature - Human Wizard 1/3
 * |T: Draw a card, then discard a card. Activate this ability only if you've cast a noncreature spell this turn. */

/* Shimmerscale Drake	|4|U
 * Creature - Drake 3/4
 * Flying
 * Cycling |2 */

/* Slither Blade	|U	=>avacyn_restored.c:card_latch_seeker 0x2005b0b
 * Creature - Naga Rogue 1/2
 * ~ can't be blocked. */

/* Tah-Crop Skirmisher	|1|U
 * Creature - Naga Warrior 2/1
 * Embalm |3|U */

/* Trial of Knowledge	|3|U
 * Enchantment
 * When ~ enters the battlefield, draw three cards, then discard a card.
 * When a Cartouche enters the battlefield under your control, return ~ to its owner's hand. */

/* Vizier of Many Faces	|2|U|U
 * Creature - Shapeshifter Cleric 0/0
 * You may have ~ enter the battlefield as a copy of any creature on the battlefield, except if ~ was embalmed, the token has no mana cost, it's |Swhite, and it's a Zombie in addition to its other types.
 * Embalm |3|U|U */

/* Vizier of Tumbling Sands	|2|U
 * Creature - Human Cleric 1/3
 * |T: Untap another target permanent.
 * Cycling |1|U
 * When you cycle ~, untap target permanent. */

/* Winds of Rebuke	|1|U
 * Instant
 * Return target nonland permanent to its owner's hand. Each player puts the top two cards of their library into their graveyard. */

/* Zenith Seeker	|3|U
 * Creature - Bird Wizard 2/2
 * Flying
 * Whenever you cycle or discard a card, target creature gains flying until end of turn. */

/* Archfiend of Ifnir	|3|B|B
 * Creature - Demon 5/4
 * Flying
 * Whenever you cycle or discard another card, put a -1/-1 counter on each creature your opponents control.
 * Cycling |2 */

/* Baleful Ammit	|2|B
 * Creature - Crocodile Demon 4/3
 * Lifelink
 * When ~ enters the battlefield, put a -1/-1 counter on target creature you control. */

/* Blighted Bat	|2|B
 * Creature - Zombie Bat 2/1
 * Flying
 * |1: ~ gains haste until end of turn. */

/* Bone Picker	|3|B
 * Creature - Bird 3/2
 * This spell costs |3 less to cast if a creature died this turn.
 * Flying, deathtouch */

/* Bontu the Glorified	|2|B
 * Legendary Creature - God 4/6
 * Menace, indestructible
 * ~ can't attack or block unless a creature died under your control this turn.
 * |1|B, Sacrifice another creature: Scry 1. Each opponent loses 1 life and you gain 1 life. */

/* Cartouche of Ambition	|2|B
 * Enchantment - Aura Cartouche
 * Enchant creature you control
 * When ~ enters the battlefield, you may put a -1/-1 counter on target creature.
 * Enchanted creature gets +1/+1 and has lifelink. */

/* Cruel Reality	|5|B|B
 * Enchantment - Aura Curse
 * Enchant player
 * At the beginning of enchanted player's upkeep, that player sacrifices a creature or planeswalker. If the player can't, they lose 5 life. */

/* Cursed Minotaur	|2|B
 * Creature - Zombie Minotaur 3/2
 * Menace */

/* Dispossess	|2|B
 * Sorcery
 * Choose an artifact card name. Search target opponent's graveyard, hand, and library for any number of cards with the chosen name and exile them. Then that player shuffles their library. */

/* Doomed Dissenter	|1|B
 * Creature - Human 1/1
 * When ~ dies, create a 2/2 |Sblack Zombie creature token. */

/* Dread Wanderer	|B
 * Creature - Zombie Jackal 2/1
 * ~ enters the battlefield tapped.
 * |2|B: Return ~ from your graveyard to the battlefield. Activate this ability only any time you could cast a sorcery and only if you have one or fewer cards in hand. */

/* Dune Beetle	|1|B
 * Creature - Insect 1/4 */

/* Faith of the Devoted	|2|B
 * Enchantment
 * Whenever you cycle or discard a card, you may pay |1. If you do, each opponent loses 2 life and you gain 2 life. */

/* Festering Mummy	|B	=>scars_of_mirrodin.c:card_necropede 0x2004337
 * Creature - Zombie 1/1
 * When ~ dies, you may put a -1/-1 counter on target creature. */

/* Final Reward	|4|B	=>oath_of_the_gatewatch.c:card_oblivion_strike 0x200e56c
 * Instant
 * Exile target creature. */

// Gravedigger => portal_1_2_3k.c

/* Grim Strider	|3|B
 * Creature - Horror 6/6
 * ~ gets -1/-1 for each card in your hand. */

/* Horror of the Broken Lands	|4|B
 * Creature - Horror 4/4
 * Whenever you cycle or discard another card, ~ gets +2/+1 until end of turn.
 * Cycling |B */

/* Lay Bare the Heart	|1|B
 * Sorcery
 * Target opponent reveals their hand. You choose a nonlegendary, nonland card from it. That player discards that card. */

/* Liliana, Death's Majesty	|3|B|B
 * Legendary Planeswalker - Liliana (5)
 * +1: Create a 2/2 |Sblack Zombie creature token. Put the top two cards of your library into your graveyard.
 * -3: Return target creature card from your graveyard to the battlefield. That creature is a |Sblack Zombie in addition to its other colors and types.
 * -7: Destroy all non-Zombie creatures. */

/* Liliana's Mastery	|3|B|B
 * Enchantment
 * Zombies you control get +1/+1.
 * When ~ enters the battlefield, create two 2/2 |Sblack Zombie creature tokens. */

/* Lord of the Accursed	|2|B
 * Creature - Zombie 2/3
 * Other Zombies you control get +1/+1.
 * |1|B, |T: All Zombies gain menace until end of turn. */

/* Miasmic Mummy	|1|B	=>dragons_of_tarkir.c:card_sibsig_icebreakers 0x200dae0
 * Creature - Zombie Jackal 2/2
 * When ~ enters the battlefield, each player discards a card. */

/* Nest of Scarabs	|2|B
 * Enchantment
 * Whenever you put one or more -1/-1 counters on a creature, create that many 1/1 |Sblack Insect creature tokens. */

/* Painful Lesson	|2|B	=>m11.c:card_sign_in_blood 0x2002505
 * Sorcery
 * Target player draws two cards and loses 2 life. */

/* Pitiless Vizier	|3|B
 * Creature - Minotaur Cleric 4/2
 * Whenever you cycle or discard a card, ~ gains indestructible until end of turn. */

/* Plague Belcher	|2|B
 * Creature - Zombie Beast 5/4
 * Menace
 * When ~ enters the battlefield, put two -1/-1 counters on target creature you control.
 * Whenever another Zombie you control dies, each opponent loses 1 life. */

/* Ruthless Sniper	|B
 * Creature - Human Archer 1/2
 * Whenever you cycle or discard a card, you may pay |1. If you do, put a -1/-1 counter on target creature. */

/* Scarab Feast	|B
 * Instant
 * Exile up to three target cards from a single graveyard.
 * Cycling |B */

/* Shadow of the Grave	|1|B
 * Instant
 * Return to your hand all cards in your graveyard that you cycled or discarded this turn. */

/* Soulstinger	|3|B
 * Creature - Scorpion Demon 4/5
 * When ~ enters the battlefield, put two -1/-1 counters on target creature you control.
 * When ~ dies, you may put a -1/-1 counter on target creature for each -1/-1 counter on ~. */

/* Splendid Agony	|2|B
 * Instant
 * Distribute two -1/-1 counters among one or two target creatures. */

/* Stir the Sands	|4|B|B
 * Sorcery
 * Create three 2/2 |Sblack Zombie creature tokens.
 * Cycling |3|B
 * When you cycle ~, create a 2/2 |Sblack Zombie creature token. */

/* Supernatural Stamina	|B
 * Instant
 * Until end of turn, target creature gets +2/+0 and gains "When this creature dies, return it to the battlefield tapped under its owner's control." */

/* Trespasser's Curse	|1|B
 * Enchantment - Aura Curse
 * Enchant player
 * Whenever a creature enters the battlefield under enchanted player's control, that player loses 1 life and you gain 1 life. */

/* Trial of Ambition	|1|B
 * Enchantment
 * When ~ enters the battlefield, target opponent sacrifices a creature.
 * When a Cartouche enters the battlefield under your control, return ~ to its owner's hand. */

// Unburden => scourge.c

/* Wander in Death	|2|B
 * Sorcery
 * Return up to two target creature cards from your graveyard to your hand.
 * Cycling |2 */

/* Wasteland Scorpion	|2|B
 * Creature - Scorpion 2/2
 * Deathtouch
 * Cycling |2 */

/* Ahn-Crop Crasher	|2|R
 * Creature - Minotaur Warrior 3/2
 * Haste
 * You may exert ~ as it attacks. When you do, target creature can't block this turn. */

/* Battlefield Scavenger	|1|R
 * Creature - Jackal Rogue 2/2
 * You may exert ~ as it attacks.
 * Whenever you exert a creature, you may discard a card. If you do, draw a card. */

/* Blazing Volley	|R
 * Sorcery
 * ~ deals 1 damage to each creature your opponents control. */

/* Bloodlust Inciter	|R	=>saviors_of_kamigawa.c:card_akki_drillmaster 0x2006330
 * Creature - Human Warrior 1/1
 * |T: Target creature gains haste until end of turn. */

/* Bloodrage Brawler	|1|R
 * Creature - Minotaur Warrior 4/3
 * When ~ enters the battlefield, discard a card. */

// Brute Strength => oath_of_the_gatewatch.c

/* By Force	|X|R
 * Sorcery
 * Destroy X target artifacts. */

/* Cartouche of Zeal	|R
 * Enchantment - Aura Cartouche
 * Enchant creature you control
 * When ~ enters the battlefield, target creature can't block this turn.
 * Enchanted creature gets +1/+1 and has haste. */

/* Combat Celebrant	|2|R
 * Creature - Human Warrior 4/1
 * If ~ hasn't been exerted this turn, you may exert it as it attacks. When you do, untap all other creatures you control and after this phase, there is an additional combat phase. */

/* Consuming Fervor	|R
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature gets +3/+3 and has "At the beginning of your upkeep, put a -1/-1 counter on this creature." */

/* Deem Worthy	|4|R
 * Instant
 * ~ deals 7 damage to target creature.
 * Cycling |3|R
 * When you cycle ~, you may have it deal 2 damage to target creature. */

/* Desert Cerodon	|5|R
 * Creature - Beast 6/4
 * Cycling |R */

/* Electrify	|3|R	=>rise_of_the_eldrazi.c:card_flame_slash 0x200547b
 * Instant
 * ~ deals 4 damage to target creature. */

/* Emberhorn Minotaur	|3|R
 * Creature - Minotaur Warrior 4/3
 * You may exert ~ as it attacks. When you do, it gets +1/+1 and gains menace until end of turn. */

/* Flameblade Adept	|R
 * Creature - Jackal Warrior 1/2
 * Menace
 * Whenever you cycle or discard a card, ~ gets +1/+0 until end of turn. */

// Fling => m11.c

/* Glorious End	|2|R
 * Instant
 * End the turn.
 * At the beginning of your next end step, you lose the game. */

/* Glorybringer	|3|R|R
 * Creature - Dragon 4/4
 * Flying, haste
 * You may exert ~ as it attacks. When you do, it deals 4 damage to target non-Dragon creature an opponent controls. */

/* Harsh Mentor	|1|R
 * Creature - Human Cleric 2/2
 * Whenever an opponent activates an ability of an artifact, creature, or land on the battlefield, if it isn't a mana ability, ~ deals 2 damage to that player. */

/* Hazoret the Fervent	|3|R
 * Legendary Creature - God 5/4
 * Indestructible, haste
 * ~ can't attack or block unless you have one or fewer cards in hand.
 * |2|R, Discard a card: ~ deals 2 damage to each opponent. */

/* Hazoret's Favor	|2|R
 * Enchantment
 * At the beginning of combat on your turn, you may have target creature you control get +2/+0 and gain haste until end of turn. If you do, sacrifice it at the beginning of the next end step. */

/* Heart-Piercer Manticore	|2|R|R
 * Creature - Manticore 4/3
 * When ~ enters the battlefield, you may sacrifice another creature. When you do, ~ deals damage equal to that creature's power to any target.
 * Embalm |5|R */

/* Hyena Pack	|2|R|R	0x401000
 * Creature - Hyena 3/4 */

/* Limits of Solidarity	|3|R
 * Sorcery
 * Gain control of target creature until end of turn. Untap that creature. It gains haste until end of turn.
 * Cycling |2 */

// Magma Spray => shards_of_alara.c

/* Manticore of the Gauntlet	|4|R
 * Creature - Manticore 5/4
 * When ~ enters the battlefield, put a -1/-1 counter on target creature you control. ~ deals 3 damage to target opponent or planeswalker. */

/* Minotaur Sureshot	|2|R	=>return_to_ravnica.c:card_bellows_lizard 0x200760e
 * Creature - Minotaur Archer 2/3
 * Reach
 * |1|R: ~ gets +1/+0 until end of turn. */

/* Nef-Crop Entangler	|1|R
 * Creature - Human Warrior 2/1
 * Trample
 * You may exert ~ as it attacks. When you do, it gets +1/+2 until end of turn. */

/* Nimble-Blade Khenra	|1|R	=>khans_of_tarkir.c:card_jeskai_windscout 0x200ce74
 * Creature - Jackal Warrior 1/3
 * Prowess */

/* Pathmaker Initiate	|1|R	=>unlimited.c:card_dwarven_warriors 0x200bf9c
 * Creature - Human Wizard 2/1
 * |T: Target creature with power 2 or less can't be blocked this turn. */

/* Pursue Glory	|3|R
 * Instant
 * Attacking creatures get +2/+0 until end of turn.
 * Cycling |2 */

/* Soul-Scar Mage	|R
 * Creature - Human Wizard 1/2
 * Prowess
 * If a source you control would deal noncombat damage to a creature an opponent controls, put that many -1/-1 counters on that creature instead. */

/* Sweltering Suns	|1|R|R
 * Sorcery
 * ~ deals 3 damage to each creature.
 * Cycling |3 */

/* Thresher Lizard	|2|R
 * Creature - Lizard 3/2
 * ~ gets +1/+2 as long as you have one or fewer cards in hand. */

// Tormenting Voice => khans_of_tarkir.c

/* Trial of Zeal	|2|R
 * Enchantment
 * When ~ enters the battlefield, it deals 3 damage to any target.
 * When a Cartouche enters the battlefield under your control, return ~ to its owner's hand. */

/* Trueheart Twins	|4|R
 * Creature - Jackal Warrior 4/4
 * You may exert ~ as it attacks.
 * Whenever you exert a creature, creatures you control get +1/+0 until end of turn. */

/* Violent Impact	|3|R
 * Sorcery
 * Destroy target artifact or land.
 * Cycling |2 */

/* Warfire Javelineer	|3|R
 * Creature - Minotaur Warrior 2/3
 * When ~ enters the battlefield, it deals X damage to target creature an opponent controls, where X is the number of instant and sorcery cards in your graveyard. */

/* Benefaction of Rhonas	|2|G
 * Sorcery
 * Reveal the top five cards of your library. You may put a creature card and/or an enchantment card from among them into your hand. Put the rest into your graveyard. */

/* Bitterblade Warrior	|1|G
 * Creature - Jackal Warrior 2/2
 * You may exert ~ as it attacks. When you do, it gets +1/+0 and gains deathtouch until end of turn. */

/* Cartouche of Strength	|2|G
 * Enchantment - Aura Cartouche
 * Enchant creature you control
 * When ~ enters the battlefield, you may have enchanted creature fight target creature an opponent controls.
 * Enchanted creature gets +1/+1 and has trample. */

/* Champion of Rhonas	|3|G
 * Creature - Jackal Warrior 3/3
 * You may exert ~ as it attacks. When you do, you may put a creature card from your hand onto the battlefield. */

/* Channeler Initiate	|1|G
 * Creature - Human Druid 3/4
 * When ~ enters the battlefield, put three -1/-1 counters on target creature you control.
 * |T, Remove a -1/-1 counter from ~: Add one mana of any color. */

/* Colossapede	|4|G	0x401000
 * Creature - Insect 5/5 */

/* Crocodile of the Crossing	|3|G
 * Creature - Crocodile 5/4
 * Haste
 * When ~ enters the battlefield, put a -1/-1 counter on target creature you control. */

/* Defiant Greatmaw	|2|G
 * Creature - Hippo 4/5
 * When ~ enters the battlefield, put two -1/-1 counters on target creature you control.
 * Whenever you put one or more -1/-1 counters on ~, remove a -1/-1 counter from another target creature you control. */

/* Dissenter's Deliverance	|1|G
 * Instant
 * Destroy target artifact.
 * Cycling |G */

/* Exemplar of Strength	|1|G
 * Creature - Human Warrior 4/4
 * When ~ enters the battlefield, put three -1/-1 counters on target creature you control.
 * Whenever ~ attacks, remove a -1/-1 counter from it. If you do, you gain 1 life. */

// Giant Spider => vanilla, first printed in A

/* Gift of Paradise	|2|G
 * Enchantment - Aura
 * Enchant land
 * When ~ enters the battlefield, you gain 3 life.
 * Enchanted land has "|T: Add two mana of any one color." */

/* Greater Sandwurm	|5|G|G
 * Creature - Wurm 7/7
 * ~ can't be blocked by creatures with power 2 or less.
 * Cycling |2 */

/* Hapatra's Mark	|G
 * Instant
 * Target creature you control gains hexproof until end of turn. Remove all -1/-1 counters from it. */

/* Harvest Season	|2|G
 * Sorcery
 * Search your library for up to X basic land cards, where X is the number of tapped creatures you control, and put those cards onto the battlefield tapped. Then shuffle your library. */

/* Haze of Pollen	|1|G
 * Instant
 * Prevent all combat damage that would be dealt this turn.
 * Cycling |3 */

/* Honored Hydra	|5|G
 * Creature - Snake Hydra 6/6
 * Trample
 * Embalm |3|G */

/* Hooded Brawler	|2|G
 * Creature - Naga Warrior 3/2
 * You may exert ~ as it attacks. When you do, it gets +2/+2 until end of turn. */

/* Initiate's Companion	|1|G
 * Creature - Cat 3/1
 * Whenever ~ deals combat damage to a player, untap target creature or land. */

/* Manglehorn	|2|G
 * Creature - Beast 2/2
 * When ~ enters the battlefield, you may destroy target artifact.
 * Artifacts your opponents control enter the battlefield tapped. */

/* Naga Vitalist	|1|G
 * Creature - Naga Druid 1/2
 * |T: Add one mana of any type that a land you control could produce. */

/* Oashra Cultivator	|G
 * Creature - Human Druid 0/3
 * |2|G, |T, Sacrifice ~: Search your library for a basic land card, put it onto the battlefield tapped, then shuffle your library. */

/* Ornery Kudu	|2|G
 * Creature - Antelope 3/4
 * When ~ enters the battlefield, put a -1/-1 counter on target creature you control. */

/* Pouncing Cheetah	|2|G	=>time_spiral.c:card_ashcoat_bear 0x200cb63
 * Creature - Cat 3/2
 * Flash */

/* Prowling Serpopard	|1|G|G
 * Creature - Cat Snake 4/3
 * This spell can't be countered.
 * Creature spells you control can't be countered. */

/* Quarry Hauler	|3|G
 * Creature - Camel 4/3
 * When ~ enters the battlefield, for each kind of counter on target permanent, put another counter of that kind on it or remove one from it. */

/* Rhonas the Indomitable	|2|G
 * Legendary Creature - God 5/5
 * Deathtouch, indestructible
 * ~ can't attack or block unless you control another creature with power 4 or greater.
 * |2|G: Another target creature gets +2/+0 and gains trample until end of turn. */

/* Sandwurm Convergence	|6|G|G
 * Enchantment
 * Creatures with flying can't attack you or planeswalkers you control.
 * At the beginning of your end step, create a 5/5 |Sgreen Wurm creature token. */

/* Scaled Behemoth	|4|G|G	=>m12.c:card_aven_fleetwing 0x2004021
 * Creature - Crocodile 6/7
 * Hexproof */

/* Shed Weakness	|G
 * Instant
 * Target creature gets +2/+2 until end of turn. You may remove a -1/-1 counter from it. */

/* Shefet Monitor	|5|G
 * Creature - Lizard 6/5
 * Cycling |3|G
 * When you cycle ~, you may search your library for a basic land card or a Desert card, put it onto the battlefield, then shuffle your library. */

/* Sixth Sense	|G
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature has "Whenever this creature deals combat damage to a player, you may draw a card." */

// Spidery Grasp => innistrad.c

/* Stinging Shot	|G
 * Instant
 * Put three -1/-1 counters on target creature with flying.
 * Cycling |2 */

/* Synchronized Strike	|2|G
 * Instant
 * Untap up to two target creatures. They each get +2/+2 until end of turn. */

/* Trial of Strength	|2|G
 * Enchantment
 * When ~ enters the battlefield, create a 4/2 |Sgreen Beast creature token.
 * When a Cartouche enters the battlefield under your control, return ~ to its owner's hand. */

/* Vizier of the Menagerie	|3|G
 * Creature - Naga Cleric 3/4
 * You may look at the top card of your library.
 * You may cast the top card of your library if it's a creature card.
 * You may spend mana as though it were mana of any type to cast creature spells. */

/* Watchful Naga	|2|G
 * Creature - Naga Wizard 2/2
 * You may exert ~ as it attacks. When you do, draw a card. */

/* Ahn-Crop Champion	|2|G|W
 * Creature - Human Warrior 4/4
 * You may exert ~ as it attacks. When you do, untap all other creatures you control. */

/* Aven Wind Guide	|2|W|U
 * Creature - Bird Warrior 2/3
 * Flying, vigilance
 * Creature tokens you control have flying and vigilance.
 * Embalm |4|W|U */

/* Bounty of the Luxa	|2|G|U
 * Enchantment
 * At the beginning of your precombat main phase, remove all flood counters from ~. If no counters were removed this way, put a flood counter on ~ and draw a card. Otherwise, add |C|G|U. */

/* Decimator Beetle	|3|B|G
 * Creature - Insect 4/5
 * When ~ enters the battlefield, put a -1/-1 counter on target creature you control.
 * Whenever ~ attacks, remove a -1/-1 counter from target creature you control and put a -1/-1 counter on up to one target creature defending player controls. */

/* Enigma Drake	|1|U|R	=>theros.c:card_spellheart_chimera 0x200b0dd
 * Creature - Drake 100/4
 * Flying
 * ~'s power is equal to the number of instant and sorcery cards in your graveyard. */

/* Hapatra, Vizier of Poisons	|B|G
 * Legendary Creature - Human Cleric 2/2
 * Whenever ~ deals combat damage to a player, you may put a -1/-1 counter on target creature.
 * Whenever you put one or more -1/-1 counters on a creature, create a 1/1 |Sgreen Snake creature token with deathtouch. */

/* Honored Crop-Captain	|R|W	=>mirrodin_besieged.c:card_accorder_paladin 0x2004481
 * Creature - Human Warrior 3/2
 * Whenever ~ attacks, other attacking creatures get +1/+0 until end of turn. */

/* Khenra Charioteer	|1|R|G
 * Creature - Jackal Warrior 3/3
 * Trample
 * Other creatures you control have trample. */

/* Merciless Javelineer	|2|B|R
 * Creature - Minotaur Warrior 4/2
 * |2, Discard a card: Put a -1/-1 counter on target creature. That creature can't block this turn. */

/* Neheb, the Worthy	|1|B|R
 * Legendary Creature - Minotaur Warrior 2/2
 * First strike
 * Other Minotaurs you control have first strike.
 * As long as you have one or fewer cards in hand, Minotaurs you control get +2/+0.
 * Whenever ~ deals combat damage to a player, each player discards a card. */

/* Nissa, Steward of Elements	|X|G|U
 * Legendary Planeswalker - Nissa (X)
 * +2: Scry 2.
 * 0: Look at the top card of your library. If it's a land card or a creature card with converted mana cost less than or equal to the number of loyalty counters on ~, you may put that card onto the battlefield.
 * -6: Untap up to two target lands you control. They become 5/5 Elemental creatures with flying and haste until end of turn. They're still lands. */

/* Samut, Voice of Dissent	|3|R|G
 * Legendary Creature - Human Warrior 3/4
 * Flash
 * Double strike, vigilance, haste
 * Other creatures you control have haste.
 * |W, |T: Untap another target creature. */

/* Shadowstorm Vizier	|U|B
 * Creature - Human Cleric 1/3
 * Flying
 * Whenever you cycle or discard a card, ~ gets +1/+1 until end of turn. */

/* Temmet, Vizier of Naktamun	|W|U
 * Legendary Creature - Human Cleric 2/2
 * At the beginning of combat on your turn, target creature token you control gets +1/+1 until end of turn and can't be blocked this turn.
 * Embalm |3|W|U */

/* Wayward Servant	|W|B
 * Creature - Zombie 2/2
 * Whenever another Zombie enters the battlefield under your control, each opponent loses 1 life and you gain 1 life. */

/* Weaver of Currents	|1|G|U	=>scars_of_mirrodin.c:card_palladium_myr 0x200acf5
 * Creature - Naga Druid 2/2
 * |T: Add |C|C. */

/* Dusk // Dawn	|2|W|W|/|3|W|W
 * Sorcery // Sorcery
 * Destroy all creatures with power 3 or greater.
 * //
 * Aftermath
 * Return all creature cards with power 2 or less from your graveyard to your hand. */

/* Commit // Memory	|3|U|/|4|U|U
 * Instant // Sorcery
 * Put target spell or nonland permanent into its owner's library second from the top.
 * //
 * Aftermath
 * Each player shuffles their hand and graveyard into their library, then draws seven cards. */

/* Never // Return	|1|B|B|/|3|B
 * Sorcery // Sorcery
 * Destroy target creature or planeswalker.
 * //
 * Aftermath
 * Exile target card from a graveyard. Create a 2/2 |Sblack Zombie creature token. */

/* Insult // Injury	|2|R|/|2|R
 * Sorcery // Sorcery
 * Damage can't be prevented this turn. If a source you control would deal damage this turn, it deals double that damage instead.
 * //
 * Aftermath
 * Injury deals 2 damage to target creature and 2 damage to target player or planeswalker. */

/* Mouth // Feed	|2|G|/|3|G
 * Sorcery // Sorcery
 * Create a 3/3 |Sgreen Hippo creature token.
 * //
 * Aftermath
 * Draw a card for each creature you control with power 3 or greater. */

/* Start // Finish	|2|W|/|2|B
 * Instant // Sorcery
 * Create two 1/1 |Swhite Warrior creature tokens with vigilance.
 * //
 * Aftermath
 * As an additional cost to cast Finish, sacrifice a creature.
 * Destroy target creature. */

/* Reduce // Rubble	|2|U|/|2|R
 * Instant // Sorcery
 * Counter target spell unless its controller pays |3.
 * //
 * Aftermath
 * Up to three target lands don't untap during their controller's next untap step. */

/* Destined // Lead	|1|B|/|3|G
 * Instant // Sorcery
 * Target creature gets +1/+0 and gains indestructible until end of turn.
 * //
 * Aftermath
 * All creatures able to block target creature this turn do so. */

/* Onward // Victory	|2|R|/|2|W
 * Instant // Sorcery
 * Target creature gets +X/+0 until end of turn, where X is its power.
 * //
 * Aftermath
 * Target creature gains double strike until end of turn. */

/* Spring // Mind	|2|G|/|4|U|U
 * Sorcery // Instant
 * Search your library for a basic land card, put it onto the battlefield tapped, then shuffle your library.
 * //
 * Aftermath
 * Draw two cards. */

/* Prepare // Fight	|1|W|/|3|G
 * Instant // Sorcery
 * Untap target creature. It gets +2/+2 and gains lifelink until end of turn.
 * //
 * Aftermath
 * Target creature you control fights target creature an opponent controls. */

/* Failure // Comply	|1|U|/|W
 * Instant // Sorcery
 * Return target spell to its owner's hand.
 * //
 * Aftermath
 * Choose a card name. Until your next turn, your opponents can't cast spells with the chosen name. */

/* Rags // Riches	|2|B|B|/|5|U|U
 * Sorcery // Sorcery
 * All creatures get -2/-2 until end of turn.
 * //
 * Aftermath
 * Each opponent chooses a creature they control. You gain control of those creatures. */

/* Cut // Ribbons	|1|R|/|X|B|B
 * Sorcery // Sorcery
 * Cut deals 4 damage to target creature.
 * //
 * Aftermath
 * Each opponent loses X life. */

/* Heaven // Earth	|X|G|/|X|R|R
 * Instant // Sorcery
 * Heaven deals X damage to each creature with flying.
 * //
 * Aftermath
 * Earth deals X damage to each creature without flying. */

/* Bontu's Monument	|3
 * Legendary Artifact
 * |SBlack creature spells you cast cost |1 less to cast.
 * Whenever you cast a creature spell, each opponent loses 1 life and you gain 1 life. */

/* Edifice of Authority	|3
 * Artifact
 * |1, |T: Target creature can't attack this turn. Put a brick counter on ~.
 * |1, |T: Until your next turn, target creature can't attack or block and its activated abilities can't be activated. Activate this ability only if there are three or more brick counters on ~. */

/* Embalmer's Tools	|2
 * Artifact
 * Activated abilities of creature cards in your graveyard cost |1 less to activate.
 * Tap an untapped Zombie you control: Target player puts the top card of their library into their graveyard. */

/* Gate to the Afterlife	|3
 * Artifact
 * Whenever a nontoken creature you control dies, you gain 1 life. Then you may draw a card. If you do, discard a card.
 * |2, |T, Sacrifice ~: Search your graveyard, hand, and/or library for a card named God-Pharaoh's Gift and put it onto the battlefield. If you search your library this way, shuffle it. Activate this ability only if there are six or more creature cards in your graveyard. */

/* Hazoret's Monument	|3
 * Legendary Artifact
 * |SRed creature spells you cast cost |1 less to cast.
 * Whenever you cast a creature spell, you may discard a card. If you do, draw a card. */

/* Honed Khopesh	|1	=>mirrodin.c:card_leonin_scimitar 0x200529b
 * Artifact - Equipment
 * Equipped creature gets +1/+1.
 * Equip |1 */

/* Kefnet's Monument	|3
 * Legendary Artifact
 * |SBlue creature spells you cast cost |1 less to cast.
 * Whenever you cast a creature spell, target creature an opponent controls doesn't untap during its controller's next untap step. */

/* Luxa River Shrine	|3
 * Artifact
 * |1, |T: You gain 1 life. Put a brick counter on ~.
 * |T: You gain 2 life. Activate this ability only if there are three or more brick counters on ~. */

/* Oketra's Monument	|3
 * Legendary Artifact
 * |SWhite creature spells you cast cost |1 less to cast.
 * Whenever you cast a creature spell, create a 1/1 |Swhite Warrior creature token with vigilance. */

/* Oracle's Vault	|4
 * Artifact
 * |2, |T: Exile the top card of your library. Until end of turn, you may play that card. Put a brick counter on ~.
 * |T: Exile the top card of your library. Until end of turn, you may play that card without paying its mana cost. Activate this ability only if there are three or more brick counters on ~. */

/* Pyramid of the Pantheon	|1
 * Artifact
 * |2, |T: Add one mana of any color. Put a brick counter on ~.
 * |T: Add three mana of any one color. Activate this ability only if there are three or more brick counters on ~. */

/* Rhonas's Monument	|3
 * Legendary Artifact
 * |SGreen creature spells you cast cost |1 less to cast.
 * Whenever you cast a creature spell, target creature you control gets +2/+2 and gains trample until end of turn. */

/* Throne of the God-Pharaoh	|2
 * Legendary Artifact
 * At the beginning of your end step, each opponent loses life equal to the number of tapped creatures you control. */

/* Watchers of the Dead	|2
 * Artifact Creature - Cat 2/2
 * Exile ~: Each opponent chooses two cards in their graveyard and exiles the rest. */

/* Canyon Slough	""	=>urza_saga.c:card_drifting_meadow 0x200a21e
 * Land - |H2Swamp |H2Mountain
 * ~ enters the battlefield tapped.
 * Cycling |2 */

/* Cascading Cataracts	""
 * Land
 * Indestructible
 * |T: Add |C.
 * |5, |T: Add five mana in any combination of colors. */

/* Cradle of the Accursed	""
 * Land - Desert
 * |T: Add |C.
 * |3, |T, Sacrifice ~: Create a 2/2 |Sblack Zombie creature token. Activate this ability only any time you could cast a sorcery. */

// Evolving Wilds => m11.c:card_terramorphic_expanse(), first printed in ROE

/* Fetid Pools	""	=>urza_saga.c:card_drifting_meadow 0x200a21e
 * Land - |H2Island |H2Swamp
 * ~ enters the battlefield tapped.
 * Cycling |2 */

/* Grasping Dunes	""
 * Land - Desert
 * |T: Add |C.
 * |1, |T, Sacrifice ~: Put a -1/-1 counter on target creature. Activate this ability only any time you could cast a sorcery. */

/* Irrigated Farmland	""	=>urza_saga.c:card_drifting_meadow 0x200a21e
 * Land - |H2Plains |H2Island
 * ~ enters the battlefield tapped.
 * Cycling |2 */

/* Painted Bluffs	""
 * Land - Desert
 * |T: Add |C.
 * |1, |T: Add one mana of any color. */

/* Scattered Groves	""	=>urza_saga.c:card_drifting_meadow 0x200a21e
 * Land - |H2Forest |H2Plains
 * ~ enters the battlefield tapped.
 * Cycling |2 */

/* Sheltered Thicket	""	=>urza_saga.c:card_drifting_meadow 0x200a21e
 * Land - |H2Mountain |H2Forest
 * ~ enters the battlefield tapped.
 * Cycling |2 */

/* Sunscorched Desert	""
 * Land - Desert
 * When ~ enters the battlefield, it deals 1 damage to target player or planeswalker.
 * |T: Add |C. */

/* Gideon, Martial Paragon	|4|W
 * Legendary Planeswalker - Gideon (5)
 * +2: Untap all creatures you control. Those creatures get +1/+1 until end of turn.
 * 0: Until end of turn, ~ becomes a 5/5 Human Soldier creature with indestructible that's still a planeswalker. Prevent all damage that would be dealt to him this turn.
 * -10: Creatures you control get +2/+2 until end of turn. Tap all creatures your opponents control. */

/* Companion of the Trials	|2|W
 * Creature - Bird Soldier 2/2
 * Flying
 * |1|W: Untap target creature. Activate this ability only if you control a Gideon planeswalker. */

/* Gideon's Resolve	|4|W
 * Enchantment
 * When ~ enters the battlefield, you may search your library and/or graveyard for a card named Gideon, Martial Paragon, reveal it, and put it into your hand. If you search your library this way, shuffle it.
 * Creatures you control get +1/+1. */

/* Graceful Cat	|2|W	=>m12.c:card_benalish_veteran 0x2003ff9
 * Creature - Cat 2/2
 * Whenever ~ attacks, it gets +1/+1 until end of turn. */

// Stone Quarry => produce_mana.c:mana_producer_tapped(), first printed in SOI

/* Liliana, Death Wielder	|5|B|B
 * Legendary Planeswalker - Liliana (5)
 * +2: Put a -1/-1 counter on up to one target creature.
 * -3: Destroy target creature with a -1/-1 counter on it.
 * -10: Return all creature cards from your graveyard to the battlefield. */

/* Desiccated Naga	|2|B
 * Creature - Zombie Naga 3/2
 * |3|B: Target opponent loses 2 life and you gain 2 life. Activate this ability only if you control a Liliana planeswalker. */

/* Liliana's Influence	|4|B|B
 * Sorcery
 * Put a -1/-1 counter on each creature you don't control. You may search your library and/or graveyard for a card named Liliana, Death Wielder, reveal it, and put it into your hand. If you search your library this way, shuffle it. */

/* Tattered Mummy	|1|B	=>scars_of_mirrodin.c:card_blistergrub 0x20041c0
 * Creature - Zombie Jackal 1/2
 * When ~ dies, each opponent loses 2 life. */

// Foul Orchard => produce_mana.c:mana_producer_tapped(), first printed in SOI

// Cinder Barrens => produce_mana.c:mana_producer_tapped(), first printed in OGW

// Forsaken Sanctuary => produce_mana.c:mana_producer_tapped(), first printed in SOI

// Highland Lake => produce_mana.c:mana_producer_tapped(), first printed in SOI

// Meandering River => produce_mana.c:mana_producer_tapped(), first printed in OGW

// Submerged Boneyard => produce_mana.c:mana_producer_tapped(), first printed in OGW

// Timber Gorge => produce_mana.c:mana_producer_tapped(), first printed in OGW

// Tranquil Expanse => produce_mana.c:mana_producer_tapped(), first printed in OGW

// Woodland Stream => produce_mana.c:mana_producer_tapped(), first printed in SOI

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22d8470; original cards/amonkhet.c:409. */
int card_gideon_of_the_trials_emblem(int player, int card, event_t event)
{
  int result;
  result = check_battlefield_for_subtype(player, 0x100, 0x7005);
  if (result == 0)
  {
    return 0;
  }
  cannot_lose_the_game(player, card, event, player);
  return 0;
}

/* ManalinkEh 2022: 0x22d93a0; original cards/amonkhet.c:789. */
int card_trial_of_solidarity(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    pump_subtype_until_eot(player, card, player, -1, 2, 1, 0, 8);
  }
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 4);
    test.subtype = 0x4011;
    result = new_specific_cip(player, card, event, player, 2, &test);
    if (result != 0)
    {
      bounce_permanent(player, card);
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22d96e0; original cards/amonkhet.c:864. */
int card_vizier_of_remedies(int player, int card, event_t event) { return 0; }

/* ManalinkEh 2022: 0x22d96f0; original cards/amonkhet.c:873. */
int card_winged_shepherd(int player, int card, event_t event)
{
  int result;
  vigilance(player, card, event);
  if (2 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    return 0;
  }
  result = cycling(player, card, event, 0, 0, 0, 0, 0, 1);
  return result;
}

/* ManalinkEh 2022: 0x22d9e20; original cards/amonkhet.c:1017. */
int card_censor(int player, int card, event_t event)
{
  int result;
  if (2 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      counterspell_resolve_unless_pay_x(player, card, NULL, 0, 1);
      kill_card(player, card, KILL_DESTROY);
    }
    result = generic_spell(player, card, event, 2, NULL, NULL, 1, NULL);
    return result;
  }
  result = cycling(player, card, event, 0, 0, 1, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x22d9f00; original cards/amonkhet.c:1042. */
int card_cryptic_serpent(int player, int card, event_t event)
{
  int result;
  if (event != EVENT_MODIFY_COST)
  {
    return 0;
  }
  result = count_graveyard_by_type(player, 0x38);
  COST_COLORLESS = COST_COLORLESS - (char)result;
  return 0;
}

/* ManalinkEh 2022: 0x22d9f40; original cards/amonkhet.c:1053. */
int card_curator_of_mysteries(int player, int card, event_t event)
{
  int result;
  result = discard_trigger(player, card, event, player, 2, 0);
  if (result != 0)
  {
    scry(player, 1);
  }
  result = cycling(player, card, event, 0, 0, 1, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x22da0f0; original cards/amonkhet.c:1103. */
int card_galestrike(int player, int card, event_t event)
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
        td.required_state = 1;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            bounce_permanent(instance->targets[0].player, instance->targets[0].card);
            draw_a_card(player);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_022da15d;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.required_state = 1;
  get_card_instance(player, card);
LAB_022da15d:
  result = generic_spell(player, card, event, 0x81, &td, "Select target tapped creature.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22da250; original cards/amonkhet.c:1135. */
int card_hekma_sentinels(int player, int card, event_t event)
{
  int result;
  result = discard_trigger(player, card, event, player, 2, 0);
  if (result == 0)
  {
    return 0;
  }
  pump_until_eot_merge_previous(player, card, player, card, 1, 1);
  return 0;
}

/* ManalinkEh 2022: 0x22da2d0; original cards/amonkhet.c:1146. */
int card_hieroglyphic_illumination(int player, int card, event_t event)
{
  int result;
  if (2 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      draw_cards(player, 2);
      kill_card(player, card, KILL_DESTROY);
    }
    result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
    return result;
  }
  result = cycling(player, card, event, 0, 0, 1, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x22da6a0; original cards/amonkhet.c:1233. */
int card_naga_oracle(int player, int card, event_t event)
{
  int result;
  int number;
  test_definition_t this_test;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    result = count_deck(player);
    number = result;
    if (3 < result)
    {
      number = 3;
    }
    new_default_test_definition(&this_test, 0xff, "Select a card to put into your graveyard.");
    this_test.no_shuffle = 1;
    while (result != 0)
    {
      this_test.create_minideck = (unsigned short)number;
      result = new_global_tutor(player, player, 1, 3, 0, 3, &this_test);
      if (result < 0)
      {
        if (number < 2)
        {
          return 0;
        }
        rearrange_top_x(player, player, number);
        return 0;
      }
      number = number + -1;
      result = number;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22da7b0; original cards/amonkhet.c:1263. */
int card_new_perspectives(int player, int card, event_t event)
{
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    draw_cards(player, 3);
  }
  if (event != EVENT_SHOULD_AI_PLAY)
  {
    return (unsigned int)(event == EVENT_CAN_CAST);
  }
  ai_modifier = ai_modifier + (-(unsigned int)(player == 0) & 0xfffffff0) + 8;
  return 0;
}

/* ManalinkEh 2022: 0x22dad30; original cards/amonkhet.c:1405. */
int card_scribe_of_the_mindful(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (event == EVENT_ACTIVATE)
  {
    new_default_test_definition(&this_test, 0x38, "Select an instant or sorcery card.");
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition(&this_test, 0x38, "Select an instant or sorcery card.");
    if ((event == EVENT_RESOLVE_ACTIVATION) &&
        (result = validate_target_from_grave_source(player, card, player, 0), result != -1))
    {
      from_grave_to_hand(player, result, 1);
    }
  }
  result = generic_activated_ability_with_target_in_grave(
      player, card, event, GAA_SACRIFICE_ME | GAA_LITERAL_PROMPT | GAA_UNTAPPED, 1, 0, 0, 0, 0, 0,
      0, NULL, NULL, TYPE_NONE, &this_test, 8);
  return result;
}

/* ManalinkEh 2022: 0x22dae70; original cards/amonkhet.c:1428. */
int card_seeker_of_insight(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  int iVar3;

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    draw_cards(player, 1);
    discard(player, 0, player);
    iVar1 = generic_activated_ability(player, card, EVENT_RESOLVE_ACTIVATION, 1, 0, 0, 0, 0, 0, 0,
                                      0, NULL, NULL);
    return iVar1;
  }
  iVar1 = generic_activated_ability(player, card, event, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  if ((event == EVENT_CAN_ACTIVATE) && (iVar1 != 0))
  {
    iVar2 = get_specific_storm_count(player);
    iVar3 = get_stormcreature_count(player);
    if (iVar2 <= iVar3)
    {
      return 0;
    }
  }
  return iVar1;
}

/* ManalinkEh 2022: 0x22db020; original cards/amonkhet.c:1463. */
int card_trial_of_knowledge(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    draw_cards(player, 3);
    discard(player, 0, player);
  }
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 4);
    test.subtype = 0x4011;
    result = new_specific_cip(player, card, event, player, 2, &test);
    if (result != 0)
    {
      bounce_permanent(player, card);
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22db3a0; original cards/amonkhet.c:1540. */
int card_winds_of_rebuke(int player, int card, event_t event)
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
            mill(current_turn, 2);
            mill(current_turn ^ 1, 2);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_022db40d;
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
LAB_022db40d:
  result = generic_spell(player, card, event, 0x81, &td,
                         "Select target nonland permanent to bounce.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22db520; original cards/amonkhet.c:1566. */
int card_zenith_seeker(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  result = discard_trigger(player, card, event, player, 2, 0);
  if (result != 0)
  {
    default_target_definition(player, card, &td, 2);
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    instance->number_of_targets = '\0';
    result = can_target(&td);
    if (result != 0)
    {
      result = pick_target(&td, "TARGET_CREATURE");
      if (result != 0)
      {
        pump_ability_until_eot(player, card, instance->targets[0].player, instance->targets[0].card,
                               0, 0, 0x20, 0);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22db610; original cards/amonkhet.c:1586. */
int card_arcfiend_of_ifnir(int player, int card, event_t event)
{
  int result;
  result = discard_trigger(player, card, event, player, 2, 0);
  if (result != 0)
  {
    manipulate_type(player, card, 1 - player, TYPE_CREATURE, 0x1010001);
  }
  result = cycling(player, card, event, 2, 0, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x22db7d0; original cards/amonkhet.c:1627. */
int card_blighted_bat(int player, int card, event_t event)
{
  int result;
  result = generic_shade(player, card, event, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 4);
  return result;
}

/* ManalinkEh 2022: 0x22db850; original cards/amonkhet.c:1636. */
int card_bone_picker(int player, int card, event_t event)
{
  deathtouch(player, card, event);
  if ((event == EVENT_MODIFY_COST) && (0 < creatures_dead_this_turn))
  {
    COST_COLORLESS = COST_COLORLESS + -3;
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x22dc0a0; original cards/amonkhet.c:1817. */
int card_doomed_dissenter(int player, int card, event_t event)
{
  int result;
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  generate_token_by_id(player, card, 0x49ac);
  return 0;
}

/* ManalinkEh 2022: 0x22dc100; original cards/amonkhet.c:1829. */
int card_dread_wanderer(int player, int card, event_t event)
{
  int result;
  comes_into_play_tapped(player, card, event);
  if (event == EVENT_GRAVEYARD_ABILITY)
  {
    result = has_mana_multi(player, 2, 1, 0, 0, 0, 0);
    if (((result != 0) &&
         (result = can_sorcery_be_played(player, EVENT_GRAVEYARD_ABILITY), result != 0)) &&
        (hand_count[player] < 2))
    {
      return 0x88;
    }
  }
  else if (event == EVENT_PAY_FLASHBACK_COSTS)
  {
    charge_mana_multi(player, 2, 1, 0, 0, 0, 0);
    return (unsigned int)(spell_fizzled != 1);
  }
  return 0;
}

/* ManalinkEh 2022: 0x22dc340; original cards/amonkhet.c:1894. */
int card_horror_of_the_broken_lands(int player, int card, event_t event)
{
  int result;
  result = discard_trigger(player, card, event, player, 2, 0);
  if (result != 0)
  {
    pump_until_eot_merge_previous(player, card, player, card, 2, 1);
  }
  result = cycling(player, card, event, 0, 1, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x22dc3f0; original cards/amonkhet.c:1907. */
int card_lay_bare_the_heart(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 2);
        td.zone = 0x1000;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            default_ec_definition(instance->targets[0].player, player, &ec);
            new_default_test_definition(&this_test, 1, "Select a nonlegendary, nonland card.");
            this_test.subtype = 0x2001;
            this_test.type_flag = 1;
            this_test.subtype_flag = 1;
            new_effect_coercion(&ec, &this_test);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_022dc46d;
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
LAB_022dc46d:
  result = generic_spell(player, card, event, 0x40, &td, NULL, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22dc980; original cards/amonkhet.c:2020. */
int card_lilianas_mastery(int player, int card, event_t event)
{
  int result;
  token_generation_t token;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_token_definition(player, card, 0x49ac, &token);
    token.qty = '\x02';
    generate_token(&token);
  }
  boost_creature_type(player, card, event, SUBTYPE_ZOMBIE, 1, 1, 0,
                      BCT_INCLUDE_SELF | BCT_CONTROLLER_ONLY);
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22dca40; original cards/amonkhet.c:2040. */
int card_lord_of_the_accursed(int player, int card, event_t event)
{
  int result;
  boost_creature_type(player, card, event, SUBTYPE_ZOMBIE, 1, 1, 0, BCT_CONTROLLER_ONLY);
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    pump_subtype_until_eot(player, card, 2, 0xde, 0, 0, 0, 0x10000);
  }
  result = generic_activated_ability(player, card, event, 1, 1, 1, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22dcc00; original cards/amonkhet.c:2081. */
int card_pitiless_vizier(int player, int card, event_t event)
{
  int result;
  result = discard_trigger(player, card, event, player, 2, 0);
  if (result == 0)
  {
    return 0;
  }
  pump_ability_until_eot(player, card, player, card, 0, 0, 0, 0x800);
  return 0;
}

/* ManalinkEh 2022: 0x22dd770; original cards/amonkhet.c:2421. */
int card_trial_of_ambition(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t test;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    result = target_opponent(player, card);
    if (result != 0)
    {
      instance = get_card_instance(player, card);
      player_sacrifices_a_permanent(player, card, instance->targets[0].player, 2, SAC_NO_CANCEL);
    }
  }
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 4);
    test.subtype = 0x4011;
    result = new_specific_cip(player, card, event, player, 2, &test);
    if (result != 0)
    {
      bounce_permanent(player, card);
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22dd8f0; original cards/amonkhet.c:2461. */
int card_wasteland_scorpion(int player, int card, event_t event)
{
  int result;
  deathtouch(player, card, event);
  result = cycling(player, card, event, 2, 0, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x22ddbe0; original cards/amonkhet.c:2518. */
int card_blazing_volley(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    new_damage_all(player, card, 1 - player, 1, 1, NULL);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22ddc60; original cards/amonkhet.c:2535. */
int card_bloodrage_brawler(int player, int card, event_t event)
{
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  discard(player, 0, player);
  return 0;
}

/* ManalinkEh 2022: 0x22de680; original cards/amonkhet.c:2734. */
int card_desert_cerodon(int player, int card, event_t event)
{
  int result;
  if (2 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    return 0;
  }
  result = cycling(player, card, event, 0, 0, 0, 0, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x22de7c0; original cards/amonkhet.c:2763. */
int card_flameblade_adept(int player, int card, event_t event)
{
  int result;
  result = discard_trigger(player, card, event, player, 2, 0);
  if (result == 0)
  {
    return 0;
  }
  pump_until_eot_merge_previous(player, card, player, card, 1, 0);
  return 0;
}

/* ManalinkEh 2022: 0x22d7410; original cards/amonkhet.c:2777. */
static int glorious_end_legacy(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  if ((((current_turn == player) && (trigger_condition == 0xcd)) && (card == affected_card)) &&
      ((player == affected_card_controller && (reason_for_trigger_controller == player))))
  {
    result = instance->targets[1].player;
    if (result < 1)
    {
      result = eot_trigger(player, card, event);
      if (result != 0)
      {
        lose_the_game(player);
        kill_card(player, card, KILL_EXILE);
      }
    }
    else
    {
      instance->targets[1].player = result + -1;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22df3b0; original cards/amonkhet.c:3049. */
int card_sweltering_suns(int player, int card, event_t event)
{
  int result;
  if (2 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      new_damage_all(player, card, 2, 3, 1, NULL);
      kill_card(player, card, KILL_DESTROY);
    }
    result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
    return result;
  }
  result = cycling(player, card, event, 3, 0, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x22df520; original cards/amonkhet.c:3083. */
int card_trial_of_zeal(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    td.allow_cancel = 0;
    result = can_target(&td);
    if (result != 0)
    {
      result = pick_target(&td, "TARGET_ANY");
      if (result != 0)
      {
        damage_target0(player, card, 3);
      }
    }
    instance = get_card_instance(player, card);
    instance->number_of_targets = '\0';
  }
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition((test_definition_t *)&td, 4);
    result = new_specific_cip(player, card, event, player, 2, (test_definition_t *)&td);
    if (result != 0)
    {
      bounce_permanent(player, card);
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22df6f0; original cards/amonkhet.c:3135. */
int card_warfire_javelineer(int player, int card, event_t event)
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
  td.allowed_controller = 1 - player;
  td.allow_cancel = 0;
  td.preferred_controller = td.allowed_controller;
  instance = get_card_instance(player, card);
  result = can_target(&td);
  if (result != 0)
  {
    result = new_pick_target(&td, "Select target creature an opponent controls.", 0, 0x80);
    if (result != 0)
    {
      result = count_graveyard_by_type(player, 0x38);
      damage_creature(instance->targets[0].player, instance->targets[0].card, result, player, card);
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x22df7f0; original cards/amonkhet.c:3160. */
int card_benefaction_of_rhonas(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  unsigned int uVar3;
  test_definition_t test;
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      ai_modifier = ai_modifier + 0x30;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    if (*deck_ptr[player] != -1)
    {
      iVar1 = count_deck(player);
      if (5 < iVar1)
      {
        iVar1 = 5;
      }
      new_default_test_definition(&test, 2, "Select a creature card.");
      test.create_minideck = (unsigned short)iVar1;
      test.no_shuffle = 1;
      iVar2 = new_global_tutor(player, player, 1, 1, 0, 1, &test);
      iVar1 = iVar1 - (unsigned int)(iVar2 != -1);
      if (*deck_ptr[player] != -1)
      {
        new_default_test_definition(&test, 4, "Select an enchantment card.");
        test.create_minideck = (unsigned short)iVar1;
        test.no_shuffle = 1;
        uVar3 = new_global_tutor(player, player, 1, 1, 0, 1, &test);
        iVar1 = iVar1 - (unsigned int)(uVar3 < 0x80000000);
      }
      if (0 < iVar1)
      {
        mill(player, iVar1);
      }
    }
    kill_card(player, card, KILL_DESTROY);
  }
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x22e04c0; original cards/amonkhet.c:3433. */
int card_gift_of_paradise(int player, int card, event_t event)
{
  int result;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    gain_life(player, 3);
  }
  result = enchanted_land_has_T_add_n_mana_of_any_one_color(player, card, event, 2);
  return result;
}

/* ManalinkEh 2022: 0x22e0740; original cards/amonkhet.c:3488. */
int card_harvest_season(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  test_definition_t this_test2;
  if (event == EVENT_RESOLVE_SPELL)
  {
    default_test_definition(&this_test, 2);
    this_test.state = 0x10;
    result = check_battlefield_for_special_card(player, card, player, 4, &this_test);
    if (0 < result)
    {
      new_default_test_definition(&this_test2, 1, "Select a basic land card.");
      this_test2.subtype = 0x2000;
      this_test2.qty = (unsigned short)result;
      new_global_tutor(player, player, 1, 5, 0, 5, &this_test2);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22e0850; original cards/amonkhet.c:3513. */
int card_haze_of_pollen(int player, int card, event_t event)
{
  int result;
  if (2 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if (((card == affected_card) && (player == affected_card_controller)) &&
          ((current_turn == player ||
            ((current_phase != 0x18 || (result = count_attackers(current_turn), result < 1))))))
      {
        ai_modifier = ai_modifier + -0x30;
      }
    }
    else if (event == EVENT_RESOLVE_SPELL)
    {
      fog_effect(player, card);
      kill_card(player, card, KILL_DESTROY);
    }
    result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
    return result;
  }
  result = cycling(player, card, event, 3, 0, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x22e09f0; original cards/amonkhet.c:3548. */
int card_initiates_companion(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  result =
      damage_dealt_by_me(player, card, event, DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_BE_COMBAT_DAMAGE);
  if (result == 0)
  {
    return 0;
  }
  default_target_definition(player, card, &td, 7);
  td.preferred_controller = player;
  td.allow_cancel = 0;
  instance = get_card_instance(player, card);
  result = can_target(&td);
  if (result != 0)
  {
    result = new_pick_target(&td, "Select a creature or land to untap.", 0, 0x80);
    if (result != 0)
    {
      untap_card(instance->targets[0].player, instance->targets[0].card);
    }
  }
  instance->number_of_targets = '\0';
  spell_fizzled = 0;
  return 0;
}

/* ManalinkEh 2022: 0x22e0ac0; original cards/amonkhet.c:3570. */
int card_manglehorn(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 0x40);
    instance = get_card_instance(player, card);
    result = can_target(&td);
    if (result != 0)
    {
      result = pick_target(&td, "TARGET_ARTIFACT");
      if (result != 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      }
    }
    instance->number_of_targets = '\0';
  }
  permanents_enters_battlefield_tapped(player, card, event, 1 - player, 0x40, NULL);
  return 0;
}

/* ManalinkEh 2022: 0x22e0bb0; original cards/amonkhet.c:3595. */
int card_naga_vitalist(int player, int card, event_t event)
{
  int result;
  color_test_t available;

  if (event == EVENT_RESOLVE_SPELL)
  {
    result = get_id(player, card);
    get_color_of_mana_produced_by_id(result, 0x3e, player);
  }
  result = get_id(player, card);
  available = get_color_of_mana_produced_by_id(result, 0x3e, player);
  result = mana_producing_creature_all_one_color(player, card, event, 0, available, 1);
  return result;
}

/* ManalinkEh 2022: 0x22e0c40; original cards/amonkhet.c:3611. */
int card_oashra_cultivator(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_ACTIVATE)
  {
    if (1 - player == current_turn)
    {
      result = count_subtype(player, 1, -1);
      if (result < 4)
      {
        result = count_subtype(player, 2, -1);
        ai_modifier = ai_modifier + result * 8;
      }
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    tutor_basic_lands(player, TUTOR_PLAY_TAPPED, 1);
  }
  result = generic_activated_ability(player, card, event, 9, 2, 0, 0, 1, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22e1810; original cards/amonkhet.c:3859. */
int card_sixth_sense(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  instance = get_card_instance(player, card);
  if (-1 < instance->damage_target_player)
  {
    result = duh_mode(player);
    result = attached_creature_deals_damage(
        player, card, event,
        (result != 0) + (DDBM_MUST_DAMAGE_PLAYER | DDBM_REPORT_DAMAGE_DEALT |
                         DDBM_MUST_BE_COMBAT_DAMAGE | DDBM_TRIGGER_OPTIONAL));
    if (result != 0)
    {
      draw_cards(player, 1);
    }
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_022e1850;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_022e1850;
    if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         (((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)) &&
           (event != EVENT_CHANGE_TYPE)))) &&
        (((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)) &&
          (event != EVENT_STATIC_EFFECTS))))
    {
      return 0;
    }
  }
LAB_022e1850:
  result = vanilla_aura(player, card, event, player);
  return result;
}

/* ManalinkEh 2022: 0x22e1b10; original cards/amonkhet.c:3911. */
int card_synchronized_strike(int player, int card, event_t event)
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
    td.allow_cancel = 3;
    get_card_instance(player, card);
    iVar5 = generic_spell(player, card, EVENT_CAN_CAST, 0x20, &td, "TARGET_CREATURE", 1, NULL);
    return iVar5;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 2);
      td.allow_cancel = 3;
      instance = get_card_instance(player, card);
      iVar5 = check_special_flags2(player, card, 0x2000);
      if (iVar5 == 0)
      {
        check_special_flags2(player, card, 0x8000);
      }
      iVar5 = new_pick_target(&td, "TARGET_CREATURE", 0, 0);
      if (iVar5 != 0)
      {
        state_untargettable(instance->targets[0].player, instance->targets[0].card, 1);
      }
      iVar5 = can_target(&td);
      if (iVar5 != 0)
      {
        new_pick_target(&td, "TARGET_CREATURE", 1, 0);
        state_untargettable(instance->targets[0].player, instance->targets[0].card, 0);
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    td.allow_cancel = 3;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      iVar5 = 0;
      if (instance->number_of_targets != '\0')
      {
        do
        {
          while ((instance->targets[iVar5].card == -1 ||
                  (iVar4 = validate_target(player, card, &td, iVar5), iVar4 == 0)))
          {
            iVar5 = iVar5 + 1;
            if ((int)(unsigned int)instance->number_of_targets <= iVar5)
              goto LAB_022e1c18;
          }
          untap_card(instance->targets[iVar5].player, instance->targets[iVar5].card);
          ptVar2 = instance->targets + iVar5;
          ptVar1 = instance->targets + iVar5;
          iVar5 = iVar5 + 1;
          pump_until_eot(player, card, ptVar1->player, ptVar2->card, 2, 2);
        } while (iVar5 < (int)(unsigned int)instance->number_of_targets);
      }
    LAB_022e1c18:
      kill_card(player, card, KILL_DESTROY);
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td, 2);
    td.allow_cancel = 3;
    get_card_instance(player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x22e1e30; original cards/amonkhet.c:3996. */
int card_trial_of_strength(int player, int card, event_t event)
{
  int result;
  token_generation_t token;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_token_definition(player, card, 0x4907, &token);
    token.pow = 4;
    token.tou = 2;
    generate_token(&token);
  }
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition((test_definition_t *)&token, 4);
    result = new_specific_cip(player, card, event, player, 2, (test_definition_t *)&token);
    if (result != 0)
    {
      bounce_permanent(player, card);
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22e2bf0; original cards/amonkhet.c:4283. */
int card_khenra_charioteer(int player, int card, event_t event)
{
  boost_creature_type(player, card, event, ~SUBTYPE_INVALID, 0, 0, 0x80, BCT_CONTROLLER_ONLY);
  return (unsigned int)(event == EVENT_CAN_CAST);
}

/* ManalinkEh 2022: 0x22e2de0; original cards/amonkhet.c:4318. */
int card_neheb_the_worthy(int player, int card, event_t event)
{
  int result;
  check_legend_rule(player, card, event);
  boost_subtype(player, card, event, SUBTYPE_MINOTAUR, 0, 0, 0x100, 0, BCT_CONTROLLER_ONLY);
  if (hand_count[player] < 2)
  {
    boost_subtype(player, card, event, SUBTYPE_MINOTAUR, 2, 0, 0, 0,
                  BCT_INCLUDE_SELF | BCT_CONTROLLER_ONLY);
  }
  result =
      damage_dealt_by_me(player, card, event, DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_BE_COMBAT_DAMAGE);
  if (result != 0)
  {
    discard(current_turn, 0, player);
    discard(current_turn ^ 1, 0, player);
  }
  return 0;
}

/* ManalinkEh 2022: 0x22e33e0; original cards/amonkhet.c:4435. */
int card_samut_voice_of_dissent(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  check_legend_rule(player, card, event);
  if (event == EVENT_ABILITIES)
  {
    vigilance(player, card, EVENT_ABILITIES);
    haste(player, card, EVENT_ABILITIES);
    boost_subtype(player, card, EVENT_ABILITIES, ~SUBTYPE_INVALID, 0, 0, 0, 4, BCT_CONTROLLER_ONLY);
  }
  else if ((((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
            (event == EVENT_RESOLVE_ACTIVATION)) ||
           (event == EVENT_CLEANUP))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.illegal_abilities = 0;
    td.special = 0x4000;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        instance = get_card_instance(player, card);
        result = instance->targets[0].card;
        instance = get_card_instance(player, card);
        untap_card(instance->targets[0].player, result);
      }
    }
    result = generic_activated_ability(player, card, event, 5, 0, 0, 0, 0, 0, 1, 0, &td,
                                       "SELECT_ANOTHER_CREATURE");
    return result;
  }
  result = flash(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22e36e0; original cards/amonkhet.c:4510. */
int card_wayward_servant(int player, int card, event_t event)
{
  test_definition_t test;
  if ((((trigger_condition == 0xdb) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (reason_for_trigger_controller == player))
  {
    new_default_test_definition(&test, 0x47, "");
    test.subtype = 0xde;
    test.not_me = 1;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event != EVENT_ETB)
  {
    return 0;
  }
  lose_life(1 - player, 1);
  gain_life(player, 1);
  return 0;
}

/* ManalinkEh 2022: 0x22e8210; original cards/amonkhet.c:5836. */
int card_throne_of_the_god_pharaoh(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  check_legend_rule(player, card, event);
  if (current_turn == player)
  {
    result = eot_trigger(player, card, event);
    if (result != 0)
    {
      default_test_definition(&this_test, 2);
      this_test.state = 0x10;
      result = check_battlefield_for_special_card(player, card, player, 4, &this_test);
      lose_life(1 - player, result);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22e8be0; original cards/amonkhet.c:6062. */
int card_sunscorched_desert(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 0x100);
    td.zone = 0x1200;
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    result = can_target(&td);
    if (result != 0)
    {
      result = pick_target(&td, "TARGET_PLAYER_OR_PLANESWALKER");
      if (result != 0)
      {
        damage_target0(player, card, 1);
      }
    }
    instance->number_of_targets = '\0';
  }
  result = mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22e9080; original cards/amonkhet.c:6166. */
int card_companion_of_the_trials(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = count_subtype(player, 0x47, 0x7005);
      if (result < 1)
      {
        return 0;
      }
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 1, 0, 0, 0, 0, 1, 0,
                                         &td, "TARGET_CREATURE");
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {
      result = count_subtype(player, 0x47, 0x7005);
      if (0 < result)
      {
        result = generic_activated_ability(player, card, EVENT_ACTIVATE, 4, 1, 0, 0, 0, 0, 1, 0,
                                           &td, "TARGET_CREATURE");
        return result;
      }
      if (spell_fizzled == 1)
      {
        return 0;
      }
      ai_mod_for_untap_effects(player, card, 0, 0);
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
    untap_card(instance->targets[0].player, instance->targets[0].card);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x22e92a0; original cards/amonkhet.c:6207. */
int card_gideons_resolve(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    new_default_test_definition(&test, 0xff, "Select a card named Gideon, Martial Paragon.");
    test.id = 0x4217;
    result = new_global_tutor(player, player, 1, 1, 0, 5, &test);
    if (result < 0)
    {
      new_global_tutor(player, player, 2, 1, 0, 5, &test);
    }
  }
  boost_creature_type(player, card, event, ~SUBTYPE_INVALID, 1, 1, 0, BCT_CONTROLLER_ONLY);
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22e9a10; original cards/amonkhet.c:6359. */
int card_lilianas_influence(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (event == EVENT_RESOLVE_SPELL)
  {
    manipulate_type(player, card, 1 - player, TYPE_CREATURE, 0x1010001);
    new_default_test_definition(&test, 0xff, "Select a card named Liliana, Death Wielder.");
    test.id = 0x4241;
    result = new_global_tutor(player, player, 1, 1, 0, 5, &test);
    if (result < 0)
    {
      new_global_tutor(player, player, 2, 1, 0, 5, &test);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}
