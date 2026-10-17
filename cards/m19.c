#include "manalink.h"

/* Abnormal Endurance	|1|B
 * Instant
 * Until end of turn, target creature gets +2/+0 and gains "When this creature dies, return it to the battlefield tapped under its owner's control." */

/* Aegis of the Heavens	|1|W
 * Instant
 * Target creature gets +1/+7 until end of turn. */

/* Aerial Engineer	|2|W|U
 * Creature - Human Artificer 2/4
 * As long as you control an artifact, ~ gets +2/+0 and has flying. */

/* Aether Tunnel	|1|U
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature gets +1/+0 and can't be blocked. */

/* Aethershield Artificer	|3|W
 * Creature - Dwarf Artificer 3/3
 * At the beginning of combat on your turn, target artifact creature you control gets +2/+2 and gains indestructible until end of turn. */

/* Aggressive Mammoth	|3|G|G|G
 * Creature - Elephant 8/8
 * Trample
 * Other creatures you control have trample. */

/* Ajani's Influence	|2|W|W
 * Sorcery
 * Put two +1/+1 counters on target creature.
 * Look at the top five cards of your library. You may reveal a |Swhite card from among them and put it into your hand. Put the rest on the bottom of your library in a random order. */

/* Ajani's Last Stand	|2|W|W
 * Enchantment
 * Whenever a creature or planeswalker you control dies, you may sacrifice ~. If you do, create a 4/4 |Swhite Avatar creature token with flying.
 * When a spell or ability an opponent controls causes you to discard this card, if you control |Ha Plains, create a 4/4 |Swhite Avatar creature token with flying. */

/* Ajani's Welcome	|W
 * Enchantment
 * Whenever a creature enters the battlefield under your control, you gain 1 life. */

/* Ajani, Adversary of Tyrants	|2|W|W
 * Legendary Planeswalker - Ajani (4)
 * +1: Put a +1/+1 counter on each of up to two target creatures.
 * -2: Return target creature card with converted mana cost 2 or less from your graveyard to the battlefield.
 * -7: You get an emblem with "At the beginning of your end step, create three 1/1 |Swhite Cat creature tokens with lifelink." */

/* Ajani, Adversary of Tyrants Emblem	""
 * Emblem - Ajani
 * At the beginning of your end step, create three 1/1 |Swhite Cat creature tokens with lifelink. */

/* Ajani, Wise Counselor	|3|W|W
 * Legendary Planeswalker - Ajani (5)
 * +2: You gain 1 life for each creature you control.
 * -3: Creatures you control get +2/+2 until end of turn.
 * -9: Put X +1/+1 counters on target creature, where X is your life total. */

/* Alpine Moon	|R
 * Enchantment
 * As ~ enters the battlefield, choose a nonbasic land card name.
 * Lands your opponents control with the chosen name lose all land types and abilities, and they gain "|T: Add one mana of any color." */

/* Amulet of Safekeeping	|2
 * Artifact
 * Whenever you become the target of a spell or ability an opponent controls, counter that spell or ability unless its controller pays |1.
 * Creature tokens get -1/-0. */

/* Angel of the Dawn	|4|W
 * Creature - Angel 3/3
 * Flying
 * When ~ enters the battlefield, creatures you control get +1/+1 and gain vigilance until end of turn. */

/* Apex of Power	|7|R|R|R
 * Sorcery
 * Exile the top seven cards of your library. Until end of turn, you may cast nonland cards exiled this way.
 * If this spell was cast from your hand, add ten mana of any one color. */

/* Arcades, the Strategist	|1|G|W|U
 * Legendary Creature - Elder Dragon 3/5
 * Flying, vigilance
 * Whenever a creature with defender enters the battlefield under your control, draw a card.
 * Each creature you control with defender assigns combat damage equal to its toughness rather than its power and can attack as though it didn't have defender. */

/* Arcane Encyclopedia	|3
 * Artifact
 * |3, |T: Draw a card. */

/* Arisen Gorgon	|1|B|B
 * Creature - Zombie Gorgon 3/3
 * ~ has deathtouch as long as you control a Liliana planeswalker. */

/* Aven Wind Mage	|2|U
 * Creature - Bird Wizard 2/2
 * Flying
 * Whenever you cast an instant or sorcery spell, ~ gets +1/+1 until end of turn. */

/* Aviation Pioneer	|2|U
 * Creature - Human Artificer 1/2
 * When ~ enters the battlefield, create a 1/1 colorless Thopter artifact creature token with flying. */

/* Blood Divination	|3|B
 * Sorcery
 * As an additional cost to cast this spell, sacrifice a creature.
 * Draw three cards. */

/* Bogstomper	|4|B|B
 * Creature - Beast 6/5 */

/* Bone Dragon	|3|B|B
 * Creature - Dragon Skeleton 5/4
 * Flying
 * |3|B|B, Exile seven other cards from your graveyard: Return ~ from your graveyard to the battlefield tapped. */

/* Brawl-Bash Ogre	|2|B|R
 * Creature - Ogre Warrior 3/3
 * Menace
 * Whenever ~ attacks, you may sacrifice another creature. If you do, ~ gets +2/+2 until end of turn. */

/* Bristling Boar	|3|G
 * Creature - Boar 4/3
 * ~ can't be blocked by more than one creature. */

/* Catalyst Elemental	|2|R
 * Creature - Elemental 2/2
 * Sacrifice ~: Add |R|R. */

/* Cavalry Drillmaster	|1|W
 * Creature - Human Knight 2/1
 * When ~ enters the battlefield, target creature gets +2/+0 and gains first strike until end of turn. */

/* Chaos Wand	|3
 * Artifact
 * |4, |T: Target opponent exiles cards from the top of their library until they exile an instant or sorcery card. You may cast that card without paying its mana cost. Then put the exiled cards that weren't cast this way on the bottom of that library in a random order. */

/* Chromium, the Mutable	|4|W|U|B
 * Legendary Creature - Elder Dragon 7/7
 * Flash
 * This spell can't be countered.
 * Flying
 * Discard a card: Until end of turn, ~ becomes a Human with base power and toughness 1/1, loses all abilities, and gains hexproof. It can't be blocked this turn. */

/* Cleansing Nova	|3|W|W
 * Sorcery
 * Choose one -
 * * Destroy all creatures.
 * * Destroy all artifacts and enchantments. */

/* Colossal Majesty	|2|G
 * Enchantment
 * At the beginning of your upkeep, if you control a creature with power 4 or greater, draw a card. */

/* Court Cleric	|W
 * Creature - Human Cleric 1/1
 * Lifelink
 * ~ gets +1/+1 as long as you control an Ajani planeswalker. */

/* Dark-Dweller Oracle	|1|R
 * Creature - Goblin Shaman 2/2
 * |1, Sacrifice a creature: Exile the top card of your library. You may play that card this turn. */

/* Daybreak Chaplain	|1|W
 * Creature - Human Cleric 1/3
 * Lifelink */

/* Declare Dominance	|3|G|G
 * Sorcery
 * Target creature gets +3/+3 until end of turn. All creatures able to block it this turn do so. */

/* Demanding Dragon	|3|R|R
 * Creature - Dragon 5/5
 * Flying
 * When ~ enters the battlefield, it deals 5 damage to target opponent unless that player sacrifices a creature. */

/* Demon of Catastrophes	|2|B|B
 * Creature - Demon 6/6
 * As an additional cost to cast this spell, sacrifice a creature.
 * Flying, trample */

/* Departed Deckhand	|1|U
 * Creature - Spirit Pirate 2/2
 * When ~ becomes the target of a spell, sacrifice it.
 * ~ can't be blocked except by Spirits.
 * |3|U: Another target creature you control can't be blocked this turn except by Spirits. */

/* Desecrated Tomb	|3
 * Artifact
 * Whenever one or more creature cards leave your graveyard, create a 1/1 |Sblack Bat creature token with flying. */

/* Detection Tower	""
 * Land
 * |T: Add |C.
 * |1, |T: Until end of turn, your opponents and creatures your opponents control with hexproof can be the targets of spells and abilities you control as though they didn't have hexproof. */

/* Diamond Mare	|2
 * Artifact Creature - Horse 1/3
 * As ~ enters the battlefield, choose a color.
 * Whenever you cast a spell of the chosen color, you gain 1 life. */

/* Dismissive Pyromancer	|1|R
 * Creature - Human Wizard 2/2
 * |R, |T, Discard a card: Draw a card.
 * |2|R, |T, Sacrifice ~: It deals 4 damage to target creature. */

/* Doublecast	|R|R
 * Sorcery
 * When you cast your next instant or sorcery spell this turn, copy that spell. You may choose new targets for the copy. */

/* Draconic Disciple	|1|R|G
 * Creature - Human Shaman 2/2
 * |T: Add one mana of any color.
 * |7, |T, Sacrifice ~: Create a 5/5 |Sred Dragon creature token with flying. */

/* Dragon's Hoard	|3
 * Artifact
 * Whenever a Dragon enters the battlefield under your control, put a gold counter on ~.
 * |T, Remove a gold counter from ~: Draw a card.
 * |T: Add one mana of any color. */

/* Druid of Horns	|3|G
 * Creature - Human Druid 2/3
 * Whenever you cast an Aura spell that targets ~, create a 3/3 |Sgreen Beast creature token. */

/* Dryad Greenseeker	|1|G
 * Creature - Dryad 1/3
 * |T: Look at the top card of your library. If it's a land card, you may reveal it and put it into your hand. */

/* Dwarven Priest	|3|W
 * Creature - Dwarf Cleric 2/4
 * When ~ enters the battlefield, you gain 1 life for each creature you control. */

/* Dwindle	|2|U
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature gets -6/-0.
 * When enchanted creature blocks, destroy it. */

/* Elvish Clancaller	|G|G
 * Creature - Elf Druid 1/1
 * Other Elves you control get +1/+1.
 * |4|G|G, |T: Search your library for a card named ~, put it onto the battlefield, then shuffle your library. */

/* Elvish Rejuvenator	|2|G
 * Creature - Elf Druid 1/1
 * When ~ enters the battlefield, look at the top five cards of your library. You may put a land card from among them onto the battlefield tapped. Put the rest on the bottom of your library in a random order. */

/* Epicure of Blood	|4|B
 * Creature - Vampire 4/4
 * Whenever you gain life, each opponent loses 1 life. */

/* Exclusion Mage	|2|U
 * Creature - Human Wizard 2/2
 * When ~ enters the battlefield, return target creature an opponent controls to its owner's hand. */

/* Fell Specter	|3|B
 * Creature - Specter 1/3
 * Flying
 * When ~ enters the battlefield, target opponent discards a card.
 * Whenever an opponent discards a card, that player loses 2 life. */

/* Fiery Finish	|4|R|R
 * Sorcery
 * ~ deals 7 damage to target creature. */

/* Fountain of Renewal	|1
 * Artifact
 * At the beginning of your upkeep, you gain 1 life.
 * |3, Sacrifice ~: Draw a card. */

/* Fraying Omnipotence	|3|B|B
 * Sorcery
 * Each player loses half their life, then discards half the cards in their hand, then sacrifices half the creatures they control. Round up each time. */

/* Frilled Sea Serpent	|4|U|U
 * Creature - Serpent 4/6
 * |5|U|U: ~ can't be blocked this turn. */

/* Gallant Cavalry	|3|W
 * Creature - Human Knight 2/2
 * Vigilance
 * When ~ enters the battlefield, create a 2/2 |Swhite Knight creature token with vigilance. */

/* Gearsmith Guardian	|5
 * Artifact Creature - Construct 3/5
 * ~ gets +2/+0 as long as you control a |Sblue creature. */

/* Gearsmith Prodigy	|U
 * Creature - Human Artificer 1/2
 * ~ gets +1/+0 as long as you control an artifact. */

/* Ghastbark Twins	|5|G|G
 * Creature - Treefolk 7/7
 * Trample
 * ~ can block an additional creature each combat. */

/* Gigantosaurus	|G|G|G|G|G
 * Creature - Dinosaur 10/10 */

/* Goblin Instigator	|1|R
 * Creature - Goblin Rogue 1/1
 * When ~ enters the battlefield, create a 1/1 |Sred Goblin creature token. */

/* Goblin Motivator	|R
 * Creature - Goblin Warrior 1/1
 * |T: Target creature gains haste until end of turn. */

/* Goblin Trashmaster	|2|R|R
 * Creature - Goblin Warrior 3/3
 * Other Goblins you control get +1/+1.
 * Sacrifice a Goblin: Destroy target artifact. */

/* Goreclaw, Terror of Qal Sisma	|3|G
 * Legendary Creature - Bear 4/3
 * Creature spells you cast with power 4 or greater cost |2 less to cast.
 * Whenever ~ attacks, each creature you control with power 4 or greater gets +1/+1 and gains trample until end of turn. */

/* Gravewaker	|4|B|B
 * Creature - Bird Spirit 5/5
 * Flying
 * |5|B|B: Return target creature card from your graveyard to the battlefield tapped. */

/* Graveyard Marshal	|B|B
 * Creature - Zombie Soldier 3/2
 * |2|B, Exile a creature card from your graveyard: Create a tapped 2/2 |Sblack Zombie creature token. */

/* Greenwood Sentinel	|1|G
 * Creature - Elf Scout 2/2
 * Vigilance */

/* Havoc Devils	|2|R|R
 * Creature - Devil 4/3
 * Trample */

/* Herald of Faith	|3|W|W
 * Creature - Angel 4/3
 * Flying
 * Whenever ~ attacks, you gain 2 life. */

/* Heroic Reinforcements	|2|R|W
 * Sorcery
 * Create two 1/1 |Swhite Soldier creature tokens. Until end of turn, creatures you control get +1/+1 and gain haste. */

/* Hieromancer's Cage	|3|W
 * Enchantment
 * When ~ enters the battlefield, exile target nonland permanent an opponent controls until ~ leaves the battlefield. */

/* Hired Blade	|2|B
 * Creature - Human Assassin 3/2
 * Flash */

/* Hostile Minotaur	|3|R
 * Creature - Minotaur 3/3
 * Haste */

/* Hungering Hydra	|X|G
 * Creature - Hydra 0/0
 * ~ enters the battlefield with X +1/+1 counters on it.
 * ~ can't be blocked by more than one creature.
 * Whenever ~ is dealt damage, put that many +1/+1 counters on it. */

/* Infernal Reckoning	|B
 * Instant
 * Exile target colorless creature. You gain life equal to its power. */

/* Inferno Hellion	|3|R
 * Creature - Hellion 7/3
 * Trample
 * At the beginning of each end step, if ~ attacked or blocked this turn, its owner shuffles it into their library. */

/* Isareth the Awakener	|1|B|B
 * Legendary Creature - Human Wizard 3/3
 * Deathtouch
 * Whenever ~ attacks, you may pay |X. When you do, return target creature card with converted mana cost X from your graveyard to the battlefield with a corpse counter on it. If that creature would leave the battlefield, exile it instead of putting it anywhere else. */

/* Isolate	|W
 * Instant
 * Exile target permanent with converted mana cost 1. */

/* Kargan Dragonrider	|1|R
 * Creature - Human Warrior 2/2
 * As long as you control a Dragon, ~ has flying. */

/* Knight of the Tusk	|4|W|W
 * Creature - Human Knight 3/7
 * Vigilance */

/* Knight's Pledge	|1|W
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature gets +2/+2. */

/* Lathliss, Dragon Queen	|4|R|R
 * Legendary Creature - Dragon 6/6
 * Flying
 * Whenever another nontoken Dragon enters the battlefield under your control, create a 5/5 |Sred Dragon creature token with flying.
 * |1|R: Dragons you control get +1/+0 until end of turn. */

/* Lena, Selfless Champion	|4|W|W
 * Legendary Creature - Human Knight 3/3
 * When ~ enters the battlefield, create a 1/1 |Swhite Soldier creature token for each nontoken creature you control.
 * Sacrifice ~: Creatures you control with power less than ~'s power gain indestructible until end of turn. */

/* Leonin Vanguard	|W
 * Creature - Cat Soldier 1/1
 * At the beginning of combat on your turn, if you control three or more creatures, ~ gets +1/+1 until end of turn and you gain 1 life. */

/* Leonin Warleader	|2|W|W
 * Creature - Cat Soldier 4/4
 * Whenever ~ attacks, create two 1/1 |Swhite Cat creature tokens with lifelink that are tapped and attacking. */

/* Lich's Caress	|3|B|B
 * Sorcery
 * Destroy target creature. You gain 3 life. */

/* Lightning Mare	|R|R
 * Creature - Elemental Horse 3/1
 * This spell can't be countered.
 * ~ can't be blocked by |Sblue creatures.
 * |1|R: ~ gets +1/+0 until end of turn. */

/* Liliana's Contract	|3|B|B
 * Enchantment
 * When ~ enters the battlefield, you draw four cards and you lose 4 life.
 * At the beginning of your upkeep, if you control four or more Demons with different names, you win the game. */

/* Liliana's Spoils	|3|B
 * Sorcery
 * Target opponent discards a card.
 * Look at the top five cards of your library. You may reveal a |Sblack card from among them and put it into your hand. Put the rest on the bottom of your library in a random order. */

/* Liliana, Untouched by Death	|2|B|B
 * Legendary Planeswalker - Liliana (4)
 * +1: Put the top three cards of your library into your graveyard. If at least one of them is a Zombie card, each opponent loses 2 life and you gain 2 life.
 * -2: Target creature gets -X/-X until end of turn, where X is the number of Zombies you control.
 * -3: You may cast Zombie cards from your graveyard this turn. */

/* Liliana, the Necromancer	|3|B|B
 * Legendary Planeswalker - Liliana (4)
 * +1: Target player loses 2 life.
 * -1: Return target creature card from your graveyard to your hand.
 * -7: Destroy up to two target creatures. Put up to two creature cards from graveyards onto the battlefield under your control. */

/* Loxodon Line Breaker	|2|W
 * Creature - Elephant Soldier 3/2 */

/* Marauder's Axe	|2
 * Artifact - Equipment
 * Equipped creature gets +2/+0.
 * Equip |2 */

/* Metamorphic Alteration	|1|U
 * Enchantment - Aura
 * Enchant creature
 * As ~ enters the battlefield, choose a creature.
 * Enchanted creature is a copy of the chosen creature. */

/* Meteor Golem	|7
 * Artifact Creature - Golem 3/3
 * When ~ enters the battlefield, destroy target nonland permanent an opponent controls. */

/* Militia Bugler	|2|W
 * Creature - Human Soldier 2/3
 * Vigilance
 * When ~ enters the battlefield, look at the top four cards of your library. You may reveal a creature card with power 2 or less from among them and put it into your hand. Put the rest on the bottom of your library in a random order. */

/* Mirror Image	|2|U
 * Creature - Shapeshifter 0/0
 * You may have ~ enter the battlefield as a copy of any creature you control. */

/* Mistcaller	|U
 * Creature - Merfolk Wizard 1/1
 * Sacrifice ~: Until end of turn, if a nontoken creature would enter the battlefield and it wasn't cast, exile it instead. */

/* Mystic Archaeologist	|1|U
 * Creature - Human Wizard 2/1
 * |3|U|U: Draw two cards. */

/* Nexus of Fate	|5|U|U
 * Instant
 * Take an extra turn after this one.
 * If ~ would be put into a graveyard from anywhere, reveal ~ and shuffle it into its owner's library instead. */

/* Nicol Bolas, the Ravager	|1|U|B|R
 * Legendary Creature - Elder Dragon 4/4
 * Flying
 * When ~ enters the battlefield, each opponent discards a card.
 * |4|U|B|R: Exile ~, then return him to the battlefield transformed under his owner's control. Activate this ability only any time you could cast a sorcery. */
/* Nicol Bolas, the Arisen	""
 * Legendary Planeswalker - Bolas (7)
 * +2: Draw two cards.
 * -3: ~ deals 10 damage to target creature or planeswalker.
 * -4: Put target creature or planeswalker card from a graveyard onto the battlefield under your control.
 * -12: Exile all but the bottom card of target player's library. */

/* Nightmare's Thirst	|B
 * Instant
 * You gain 1 life. Target creature gets -X/-X until end of turn, where X is the amount of life you gained this turn. */

/* Novice Knight	|W
 * Creature - Human Knight 2/3
 * Defender
 * As long as ~ is enchanted or equipped, it can attack as though it didn't have defender. */

/* Onakke Ogre	|2|R
 * Creature - Ogre Warrior 4/2 */

/* One with the Machine	|3|U
 * Sorcery
 * Draw cards equal to the highest converted mana cost among artifacts you control. */

/* Open the Graves	|3|B|B
 * Enchantment
 * Whenever a nontoken creature you control dies, create a 2/2 |Sblack Zombie creature token. */

/* Ox	""(token)
 * Creature - Ox 2/4 */

/* Palladia-Mors, the Ruiner	|3|R|G|W
 * Legendary Creature - Elder Dragon 6/6
 * Flying, vigilance, trample
 * ~ has hexproof if it hasn't dealt damage yet. */

/* Patient Rebuilding	|3|U|U
 * Enchantment
 * At the beginning of your upkeep, target opponent puts the top three cards of their library into their graveyard, then you draw a card for each land card put into that graveyard this way. */

/* Plague Mare	|1|B|B
 * Creature - Nightmare Horse 2/2
 * ~ can't be blocked by |Swhite creatures.
 * When ~ enters the battlefield, creatures your opponents control get -1/-1 until end of turn. */

/* Poison-Tip Archer	|2|B|G
 * Creature - Elf Archer 2/3
 * Reach
 * Deathtouch
 * Whenever another creature dies, each opponent loses 1 life. */

/* Prodigious Growth	|4|G|G
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature gets +7/+7 and has trample. */

/* Psychic Corrosion	|2|U
 * Enchantment
 * Whenever you draw a card, each opponent puts the top two cards of their library into their graveyard. */

/* Psychic Symbiont	|4|U|B
 * Creature - Nightmare Horror 3/3
 * Flying
 * When ~ enters the battlefield, target opponent discards a card and you draw a card. */

/* Ravenous Harpy	|2|B
 * Creature - Harpy 1/2
 * Flying
 * |1, Sacrifice another creature: Put a +1/+1 counter on ~. */

/* Regal Bloodlord	|3|W|B
 * Creature - Vampire Soldier 2/4
 * Flying
 * At the beginning of each end step, if you gained life this turn, create a 1/1 |Sblack Bat creature token with flying. */

/* Remorseful Cleric	|1|W
 * Creature - Spirit Cleric 2/1
 * Flying
 * Sacrifice ~: Exile all cards from target player's graveyard. */

/* Resplendent Angel	|1|W|W
 * Creature - Angel 3/3
 * Flying
 * At the beginning of each end step, if you gained 5 or more life this turn, create a 4/4 |Swhite Angel creature token with flying and vigilance.
 * |3|W|W|W: Until end of turn, ~ gets +2/+2 and gains lifelink. */

/* Revitalize	|1|W
 * Instant
 * You gain 3 life.
 * Draw a card. */

/* Rhox Oracle	|4|G
 * Creature - Rhino Monk 4/2
 * When ~ enters the battlefield, draw a card. */

/* Riddlemaster Sphinx	|4|U|U
 * Creature - Sphinx 5/5
 * Flying
 * When ~ enters the battlefield, you may return target creature an opponent controls to its owner's hand. */

/* Root Snare	|1|G
 * Instant
 * Prevent all combat damage that would be dealt this turn. */

/* Runic Armasaur	|1|G|G
 * Creature - Dinosaur 2/5
 * Whenever an opponent activates an ability of a creature or land that isn't a mana ability, you may draw a card. */

/* Rustwing Falcon	|W
 * Creature - Bird 1/2
 * Flying */

/* Sai, Master Thopterist	|2|U
 * Legendary Creature - Human Artificer 1/4
 * Whenever you cast an artifact spell, create a 1/1 colorless Thopter artifact creature token with flying.
 * |1|U, Sacrifice two artifacts: Draw a card. */

/* Salvager of Secrets	|3|U|U
 * Creature - Merfolk Wizard 2/2
 * When ~ enters the battlefield, return target instant or sorcery card from your graveyard to your hand. */

/* Sarkhan's Dragonfire	|3|R|R
 * Sorcery
 * ~ deals 3 damage to any target.
 * Look at the top five cards of your library. You may reveal a |Sred card from among them and put it into your hand. Put the rest on the bottom of your library in a random order. */

/* Sarkhan's Unsealing	|3|R
 * Enchantment
 * Whenever you cast a creature spell with power 4, 5, or 6, ~ deals 4 damage to any target.
 * Whenever you cast a creature spell with power 7 or greater, ~ deals 4 damage to each opponent and each creature and planeswalker they control. */

/* Sarkhan's Whelp	|2|R
 * Creature - Dragon 2/2
 * Flying
 * Whenever you activate an ability of a Sarkhan planeswalker, ~ deals 1 damage to any target. */

/* Sarkhan, Dragonsoul	|4|R|R
 * Legendary Planeswalker - Sarkhan (5)
 * +2: ~ deals 1 damage to each opponent and each creature your opponents control.
 * -3: ~ deals 4 damage to target player or planeswalker.
 * -9: Search your library for any number of Dragon creature cards, put them onto the battlefield, then shuffle your library. */

/* Sarkhan, Fireblood	|1|R|R
 * Legendary Planeswalker - Sarkhan (3)
 * +1: You may discard a card. If you do, draw a card.
 * +1: Add two mana in any combination of colors. Spend this mana only to cast Dragon spells.
 * -7: Create four 5/5 |Sred Dragon creature tokens with flying. */

/* Satyr Enchanter	|1|G|W
 * Creature - Satyr Druid 2/2
 * Whenever you cast an enchantment spell, draw a card. */

/* Scholar of Stars	|3|U
 * Creature - Human Artificer 3/2
 * When ~ enters the battlefield, if you control an artifact, draw a card. */

/* Serra's Guardian	|4|W|W
 * Creature - Angel 5/5
 * Flying
 * Vigilance
 * Other creatures you control have vigilance. */

/* Shield Mare	|1|W|W
 * Creature - Horse 2/3
 * ~ can't be blocked by |Sred creatures.
 * When ~ enters the battlefield or becomes the target of a spell or ability an opponent controls, you gain 3 life. */

/* Siegebreaker Giant	|3|R|R
 * Creature - Giant Warrior 6/3
 * Trample
 * |3|R: Target creature can't block this turn. */

/* Sigiled Sword of Valeron	|3
 * Artifact - Equipment
 * Equipped creature gets +2/+0, has vigilance, and is a Knight in addition to its other types.
 * Whenever equipped creature attacks, create a 2/2 |Swhite Knight creature token with vigilance that's attacking.
 * Equip |3 */

/* Silverbeak Griffin	|W|W
 * Creature - Griffin 2/2
 * Flying */

/* Skalla Wolf	|3|G|G
 * Creature - Wolf Spirit 3/3
 * When ~ enters the battlefield, look at the top five cards of your library. You may reveal a |Sgreen card from among them and put it into your hand. Put the rest on the bottom of your library in a random order. */

/* Skeleton Archer	|3|B
 * Creature - Skeleton Archer 3/3
 * When ~ enters the battlefield, it deals 1 damage to any target. */

/* Skilled Animator	|2|U
 * Creature - Human Artificer 1/3
 * When ~ enters the battlefield, target artifact you control becomes an artifact creature with base power and toughness 5/5 for as long as ~ remains on the battlefield. */

/* Skyrider Patrol	|2|G|U
 * Creature - Elf Scout 2/3
 * Flying
 * At the beginning of combat on your turn, you may pay |G|U. When you do, put a +1/+1 counter on another target creature you control, and that creature gains flying until end of turn. */

/* Skyscanner	|3
 * Artifact Creature - Thopter 1/1
 * Flying
 * When ~ enters the battlefield, draw a card. */

/* Sovereign's Bite	|1|B
 * Sorcery
 * Target player loses 3 life and you gain 3 life. */

/* Sparktongue Dragon	|3|R|R
 * Creature - Dragon 3/3
 * Flying
 * When ~ enters the battlefield, you may pay |2|R. When you do, it deals 3 damage to any target. */

/* Spit Flame	|2|R
 * Instant
 * ~ deals 4 damage to target creature.
 * Whenever a Dragon enters the battlefield under your control, you may pay |R. If you do, return ~ from your graveyard to your hand. */

/* Star-Crowned Stag	|3|W
 * Creature - Elk 3/3
 * Whenever ~ attacks, tap target creature defending player controls. */

/* Stitcher's Supplier	|B
 * Creature - Zombie 1/1
 * When ~ enters the battlefield or dies, put the top three cards of your library into your graveyard. */

/* Strangling Spores	|3|B
 * Instant
 * Target creature gets -3/-3 until end of turn. */

/* Suncleanser	|1|W
 * Creature - Human Cleric 1/4
 * When ~ enters the battlefield, choose one -
 * * Remove all counters from target creature. It can't have counters put on it for as long as ~ remains on the battlefield.
 * * Target opponent loses all counters. That player can't get counters for as long as ~ remains on the battlefield. */

/* Supreme Phantom	|1|U
 * Creature - Spirit 1/3
 * Flying
 * Other Spirits you control get +1/+1. */

/* Surge Mare	|U|U
 * Creature - Horse Fish 0/5
 * ~ can't be blocked by |Sgreen creatures.
 * Whenever ~ deals damage to an opponent, you may draw a card. If you do, discard a card.
 * |1|U: ~ gets +2/-2 until end of turn. */

/* Suspicious Bookcase	|2
 * Artifact Creature - Wall 0/4
 * Defender
 * |3, |T: Target creature can't be blocked this turn. */

/* Take Vengeance	|1|W
 * Sorcery
 * Destroy target tapped creature. */

/* Talons of Wildwood	|1|G
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature gets +1/+1 and has trample.
 * |2|G: Return ~ from your graveyard to your hand. */

/* Tezzeret's Gatebreaker	|4
 * Artifact
 * When ~ enters the battlefield, look at the top five cards of your library. You may reveal a |Sblue or artifact card from among them and put it into your hand. Put the rest on the bottom of your library in a random order.
 * |5|U, |T, Sacrifice ~: Creatures you control can't be blocked this turn. */

/* Tezzeret's Strider	|3
 * Artifact Creature - Golem 3/1
 * As long as you control a Tezzeret planeswalker, ~ has menace. */

/* Tezzeret, Artifice Master	|3|U|U
 * Legendary Planeswalker - Tezzeret (5)
 * +1: Create a 1/1 colorless Thopter artifact creature token with flying.
 * 0: Draw a card. If you control three or more artifacts, draw two cards instead.
 * -9: You get an emblem with "At the beginning of your end step, search your library for a permanent card, put it onto the battlefield, then shuffle your library." */

/* Tezzeret, Artifice Master Emblem	""
 * Emblem - Tezzeret
 * At the beginning of your end step, search your library for a permanent card, put it onto the battlefield, then shuffle your library. */

/* Tezzeret, Cruel Machinist	|4|U|U
 * Legendary Planeswalker - Tezzeret (4)
 * +1: Draw a card.
 * 0: Until your next turn, target artifact you control becomes a 5/5 creature in addition to its other types.
 * -7: Put any number of cards from your hand onto the battlefield face down. They're 5/5 artifact creatures. */

/* Thorn Lieutenant	|1|G
 * Creature - Elf Warrior 2/3
 * Whenever ~ becomes the target of a spell or ability an opponent controls, create a 1/1 |Sgreen Elf Warrior creature token.
 * |5|G: ~ gets +4/+4 until end of turn. */

/* Thud	|R
 * Sorcery
 * As an additional cost to cast this spell, sacrifice a creature.
 * ~ deals damage equal to the sacrificed creature's power to any target. */

/* Transmogrifying Wand	|3
 * Artifact
 * ~ enters the battlefield with three charge counters on it.
 * |1, |T, Remove a charge counter from ~: Destroy target creature. Its controller creates a 2/4 |Swhite Ox creature token. Activate this ability only any time you could cast a sorcery. */

/* Trusty Packbeast	|2|W
 * Creature - Beast 2/3
 * When ~ enters the battlefield, return target artifact card from your graveyard to your hand. */

/* Two-Headed Zombie	|3|B
 * Creature - Zombie 4/2
 * Menace */

/* Uncomfortable Chill	|2|U
 * Instant
 * Creatures your opponents control get -2/-0 until end of turn.
 * Draw a card. */

/* Ursine Champion	|1|G
 * Creature - Human Berserker 2/2
 * |5|G: ~ gets +3/+3 and becomes a Bear Berserker until end of turn. Activate this ability only once each turn. */

/* Vaevictis Asmadi, the Dire	|3|B|R|G
 * Legendary Creature - Elder Dragon 6/6
 * Flying
 * Whenever ~ attacks, for each player, choose target permanent that player controls. Those players sacrifice those permanents. Each player who sacrificed a permanent this way reveals the top card of their library, then puts it onto the battlefield if it's a permanent card. */

/* Valiant Knight	|3|W
 * Creature - Human Knight 3/4
 * Other Knights you control get +1/+1.
 * |3|W|W: Knights you control gain double strike until end of turn. */

/* Vampire Neonate	|B
 * Creature - Vampire 0/3
 * |2, |T: Each opponent loses 1 life and you gain 1 life. */

/* Vampire Sovereign	|3|B|B
 * Creature - Vampire 3/4
 * Flying
 * When ~ enters the battlefield, target opponent loses 3 life and you gain 3 life. */

/* Viashino Pyromancer	|1|R
 * Creature - Viashino Wizard 2/1
 * When ~ enters the battlefield, it deals 2 damage to target player or planeswalker. */

/* Vigilant Baloth	|3|G|G
 * Creature - Beast 5/5
 * Vigilance */

/* Vine Mare	|2|G|G
 * Creature - Elemental Horse 5/3
 * Hexproof
 * ~ can't be blocked by |Sblack creatures. */

/* Vivien Reid	|3|G|G
 * Legendary Planeswalker - Vivien (5)
 * +1: Look at the top four cards of your library. You may reveal a creature or land card from among them and put it into your hand. Put the rest on the bottom of your library in a random order.
 * -3: Destroy target artifact, enchantment, or creature with flying.
 * -8: You get an emblem with "Creatures you control get +2/+2 and have vigilance, trample, and indestructible." */

/* Vivien Reid Emblem	""
 * Emblem - Vivien
 * Creatures you control get +2/+2 and have vigilance, trample, and indestructible. */

/* Vivien of the Arkbow	|4|G|G
 * Legendary Planeswalker - Vivien (5)
 * +2: Put two +1/+1 counters on up to one target creature.
 * -3: Target creature you control deals damage equal to its power to target creature you don't control.
 * -9: Creatures you control get +4/+4 and gain trample until end of turn. */

/* Vivien's Invocation	|5|G|G
 * Sorcery
 * Look at the top seven cards of your library. You may put a creature card from among them onto the battlefield. Put the rest on the bottom of your library in a random order. When a creature is put onto the battlefield this way, it deals damage equal to its power to target creature an opponent controls. */

/* Vivien's Jaguar	|2|G
 * Creature - Cat Spirit 3/2
 * Reach
 * |2|G: Return ~ from your graveyard to your hand. Activate this ability only if you control a Vivien planeswalker. */

/* Volley Veteran	|3|R
 * Creature - Goblin Warrior 4/2
 * When ~ enters the battlefield, it deals damage to target creature an opponent controls equal to the number of Goblins you control. */

/* Wall of Mist	|1|U
 * Creature - Wall 0/5
 * Defender */

/****************************
 * 2018 Christmas Gift Pack *
 ****************************/

/* Angelic Guardian	|4|W|W
 * Creature - Angel 5/5
 * Flying
 * Whenever one or more creatures you control attack, they gain indestructible until end of turn. */

/* Angler Turtle	|5|U|U
 * Creature - Turtle 5/7
 * Hexproof
 * Creatures your opponents control attack each combat if able. */

/* Vengeant Vampire	|4|B|B
 * Creature - Vampire 4/4
 * Lifelink
 * When ~ dies, destroy target creature an opponent controls and you gain 4 life. */

/* Immortal Phoenix	|4|R|R
 * Creature - Phoenix 5/3
 * Flying
 * When ~ dies, return it to its owner's hand. */

/* Rampaging Brontodon	|5|G|G
 * Creature - Dinosaur 7/7
 * Trample
 * Whenever ~ attacks, it gets +1/+1 until end of turn for each land you control. */

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int doublecast_legacy(int player, int card, event_t event);
static int patient_rebuilding_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x22067b0; original cards/m19.c:7. */
int card_aegis_of_the_heavens(int player, int card, event_t event)
{
  int result;
  result = vanilla_instant_pump(player, card, event, 2, player, 1, 7, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x2206930; original cards/m19.c:59. */
int card_ajanis_welcome(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 2);
    result = new_specific_cip(player, card, event, player, 2, &test);
    if (result != 0)
    {
      gain_life(player, 1);
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2206f30; original cards/m19.c:207. */
int card_apex_of_power(int player, int card, event_t event)
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
      default_test_definition(&this_test, 1);
      this_test.type_flag = 1;
      exile_top_x_card_you_may_play_them(player, card, 7, &this_test, player, MPCFE_UNTIL_EOT);
      result = not_played_from_hand(player, card);
      if ((result == 0) && (result = check_special_flags(player, card, 0x40000), result == 0))
      {
        produce_mana_all_one_color(player, COLOR_TEST_ANY_COLORED, 10);
      }
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

/* ManalinkEh 2022: 0x22071b0; original cards/m19.c:256. */
int card_arcane_encyclopedia(int player, int card, event_t event)
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
      draw_a_card(player);
    }
  }
  result = generic_activated_ability(player, card, event, 1, 3, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22078d0; original cards/m19.c:421. */
int card_colossal_majesty(int player, int card, event_t event)
{
  int result;
  upkeep_trigger_ability(player, card, event, player);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    result = ferocious(player, card);
    if (result != 0)
    {
      draw_a_card(player);
    }
  }
  if (event != EVENT_SHOULD_AI_PLAY)
  {
    return (unsigned int)(event == EVENT_CAN_CAST);
  }
  ai_modifier = ai_modifier + (-(unsigned int)(player == 0) & 0xfffffff0) + 8;
  return 0;
}

/* ManalinkEh 2022: 0x2207940; original cards/m19.c:454. */
int card_demon_of_catastrophes(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (event == EVENT_CAN_CAST)
  {
    result = can_sacrifice_type_as_cost(player, 1, TYPE_CREATURE);
    return result;
  }
  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
    result = new_sacrifice(player, card, player, SAC_AS_COST, &test);
    if (result == 0)
    {
      spell_fizzled = 1;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22058a0; original cards/m19.c:570. */
int doublecast_legacy(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (((trigger_condition == 0xd3) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = check_special_flags(trigger_cause_controller, trigger_cause, 0x40000);
    if (result == 0)
    {
      default_test_definition(&test, 0x38);
      result = new_specific_spell_played(player, card, event, player, 2, &test);
      if (result != 0)
      {
        copy_spell_from_stack(player, trigger_cause_controller, trigger_cause);
        kill_card(player, card, KILL_EXILE);
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

/* ManalinkEh 2022: 0x2207ef0; original cards/m19.c:585. */
int card_doublecast(int player, int card, event_t event)
{
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
      create_legacy_effect(player, card, doublecast_legacy);
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

/* ManalinkEh 2022: 0x2208370; original cards/m19.c:707. */
int card_exclusion_mage(int player, int card, event_t event)
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
  instance = get_card_instance(player, card);
  result = can_target(&td);
  if (result != 0)
  {
    result = new_pick_target(&td, "TARGET_CREATURE_OPPONENT_CONTROLS", 0, 0);
    if (result != 0)
    {
      bounce_permanent(instance->targets[0].player, instance->targets[0].card);
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x22084e0; original cards/m19.c:747. */
int card_fiery_finish(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 2);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            damage_target0(player, card, 7);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02208531;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
LAB_02208531:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2208600; original cards/m19.c:774. */
int card_fraying_omnipotence(int player, int card, event_t event)
{
  int result;
  unsigned int uVar2;
  unsigned int uVar3;
  unsigned int uVar4;
  test_definition_t test;
  result = current_turn;
  if (event == EVENT_RESOLVE_SPELL)
  {
    uVar3 = 0;
    uVar2 = round_up_value(life[current_turn]);
    lose_life(result, uVar2);
    uVar4 = current_turn ^ 1;
    uVar2 = round_up_value(life[uVar4]);
    lose_life(uVar4, uVar2);
    result = current_turn;
    uVar2 = round_up_value(hand_count[current_turn]);
    new_multidiscard(result, uVar2, 0, player);
    uVar4 = current_turn ^ 1;
    uVar2 = round_up_value(hand_count[uVar4]);
    new_multidiscard(uVar4, uVar2, 0, player);
    do
    {
      uVar4 = current_turn ^ uVar3;
      uVar3 = uVar3 + 1;
      uVar2 = count_subtype(uVar4, 2, -1);
      uVar2 = round_up_value(uVar2);
      new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
      test.qty = (unsigned short)uVar2;
      new_sacrifice(player, card, uVar4, SAC_CAUSED | SAC_NO_CANCEL, &test);
    } while (uVar3 != 2);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2208780; original cards/m19.c:799. */
int card_frilled_sea_serpent(int player, int card, event_t event)
{
  int result;
  result = generic_shade(player, card, event, 0, 5, 0, 2, 0, 0, 0, 0, 0, 0, 0x10);
  return result;
}

/* ManalinkEh 2022: 0x2208800; original cards/m19.c:828. */
int card_goblin_instigator(int player, int card, event_t event)
{
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  generate_token_by_id(player, card, 0x493a);
  return 0;
}

/* ManalinkEh 2022: 0x2208bd0; original cards/m19.c:894. */
int card_gravewaker(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (event == EVENT_ACTIVATE)
  {
    new_default_test_definition(&test, 2, "Select target creature card.");
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition(&test, 2, "Select target creature card.");
    if ((event == EVENT_RESOLVE_ACTIVATION) &&
        (result = validate_target_from_grave_source(player, card, player, 0), -1 < result))
    {
      reanimate_permanent(player, card, player, result, REANIMATE_TAP);
    }
  }
  result = generic_activated_ability_with_target_in_grave(player, card, event, GAA_NONE, 5, 2, 0, 0,
                                                          0, 0, 0, NULL, NULL, TYPE_NONE, &test, 8);
  return result;
}

/* ManalinkEh 2022: 0x2208fa0; original cards/m19.c:1040. */
int card_inferno_hellion(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  if (trigger_condition != 0xcd)
  {
    return 0;
  }
  if (event - EVENT_TRIGGER < 2)
  {
    instance = get_card_instance(player, card);
    if ((instance->state & 0x4040) != 0)
    {
      result = eot_trigger(player, card, event);
      if (result != 0)
      {
        shuffle_into_library(player, card);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2209d10; original cards/m19.c:1473. */
int card_meteor_golem(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  default_target_definition(player, card, &td, 0x47);
  td.allowed_controller = 1 - player;
  td.illegal_type = 1;
  td.allow_cancel = 0;
  result = can_target(&td);
  if (result != 0)
  {
    result = new_pick_target(&td, "Select target nonland permanent an opponent controls.", 0, 0x81);
    if (result != 0)
    {
      action_on_target(player, card, 0, ACT_KILL_DESTROY);
    }
  }
  instance = get_card_instance(player, card);
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x2209e00; original cards/m19.c:1500. */
int card_mirror_image(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  if (event == EVENT_RESOLVE_SPELL)
  {
    base_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
  }
  result = enters_the_battlefield_as_copy_of_any(player, card, event, &td,
                                                 "SELECT_A_CREATURE_YOU_CONTROL");
  return result;
}

/* ManalinkEh 2022: 0x2209e60; original cards/m19.c:1516. */
int card_mystic_archaeologist(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    draw_cards(player, 2);
  }
  result = generic_activated_ability(player, card, event, 0, 3, 0, 2, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x220a580; original cards/m19.c:1651. */
int card_one_with_the_machine(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = get_highest_cmc(player, TYPE_ARTIFACT);
      ai_modifier = ai_modifier + result * 0xc;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    result = get_highest_cmc(player, TYPE_ARTIFACT);
    draw_cards(player, result);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2206160; original cards/m19.c:1789. */
static int patient_rebuilding_legacy(int player, int card, event_t event)
{
  xtrigger_t xVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  int amount;
  enable_xtrigger_flags = enable_xtrigger_flags | ENABLE_XTRIGGER_MILLED;
  xVar1 = trigger_condition;
  if (trigger_condition == 0xe5)
  {
    xVar1 = xtrigger_impl_value_dont_use_directly;
  }
  if (xVar1 != XTRIGGER_MILLED)
  {
    return 0;
  }
  if (((card == affected_card) && (player == affected_card_controller)) &&
      (reason_for_trigger_controller == player))
  {
    instance = get_card_instance(player, card);
    if (instance->targets[0].player == trigger_cause_controller)
    {
      if (event == EVENT_TRIGGER)
      {
        event_result = event_result | 2;
      }
      else if (event == EVENT_RESOLVE_TRIGGER)
      {
        amount = 0;
        iVar4 = 0;
        if (0 < num_cards_milled)
        {
          do
          {
            if (cards_milled[iVar4].source != -1)
            {
              iVar3 = is_what(-1, cards_milled[iVar4].internal_card_id, 1);
              amount = (amount + 1) - (unsigned int)(iVar3 == 0);
            }
            iVar4 = iVar4 + 1;
          } while (iVar4 < num_cards_milled);
          if (amount != 0)
          {
            draw_cards(player, amount);
          }
        }
        kill_card(player, card, KILL_EXILE);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x220a890; original cards/m19.c:1814. */
int card_patient_rebuilding(int player, int card, event_t event)
{
  int result;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  target_definition_t td;
  upkeep_trigger_ability(player, card, event, player);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = 1 - player;
    td.zone = 0x1000;
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "TARGET_OPPONENT", 0, 1);
      if (result != 0)
      {
        pcVar2 = get_card_instance(player, card);
        result = create_legacy_effect(player, card, patient_rebuilding_legacy);
        pcVar3 = get_card_instance(player, result);
        result = pcVar2->targets[0].player;
        pcVar3->targets[0].player = result;
        mill(result, 3);
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x220a990; original cards/m19.c:1835. */
int card_plague_mare(int player, int card, event_t event)
{
  unsigned int uVar1;
  unsigned int uVar2;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    if (((event == EVENT_BLOCK_LEGALITY) && (attacking_card_controller == player)) &&
        (attacking_card == card))
    {
      uVar1 = get_color(affected_card_controller, affected_card);
      uVar2 = get_sleighted_color_test(player, card, 0x20);
      if ((uVar1 & uVar2) != 0)
      {
        event_result = 1;
      }
    }
    return 0;
  }
  pump_subtype_until_eot(player, card, 1 - player, -1, -1, -1, 0, 0);
  return 0;
}

/* ManalinkEh 2022: 0x220ac80; original cards/m19.c:1965. */
int card_psychic_corrosion(int player, int card, event_t event)
{
  int result;
  result = card_drawn_trigger(player, card, event, player, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    mill(1 - player, 2);
  }
  if (event != EVENT_SHOULD_AI_PLAY)
  {
    return (unsigned int)(event == EVENT_CAN_CAST);
  }
  ai_modifier = ai_modifier + (-(unsigned int)(player == 0) & 0xfffffff0) + 8;
  return 0;
}

/* ManalinkEh 2022: 0x220acf0; original cards/m19.c:1984. */
int card_regal_bloodlord(int player, int card, event_t event)
{
  int result;
  token_generation_t token;
  result = get_trap_condition(player, 1);
  if (result != 0)
  {
    result = eot_trigger(player, card, event);
    if (result != 0)
    {
      default_token_definition(player, card, 0x4905, &token);
      token.tou = 1;
      token.pow = 1;
      generate_token(&token);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x220ad70; original cards/m19.c:1999. */
int card_remorseful_cleric(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  card_instance_t *instance;
  target_definition_t td;

  if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    instance = get_card_instance(player, card);
    iVar2 = generic_activated_ability(player, card, event, 0xc, 0, 0, 0, 0, 0, 0, 0, &td,
                                      "TARGET_PLAYER");
  LAB_0220ae39:
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      return iVar2;
    }
    rfg_whole_graveyard(instance->targets[0].player);
    return iVar2;
  }
  default_target_definition(player, card, &td, 0);
  td.zone = 0x1000;
  instance = get_card_instance(player, card);
  iVar2 = generic_activated_ability(player, card, event, 0xc, 0, 0, 0, 0, 0, 0, 0, &td,
                                    "TARGET_PLAYER");
  if (event != EVENT_ACTIVATE)
    goto LAB_0220ae39;
  if (ai_is_speculating == 1)
  {
    iVar1 = instance->targets[0].player;
    iVar3 = count_graveyard_by_type(iVar1, 0x4a);
    iVar4 = count_graveyard(iVar1);
    iVar4 = iVar4 + iVar3 * 2;
    if (player == iVar1)
      goto LAB_0220afc8;
    iVar4 = iVar4 + -0x1e;
  }
  else
  {
    if (player != 1)
    {
      return iVar2;
    }
    if (((unsigned char)trace_mode & 2) != 0)
    {
      return iVar2;
    }
    iVar1 = instance->targets[0].player;
    iVar3 = count_graveyard_by_type(iVar1, 0x4a);
    iVar4 = count_graveyard(iVar1);
    iVar4 = iVar4 + iVar3 * 2;
    if (iVar1 != 1)
    {
      iVar4 = iVar4 + -0x1e;
      goto LAB_0220af41;
    }
  LAB_0220afc8:
    iVar4 = iVar4 + -0x9e;
  }
  if (player != 1)
  {
    ai_modifier = ai_modifier - iVar4;
    return iVar2;
  }
LAB_0220af41:
  ai_modifier = ai_modifier + iVar4;
  return iVar2;
}

/* ManalinkEh 2022: 0x220afd0; original cards/m19.c:2032. */
int card_resplendent_angel(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  token_generation_t token;
  result = get_trap_condition(player, 1);
  if ((4 < result) && (result = eot_trigger(player, card, event), result != 0))
  {
    default_token_definition(player, card, 0x48ff, &token);
    token.s_key_plus = 8;
    generate_token(&token);
  }
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      instance = get_card_instance(player, card);
      result = instance->parent_card;
      instance = get_card_instance(player, card);
      pump_ability_until_eot(player, card, instance->parent_controller, result, 2, 2, 0, 2);
    }
  }
  result = generic_activated_ability(player, card, event, 0, 3, 0, 0, 0, 0, 3, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x220b140; original cards/m19.c:2053. */
int card_revitalize(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    gain_life(player, 3);
    draw_a_card(player);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x220b710; original cards/m19.c:2278. */
int card_stitchers_supplier(int player, int card, event_t event)
{
  int result;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    mill(player, 3);
  }
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  mill(player, 3);
  return 0;
}

/* ManalinkEh 2022: 0x220b9f0; original cards/m19.c:2337. */
int card_suspicious_bookcase(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      instance = get_card_instance(player, card);
      pump_ability_until_eot(player, card, instance->targets[0].player, instance->targets[0].card,
                             0, 0, 0, 0x10);
    }
  }
  result = generic_activated_ability(player, card, event, 5, 3, 0, 0, 0, 0, 0, 0, &td,
                                     "TARGET_CREATURE");
  return result;
}

/* ManalinkEh 2022: 0x220bdf0; original cards/m19.c:2464. */
int card_transmogrifying_wand(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  token_generation_t token;
  target_definition_t td;
  enters_the_battlefield_with_counters(player, card, event, COUNTER_CHARGE, 3);
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
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      default_token_definition(player, card, 0x4a20, &token);
      token.t_player = instance->targets[0].player;
      generate_token(&token);
    }
  }
  result = generic_activated_ability(player, card, event, 5, 1, 0, 0, 0, 0, 0, 0x17b0100, &td,
                                     "TARGET_CREATURE");
  return result;
}

/* ManalinkEh 2022: 0x220bf90; original cards/m19.c:2500. */
int card_uncomfortable_chill(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    pump_subtype_until_eot(player, card, 1 - player, -1, -2, 0, 0, 0);
    draw_cards(player, 1);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x220c250; original cards/m19.c:2563. */
int card_vampire_neonate(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    lose_life(1 - player, 1);
    gain_life(player, 1);
  }
  result = generic_activated_ability(player, card, event, 1, 2, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x220c300; original cards/m19.c:2581. */
int card_viashino_pyromancer(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  default_target_definition(player, card, &td, 0x100);
  td.zone = 0x1200;
  td.allow_cancel = 0;
  instance = get_card_instance(player, card);
  result = can_target(&td);
  if (result != 0)
  {
    result = new_pick_target(&td, "TARGET_PLAYER_OR_PLANESWALKER", 0, 0);
    if (result != 0)
    {
      damage_target0(player, card, 2);
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x220c3e0; original cards/m19.c:2608. */
int card_vine_mare(int player, int card, event_t event)
{
  unsigned int uVar1;
  unsigned int uVar2;
  hexproof(player, card, event);
  if (((event == EVENT_BLOCK_LEGALITY) && (attacking_card_controller == player)) &&
      (attacking_card == card))
  {
    uVar1 = get_color(affected_card_controller, affected_card);
    uVar2 = get_sleighted_color_test(player, card, 2);
    if ((uVar1 & uVar2) != 0)
    {
      event_result = 1;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x220c470; original cards/m19.c:2630. */
int card_vivien_reid_emblem(int player, int card, event_t event)
{
  boost_subtype(player, card, event, ~SUBTYPE_INVALID, 2, 2, 0x80, 0x808, BCT_CONTROLLER_ONLY);
  return 0;
}

/* ManalinkEh 2022: 0x220c7d0; original cards/m19.c:2717. */
int card_volley_veteran(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  test_definition_t test;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  default_target_definition(player, card, &td, 2);
  td.allowed_controller = 1 - player;
  td.allow_cancel = 0;
  result = can_target(&td);
  if (result != 0)
  {
    result = new_pick_target(&td, "TARGET_CREATURE_OPPONENT_CONTROLS", 0, 0);
    if (result != 0)
    {
      new_default_test_definition(&test, 0x47, "");
      test.subtype = 0x4b;
      result = check_battlefield_for_special_card(player, card, player, 4, &test);
      damage_target0(player, card, result);
    }
  }
  instance = get_card_instance(player, card);
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x220c900; original cards/m19.c:2759. */
int card_vengeant_vampire(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
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
        action_on_target(player, card, 0, ACT_KILL_DESTROY);
        gain_life(player, 4);
        instance->number_of_targets = '\0';
      }
    }
  }
  return 0;
}
