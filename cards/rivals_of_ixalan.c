#include "manalink.h"

/*********
 * White *
 *********/

/* Baffling End	|1|W
 * Enchantment
 * When ~ enters the battlefield, exile target creature an opponent controls with converted mana cost 3 or less.
 * When ~ leaves the battlefield, target opponent creates a 3/3 |Sgreen Dinosaur creature token with trample. */

/* Bishop of Binding	|3|W
 * Creature - Vampire Cleric 1/1
 * When ~ enters the battlefield, exile target creature an opponent controls until ~ leaves the battlefield.
 * Whenever ~ attacks, target Vampire gets +X/+X until end of turn, where X is the power of the exiled card. */

/* Blazing Hope	|W
 * Instant
 * Exile target creature with power greater than or equal to your life total. */

/* Cleansing Ray	|1|W
 * Sorcery
 * Choose one -
 * * Destroy target Vampire.
 * * Destroy target enchantment. */

// Divine Verdict - reprint from Magic 2010

/* Everdawn Champion	|1|W|W
 * Creature - Human Soldier 2/2
 * Prevent all combat damage that would be dealt to ~. */

/* Exultant Skymarcher	|1|W|W
 * Creature - Vampire Soldier 2/3
 * Flying */

/* Famished Paladin	|1|W
 * Creature - Vampire Knight 3/3
 * ~ doesn't untap during your untap step.
 * Whenever you gain life, untap ~. */

/* Forerunner of the Legion	|2|W
 * Creature - Vampire Knight 2/2
 * When ~ enters the battlefield, you may search your library for a Vampire card, reveal it, then shuffle your library and put that card on top of it.
 * Whenever another Vampire enters the battlefield under your control, target creature gets +1/+1 until end of turn. */

/* Imperial Ceratops	|4|W
 * Creature - Dinosaur 3/5
 * Enrage - Whenever ~ is dealt damage, you gain 2 life. */

// Legion Conquistador - reprint from Ixalan

/* Luminous Bonds	|2|W
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature can't attack or block. */

/* Majestic Heliopterus	|3|W
 * Creature - Dinosaur 2/2
 * Flying
 * Whenever ~ attacks, another target Dinosaur you control gains flying until end of turn. */

/* Martyr of Dusk	|1|W
 * Creature - Vampire Soldier 2/1
 * When ~ dies, create a 1/1 |Swhite Vampire creature token with lifelink. */

/* Moment of Triumph	|W
 * Instant
 * Target creature gets +2/+2 until end of turn. You gain 2 life. */

/* Paladin of Atonement	|1|W
 * Creature - Vampire Knight 1/1
 * At the beginning of each upkeep, if you lost life last turn, put a +1/+1 counter on ~.
 * When ~ dies, you gain life equal to its toughness. */

/* Pride of Conquerors	|1|W
 * Instant
 * Ascend
 * Creatures you control get +1/+1 until end of turn. If you have the city's blessing, those creatures get +2/+2 until end of turn instead. */

/* Radiant Destiny	|2|W
 * Enchantment
 * Ascend
 * As ~ enters the battlefield, choose a creature type.
 * Creatures you control of the chosen type get +1/+1. As long as you have the city's blessing, they also have vigilance. */

// Raptor Companion - reprint from Ixalan

/* Sanguine Glorifier	|3|W
 * Creature - Vampire Cleric 3/3
 * When ~ enters the battlefield, put a +1/+1 counter on another target Vampire you control. */

/* Skymarcher Aspirant	|W
 * Creature - Vampire Soldier 2/1
 * Ascend
 * ~ has flying as long as you have the city's blessing. */

/* Slaughter the Strong	|1|W|W
 * Sorcery
 * Each player chooses any number of creatures they control with total power 4 or less, then sacrifices all other creatures they control. */

/* Snubhorn Sentry	|W
 * Creature - Dinosaur 0/3
 * Ascend
 * ~ gets +3/+0 as long as you have the city's blessing. */

/* Sphinx's Decree	|1|W
 * Sorcery
 * Each opponent can't cast instant or sorcery spells during that player's next turn. */

/* Squire's Devotion	|2|W
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature gets +1/+1 and has lifelink.
 * When ~ enters the battlefield, create a 1/1 |Swhite Vampire creature token with lifelink. */

/* Sun Sentinel	|1|W
 * Creature - Human Soldier 2/2
 * Vigilance */

/* Sun-Crested Pterodon	|4|W
 * Creature - Dinosaur 2/5
 * Flying
 * ~ has vigilance as long as you control another Dinosaur. */

/* Temple Altisaur	|4|W
 * Creature - Dinosaur 3/4
 * If a source would deal damage to another Dinosaur you control, prevent all but 1 of that damage. */

/* Trapjaw Tyrant	|3|W|W
 * Creature - Dinosaur 5/5
 * Enrage - Whenever ~ is dealt damage, exile target creature an opponent controls until ~ leaves the battlefield. */

/* Zetalpa, Primal Dawn	|6|W|W
 * Legendary Creature - Elder Dinosaur 4/8
 * Flying, double strike, vigilance, trample, indestructible */

/********
 * Blue *
 ********/

/* Admiral's Order	|1|U|U
 * Instant
 * Raid - If you attacked with a creature this turn, you may pay |U rather than pay this spell's mana cost.
 * Counter target spell. */

/* Aquatic Incursion	|3|U
 * Enchantment
 * When ~ enters the battlefield, create two 1/1 |Sblue Merfolk creature tokens with hexproof.
 * |3|U: Target Merfolk can't be blocked this turn. */

/* Crafty Cutpurse	|3|U
 * Creature - Human Pirate 2/2
 * Flash
 * When ~ enters the battlefield, each token that would be created under an opponent's control this turn is created under your control instead. */

/* Crashing Tide	|2|U
 * Sorcery
 * ~ has flash as long as you control a Merfolk.
 * Return target creature to its owner's hand.
 * Draw a card. */

/* Curious Obsession	|U
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature gets +1/+1 and has "Whenever this creature deals combat damage to a player, you may draw a card."
 * At the beginning of your end step, if you didn't attack with a creature this turn, sacrifice ~. */

/* Deadeye Rig-Hauler	|3|U
 * Creature - Human Pirate 3/2
 * Raid - When ~ enters the battlefield, if you attacked with a creature this turn, you may return target creature to its owner's hand. */

/* Expel from Orazca	|1|U
 * Instant
 * Ascend
 * Return target nonland permanent to its owner's hand. If you have the city's blessing, you may put that permanent on top of its owner's library instead. */

/* Flood of Recollection	|U|U
 * Sorcery
 * Return target instant or sorcery card from your graveyard to your hand. Exile ~. */

/* Hornswoggle	|2|U
 * Instant
 * Counter target creature spell. You create a colorless Treasure artifact token with "|T, Sacrifice this artifact: Add one mana of any color." */

/* Induced Amnesia	|2|U
 * Enchantment
 * When ~ enters the battlefield, target player exiles all cards from their hand face down, then draws that many cards.
 * When ~ is put into a graveyard from the battlefield, return the exiled cards to their owner's hand. */

/* Kitesail Corsair	|1|U
 * Creature - Human Pirate 2/1
 * ~ has flying as long as it's attacking. */

/* Kumena's Awakening	|2|U|U
 * Enchantment
 * Ascend
 * At the beginning of your upkeep, each player draws a card. If you have the city's blessing, instead only you draw a card. */

/* Mist-Cloaked Herald	|U
 * Creature - Merfolk Warrior 1/1
 * ~ can't be blocked. */

// Negate - reprint from Morningtide

/* Nezahal, Primal Tide	|5|U|U
 * Legendary Creature - Elder Dinosaur 7/7
 * ~ can't be countered.
 * You have no maximum hand size.
 * Whenever an opponent casts a noncreature spell, draw a card.
 * Discard three cards: Exile ~. Return it to the battlefield tapped under its owner's control at the beginning of the next end step. */

/* Release to the Wind	|2|U
 * Instant
 * Exile target nonland permanent. For as long as that card remains exiled, its owner may cast it without paying its mana cost. */

/* River Darter	|2|U
 * Creature - Merfolk Warrior 2/3
 * ~ can't be blocked by Dinosaurs. */

/* Riverwise Augur	|3|U
 * Creature - Merfolk Wizard 2/2
 * When ~ enters the battlefield, draw three cards, then put two cards from your hand on top of your library in any order. */

// Sailor of Means - reprint from Ixalan

/* Sea Legs	|U
 * Enchantment - Aura
 * Flash
 * Enchant creature
 * Enchanted creature gets +0/+2 as long as it's a Pirate. Otherwise, it gets -2/-0. */

/* Seafloor Oracle	|2|U|U
 * Creature - Merfolk Wizard 2/3
 * Whenever a Merfolk you control deals combat damage to a player, draw a card. */

/* Secrets of the Golden City	|1|U|U
 * Sorcery
 * Ascend
 * Draw two cards. If you have the city's blessing, draw three cards instead. */

// Silvergill Adept - reprint from Lorwyn

/* Siren Reaver	|3|U
 * Creature - Siren Pirate 3/2
 * Raid - This spell costs |1 less to cast if you attacked with a creature this turn.
 * Flying */

/* Slippery Scoundrel	|2|U
 * Creature - Human Pirate 2/2
 * Ascend
 * As long as you have the city's blessing, ~ has hexproof and can't be blocked. */

/* Soul of the Rapids	|3|U|U
 * Creature - Elemental 3/2
 * Flying
 * Hexproof */

/* Spire Winder	|3|U
 * Creature - Snake 2/3
 * Flying
 * Ascend
 * ~ gets +1/+1 as long as you have the city's blessing. */

/* Sworn Guardian	|1|U
 * Creature - Merfolk Warrior 1/3 */

/* Timestream Navigator	|1|U
 * Creature - Human Pirate Wizard 1/1
 * Ascend
 * |2|U|U, |T, Put ~ on the bottom of its owner's library: Take an extra turn after this one. Activate this ability only if you have the city's blessing. */

/* Warkite Marauder	|1|U
 * Creature - Human Pirate 2/1
 * Flying
 * Whenever ~ attacks, target creature defending player controls loses all abilities and has base power and toughness 0/1 until end of turn. */

/* Waterknot	|1|U|U
 * Enchantment - Aura
 * Enchant creature
 * When ~ enters the battlefield, tap enchanted creature.
 * Enchanted creature doesn't untap during its controller's untap step. */

/*********
 * Black *
 ********/

/* Arterial Flow	|1|B|B
 * Sorcery
 * Each opponent discards two cards. If you control a Vampire, each opponent loses 2 life and you gain 2 life. */

/* Canal Monitor	|4|B
 * Creature - Lizard 5/3 */

/* Champion of Dusk	|3|B|B
 * Creature - Vampire Knight 4/4
 * When ~ enters the battlefield, you draw X cards and you lose X life, where X is the number of Vampires you control. */

/* Dark Inquiry	|2|B
 * Sorcery
 * Target opponent reveals their hand. You choose a nonland card from it. That player discards that card. */

/* Dead Man's Chest	|1|B
 * Enchantment - Aura
 * Enchant creature an opponent controls
 * When enchanted creature dies, exile cards equal to its power from the top of its owner's library. You may cast nonland cards from among them for as long as they remain exiled, and you may spend mana as though it were mana of any type to cast those spells. */

/* Dinosaur Hunter	|1|B
 * Creature - Human Pirate 2/2
 * Whenever ~ deals damage to a Dinosaur, destroy that creature. */

/* Dire Fleet Poisoner	|1|B
 * Creature - Human Pirate 2/2
 * Flash
 * Deathtouch
 * When ~ enters the battlefield, target attacking Pirate you control gets +1/+1 and gains deathtouch until end of turn. */

/* Dusk Charger	|3|B
 * Creature - Horse 3/3
 * Ascend
 * ~ gets +2/+2 as long as you have the city's blessing. */

/* Dusk Legion Zealot	|1|B
 * Creature - Vampire Soldier 1/1
 * When ~ enters the battlefield, you draw a card and you lose 1 life. */

/* Fathom Fleet Boarder	|2|B
 * Creature - Orc Pirate 3/3
 * When ~ enters the battlefield, you lose 2 life unless you control another Pirate. */

/* Forerunner of the Coalition	|2|B
 * Creature - Human Pirate 2/2
 * When ~ enters the battlefield, you may search your library for a Pirate card, reveal it, then shuffle your library and put that card on top of it.
 * Whenever another Pirate enters the battlefield under your control, each opponent loses 1 life. */

/* Golden Demise	|1|B|B
 * Sorcery
 * Ascend
 * All creatures get -2/-2 until end of turn. If you have the city's blessing, instead only creatures your opponents control get -2/-2 until end of turn. */

/* Grasping Scoundrel	|B
 * Creature - Human Pirate 1/1
 * ~ gets +1/+0 as long as it's attacking. */

/* Gruesome Fate	|2|B
 * Sorcery
 * Each opponent loses 1 life for each creature you control. */

/* Impale	|2|B|B
 * Sorcery
 * Destroy target creature. */

/* Mastermind's Acquisition	|2|B|B
 * Sorcery
 * Choose one -
 * * Search your library for a card, put it into your hand, then shuffle your library.
 * * Choose a card you own from outside the game and put it into your hand. */

/* Mausoleum Harpy	|4|B
 * Creature - Harpy 3/3
 * Flying
 * Ascend
 * Whenever another creature you control dies, if you have the city's blessing, put a +1/+1 counter on ~. */

/* Moment of Craving	|1|B
 * Instant
 * Target creature gets -2/-2 until end of turn. You gain 2 life. */

/* Oathsworn Vampire	|1|B
 * Creature - Vampire Knight 2/2
 * ~ enters the battlefield tapped.
 * You may cast ~ from your graveyard if you gained life this turn. */

/* Pitiless Plunderer	|3|B
 * Creature - Human Pirate 1/4
 * Whenever another creature you control dies, create a colorless Treasure artifact token with "|T, Sacrifice this artifact: Add one mana of any color." */

/* Ravenous Chupacabra	|2|B|B
 * Creature - Beast Horror 2/2
 * When ~ enters the battlefield, destroy target creature an opponent controls. */

/* Reaver Ambush	|2|B
 * Instant
 * Exile target creature with power 3 or less. */

// Recover - reprint from Invasion

/* Sadistic Skymarcher	|2|B
 * Creature - Vampire Soldier 2/2
 * As an additional cost to cast this spell, reveal a Vampire card from your hand or pay |1.
 * Flying, lifelink */

/* Tetzimoc, Primal Death	|4|B|B
 * Legendary Creature - Elder Dinosaur 6/6
 * Deathtouch
 * |B, Reveal ~ from your hand: Put a prey counter on target creature. Activate this ability only during your turn.
 * When ~ enters the battlefield, destroy each creature your opponents control with a prey counter on it. */

/* Tomb Robber	|2|B
 * Creature - Human Pirate 1/1
 * Menace
 * |1, Discard a card: ~ explores. */

/* Twilight Prophet	|2|B|B
 * Creature - Vampire Cleric 2/4
 * Flying
 * Ascend
 * At the beginning of your upkeep, if you have the city's blessing, reveal the top card of your library and put it into your hand. Each opponent loses X life and you gain X life, where X is that card's converted mana cost. */

/* Vampire Revenant	|3|B
 * Creature - Vampire Spirit 3/1
 * Flying */

/* Vona's Hunger	|2|B
 * Instant
 * Ascend
 * Each opponent sacrifices a creature. If you have the city's blessing, instead each opponent sacrifices half the creatures they control, rounded up. */

/* Voracious Vampire	|2|B
 * Creature - Vampire Knight 2/2
 * Menace
 * When ~ enters the battlefield, target Vampire you control gets +1/+1 and gains menace until end of turn. */

/*******
 * Red *
 *******/

/* Blood Sun	|2|R
 * Enchantment
 * When ~ enters the battlefield, draw a card.
 * All lands lose all abilities except mana abilities. */

/* Bombard	|2|R
 * Instant
 * ~ deals 4 damage to target creature. */

/* Brass's Bounty	|6|R
 * Sorcery
 * For each land you control, create a colorless Treasure artifact token with "|T, Sacrifice this artifact: Add one mana of any color." */

/* Brazen Freebooter	|3|R
 * Creature - Human Pirate 3/3
 * When ~ enters the battlefield, create a colorless Treasure artifact token with "|T, Sacrifice this artifact: Add one mana of any color." */

/* Buccaneer's Bravado	|1|R
 * Instant
 * Choose one -
 * * Target creature gets +1/+1 and gains first strike until end of turn.
 * * Target Pirate gets +1/+1 and gains double strike until end of turn. */

/* Charging Tuskodon	|3|R|R
 * Creature - Dinosaur 4/4
 * Trample
 * If ~ would deal combat damage to a player, it deals double that damage to that player instead. */

/* Daring Buccaneer	|R
 * Creature - Human Pirate 2/2
 * As an additional cost to cast this spell, reveal a Pirate card from your hand or pay |2. */

/* Dire Fleet Daredevil	|1|R
 * Creature - Human Pirate 2/1
 * First strike
 * When ~ enters the battlefield, exile target instant or sorcery card from an opponent's graveyard. You may cast that card this turn, and you may spend mana as though it were mana of any type to cast that spell. If that card would be put into a graveyard this turn, exile it instead. */

/* Etali, Primal Storm	|4|R|R
 * Legendary Creature - Elder Dinosaur 6/6
 * Whenever ~ attacks, exile the top card of each player's library, then you may cast any number of nonland cards exiled this way without paying their mana costs. */

/* Fanatical Firebrand	|R
 * Creature - Goblin Pirate 1/1
 * Haste
 * |T, Sacrifice ~: It deals 1 damage to any target. */

/* Forerunner of the Empire	|3|R
 * Creature - Human Soldier 1/3
 * When ~ enters the battlefield, you may search your library for a Dinosaur card, reveal it, then shuffle your library and put that card on top of it.
 * Whenever a Dinosaur enters the battlefield under your control, you may have ~ deal 1 damage to each creature. */

/* Form of the Dinosaur	|4|R|R
 * Enchantment
 * When ~ enters the battlefield, your life total becomes 15.
 * At the beginning of your upkeep, ~ deals 15 damage to target creature an opponent controls and that creature deals damage equal to its power to you. */

/* Frilled Deathspitter	|2|R
 * Creature - Dinosaur 3/2
 * Enrage - Whenever ~ is dealt damage, it deals 2 damage to target opponent or planeswalker. */

/* Goblin Trailblazer	|1|R
 * Creature - Goblin Pirate 2/1
 * Menace */

/* Mutiny	|R
 * Sorcery
 * Target creature an opponent controls deals damage equal to its power to another target creature that player controls. */

/* Needletooth Raptor	|3|R
 * Creature - Dinosaur 2/2
 * Enrage - Whenever ~ is dealt damage, it deals 5 damage to target creature an opponent controls. */

/* Orazca Raptor	|2|R|R
 * Creature - Dinosaur 3/4 */

/* Pirate's Pillage	|3|R
 * Sorcery
 * As an additional cost to cast this spell, discard a card.
 * Draw two cards and create two colorless Treasure artifact tokens with "|T, Sacrifice this artifact: Add one mana of any color." */

/* Reckless Rage	|R
 * Instant
 * ~ deals 4 damage to target creature you don't control and 2 damage to target creature you control. */

/* Rekindling Phoenix	|2|R|R
 * Creature - Phoenix 4/3
 * Flying
 * When ~ dies, create a 0/1 |Sred Elemental creature token with "At the beginning of your upkeep, sacrifice this creature and return target card named ~ from your graveyard to the battlefield. It gains haste until end of turn." */

/* See Red	|1|R
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature gets +2/+1 and has first strike.
 * At the beginning of your end step, if you didn't attack with a creature this turn, sacrifice ~. */

/* Shake the Foundations	|2|R
 * Instant
 * ~ deals 1 damage to each creature without flying.
 * Draw a card. */

// Shatter - reprint from Alpha

/* Silverclad Ferocidons	|5|R|R
 * Creature - Dinosaur 8/5
 * Enrage - Whenever ~ is dealt damage, each opponent sacrifices a permanent. */

/* Stampeding Horncrest	|4|R
 * Creature - Dinosaur 4/4
 * ~ has haste as long as you control another Dinosaur. */

/* Storm Fleet Swashbuckler	|1|R
 * Creature - Human Pirate 2/2
 * Ascend
 * ~ has double strike as long as you have the city's blessing. */

/* Sun-Collared Raptor	|1|R
 * Creature - Dinosaur 1/2
 * Trample
 * |2|R: ~ gets +3/+0 until end of turn. */

/* Swaggering Corsair	|2|R
 * Creature - Human Pirate 2/2
 * Raid - ~ enters the battlefield with a +1/+1 counter on it if you attacked with a creature this turn. */

/* Tilonalli's Crown	|1|R
 * Enchantment - Aura
 * Enchant creature
 * When ~ enters the battlefield, it deals 1 damage to enchanted creature.
 * Enchanted creature gets +3/+0 and has trample. */

/* Tilonalli's Summoner	|1|R
 * Creature - Human Shaman 1/1
 * Ascend
 * Whenever ~ attacks, you may pay |X|R. If you do, create X 1/1 |Sred Elemental creature tokens that are tapped and attacking. At the beginning of the next end step, exile those tokens unless you have the city's blessing. */

/*********
 * Green *
 *********/

// Aggressive Urge - reprint from Invasion

/* Cacophodon	|3|G
 * Creature - Dinosaur 2/5
 * Enrage - Whenever ~ is dealt damage, untap target permanent. */

/* Cherished Hatchling	|1|G
 * Creature - Dinosaur 2/1
 * When ~ dies, you may cast Dinosaur spells this turn as though they had flash, and whenever you cast a Dinosaur spell this turn, it gains "When this creature enters the battlefield, you may have it fight another target creature." */

// Colossal Dreadmaw - reprint from Ixalan

/* Crested Herdcaller	|3|G|G
 * Creature - Dinosaur 3/3
 * Trample
 * When ~ enters the battlefield, create a 3/3 |Sgreen Dinosaur creature token with trample. */

/* Deeproot Elite	|1|G
 * Creature - Merfolk Warrior 1/1
 * Whenever another Merfolk enters the battlefield under your control, put a +1/+1 counter on target Merfolk you control. */

/* Enter the Unknown	|G
 * Sorcery
 * Target creature you control explores.
 * You may play an additional land this turn. */

/* Forerunner of the Heralds	|3|G
 * Creature - Merfolk Scout 3/2
 * When ~ enters the battlefield, you may search your library for a Merfolk card, reveal it, then shuffle your library and put that card on top of it.
 * Whenever another Merfolk enters the battlefield under your control, put a +1/+1 counter on ~. */

/* Ghalta, Primal Hunger	|10|G|G
 * Legendary Creature - Elder Dinosaur 12/12
 * ~ costs |X less to cast, where X is the total power of creatures you control.
 * Trample */

/* Giltgrove Stalker	|1|G
 * Creature - Merfolk Warrior 2/1
 * ~ can't be blocked by creatures with power 2 or less. */

/* Hardy Veteran	|1|G
 * Creature - Human Warrior 2/2
 * As long as it's your turn, ~ gets +0/+2. */

// Hunt the Weak - reprint from Magic 2014

/* Jade Bearer	|G
 * Creature - Merfolk Shaman 1/1
 * When ~ enters the battlefield, put a +1/+1 counter on another target Merfolk you control. */

/* Jadecraft Artisan	|3|G
 * Creature - Merfolk Shaman 3/3
 * When ~ enters the battlefield, target creature gets +2/+2 until end of turn. */

/* Jadelight Ranger	|1|G|G
 * Creature - Merfolk Scout 2/1
 * When ~ enters the battlefield, it explores, then it explores again. */

/* Jungleborn Pioneer	|2|G
 * Creature - Merfolk Scout 2/2
 * When ~ enters the battlefield, create a 1/1 |Sblue Merfolk creature token with hexproof. */

/* Knight of the Stampede	|3|G
 * Creature - Human Knight 2/4
 * Dinosaur spells you cast cost |2 less to cast. */

// Naturalize - reprint from Onslaught

/* Orazca Frillback	|2|G
 * Creature - Dinosaur 4/2 */

/* Overgrown Armasaur	|3|G|G
 * Creature - Dinosaur 4/4
 * Enrage - Whenever ~ is dealt damage, create a 1/1 |Sgreen Saproling creature token. */

/* Path of Discovery	|3|G
 * Enchantment
 * Whenever a creature enters the battlefield under your control, it explores. */

// Plummet - reprint from Magic 2011

/* Polyraptor	|6|G|G
 * Creature - Dinosaur 5/5
 * Enrage - Whenever ~ is dealt damage, create a token that's a copy of ~. */

/* Strength of the Pack	|4|G|G
 * Sorcery
 * Put two +1/+1 counters on each creature you control. */

/* Swift Warden	|1|G|G
 * Creature - Merfolk Warrior 3/3
 * Flash
 * When ~ enters the battlefield, target Merfolk you control gains hexproof until end of turn. */

/* Tendershoot Dryad	|4|G
 * Creature - Dryad 2/2
 * Ascend
 * At the beginning of each upkeep, create a 1/1 |Sgreen Saproling creature token.
 * Saprolings you control get +2/+2 as long as you have the city's blessing. */

/* Thrashing Brontodon	|1|G|G
 * Creature - Dinosaur 3/4
 * |1, Sacrifice ~: Destroy target artifact or enchantment. */

/* Thunderherd Migration	|1|G
 * Sorcery
 * As an additional cost to cast this spell, reveal a Dinosaur card from your hand or pay |1.
 * Search your library for a basic land card, put it onto the battlefield tapped, then shuffle your library. */

/* Wayward Swordtooth	|2|G
 * Creature - Dinosaur 5/5
 * Ascend
 * You may play an additional land on each of your turns.
 * ~ can't attack or block unless you have the city's blessing. */

/* World Shaper	|3|G
 * Creature - Merfolk Shaman 3/3
 * Whenever ~ attacks, you may put the top three cards of your library into your graveyard.
 * When ~ dies, put all land cards from your graveyard onto the battlefield tapped. */

/********
 * Gold *
 ********/

/* Angrath, the Flame-Chained	|3|B|R
 * Legendary Planeswalker - Angrath (4)
 * +1: Each opponent discards a card and loses 2 life.
 * -3: Gain control of target creature until end of turn. Untap it. It gains haste until end of turn. Sacrifice it at the beginning of the next end step if it has converted mana cost 3 or less.
 * -8: Each opponent loses life equal to the number of cards in their graveyard. */

/* Atzocan Seer	|1|G|W
 * Creature - Human Druid 2/3
 * |T: Add one mana of any color.
 * Sacrifice ~: Return target Dinosaur card from your graveyard to your hand. */

/* Azor, the Lawbringer	|2|W|W|U|U
 * Legendary Creature - Sphinx 6/6
 * Flying
 * When ~ enters the battlefield, each opponent can't cast instant or sorcery spells during that player's next turn.
 * Whenever ~ attacks, you may pay |X|W|U|U. If you do, you gain X life and draw X cards. */

/* Deadeye Brawler	|2|U|B
 * Creature - Human Pirate 2/4
 * Deathtouch
 * Ascend
 * Whenever ~ deals combat damage to a player, if you have the city's blessing, draw a card. */

/* Dire Fleet Neckbreaker	|2|B|R
 * Creature - Orc Pirate 3/2
 * Attacking Pirates you control get +2/+0. */

/* Elenda, the Dusk Rose	|2|W|B
 * Legendary Creature - Vampire Knight 1/1
 * Lifelink
 * Whenever another creature dies, put a +1/+1 counter on ~.
 * When ~ dies, create X 1/1 |Swhite Vampire creature tokens with lifelink, where X is ~'s power. */

/* Hadana's Climb	|1|G|U
 * Legendary Enchantment
 * At the beginning of combat on your turn, put a +1/+1 counter on target creature you control. Then if that creature has three or more +1/+1 counters on it, transform ~. */

/* Huatli, Radiant Champion	|2|G|W
 * Legendary Planeswalker - Huatli (3)
 * +1: Put a loyalty counter on ~ for each creature you control.
 * -1: Target creature gets +X/+X until end of turn, where X is the number of creatures you control.
 * -8: You get an emblem with "Whenever a creature enters the battlefield under your control, you may draw a card." */

/* Huatli, Radiant Champion Emblem	""
 * Emblem - Huatli
 * Whenever a creature enters the battlefield under your control, you may draw a card. */

/* Journey to Eternity	|1|B|G
 * Legendary Enchantment - Aura
 * Enchant creature you control
 * When enchanted creature dies, return it to the battlefield under your control, then return ~ to the battlefield transformed under your control. */

/* Jungle Creeper	|1|B|G
 * Creature - Elemental 3/3
 * |3|B|G: Return ~ from your graveyard to your hand. */

/* Kumena, Tyrant of Orazca	|1|G|U
 * Legendary Creature - Merfolk Shaman 2/4
 * Tap another untapped Merfolk you control: ~ can't be blocked this turn.
 * Tap three untapped Merfolk you control: Draw a card.
 * Tap five untapped Merfolk you control: Put a +1/+1 counter on each Merfolk you control. */

/* Legion Lieutenant	|W|B
 * Creature - Vampire Knight 2/2
 * Other Vampires you control get +1/+1. */

/* Merfolk Mistbinder	|G|U
 * Creature - Merfolk Shaman 2/2
 * Other Merfolk you control get +1/+1. */

/* Path of Mettle	|R|W
 * Legendary Enchantment
 * When ~ enters the battlefield, it deals 1 damage to each creature that doesn't have first strike, double strike, vigilance, or haste.
 * Whenever you attack with at least two creatures that have first strike, double strike, vigilance, and/or haste, transform ~. */

/* Profane Procession	|1|W|B
 * Legendary Enchantment
 * |3|W|B: Exile target creature. Then if there are three or more cards exiled with ~, transform it. */

/* Protean Raider	|1|U|R
 * Creature - Shapeshifter Pirate 2/2
 * Raid - If you attacked with a creature this turn, you may have ~ enter the battlefield as a copy of any creature on the battlefield. */

/* Raging Regisaur	|2|R|G
 * Creature - Dinosaur 4/4
 * Whenever ~ attacks, it deals 1 damage to any target. */

/* Relentless Raptor	|R|W
 * Creature - Dinosaur 3/3
 * Vigilance
 * ~ attacks or blocks each combat if able. */

/* Resplendent Griffin	|1|W|U
 * Creature - Griffin 2/2
 * Flying
 * Ascend
 * Whenever ~ attacks, if you have the city's blessing, put a +1/+1 counter on it. */

/* Siegehorn Ceratops	|G|W
 * Creature - Dinosaur 2/2
 * Enrage - Whenever ~ is dealt damage, put two +1/+1 counters on it. */

/* Storm Fleet Sprinter	|1|U|R
 * Creature - Human Pirate 2/2
 * Haste
 * ~ can't be blocked. */

/* Storm the Vault	|2|U|R
 * Legendary Enchantment
 * Whenever one or more creatures you control deal combat damage to a player, create a colorless Treasure artifact token with "|T, Sacrifice this artifact: Add one mana of any color."
 * At the beginning of your end step, if you control five or more artifacts, transform ~. */

/* Zacama, Primal Calamity	|6|R|G|W
 * Legendary Creature - Elder Dinosaur 9/9
 * Vigilance, reach, trample
 * When ~ enters the battlefield, if you cast it, untap all lands you control.
 * |2|R: ~ deals 3 damage to target creature.
 * |2|G: Destroy target artifact or enchantment.
 * |2|W: You gain 3 life. */

/************
 * Artifact *
 ************/

/* Awakened Amalgam	|4
 * Artifact Creature - Golem 100/100
 * ~'s power and toughness are each equal to the number of differently named lands you control. */

/* Azor's Gateway	|2
 * Legendary Artifact
 * |1, |T: Draw a card, then exile a card from your hand. If cards with five or more different converted mana costs are exiled with ~, you gain 5 life, untap ~, and transform it. */

/* Captain's Hook	|3
 * Artifact - Equipment
 * Equipped creature gets +2/+0, has menace, and is a Pirate in addition to its other creature types.
 * Whenever ~ becomes unattached from a permanent, destroy that permanent.
 * Equip |1 */

/* Gleaming Barrier	|2
 * Artifact Creature - Wall 0/4
 * Defender
 * When ~ dies, create a colorless Treasure artifact token with "|T, Sacrifice this artifact: Add one mana of any color." */

/* Golden Guardian	|4
 * Artifact Creature - Golem 4/4
 * Defender
 * |2: ~ fights another target creature you control. When ~ dies this turn, return it to the battlefield transformed under your control. */

/* The Immortal Sun	|6
 * Legendary Artifact
 * Players can't activate planeswalkers' loyalty abilities.
 * At the beginning of your draw step, draw an additional card.
 * Spells you cast cost |1 less to cast.
 * Creatures you control get +1/+1. */

/* Orazca Relic	|3
 * Artifact
 * Ascend
 * |T: Add |C.
 * |T, Sacrifice ~: You gain 3 life and draw a card. Activate this ability only if you have the city's blessing. */

/* Silent Gravestone	|1
 * Artifact
 * Cards in graveyards can't be the targets of spells or abilities.
 * |4, |T: Exile ~ and all cards from all graveyards. Draw a card. */

// Strider Harness - reprint from Scars of Mirrodin

// Traveler's Amulet - reprint from Innistrad

/********
 * Land *
 ********/

/* Arch of Orazca	""
 * Land
 * Ascend
 * |T: Add |C.
 * |5, |T: Draw a card. Activate this ability only if you have the city's blessing. */

// Evolving Wilds - reprint from Rise of the Eldrazi

// Forsaken Sanctuary - reprint from Shadows over Innistrad

// Foul Orchard - reprint from Shadows over Innistrad

// Highland Lake - reprint from Shadows over Innistrad

// Stone Quarry - reprint from Shadows over Innistrad

// Woodland Stream - reprint from Shadows over Innistrad

// Plains, Island, Swamp, Mountain, Forest

/**********************
 * Planeswalker Decks *
 **********************/

/* Vraska, Scheming Gorgon	|4|B|B
 * Legendary Planeswalker - Vraska (5)
 * +2: Creatures you control get +1/+0 until end of turn.
 * -3: Destroy target creature.
 * -10: Until end of turn, creatures you control gain deathtouch and "Whenever this creature deals damage to an opponent, that player loses the game." */

/* Vampire Champion	|3|B
 * Creature - Vampire Soldier 3/3
 * Deathtouch */

/* Vraska's Conquistador	|1|B
 * Creature - Vampire Soldier 2/1
 * Whenever ~ attacks or blocks, if you control a Vraska planeswalker, target opponent loses 2 life and you gain 2 life. */

/* Vraska's Scorn	|2|B|B
 * Sorcery
 * Target opponent loses 4 life. You may search your library and/or graveyard for a card named Vraska, Scheming Gorgon, reveal it, and put it into your hand. If you search your library this way, shuffle it. */

/* Angrath, Minotaur Pirate	|4|B|R
 * Legendary Planeswalker - Angrath (5)
 * +2: ~ deals 1 damage to target opponent or planeswalker and each creature that player or that planeswalker's controller controls.
 * -3: Return target Pirate card from your graveyard to the battlefield.
 * -11: Destroy all creatures target opponent controls. ~ deals damage to that player equal to their total power. */

/* Angrath's Ambusher	|2|B
 * Creature - Orc Pirate 2/3
 * ~ gets +2/+0 as long as you control an Angrath planeswalker. */

/* Swab Goblin	|1|R
 * Creature - Goblin Pirate 2/2 */

/* Angrath's Fury	|3|B|R
 * Sorcery
 * Destroy target creature. ~ deals 3 damage to target player or planeswalker. You may search your library and/or graveyard for a card named Angrath, Minotaur Pirate, reveal it, and put it into your hand. If you search your library this way, shuffle it. */

// Cinder Barrens - reprint from Oath of the Gatewatch

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2190240; original cards/rivals_of_ixalan.c:378. */
int card_zetalpa_primal_dawn(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  vigilance_2022(player, card, event);
  indestructible_2022(player, card, event);
  return 0;
}

/* ManalinkEh 2022: 0x2190490; original cards/rivals_of_ixalan.c:449. */
int card_flood_of_recollection(int player, int card, event_t event)
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
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        new_default_test_definition(&this_test, 0x38, "Select a instant or sorcery spell.");
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = validate_target_from_grave_source(player, card, player, 0);
          if (result != -1)
          {
            from_grave_to_hand(player, result, 1);
          }
          kill_card(player, card, KILL_EXILE);
        }
        goto LAB_021904e8;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  new_default_test_definition(&this_test, 0x38, "Select a instant or sorcery spell.");
LAB_021904e8:
  result = generic_spell(player, card, event, 8, NULL, NULL, 1, &this_test);
  return result;
}

/* ManalinkEh 2022: 0x2190bd0; original cards/rivals_of_ixalan.c:666. */
int card_champion_of_dusk(int player, int card, event_t event)
{
  int amount;
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  amount = count_subtype(player, -1, 0xcd);
  lose_life(player, amount);
  draw_cards(player, amount);
  return 0;
}

/* ManalinkEh 2022: 0x2190c50; original cards/rivals_of_ixalan.c:713. */
int card_forerunner_of_the_coalition(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  result = duh_mode(player);
  etb_ability_2022(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 0x47);
    test.subtype = 0x95;
    test.not_me = 1;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_ETB)
  {
    if ((trigger_cause_controller != player) || (trigger_cause != card))
    {
      lose_life(1 - player, 1);
      return 0;
    }
    new_default_test_definition(&test, 0xff, "Select a Pirate card.");
    test.subtype = 0x95;
    new_global_tutor(player, player, 1, 2, 0, 1, &test);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2191130; original cards/rivals_of_ixalan.c:836. */
int card_moment_of_craving(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    gain_life(player, 2);
  }
  result = vanilla_instant_pump_2022(player, card, event, 2, 1 - player, -2, -2, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x21913b0; original cards/rivals_of_ixalan.c:951. */
int card_ravenous_chupacabra(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  instance = get_card_instance(player, card);
  default_target_definition(player, card, &td, 2);
  td.allowed_controller = 1 - player;
  td.allow_cancel = 0;
  result = can_target(&td);
  if (result != 0)
  {
    result = pick_target(&td, "TARGET_CREATURE_OPPONENT_CONTROLS");
    if (result != 0)
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x2191ec0; original cards/rivals_of_ixalan.c:1271. */
int card_fanatical_firebrand(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  haste(player, card, event);
  default_target_definition(player, card, &td, 0x102);
  td.zone = 0x1200;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    result = validate_target(player, card, &td, 0);
    if (result != 0)
    {
      damage_target0(player, card, 1);
    }
  }
  result =
      generic_activated_ability(player, card, event, 0xd, 0, 0, 0, 0, 0, 0, 0, &td, "TARGET_ANY");
  return result;
}

/* ManalinkEh 2022: 0x2191fc0; original cards/rivals_of_ixalan.c:1289. */
int card_forerunner_of_the_empire(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  result = duh_mode(player);
  etb_ability_2022(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 0x47);
    test.subtype = 0xf0;
    test.not_me = 1;
    result = duh_mode(player);
    etb_ability_global(player, event, (result != 0) + 1, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_ETB)
  {
    if ((trigger_cause_controller != player) || (trigger_cause != card))
    {
      default_test_definition(&test, 2);
      new_damage_all(player, card, 2, 1, 1, &test);
      return 0;
    }
    new_default_test_definition(&test, 0xff, "Select a Dinosaur card.");
    test.subtype = 0xf0;
    new_global_tutor(player, player, 1, 2, 0, 1, &test);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2192160; original cards/rivals_of_ixalan.c:1355. */
int card_rekindling_phoenix(int player, int card, event_t event)
{
  int result;
  token_generation_t token;
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  default_token_definition(player, card, 0x4926, &token);
  token.pow = 0;
  token.tou = 1;
  token.special_infos = 0x44;
  generate_token(&token);
  return 0;
}

/* ManalinkEh 2022: 0x21925d0; original cards/rivals_of_ixalan.c:1566. */
int card_ghalta_primal_hunger(int player, int card, event_t event)
{
  int result;
  check_legend_rule(player, card, event);
  if (event != EVENT_MODIFY_COST)
  {
    return 0;
  }
  result = get_power_sum(player);
  COST_COLORLESS = COST_COLORLESS - (char)result;
  return 0;
}

/* ManalinkEh 2022: 0x2192870; original cards/rivals_of_ixalan.c:1681. */
int card_thrashing_brontodon(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 0x44);
    get_card_instance(player, card);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return (unsigned int)(event == EVENT_CAN_CAST);
    }
    default_target_definition(player, card, &td, 0x44);
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = validate_target(player, card, &td, 0);
      if (result != 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_SACRIFICE);
      }
    }
  }
  result = generic_activated_ability(player, card, event, 0xc, 1, 0, 0, 0, 0, 0, 0, &td,
                                     "TARGET_ARTIFACT_ENCHANTMENT");
  return result;
}

/* ManalinkEh 2022: 0x218f6a0; original cards/rivals_of_ixalan.c:1878. */
static int azor_the_lawbringer_legacy(int player, int card, event_t event)
{
  if ((event == EVENT_CAN_SKIP_TURN) && (1 - player == current_turn))
  {
    target_player_cant_cast_type(player, card, 1 - player, 0x38);
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2193e30; original cards/rivals_of_ixalan.c:2141. */
int card_huatli_radiant_champion_emblem(int player, int card, event_t event)
{
  int result;
  result = duh_mode(player);
  etb_ability_global(player, event, (result != 0) + 1, TYPE_CREATURE, NULL, player, card);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  draw_cards(player, 1);
  return 0;
}

/* ManalinkEh 2022: 0x2194c00; original cards/rivals_of_ixalan.c:2389. */
int card_legion_lieutenant(int player, int card, event_t event)
{
  if (1 < event - EVENT_POWER)
  {
    return 0;
  }
  boost_creature_type(player, card, event, SUBTYPE_VAMPIRE, 1, 1, 0, BCT_CONTROLLER_ONLY);
  return 0;
}

/* ManalinkEh 2022: 0x2194c60; original cards/rivals_of_ixalan.c:2400. */
int card_merfolk_mistbinder(int player, int card, event_t event)
{
  if (1 < event - EVENT_POWER)
  {
    return 0;
  }
  boost_creature_type(player, card, event, SUBTYPE_MERFOLK, 1, 1, 0, BCT_CONTROLLER_ONLY);
  return 0;
}

/* ManalinkEh 2022: 0x218f2c0; original cards/rivals_of_ixalan.c:2411. */
static int not_fs_ds_v_or_h(int unused, int unused2, int player, int card)
{
  int result;
  result = check_for_ability(player, card, 0x100);
  if (result == 0)
  {
    result = check_for_ability(player, card, 0x40000000);
    if (result == 0)
    {
      result = check_for_special_ability(player, card, 8);
      if (result == 0)
      {
        result = check_for_special_ability(player, card, 4);
        return (unsigned int)(result == 0);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x218f230; original cards/rivals_of_ixalan.c:2423. */
static int has_fs_ds_v_or_h(int unused, int unused2, int player, int card)
{
  int result;
  result = check_for_ability(player, card, 0x100);
  if (result == 0)
  {
    result = check_for_ability(player, card, 0x40000000);
    if (result == 0)
    {
      result = check_for_special_ability(player, card, 8);
      if (result == 0)
      {
        result = check_for_special_ability(player, card, 4);
        return (unsigned int)(result != 0);
      }
    }
  }
  return 1;
}

/* ManalinkEh 2022: 0x21953f0; original cards/rivals_of_ixalan.c:2548. */
int card_profane_procession(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  double_faced_card(player, card, event);
  check_legend_rule(player, card, event);
  default_target_definition(player, card, &td, 2);
  if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
  {
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      instance = get_card_instance(player, card);
      result = validate_target(player, card, &td, 0);
      if (result != 0)
      {
        exile_card_and_remember_it_on_exiledby(instance->parent_controller, instance->parent_card,
                                               instance->targets[0].player,
                                               instance->targets[0].card);
      }
      result = exiledby_count(player, instance->parent_card, 2);
      if (2 < result)
      {
        transform(player, instance->parent_card);
      }
    }
    result = global_enchantment(player, card, event);
    return result;
  }
  result = generic_activated_ability(player, card, event, 4, 3, 1, 0, 0, 0, 1, 0, &td,
                                     "TARGET_CREATURE");
  return result;
}

/* ManalinkEh 2022: 0x2195ad0; original cards/rivals_of_ixalan.c:2706. */
int card_storm_fleet_sprinter(int player, int card, event_t event)
{
  haste(player, card, event);
  unblockable(player, card, event);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21900a0; original cards/rivals_of_ixalan.c:333. */
int card_temple_altisaur(int player, int card, event_t event)
{
  char cVar1;
  card_instance_t *instance;
  int result;

  if (event == EVENT_PREVENT_DAMAGE)
  {
    instance = get_card_instance(affected_card_controller, affected_card);
    if ((((instance->internal_card_id == damage_card) &&
          (cVar1 = instance->damage_target_player, -1 < cVar1)) &&
         (result = instance->damage_target_card, -1 < result)) &&
        ((result != card && (cVar1 == player))))
    {
      result = has_subtype((int)cVar1, result, SUBTYPE_DINOSAUR);
      if ((result != 0) && (1 < instance->info_slot))
      {
        prevent_damage(affected_card_controller, affected_card, instance->info_slot + -1);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2191ae0; original cards/rivals_of_ixalan.c:1157. */
int card_brasss_bounty(int player, int card, event_t event)
{
  int result;
  token_generation_t token;

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
      result = count_permanents_by_type(player, TYPE_LAND);
      default_token_definition(player, card, 0x4998, &token);
      token.qty = (unsigned char)result;
      generate_token(&token);
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

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int has_the_citys_blessing(int player);
static void ascend(int player, int card, event_t event);

/* ManalinkEh 2022: 0x218f4e0; original cards/rivals_of_ixalan.c:5. */
static void ascend(int player, int card, event_t event)
{
  bool bVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;

  if (event == EVENT_STATIC_EFFECTS)
  {
    iVar4 = 0x47;
  }
  else
  {
    if (event != EVENT_RESOLVE_SPELL)
    {
      return;
    }
    iVar4 = 0x38;
  }
  iVar4 = is_what(player, card, iVar4);
  if ((iVar4 != 0) && (iVar4 = count_permanents_by_type(player, TYPE_PERMANENT), 9 < iVar4))
  {

    bVar1 = false;
    if (0 < active_cards_count[player])
    {
      iVar4 = 0;
      do
      {
        instance = in_play(player, iVar4);
        if ((instance != NULL) && (iVar3 = get_id(player, iVar4), iVar3 == 0x47a0))
        {
          bVar1 = true;
        }
        iVar4 = iVar4 + 1;
      } while (iVar4 < active_cards_count[player]);
      if (bVar1)
      {
        return;
      }
    }
    generate_reserved_token_by_id(player, 0x47a0);
  }
  return;
}

/* ManalinkEh 2022: 0x218ed50; original cards/rivals_of_ixalan.c:25. */
static int has_the_citys_blessing(int player)
{
  card_instance_t *instance;
  int result;
  int card;

  if (0 < active_cards_count[player])
  {
    card = 0;
    do
    {
      instance = in_play(player, card);
      if ((instance != NULL) && (result = get_id(player, card), result == 0x47a0))
      {
        return card;
      }
      card = card + 1;
    } while (card < active_cards_count[player]);
  }
  return 0;
}

/* ManalinkEh 2022: 0x218fa90; original cards/rivals_of_ixalan.c:171. */
int card_pride_of_conquerors(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {

    ascend(player, card, EVENT_RESOLVE_SPELL);
    result = has_the_citys_blessing(player);
    result = (result != 0) + 1;
    pump_creatures_until_eot_merge_pt(player, card, player, result, result, NULL);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x218fb20; original cards/rivals_of_ixalan.c:186. */
int card_radiant_destiny(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  ascend(player, card, event);
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
    in_play(player, card);
  }
  else
  {
    instance = in_play(player, card);
    if (((instance != NULL) && (event - EVENT_POWER < 3)) && (affected_card_controller == player))
    {
      instance = get_card_instance(player, card);
      result = has_creature_type(player, affected_card, instance->info_slot);
      if (result != 0)
      {
        if (event < EVENT_ABILITIES)
        {
          event_result = event_result + 1;
        }
        else
        {
          result = has_the_citys_blessing(player);
          if (result != 0)
          {
            vigilance_2022(player, affected_card, EVENT_ABILITIES);
          }
        }
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x218fc80; original cards/rivals_of_ixalan.c:232. */
int card_skymarcher_aspirant(int player, int card, event_t event)
{
  int result;

  ascend(player, card, event);
  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = has_the_citys_blessing(player);
    if (result != 0)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        event_result = event_result | 0x20;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2190030; original cards/rivals_of_ixalan.c:303. */
int card_snubhorn_sentry(int player, int card, event_t event)
{
  int result;

  ascend(player, card, event);
  if (((event == EVENT_POWER) && (card == affected_card)) && (player == affected_card_controller))
  {
    result = has_the_citys_blessing(player);
    if (result != 0)
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

/* ManalinkEh 2022: 0x2190140; original cards/rivals_of_ixalan.c:355. */
int card_trapjaw_tyrant(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  return_from_oblivion(player, card, event);
  result = is_humiliated_for_ability(player, card, AL_TRIG);
  if (result == 0)
  {
    result = damage_dealt_to_me_arbitrary(player, card, event, DDBM_0, player, card);
    if (result != 0)
    {

      default_target_definition(player, card, &td, 2);
      td.allowed_controller = 1 - player;
      td.allow_cancel = 0;
      instance = get_card_instance(player, card);
      result = can_target(&td);
      if (result != 0)
      {
        result = new_pick_target(&td, "TARGET_CREATURE", 0, 0);
        if (result != 0)
        {
          obliviation(player, card, instance->targets[0].player, instance->targets[0].card);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2190290; original cards/rivals_of_ixalan.c:415. */
int card_curious_obsession(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  result = is_humiliated_for_ability(player, card, AL_TRIG);
  if ((result == 0) &&
      (instance = get_card_instance(player, card), -1 < instance->damage_target_player))
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
  if (((current_turn == player) && (result = eot_trigger(player, card, event), result != 0)) &&
      (result = get_trap_condition(player, 0x606), result == 0))
  {
    kill_card(player, card, KILL_SACRIFICE);
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_021902f0;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_021902f0;
    if (((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
        (((((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)) &&
            (event != EVENT_CHANGE_TYPE)) &&
           ((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)))) &&
          (event != EVENT_STATIC_EFFECTS))))
    {
      return 0;
    }
  }
LAB_021902f0:
  result = generic_aura_2022(player, card, event, player, 1, 1, 0, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x21905d0; original cards/rivals_of_ixalan.c:480. */
int card_kitesail_corsair(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      result = is_attacking(player, card);
      if (result != 0)
      {
        result = is_humiliated_for_ability(player, card, AL_STATIC);
        if (result == 0)
        {
          event_result = event_result | 0x20;
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21908e0; original cards/rivals_of_ixalan.c:602. */
int card_timestream_navigator(int player, int card, event_t event)
{
  int *piVar1;
  int iVar2;
  int iVar3;
  card_instance_t *instance;

  ascend(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    iVar2 = has_the_citys_blessing(player);
    if (iVar2 == 0)
    {
      return 0;
    }
    if (event == EVENT_ACTIVATE)
    {
      piVar1 = deck_ptr[player];
      iVar2 = count_deck(player);
      iVar3 = get_original_internal_card_id(player, card);
      piVar1[iVar2] = iVar3;
      obliterate_card(player, card);
      event = EVENT_ACTIVATE;
      goto LAB_02190989;
    }
  }
  else if (((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP)) ||
           (iVar2 = has_the_citys_blessing(player), iVar2 == 0))
  {
    return 0;
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    iVar2 = instance->parent_card;
    instance = get_card_instance(player, card);
    time_walk_effect(instance->parent_controller, iVar2);
  }
LAB_02190989:
  iVar2 = generic_activated_ability(player, card, event, 1, 2, 0, 2, 0, 0, 0, 0, NULL, NULL);
  return iVar2;
}

/* ManalinkEh 2022: 0x2190ab0; original cards/rivals_of_ixalan.c:626. */
int card_warkite_marauder(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  test_definition_t hc;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = 1 - current_turn;
    td.allow_cancel = 0;
    result = can_target(&td);
    if (result != 0)
    {
      result = pick_target(&td, "TARGET_CREATURE_OPPONENT_CONTROLS");
      if (result != 0)
      {
        default_test_definition(&hc, 2);
        hc.power = 0;
        hc.toughness = 1;
        instance = get_card_instance(player, card);
        result = instance->targets[0].card;
        instance = get_card_instance(player, card);
        humiliate_and_set_pt_abilities(player, card, instance->targets[0].player, result, 4, &hc);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2190db0; original cards/rivals_of_ixalan.c:742. */
int card_golden_demise(int player, int card, event_t event)
{
  int result;
  int t_player;
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
    else if (event == EVENT_RESOLVE_SPELL)
    {

      ascend(player, card, EVENT_RESOLVE_SPELL);
      default_test_definition(&test, 2);
      result = has_the_citys_blessing(player);
      t_player = 2;
      if (result != 0)
      {
        t_player = 1 - player;
      }
      pump_creatures_until_eot(player, card, t_player, 0, -2, -2, 0, 0, NULL);
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

/* ManalinkEh 2022: 0x21917e0; original cards/rivals_of_ixalan.c:1068. */
int card_vonas_hunger(int player, int card, event_t event)
{
  int result;
  unsigned int uVar2;
  test_definition_t test;

  ascend(player, card, event);
  if (event != EVENT_RESOLVE_SPELL)
    goto LAB_02191805;
  result = has_the_citys_blessing(player);
  if (result == 0)
  {
    result = count_permanents_by_type(player, TYPE_PERMANENT);
    if (9 < result)
      goto LAB_0219183c;
    new_default_test_definition(&test, 2, "SACRIFICE_CREATURE");
  }
  else
  {
  LAB_0219183c:

    uVar2 = count_subtype(1 - player, 2, -1);
    uVar2 = round_up_value(uVar2);
    new_default_test_definition(&test, 2, "SACRIFICE_CREATURE");
    test.qty = (unsigned short)uVar2;
  }
  new_sacrifice(player, card, 1 - player, SAC_CAUSED | SAC_NO_CANCEL, &test);
  kill_card(player, card, KILL_DESTROY);
LAB_02191805:
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2191d00; original cards/rivals_of_ixalan.c:1226. */
int card_etali_primal_storm(int player, int card, event_t event)
{
  unsigned short uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iStack_f4;
  int exiled[2];
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {

    exiled[0] = -1;
    exiled[1] = -1;
    iVar4 = 0;
    iStack_f4 = 0;
    do
    {

      iVar2 = *deck_ptr[iVar4];
      if (iVar2 != -1)
      {
        rfg_top_card_of_deck(iVar4);
      }
      if (((cards_data[iVar2].type & 0xfe) != 0) &&
          (iVar3 = can_legally_play_iid(player, iVar2), iVar3 != 0))
      {
        exiled[iVar4] = iVar2;
        iStack_f4 = iStack_f4 + 1;
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 != 2);
    if (0 < iStack_f4)
    {
      do
      {
        new_default_test_definition(&this_test, 0xff, "Select a card to play for free");
        iVar4 = select_card_from_zone(player, player, exiled, iStack_f4, 0, 1, -1, &this_test);
        if (iVar4 == -1)
        {
          return 0;
        }

        uVar1 = cards_data[exiled[iVar4]].id;
        if (iVar4 < iStack_f4)
        {
          iVar2 = iVar4 + 1;
          exiled[iVar4] = exiled[iVar2];
          if (iVar2 < iStack_f4)
          {
            exiled[iVar2] = *(int *)(this_test.message + iVar4 * 4);
          }
        }
        iStack_f4 = iStack_f4 + -1;
        play_card_in_exile_for_free(player, iVar4, (unsigned int)uVar1);
      } while (iStack_f4 != 0);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21921e0; original cards/rivals_of_ixalan.c:1389. */
int card_silverclad_ferocidons(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  result = is_humiliated_for_ability(player, card, AL_TRIG);
  if (result == 0)
  {
    result = damage_dealt_to_me_arbitrary(player, card, event, DDBM_0, player, card);
    if (result != 0)
    {
      new_default_test_definition(&test, 0x47, "Select a permanent to sacrifice");
      new_sacrifice(player, card, 1 - player, SAC_CAUSED | SAC_NO_CANCEL, &test);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2192730; original cards/rivals_of_ixalan.c:1642. */
int card_polyraptor(int player, int card, event_t event)
{
  int result;

  result = is_humiliated_for_ability(player, card, AL_TRIG);
  if (result == 0)
  {
    result = damage_dealt_to_me_arbitrary(player, card, event, DDBM_0, player, card);
    if (result != 0)
    {
      copy_token(player, card, player, card);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21927b0; original cards/rivals_of_ixalan.c:1662. */
int card_tendershoot_dryad(int player, int card, event_t event)
{
  int result;

  ascend(player, card, event);
  upkeep_trigger_ability(player, card, event, 2);
  if (event != EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    if (event - EVENT_POWER < 2)
    {
      result = has_the_citys_blessing(player);
      if (result != 0)
      {
        boost_creature_type(player, card, event, SUBTYPE_SAPROLING, 2, 2, 0, BCT_CONTROLLER_ONLY);
      }
    }
    return 0;
  }
  generate_token_by_id(player, card, 0x497d);
  return 0;
}

/* ManalinkEh 2022: 0x21929d0; original cards/rivals_of_ixalan.c:1709. */
int card_wayward_swordtooth(int player, int card, event_t event)
{
  int result;

  check_playable_lands(player);
  ascend(player, card, event);
  if (event == EVENT_ATTACK_LEGALITY)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = has_the_citys_blessing(player);
      if (result == 0)
      {
        result = is_humiliated_for_ability(player, card, AL_STATIC);
        if (result == 0)
        {
          event_result = 1;
        }
      }
    }
  }
  else if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    result = has_the_citys_blessing(player);
    if (result == 0)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        cannot_block(player, card, EVENT_ABILITIES);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2192ab0; original cards/rivals_of_ixalan.c:1733. */
int card_world_shaper(int player, int card, event_t event)
{
  int result;

  result = duh_mode(player);
  dat_ability(player, card, event, (result != 0) + DAT_RESOLVE_TRIGGER_OPTIONAL, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    mill(player, 3);
  }
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  reanimate_all(player, card, player, 1, 0, 0, 0, 0, 0, 0, 0, -1, 0, REANIMATE_TAP);
  return 0;
}

/* ManalinkEh 2022: 0x2192bc0; original cards/rivals_of_ixalan.c:1752. */
int card_angrath_the_flame_chained(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t *ptStack_68;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = can_target(&td);
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017",
                           "Opponent discards & loses life", 1, 5, 1, "Steal a creature", result,
                           10, 0xfffffffd, "Opponent loses life from graveyard cards", 1, 0x14,
                           0xfffffff8, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (result == 2)
      {
        new_pick_target(&td, "TARGET_CREATURE", 0, 1);
      }
      goto LAB_02192c32;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_02192c32;
    default_target_definition(player, card, &td, 2);
    instance = get_card_instance(player, card);
  }
  ptStack_68 = &td;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    result = instance->info_slot;
    if (result == 1)
    {
      discard(1 - player, 0, player);
      lose_life_2022(1 - player, 2);
    }
    else if (result == 2)
    {
      result = validate_target(player, card, ptStack_68, 0);
      if (result != 0)
      {
        effect_act_of_treason(player, card, instance->targets[0].player, instance->targets[0].card);
        result = get_cmc(instance->targets[0].player, instance->targets[0].card);
        if (result < 4)
        {
          haste_and_remove_at_beginning_of_next_end_step(player, card, instance->targets[0].player,
                                                         instance->targets[0].card, 3);
        }
      }
    }
    else if (result == 3)
    {
      result = count_graveyard(1 - player);
      lose_life_2022(1 - player, result);
    }
  }
LAB_02192c32:
  result = planeswalker(player, card, event, 4);
  return result;
}

/* ManalinkEh 2022: 0x2192ea0; original cards/rivals_of_ixalan.c:1816. */
int card_atzocan_seer(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  event_t event_00;
  const char *prompt;
  test_definition_t test;

  if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      result = mana_producing_creature(player, card, event, 0x18, COLOR_GREEN, 1);
      return result;
    }
    instance = get_card_instance(player, card);
    new_default_test_definition(&test, 2, "Select target Dinosaur creature card.");
  LAB_02192f08:
    test.subtype = 0xf0;

    if (((event == EVENT_RESOLVE_ACTIVATION) && (instance->info_slot == 2)) &&
        (result = validate_target_from_grave_source(player, card, player, 0), result != -1))
    {
      from_grave_to_hand(player, result, 1);
    }
    return 0;
  }
  instance = get_card_instance(player, card);
  new_default_test_definition(&test, 2, "Select target Dinosaur creature card.");
  test.subtype = 0xf0;
  if (event == EVENT_CAN_ACTIVATE)
  {
    result = mana_producing_creature(player, card, EVENT_CAN_ACTIVATE, 0x18, COLOR_GREEN, 1);
    if (result != 0)
    {
      return 1;
    }
    prompt = NULL;
    event_00 = EVENT_CAN_ACTIVATE;
  }
  else
  {
    if (event != EVENT_ACTIVATE)
      goto LAB_02192f08;
    result = paying_mana();
    if ((result != 0) ||
        (result = generic_activated_ability_with_target_in_grave(
             player, card, EVENT_CAN_ACTIVATE, GAA_SACRIFICE_ME, 0, 0, 0, 0, 0, 0, 0, NULL,
             "Select target Dinosaur creature card.", TYPE_CREATURE, &test, 8),
         result == 0))
    {
    LAB_02193082:
      result = mana_producer(player, card, EVENT_ACTIVATE);
      return result;
    }
    if (player != 1)
    {
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Get mana", 1, 1,
                           "Return a dino", 1, 1, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (result == 1)
        goto LAB_02193082;
      if (result != 2)
      {
        return 0;
      }
    }
    prompt = "Select target Dinosaur creature card.";
    event_00 = EVENT_ACTIVATE;
  }

  result = generic_activated_ability_with_target_in_grave(player, card, event_00, GAA_SACRIFICE_ME,
                                                          0, 0, 0, 0, 0, 0, 0, NULL, prompt,
                                                          TYPE_NONE, &test, 8);
  return result;
}

/* ManalinkEh 2022: 0x2193290; original cards/rivals_of_ixalan.c:1888. */
int card_azor_the_lawbringer(int player, int card, event_t event)
{
  xtrigger_t xVar1;
  declare_attackers_trigger_t mode;
  card_instance_t *instance;
  int iVar3;
  int iVar4;

  check_legend_rule(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    if (0 < active_cards_count[player])
    {
      iVar4 = 0;
      do
      {
        while ((instance = in_play(player, iVar4),
                instance == NULL || (iVar3 = is_what(player, iVar4, 0x80), iVar3 == 0)))
        {
          iVar4 = iVar4 + 1;
          if (active_cards_count[player] <= iVar4)
            goto LAB_02193410;
        }

        instance = get_card_instance(player, iVar4);
        if (instance->info_slot_fn == azor_the_lawbringer_legacy)
          goto LAB_021932d9;
        iVar4 = iVar4 + 1;
      } while (iVar4 < active_cards_count[player]);
    }
  LAB_02193410:

    iVar4 = create_legacy_effect(player, card, azor_the_lawbringer_legacy);
    add_status(player, iVar4, 0x10000);
  }
LAB_021932d9:
  store_attackers(player, card, event, 0, player, card, NULL);
  xVar1 = trigger_condition;
  if (trigger_condition == 0xe5)
  {
    xVar1 = xtrigger_impl_value_dont_use_directly;
  }
  if (((event != EVENT_DECLARE_ATTACKERS) && (xVar1 != XTRIGGER_ATTACKING)) ||
      (iVar4 = has_mana_multi(player, 0, 0, 2, 0, 0, 1), iVar4 == 0))
    goto LAB_02193321;

  if ((player == 0) && (ai_is_speculating != 1))
  {
    iVar4 = duh_mode(0);
    if (iVar4 == 0)
    {
      mode = DAT_RESOLVE_TRIGGER_OPTIONAL;
    }
    else
    {
    LAB_02193500:
      mode = DAT_RESOLVE_TRIGGER_MANDATORY;
    }
  }
  else
  {
    iVar4 = has_mana_multi(player, 1, 0, 2, 0, 0, 1);
    mode = 0;
    if (iVar4 != 0)
      goto LAB_02193500;
  }
  dat_ability(player, card, event, mode, 1);
LAB_02193321:
  if ((event == EVENT_DAT_ABILITY) &&
      (iVar4 = charge_mana_multi_while_resolving(player, card, EVENT_RESOLVE_TRIGGER, player, -1, 0,
                                                 2, 0, 0, 1),
       iVar4 != 0))
  {
    gain_life(player, x_value);
    draw_cards(player, x_value);
  }
  return 0;
}

/* ManalinkEh 2022: 0x21937f0; original cards/rivals_of_ixalan.c:2016. */
int card_winged_temple_of_orazca(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  target_definition_t *ptStack_68;
  target_definition_t td;

  double_faced_card(player, card, event);
  check_legend_rule(player, card, event);
  if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      default_target_definition(player, card, &td, 2);
      td.allowed_controller = player;
      get_card_instance(player, card);

      iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, 1, 0, 1, 1, 0, 0, 0,
                                        &td, NULL);
      if ((iVar1 != 0) && (iVar2 = paying_mana(), iVar2 == 0))
      {
        iVar1 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Get mana", 1, 1,
                            "Pump target creature", iVar1, 5, 0, 0, 0, 0);
        if (iVar1 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        if (iVar1 != 1)
        {
          if (iVar1 != 2)
          {
            return 0;
          }
          add_state(player, card, 0x10);
          generic_activated_ability(player, card, EVENT_ACTIVATE, 5, 1, 0, 1, 1, 0, 0, 0, &td,
                                    "TARGET_CREATURE_YOU_CONTROL");
          if (spell_fizzled != 1)
          {
            return 0;
          }
          remove_state(player, card, 0x10);
          return 0;
        }
      }
      iVar1 = mana_producer(player, card, EVENT_ACTIVATE);
      return iVar1;
    }
    if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      default_target_definition(player, card, &td, 2);
      td.allowed_controller = player;
      instance = get_card_instance(player, card);
      ptStack_68 = &td;
      if ((event == EVENT_RESOLVE_ACTIVATION) &&
          ((instance->info_slot == 2 &&
            (iVar1 = validate_target(player, card, ptStack_68, 0), iVar1 != 0))))
      {

        iVar1 = get_power(instance->targets[0].player, instance->targets[0].card);
        pump_ability_until_eot_2022(player, card, instance->targets[0].player, instance->targets[0].card,
                               iVar1, iVar1, 0x20, 0);
        return 0;
      }
      return 0;
    }
  }
  iVar1 = mana_producer(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x2194220; original cards/rivals_of_ixalan.c:2232. */
int card_atzal_cave_of_eternity(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  test_definition_t test;

  double_faced_card(player, card, event);
  check_legend_rule(player, card, event);
  if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      new_default_test_definition(&test, 2, "Select target creature card.");
      get_card_instance(player, card);

      iVar1 = generic_activated_ability_with_target_in_grave(player, card, EVENT_CAN_ACTIVATE,
                                                             GAA_UNTAPPED, 3, 1, 0, 1, 0, 0, 0,
                                                             NULL, NULL, TYPE_NONE, &test, 8);
      if ((iVar1 != 0) && (iVar2 = paying_mana(), iVar2 == 0))
      {
        iVar1 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Get mana", 1, 1,
                            "Reanimate a creature", iVar1, 5, 0, 0, 0, 0);
        if (iVar1 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        if (iVar1 != 1)
        {
          if (iVar1 != 2)
          {
            return 0;
          }
          add_state(player, card, 0x10);
          generic_activated_ability_with_target_in_grave(player, card, EVENT_ACTIVATE, GAA_UNTAPPED,
                                                         3, 1, 0, 1, 0, 0, 0, NULL, NULL, TYPE_NONE,
                                                         &test, 8);
          if (spell_fizzled != 1)
          {
            return 0;
          }
          remove_state(player, card, 0x10);
          return 0;
        }
      }
      iVar1 = mana_producer(player, card, EVENT_ACTIVATE);
      return iVar1;
    }
    if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      new_default_test_definition(&test, 2, "Select target creature card.");
      instance = get_card_instance(player, card);

      if ((event == EVENT_RESOLVE_ACTIVATION) &&
          ((instance->info_slot == 2 &&
            (iVar1 = validate_target_from_grave_source(player, card, player, 0), iVar1 != -1))))
      {
        reanimate_permanent(player, card, player, iVar1, REANIMATE_DEFAULT);
        return 0;
      }
      return 0;
    }
  }
  iVar1 = mana_producer(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x2194cc0; original cards/rivals_of_ixalan.c:2435. */
int card_path_of_mettle(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t etb_test;

  double_faced_card(player, card, event);
  check_legend_rule(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_test_definition(&etb_test, 2);
    etb_test.special_selection_function = not_fs_ds_v_or_h;
    new_damage_all(player, card, 2, 1, 0, &etb_test);
    dat_ability(player, card, EVENT_ETB, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  }
  else if (event == EVENT_DECLARE_ATTACKERS)
  {
    default_test_definition(&etb_test, 2);
    etb_test.special_selection_function = has_fs_ds_v_or_h;
    store_attackers(player, card, EVENT_DECLARE_ATTACKERS, DAT_TRACK, player, -1, &etb_test);
    dat_ability(player, card, EVENT_DECLARE_ATTACKERS, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  }
  else
  {
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
    if (event == EVENT_DAT_ABILITY)
    {
      instance = get_card_instance(player, card);
      if (1 < instance->info_slot)
      {
        transform(player, card);
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2195590; original cards/rivals_of_ixalan.c:2572. */
int card_tomb_of_the_dusk_rose(int player, int card, event_t event)
{
  unsigned int uVar1;
  int result;
  card_instance_t *pcVar3;
  int *piVar4;
  int32_t *piVar5;
  card_instance_t *pcVar6;
  int card_00;
  bool bVar7;

  double_faced_card(player, card, event);
  check_legend_rule(player, card, event);
  if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      pcVar3 = get_card_instance(player, card);
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 3, 1, 0, 0, 0, 1, 0,
                                         NULL, NULL);
      if (((result != 0) &&
           (result = exiledby_choose(player, card, 0x4759, EXBY_FIRST_FOUND, 0, "Card", 1),
            result != 0)) &&
          (result = paying_mana(), result == 0))
      {
        result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Get mana", 1, 1,
                             "Return a creature", 1, 5, 0, 0, 0, 0);
        if (result == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        if (result != 1)
        {
          if (result != 2)
          {
            return 0;
          }

          add_state(player, card, 0x10);
          generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 2, 1, 0, 0, 0, 1, 0, NULL,
                                    NULL);
          piVar5 = (int32_t *)exiledby_choose(player, card, 0x4759, EXBY_CHOOSE, 0, "creature", 1);
          if (piVar5 == NULL)
          {
            spell_fizzled = 1;
          }
          else
          {
            pcVar3->targets[1].player = -1;
            bVar7 = spell_fizzled != 1;
            pcVar3->targets[1].card = *piVar5;
            if (bVar7)
            {
              return 0;
            }
          }
          remove_state(player, card, 0x10);
          return 0;
        }
      }
      event = EVENT_ACTIVATE;
    }
    else if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      pcVar3 = get_card_instance(player, card);
      if (event != EVENT_RESOLVE_ACTIVATION)
      {
        return 0;
      }
      if (pcVar3->info_slot == 2)
      {

        piVar4 = exiledby_find(player, pcVar3->parent_card, pcVar3->targets[1].card, NULL, NULL);
        if (piVar4 != NULL)
        {

          *piVar4 = -1;
          uVar1 = pcVar3->targets[1].card;
          result = remove_card_from_rfg(
              uVar1 >> 0x1f,
              (unsigned int)*(unsigned short *)(cards_data[0].cc +
                                                ((uVar1 & 0x7fffffff) + uVar1 * 8) * 8 + -7));
          if (result != 0)
          {

            result = add_card_to_hand(player, uVar1 & 0x7fffffff);
            if (player != uVar1 >> 0x1f)
            {
              pcVar6 = get_card_instance(player, result);
              pcVar6->state = pcVar6->state ^ 0x1000;
            }
            card_00 =
                create_targetted_legacy_effect(player, pcVar3->parent_card, empty, player, result);
            add_status(player, card_00, 0x10000);
            put_into_play(player, result);
            return 0;
          }
        }
        spell_fizzled = 1;
        return 0;
      }
      return 0;
    }
  }

  result = mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2195990; original cards/rivals_of_ixalan.c:2661. */
int card_raging_regisaur(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    instance->number_of_targets = '\0';
    result = pick_target(&td, "TARGET_ANY");
    if (result != 0)
    {
      damage_target0(player, card, 1);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2195b10; original cards/rivals_of_ixalan.c:2718. */
int card_storm_the_vault(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  double_faced_card(player, card, event);
  check_legend_rule(player, card, event);
  instance = combat_damage_being_dealt(event);
  if (((instance != NULL) && (instance->damage_source_player == player)) &&
      (instance->damage_target_card == -1))
  {
    result = check_special_flags_2022(player, instance->damage_source_card, 0x40);
    if ((result == 0) && ((instance->targets[3].player & 2) != 0))
    {
      instance = get_card_instance(player, card);
      instance->targets[8].player = 1;
    }
  }
  result = resolve_damage_trigger(player, card, event, DDBM_0, player, card);
  if (result != 0)
  {
    generate_token_by_id(player, card, 0x4998);
  }
  if (((current_turn == player) && (event == EVENT_PHASE_CHANGED)) && (current_phase == 0x22))
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result == 0)
    {
      result = count_permanents_by_type(player, TYPE_ARTIFACT);
      if (4 < result)
      {
        transform(player, card);
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2195ed0; original cards/rivals_of_ixalan.c:2809. */
int card_zacama_primal_calamity(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  int32_t iVar5;
  target_definition_t *td;
  const char *prompt;
  target_definition_t td1;
  target_definition_t td2;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  vigilance(player, card, event);
  if (event == EVENT_ETB)
  {
    iVar2 = check_special_flags_2022(player, card, 0x40000);
    if (iVar2 == 0)
    {
      manipulate_type(player, card, player, TYPE_LAND, ACT_UNTAP);
    }
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      default_target_definition(player, card, &td1, 2);
      default_target_definition(player, card, &td2, 0x44);
      instance = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {
        iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 2, 0, 0, 0, 1, 0, 0,
                                          &td1, "TARGET_CREATURE");
        if ((iVar2 == 0) &&
            (iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 2, 0, 0, 1, 0,
                                               0, 0, &td2, "TARGET_ARTIFACT_ENCHANTMENT"),
             iVar2 == 0))
        {
          iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 2, 0, 0, 0, 0, 1,
                                            0, NULL, NULL);
          return iVar2;
        }
        return 1;
      }
      if (event == EVENT_ACTIVATE)
      {

        iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 2, 0, 0, 0, 1, 0, 0,
                                          &td1, "TARGET_CREATURE");
        iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 2, 0, 0, 1, 0, 0, 0,
                                          &td2, "TARGET_ARTIFACT_ENCHANTMENT");
        iVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 2, 0, 0, 0, 0, 1, 0,
                                          NULL, NULL);
        iVar2 = dialog_impl(
            player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "Damage creature", iVar2, 10,
            "Destroy artifact or enchantment", iVar3, 10, "Gain Life", iVar4,
            0xfU - life[player] & (int)(0xfU - life[player] ^ 0xffffffff) >> 0x1f, 0, 0, 0, 0);
        if (iVar2 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        if (iVar2 == 1)
        {
          td = &td1;
          prompt = "TARGET_CREATURE";
          iVar3 = 1;
          iVar2 = 0;
        }
        else
        {
          if (iVar2 != 2)
          {
            if (iVar2 != 3)
            {
              return 0;
            }
            iVar2 = generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 2, 0, 0, 0, 0, 1, 0,
                                              NULL, NULL);
            return iVar2;
          }
          td = &td2;
          prompt = "TARGET_ARTIFACT_ENCHANTMENT";
          iVar3 = 0;
          iVar2 = 1;
        }
        iVar2 = generic_activated_ability(player, card, EVENT_ACTIVATE, 4, 2, 0, 0, iVar2, iVar3, 0,
                                          0, td, prompt);
        return iVar2;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      {
        return 0;
      }
      default_target_definition(player, card, &td1, 2);
      default_target_definition(player, card, &td2, 0x44);
      instance = get_card_instance(player, card);
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      iVar5 = instance->info_slot;
      if (iVar5 == 1)
      {
        damage_target0(player, card, 3);
        iVar5 = instance->info_slot;
      }
      if (iVar5 == 2)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
        iVar5 = instance->info_slot;
      }
      if (iVar5 == 3)
      {
        gain_life(player, 3);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2196530; original cards/rivals_of_ixalan.c:2900. */
int card_azors_gateway(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int leg;
  int idx;
  char buffer[100];
  test_definition_t this_test;

  double_faced_card(player, card, event);
  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      if ((player == 0) &&
          (iVar2 = exiledby_choose(0, card, 0x46f0, EXBY_FIRST_FOUND, 0, "Card", 1), iVar2 != 0))
      {
        return 1;
      }
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 1, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      return iVar2;
    }
    if (event == EVENT_ACTIVATE)
    {
      iVar2 = exiledby_choose(player, card, 0x46f0, EXBY_FIRST_FOUND, 0, "Card", 1);
      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 1, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Draw and exile", iVar3, 5,
                          "Show exiled cards", (unsigned int)(iVar2 != 0 && player == 0), 0, 0, 0,
                          0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 != 2)
      {
        if (iVar2 != 1)
        {
          return 0;
        }
        iVar2 = generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 1, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        return iVar2;
      }
      exiledby_choose(player, card, 0x46f0, EXBY_CHOOSE, 0, "Card", 1);
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
    instance = get_card_instance(player, card);
  }
  if ((event == EVENT_RESOLVE_ACTIVATION) && (instance->info_slot == 1))
  {
    draw_cards(player, 1);
    if (0 < hand_count[player])
    {

      leg = 0;
      idx = 0;
      scnprintf(buffer, 100, "Select a card to exile");
      new_default_test_definition(&this_test, 0xff, buffer);
      iVar2 = new_select_a_card(player, player, 4, 1, 3, -1, &this_test);
      iVar3 = get_original_internal_card_id(player, iVar2);
      exiledby_remember(player, instance->parent_card, player, iVar3, &leg, &idx);
      rfg_card_in_hand(player, iVar2);
      exile_card_and_remember_it_on_exiledby(player, card, player, iVar2);
    }
    iVar2 = exiledby_count_cmcs(player, instance->parent_card, 2);
    if (4 < iVar2)
    {
      gain_life_2022(player, 5);
      untap_card(player, instance->parent_card);
      transform(player, instance->parent_card);
      return 0;
    }
  }

  return 0;
}

/* ManalinkEh 2022: 0x2196eb0; original cards/rivals_of_ixalan.c:3110. */
int card_gold_forge_garrison(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  token_generation_t token;

  double_faced_card(player, card, event);
  if (event == EVENT_COUNT_MANA)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (iVar3 = mana_producer(player, card, EVENT_CAN_ACTIVATE), iVar3 != 0))
    {
      declare_mana_available_maybe_hex(player, COLOR_TEST_ANY_COLORED, 2);
    }
    get_card_instance(player, card);
    return 0;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
    iVar3 = mana_producer(player, card, EVENT_CAN_ACTIVATE);
  }
  else
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 6, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      if ((iVar3 != 0) && (iVar2 = paying_mana(), iVar2 == 0))
      {
        iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Get mana", 1, 1,
                            "Create a token", iVar3, 5, 0, 0, 0, 0);
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
          add_state(player, card, 0x10);
          generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 4, 0, 0, 0, 0, 0, 0, NULL,
                                    NULL);
          if (spell_fizzled != 1)
          {
            return 0;
          }
          remove_state(player, card, 0x10);
          return 0;
        }
      }
      instance->info_slot = 0;
      iVar3 = produce_mana_tapped_all_one_color_2022(player, card, COLOR_TEST_ANY_COLORED, 2);
      return iVar3;
    }
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      return 0;
    }
    if (instance->info_slot != 2)
    {
      return 0;
    }
    default_token_definition(player, card, 0x4940, &token);
    token.tou = 4;
    token.pow = 4;
    generate_token(&token);
    iVar3 = 0;
    instance->info_slot = 0;
  }
  return iVar3;
}

/* ManalinkEh 2022: 0x21971d0; original cards/rivals_of_ixalan.c:3178. */
int card_the_immortal_sun(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_ABILITIES)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      result = is_what(affected_card_controller, affected_card, 0x100);
      if (result != 0)
      {
        instance = get_card_instance(affected_card_controller, affected_card);
        instance->targets[0x11].card = instance->targets[0x11].card | 0x20;
      }
    }
  }
  else if (event == EVENT_DRAW_PHASE)
  {
    if (current_turn == player)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        event_result = event_result + 1;
      }
    }
  }
  else if ((event == EVENT_MODIFY_COST_GLOBAL) && (affected_card_controller == player))
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      COST_COLORLESS = COST_COLORLESS + -1;
    }
  }
  boost_creature_type(player, card, event, ~SUBTYPE_INVALID, 1, 1, 0, BCT_CONTROLLER_ONLY);
  return 0;
}

/* ManalinkEh 2022: 0x2197320; original cards/rivals_of_ixalan.c:3206. */
int card_silent_gravestone(int player, int card, event_t event)
{
  int result;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;

  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    rfg_whole_graveyard(current_turn);
    rfg_whole_graveyard(current_turn ^ 1);
    draw_a_card(player);
    pcVar2 = get_card_instance(player, card);
    pcVar3 = in_play(pcVar2->parent_controller, pcVar2->parent_card);
    if (pcVar3 != NULL)
    {
      kill_card(pcVar2->parent_controller, pcVar2->parent_card, KILL_EXILE);
    }
  }
  result = generic_activated_ability(player, card, event, 1, 4, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2197420; original cards/rivals_of_ixalan.c:3274. */
int card_angrath_minotaur_pirate(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iStack_270;
  target_definition_t *ptStack_26c;
  target_definition_t td;
  target_definition_t td_player;
  test_definition_t test;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0x100);
    td.zone = 0x1200;
    default_target_definition(player, card, &td_player, 2);
    td_player.zone = 0x1000;
    new_default_test_definition(&test, 0x47, "Select target Pirate card.");
    test.subtype = 0x95;
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      iVar2 = generic_activated_ability_with_target_in_grave(player, card, EVENT_CAN_ACTIVATE,
                                                             GAA_NONE, 0, 0, 0, 0, 0, 0, 0, NULL,
                                                             NULL, TYPE_NONE, &test, 8);
      if ((player == 1) &&
          (iVar4 = count_counters(1, card, COUNTER_LOYALTY), iVar3 = current_turn, 1 < iVar4))
      {

        iStack_270 = 1;
        iVar4 = active_cards_count[current_turn] + -1;
        if (-1 < iVar4)
        {
          do
          {
            if (((iVar3 != 1) && (instance = in_play(iVar3, iVar4), instance != NULL)) &&
                (iVar5 = is_what(iVar3, iVar4, 2), iVar5 != 0))
            {
              iVar5 = get_toughness(iVar3, iVar4);
              instance = get_card_instance(iVar3, iVar4);
              if (iVar5 - (unsigned int)instance->damage_on_card == 1)
              {
                iStack_270 = iStack_270 + 3;
              }
            }
            iVar4 = iVar4 + -1;
          } while (iVar4 != -1);
        }
        iVar3 = 1 - current_turn;
        iVar4 = active_cards_count[iVar3] + -1;
        if (-1 < iVar4)
        {
          do
          {
            if (((iVar3 != 1) && (instance = in_play(iVar3, iVar4), instance != NULL)) &&
                (iVar5 = is_what(iVar3, iVar4, 2), iVar5 != 0))
            {
              iVar5 = get_toughness(iVar3, iVar4);
              instance = get_card_instance(iVar3, iVar4);
              if (iVar5 - (unsigned int)instance->damage_on_card == 1)
              {
                iStack_270 = iStack_270 + 3;
              }
            }
            iVar4 = iVar4 + -1;
          } while (iVar4 != -1);
        }
      }
      else
      {
        iStack_270 = 1;
      }
      iVar3 = opponent_is_valid_target(player, card);
      iVar4 = can_target(&td);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017",
                          "1 damage to opp and opp creatures", iVar4, iStack_270, 2,
                          "Resurrect a Pirate", iVar2, 6, 0xfffffffd, "Wipe creatures & damage",
                          iVar3, 0xf, 0xfffffff5, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 == 1)
      {
        new_pick_target(&td, "TARGET_OPPONENT_OR_PLANESWALKER", 0, 1);
      }
      else if (iVar2 == 2)
      {
        generic_activated_ability_with_target_in_grave(player, card, EVENT_ACTIVATE, GAA_NONE, 0, 0,
                                                       0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test,
                                                       8);
      }
      else if (iVar2 == 3)
      {
        target_opponent(player, card);
      }
      goto LAB_02197502;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_02197502;
    default_target_definition(player, card, &td, 0x100);
    td.zone = 0x1200;
    default_target_definition(player, card, &td_player, 2);
    td_player.zone = 0x1000;
    new_default_test_definition(&test, 0x47, "Select target Pirate card.");
    test.subtype = 0x95;
    instance = get_card_instance(player, card);
  }
  ptStack_26c = &td;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    iVar2 = instance->info_slot;
    if (iVar2 == 1)
    {
      iVar2 = validate_target(player, card, ptStack_26c, 0);
      if (iVar2 != 0)
      {
        damage_target0(player, card, 1);
        new_damage_all(player, card, instance->targets[0].player, 1, 1, NULL);
      }
    }
    else if (iVar2 == 2)
    {

      iVar2 = validate_target_from_grave_source(player, card, player, 0);
      if (iVar2 != -1)
      {
        reanimate_permanent(player, card, player, iVar2, REANIMATE_DEFAULT);
      }
    }
    else if ((iVar2 == 3) && (iVar2 = validate_target(player, card, &td_player, 0), iVar2 != 0))
    {

      iVar2 = get_power_sum(instance->targets[0].player);
      default_test_definition(&this_test, 2);
      new_manipulate_all(player, card, 1 - player, &this_test, ACT_KILL_DESTROY);
      damage_target0(player, card, iVar2);
    }
  }
LAB_02197502:
  iVar2 = planeswalker(player, card, event, 5);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int pitiless_plunderer_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x218ee10; original cards/rivals_of_ixalan.c:850. */
static int pitiless_plunderer_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int iVar3;
  int iVar4;
  token_generation_t token;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (iVar3 = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           iVar3 != 0x4756))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (iVar3 = effect_follows_control_of_attachment(player, card, 0x4700), iVar3 != 0))
    {
      return 0;
    }
  LAB_0218ef38:
    iVar3 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (iVar3 == 0)
      goto LAB_0218ef56;
  }
  else
  {
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (iVar3 = effect_follows_control_of_attachment(player, card, event), iVar3 != 0))
    {
      return 0;
    }
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_0218ef38;

    pcVar1 = get_card_instance(player, card);
    pcVar2 = in_play(affected_card_controller, affected_card);
    if ((pcVar2 != NULL) &&
        (iVar3 = is_this_dying(affected_card_controller, affected_card), iVar3 != 0))
    {

      if ((pcVar1->damage_target_card == affected_card) &&
          (pcVar1->damage_target_player == affected_card_controller))
      {
        pcVar1->damage_target_card = -1;
        pcVar1->damage_target_player = -1;
        remove_status(player, card, 0x10000);
        if (pcVar1->targets[0xb].player < 0)
        {
          pcVar1->targets[0xb].player = 0;
        }
        *(uint8_t *)((int)&pcVar1->targets[0xb].player + 2) = 1;
        return 0;
      }
      count_for_gfp_ability(player, card, 0x77, player, 2, NULL);
    }
    iVar3 = resolve_gfp_ability_2022(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (iVar3 == 0)
    {
      return 0;
    }
  }
  iVar4 = check_status(player, card, 0x10000);
  if (iVar4 == 0)
  {

    default_token_definition(player, card, 0x4998, &token);
    token.qty = (unsigned char)iVar3;
    generate_token(&token);
    kill_card(player, card, KILL_EXILE);
  }
LAB_0218ef56:
  if ((event == EVENT_CLEANUP) && (iVar3 = check_status(player, card, 0x10000), iVar3 == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x21911b0; original cards/rivals_of_ixalan.c:902. */
int card_pitiless_plunderer(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_this_test;

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
                instance->info_slot_fn == pitiless_plunderer_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_021911d5;
          iVar1 = iVar1 + 1;
        } while (iVar1 < active_cards_count[player]);
      }

      iVar1 = create_targetted_legacy_effect(player, card, pitiless_plunderer_effect, player, card);
      instance = get_card_instance(player, iVar1);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar1, 0x10000);
    }
  }
  else if ((event == EVENT_GRAVEYARD_FROM_PLAY) &&
           (instance = in_play(player, card), instance != NULL))
  {
    default_test_definition(&recovered_stack_this_test.test, 2);
    recovered_stack_this_test.test.not_me = 1;
    count_for_gfp_ability(player, card, 0x77, player, 0, &recovered_stack_this_test.test);
  }
LAB_021911d5:
  iVar1 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (iVar1 != 0)
  {

    default_token_definition(player, card, 0x4998, &recovered_stack_this_test.token);
    recovered_stack_this_test.test.message[0x2b] = (char)iVar1;
    generate_token(&recovered_stack_this_test.token);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2194550; original cards/rivals_of_ixalan.c:2301. */
int card_kumena_tyrant_of_orazca(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  uint32_t uVar5;
  target_definition_t *td_subtype;
  int32_t iVar6;
  int iVar7;
  target_definition_t td;
  target_definition_t td1;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0x47);
    td.allowed_controller = player;
    td.preferred_controller = player;
    td.required_subtype = 0x74;
    td.illegal_abilities = 0;
    td.illegal_state = 1;
    td.special = 0x4000;
    default_target_definition(player, card, &td1, 0x47);
    td1.allowed_controller = player;
    td1.preferred_controller = player;
    td1.required_subtype = 0x74;
    td1.illegal_abilities = 0;
    td1.illegal_state = 1;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar2 = generic_activated_ability_with_tapsubtype_as_cost(
          player, card, EVENT_CAN_ACTIVATE, GAA_NONE, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, &td1, 3);
      if (iVar2 != 0)
      {
        return 1;
      }
      iVar2 = generic_activated_ability_with_tapsubtype_as_cost(
          player, card, EVENT_CAN_ACTIVATE, GAA_NONE, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, &td, 1);
      return iVar2;
    }
    if (event == EVENT_ACTIVATE)
    {

      iVar2 = generic_activated_ability_with_tapsubtype_as_cost(
          player, card, EVENT_CAN_ACTIVATE, GAA_NONE, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, &td, 1);
      iVar3 = generic_activated_ability_with_tapsubtype_as_cost(
          player, card, EVENT_CAN_ACTIVATE, GAA_NONE, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, &td1, 3);
      iVar4 = generic_activated_ability_with_tapsubtype_as_cost(
          player, card, EVENT_CAN_ACTIVATE, GAA_NONE, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, &td1, 5);
      if (((current_turn == player) && (current_phase < 0x17)) &&
          (iVar7 = check_for_special_ability(player, card, 0x10), iVar7 == 0))
      {
        uVar5 = 6;
      }
      else
      {
        uVar5 = 1;
      }
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016",
                          "Make Kumena unblockable", iVar2, uVar5, "Draw a card", iVar3, 5,
                          "Pump your Merfolk", iVar4, 10, 0, 0, 0, 0);
      if (iVar2 != 0)
      {
        if (iVar2 == 1)
        {
          iVar2 = 1;
          td_subtype = &td;
        }
        else
        {
          if (iVar2 == 2)
          {
            iVar2 = 3;
          }
          else
          {
            if (iVar2 != 3)
            {
              return 0;
            }
            iVar2 = 5;
          }
          td_subtype = &td1;
        }
        iVar2 = generic_activated_ability_with_tapsubtype_as_cost(player, card, EVENT_ACTIVATE,
                                                                  GAA_NONE, 0, 0, 0, 0, 0, 0, 0,
                                                                  NULL, NULL, td_subtype, iVar2);
        return iVar2;
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
    default_target_definition(player, card, &td, 0x47);
    td.allowed_controller = player;
    td.preferred_controller = player;
    td.required_subtype = 0x74;
    td.illegal_abilities = 0;
    td.illegal_state = 1;
    td.special = 0x4000;
    default_target_definition(player, card, &td1, 0x47);
    td1.allowed_controller = player;
    td1.preferred_controller = player;
    td1.required_subtype = 0x74;
    td1.illegal_abilities = 0;
    td1.illegal_state = 1;
    instance = get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    iVar6 = instance->info_slot;
    if (iVar6 == 1)
    {
      pump_ability_until_eot_no_repeat_2022(player, card, instance->parent_controller,
                                       instance->parent_card, 0, 0x10);
      iVar6 = instance->info_slot;
    }
    if (iVar6 == 2)
    {
      draw_a_card(player);
      iVar6 = instance->info_slot;
    }
    if (iVar6 == 3)
    {
      default_test_definition(&this_test, 0x47);
      this_test.subtype = 0x74;
      new_manipulate_all(instance->parent_controller, instance->parent_card, player, &this_test,
                         ACT_ADD_COUNTERS_BASE | ACT_KILL_BURY);
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2192460; original cards/rivals_of_ixalan.c:1534. */
int card_forerunner_of_the_heralds(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  result = duh_mode(player);
  etb_ability_2022(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 0x47);
    test.subtype = 0x74;
    test.not_me = 1;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_ETB)
  {
    if ((trigger_cause_controller != player) || (trigger_cause != card))
    {
      add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
      return 0;
    }
    new_default_test_definition(&test, 0xff, "Select a Merfolk card.");
    test.subtype = 0x74;
    new_global_tutor(player, player, 1, 2, 0, 1, &test);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2193530; original cards/rivals_of_ixalan.c:1949. */
int card_elenda_the_dusk_rose(int player, int card, event_t event)
{
  uint16_t uVar1;
  unsigned int uVar2;
  int result;
  card_instance_t *instance;
  token_generation_t token;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  lifelink(player, card, event);
  default_test_definition(&this_test, 2);
  this_test.not_me = 1;
  count_for_gfp_ability(player, card, event, 2, 0, &this_test);
  uVar2 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (uVar2 != 0)
  {

    for (uVar2 = uVar2 & 0xff; uVar2 != 0; uVar2 = uVar2 - 1)
    {
      add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
    }
  }
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  default_token_definition(player, card, 0x49a0, &token);
  token.tou = 1;
  token.pow = 1;
  token.color_forced = ' ';
  token.s_key_plus = 2;
  instance = get_card_instance(player, card);
  uVar1 = instance->power;
  token.qty = (unsigned char)uVar1;
  generate_token(&token);
  return 0;
}

/* ManalinkEh 2022: 0x2193b30; original cards/rivals_of_ixalan.c:2081. */
int card_huatli_radiant_champion(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  int power;
  target_definition_t *ptStack_68;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = can_target(&td);
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017",
                           "Get Loyalty from creatures", 1, 5, 1, "Pump a creature", result, 10,
                           0xffffffff, "Emblem", 1, 0x14, 0xfffffff8, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (result == 2)
      {
        new_pick_target(&td, "TARGET_CREATURE", 0, 1);
      }
      goto LAB_02193ba2;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_02193ba2;
    default_target_definition(player, card, &td, 2);
    instance = get_card_instance(player, card);
  }
  ptStack_68 = &td;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    result = instance->info_slot;
    if (result == 1)
    {
      result = count_subtype(instance->parent_controller, 2, -1);
      add_counters_from_source(player, card, instance->parent_controller, instance->parent_card,
                               COUNTER_LOYALTY, result);
    }
    else if (result == 2)
    {
      result = validate_target(player, card, ptStack_68, 0);
      if (result != 0)
      {
        result = count_subtype(instance->parent_controller, 2, -1);
        power = count_subtype(instance->parent_controller, 2, -1);
        pump_until_eot_merge_previous_2022(player, card, instance->targets[0].player,
                                      instance->targets[0].card, power, result);
      }
    }
    else if (result == 3)
    {
      generate_token_by_id(player, card, 0x4730);
    }
  }
LAB_02193ba2:
  result = planeswalker(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x2195a40; original cards/rivals_of_ixalan.c:2695. */
int card_siegehorn_ceratops(int player, int card, event_t event)
{
  int result;

  result = is_humiliated_for_ability(player, card, AL_TRIG);
  if (result == 0)
  {
    result = damage_dealt_to_me_arbitrary(player, card, event, DDBM_0, player, card);
    if (result != 0)
    {
      add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 2);
      return 0;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21914d0; original cards/rivals_of_ixalan.c:989. */
int card_tetzimoc_primal_death(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int card_00;
  target_definition_t td;

  check_legend_rule(player, card, event);
  deathtouch_2022(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    iVar2 = 1 - player;
    if (0 < active_cards_count[iVar2])
    {
      card_00 = 0;
      do
      {
        instance = in_play(iVar2, card_00);
        if (((instance != NULL) && (iVar3 = is_what(iVar2, card_00, 2), iVar3 != 0)) &&
            (iVar3 = count_counters(iVar2, card_00, COUNTER_PREY), 0 < iVar3))
        {
          kill_card(iVar2, card_00, KILL_DESTROY);
        }
        card_00 = card_00 + 1;
      } while (card_00 < active_cards_count[iVar2]);
    }
  }
  else if (event - EVENT_CAN_ACTIVATE_FROM_HAND < 3)
  {
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 1 - player;
    if (event == EVENT_CAN_ACTIVATE_FROM_HAND)
    {
      iVar2 = can_target(&td);
      if (iVar2 == 0)
      {
        return 0;
      }
      if (current_turn != player)
      {
        return 0;
      }
      iVar2 = has_mana(player, 1, 1);
    }
    else
    {
      if (event != EVENT_ACTIVATE_FROM_HAND)
      {
        if (event != EVENT_RESOLVE_ACTIVATION_FROM_HAND)
        {
          return 0;
        }
        if (spell_fizzled == 1)
        {
          return 0;
        }
        add_counters_from_source(player, card, instance->targets[0].player,
                                 instance->targets[0].card, COUNTER_PREY, 1);
        return 0;
      }
      iVar2 = charge_mana_for_activated_ability(player, card, 0, 1, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        return 0;
      }
      iVar2 = pick_target(&td, "TARGET_CREATURE");
    }
    if (iVar2 != 0)
    {
      return 1;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2191710; original cards/rivals_of_ixalan.c:1040. */
int card_twilight_prophet(int player, int card, event_t event)
{
  int result;
  int amount;

  ascend(player, card, event);
  upkeep_trigger_ability(player, card, event, player);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    result = has_the_citys_blessing(player);
    if ((result != 0) && (result = *deck_ptr[player], result != -1))
    {

      remove_card_from_deck(player, 0);
      reveal_card_iid(player, card, result);
      add_card_to_hand(player, result);
      amount = get_cmc_by_internal_id(result);
      lose_life(1 - player, amount);
      result = get_cmc_by_internal_id(result);
      gain_life_2022(player, result);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2192630; original cards/rivals_of_ixalan.c:1598. */
int card_jadelight_ranger(int player, int card, event_t event)
{
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  explore(player, card, EVENT_ETB);
  explore(player, card, EVENT_ETB);
  return 0;
}

/* ManalinkEh 2022: 0x21926b0; original cards/rivals_of_ixalan.c:1627. */
int card_path_of_discovery(int player, int card, event_t event)
{
  int result;

  etb_ability_global(player, event, 2, TYPE_CREATURE, NULL, player, card);
  if (event == EVENT_ETB)
  {
    explore(trigger_cause_controller, trigger_cause, EVENT_ETB);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2195c70; original cards/rivals_of_ixalan.c:2747. */
int card_vault_of_catlacan(int player, int card, event_t event)
{
  int result;

  double_faced_card(player, card, event);
  check_legend_rule(player, card, event);
  if (event == EVENT_CAN_ACTIVATE)
  {

    result = mana_producer(player, card, EVENT_CAN_ACTIVATE);
    if ((result != 0) &&
        (((ai_is_speculating != 1 && ((player != 1 || (((unsigned char)trace_mode & 2) != 0)))) ||
          (result = count_subtype(player, 0x40, -1), result != 0))))
    {
      return 1;
    }
  }
  else if (event == EVENT_ACTIVATE)
  {
    result = count_subtype(player, 0x40, -1);
    result = do_dialog(player, player, card, -1, -1,
                       " Produce one mana of any color\n Tolarian Academy\n Cancel",
                       (unsigned int)(1 < result));
    if (result == 0)
    {
      event = EVENT_ACTIVATE;
    LAB_02195cd3:

      result = mana_producer(player, card, event);
      return result;
    }
    if (result == 1)
    {
      result = count_subtype(player, 0x40, -1);
      produce_mana_tapped_2022(player, card, COLOR_BLUE, result);
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
      goto LAB_02195cd3;
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = mana_producer(player, card, EVENT_CAN_ACTIVATE);
      if ((result != 0) && (result = count_subtype(player, 0x40, -1), 1 < result))
      {
        result = count_subtype(player, 0x40, -1);
        declare_mana_available(player, 2, result);
        return 0;
      }
      result = mana_producer(player, card, EVENT_CAN_ACTIVATE);
      if (result != 0)
      {
        declare_mana_available(player, 7, 1);
        return 0;
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2190650; original cards/rivals_of_ixalan.c:502. */
int card_nezahal_primal_tide(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int result;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  cannot_be_countered(player, card, event);
  if (event == EVENT_MAX_HAND_SIZE)
  {
    if ((current_turn == player) &&
        (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0))
    {
      event_result = 1000;
    }
    if (trigger_condition != 0xd3)
    {
      return 0;
    }
  }
  else if (trigger_condition != 0xd3)
    goto LAB_0219069e;
  if ((card == affected_card) && (player == affected_card_controller))
  {
    default_test_definition(&this_test, 2);
    this_test.type_flag = 1;
    sp_ability_trigger(1 - player, event, 2, TYPE_NONE, &this_test, player, card);
  }
LAB_0219069e:
  if (event == EVENT_SPA)
  {
    draw_a_card(player);
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    if (2 < hand_count[player])
    {
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0, 0, 0, 0, 0, 0, 0,
                                         NULL, NULL);
      return result;
    }
  }
  else if (event == EVENT_ACTIVATE)
  {
    result = charge_mana_for_activated_ability_2022(player, card, 0, 0, 0, 0, 0, 0);
    if (result != 0)
    {
      new_multidiscard(player, 3, 0, player);
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {

    pcVar1 = get_card_instance(player, card);
    pcVar2 = in_play(pcVar1->parent_controller, pcVar1->parent_card);
    if (pcVar2 != NULL)
    {
      remove_until_eot(pcVar1->parent_controller, pcVar1->parent_card, pcVar1->parent_controller,
                       pcVar1->parent_card);
      return 0;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x218f700; original cards/rivals_of_ixalan.c:39. */
int card_baffling_end(int player, int card, event_t event)
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

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  instance = get_card_instance(player, card);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &recovered_stack_td.target, 2);
    recovered_stack_td.target.allowed_controller = 1 - player;
    recovered_stack_td.target.special = 0x800;
    recovered_stack_td.target.extra = 3;
    recovered_stack_td.target.allow_cancel = 0;
    result = can_target(&recovered_stack_td.target);
    if (result != 0)
    {
      result = new_pick_target(
          &recovered_stack_td.target,
          "Select target creature an opponent controls with converted mana cost 3 or less.", 0,
          0x80);
      if (result != 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
      }
    }
    leaves_play_2022(player, card, EVENT_ETB, 1, 1);
  }
  else
  {
    leaves_play_2022(player, card, event, 1, 1);
    if (event == EVENT_LEAVES_PLAY_ABILITY)
    {
      target_opponent(player, card);
      default_token_definition(player, card, 0x491d, &recovered_stack_td.token);
      recovered_stack_td.target.illegal_type = instance->targets[0].player;
      generate_token(&recovered_stack_td.token);
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2196c50; original cards/rivals_of_ixalan.c:3033. */
int card_captains_hook(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  leaves_play_2022(player, card, event, 1, 0);
  if ((event == EVENT_UNATTACH) || (event == EVENT_LEAVES_PLAY_ABILITY))
  {
    instance = is_equipping(player, card);
    if (instance != NULL)
    {

      instance = get_card_instance(player, card);
      if (instance->damage_target_player == player)
      {
        kill_card(player, instance->damage_target_card, KILL_SACRIFICE);
      }
    }
  }
  attached_is_additional_subtype(player, card, event, SUBTYPE_PIRATE);
  result = vanilla_equipment(player, card, event, 1, 2, 0, 0, 0x10000);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int golden_guardian_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x21936a0; original cards/rivals_of_ixalan.c:1985. */
int card_hadanas_climb(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  double_faced_card(player, card, event);
  check_legend_rule(player, card, event);
  result = beginning_of_combat_2022(player, card, event, player);
  if (result != 0)
  {

    instance = get_card_instance(player, card);
    if (current_turn == player)
    {
      default_target_definition(player, card, &td, 2);
      td.allow_cancel = 0;
      instance->number_of_targets = '\0';
      td.preferred_controller = player;
      td.allowed_controller = player;
      result = can_target(&td);
      if (result != 0)
      {
        result = new_pick_target(&td, "TARGET_CREATURE_YOU_CONTROL", 0, 0);
        if (result != 0)
        {
          add_counters_from_source(player, card, instance->targets[0].player,
                                   instance->targets[0].card, COUNTER_P1_P1, 1);
          result =
              count_counters(instance->targets[0].player, instance->targets[0].card, COUNTER_P1_P1);
          if (2 < result)
          {
            transform(player, card);
          }
        }
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x218f0d0; original cards/rivals_of_ixalan.c:2155. */
static int journey_to_eternity_legacy(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;

  instance = get_card_instance(player, card);
  iVar3 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (iVar3 != 0)
  {

    seek_grave_for_id_to_reanimate(player, card, instance->targets[0].player,
                                   instance->targets[0].card, REANIMATE_DEFAULT);
  }
  other_leaves_play_2022(player, card, instance->targets[1].player, instance->targets[1].card,
                         event, 1, 0);
  if (event == EVENT_LEAVES_PLAY_ABILITY)
  {

    iVar3 = instance->targets[2].player;
    iVar1 = instance->targets[2].card;
    iVar4 = count_graveyard(iVar3);
    iVar4 = iVar4 + -1;
    if (-1 < iVar4)
    {
      do
      {
        if (graveyard_ptr_mutable[iVar3][iVar4] == iVar1)
        {
          reanimate_permanent(player, card, iVar3, iVar4, REANIMATE_RETURN_TO_PLAY_TRANSFORMED);
          break;
        }
        iVar4 = iVar4 + -1;
      } while (iVar4 != -1);
    }
    kill_card(player, card, KILL_EXILE);
  }
  iVar3 = eot_trigger(player, card, event);
  if (iVar3 == 0)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x218f350; original cards/rivals_of_ixalan.c:3061. */
static int golden_guardian_legacy(int player, int card, event_t event)
{
  char cVar1;
  card_instance_t *instance;
  int result;
  int owner;
  int position;

  instance = get_card_instance(player, card);
  other_leaves_play_2022(player, card, (int)instance->damage_target_player,
                         instance->damage_target_card, event, 1, 0);
  if (event == EVENT_LEAVES_PLAY_ABILITY)
  {
    cVar1 = instance->damage_target_player;
    result = instance->damage_target_card;
    instance->targets[6].player = (int)cVar1;
    instance->targets[6].card = result;
    result = find_in_owners_graveyard((int)cVar1, result, &owner, &position);
    if (result != 0)
    {

      remove_card_from_grave(owner, position);
      result = get_internal_card_id_from_csv_id(0x4728);
      result = add_card_to_hand(player, result);
      set_special_flags3_2022(player, result, 0x201);
      put_into_play(player, result);
      dispatch_event_with_initial_event_result(player, result,
                                               EVENT_ANOTHER_PERMANENT_HAS_TRANSFORMED, 0);
      verify_legend_rule(player, result, (unsigned int)cards_data[instance->targets[0xc].card].id);
    }
    kill_card(player, card, KILL_EXILE);
  }
  result = eot_trigger(player, card, event);
  if (result == 0)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x2196d40; original cards/rivals_of_ixalan.c:3084. */
int card_golden_guardian(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  double_faced_card(player, card, event);
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 2);
    td.special = 0x4000;
    td.allowed_controller = player;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.special = 0x4000;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      instance = get_card_instance(player, card);
      fight(instance->parent_controller, instance->parent_card, instance->targets[0].player,
            instance->targets[0].card);
      create_targetted_legacy_effect_no_repeat(player, card, golden_guardian_legacy,
                                               instance->parent_controller, instance->parent_card);
    }
  }
  result = generic_activated_ability(player, card, event, 4, 2, 0, 0, 0, 0, 0, 0, &td,
                                     "SELECT_A_CREATURE_YOU_CONTROL");
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2191bf0; original cards/rivals_of_ixalan.c:1199. */
int card_dire_fleet_daredevil(int player, int card, event_t event)
{
  enum
  {
    MPCFE_EXILE_ON_RESOLUTION = 128
  };

  unsigned short uVar1;
  int result;
  int player_00;
  test_definition_t this_test;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    player_00 = 1 - player;
    new_default_test_definition(&this_test, 0x38, "Select an instant or sorcery card to exile.");
    this_test.type_flag = 2;
    result = new_special_count_grave(player_00, &this_test);
    if (0 < result)
    {
      result = graveyard_has_shroud(player_00);
      if (result == 0)
      {

        result = new_select_a_card(player, player_00, 2, 1, 2, -1, &this_test);
        if (-1 < result)
        {

          uVar1 = cards_data[graveyard_ptr_mutable[player_00][result]].id;
          rfg_card_from_grave(player_00, result);
          create_may_play_card_from_exile_effect(player, card, player_00, (unsigned int)uVar1,
                                                 MPCFE_EXILE_ON_RESOLUTION | MPCFE_UNTIL_EOT |
                                                     MPCFE_FOR_CMC);
        }
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x218fcf0; original cards/rivals_of_ixalan.c:244. */
int card_slaughter_the_strong(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *pcVar3;
  card_instance_t *pcVar4;
  unsigned int uVar5;
  char *pcVar6;
  unsigned int uVar7;
  bool bVar8;
  unsigned char bVar9;
  unsigned int uStack_270;
  target_definition_t td;
  char msg[100];
  char marked[2][151];

  bVar9 = 0;
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

      pcVar6 = marked[0];
      uVar5 = 0x12e;
      pcVar3 = get_card_instance(player, card);
      bVar8 = ((unsigned int)pcVar6 & 2) != 0;
      if (bVar8)
      {
        marked[0][0] = '\0';
        marked[0][1] = '\0';
        pcVar6 = marked[0] + 2;
        uVar5 = 300;
      }
      for (uVar5 = uVar5 >> 2; uVar5 != 0; uVar5 = uVar5 - 1)
      {
        pcVar6[0] = '\0';
        pcVar6[1] = '\0';
        pcVar6[2] = '\0';
        pcVar6[3] = '\0';
        pcVar6 = pcVar6 + (unsigned int)bVar9 * -8 + 4;
      }
      if (!bVar8)
      {
        pcVar6[0] = '\0';
        pcVar6[1] = '\0';
      }

      uStack_270 = 0;
      do
      {
        uVar7 = uStack_270 ^ current_turn;

        uVar5 = 4;
        do
        {
          default_target_definition(player, card, &td, 2);
          td.power_requirement = uVar5 | 0x2000;
          td.allow_cancel = 2;
          td.illegal_abilities = 0;
          td.who_chooses = uVar7;
          td.allowed_controller = uVar7;
          td.preferred_controller = uVar7;
          scnprintf(msg, 100, "Select a creature card with pow %d or less.", uVar5);
          iVar2 = can_target(&td);
          if ((iVar2 == 0) || (iVar2 = new_pick_target(&td, msg, 0, 0x80), iVar2 == 0))
            break;

          marked[pcVar3->targets[0].player][pcVar3->targets[0].card] = '\x01';
          pcVar4 = get_card_instance(pcVar3->targets[0].player, pcVar3->targets[0].card);
          pcVar4->state = pcVar4->state | 0x100000;
          iVar2 = get_power(pcVar3->targets[0].player, pcVar3->targets[0].card);
          uVar5 = uVar5 - iVar2;
        } while (0 < (int)uVar5);
        if (0 < active_cards_count[uVar7])
        {
          iVar2 = 0;
          do
          {
            while (marked[uVar7][iVar2] == '\0')
            {
              iVar2 = iVar2 + 1;
              if (active_cards_count[uVar7] <= iVar2)
                goto LAB_0218ff48;
            }
            pcVar4 = get_card_instance(uVar7, iVar2);
            iVar2 = iVar2 + 1;
            iVar1 = active_cards_count[uVar7];
            pcVar4->state = pcVar4->state & 0xffefffff;
          } while (iVar2 < iVar1);
        }
      LAB_0218ff48:
        uStack_270 = uStack_270 + 1;
      } while (uStack_270 != 2);

      uVar5 = 0;
      do
      {
        uVar7 = current_turn ^ uVar5;
        iVar2 = can_sacrifice(player, uVar7, 1, TYPE_CREATURE, SUBTYPE_INVALID);
        if (iVar2 != 0)
        {
          default_test_definition((test_definition_t *)msg, 2);
          new_manipulate_all(player, card, uVar7, (test_definition_t *)msg, ACT_KILL_SACRIFICE);
        }
        uVar5 = uVar5 + 1;
      } while (uVar5 != 2);
      kill_card(player, card, KILL_DESTROY);
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  iVar2 = basic_spell(player, card, event);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x218f890; original cards/rivals_of_ixalan.c:102. */
int card_forerunner_of_the_legion(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_test;

  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&recovered_stack_test.test, 0x47);
    recovered_stack_test.test.subtype = 0xcd;
    recovered_stack_test.test.not_me = 1;
    etb_ability_global(player, event, 2, TYPE_NONE, &recovered_stack_test.test, player, card);
  }
  if (event == EVENT_ETB)
  {
    if ((trigger_cause_controller != player) || (trigger_cause != card))
    {
      instance = get_card_instance(player, card);
      default_target_definition(player, card, &recovered_stack_test.target, 2);
      recovered_stack_test.target.preferred_controller = (int32_t)(player);
      recovered_stack_test.test.message[0x44] = '\0';
      recovered_stack_test.test.message[0x45] = '\0';
      recovered_stack_test.test.message[0x46] = '\0';
      recovered_stack_test.test.message[0x47] = '\0';
      result = can_target(&recovered_stack_test.target);
      if (result != 0)
      {
        result = pick_target(&recovered_stack_test.target, "TARGET_CREATURE");
        if (result != 0)
        {
          pump_until_eot_merge_previous(player, card, instance->targets[0].player,
                                        instance->targets[0].card, 1, 1);
        }
      }
      spell_fizzled = 0;
      instance->number_of_targets = '\0';
      return 0;
    }
    new_default_test_definition(&recovered_stack_test.test, 0xff, "Select a Vampire card.");
    recovered_stack_test.test.subtype = 0xcd;
    new_global_tutor(player, player, 1, 2, 0, 1, &recovered_stack_test.test);
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2191920; original cards/rivals_of_ixalan.c:1103. */
int card_blood_sun(int player, int card, event_t event)
{
  unsigned int uVar1;
  card_instance_t *instance;
  int result;
  test_definition_t test;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    draw_a_card(player);
    instance = in_play(player, card);
    if (instance == NULL)
      goto LAB_0219198a;
  LAB_02191969:
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      default_test_definition(&test, 1);
      remove_abilities_by_test(2, event, &test, AL_ALL_NM);
    }
  }
  else
  {
    instance = in_play(player, card);
    if (instance != NULL)
      goto LAB_02191969;
  }
  if (event == EVENT_ABILITIES)
  {

    instance = in_play(player, card);
    if ((((instance == NULL) ||
          (instance = in_play(affected_card_controller, affected_card), instance == NULL)) ||
         (result = is_humiliated_for_ability(player, card, AL_STATIC), result != 0)) ||
        (result = is_what(affected_card_controller, affected_card, 1), result == 0))
    {
      return 0;
    }
    event_result = event_result & 0x9f080000U | 0x1f000000;
    instance = get_card_instance(affected_card_controller, affected_card);
    uVar1 = instance->targets[0x11].card;
    instance->state = instance->state & 0xffffdfff;
    instance->targets[0x11].card = uVar1 | 0x6e;
    instance->ext->sp_keywords = instance->ext->sp_keywords & 0x993c00c0;
  }
LAB_0219198a:
  result = global_enchantment(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21969f0; original cards/rivals_of_ixalan.c:2977. */
int card_sanctum_of_the_sun(int player, int card, event_t event)
{
  int result;

  double_faced_card(player, card, event);
  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
    LAB_02196b98:
      play_land_sound_effect(player, card);
      return 0;
    }
    if (event == EVENT_CAN_ACTIVATE)
    {

      result = mana_producer(player, card, EVENT_CAN_ACTIVATE);
      if (result == 0)
      {
        return 0;
      }
      if (ai_is_speculating != 1)
      {
        if (player != 1)
        {
          return 1;
        }
        if (((unsigned char)trace_mode & 2) != 0)
        {
          return 1;
        }
      }
      if (life[player] < 1)
      {
        return 0;
      }
      return 1;
    }
    if (event == EVENT_ACTIVATE)
    {
      if (mana_colors_added[player] != 0)
      {

        result = do_dialog(player, player, card, -1, -1,
                           " Basic land ability\n Multiple Mana ability\n Cancel", 1);
        if (result == 0)
        {
          produce_mana_tapped_all_one_color_2022(player, card, mana_colors_added[player], 1);
          return 0;
        }
        if (result == 2)
        {
          spell_fizzled = 1;
          return 0;
        }
      }
      produce_mana_tapped_all_one_color_2022(player, card, COLOR_TEST_ANY_COLORED, life[player]);
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      if (((event != EVENT_COUNT_MANA) || (card != affected_card)) ||
          (player != affected_card_controller))
      {
        result = mana_producer(player, card, event);
        return result;
      }
      goto LAB_02196b16;
    }
    if (event == EVENT_RESOLVE_SPELL)
      goto LAB_02196b98;
  }
  if (event != EVENT_COUNT_MANA)
  {
    return 0;
  }
LAB_02196b16:
  if (((card == affected_card) && (player == affected_card_controller)) &&
      ((result = mana_producer(player, card, EVENT_CAN_ACTIVATE),
        result != 0 && ((declare_mana_available_hex(player, 0x3e, life[player]),
                         life[player] < 1 && (mana_colors_added[player] != COLOR_TEST_0))))))
  {
    declare_mana_available_maybe_hex(player, mana_colors_added[player], 1);
  }
  return 0;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2193eb0; original cards/rivals_of_ixalan.c:2184. */
int card_journey_to_eternity(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int iVar3;
  int iVar4;
  int player_00;
  target_definition_2022_t td;

  double_faced_card(player, card, event);
  check_legend_rule(player, card, event);
  pcVar1 = get_card_instance(player, card);
  pcVar2 = in_play(player, card);
  if (pcVar2 == NULL)
  {
  LAB_02193fb0:

    if ((((pcVar1->damage_target_player < '\0') ||
          (pcVar2 = in_play(player, card), pcVar2 == NULL)) ||
         (iVar3 = is_humiliated_for_ability(player, card, AL_TRIG), iVar3 != 0)) ||
        (event != EVENT_GRAVEYARD_FROM_PLAY))
      goto LAB_02193f0e;
    iVar3 = pcVar1->damage_target_card;
    if (((iVar3 == affected_card) &&
         (player_00 = (int)pcVar1->damage_target_player, player_00 == affected_card_controller)) &&
        ((iVar4 = is_this_dying(player_00, iVar3),
          iVar4 != 0 && (iVar3 = check_for_death_trigger_removal(player_00, iVar3), iVar3 == 0))))
    {

      iVar3 = create_legacy_effect(player, card, journey_to_eternity_legacy);
      pcVar2 = get_card_instance(player, iVar3);
      iVar3 = get_owner((int)pcVar1->damage_target_player, pcVar1->damage_target_card);
      pcVar2->targets[0].player = iVar3;
      iVar3 = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card);
      pcVar2->targets[1].player = player;
      pcVar2->targets[1].card = card;
      pcVar2->targets[0].card = iVar3;
      iVar3 = get_owner(player, card);
      pcVar2->targets[2].player = iVar3;
      iVar3 = get_original_internal_card_id(player, card);
      pcVar2->targets[0xb].player = 1;
      pcVar2->targets[2].card = iVar3;
    }
    goto LAB_021940e8;
  }
  if (-1 < pcVar1->damage_target_player)
  {
    iVar3 = is_humiliated_for_ability(player, card, AL_TRIG);
    if (iVar3 == 0)
    {
      nice_creature_to_sacrifice((int)pcVar1->damage_target_player, pcVar1->damage_target_card);
    }
    goto LAB_02193fb0;
  }
LAB_02193f0e:
  if (event == EVENT_CAN_CAST)
    goto LAB_02193f38;
  if (event == EVENT_CAST_SPELL)
  {
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_021940e8;
  LAB_02194158:
    if (player == affected_card_controller)
      goto LAB_02193f38;
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    if (event != EVENT_CAST_SPELL)
      goto LAB_02193f38;
  LAB_021941c0:
    if (card == affected_card)
      goto LAB_02194158;
  }
  else
  {
    if (event == EVENT_CHANGE_TARGET)
      goto LAB_02193f38;
  LAB_021940e8:
    if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) == EVENT_CAN_MOVE_AURA) ||
         (event - EVENT_POWER < 3)) ||
        (event == EVENT_RESOLVE_MOVING_AURA))
    {
    LAB_021941e8:
      if (event == EVENT_CAST_SPELL)
        goto LAB_021941c0;
    }
    else if ((event != EVENT_CHANGE_TYPE) && (event != EVENT_SET_COLOR))
    {
      if ((event != EVENT_GET_SELECTED_CARD) && (event != EVENT_STATIC_EFFECTS))
      {
        return 0;
      }
      goto LAB_021941e8;
    }
    if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
        (event == EVENT_CHANGE_TARGET))
      goto LAB_02193f38;
  }
  if ((((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         (2 < event - EVENT_POWER)) &&
        (event != EVENT_RESOLVE_MOVING_AURA)) &&
       ((event != EVENT_CHANGE_TYPE && (event != EVENT_SET_COLOR)))) &&
      ((event != EVENT_GET_SELECTED_CARD && (event != EVENT_STATIC_EFFECTS))))
  {
    return 0;
  }
LAB_02193f38:
  default_target_definition_2022(player, card, &td, 2);
  td.allowed_controller = player;
  iVar3 = aura(player, card, event, &td, "TARGET_CREATURE_YOU_CONTROL");
  return iVar3;
}
#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2191480; original cards/rivals_of_ixalan.c:979. */
int card_sadistic_skymarcher(int player, int card, event_t event)
{
  int result;

  lifelink(player, card, event);
  result = buddy_creature_2022(player, card, event, SUBTYPE_VAMPIRE, 1);
  return result;
}

/* ManalinkEh 2022: 0x2191bb0; original cards/rivals_of_ixalan.c:1191. */
int card_daring_buccaneer(int player, int card, event_t event)
{
  int result;

  result = buddy_creature_2022(player, card, event, SUBTYPE_PIRATE, 2);
  return result;
}

#endif
