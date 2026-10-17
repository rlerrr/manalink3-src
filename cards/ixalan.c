#include "manalink.h"

/* Adanto Vanguard	|1|W
 * Creature - Vampire Soldier 1/1
 * As long as ~ is attacking, it gets +2/+0.
 * Pay 4 life: ~ gains indestructible until end of turn. */

/* Ashes of the Abhorrent	|1|W
 * Enchantment
 * Players can't cast spells from graveyards or activate abilities of cards in graveyards.
 * Whenever a creature dies, you gain 1 life. */

/* Axis of Mortality	|4|W|W
 * Enchantment
 * At the beginning of your upkeep, you may have two target players exchange life totals. */

/* Bellowing Aegisaur	|5|W
 * Creature - Dinosaur 3/5
 * Enrage - Whenever ~ is dealt damage, put a +1/+1 counter on each other creature you control. */

/* Bishop of Rebirth	|3|W|W
 * Creature - Vampire Cleric 3/4
 * Vigilance
 * Whenever ~ attacks, you may return target creature card with converted mana cost 3 or less from your graveyard to the battlefield. */

/* Bishop's Soldier	|1|W	=>m10.c:card_child_of_night 0x200415c
 * Creature - Vampire Soldier 2/2
 * Lifelink */

/* Bright Reprisal	|4|W
 * Instant
 * Destroy target attacking creature.
 * Draw a card. */

// Demystify: reprint from Onslaught

/* Duskborne Skymarcher	|W
 * Creature - Vampire Cleric 1/1
 * Flying
 * |W, |T: Target attacking Vampire gets +1/+1 until end of turn. */

/* Emissary of Sunrise	|2|W
 * Creature - Human Cleric 2/1
 * First strike
 * When ~ enters the battlefield, it explores. */

/* Encampment Keeper	|W
 * Creature - Hound 1/1
 * First strike
 * |7|W, |T, Sacrifice ~: Creatures you control get +2/+2 until end of turn. */

/* Glorifier of Dusk	|3|W|W
 * Creature - Vampire Soldier 4/4
 * Pay 2 life: ~ gains flying until end of turn.
 * Pay 2 life: ~ gains vigilance until end of turn. */

/* Goring Ceratops	|5|W|W
 * Creature - Dinosaur 3/3
 * Double strike
 * Whenever ~ attacks, other creatures you control gain double strike until end of turn. */

/* Imperial Aerosaur	|3|W
 * Creature - Dinosaur 3/3
 * Flying
 * When ~ enters the battlefield, another target creature you control gets +1/+1 and gains flying until end of turn. */

/* Imperial Lancer	|W
 * Creature - Human Knight 1/1
 * ~ has double strike as long as you control a Dinosaur. */

/* Inspiring Cleric	|2|W	=>rise_of_the_eldrazi.c:card_lone_missionary 0x20056dd
 * Creature - Vampire Cleric 3/2
 * When ~ enters the battlefield, you gain 4 life. */

/* Ixalan's Binding	|3|W
 * Enchantment
 * When ~ enters the battlefield, exile target nonland permanent an opponent controls until ~ leaves the battlefield.
 * Your opponents can't cast spells with the same name as the exiled card. */

/* Kinjalli's Caller	|W
 * Creature - Human Cleric 0/3
 * Dinosaur spells you cast cost |1 less to cast. */

/* Kinjalli's Sunwing	|2|W	=>m14.c:card_imposing_sovereign 0x200aa84
 * Creature - Dinosaur 2/3
 * Flying
 * Creatures your opponents control enter the battlefield tapped. */

/* Legion Conquistador	|2|W
 * Creature - Vampire Soldier 2/2
 * When ~ enters the battlefield, you may search your library for any number of cards named ~, reveal them, put them into your hand, then shuffle your library. */

/* Legion's Judgment	|2|W	=>innistrad.c:card_smite_the_monstrous 0x2004a44
 * Sorcery
 * Destroy target creature with power 4 or greater. */

/* Legion's Landing	|W
 * Legendary Enchantment
 * When ~ enters the battlefield, create a 1/1 |Swhite Vampire creature token with lifelink.
 * When you attack with three or more creatures, transform ~. */
/* Adanto, the First Fort	""
 * Legendary Land
 * |T: Add |W.
 * |2|W, |T: Create a 1/1 |Swhite Vampire creature token with lifelink. */

/* Looming Altisaur	|3|W	0x401000
 * Creature - Dinosaur 1/7 */

/* Mavren Fein, Dusk Apostle	|2|W
 * Legendary Creature - Vampire Cleric 2/2
 * Whenever one or more nontoken Vampires you control attack, create a 1/1 |Swhite Vampire creature token with lifelink. */

/* Paladin of the Bloodstained	|3|W
 * Creature - Vampire Knight 3/2
 * When ~ enters the battlefield, create a 1/1 |Swhite Vampire creature token with lifelink. */

/* Pious Interdiction	|3|W
 * Enchantment - Aura
 * Enchant creature
 * When ~ enters the battlefield, you gain 2 life.
 * Enchanted creature can't attack or block. */

/* Priest of the Wakening Sun	|W
 * Creature - Human Cleric 1/1
 * At the beginning of your upkeep, you may reveal a Dinosaur card from your hand. If you do, you gain 2 life.
 * |3|W|W, Sacrifice ~: Search your library for a Dinosaur card, reveal it, put it into your hand, then shuffle your library. */

/* Pterodon Knight	|3|W
 * Creature - Human Knight 3/3
 * ~ has flying as long as you control a Dinosaur. */

/* Queen's Commission	|2|W
 * Sorcery
 * Create two 1/1 |Swhite Vampire creature tokens with lifelink. */

/* Rallying Roar	|2|W
 * Instant
 * Creatures you control get +1/+1 until end of turn. Untap them. */

/* Raptor Companion	|1|W	0x401000
 * Creature - Dinosaur 3/1 */

/* Ritual of Rejuvenation	|2|W
 * Instant
 * You gain 4 life.
 * Draw a card. */

/* Sanguine Sacrament	|X|W|W
 * Instant
 * You gain twice X life. Put ~ on the bottom of its owner's library. */

/* Settle the Wreckage	|2|W|W
 * Instant
 * Exile all attacking creatures target player controls. That player may search their library for that many basic land cards, put those cards onto the battlefield tapped, then shuffle their library. */

/* Sheltering Light	|W
 * Instant
 * Target creature gains indestructible until end of turn. Scry 1. */

/* Shining Aerosaur	|4|W	0x401000
 * Creature - Dinosaur 3/4
 * Flying */

/* Skyblade of the Legion	|1|W	0x401000
 * Creature - Vampire Soldier 1/3
 * Flying */

/* Slash of Talons	|W	=>avacyn_restored.c:card_righteous_blow 0x2005cff
 * Instant
 * ~ deals 2 damage to target attacking or blocking creature. */

/* Steadfast Armasaur	|3|W
 * Creature - Dinosaur 2/3
 * Vigilance
 * |1|W, |T: ~ deals damage equal to its toughness to target creature blocking or blocked by it. */

/* Sunrise Seeker	|4|W
 * Creature - Human Scout 3/3
 * Vigilance
 * When ~ enters the battlefield, it explores. */

/* Territorial Hammerskull	|2|W
 * Creature - Dinosaur 2/3
 * Whenever ~ attacks, tap target creature an opponent controls. */

/* Tocatli Honor Guard	|1|W	=>0x401000; =CARD_ID_TORPOR_ORB
 * Creature - Human Soldier 1/3
 * Creatures entering the battlefield don't cause abilities to trigger. */

/* Vampire's Zeal	|W
 * Instant
 * Target creature gets +2/+2 until end of turn. If it's a Vampire, it gains first strike until end of turn. */

/* Wakening Sun's Avatar	|5|W|W|W
 * Creature - Dinosaur Avatar 7/7
 * When ~ enters the battlefield, if you cast it from your hand, destroy all non-Dinosaur creatures. */

// Air Elemental: reprint from Alpha

/* Arcane Adaptation	|2|U
 * Enchantment
 * As ~ enters the battlefield, choose a creature type.
 * Creatures you control are the chosen type in addition to their other types. The same is true for creature spells you control and creature cards you own that aren't on the battlefield. */

// Cancel: reprint from Time Spiral

/* Chart a Course	|1|U
 * Sorcery
 * Draw two cards. Then discard a card unless you attacked with a creature this turn. */

/* Daring Saboteur	|1|U
 * Creature - Human Pirate 2/1
 * |2|U: ~ can't be blocked this turn.
 * Whenever ~ deals combat damage to a player, you may draw a card. If you do, discard a card. */

/* Deadeye Quartermaster	|3|U
 * Creature - Human Pirate 2/2
 * When ~ enters the battlefield, you may search your library for an Equipment or Vehicle card, reveal it, put it into your hand, then shuffle your library. */

/* Deeproot Waters	|2|U
 * Enchantment
 * Whenever you cast a Merfolk spell, create a 1/1 |Sblue Merfolk creature token with hexproof. */

/* Depths of Desire	|2|U
 * Instant
 * Return target creature to its owner's hand. Create a colorless Treasure artifact token with "|T, Sacrifice this artifact: Add one mana of any color." */

/* Dive Down	|U	=>dragons_of_tarkir.c:card_glint 0x200da27
 * Instant
 * Target creature you control gets +0/+3 and gains hexproof until end of turn. */

/* Dreamcaller Siren	|2|U|U
 * Creature - Siren Pirate 3/3
 * Flash
 * Flying
 * ~ can block only creatures with flying.
 * When ~ enters the battlefield, if you control another Pirate, tap up to two target nonland permanents. */

/* Entrancing Melody	|X|U|U
 * Sorcery
 * Gain control of target creature with converted mana cost X. */

// Favorable Winds: reprint from Avacyn Restored

/* Fleet Swallower	|5|U|U
 * Creature - Fish 6/6
 * Whenever ~ attacks, target player puts the top half of their library, rounded up, into their graveyard. */

/* Headwater Sentries	|3|U	0x401000
 * Creature - Merfolk Warrior 2/5 */

/* Herald of Secret Streams	|3|U
 * Creature - Merfolk Warrior 2/3
 * Creatures you control with +1/+1 counters on them can't be blocked. */

/* Jace, Cunning Castaway	|1|U|U
 * Legendary Planeswalker - Jace (3)
 * +1: Whenever one or more creatures you control deal combat damage to a player this turn, draw a card, then discard a card.
 * -2: Create a 2/2 |Sblue Illusion creature token with "When this creature becomes the target of a spell, sacrifice it."
 * -5: Create two tokens that are copies of ~, except they're not legendary. */

/* Kopala, Warden of Waves	|1|U|U
 * Legendary Creature - Merfolk Wizard 2/2
 * Spells your opponents cast that target a Merfolk you control cost |2 more to cast.
 * Abilities your opponents activate that target a Merfolk you control cost |2 more to activate. */

/* Lookout's Dispersal	|2|U
 * Instant
 * This spell costs |1 less to cast if you control a Pirate.
 * Counter target spell unless its controller pays |4. */

/* Navigator's Ruin	|2|U
 * Enchantment
 * Raid - At the beginning of your end step, if you attacked with a creature this turn, target opponent puts the top four cards of their library into their graveyard. */

/* One With the Wind	|1|U	=>innistrad.c:card_spectral_flight 0x2004a4e
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature gets +2/+2 and has flying. */

// Opt: reprint from Invasion

/* Overflowing Insight	|4|U|U|U
 * Sorcery
 * Target player draws seven cards. */

/* Perilous Voyage	|1|U
 * Instant
 * Return target nonland permanent you don't control to its owner's hand. If its converted mana cost was 2 or less, scry 2. */

/* Pirate's Prize	|3|U
 * Sorcery
 * Draw two cards. Create a colorless Treasure artifact token with "|T, Sacrifice this artifact: Add one mana of any color." */

/* Prosperous Pirates	|4|U
 * Creature - Human Pirate 3/4
 * When ~ enters the battlefield, create two colorless Treasure artifact tokens with "|T, Sacrifice this artifact: Add one mana of any color." */

/* River Sneak	|1|U
 * Creature - Merfolk Warrior 1/1
 * ~ can't be blocked.
 * Whenever another Merfolk enters the battlefield under your control, ~ gets +1/+1 until end of turn. */

/* River's Rebuke	|4|U|U
 * Sorcery
 * Return all nonland permanents target player controls to their owner's hand. */

/* Run Aground	|3|U
 * Instant
 * Put target artifact or creature on top of its owner's library. */

/* Sailor of Means	|2|U
 * Creature - Human Pirate 1/4
 * When ~ enters the battlefield, create a colorless Treasure artifact token with "|T, Sacrifice this artifact: Add one mana of any color." */

/* Search for Azcanta	|1|U
 * Legendary Enchantment
 * At the beginning of your upkeep, look at the top card of your library. You may put it into your graveyard. Then if you have seven or more cards in your graveyard, you may transform ~. */
/* Azcanta, the Sunken Ruin	""
 * Legendary Land
 * |T: Add |U.
 * |2|U, |T: Look at the top four cards of your library. You may reveal a noncreature, nonland card from among them and put it into your hand. Put the rest on the bottom of your library in any order. */

/* Shaper Apprentice	|1|U
 * Creature - Merfolk Wizard 2/1
 * ~ has flying as long as you control another Merfolk. */

/* Shipwreck Looter	|1|U
 * Creature - Human Pirate 2/1
 * Raid - When ~ enters the battlefield, if you attacked with a creature this turn, you may draw a card. If you do, discard a card. */

/* Shore Keeper	|U
 * Creature - Trilobite 0/3
 * |7|U, |T, Sacrifice ~: Draw three cards. */

/* Siren Lookout	|2|U
 * Creature - Siren Pirate 1/2
 * Flying
 * When ~ enters the battlefield, it explores. */

/* Siren Stormtamer	|U
 * Creature - Siren Pirate Wizard 1/1
 * Flying
 * |U, Sacrifice ~: Counter target spell or ability that targets you or a creature you control. */

/* Siren's Ruse	|1|U
 * Instant
 * Exile target creature you control, then return that card to the battlefield under its owner's control. If a Pirate was exiled this way, draw a card. */

// Spell Pierce: reprint from Zendikar

/* Spell Swindle	|3|U|U
 * Instant
 * Counter target spell. Create X colorless Treasure artifact tokens, where X is that spell's converted mana cost. They have "|T, Sacrifice this artifact: Add one mana of any color." */

/* Storm Fleet Aerialist	|1|U
 * Creature - Human Pirate 1/2
 * Flying
 * Raid - ~ enters the battlefield with a +1/+1 counter on it if you attacked with a creature this turn. */

/* Storm Fleet Spy	|2|U
 * Creature - Human Pirate 2/2
 * Raid - When ~ enters the battlefield, if you attacked with a creature this turn, draw a card. */

/* Storm Sculptor	|3|U
 * Creature - Merfolk Wizard 3/2
 * ~ can't be blocked.
 * When ~ enters the battlefield, return a creature you control to its owner's hand. */

/* Tempest Caller	|2|U|U
 * Creature - Merfolk Wizard 2/3
 * When ~ enters the battlefield, tap all creatures target opponent controls. */

/* Watertrap Weaver	|2|U	=>m15.c:card_frost_lynx 0x200c8c0
 * Creature - Merfolk Wizard 2/2
 * When ~ enters the battlefield, tap target creature an opponent controls. That creature doesn't untap during its controller's next untap step. */

/* Wind Strider	|4|U	=>time_spiral.c:card_ashcoat_bear 0x200cb63
 * Creature - Merfolk Wizard 3/3
 * Flash
 * Flying */

/* Anointed Deacon	|4|B
 * Creature - Vampire Cleric 3/3
 * At the beginning of combat on your turn, you may have target Vampire get +2/+0 until end of turn. */

/* Arguel's Blood Fast	|1|B
 * Legendary Enchantment
 * |1|B, Pay 2 life: Draw a card.
 * At the beginning of your upkeep, if you have 5 or less life, you may transform ~. */
/* Temple of Aclazotz	""
 * Legendary Land
 * |T: Add |B.
 * |T, Sacrifice a creature: You gain life equal to the sacrificed creature's toughness. */

/* Bishop of the Bloodstained	|3|B|B
 * Creature - Vampire Cleric 3/3
 * When ~ enters the battlefield, target opponent loses 1 life for each Vampire you control. */

/* Blight Keeper	|B
 * Creature - Bat Imp 1/1
 * Flying
 * |7|B, |T, Sacrifice ~: Target opponent loses 4 life and you gain 4 life. */

/* Bloodcrazed Paladin	|1|B
 * Creature - Vampire Knight 1/1
 * Flash
 * ~ enters the battlefield with a +1/+1 counter on it for each creature that died this turn. */

/* Boneyard Parley	|5|B|B
 * Sorcery
 * Exile up to five target creature cards from graveyards. An opponent separates those cards into two piles. Put all cards from the pile of your choice onto the battlefield under your control and the rest into their owners' graveyards. */

/* Contract Killing	|3|B|B
 * Sorcery
 * Destroy target creature. Create two colorless Treasure artifact tokens with "|T, Sacrifice this artifact: Add one mana of any color." */

/* Costly Plunder	|1|B
 * Instant
 * As an additional cost to cast this spell, sacrifice an artifact or creature.
 * Draw two cards. */

/* Dark Nourishment	|4|B	=>darksteel.c:card_essence_drain 0x2006768
 * Instant
 * ~ deals 3 damage to any target. You gain 3 life. */

/* Deadeye Tormentor	|2|B
 * Creature - Human Pirate 2/2
 * Raid - When ~ enters the battlefield, if you attacked with a creature this turn, target opponent discards a card. */

/* Deadeye Tracker	|B
 * Creature - Human Pirate 1/1
 * |1|B, |T: Exile two target cards from an opponent's graveyard. ~ explores. */

/* Deathless Ancient	|4|B|B
 * Creature - Vampire Knight 4/4
 * Flying
 * Tap three untapped Vampires you control: Return ~ from your graveyard to your hand. */

/* Desperate Castaways	|1|B
 * Creature - Human Pirate 2/3
 * ~ can't attack unless you control an artifact. */

/* Dire Fleet Hoarder	|1|B
 * Creature - Human Pirate 2/1
 * When ~ dies, create a colorless Treasure artifact token with "|T, Sacrifice this artifact: Add one mana of any color." */

/* Dire Fleet Interloper	|3|B
 * Creature - Human Pirate 2/2
 * Menace
 * When ~ enters the battlefield, it explores. */

/* Dire Fleet Ravager	|3|B|B
 * Creature - Orc Pirate Wizard 4/4
 * Menace, deathtouch
 * When ~ enters the battlefield, each player loses a third of their life, rounded up. */

// Duress: reprint from Urza's Saga

/* Fathom Fleet Captain	|1|B
 * Creature - Human Pirate 2/1
 * Menace
 * Whenever ~ attacks, if you control another nontoken Pirate, you may pay |2. If you do, create a 2/2 |Sblack Pirate creature token with menace. */

/* Fathom Fleet Cutthroat	|3|B
 * Creature - Human Pirate 3/3
 * When ~ enters the battlefield, destroy target creature an opponent controls that was dealt damage this turn. */

/* Grim Captain's Call	|2|B
 * Sorcery
 * Return a Pirate card from your graveyard to your hand, then do the same for Vampire, Dinosaur, and Merfolk. */

/* Heartless Pillage	|2|B
 * Sorcery
 * Target opponent discards two cards.
 * Raid - If you attacked with a creature this turn, create a colorless Treasure artifact token with "|T, Sacrifice this artifact: Add one mana of any color." */

/* Kitesail Freebooter	|1|B
 * Creature - Human Pirate 1/2
 * Flying
 * When ~ enters the battlefield, target opponent reveals their hand. You choose a noncreature, nonland card from it. Exile that card until ~ leaves the battlefield. */

/* Lurking Chupacabra	|3|B
 * Creature - Beast Horror 2/3
 * Whenever a creature you control explores, target creature an opponent controls gets -2/-2 until end of turn. */

/* March of the Drowned	|B
 * Sorcery
 * Choose one -
 * * Return target creature card from your graveyard to your hand.
 * * Return two target Pirate cards from your graveyard to your hand. */

// Mark of the Vampire: reprint from Magic 2013

/* Queen's Agent	|5|B
 * Creature - Vampire Scout 3/3
 * Lifelink
 * When ~ enters the battlefield, it explores. */

/* Queen's Bay Soldier	|1|B	0x401000
 * Creature - Vampire Soldier 2/2 */

/* Raiders' Wake	|3|B
 * Enchantment
 * Whenever an opponent discards a card, that player loses 2 life.
 * Raid - At the beginning of your end step, if you attacked with a creature this turn, target opponent discards a card. */

/* Revel in Riches	|4|B
 * Enchantment
 * Whenever a creature an opponent controls dies, create a colorless Treasure artifact token with "|T, Sacrifice this artifact: Add one mana of any color."
 * At the beginning of your upkeep, if you control ten or more Treasures, you win the game. */

/* Ruin Raider	|2|B
 * Creature - Orc Pirate 3/2
 * Raid - At the beginning of your end step, if you attacked with a creature this turn, reveal the top card of your library and put that card into your hand. You lose life equal to the card's converted mana cost. */

/* Ruthless Knave	|2|B
 * Creature - Orc Pirate 3/2
 * |2|B, Sacrifice a creature: Create two colorless Treasure artifact tokens with "|T, Sacrifice this artifact: Add one mana of any color."
 * Sacrifice three Treasures: Draw a card. */

/* Sanctum Seeker	|2|B|B
 * Creature - Vampire Knight 3/4
 * Whenever a Vampire you control attacks, each opponent loses 1 life and you gain 1 life. */

/* Seekers' Squire	|1|B
 * Creature - Human Scout 1/2
 * When ~ enters the battlefield, it explores. */

/* Skittering Heartstopper	|B	=>morningtide.c:card_moonglove_changeling 0x2007b63
 * Creature - Insect 1/2
 * |B: ~ gains deathtouch until end of turn. */

/* Skulduggery	|B
 * Instant
 * Until end of turn, target creature you control gets +1/+1 and target creature an opponent controls gets -1/-1. */

/* Skymarch Bloodletter	|2|B
 * Creature - Vampire Soldier 2/2
 * Flying
 * When ~ enters the battlefield, target opponent loses 1 life and you gain 1 life. */

/* Spreading Rot	|4|B
 * Sorcery
 * Destroy target land. Its controller loses 2 life. */

/* Sword-Point Diplomacy	|2|B
 * Sorcery
 * Reveal the top three cards of your library. For each of those cards, put that card into your hand unless any opponent pays 3 life. Then exile the rest. */

/* Vanquish the Weak	|2|B	=>origins.c:card_reave_soul 0x200dd01
 * Instant
 * Destroy target creature with power 3 or less. */

/* Vicious Conquistador	|B	=>worldwake.c:card_pulse_tracker 0x200292e
 * Creature - Vampire Soldier 1/2
 * Whenever ~ attacks, each opponent loses 1 life. */

/* Vraska's Contempt	|2|B|B
 * Instant
 * Exile target creature or planeswalker. You gain 2 life. */

/* Walk the Plank	|B|B
 * Sorcery
 * Destroy target non-Merfolk creature. */

/* Wanted Scoundrels	|1|B
 * Creature - Human Pirate 4/3
 * When ~ dies, target opponent creates two colorless Treasure artifact tokens with "|T, Sacrifice this artifact: Add one mana of any color." */

/* Angrath's Marauders	|5|R|R
 * Creature - Human Pirate 4/4
 * If a source you control would deal damage to a permanent or player, it deals double that damage to that permanent or player instead. */

/* Bonded Horncrest	|3|R
 * Creature - Dinosaur 5/5
 * ~ can't attack or block alone. */

/* Brazen Buccaneers	|3|R
 * Creature - Human Pirate 2/2
 * Haste
 * When ~ enters the battlefield, it explores. */

/* Burning Sun's Avatar	|3|R|R|R
 * Creature - Dinosaur Avatar 6/6
 * When ~ enters the battlefield, it deals 3 damage to target opponent or planeswalker and 3 damage to up to one target creature. */

/* Captain Lannery Storm	|2|R
 * Legendary Creature - Human Pirate 2/2
 * Haste
 * Whenever ~ attacks, create a colorless Treasure artifact token with "|T, Sacrifice this artifact: Add one mana of any color."
 * Whenever you sacrifice a Treasure, ~ gets +1/+0 until end of turn. */

/* Captivating Crew	|3|R
 * Creature - Human Pirate 4/3
 * |3|R: Gain control of target creature an opponent controls until end of turn. Untap that creature. It gains haste until end of turn. Activate this ability only any time you could cast a sorcery. */

/* Charging Monstrosaur	|4|R	=>homelands.c:card_ambush_party	0x200bb46
 * Creature - Dinosaur 5/5
 * Trample, haste */

// Demolish: reprint from Odyssey

/* Dinosaur Stampede	|2|R
 * Instant
 * Attacking creatures get +2/+0 until end of turn. Dinosaurs you control gain trample until end of turn. */

// Dual Shot: reprint from Shadows over Innistrad

/* Fathom Fleet Firebrand	|1|R	=> card_wildfire_emissary 200bb0a
 * Creature - Human Pirate 2/2
 * |1|R: ~ gets +1/+0 until end of turn. */

/* Fiery Cannonade	|2|R
 * Instant
 * ~ deals 2 damage to each non-Pirate creature. */

/* Fire Shrine Keeper	|R
 * Creature - Elemental 1/1
 * Menace
 * |7|R, |T, Sacrifice ~: It deals 3 damage to each of up to two target creatures. */

/* Firecannon Blast	|1|R|R
 * Sorcery
 * ~ deals 3 damage to target creature.
 * Raid - ~ deals 6 damage to that creature instead if you attacked with a creature this turn. */

/* Frenzied Raptor	|2|R
 * Creature - Dinosaur 4/2 */

/* Headstrong Brute	|2|R
 * Creature - Orc Pirate 3/3
 * ~ can't block.
 * ~ has menace as long as you control another Pirate. */

// Hijack: reprint from Kaladesh

// Lightning Strike: reprint from Theros

/* Lightning-Rig Crew	|2|R
 * Creature - Goblin Pirate 0/5
 * |T: ~ deals 1 damage to each opponent.
 * Whenever you cast a Pirate spell, untap ~. */

/* Makeshift Munitions	|1|R
 * Enchantment
 * |1, Sacrifice an artifact or creature: ~ deals 1 damage to any target. */

/* Nest Robber	|1|R	=>homelands.c:card_ambush_party	0x200bb46
 * Creature - Dinosaur 2/1
 * Haste */

/* Otepec Huntmaster	|1|R
 * Creature - Human Shaman 1/2
 * Dinosaur spells you cast cost |1 less to cast.
 * |T: Target Dinosaur gains haste until end of turn. */

/* Rampaging Ferocidon	|2|R
 * Creature - Dinosaur 3/3
 * Menace
 * Players can't gain life.
 * Whenever another creature enters the battlefield, ~ deals 1 damage to that creature's controller. */

/* Raptor Hatchling	|1|R
 * Creature - Dinosaur 1/1
 * Enrage - Whenever ~ is dealt damage, create a 3/3 |Sgreen Dinosaur creature token with trample. */

/* Repeating Barrage	|1|R|R
 * Sorcery
 * ~ deals 3 damage to any target.
 * Raid - |3|R|R: Return ~ from your graveyard to your hand. Activate this ability only if you attacked with a creature this turn. */

/* Rigging Runner	|R
 * Creature - Goblin Pirate 1/1
 * First strike
 * Raid - ~ enters the battlefield with a +1/+1 counter on it if you attacked with a creature this turn. */

/* Rile	|R
 * Sorcery
 * ~ deals 1 damage to target creature you control. That creature gains trample until end of turn.
 * Draw a card. */

/* Rowdy Crew	|2|R|R
 * Creature - Human Pirate 3/3
 * Trample
 * When ~ enters the battlefield, draw three cards, then discard two cards at random. If two cards that share a card type are discarded this way, put two +1/+1 counters on ~. */

// Rummaging Goblin: reprint from Magic 2013

/* Star of Extinction	|5|R|R
 * Sorcery
 * Destroy target land. ~ deals 20 damage to each creature and each planeswalker. */

/* Storm Fleet Arsonist	|4|R
 * Creature - Orc Pirate 4/4
 * Raid - When ~ enters the battlefield, if you attacked with a creature this turn, target opponent sacrifices a permanent. */

/* Storm Fleet Pyromancer	|4|R	=>khans_of_tarkir.c:card_mardu_heart_piercer 0x200cea6
 * Creature - Human Pirate Wizard 3/2
 * Raid - When ~ enters the battlefield, if you attacked with a creature this turn, ~ deals 2 damage to any target. */

/* Sun-Crowned Hunters	|4|R|R
 * Creature - Dinosaur 5/4
 * Enrage - Whenever ~ is dealt damage, it deals 3 damage to target opponent or planeswalker. */

/* Sunbird's Invocation	|5|R
 * Enchantment
 * Whenever you cast a spell from your hand, reveal the top X cards of your library, where X is that spell's converted mana cost. You may cast a card revealed this way with converted mana cost X or less without paying its mana cost. Put the rest on the bottom of your library in a random order. */

// Sure Strike: reprint from Battle for Zendikar

/* Swashbuckling	|1|R	=>zendikar.c:card_goblin_war_paint 0x2001b2d
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature gets +2/+2 and has haste. */

/* Thrash of Raptors	|3|R
 * Creature - Dinosaur 3/3
 * As long as you control another Dinosaur, ~ gets +2/+0 and has trample. */

/* Tilonalli's Knight	|1|R
 * Creature - Human Knight 2/2
 * Whenever ~ attacks, if you control a Dinosaur, ~ gets +1/+1 until end of turn. */

/* Tilonalli's Skinshifter	|2|R
 * Creature - Human Shaman 0/1
 * Haste
 * Whenever ~ attacks, it becomes a copy of another target nonlegendary attacking creature until end of turn. */

/* Trove of Temptation	|3|R
 * Enchantment
 * Each opponent must attack you or a planeswalker you control with at least one creature each combat if able.
 * At the beginning of your end step, create a colorless Treasure artifact token with "|T, Sacrifice this artifact: Add one mana of any color." */

/* Unfriendly Fire	|4|R	=>tempest.c:card_lightning_blast 0x200cce4
 * Instant
 * ~ deals 4 damage to any target. */

/* Vance's Blasting Cannons	|3|R
 * Legendary Enchantment
 * At the beginning of your upkeep, exile the top card of your library. If it's a nonland card, you may cast that card this turn.
 * Whenever you cast your third spell in a turn, you may transform ~. */
/* Spitfire Bastion	""
 * Legendary Land
 * |T: Add |R.
 * |2|R, |T: ~ deals 3 damage to any target. */

/* Wily Goblin	|R|R
 * Creature - Goblin Pirate 1/1
 * When ~ enters the battlefield, create a colorless Treasure artifact token with "|T, Sacrifice this artifact: Add one mana of any color." */

/* Ancient Brontodon	|6|G|G	0x401000
 * Creature - Dinosaur 9/9 */

/* Atzocan Archer	|2|G
 * Creature - Human Archer 1/4
 * Reach
 * When ~ enters the battlefield, you may have it fight another target creature. */

/* Blinding Fog	|2|G
 * Instant
 * Prevent all damage that would be dealt to creatures this turn. Creatures you control gain hexproof until end of turn. */

/* Blossom Dryad	|2|G	=>unlimited.c:card_ley_druid 0x2003c39
 * Creature - Dryad 2/2
 * |T: Untap target land. */

/* Carnage Tyrant	|4|G|G
 * Creature - Dinosaur 7/6
 * This spell can't be countered.
 * Trample, hexproof */

/* Colossal Dreadmaw	|4|G|G	0x401000
 * Creature - Dinosaur 6/6
 * Trample */

/* Commune with Dinosaurs	|G
 * Sorcery
 * Look at the top five cards of your library. You may reveal a Dinosaur or land card from among them and put it into your hand. Put the rest on the bottom of your library in any order. */

/* Crash the Ramparts	|2|G	=>mirrodin.c:card_predators_strike 0x200532c
 * Instant
 * Target creature gets +3/+3 and gains trample until end of turn. */

/* Crushing Canopy	|2|G
 * Instant
 * Choose one -
 * * Destroy target creature with flying.
 * * Destroy target enchantment. */

/* Deathgorge Scavenger	|2|G
 * Creature - Dinosaur 3/2
 * Whenever ~ enters the battlefield or attacks, you may exile target card from a graveyard. If a creature card is exiled this way, you gain 2 life. If a noncreature card is exiled this way, ~ gets +1/+1 until end of turn. */

/* Deeproot Champion	|1|G
 * Creature - Merfolk Shaman 1/1
 * Whenever you cast a noncreature spell, put a +1/+1 counter on ~. */

/* Deeproot Warrior	|1|G
 * Creature - Merfolk Warrior 2/2
 * Whenever ~ becomes blocked, it gets +1/+1 until end of turn. */

/* Drover of the Mighty	|1|G
 * Creature - Human Druid 1/1
 * ~ gets +2/+2 as long as you control a Dinosaur.
 * |T: Add one mana of any color. */

/* Emergent Growth	|3|G
 * Sorcery
 * Target creature gets +5/+5 until end of turn and must be blocked this turn if able. */

/* Emperor's Vanguard	|3|G
 * Creature - Human Scout 4/3
 * Whenever ~ deals combat damage to a player, it explores. */

/* Grazing Whiptail	|2|G|G
 * Creature - Dinosaur 3/4
 * Reach */

/* Growing Rites of Itlimoc	|2|G
 * Legendary Enchantment
 * When ~ enters the battlefield, look at the top four cards of your library. You may reveal a creature card from among them and put it into your hand. Put the rest on the bottom of your library in any order.
 * At the beginning of your end step, if you control four or more creatures, transform ~. */
/* Itlimoc, Cradle of the Sun	""
 * Legendary Land
 * |T: Add |G.
 * |T: Add |G for each creature you control. */

/* Ixalli's Diviner	|1|G
 * Creature - Human Druid 0/3
 * When ~ enters the battlefield, it explores. */

/* Ixalli's Keeper	|1|G
 * Creature - Human Shaman 2/2
 * |7|G, |T, Sacrifice ~: Target creature gets +5/+5 and gains trample until end of turn. */

/* Jade Guardian	|3|G
 * Creature - Merfolk Shaman 2/2
 * Hexproof
 * When ~ enters the battlefield, put a +1/+1 counter on target Merfolk you control. */

/* Jungle Delver	|G
 * Creature - Merfolk Warrior 1/1
 * |3|G: Put a +1/+1 counter on ~. */

/* Kumena's Speaker	|G
 * Creature - Merfolk Shaman 1/1
 * ~ gets +1/+1 as long as you control another Merfolk or |Han Island. */

/* Merfolk Branchwalker	|1|G
 * Creature - Merfolk Scout 2/1
 * When ~ enters the battlefield, it explores. */

/* New Horizons	|2|G
 * Enchantment - Aura
 * Enchant land
 * When ~ enters the battlefield, put a +1/+1 counter on target creature you control.
 * Enchanted land has "|T: Add two mana of any one color." */

/* Old-Growth Dryads	|G
 * Creature - Dryad 3/3
 * When ~ enters the battlefield, each opponent may search their library for a basic land card, put it onto the battlefield tapped, then shuffle their library. */

/* Pounce	|1|G	=>innistrad.c:card_prey_upon 0x20049db
 * Instant
 * Target creature you control fights target creature you don't control. */

/* Ranging Raptors	|2|G
 * Creature - Dinosaur 2/3
 * Enrage - Whenever ~ is dealt damage, you may search your library for a basic land card, put it onto the battlefield tapped, then shuffle your library. */

/* Ravenous Daggertooth	|2|G
 * Creature - Dinosaur 3/2
 * Enrage - Whenever ~ is dealt damage, you gain 2 life. */

/* Ripjaw Raptor	|2|G|G
 * Creature - Dinosaur 4/5
 * Enrage - Whenever ~ is dealt damage, draw a card. */

/* River Heralds' Boon	|1|G
 * Instant
 * Put a +1/+1 counter on target creature and a +1/+1 counter on up to one target Merfolk. */

/* Savage Stomp	|2|G
 * Sorcery
 * This spell costs |2 less to cast if it targets a Dinosaur you control.
 * Put a +1/+1 counter on target creature you control. Then that creature fights target creature you don't control. */

/* Shapers' Sanctuary	|G
 * Enchantment
 * Whenever a creature you control becomes the target of a spell or ability an opponent controls, you may draw a card. */

// Slice in Twain: reprint from Scars of Mirrodin

/* Snapping Sailback	|4|G
 * Creature - Dinosaur 4/4
 * Flash
 * Enrage - Whenever ~ is dealt damage, put a +1/+1 counter on it. */

/* Spike-Tailed Ceratops	|4|G	=>unlimited.c:card_two_headed_giant_of_foriys 0x200b263
 * Creature - Dinosaur 4/4
 * ~ can block an additional creature each combat. */

/* Thundering Spineback	|5|G|G
 * Creature - Dinosaur 5/5
 * Other Dinosaurs you control get +1/+1.
 * |5|G: Create a 3/3 |Sgreen Dinosaur creature token with trample. */

/* Tishana's Wayfinder	|2|G
 * Creature - Merfolk Scout 2/2
 * When ~ enters the battlefield, it explores. */

/* Verdant Rebirth	|1|G
 * Instant
 * Until end of turn, target creature gains "When this creature dies, return it to its owner's hand."
 * Draw a card. */

/* Verdant Sun's Avatar	|5|G|G
 * Creature - Dinosaur Avatar 5/5
 * Whenever ~ or another creature enters the battlefield under your control, you gain life equal to that creature's toughness. */

/* Vineshaper Mystic	|2|G
 * Creature - Merfolk Shaman 1/3
 * When ~ enters the battlefield, put a +1/+1 counter on each of up to two target Merfolk you control. */

/* Waker of the Wilds	|2|G|G
 * Creature - Merfolk Shaman 3/3
 * |X|G|G: Put X +1/+1 counters on target land you control. That land becomes a 0/0 Elemental creature with haste. It's still a land. */

/* Wildgrowth Walker	|1|G
 * Creature - Elemental 1/3
 * Whenever a creature you control explores, put a +1/+1 counter on ~ and you gain 3 life. */

/* Admiral Beckett Brass	|1|U|B|R
 * Legendary Creature - Human Pirate 3/3
 * Other Pirates you control get +1/+1.
 * At the beginning of your end step, gain control of target nonland permanent controlled by a player who was dealt combat damage by three or more Pirates this turn. */

/* Belligerent Brontodon	|5|G|W
 * Creature - Dinosaur 4/6
 * Each creature you control assigns combat damage equal to its toughness rather than its power. */

/* Call to the Feast	|2|W|B
 * Sorcery
 * Create three 1/1 |Swhite Vampire creature tokens with lifelink. */

/* Deadeye Plunderers	|3|U|B
 * Creature - Human Pirate 3/3
 * ~ gets +1/+1 for each artifact you control.
 * |2|U|B: Create a colorless Treasure artifact token with "|T, Sacrifice this artifact: Add one mana of any color." */

/* Dire Fleet Captain	|B|R
 * Creature - Orc Pirate 2/2
 * Whenever ~ attacks, it gets +1/+1 until end of turn for each other attacking Pirate. */

/* Gishath, Sun's Avatar	|5|R|G|W
 * Legendary Creature - Dinosaur Avatar 7/6
 * Vigilance, trample, haste
 * Whenever ~ deals combat damage to a player, reveal that many cards from the top of your library. Put any number of Dinosaur creature cards from among them onto the battlefield and the rest on the bottom of your library in a random order. */

/* Hostage Taker	|2|U|B
 * Creature - Human Pirate 2/3
 * When ~ enters the battlefield, exile another target creature or artifact until ~ leaves the battlefield. You may cast that card for as long as it remains exiled, and you may spend mana as though it were mana of any type to cast that spell. */

/* Huatli, Warrior Poet	|3|R|W
 * Legendary Planeswalker - Huatli (3)
 * +2: You gain life equal to the greatest power among creatures you control.
 * 0: Create a 3/3 |Sgreen Dinosaur creature token with trample.
 * -X: ~ deals X damage divided as you choose among any number of target creatures. Creatures dealt damage this way can't block this turn. */

/* Marauding Looter	|2|U|R
 * Creature - Human Pirate 4/3
 * Raid - At the beginning of your end step, if you attacked with a creature this turn, you may draw a card. If you do, discard a card. */

/* Raging Swordtooth	|3|R|G
 * Creature - Dinosaur 5/5
 * Trample
 * When ~ enters the battlefield, it deals 1 damage to each other creature. */

/* Regisaur Alpha	|3|R|G
 * Creature - Dinosaur 4/4
 * Other Dinosaurs you control have haste.
 * When ~ enters the battlefield, create a 3/3 |Sgreen Dinosaur creature token with trample. */

/* Shapers of Nature	|1|G|U
 * Creature - Merfolk Shaman 3/3
 * |3|G: Put a +1/+1 counter on target creature.
 * |2|U, Remove a +1/+1 counter from a creature you control: Draw a card. */

/* Sky Terror	|R|W
 * Creature - Dinosaur 2/2
 * Flying, menace */

/* Tishana, Voice of Thunder	|5|G|U
 * Legendary Creature - Merfolk Shaman 100/100
 * ~'s power and toughness are each equal to the number of cards in your hand.
 * You have no maximum hand size.
 * When ~ enters the battlefield, draw a card for each creature you control. */

/* Vona, Butcher of Magan	|3|W|B
 * Legendary Creature - Vampire Knight 4/4
 * Vigilance, lifelink
 * |T, Pay 7 life: Destroy target nonland permanent. Activate this ability only during your turn. */

/* Vraska, Relic Seeker	|4|B|G
 * Legendary Planeswalker - Vraska (6)
 * +2: Create a 2/2 |Sblack Pirate creature token with menace.
 * -3: Destroy target artifact, creature, or enchantment. Create a colorless Treasure artifact token with "|T, Sacrifice this artifact: Add one mana of any color."
 * -10: Target player's life total becomes 1. */

// Cobbled Wings: reprint from Innistrad

/* Conqueror's Galleon	|4
 * Artifact - Vehicle 2/10
 * When ~ attacks, exile it at end of combat, then return it to the battlefield transformed under your control.
 * Crew 4 */
/* Conqueror's Foothold	""
 * Land
 * |T: Add |C.
 * |2, |T: Draw a card, then discard a card.
 * |4, |T: Draw a card.
 * |6, |T: Return target card from your graveyard to your hand. */

/* Dowsing Dagger	|2
 * Artifact - Equipment
 * When ~ enters the battlefield, target opponent creates two 0/2 |Sgreen Plant creature tokens with defender.
 * Equipped creature gets +2/+1.
 * Whenever equipped creature deals combat damage to a player, you may transform ~.
 * Equip |2 */
/* Lost Vale	""
 * Land
 * |T: Add three mana of any one color. */

/* Dusk Legion Dreadnought	|5
 * Artifact - Vehicle 4/6
 * Vigilance
 * Crew 2 */

/* Elaborate Firecannon	|2
 * Artifact
 * ~ doesn't untap during your untap step.
 * |4, |T: ~ deals 2 damage to any target.
 * At the beginning of your upkeep, you may discard a card. If you do, untap ~. */

/* Fell Flagship	|3
 * Artifact - Vehicle 3/3
 * Pirates you control get +1/+0.
 * Whenever ~ deals combat damage to a player, that player discards a card.
 * Crew 3 */

/* Gilded Sentinel	|4	0x401000
 * Artifact Creature - Golem 3/3 */

/* Hierophant's Chalice	|3
 * Artifact
 * When ~ enters the battlefield, target opponent loses 1 life and you gain 1 life.
 * |T: Add |C. */

/* Pillar of Origins	|2
 * Artifact
 * As ~ enters the battlefield, choose a creature type.
 * |T: Add one mana of any color. Spend this mana only to cast a creature spell of the chosen type. */

/* Pirate's Cutlass	|3
 * Artifact - Equipment
 * When ~ enters the battlefield, attach it to target Pirate you control.
 * Equipped creature gets +2/+1.
 * Equip |2 */

/* Primal Amulet	|4
 * Artifact
 * Instant and sorcery spells you cast cost |1 less to cast.
 * Whenever you cast an instant or sorcery spell, put a charge counter on ~. Then if there are four or more charge counters on it, you may remove those counters and transform it. */
/* Primal Wellspring	""
 * Land
 * |T: Add one mana of any color. When that mana is spent to cast an instant or sorcery spell, copy that spell and you may choose new targets for the copy. */

/* Prying Blade	|1
 * Artifact - Equipment
 * Equipped creature gets +1/+0.
 * Whenever equipped creature deals combat damage to a player, create a colorless Treasure artifact token with "|T, Sacrifice this artifact: Add one mana of any color."
 * Equip |2 */

/* Sentinel Totem	|1
 * Artifact
 * When ~ enters the battlefield, scry 1.
 * |T, Exile ~: Exile all cards from all graveyards. */

/* Shadowed Caravel	|2
 * Artifact - Vehicle 2/2
 * Whenever a creature you control explores, put a +1/+1 counter on ~.
 * Crew 2 */

/* Sleek Schooner	|3	=>kaladesh.c:card_sky_stiff	0x200f4c1
 * Artifact - Vehicle 4/3
 * Crew 1 */

/* Sorcerous Spyglass	|2
 * Artifact
 * As ~ enters the battlefield, look at an opponent's hand, then choose any card name.
 * Activated abilities of sources with the chosen name can't be activated unless they're mana abilities. */

/* Thaumatic Compass	|2
 * Artifact
 * |3, |T: Search your library for a basic land card, reveal it, put it into your hand, then shuffle your library.
 * At the beginning of your end step, if you control seven or more lands, transform ~. */
/* Spires of Orazca	""
 * Land
 * |T: Add |C.
 * |T: Untap target attacking creature an opponent controls and remove it from combat. */

/* Treasure Map	|2
 * Artifact
 * |1, |T: Scry 1. Put a landmark counter on ~. Then if there are three or more landmark counters on it, remove those counters, transform ~, and create three colorless Treasure artifact tokens with "|T, Sacrifice this artifact: Add one mana of any color." */
/* Treasure Cove	""
 * Land
 * |T: Add |C.
 * |T, Sacrifice a Treasure: Draw a card. */

/* Vanquisher's Banner	|5
 * Artifact
 * As ~ enters the battlefield, choose a creature type.
 * Creatures you control of the chosen type get +1/+1.
 * Whenever you cast a creature spell of the chosen type, draw a card. */

// Dragonskull Summit: reprint from Magic 2010

// Drowned Catacomb: reprint from Magic 2010

/* Field of Ruin	""
 * Land
 * |T: Add |C.
 * |2, |T, Sacrifice ~: Destroy target nonbasic land an opponent controls. Each player searches their library for a basic land card, puts it onto the battlefield, then shuffles their library. */

// Glacial Fortress: reprint from Magic 2010

// Rootbound Crag: reprint from Magic 2010

// Sunpetal Grove: reprint from Magic 2010

/* Unclaimed Territory	""
 * Land
 * As ~ enters the battlefield, choose a creature type.
 * |T: Add |C.
 * |T: Add one mana of any color. Spend this mana only to cast a creature spell of the chosen type. */

// Unknown Shores: reprint from Theros

// Plains
// Plains
// Plains
// Plains
// Island
// Island
// Island
// Island
// Swamp
// Swamp
// Swamp
// Swamp
// Mountain
// Mountain
// Mountain
// Mountain
// Forest
// Forest
// Forest
// Forest

/* Jace, Ingenious Mind-Mage	|4|U|U
 * Legendary Planeswalker - Jace (5)
 * +1: Draw a card.
 * +1: Untap all creatures you control.
 * -9: Gain control of up to three target creatures. */

/* Castaway's Despair	|3|U	=>innistrad.c:card_claustrophobia 0x200485f
 * Enchantment - Aura
 * Enchant creature
 * When ~ enters the battlefield, tap enchanted creature.
 * Enchanted creature doesn't untap during its controller's untap step. */

/* Grasping Current	|4|U
 * Sorcery
 * Return up to two target creatures to their owner's hand.
 * Search your library and/or graveyard for a card named Jace, Ingenious Mind-Mage, reveal it, and put it into your hand. If you searched your library this way, shuffle it. */

/* Jace's Sentinel	|1|U
 * Creature - Merfolk Warrior 1/3
 * As long as you control a Jace planeswalker, ~ gets +1/+0 and can't be blocked. */

// Woodland Stream: reprint from Shadows over Innistrad

/* Huatli, Dinosaur Knight	|4|R|W
 * Legendary Planeswalker - Huatli (4)
 * +2: Put two +1/+1 counters on up to one target Dinosaur you control.
 * -3: Target Dinosaur you control deals damage equal to its power to target creature you don't control.
 * -7: Dinosaurs you control get +4/+4 until end of turn. */

/* Huatli's Snubhorn	|1|W	=>unlimited.c:card_serra_angel 0x200c122
 * Creature - Dinosaur 2/2
 * Vigilance */

/* Huatli's Spurring	|R
 * Instant
 * Target creature gets +2/+0 until end of turn. If you control a Huatli planeswalker, that creature gets +4/+0 until end of turn instead. */

/* Sun-Blessed Mount	|3|R|W
 * Creature - Dinosaur 4/4
 * When ~ enters the battlefield, you may search your library and/or graveyard for a card named Huatli, Dinosaur Knight, reveal it, then put it into your hand. If you searched your library this way, shuffle it. */

// Stone Quarry: reprint from Shadows over Innistrad

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2212ae0; original cards/ixalan.c:41. */
int card_adanto_vanguard(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  if (event == EVENT_POWER)
  {
    instance = get_card_instance(player, card);
    if ((instance->state & 4) != 0)
    {
      event_result = event_result + 2;
    }
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_02212b61:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_02212b66;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_02212b61;
  }
  instance = get_card_instance(player, card);
  pump_ability_until_eot(instance->parent_controller, instance->parent_card,
                         instance->parent_controller, instance->parent_card, 0, 0, 0, 0x800);
LAB_02212b66:
  result = generic_activated_ability(player, card, event, 0, 0, 0, 0, 0, 0, 0, 4, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2213730; original cards/ixalan.c:428. */
int card_chart_a_course(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      ai_modifier = ai_modifier + ((current_turn != player) - 1 & 0xffffffd0) + 0x18;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    draw_cards(player, 2);
    result = get_trap_condition(player, 0x606);
    if (result < 1)
    {
      discard(player, 0, player);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2213800; original cards/ixalan.c:475. */
int card_entrancing_melody(int player, int card, event_t event)
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
            gain_control_permanently(player, instance->targets[0].player,
                                     instance->targets[0].card);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02213866;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  get_card_instance(player, card);
LAB_02213866:
  result = generic_spell(player, card, event, 5, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2213a30; original cards/ixalan.c:563. */
int card_prosperous_pirates(int player, int card, event_t event)
{
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  generate_tokens_by_id(player, card, 0x4998, 2);
  return 0;
}

/* ManalinkEh 2022: 0x2213aa0; original cards/ixalan.c:581. */
int card_rivers_rebuke(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
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
        td.preferred_controller = player;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          default_test_definition(&this_test, 4);
          new_manipulate_all(player, card, instance->targets[0].player, &this_test, ACT_BOUNCE);
          default_test_definition(&this_test, 1);
          this_test.type_flag = 1;
          new_manipulate_all(player, card, instance->targets[0].player, &this_test, ACT_BOUNCE);
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02213bbf;
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
LAB_02213bbf:
  result = generic_spell(player, card, event, 1, &td, "TARGET_PLAYER", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2213c70; original cards/ixalan.c:615. */
int card_sailor_of_means(int player, int card, event_t event)
{
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  generate_token_by_id(player, card, 0x4998);
  return 0;
}

/* ManalinkEh 2022: 0x2214060; original cards/ixalan.c:733. */
int card_siren_stormtamer(int player, int card, event_t event)
{
  int card_00;
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  if (event == EVENT_CAN_ACTIVATE)
  {
    iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x408, 0, 0, 1, 0, 0, 0, 0,
                                      NULL, NULL);
    if ((iVar1 != 0) && (instance = get_card_instance(card_on_stack_controller, card_on_stack),
                         instance->number_of_targets != '\0'))
    {
      iVar1 = 0;
      do
      {
        iVar3 = instance->targets[iVar1].player;
        card_00 = instance->targets[iVar1].card;
        if (iVar3 == player)
        {
          if (card_00 == -1)
          {
            return 99;
          }
        LAB_0221421c:
          iVar3 = is_what(iVar3, card_00, 2);
          if ((iVar3 != 0) && (instance->targets[iVar1].player == player))
          {
            return 99;
          }
        }
        else if (card_00 != -1)
          goto LAB_0221421c;
        iVar1 = iVar1 + 1;
      } while (iVar1 < (int)(unsigned int)instance->number_of_targets);
    }
    return 0;
  }
  if (event == EVENT_ACTIVATE)
  {
  LAB_022140a1:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_022140a6;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_022140a1;
  }
  instance = get_card_instance(player, card);
  iVar1 = counterspell_validate(player, card, NULL, 0);
  if (iVar1 != 0)
  {
    real_counter_a_spell(player, card, instance->targets[0].player, instance->targets[0].card);
  }
  kill_card(player, card, KILL_SACRIFICE);
LAB_022140a6:
  iVar1 = generic_activated_ability(player, card, event, 0x408, 0, 0, 1, 0, 0, 0, 0, NULL, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x22143b0; original cards/ixalan.c:831. */
int card_arguels_blood_fast(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  double_faced_card(player, card, event);
  check_legend_rule(player, card, event);
  iVar1 = duh_mode(player);
  upkeep_trigger_ability_mode(player, card, event, player, (iVar1 != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    if (life[player] < 6)
    {
      transform(player, card);
    }
  LAB_0221450a:
    if (event != EVENT_SHOULD_AI_PLAY)
    {
      return (unsigned int)(event == EVENT_CAN_CAST);
    }
    ai_modifier = ai_modifier + (-(unsigned int)(player == 0) & 0xfffffff0) + 8;
    return 0;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
  LAB_0221446d:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_02214472;
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      iVar1 = life[player];
      iVar2 = count_subtype(player, 1, -1);
      ai_modifier = ai_modifier + iVar1 + -0xe + iVar2 * 2;
      goto LAB_02214472;
    }
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event != EVENT_CLEANUP)
        goto LAB_0221450a;
      goto LAB_0221446d;
    }
  }
  draw_cards(player, 1);
LAB_02214472:
  iVar1 = generic_activated_ability(player, card, event, 0, 1, 1, 0, 0, 0, 0, 2, NULL, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x2214d00; original cards/ixalan.c:1188. */
int card_skymarch_bloodletter(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 2);
    td.zone = 0x1000;
    result = would_validate_arbitrary_target(&td, 1 - player, -1);
    if (result != 0)
    {
      lose_life(1 - player, 1);
      gain_life(player, 1);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2214db0; original cards/ixalan.c:1225. */
int card_vraskas_contempt(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 0x102);
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = validate_target(player, card, &td, 0);
          if (result != 0)
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
            gain_life(player, 2);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02214e16;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x102);
  get_card_instance(player, card);
LAB_02214e16:
  result = generic_spell(player, card, event, 0x81, &td, "Select target creature or planeswalker.",
                         1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2214f20; original cards/ixalan.c:1249. */
int card_walk_the_plank(int player, int card, event_t event)
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
        td.required_subtype = 0x74;
        td.special = 0x1000;
        if (event == EVENT_RESOLVE_SPELL)
        {
          instance = get_card_instance(player, card);
          result = validate_target(player, card, &td, 0);
          if (result != 0)
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02214f83;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.required_subtype = 0x74;
  td.special = 0x1000;
LAB_02214f83:
  result =
      generic_spell(player, card, event, 0x81, &td, "Select target non-Merfolk creature.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2215260; original cards/ixalan.c:1345. */
int card_fiery_cannonade(int player, int card, event_t event)
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
    else if (event == EVENT_RESOLVE_SPELL)
    {
      default_test_definition(&this_test, 2);
      this_test.subtype = 0x95;
      this_test.subtype_flag = 1;
      new_damage_all(player, card, 2, 2, 0, &this_test);
      kill_card(player, card, KILL_DESTROY);
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2215c10; original cards/ixalan.c:1627. */
int card_vances_blasting_cannons(int player, int card, event_t event)
{
  unsigned short uVar1;
  int iVar2;
  int iVar3;
  double_faced_card(player, card, event);
  check_legend_rule(player, card, event);
  upkeep_trigger_ability(player, card, event, player);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    uVar1 = cards_data[*deck_ptr[player]].id;
    rfg_top_card_of_deck(player);
    iVar2 = is_what(-1, (unsigned int)uVar1, 1);
    if (iVar2 == 0)
    {
      create_may_play_card_from_exile_effect(player, card, player, (unsigned int)uVar1,
                                             MPCFE_UNTIL_EOT);
    }
  }
  iVar2 = get_specific_storm_count(player);
  if (iVar2 == 2)
  {
    iVar3 = duh_mode(player);
    iVar2 = 2;
    if (iVar3 == 0)
    {
      iVar2 = 1;
    }
    iVar2 = new_specific_spell_played(player, card, event, player, iVar2, NULL);
    if (iVar2 != 0)
    {
      transform(player, card);
    }
  }
  if (event != EVENT_SHOULD_AI_PLAY)
  {
    return (unsigned int)(event == EVENT_CAN_CAST);
  }
  ai_modifier = ai_modifier + (-(unsigned int)(player == 0) & 0xfffffff0) + 8;
  return 0;
}

/* ManalinkEh 2022: 0x2215fe0; original cards/ixalan.c:1721. */
int card_carnage_tyrant(int player, int card, event_t event)
{
  cannot_be_countered(player, card, event);
  hexproof(player, card, event);
  return 0;
}

/* ManalinkEh 2022: 0x2216860; original cards/ixalan.c:1914. */
int card_growing_rites_of_itlimoc(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  test_definition_t test;
  double_faced_card(player, card, event);
  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if ((event == EVENT_ETB) && (iVar1 = count_deck(player), 0 < iVar1))
  {
    if (4 < iVar1)
    {
      iVar1 = 4;
    }
    new_default_test_definition(&test, 2, "Select a creature card.");
    test.create_minideck = (unsigned short)iVar1;
    test.no_shuffle = 1;
    iVar2 = new_global_tutor(player, player, 1, 1, 0, 1, &test);
    if ((iVar2 < 0) || (iVar1 = iVar1 + -1, 0 < iVar1))
    {
      put_top_x_on_bottom(player, player, iVar1);
    }
  }
  if (((current_turn == player) && (iVar1 = eot_trigger(player, card, event), iVar1 != 0)) &&
      (iVar1 = count_permanents_by_type(player, TYPE_CREATURE), 3 < iVar1))
  {
    transform(player, card);
  }
  iVar1 = global_enchantment(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x2216d20; original cards/ixalan.c:2095. */
int card_thundering_spineback(int player, int card, event_t event)
{
  int result;
  boost_creature_type(player, card, event, SUBTYPE_DINOSAUR, 1, 1, 0, BCT_CONTROLLER_ONLY);
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      generate_token_by_id(player, card, 0x491d);
    }
  }
  result = generic_activated_ability(player, card, event, 0, 5, 0, 0, 1, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22174d0; original cards/ixalan.c:2275. */
int card_hostage_taker(int player, int card, event_t event)
{
  card_instance_t *instance;
  int csvid;
  int result;
  target_definition_t td;
  return_from_oblivion(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 0x42);
    td.special = 0x4000;
    td.allow_cancel = 0;
    result = can_target(&td);
    if ((result != 0) && (result = new_pick_target(&td, "PUTREFY", 0, 0), result != 0))
    {
      instance = get_card_instance(player, card);
      result = instance->targets[0].card;
      instance = get_card_instance(player, card);
      csvid = get_id(instance->targets[0].player, result);
      instance = get_card_instance(player, card);
      result = instance->targets[0].card;
      instance = get_card_instance(player, card);
      obliviation(player, card, instance->targets[0].player, result);
      instance = get_card_instance(player, card);
      create_may_play_card_from_exile_effect(player, card, instance->targets[0].player, csvid,
                                             MPCFE_FOR_CMC);
    }
    return 0;
  }
  if (event == EVENT_CAN_CAST)
  {
    result = basic_spell(player, card, EVENT_CAN_CAST);
    return result;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2217b50; original cards/ixalan.c:2411. */
int card_raging_swordtooth(int player, int card, event_t event)
{
  test_definition_t this_test;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  default_test_definition(&this_test, 2);
  this_test.not_me = 1;
  new_damage_all(player, card, 2, 1, 0, &this_test);
  return 0;
}

/* ManalinkEh 2022: 0x2217bf0; original cards/ixalan.c:2428. */
int card_regisaur_alpha(int player, int card, event_t event)
{
  token_generation_t token;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_token_definition(player, card, 0x491d, &token);
    token.tou = 3;
    token.pow = 3;
    token.color_forced = '\b';
    generate_token(&token);
    return 0;
  }
  if (event != EVENT_ABILITIES)
  {
    return 0;
  }
  boost_subtype(player, card, EVENT_ABILITIES, SUBTYPE_DINOSAUR, 0, 0, 0, 4, BCT_CONTROLLER_ONLY);
  return 0;
}

/* ManalinkEh 2022: 0x2217dd0; original cards/ixalan.c:2480. */
int card_vona_butcher_of_magan(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  check_legend_rule(player, card, event);
  vigilance(player, card, event);
  lifelink(player, card, event);
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 0x47);
    td.illegal_type = 1;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x47);
    td.illegal_type = 1;
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      instance = get_card_instance(player, card);
      result = instance->targets[0].card;
      instance = get_card_instance(player, card);
      kill_card(instance->targets[0].player, result, KILL_DESTROY);
    }
  }
  result = generic_activated_ability(player, card, event, 0x10005, 0, 0, 0, 0, 0, 0, 7, &td,
                                     "TARGET_PERMANENT");
  return result;
}

/* ManalinkEh 2022: 0x2218d00; original cards/ixalan.c:2783. */
int card_dusk_legion_dreadnought(int player, int card, event_t event)
{
  int result;
  vigilance(player, card, event);
  result = vehicle(player, card, event, 2);
  return result;
}

/* ManalinkEh 2022: 0x2218d50; original cards/ixalan.c:2818. */
int card_pirates_cutlass(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 2);
    td.required_subtype = 0x95;
    td.preferred_controller = player;
    td.allowed_controller = player;
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "Select target Pirate you control.", 0, 0x80);
      if (result != 0)
      {
        equip_target_creature(player, card, instance->targets[0].player, instance->targets[0].card);
      }
    }
    instance->number_of_targets = '\0';
  }
  result = vanilla_equipment(player, card, event, 2, 2, 1, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x22126f0; original cards/ixalan.c:2843. */
static int primal_wellspring_legacy(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  if (event == EVENT_CAST_SPELL)
  {
    result = played_for_free(affected_card_controller, affected_card);
    if (result == 0)
    {
      result = is_token(affected_card_controller, affected_card);
      if (result == 0)
      {
        result = check_special_flags(affected_card_controller, affected_card, 0x40000);
        if ((result == 0) && (affected_card_controller == player))
        {
          result = is_what(player, affected_card, 0x38);
          if ((result != 0) && (0 < mana_paid[instance->targets[6].player]))
          {
            result = get_id(affected_card_controller, affected_card);
            copy_spell(player, result);
            kill_card(player, card, KILL_EXILE);
          }
        }
      }
    }
  }
  else if (((event == EVENT_STATIC_EFFECTS) && (stack_size == 0)) &&
           (mana_pool[player][instance->targets[6].player] < 1))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2219160; original cards/ixalan.c:2949. */
int card_thaumatic_compass(int player, int card, event_t event)
{
  int result;
  double_faced_card(player, card, event);
  if (((current_turn == player) && (result = eot_trigger(player, card, event), result != 0)) &&
      (result = count_permanents_by_type(player, TYPE_LAND), 6 < result))
  {
    if (event != EVENT_TRIGGER)
    {
      if (event == EVENT_RESOLVE_TRIGGER)
      {
        transform(player, card);
        return 0;
      }
      goto LAB_0221918e;
    }
    event_result = event_result | 2;
  LAB_02219198:
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_022191a8;
    }
  }
  else
  {
  LAB_0221918e:
    if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
      goto LAB_02219198;
  LAB_022191a8:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_022191b1;
  }
  tutor_basic_land(player, 0, 0);
LAB_022191b1:
  result = generic_activated_ability(player, card, event, 1, 3, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2214270; original cards/ixalan.c:774. */
int card_spell_swindle(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  token_generation_t token;

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_022142ae;
    }
    else if (event == EVENT_RESOLVE_SPELL)
    {
      result = counterspell_validate(player, card, NULL, 0);
      if (result != 0)
      {
        instance = get_card_instance(player, card);
        result = get_cmc(instance->targets[0].player, instance->targets[0].card);
        real_counter_a_spell(player, card, instance->targets[0].player, instance->targets[0].card);
        default_token_definition(player, card, 0x4998, &token);
        token.qty = (unsigned char)result;
        generate_token(&token);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
    return 0;
  }
LAB_022142ae:
  result = counterspell(player, card, event, NULL, 0);
  return result;
}

/* ManalinkEh 2022: 0x22150a0; original cards/ixalan.c:1276. */
int card_angraths_marauders(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = damage_being_dealt(event);
  if ((instance != NULL) && (instance->damage_source_player == player))
  {
    instance->info_slot = instance->info_slot * 2;
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2215350; original cards/ixalan.c:1390. */
int card_makeshift_munitions(int player, int card, event_t event)
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
  result = generic_activated_ability_with_sacrifice_as_cost(
      player, card, event, GAA_CAN_TARGET, 1, 0, 0, 0, 0, 0, 0, &td, "TARGET_ANY",
      TYPE_ARTIFACT | TYPE_CREATURE, NULL, SARV_0);
  return result;
}

/* ManalinkEh 2022: 0x2215610; original cards/ixalan.c:1476. */
int card_star_of_extinction(int player, int card, event_t event)
{
  int iVar1;
  int player_00;
  int iVar2;
  card_instance_t *instance;
  int card_00;
  int iStack_6c;
  target_definition_t *ptStack_68;
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
        if (event == EVENT_RESOLVE_SPELL)
        {
          iVar1 = valid_target(&td);
          if (iVar1 != 0)
          {
            instance = get_card_instance(player, card);
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
          }
          iStack_6c = 0;
          iVar1 = current_turn;
          do
          {
            iVar2 = 1;
            card_00 = active_cards_count[iVar1] + -1;
            player_00 = iVar1;
            if (-1 < card_00)
              goto LAB_02215733;
            while (iStack_6c = iVar2, iVar2 != 0)
            {
              player_00 = 1 - iVar1;
              card_00 = active_cards_count[player_00];
              iVar1 = current_turn;
              while (card_00 = card_00 + -1, current_turn = iVar1, -1 < card_00)
              {
              LAB_02215733:
                iVar1 = is_what(player_00, card_00, 2);
                if (iVar1 != 0)
                {
                  damage_creature(player_00, card_00, 0x14, player, card);
                }
                iVar2 = is_what(player_00, card_00, 0x100);
                iVar1 = current_turn;
                if (iVar2 != 0)
                {
                  damage_creature(player_00, card_00, 0x14, player, card);
                  iVar1 = current_turn;
                }
              }
              iVar2 = iStack_6c + 1;
              if (1 < iVar2)
              {
                kill_card(player, card, KILL_DESTROY);
                goto LAB_0221566a;
              }
            }
          } while (true);
        }
        goto LAB_0221566a;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 1);
LAB_0221566a:
  ptStack_68 = &td;
  iVar1 = generic_spell(player, card, event, 1, ptStack_68, "TARGET_LAND", 1, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x2219080; original cards/ixalan.c:2928. */
int card_sorcerous_spyglass(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t test;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    reveal_target_player_hand(1 - player);
    new_default_test_definition(&test, 0xff, "Select a card.");
    result = new_card_from_list(player, 3, &test);
    instance = get_card_instance(player, card);
    instance->info_slot = result;
    activated_abilities_disabler_enters_bf(player, card, 0x2a);
  }
  result = activated_abilities_disabler(player, card, event, 0x2a);
  return result;
}

/* ManalinkEh 2022: 0x2219a40; original cards/ixalan.c:3108. */
int card_vanquishers_banner(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (player == 1)
    {
      state_untargettable(1, card, 1);
      instance = get_card_instance(1, card);
      result = select_a_subtype(1, card);
      instance->info_slot = result;
      state_untargettable(1, card, 0);
    }
    else
    {
      instance = get_card_instance(player, card);
      result = select_a_subtype(player, card);
      instance->info_slot = result;
    }
  }
  else if (event - EVENT_POWER < 2)
  {
    instance = get_card_instance(player, card);
    boost_creature_type(player, card, event, instance->info_slot, 1, 1, 0, BCT_CONTROLLER_ONLY);
  }
  if ((((trigger_condition == 0xd3) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      ((reason_for_trigger_controller == player && (trigger_cause_controller == player))))
  {
    instance = get_card_instance(player, card);
    result = has_subtype(trigger_cause_controller, trigger_cause, instance->info_slot);
    if ((result != 0) &&
        (result = is_what(trigger_cause_controller, trigger_cause, 2), result != 0))
    {
      if (event == EVENT_TRIGGER)
      {
        event_result = event_result | 2;
      }
      else if (event == EVENT_RESOLVE_TRIGGER)
      {
        draw_a_card(player);
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2218fe0; original cards/ixalan.c:2891. */
int card_primal_wellspring(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  double_faced_card(player, card, event);
  if (event != EVENT_ACTIVATE)
  {
    result = mana_producer(player, card, event);
    return result;
  }

  mana_producer(player, card, EVENT_ACTIVATE);
  result = create_legacy_effect(player, card, primal_wellspring_legacy);
  instance = get_card_instance(player, result);
  instance->targets[6].player = tapped_for_mana_color;
  add_status(player, result, 0x10000);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2212cc0; original cards/ixalan.c:136. */
int card_ixalans_binding(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iStack_6c;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    iVar1 = basic_spell(player, card, EVENT_CAN_CAST);
    return iVar1;
  }
  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    iVar1 = is_humiliated_for_ability(player, card, AL_STATIC);

    if ((iVar1 == 0) && (instance = get_card_instance(player, card), 1 < instance->info_slot))
    {
      iVar1 = 1;
      do
      {
        iVar3 = get_internal_card_id_from_csv_id(instance->targets[iVar1].card);
        iVar3 = has_same_name(affected_card_controller, affected_card, -1, iVar3);
        if ((iVar3 != 0) && (affected_card_controller != player))
        {
          infinite_casting_cost();
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < instance->info_slot);
    }
    etb_ability(player, card, EVENT_MODIFY_COST_GLOBAL, RESOLVE_TRIGGER_MANDATORY);
    return_from_oblivion(player, card, EVENT_MODIFY_COST_GLOBAL);
  }
  else
  {
    etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    return_from_oblivion(player, card, event);
    if (event == EVENT_ETB)
    {

      default_target_definition(player, card, &td, 0x47);
      td.allowed_controller = 1 - player;
      td.illegal_type = 1;
      td.allow_cancel = 0;
      td.preferred_controller = td.allowed_controller;
      instance = get_card_instance(player, card);
      iVar1 = can_target(&td);
      if (iVar1 == 0)
      {
        if (player == 1)
        {
          ai_modifier = ai_modifier + -0x30;
        }
      }
      else
      {
        iVar1 =
            new_pick_target(&td, "Select target nonland permanent an opponent controls.", 0, 0x80);
        if (iVar1 != 0)
        {

          iStack_6c = instance->info_slot;
          if (iStack_6c < 1)
          {
            iStack_6c = 1;
          }
          iVar1 = get_card_name(instance->targets[0].player, instance->targets[0].card);
          instance->targets[iStack_6c].card = iVar1;
          obliviation(player, card, instance->targets[0].player, instance->targets[0].card);
          instance->info_slot_fn = (CardFunction)(iStack_6c + 1);
        }
      }
    }
  }
  iVar1 = global_enchantment(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x2212f30; original cards/ixalan.c:201. */
int card_legions_landing(int player, int card, event_t event)
{
  int result;
  token_generation_t token;

  check_legend_rule(player, card, event);
  double_faced_card(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_token_definition(player, card, 0x49a0, &token);
    token.color_forced = ' ';
    token.tou = 1;
    token.pow = 1;
    token.s_key_plus = 2;
    generate_token(&token);
    store_attackers(player, card, EVENT_ETB, DAT_ATTACKS_WITH_3_OR_MORE, player, -1, NULL);
    dat_ability(player, card, EVENT_ETB, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  }
  else
  {
    store_attackers(player, card, event, DAT_ATTACKS_WITH_3_OR_MORE, player, -1, NULL);
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
    if (event == EVENT_DAT_ABILITY)
    {
      transform(player, card);
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2213940; original cards/ixalan.c:506. */
int card_herald_of_secret_streams(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (((event == EVENT_ABILITIES) && (affected_card_controller == player)) &&
      (card != affected_card))
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        instance = in_play(affected_card_controller, affected_card);
        if (instance != NULL)
        {
          result = is_what(affected_card_controller, affected_card, 2);
          if (result != 0)
          {
            result = count_counters(affected_card_controller, affected_card, COUNTER_P1_P1);
            if (0 < result)
            {
              unblockable(affected_card_controller, affected_card, EVENT_ABILITIES);
            }
          }
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22150d0; original cards/ixalan.c:1302. */
int card_captain_lannery_storm(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  check_legend_rule(player, card, event);
  haste(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    generate_token_by_id(player, card, 0x4998);
  }
  else if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    instance = in_play(affected_card_controller, affected_card);
    if (instance != NULL)
    {
      instance = get_card_instance(affected_card_controller, affected_card);
      if (instance->kill_code == '\x03')
      {
        result = get_id(affected_card_controller, affected_card);
        if (result == 0x4998)
        {
          result = check_special_flags(affected_card_controller, affected_card, 0x800);
          if (result == 0)
          {
            count_for_gfp_ability(player, card, 0x77, player, 0x47, NULL);
          }
        }
      }
    }
  }
  result = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  pump_until_eot_merge_previous(player, card, player, card, result, 0);
  return 0;
}

/* ManalinkEh 2022: 0x2216130; original cards/ixalan.c:1771. */
int card_crushing_canopy(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_t *td;
  const char *prompt;
  target_definition_t td1;
  target_definition_t td2;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td1, 2);
    td1.required_abilities = 0x20;
    default_target_definition(player, card, &td2, 4);
    get_card_instance(player, card);
    iVar3 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td1, "TARGET_FLYING", 1, NULL);
    if (iVar3 == 0)
    {
      iVar3 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td2, "TARGET_ENCHANTMENT", 1, NULL);
      return iVar3;
    }
    return 1;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      td = &td2;
      default_target_definition(player, card, &td1, 2);
      td1.required_abilities = 0x20;
      default_target_definition(player, card, td, 4);
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        iVar3 = check_special_flags2(player, card, -0x80000000);
        if (iVar3 == 0)
        {

          iVar3 = can_target(td);
          iVar2 = can_target(&td1);
          iVar3 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\016",
                              "Destroy a flying creature", iVar2, 5, "Destroy an enchantment",
                              iVar3, 5, 0, 0, 0, 0);
          if (iVar3 == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
          if (iVar3 == 1)
          {
            td = &td1;
            prompt = "TARGET_FLYING";
          }
          else
          {
            if (iVar3 != 2)
            {
              return 0;
            }
            prompt = "TARGET_ENCHANTMENT";
          }
          iVar3 = generic_spell(player, card, EVENT_CAST_SPELL, 1, td, prompt, 1, NULL);
          return iVar3;
        }
      }
    }
    return 0;
  }
  if ((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET))
  {
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
    default_target_definition(player, card, &td1, 2);
    td1.required_abilities = 0x20;
    default_target_definition(player, card, &td2, 4);
    get_card_instance(player, card);
    return 0;
  }
  default_target_definition(player, card, &td1, 2);
  td1.required_abilities = 0x20;
  default_target_definition(player, card, &td2, 4);
  instance = get_card_instance(player, card);
  if (event != EVENT_RESOLVE_SPELL)
  {
    return 0;
  }

  if (instance->info_slot == 1)
  {
    iVar3 = valid_target(&td1);
  }
  else
  {
    if (instance->info_slot != 2)
      goto LAB_022161e2;
    iVar3 = valid_target(&td2);
  }
  if (iVar3 != 0)
  {
    kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
  }
LAB_022161e2:
  kill_card(player, card, KILL_DESTROY);
  return 0;
}

/* ManalinkEh 2022: 0x2216540; original cards/ixalan.c:1830. */
int card_deathgorge_scavenger(int player, int card, event_t event)
{
  unsigned char bVar1;
  int player_00;
  int result;
  card_instance_t *instance;
  test_definition_t test;

  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if ((event == EVENT_ETB) || (event == EVENT_DAT_ABILITY))
  {
    new_default_test_definition(&test, 0xff, "Select target card to exile.");
    result = select_target_from_either_grave(player, card, SFG_0, 3, 1, &test, 0, 1);
    if (result != -1)
    {

      instance = get_card_instance(player, card);
      result = validate_target_from_grave_source(player, card, instance->targets[0].player, 1);
      if (result != -1)
      {

        player_00 = instance->targets[0].player;
        bVar1 = cards_data[graveyard_ptr_mutable[player_00][result]].type;
        rfg_card_from_grave(player_00, result);
        if ((bVar1 & 2) == 0)
        {
          pump_until_eot_merge_previous(player, card, player, card, 1, 1);
        }
        else
        {
          gain_life(player, 2);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22167a0; original cards/ixalan.c:1884. */
int card_drover_of_the_mighty(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t test;

  if (((event - EVENT_POWER < 2) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      new_default_test_definition(&test, 0, "");
      test.subtype = 0xf0;
      result = check_battlefield_for_special_card(player, card, player, 0, &test);
      if (result != 0)
      {
        event_result = event_result + 2;
      }
    }
  }
  result = card_generic_combat_1_mana_producing_creature(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2216cb0; original cards/ixalan.c:2060. */
int card_ripjaw_raptor(int player, int card, event_t event)
{
  int result;

  result = is_humiliated_for_ability(player, card, AL_TRIG);
  if (result == 0)
  {
    result = damage_dealt_to_me_arbitrary(player, card, event, DDBM_0, player, card);
    if (result != 0)
    {
      draw_a_card(player);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2216e00; original cards/ixalan.c:2123. */
int card_verdant_suns_avatar(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t test;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  result = is_humiliated_for_ability(player, card, AL_TRIG);
  if ((((result == 0) && (trigger_condition == 0xdb)) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 2);
    test.not_me = 1;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event != EVENT_ETB)
  {
    return 0;
  }
  instance = get_card_instance(trigger_cause_controller, trigger_cause);
  instance->regen_status = instance->regen_status | 0x6000000;
  result = get_toughness(trigger_cause_controller, trigger_cause);
  gain_life(player, result);
  return 0;
}

/* ManalinkEh 2022: 0x2216f90; original cards/ixalan.c:2164. */
int card_admiral_beckett_brass(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int result;
  card_instance_t *pcVar3;
  target_definition_t td;

  if (event - EVENT_POWER < 2)
  {
    boost_creature_type(player, card, event, SUBTYPE_PIRATE, 1, 1, 0, BCT_CONTROLLER_ONLY);
    pcVar1 = get_card_instance(player, card);
    get_trap_condition(player, 0x40a);
  joined_r0x0221705a:
    if (event != EVENT_PHASE_CHANGED)
    {
      return 0;
    }
  }
  else
  {
    pcVar1 = get_card_instance(player, card);
    result = get_trap_condition(player, 0x40a);
    if ((result < 3) || (event != EVENT_PHASE_CHANGED))
      goto joined_r0x0221705a;
    if (current_phase != 0x22)
      goto LAB_02217071;
    if (current_turn != player)
    {
      return 0;
    }
    if (pcVar1->info_slot != 0)
    {
      return 0;
    }
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if ((result == 0) && (pcVar3 = in_play(player, card), pcVar3 != NULL))
    {
      default_target_definition(player, card, &td, 0x47);
      td.allowed_controller = 1 - current_turn;
      td.illegal_type = 1;
      td.allow_cancel = 0;
      result = can_target(&td);
      if ((result != 0) &&
          (result = new_pick_target(&td, "TARGET_NONLAND_PERMANENT", 0, 0), result != 0))
      {
        gain_control_permanently(player, pcVar1->targets[0].player, pcVar1->targets[0].card);
        pcVar1->info_slot = 1;
      }
    }
  }
  if (current_phase == 0x22)
  {
    return 0;
  }
LAB_02217071:
  pcVar1->info_slot = 0;
  return 0;
}

/* ManalinkEh 2022: 0x2217cd0; original cards/ixalan.c:2459. */
int card_tishana_voice_of_thunder(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    if (event - EVENT_POWER < 2)
    {
      if (((card == affected_card) && (affected_card_controller == player)) && (player != -1))
      {
        set_characteristic_pt(hand_count[player]);
      }
    }
    else if ((current_turn == player) && (event == EVENT_MAX_HAND_SIZE))
    {
      instance = in_play(player, card);
      if (instance != NULL)
      {
        result = is_humiliated_for_ability(player, card, AL_STATIC);
        if (result == 0)
        {
          event_result = event_result + 99;
        }
      }
    }
    return 0;
  }
  result = count_permanents_by_type(player, TYPE_CREATURE);
  draw_cards(player, result);
  return 0;
}

/* ManalinkEh 2022: 0x2217f60; original cards/ixalan.c:2504. */
int card_vraska_relic_seeker(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_t *ptStack_b4;
  char buffer[50];
  target_definition_t td;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0x46);
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      iVar2 = opponent_is_valid_target(player, card);
      iVar3 = can_target(&td);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Pirate Token", 1,
                          5, 2, "Destroy", iVar3, 10, 0xfffffffd, "Set life to 1", iVar2, 0x14,
                          0xfffffff6, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 == 2)
      {
        scnprintf(buffer, 0x32, "Select target artifact, creature, or enchantment");
        new_pick_target(&td, buffer, 0, 0x81);
      }
      else if (iVar2 == 3)
      {
        td.zone = 0x1000;
        new_pick_target(&td, "TARGET_PLAYER", 0, 1);
      }
      goto LAB_02217fd5;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_02217fd5;
    default_target_definition(player, card, &td, 0x46);
    instance = get_card_instance(player, card);
  }
  ptStack_b4 = &td;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    iVar2 = instance->info_slot;
    if (iVar2 == 1)
    {
      default_token_definition(player, card, 0x4972, (token_generation_t *)buffer);
      buffer[0x24] = '\0';
      buffer[0x25] = '\0';
      buffer[0x26] = '\x01';
      buffer[0x27] = '\0';
      generate_token((token_generation_t *)buffer);
    }
    else if (iVar2 == 2)
    {
      iVar2 = validate_target(player, card, ptStack_b4, 0);
      if (iVar2 != 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
        default_token_definition(player, card, 0x4998, (token_generation_t *)buffer);
        generate_token((token_generation_t *)buffer);
      }
    }
    else if (iVar2 == 3)
    {
      td.zone = 0x1000;
      iVar2 = validate_target(player, card, ptStack_b4, 0);
      if (iVar2 != 0)
      {
        set_life_total(instance->targets[0].player, 1);
      }
    }
  }
LAB_02217fd5:
  iVar2 = planeswalker(player, card, event, 6);
  return iVar2;
}

/* ManalinkEh 2022: 0x22183f0; original cards/ixalan.c:2614. */
int card_conquerors_foothold(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  int iVar3;
  card_instance_t *instance;
  int32_t iVar5;
  test_definition_t test;

  if (event != EVENT_CAN_ACTIVATE)
  {
    if (event != EVENT_ACTIVATE)
    {
      if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
      {
        new_default_test_definition(&test, 0xff, "Select target card.");
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_ACTIVATION)
        {
          iVar5 = instance->info_slot;
          if (iVar5 == 2)
          {
            draw_a_card(player);
            discard(player, 0, player);
            iVar5 = instance->info_slot;
          }
          if (iVar5 == 3)
          {
            draw_a_card(player);
            iVar5 = instance->info_slot;
          }

          if ((iVar5 == 4) &&
              (iVar1 = validate_target_from_grave_source(player, card, player, 0), iVar1 != -1))
          {
            from_grave_to_hand(player, iVar1, 1);
          }
        }
        return 0;
      }
      goto LAB_02218422;
    }
    new_default_test_definition(&test, 0xff, "Select target card.");
    instance = get_card_instance(player, card);
    instance->info_slot = 0;
    iVar1 = paying_mana();
    if (iVar1 == 0)
    {

      iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 3, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 5, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      iVar3 = generic_activated_ability_with_target_in_grave(player, card, EVENT_CAN_ACTIVATE,
                                                             GAA_UNTAPPED, 7, 0, 0, 0, 0, 0, 0,
                                                             NULL, NULL, TYPE_NONE, &test, 8);
      iVar1 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "Produce mana", 1,
                          5, "Draw & discard", iVar1, 1, "Draw", iVar2, 3, "Regrowth", iVar3, 5, 0,
                          0, 0, 0);
      if (iVar1 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      instance->info_slot = iVar1;
      if (iVar1 != 1)
      {
        add_state(player, card, 0x10);
        if (iVar1 == 2)
        {
          iVar1 = 2;
        }
        else
        {
          if (iVar1 != 3)
          {
            if (iVar1 != 4)
            {
              tapped_for_mana_color = -1;
              return 0;
            }
            generic_activated_ability_with_target_in_grave(player, card, EVENT_ACTIVATE, GAA_NONE,
                                                           6, 0, 0, 0, 0, 0, 0, NULL, NULL,
                                                           TYPE_NONE, &test, 8);
            goto joined_r0x02218932;
          }
          iVar1 = 4;
        }
        generic_activated_ability(player, card, EVENT_ACTIVATE, 0, iVar1, 0, 0, 0, 0, 0, 0, NULL,
                                  NULL);
      joined_r0x02218932:
        if (spell_fizzled == 1)
        {
          remove_state(player, card, 0x10);
        }
        tapped_for_mana_color = -1;
        return 0;
      }
    }
    else
    {
      instance->info_slot = 1;
    }
    event = EVENT_ACTIVATE;
  }
LAB_02218422:

  iVar1 = mana_producer(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x22192b0; original cards/ixalan.c:2973. */
int card_spires_of_orazca(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  bool bVar3;
  target_definition_t *ptStack_68;
  target_definition_t td;

  double_faced_card(player, card, event);
  if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      default_target_definition(player, card, &td, 2);
      td.allowed_controller = 1 - player;
      td.required_state = 2;
      instance = get_card_instance(player, card);

      instance->number_of_targets = '\0';
      result = paying_mana();
      if ((result == 0) && (result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5,
                                                               0, 0, 0, 0, 0, 0, 0, &td, NULL),
                            result != 0))
      {
        bVar3 = ai_is_speculating != 1;
        instance->info_slot = 2;
        if ((bVar3) && ((player != 1 || (((unsigned char)trace_mode & 2) != 0))))
        {
          result = can_target(&td);
          result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Get mana", 1, 1,
                               "Remove a creature from combat", result, 1, 0, 0, 0, 0);
          if (result == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
          if (result == 1)
            goto LAB_02219375;
          if (result != 2)
          {
            return 0;
          }
        }
        add_state(player, card, 0x10);
        generic_activated_ability(player, card, EVENT_ACTIVATE, 5, 0, 0, 0, 0, 0, 0, 0, &td,
                                  "TARGET_ATTACKING_CREATURE");
        if (spell_fizzled == 1)
        {
          remove_state(player, card, 0x10);
        }
        return 0;
      }
      instance->info_slot = 1;
    LAB_02219375:
      result = mana_producer(player, card, EVENT_ACTIVATE);
      return result;
    }
    if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      default_target_definition(player, card, &td, 2);
      td.allowed_controller = 1 - player;
      td.required_state = 2;
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
      result = validate_target(player, card, ptStack_68, 0);
      if (result == 0)
      {
        return 0;
      }
      untap_card(instance->targets[0].player, instance->targets[0].card);
      remove_state(instance->targets[0].player, instance->targets[0].card, 4);
      return 0;
    }
  }
  result = mana_producer(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2213380; original cards/ixalan.c:333. */
int card_settle_the_wreckage(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  test_definition_t this_test1;
  test_definition_t this_test2;

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
        if (event == EVENT_RESOLVE_SPELL)
        {

          default_test_definition(&this_test1, 2);
          this_test1.state = 4;
          instance = get_card_instance(player, card);
          result = new_manipulate_all(player, card, instance->targets[0].player, &this_test1,
                                      ACT_KILL_REMOVE);
          if (0 < result)
          {
            default_test_definition(&this_test2, 1);
            this_test2.qty = (unsigned short)result;
            this_test2.subtype = 0x2000;
            instance = get_card_instance(player, card);
            result = instance->targets[0].player;
            instance = get_card_instance(player, card);
            new_global_tutor(instance->targets[0].player, result, 1, 5, 0, 5, &this_test2);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_022133e5;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.zone = 0x1000;
LAB_022133e5:
  result = generic_spell(player, card, event, 1, &td, "TARGET_PLAYER", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2216f20; original cards/ixalan.c:2153. */
int card_wildgrowth_walker(int player, int card, event_t event)
{
  if ((event == EVENT_EXPLORE) && (affected_card_controller == player))
  {
    add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
    gain_life(player, 3);
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int revel_in_riches_effect(int player, int card, event_t event);
int explore(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2212970; original cards/ixalan.c:17. */
int explore(int player, int card, event_t event)
{
  int *piVar1;
  int revealed_iid;
  int result;

  piVar1 = deck_ptr[player];
  revealed_iid = *piVar1;
  if (revealed_iid != -1)
  {
    reveal_card_iid(player, card, revealed_iid);
    result = is_what(-1, *piVar1, 1);
    if (result == 0)
    {
      add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
      result = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\003", "\001\014",
                           "\001\012", *piVar1, "Put card on top of library", 1, 5,
                           "Put card in graveyard", 1, 5, 0, 0, 0, 0);
      if (result == 2)
      {
        mill(player, 1);
      }
    }
    else
    {
      add_card_to_hand(player, *piVar1);
      remove_card_from_deck(player, 0);
    }
  }
  dispatch_event(player, card, 0x5700);
  return revealed_iid;
}

/* ManalinkEh 2022: 0x2212c00; original cards/ixalan.c:65. */
int card_axis_of_mortality(int player, int card, event_t event)
{
  int result;

  result = duh_mode(player);
  upkeep_trigger_ability_mode(player, card, event, player,
                              (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    result = opponent_is_valid_target(player, card);
    if (result != 0)
    {
      result = do_dialog(player, player, card, -1, -1,
                         " Switch life totals\n Keep life totals as they are", 0);
      if (result == 0)
      {
        exchange_life_totals(player, 1 - player);
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22130b0; original cards/ixalan.c:226. */
int card_adanto_the_first_fort(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  token_generation_t token;

  check_legend_rule(player, card, event);
  double_faced_card(player, card, event);
  if (event == EVENT_COUNT_MANA)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      iVar2 = mana_producer(player, card, EVENT_COUNT_MANA);
      return iVar2;
    }
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      instance = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {
        iVar2 = mana_producer(player, card, EVENT_CAN_ACTIVATE);
        return iVar2;
      }
      if (event == EVENT_ACTIVATE)
      {

        iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 2, 0, 0, 0, 0, 2, 0,
                                          NULL, NULL);
        iVar3 = paying_mana();
        if ((iVar3 == 0) && (iVar2 != 0))
        {
          iVar2 = do_dialog(player, player, card, -1, -1,
                            " Produce mana\n Generate a Vampire\n Cancel", 1);
          instance->info_slot = iVar2;
          if (iVar2 != 0)
          {
            if (iVar2 == 1)
            {
              add_state(player, card, 0x10);
              tapped_for_mana_color = -1;
              iVar2 = charge_mana_for_activated_ability(player, card, 2, 0, 0, 0, 0, 1);
              if (iVar2 == 0)
              {
                untap_card_no_event(player, card);
              }
            }
            else
            {
              spell_fizzled = 1;
            }
            tapped_for_mana_color = -1;
            return 0;
          }
        }
        else
        {
          instance->info_slot = 0;
        }
        iVar2 = mana_producer(player, card, EVENT_ACTIVATE);
        return iVar2;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      {
        return 0;
      }
      instance = get_card_instance(player, card);
    }
    if ((event == EVENT_RESOLVE_ACTIVATION) && (instance->info_slot == 1))
    {
      default_token_definition(player, card, 0x49a0, &token);
      token.color_forced = ' ';
      token.tou = 1;
      token.pow = 1;
      token.s_key_plus = 2;
      generate_token(&token);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2213db0; original cards/ixalan.c:650. */
int card_azcanta_the_sunken_ruin(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  test_definition_t test;

  double_faced_card(player, card, event);
  check_legend_rule(player, card, event);
  if (event == EVENT_CAN_ACTIVATE)
  {
  LAB_02213df9:
    iVar1 = mana_producer(player, card, event);
    return iVar1;
  }
  if (event == EVENT_ACTIVATE)
  {

    instance = get_card_instance(player, card);
    instance->info_slot = 0;
    iVar1 = paying_mana();
    if (((iVar1 != 0) ||
         (iVar1 = has_mana_for_activated_ability(player, card, 2, 0, 2, 0, 0, 0), iVar1 == 0)) ||
        (iVar1 =
             do_dialog(player, player, card, -1, -1, " Add mana\n Look at top four\n Cancel", 1),
         iVar1 == 0))
    {
      iVar1 = mana_producer(player, card, EVENT_ACTIVATE);
      return iVar1;
    }
    if (iVar1 == 1)
    {
      instance = get_card_instance(player, card);
      instance->state = instance->state | 0x10;
      iVar1 = charge_mana_for_activated_ability(player, card, 2, 0, 1, 0, 0, 0);
      if (iVar1 == 0)
      {
        spell_fizzled = 1;
        untap_card_no_event(player, card);
      }
      else
      {
        instance = get_card_instance(player, card);
        instance->info_slot = 1;
      }
      tapped_for_mana_color = -1;
    }
    else
    {
      spell_fizzled = 1;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);

    if ((instance->info_slot == 1) && (iVar1 = count_deck(player), 0 < iVar1))
    {
      if (4 < iVar1)
      {
        iVar1 = 4;
      }

      new_default_test_definition(&test, 3, "Select a nonland, noncreature card.");
      test.type_flag = 1;
      test.create_minideck = (unsigned short)iVar1;
      test.no_shuffle = 1;
      iVar3 = new_global_tutor(player, player, 1, 1, 0, 1, &test);
      if ((iVar3 < 0) || (iVar1 = iVar1 + -1, 0 < iVar1))
      {
        put_top_x_on_bottom(player, player, iVar1);
      }
    }
  }
  else if (event != EVENT_CLEANUP)
    goto LAB_02213df9;
  return 0;
}

/* ManalinkEh 2022: 0x2214540; original cards/ixalan.c:857. */
int card_temple_of_aclazotz(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int uVar2;
  int result;
  unsigned int player_00;
  test_definition_t test;

  double_faced_card(player, card, event);
  check_legend_rule(player, card, event);
  if (event == EVENT_CAN_ACTIVATE)
  {

    uVar2 = mana_producer(player, card, EVENT_CAN_ACTIVATE);
    if ((((uVar2 == 0) && (result = is_tapped(player, card), result == 0)) &&
         (result = is_animated_and_sick(player, card), result == 0)) &&
        (result = can_use_activated_abilities(player, card), result != 0))
    {
      result = can_sacrifice_as_cost(player, 1, 2, 0, 0, 0, 0, 0, 0, 0, -1, 0);
      uVar2 = (unsigned int)(result != 0);
    }
  }
  else
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->info_slot = 0;
      result = paying_mana();
      if ((((result == 0) && (result = is_tapped(player, card), result == 0)) &&
           ((result = is_animated_and_sick(player, card),
             result == 0 && ((result = can_use_activated_abilities(player, card),
                              result != 0 && (result = can_sacrifice_as_cost(player, 1, 2, 0, 0, 0,
                                                                             0, 0, 0, 0, -1, 0),
                                              result != 0)))))) &&
          (result =
               do_dialog(player, player, card, -1, -1, " Add mana\n Sac and gain life\n Cancel", 1),
           result != 0))
      {
        if (result != 1)
        {
          untap_card_no_event(player, card);
          spell_fizzled = 1;
          return 0;
        }
        add_state(player, card, 0x10);
        result = charge_mana_for_activated_ability(player, card, 0, 0, 0, 0, 0, 0);
        if (result == 0)
        {
          untap_card_no_event(player, card);
          spell_fizzled = 1;
        }
        else
        {

          new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
          uVar2 = new_sacrifice(player, card, player,
                                SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK, &test);
          if (uVar2 == 0)
          {
            untap_card_no_event(player, card);
            spell_fizzled = 1;
            return 0;
          }
          player_00 = (int)uVar2 >> 0x10 & 0xff;
          result = get_toughness(player_00, uVar2 >> 0x18);
          instance->info_slot = result;
          kill_card(player_00, uVar2 >> 0x18, KILL_SACRIFICE);
        }
        tapped_for_mana_color = -1;
        return 0;
      }
      result = mana_producer(player, card, EVENT_ACTIVATE);
      return result;
    }
    if ((event == EVENT_RESOLVE_ACTIVATION) && (0 < instance->info_slot))
    {
      gain_life(player, instance->info_slot);
    }
    uVar2 = mana_producer(player, card, event);
  }
  return uVar2;
}

/* ManalinkEh 2022: 0x2212420; original cards/ixalan.c:1062. */
static int revel_in_riches_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  unsigned int uVar2;
  card_instance_t *pcVar3;
  int result;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x445c))
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
  LAB_02212455:
    uVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
      goto joined_r0x0221247a;
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
      goto LAB_02212455;

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
        *(uint8_t *)((int)&pcVar1->targets[0xb].player + 1) = 1;
      }
      count_for_gfp_ability(player, card, 0x77, 1 - player, 2, NULL);
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

    for (uVar2 = uVar2 & 0xff; uVar2 != 0; uVar2 = uVar2 - 1)
    {
      damage_player(player, 1, player, card);
      draw_a_card(player);
    }
    kill_card(player, card, KILL_EXILE);
  }
joined_r0x0221247a:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2214ad0; original cards/ixalan.c:1117. */
int card_revel_in_riches(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uVar4;

  if (((event == EVENT_STATIC_EFFECTS) && (card == affected_card)) &&
      (player == affected_card_controller))
  {

    if (0 < active_cards_count[player])
    {
      iVar2 = 0;
      do
      {
        instance = in_play(player, iVar2);

        if ((((instance != NULL) && (iVar3 = is_what(player, iVar2, 0x80), iVar3 != 0)) &&
             (instance = get_card_instance(player, iVar2),
              instance->info_slot_fn == revel_in_riches_effect)) &&
            (((instance->damage_target_player == player &&
               (instance->damage_target_card == card)) ||
              ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
          goto LAB_02214aec;
        iVar2 = iVar2 + 1;
      } while (iVar2 < active_cards_count[player]);
    }

    iVar2 = create_targetted_legacy_effect(player, card, revel_in_riches_effect, player, card);
    instance = get_card_instance(player, iVar2);
    instance->targets[0].player = player;
    instance->targets[0].card = card;
    instance->number_of_targets = '\x01';
    add_status(player, iVar2, 0x10000);
  }
LAB_02214aec:
  count_for_gfp_ability(player, card, event, 1 - player, 2, NULL);
  instance = get_card_instance(player, card);
  if (instance->kill_code == '\0')
  {

    uVar4 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    generate_tokens_by_id(player, card, 0x4998, uVar4 & 0xff);
  }
  if ((((current_turn == player) && (iVar2 = upkeep_trigger(player, card, event), iVar2 != 0)) &&
       (iVar2 = is_humiliated_for_ability(player, card, AL_TRIG), iVar2 == 0)) &&
      (iVar2 = count_subtype(player, -1, 0x3006), 9 < iVar2))
  {
    lose_the_game(1 - player);
  }
  iVar2 = global_enchantment(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x2215d40; original cards/ixalan.c:1648. */
int card_spitfire_bastion(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  double_faced_card(player, card, event);
  if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      default_target_definition(player, card, &td, 0x102);
      td.zone = 0x1200;
      instance = get_card_instance(player, card);

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = paying_mana();
      if (((result == 0) &&
           (result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, 3, 0, 0, 0, 1,
                                               0, 0, &td, "TARGET_ANY"),
            result != 0)) &&
          (result =
               do_dialog(player, player, card, -1, -1, " Produce mana\n Deal damage\n Cancel", 1),
           result != 0))
      {
        if (result == 1)
        {
          instance->state = instance->state | 0x10;
          generic_activated_ability(player, card, EVENT_ACTIVATE, 4, 2, 0, 0, 0, 1, 0, 0, &td,
                                    "TARGET_ANY");
          if (spell_fizzled == 0)
          {
            instance->info_slot = 1;
          }
          else
          {
            instance->state = instance->state & 0xffffffef;
          }
          tapped_for_mana_color = -1;
        }
        else
        {
          spell_fizzled = 1;
        }
        return 0;
      }
      result = mana_producer(player, card, EVENT_ACTIVATE);
      return result;
    }
    if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      default_target_definition(player, card, &td, 0x102);
      td.zone = 0x1200;
      instance = get_card_instance(player, card);
      if (event != EVENT_RESOLVE_ACTIVATION)
      {
        return 0;
      }
      if (instance->info_slot == 1)
      {
        damage_target0(player, card, 3);
        return 0;
      }
      return 0;
    }
  }
  result = mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22169f0; original cards/ixalan.c:1951. */
int card_itlimoc_cradle_of_the_sun(int player, int card, event_t event)
{
  int result;

  double_faced_card(player, card, event);
  check_legend_rule(player, card, event);
  if (event == EVENT_CAN_ACTIVATE)
  {

    result = mana_producer(player, card, EVENT_CAN_ACTIVATE);
    if ((result != 0) &&
        (((ai_is_speculating != 1 && ((player != 1 || (((unsigned char)trace_mode & 2) != 0)))) ||
          (result = count_subtype(player, 2, -1), result != 0))))
    {
      return 1;
    }
  }
  else if (event == EVENT_ACTIVATE)
  {
    result = count_subtype(player, 2, -1);
    result = do_dialog(player, player, card, -1, -1, " Produce G\n Gaea\'s Cradle\n Cancel",
                       (unsigned int)(1 < result));
    if (result == 0)
    {
      event = EVENT_ACTIVATE;
    LAB_02216a53:

      result = mana_producer(player, card, event);
      return result;
    }
    if (result == 1)
    {
      result = count_subtype(player, 2, -1);
      produce_mana_tapped(player, card, COLOR_GREEN, result);
    }
    else
    {
      spell_fizzled = 1;
    }
  }
  else
  {
    if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      if (event != EVENT_COUNT_MANA)
      {
        return 0;
      }
    }
    else if (((event != EVENT_COUNT_MANA) || (card != affected_card)) ||
             (player != affected_card_controller))
      goto LAB_02216a53;
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = mana_producer(player, card, EVENT_CAN_ACTIVATE);
      if ((result != 0) && (result = count_subtype(player, 2, -1), 0 < result))
      {
        result = count_subtype(player, 2, -1);
        declare_mana_available(player, 3, result);
        return 0;
      }
      result = mana_producer(player, card, EVENT_CAN_ACTIVATE);
      if (result != 0)
      {
        declare_mana_available(player, 3, 1);
        return 0;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2216c50; original cards/ixalan.c:2028. */
int card_merfolk_branchwalker(int player, int card, event_t event)
{
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  explore(player, card, EVENT_ETB);
  return 0;
}

/* ManalinkEh 2022: 0x2219790; original cards/ixalan.c:3061. */
int card_treasure_cove(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t test;

  double_faced_card(player, card, event);
  if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      new_default_test_definition(&test, 0x47, "Select a Treasure to sacrifice.");
      test.id = 0x4998;
      instance = get_card_instance(player, card);

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = paying_mana();
      if (((result == 0) && (result = generic_activated_ability_with_sacrifice_as_cost(
                                 player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED, 0, 0, 0, 0, 0, 0,
                                 0, NULL, NULL, TYPE_NONE, &test, SARV_0),
                             result != 0)) &&
          (result = do_dialog(player, player, card, -1, -1,
                              " Produce mana\n Sac a Treasure\n Cancel", 1),
           result != 0))
      {
        if (result == 1)
        {
          generic_activated_ability_with_sacrifice_as_cost(player, card, EVENT_ACTIVATE,
                                                           GAA_UNTAPPED, 0, 0, 0, 0, 0, 0, 0, NULL,
                                                           NULL, TYPE_NONE, &test, SARV_0);
          instance->info_slot = 1;
        }
        else
        {
          spell_fizzled = 1;
        }
        return 0;
      }
      result = mana_producer(player, card, EVENT_ACTIVATE);
      return result;
    }
    if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      new_default_test_definition(&test, 0x47, "Select a Treasure to sacrifice.");
      test.id = 0x4998;
      instance = get_card_instance(player, card);
      if (event != EVENT_RESOLVE_ACTIVATION)
      {
        return 0;
      }
      if (instance->info_slot == 1)
      {
        draw_a_card(player);
        return 0;
      }
      return 0;
    }
  }
  result = mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2219c10; original cards/ixalan.c:3145. */
int card_field_of_ruin(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  unsigned int uVar4;
  target_definition_t td;

  if (event == EVENT_CAN_ACTIVATE)
  {
  LAB_02219c3e:
    iVar2 = mana_producer(player, card, event);
    return iVar2;
  }
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 1);
    if (player == 1)
      goto LAB_02219e08;
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_02219c3e;
    default_target_definition(player, card, &td, 1);
    if (player != 1)
    {
      instance = get_card_instance(player, card);
      goto LAB_02219d88;
    }
  LAB_02219e08:
    td.required_subtype = 0x2000;
    td.special = 0x1000;
  }
  instance = get_card_instance(player, card);
  if (event == EVENT_ACTIVATE)
  {

    instance->number_of_targets = '\0';
    instance->info_slot = 0;
    iVar2 = paying_mana();
    if (((iVar2 == 0) && (iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0xd,
                                                            3, 0, 0, 0, 0, 0, 0, &td, NULL),
                          iVar2 != 0)) &&
        (iVar2 = do_dialog(player, player, card, -1, -1, " Produce mana\n Kill a land\n Cancel", 1),
         iVar2 != 0))
    {
      if (iVar2 == 1)
      {
        add_state(player, card, 0x10);
        iVar2 = charge_mana_for_activated_ability(player, card, 2, 0, 0, 0, 0, 0);
        if ((iVar2 == 0) || (iVar2 = new_pick_target(&td, "TARGET_LAND", 0, 1), iVar2 == 0))
        {
          untap_card_no_event(player, card);
          spell_fizzled = 1;
        }
        else
        {
          instance->info_slot = 1;
          kill_card(player, card, KILL_SACRIFICE);
        }
        tapped_for_mana_color = -1;
        return 0;
      }
      spell_fizzled = 1;
      return 0;
    }
    iVar2 = mana_producer(player, card, EVENT_ACTIVATE);
    return iVar2;
  }
LAB_02219d88:
  if (((event == EVENT_RESOLVE_ACTIVATION) && (instance->info_slot == 1)) &&
      (iVar2 = validate_target(player, card, &td, 0), iVar2 != 0))
  {

    uVar4 = 0;
    kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    iVar2 = 1;
    do
    {
      iVar1 = current_turn;
      if (current_turn == uVar4)
      {
        iVar2 = do_dialog(0, player, card, -1, -1, " Tutor a basic land\n Pass", 0);
        iVar2 = 1 - iVar2;
      }
      if (iVar2 != 0)
      {
        tutor_basic_land(iVar1 ^ uVar4, 1, 0);
      }
      uVar4 = uVar4 + 1;
    } while (uVar4 != 2);
  }
  return 0;
}
