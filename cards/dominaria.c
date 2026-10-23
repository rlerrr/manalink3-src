#include "manalink.h"

/* Academy Drake	|2|U
 * Creature - Drake 2/2
 * Kicker |4
 * Flying
 * If ~ was kicked, it enters the battlefield with two +1/+1 counters on it. */

/* Academy Journeymage	|4|U
 * Creature - Human Wizard 3/2
 * This spell costs |1 less to cast if you control a Wizard.
 * When ~ enters the battlefield, return target creature an opponent controls to its owner's hand. */

/* Adamant Will	|1|W
 * Instant
 * Target creature gets +2/+2 and gains indestructible until end of turn. */

/* Adeliz, the Cinder Wind	|1|U|R
 * Legendary Creature - Human Wizard 2/2
 * Flying, haste
 * Whenever you cast an instant or sorcery spell, Wizards you control get +1/+1 until end of turn. */

/* Adventurous Impulse	|G
 * Sorcery
 * Look at the top three cards of your library. You may reveal a creature or land card from among them and put it into your hand. Put the rest on the bottom of your library in any order. */

/* Amaranthine Wall	|4
 * Artifact Creature - Wall 0/6
 * Defender
 * |2: ~ gains indestructible until end of turn. */

/* Ancient Animus	|1|G
 * Instant
 * Put a +1/+1 counter on target creature you control if it's legendary. Then it fights target creature an opponent controls. */

/* Arbor Armament	|G
 * Instant
 * Put a +1/+1 counter on target creature. That creature gains reach until end of turn. */

/* Arcane Flight	|U
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature gets +1/+1 and has flying. */

/* Artificer's Assistant	|U
 * Creature - Bird 1/1
 * Flying
 * Whenever you cast a historic spell, scry 1. */

/* Arvad the Cursed	|3|W|B
 * Legendary Creature - Vampire Knight 3/3
 * Deathtouch, lifelink
 * Other legendary creatures you control get +2/+2. */

/* Aryel, Knight of Windgrace	|2|W|B
 * Legendary Creature - Human Knight 4/4
 * Vigilance
 * |2|W, |T: Create a 2/2 |Swhite Knight creature token with vigilance.
 * |B, |T, Tap X untapped Knights you control: Destroy target creature with power X or less. */

/* Aven Sentry	|3|W
 * Creature - Bird Soldier 3/2
 * Flying */

/* Baird, Steward of Argive	|2|W|W
 * Legendary Creature - Human Soldier 2/4
 * Vigilance
 * Creatures can't attack you or a planeswalker you control unless their controller pays |1 for each of those creatures. */

/* Baloth Gorger	|2|G|G
 * Creature - Beast 4/4
 * Kicker |4
 * If ~ was kicked, it enters the battlefield with three +1/+1 counters on it. */

/* Befuddle	|2|U
 * Instant
 * Target creature gets -4/-0 until end of turn.
 * Draw a card. */

/* Benalish Honor Guard	|1|W
 * Creature - Human Knight 2/2
 * ~ gets +1/+0 for each legendary creature you control. */

/* Benalish Marshal	|W|W|W
 * Creature - Human Knight 3/3
 * Other creatures you control get +1/+1. */

/* Blackblade Reforged	|2
 * Legendary Artifact - Equipment
 * Equipped creature gets +1/+1 for each land you control.
 * Equip legendary creature |3
 * Equip |7 */

/* Blessed Light	|4|W
 * Instant
 * Exile target creature or enchantment. */

/* Blessing of Belzenlok	|B
 * Instant
 * Target creature gets +2/+1 until end of turn. If it's legendary, it also gains lifelink until end of turn. */

/* Blink of an Eye	|1|U
 * Instant
 * Kicker |1|U
 * Return target nonland permanent to its owner's hand. If this spell was kicked, draw a card. */

/* Bloodstone Goblin	|1|R
 * Creature - Goblin Warrior 2/2
 * Whenever you cast a spell, if that spell was kicked, ~ gets +1/+1 and gains menace until end of turn. */

/* Bloodtallow Candle	|1
 * Artifact
 * |6, |T, Sacrifice ~: Target creature gets -5/-5 until end of turn. */

/* Board the Weatherlight	|1|W
 * Sorcery
 * Look at the top five cards of your library. You may reveal a historic card from among them and put it into your hand. Put the rest on the bottom of your library in a random order. */

/* Broken Bond	|1|G
 * Sorcery
 * Destroy target artifact or enchantment. You may put a land card from your hand onto the battlefield. */

/* Cabal Evangel	|1|B
 * Creature - Human Cleric 2/2 */

/* Cabal Paladin	|3|B
 * Creature - Human Knight 4/2
 * Whenever you cast a historic spell, ~ deals 2 damage to each opponent. */

/* Cabal Stronghold	""
 * Land
 * |T: Add |C.
 * |3, |T: Add |B for each basic |H2Swamp you control. */

/* Caligo Skin-Witch	|1|B
 * Creature - Human Wizard 1/3
 * Kicker |3|B
 * When ~ enters the battlefield, if it was kicked, each opponent discards two cards. */

/* Call the Cavalry	|3|W
 * Sorcery
 * Create two 2/2 |Swhite Knight creature tokens with vigilance. */

/* Cast Down	|1|B
 * Instant
 * Destroy target nonlegendary creature. */

/* Chainer's Torment	|3|B
 * Enchantment - Saga
 * I, II - ~ deals 2 damage to each opponent and you gain 2 life.
 * III - Create an X/X |Sblack Nightmare Horror creature token, where X is half your life total, rounded up. It deals X damage to you. */

/* Nightmare Horror	""(token)
 * Creature - Nightmare Horror 100/100 */

/* Champion of the Flame	|1|R
 * Creature - Human Warrior 1/1
 * Trample
 * ~ gets +2/+2 for each Aura and Equipment attached to it. */

/* Chandra's Outburst	|3|R|R
 * Sorcery
 * ~ deals 4 damage to target player or planeswalker.
 * Search your library and/or graveyard for a card named Chandra, Bold Pyromancer, reveal it, and put it into your hand. If you search your library this way, shuffle it. */

/* Chandra, Bold Pyromancer	|4|R|R
 * Legendary Planeswalker - Chandra (5)
 * +1: Add |R|R. ~ deals 2 damage to target player.
 * -3: ~ deals 3 damage to target creature or planeswalker.
 * -7: ~ deals 10 damage to target player and each creature and planeswalker they control. */

/* Charge	|W
 * Instant
 * Creatures you control get +1/+1 until end of turn. */

/* Cloudreader Sphinx	|4|U
 * Creature - Sphinx 3/4
 * Flying
 * When ~ enters the battlefield, scry 2. */

/* Cold-Water Snapper	|5|U
 * Creature - Turtle 4/5
 * Hexproof */

/* Corrosive Ooze	|1|G
 * Creature - Ooze 2/2
 * Whenever ~ blocks or becomes blocked by an equipped creature, destroy all Equipment attached to that creature at end of combat. */

/* Curator's Ward	|2|U
 * Enchantment - Aura
 * Enchant permanent
 * Enchanted permanent has hexproof.
 * When enchanted permanent leaves the battlefield, if it was historic, draw two cards. */

/* D'Avenant Trapper	|2|W
 * Creature - Human Archer 3/2
 * Whenever you cast a historic spell, tap target creature an opponent controls. */

/* Damping Sphere	|2
 * Artifact
 * If a land is tapped for two or more mana, it produces |C instead of any other type and amount.
 * Each spell a player casts costs |1 more to cast for each other spell that player has cast this turn. */

/* Danitha Capashen, Paragon	|2|W
 * Legendary Creature - Human Knight 2/2
 * First strike, vigilance, lifelink
 * Aura and Equipment spells you cast cost |1 less to cast. */

/* Darigaaz Reincarnated	|4|B|R|G
 * Legendary Creature - Dragon 7/7
 * Flying, trample, haste
 * If ~ would die, instead exile it with three egg counters on it.
 * At the beginning of your upkeep, if ~ is exiled with an egg counter on it, remove an egg counter from it. Then if ~ has no egg counters on it, return it to the battlefield. */

/* Daring Archaeologist	|3|W
 * Creature - Human Artificer 3/3
 * When ~ enters the battlefield, you may return target artifact card from your graveyard to your hand.
 * Whenever you cast a historic spell, put a +1/+1 counter on ~. */

/* Dark Bargain	|3|B
 * Instant
 * Look at the top three cards of your library. Put two of them into your hand and the other into your graveyard. ~ deals 2 damage to you. */

/* Dauntless Bodyguard	|W
 * Creature - Human Knight 2/1
 * As ~ enters the battlefield, choose another creature you control.
 * Sacrifice ~: The chosen creature gains indestructible until end of turn. */

/* Deathbloom Thallid	|2|B
 * Creature - Fungus 3/2
 * When ~ dies, create a 1/1 |Sgreen Saproling creature token. */

/* Deep Freeze	|2|U
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature has base power and toughness 0/4, has defender, loses all other abilities, and is a |Sblue Wall in addition to its other colors and types. */

/* Demonic Vigor	|B
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature gets +1/+1.
 * When enchanted creature dies, return that card to its owner's hand. */

/* Demonlord Belzenlok	|4|B|B
 * Legendary Creature - Elder Demon 6/6
 * Flying, trample
 * When ~ enters the battlefield, exile cards from the top of your library until you exile a nonland card, then put that card into your hand. If the card's converted mana cost is 4 or greater, repeat this process. ~ deals 1 damage to you for each card put into your hand this way. */

/* Diligent Excavator	|1|U
 * Creature - Human Artificer 1/3
 * Whenever you cast a historic spell, target player puts the top two cards of their library into their graveyard. */

/* Divest	|B
 * Sorcery
 * Target player reveals their hand. You choose an artifact or creature card from it. That player discards that card. */

/* Dread Shade	|B|B|B
 * Creature - Shade 3/3
 * |B: ~ gets +1/+1 until end of turn. */

/* Drudge Sentinel	|2|B
 * Creature - Skeleton Warrior 2/1
 * |3: Tap ~. It gains indestructible until end of turn. */

/* Dub	|2|W
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature gets +2/+2, has first strike, and is a Knight in addition to its other types. */

/* Elfhame Druid	|1|G
 * Creature - Elf Druid 0/2
 * |T: Add |G.
 * |T: Add |G|G. Spend this mana only to cast kicked spells. */

/* Eviscerate	|3|B
 * Sorcery
 * Destroy target creature. */

/* Evra, Halcyon Witness	|4|W|W
 * Legendary Creature - Avatar 4/4
 * Lifelink
 * |4: Exchange your life total with ~'s power. */

/* Excavation Elephant	|4|W
 * Creature - Elephant 3/5
 * Kicker |1|W
 * When ~ enters the battlefield, if it was kicked, return target artifact card from your graveyard to your hand. */

/* Fall of the Thran	|5|W
 * Enchantment - Saga
 * I - Destroy all lands.
 * II, III - Each player returns two land cards from their graveyard to the battlefield. */

/* Feral Abomination	|5|B
 * Creature - Thrull 5/5
 * Deathtouch */

/* Fervent Strike	|R
 * Instant
 * Target creature gets +1/+0 and gains first strike and haste until end of turn. */

/* Fiery Intervention	|4|R
 * Sorcery
 * Choose one -
 * * ~ deals 5 damage to target creature.
 * * Destroy target artifact. */

/* Fight with Fire	|2|R
 * Sorcery
 * Kicker |5|R
 * ~ deals 5 damage to target creature. If this spell was kicked, it deals 10 damage divided as you choose among any number of targets instead. */

/* Final Parting	|3|B|B
 * Sorcery
 * Search your library for two cards. Put one into your hand and the other into your graveyard. Then shuffle your library. */

/* Firefist Adept	|4|R
 * Creature - Human Wizard 3/3
 * When ~ enters the battlefield, it deals X damage to target creature an opponent controls, where X is the number of Wizards you control. */

/* Firesong and Sunspeaker	|4|R|W
 * Legendary Creature - Minotaur Cleric 4/6
 * |SRed instant and sorcery spells you control have lifelink.
 * Whenever a |Swhite instant or sorcery spell causes you to gain life, ~ deals 3 damage to target creature or player. */

/* Forebear's Blade	|3
 * Artifact - Equipment
 * Equipped creature gets +3/+0 and has vigilance and trample.
 * Whenever equipped creature dies, attach ~ to target creature you control.
 * Equip |3 */

/* Frenzied Rage	|1|R
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature gets +2/+1 and has menace. */

/* Fungal Infection	|B
 * Instant
 * Target creature gets -1/-1 until end of turn. Create a 1/1 |Sgreen Saproling creature token. */

/* Fungal Plots	|1|G
 * Enchantment
 * |1|G, Exile a creature card from your graveyard: Create a 1/1 |Sgreen Saproling creature token.
 * Sacrifice two Saprolings: You gain 2 life and draw a card. */

/* Gaea's Protector	|3|G
 * Creature - Elemental Warrior 4/2
 * ~ must be blocked if able. */

/* Garna, the Bloodflame	|3|B|R
 * Legendary Creature - Human Warrior 3/3
 * Flash
 * When ~ enters the battlefield, return to your hand all creature cards in your graveyard that were put there from anywhere this turn.
 * Other creatures you control have haste. */

/* Ghitu Chronicler	|1|R
 * Creature - Human Wizard 1/3
 * Kicker |3|R
 * When ~ enters the battlefield, if it was kicked, return target instant or sorcery card from your graveyard to your hand. */

/* Ghitu Journeymage	|2|R
 * Creature - Human Wizard 3/2
 * When ~ enters the battlefield, if you control another Wizard, ~ deals 2 damage to each opponent. */

/* Ghitu Lavarunner	|R
 * Creature - Human Wizard 1/2
 * As long as there are two or more instant and/or sorcery cards in your graveyard, ~ gets +1/+0 and has haste. */

/* Gift of Growth	|1|G
 * Instant
 * Kicker |2
 * Untap target creature. It gets +2/+2 until end of turn. If this spell was kicked, that creature gets +4/+4 until end of turn instead. */

/* Goblin Barrage	|3|R
 * Sorcery
 * Kicker-Sacrifice an artifact or Goblin.
 * ~ deals 4 damage to target creature. If this spell was kicked, it also deals 4 damage to target player or planeswalker. */

/* Goblin Chainwhirler	|R|R|R
 * Creature - Goblin Warrior 3/3
 * First strike
 * When ~ enters the battlefield, it deals 1 damage to each opponent and each creature and planeswalker they control. */

/* Grand Warlord Radha	|2|R|G
 * Legendary Creature - Elf Warrior 3/4
 * Haste
 * Whenever one or more creatures you control attack, add that much mana in any combination of |R and/or |G. Until end of turn, you don't lose this mana as steps and phases end. */

/* Grow from the Ashes	|2|G
 * Sorcery
 * Kicker |2
 * Search your library for a basic land card, put it onto the battlefield, then shuffle your library. If this spell was kicked, instead search your library for two basic land cards, put them onto the battlefield, then shuffle your library. */

/* Grunn, the Lonely King	|4|G|G
 * Legendary Creature - Ape Warrior 5/5
 * Kicker |3
 * If ~ was kicked, it enters the battlefield with five +1/+1 counters on it.
 * Whenever ~ attacks alone, double its power and toughness until end of turn. */

/* Guardians of Koilos	|5
 * Artifact Creature - Construct 4/4
 * When ~ enters the battlefield, you may return another target historic permanent you control to its owner's hand. */

/* Hallar, the Firefletcher	|1|R|G
 * Legendary Creature - Elf Archer 3/3
 * Trample
 * Whenever you cast a spell, if that spell was kicked, put a +1/+1 counter on ~, then ~ deals damage equal to the number of +1/+1 counters on it to each opponent. */

/* Haphazard Bombardment	|5|R
 * Enchantment
 * When ~ enters the battlefield, choose four nonenchantment permanents you don't control and put an aim counter on each of them.
 * At the beginning of your end step, if two or more permanents you don't control have an aim counter on them, destroy one of those permanents at random. */

/* Healing Grace	|W
 * Instant
 * Prevent the next 3 damage that would be dealt to any target this turn by a source of your choice. You gain 3 life. */

/* Helm of the Host	|4
 * Legendary Artifact - Equipment
 * At the beginning of combat on your turn, create a token that's a copy of equipped creature, except the token isn't legendary if equipped creature is legendary. That token gains haste.
 * Equip |5 */

/* History of Benalia	|1|W|W
 * Enchantment - Saga
 * I, II - Create a 2/2 |Swhite Knight creature token with vigilance.
 * III - Knights you control get +2/+1 until end of turn. */

/* Homarid Explorer	|3|U
 * Creature - Homarid Scout 3/3
 * When ~ enters the battlefield, target player puts the top four cards of their library into their graveyard. */

/* Howling Golem	|3
 * Artifact Creature - Golem 2/3
 * Whenever ~ attacks or blocks, each player draws a card. */

/* In Bolas's Clutches	|4|U|U
 * Legendary Enchantment - Aura
 * Enchant permanent
 * You control enchanted permanent.
 * Enchanted permanent is legendary. */

/* Invoke the Divine	|2|W
 * Instant
 * Destroy target artifact or enchantment. You gain 4 life. */

/* Jaya Ballard	|2|R|R|R
 * Legendary Planeswalker - Jaya (5)
 * +1: Add |R|R|R. Spend this mana only to cast instant or sorcery spells.
 * +1: Discard up to three cards, then draw that many cards.
 * -8: You get an emblem with "You may cast instant and sorcery cards from your graveyard. If a card cast this way would be put into your graveyard, exile it instead." */

/* Jaya Ballard Emblem	""
 * Emblem - Jaya
 * You may cast instant and sorcery cards from your graveyard. If a card cast this way would be put into your graveyard, exile it instead. */

/* Jaya's Immolating Inferno	|X|R|R
 * Legendary Sorcery
 * ~ deals X damage to each of up to three targets. */

/* Jhoira's Familiar	|4
 * Artifact Creature - Bird 2/2
 * Flying
 * Historic spells you cast cost |1 less to cast. */

/* Jhoira, Weatherlight Captain	|2|U|R
 * Legendary Creature - Human Artificer 3/3
 * Whenever you cast a historic spell, draw a card. */

/* Jodah, Archmage Eternal	|1|U|R|W
 * Legendary Creature - Human Wizard 4/3
 * Flying
 * You may pay |W|U|B|R|G rather than pay the mana cost for spells that you cast. */

/* Josu Vess, Lich Knight	|2|B|B
 * Legendary Creature - Zombie Knight 4/5
 * Kicker |5|B
 * Menace
 * When ~ enters the battlefield, if it was kicked, create eight 2/2 |Sblack Zombie Knight creature tokens with menace. */

/* Zombie Knight	""(token)
 * Creature - Zombie Knight 2/2
 * Menace */

/* Jousting Lance	|2
 * Artifact - Equipment
 * Equipped creature gets +2/+0.
 * As long as it's your turn, equipped creature has first strike.
 * Equip |3 */

/* Kamahl's Druidic Vow	|X|G|G
 * Legendary Sorcery
 * Look at the top X cards of your library. You may put any number of land and/or legendary permanent cards with converted mana cost X or less from among them onto the battlefield. Put the rest into your graveyard. */

/* Karn's Temporal Sundering	|4|U|U
 * Legendary Sorcery
 * Target player takes an extra turn after this one. Return up to one target nonland permanent to its owner's hand. Exile ~. */

/* Karn, Scion of Urza	|4
 * Legendary Planeswalker - Karn (5)
 * +1: Reveal the top two cards of your library. An opponent chooses one of them. Put that card into your hand and exile the other with a silver counter on it.
 * -1: Put a card you own with a silver counter on it from exile into your hand.
 * -2: Create a 0/0 colorless Construct artifact creature token with "This creature gets +1/+1 for each artifact you control." */

/* Karplusan Hound	|3|R
 * Creature - Hound 3/3
 * Whenever ~ attacks, if you control a Chandra planeswalker, this creature deals 2 damage to any target. */

/* Kazarov, Sengir Pureblood	|5|B|B
 * Legendary Creature - Vampire 4/4
 * Flying
 * Whenever a creature an opponent controls is dealt damage, put a +1/+1 counter on ~.
 * |3|R: ~ deals 2 damage to target creature. */

/* Keldon Overseer	|2|R
 * Creature - Human Warrior 3/1
 * Kicker |3|R
 * Haste
 * When ~ enters the battlefield, if it was kicked, gain control of target creature until end of turn. Untap that creature. It gains haste until end of turn. */

/* Keldon Raider	|2|R|R
 * Creature - Human Warrior 4/3
 * When ~ enters the battlefield, you may discard a card. If you do, draw a card. */

/* Keldon Warcaller	|1|R
 * Creature - Human Warrior 2/2
 * Whenever ~ attacks, put a lore counter on target Saga you control. */

/* Knight of Grace	|1|W
 * Creature - Human Knight 2/2
 * First strike
 * Hexproof from |Sblack
 * ~ gets +1/+0 as long as any player controls a |Sblack permanent. */

/* Knight of Malice	|1|B
 * Creature - Human Knight 2/2
 * First strike
 * Hexproof from |Swhite
 * ~ gets +1/+0 as long as any player controls a |Swhite permanent. */

/* Knight of New Benalia	|1|W
 * Creature - Human Knight 3/1 */

/* Krosan Druid	|2|G
 * Creature - Centaur Druid 2/3
 * Kicker |4|G
 * When ~ enters the battlefield, if it was kicked, you gain 10 life. */

/* Kwende, Pride of Femeref	|3|W
 * Legendary Creature - Human Knight 2/2
 * Double strike
 * Creatures you control with first strike have double strike. */

/* Lich's Mastery	|3|B|B|B
 * Legendary Enchantment
 * Hexproof
 * You can't lose the game.
 * Whenever you gain life, draw that many cards.
 * Whenever you lose life, for each 1 life you lost, exile a permanent you control or a card from your hand or graveyard.
 * When ~ leaves the battlefield, you lose the game. */

/* Lingering Phantom	|5|B
 * Creature - Spirit 5/4
 * Whenever you cast a historic spell, you may pay |B. If you do, return ~ from your graveyard to your hand. */

/* Llanowar Envoy	|2|G
 * Creature - Elf Scout 3/2
 * |1|G: Add one mana of any color. */

/* Llanowar Scout	|1|G
 * Creature - Elf Scout 1/3
 * |T: You may put a land card from your hand onto the battlefield. */

/* Lyra Dawnbringer	|3|W|W
 * Legendary Creature - Angel 5/5
 * Flying, first strike, lifelink
 * Other Angels you control get +1/+1 and have lifelink. */

/* Mammoth Spider	|4|G
 * Creature - Spider 3/5
 * Reach */

/* Marwyn, the Nurturer	|2|G
 * Legendary Creature - Elf Druid 1/1
 * Whenever another Elf enters the battlefield under your control, put a +1/+1 counter on ~.
 * |T: Add an amount of |G equal to ~'s power. */

/* Memorial to Folly	""
 * Land
 * ~ enters the battlefield tapped.
 * |T: Add |B.
 * |2|B, |T, Sacrifice ~: Return target creature card from your graveyard to your hand. */

/* Memorial to Genius	""
 * Land
 * ~ enters the battlefield tapped.
 * |T: Add |U.
 * |4|U, |T, Sacrifice ~: Draw two cards. */

/* Memorial to Glory	""
 * Land
 * ~ enters the battlefield tapped.
 * |T: Add |W.
 * |3|W, |T, Sacrifice ~: Create two 1/1 |Swhite Soldier creature tokens. */

/* Memorial to Unity	""
 * Land
 * ~ enters the battlefield tapped.
 * |T: Add |G.
 * |2|G, |T, Sacrifice ~: Look at the top five cards of your library. You may reveal a creature card from among them and put it into your hand. Then put the rest on the bottom of your library in a random order. */

/* Memorial to War	""
 * Land
 * ~ enters the battlefield tapped.
 * |T: Add |R.
 * |4|R, |T, Sacrifice ~: Destroy target land. */

/* Merfolk Trickster	|U|U
 * Creature - Merfolk Wizard 2/2
 * Flash
 * When ~ enters the battlefield, tap target creature an opponent controls. It loses all abilities until end of turn. */

/* Mesa Unicorn	|1|W
 * Creature - Unicorn 2/2
 * Lifelink */

/* Mishra's Self-Replicator	|5
 * Artifact Creature - Assembly-Worker 2/2
 * Whenever you cast a historic spell, you may pay |1. If you do, create a token that's a copy of ~. */

/* Mox Amber	|0
 * Legendary Artifact
 * |T: Add one mana of any color among legendary creatures and planeswalkers you control. */

/* Muldrotha, the Gravetide	|3|B|G|U
 * Legendary Creature - Elemental Avatar 6/6
 * During each of your turns, you may play up to one permanent card of each permanent type from your graveyard. */

/* Multani, Yavimaya's Avatar	|4|G|G
 * Legendary Creature - Elemental Avatar 0/0
 * Reach, trample
 * ~ gets +1/+1 for each land you control and each land card in your graveyard.
 * |1|G, Return two lands you control to their owner's hand: Return ~ from your graveyard to your hand. */

/* Naban, Dean of Iteration	|1|U
 * Legendary Creature - Human Wizard 2/1
 * If a Wizard entering the battlefield under your control causes a triggered ability of a permanent you control to trigger, that ability triggers an additional time. */

/* Naru Meha, Master Wizard	|2|U|U
 * Legendary Creature - Human Wizard 3/3
 * Flash
 * When ~ enters the battlefield, copy target instant or sorcery spell you control. You may choose new targets for the copy.
 * Other Wizards you control get +1/+1. */

/* Navigator's Compass	|1
 * Artifact
 * When ~ enters the battlefield, you gain 3 life.
 * |T: Until end of turn, target land you control becomes the basic land type of your choice in addition to its other types. */

/* Niambi, Faithful Healer	|1|W|U
 * Legendary Creature - Human Cleric 2/2
 * When ~ enters the battlefield, you may search your library and/or graveyard for a card named Teferi, Timebender, reveal it, and put it into your hand. If you search your library this way, shuffle it. */

/* Oath of Teferi	|3|W|U
 * Legendary Enchantment
 * When ~ enters the battlefield, exile another target permanent you control. Return it to the battlefield under its owner's control at the beginning of the next end step.
 * You may activate the loyalty abilities of planeswalkers you control twice each turn rather than only once. */

/* On Serra's Wings	|3|W
 * Legendary Enchantment - Aura
 * Enchant creature
 * Enchanted creature is legendary, gets +1/+1, and has flying, vigilance, and lifelink. */

/* Orcish Vandal	|1|R
 * Creature - Orc Warrior 1/1
 * |T, Sacrifice an artifact: ~ deals 2 damage to any target. */

/* Pardic Wanderer	|6
 * Artifact Creature - Golem 5/5
 * Trample */

/* Pegasus Courser	|2|W
 * Creature - Pegasus 1/3
 * Flying
 * Whenever ~ attacks, another target attacking creature gains flying until end of turn. */

/* Phyrexian Scriptures	|2|B|B
 * Enchantment - Saga
 * I - Put a +1/+1 counter on up to one target creature. That creature becomes an artifact in addition to its other types.
 * II - Destroy all nonartifact creatures.
 * III - Exile all cards from all opponents' graveyards. */

/* Pierce the Sky	|1|G
 * Instant
 * ~ deals 7 damage to target creature with flying. */

/* Powerstone Shard	|3
 * Artifact
 * |T: Add |C for each artifact you control named ~. */

/* Precognition Field	|3|U
 * Enchantment
 * You may look at the top card of your library.
 * You may cast the top card of your library if it's an instant or sorcery card.
 * |3: Exile the top card of your library. */

/* Primevals' Glorious Rebirth	|5|W|B
 * Legendary Sorcery
 * Return all legendary permanent cards from your graveyard to the battlefield. */

/* Primordial Wurm	|4|G|G
 * Creature - Wurm 7/6 */

/* Pyromantic Pilgrim	|2|R
 * Creature - Human Wizard 3/1
 * Haste */

/* Radiating Lightning	|3|R
 * Instant
 * ~ deals 3 damage to target player and 1 damage to each creature that player controls. */

/* Raff Capashen, Ship's Mage	|2|W|U
 * Legendary Creature - Human Wizard 3/3
 * Flash
 * Flying
 * You may cast historic spells as though they had flash. */

/* Rampaging Cyclops	|3|R
 * Creature - Cyclops 4/4
 * ~ gets -2/-0 as long as two or more creatures are blocking it. */

/* Rat Colony	|1|B
 * Creature - Rat 2/1
 * ~ gets +1/+0 for each other Rat you control.
 * A deck can have any number of cards named ~. */

/* Relic Runner	|1|U
 * Creature - Human Rogue 2/1
 * ~ can't be blocked if you've cast a historic spell this turn. */

/* Rite of Belzenlok	|2|B|B
 * Enchantment - Saga
 * I, II - Create two 0/1 |Sblack Cleric creature tokens.
 * III - Create a 6/6 |Sblack Demon creature token with flying, trample, and "At the beginning of your upkeep, sacrifice another creature. If you can't, this creature deals 6 damage to you." */

/* Rona, Disciple of Gix	|1|U|B
 * Legendary Creature - Human Artificer 2/2
 * When ~ enters the battlefield, you may exile target historic card from your graveyard.
 * You may cast nonland cards exiled with ~.
 * |4, |T: Exile the top card of your library. */

/* Run Amok	|1|R
 * Instant
 * Target attacking creature gets +3/+3 and gains trample until end of turn. */

/* Sanctum Spirit	|3|W
 * Creature - Spirit 3/2
 * Lifelink
 * Discard a historic card: ~ gains indestructible until end of turn. */

/* Saproling Migration	|1|G
 * Sorcery
 * Kicker |4
 * Create two 1/1 |Sgreen Saproling creature tokens. If this spell was kicked, create four of those tokens instead. */

/* Seal Away	|1|W
 * Enchantment
 * Flash
 * When ~ enters the battlefield, exile target tapped creature an opponent controls until ~ leaves the battlefield. */

/* Seismic Shift	|3|R
 * Sorcery
 * Destroy target land. Up to two target creatures can't block this turn. */

/* Sentinel of the Pearl Trident	|4|U
 * Creature - Merfolk Soldier 3/3
 * Flash
 * When ~ enters the battlefield, you may exile target historic permanent you control. If you do, return that card to the battlefield under its owner's control at the beginning of the next end step. */

/* Sergeant-at-Arms	|2|W
 * Creature - Human Soldier 2/3
 * Kicker |2|W
 * When ~ enters the battlefield, if it was kicked, create two 1/1 |Swhite Soldier creature tokens. */

/* Serra Disciple	|1|W
 * Creature - Bird Cleric 1/1
 * Flying, first strike
 * Whenever you cast a historic spell, ~ gets +1/+1 until end of turn. */

/* Settle the Score	|2|B|B
 * Sorcery
 * Exile target creature. Put two loyalty counters on a planeswalker you control. */

/* Shalai, Voice of Plenty	|3|W
 * Legendary Creature - Angel 3/4
 * Flying
 * You, planeswalkers you control, and other creatures you control have hexproof.
 * |4|G|G: Put a +1/+1 counter on each creature you control. */

/* Shanna, Sisay's Legacy	|G|W
 * Legendary Creature - Human Warrior 0/0
 * ~ can't be the target of abilities your opponents control.
 * ~ gets +1/+1 for each creature you control. */

/* Shield of the Realm	|2
 * Artifact - Equipment
 * If a source would deal damage to equipped creature, prevent 2 of that damage.
 * Equip |1 */

/* Shivan Fire	|R
 * Instant
 * Kicker |4
 * ~ deals 2 damage to target creature. If this spell was kicked, it deals 4 damage to that creature instead. */

/* Short Sword	|1
 * Artifact - Equipment
 * Equipped creature gets +1/+1.
 * Equip |1 */

/* Skittering Surveyor	|3
 * Artifact Creature - Construct 1/2
 * When ~ enters the battlefield, you may search your library for a basic land card, reveal it, put it into your hand, then shuffle your library. */

/* Slimefoot, the Stowaway	|1|B|G
 * Legendary Creature - Fungus 2/3
 * Whenever a Saproling you control dies, ~ deals 1 damage to each opponent and you gain 1 life.
 * |4: Create a 1/1 |Sgreen Saproling creature token. */

/* Slinn Voda, the Rising Deep	|6|U|U
 * Legendary Creature - Leviathan 8/8
 * Kicker |1|U
 * When ~ enters the battlefield, if it was kicked, return all creatures to their owners' hands except for Merfolk, Krakens, Leviathans, Octopuses, and Serpents. */

/* Song of Freyalise	|1|G
 * Enchantment - Saga
 * I, II - Until your next turn, creatures you control gain "|T: Add one mana of any color."
 * III - Put a +1/+1 counter on each creature you control. Those creatures gain vigilance, trample, and indestructible until end of turn. */

/* Sorcerer's Wand	|1
 * Artifact - Equipment
 * Equipped creature has "|T: This creature deals 1 damage to target player or planeswalker. If this creature is a Wizard, it deals 2 damage to that player or planeswalker instead."
 * Equip |3 */

/* Soul Salvage	|2|B
 * Sorcery
 * Return up to two target creature cards from your graveyard to your hand. */

/* Sparring Construct	|1
 * Artifact Creature - Construct 1/1
 * When ~ dies, put a +1/+1 counter on target creature you control. */

/* Spore Swarm	|3|G
 * Instant
 * Create three 1/1 |Sgreen Saproling creature tokens. */

/* Sporecrown Thallid	|1|G
 * Creature - Fungus 2/2
 * Each other creature you control that's a Fungus or Saproling gets +1/+1. */

/* Squee, the Immortal	|1|R|R
 * Legendary Creature - Goblin 2/1
 * You may cast ~ from your graveyard or from exile. */

/* Steel Leaf Champion	|G|G|G
 * Creature - Elf Knight 5/4
 * ~ can't be blocked by creatures with power 2 or less. */

/* Stronghold Confessor	|B
 * Creature - Human Cleric 1/1
 * Kicker |3
 * Menace
 * If ~ was kicked, it enters the battlefield with two +1/+1 counters on it. */

/* Sylvan Awakening	|2|G
 * Sorcery
 * Until your next turn, all lands you control become 2/2 Elemental creatures with reach, indestructible, and haste. They're still lands. */

/* Tatyova, Benthic Druid	|3|G|U
 * Legendary Creature - Merfolk Druid 3/3
 * Whenever a land enters the battlefield under your control, you gain 1 life and draw a card. */

/* Teferi's Sentinel	|5
 * Artifact Creature - Golem 2/6
 * As long as you control a Teferi planeswalker, ~ gets +4/+0. */

/* Teferi, Hero of Dominaria	|3|W|U
 * Legendary Planeswalker - Teferi (4)
 * +1: Draw a card. At the beginning of the next end step, untap two lands.
 * -3: Put target nonland permanent into its owner's library third from the top.
 * -8: You get an emblem with "Whenever you draw a card, exile target permanent an opponent controls." */

/* Teferi, Hero of Dominaria Emblem	""
 * Emblem - Teferi
 * Whenever you draw a card, exile target permanent an opponent controls. */

/* Teferi, Timebender	|4|W|U
 * Legendary Planeswalker - Teferi (5)
 * +2: Untap up to one target artifact or creature.
 * -3: You gain 2 life and draw two cards.
 * -9: Take an extra turn after this one. */

/* Tempest Djinn	|U|U|U
 * Creature - Djinn 0/4
 * Flying
 * ~ gets +1/+0 for each basic |H2Island you control. */

/* Temporal Machinations	|2|U
 * Sorcery
 * Return target creature to its owner's hand. If you control an artifact, draw a card. */

/* Territorial Allosaurus	|2|G|G
 * Creature - Dinosaur 5/5
 * Kicker |2|G
 * When ~ enters the battlefield, if it was kicked, it fights another target creature. */

/* Teshar, Ancestor's Apostle	|3|W
 * Legendary Creature - Bird Cleric 2/2
 * Flying
 * Whenever you cast a historic spell, return target creature card with converted mana cost 3 or less from your graveyard to the battlefield. */

/* Tetsuko Umezawa, Fugitive	|1|U
 * Legendary Creature - Human Rogue 1/3
 * Creatures you control with power or toughness 1 or less can't be blocked. */

/* Thallid Omnivore	|3|B
 * Creature - Fungus 3/3
 * |1, Sacrifice another creature: ~ gets +2/+2 until end of turn. If a Saproling was sacrificed this way, you gain 2 life. */

/* Thallid Soothsayer	|3|B
 * Creature - Fungus 2/3
 * |2, Sacrifice a creature: Draw a card. */

/* The Antiquities War	|3|U
 * Enchantment - Saga
 * I, II - Look at the top five cards of your library. You may reveal an artifact card from among them and put it into your hand. Put the rest on the bottom of your library in a random order.
 * III - Artifacts you control become artifact creatures with base power and toughness 5/5 until end of turn. */

/* The Eldest Reborn	|4|B
 * Enchantment - Saga
 * I - Each opponent sacrifices a creature or planeswalker.
 * II - Each opponent discards a card.
 * III - Put target creature or planeswalker card from a graveyard onto the battlefield under your control. */

/* The First Eruption	|2|R
 * Enchantment - Saga
 * I - ~ deals 1 damage to each creature without flying.
 * II - Add |R|R.
 * III - Sacrifice |Ha Mountain. If you do, ~ deals 3 damage to each creature. */

/* The Flame of Keld	|1|R
 * Enchantment - Saga
 * I - Discard your hand.
 * II - Draw two cards.
 * III - If a |Sred source you control would deal damage to a permanent or player this turn, it deals that much damage plus 2 to that permanent or player instead. */

/* The Mending of Dominaria	|3|G|G
 * Enchantment - Saga
 * I, II - Put the top two cards of your library into your graveyard, then you may return a creature card from your graveyard to your hand.
 * III - Return all land cards from your graveyard to the battlefield, then shuffle your graveyard into your library. */

/* The Mirari Conjecture	|4|U
 * Enchantment - Saga
 * I - Return target instant card from your graveyard to your hand.
 * II - Return target sorcery card from your graveyard to your hand.
 * III - Until end of turn, whenever you cast an instant or sorcery spell, copy it. You may choose new targets for the copy. */

/* Thran Temporal Gateway	|4
 * Legendary Artifact
 * |4, |T: You may put a historic permanent card from your hand onto the battlefield. */

/* Tiana, Ship's Caretaker	|3|R|W
 * Legendary Creature - Angel Artificer 3/3
 * Flying, first strike
 * Whenever an Aura or Equipment you control is put into a graveyard from the battlefield, you may return that card to its owner's hand at the beginning of the next end step. */

/* Time of Ice	|3|U
 * Enchantment - Saga
 * I, II - Tap target creature an opponent controls. It doesn't untap during its controller's untap step for as long as you control ~.
 * III - Return all tapped creatures to their owners' hands. */

/* Tolarian Scholar	|2|U
 * Creature - Human Wizard 2/3 */

/* Torgaar, Famine Incarnate	|6|B|B
 * Legendary Creature - Avatar 7/6
 * As an additional cost to cast this spell, you may sacrifice any number of creatures. This spell costs |2 less to cast for each creature sacrificed this way.
 * When ~ enters the battlefield, up to one target player's life total becomes half their starting life total, rounded down. */

/* Traxos, Scourge of Kroog	|4
 * Legendary Artifact Creature - Construct 7/7
 * Trample
 * ~ enters the battlefield tapped and doesn't untap during your untap step.
 * Whenever you cast a historic spell, untap ~. */

/* Triumph of Gerrard	|1|W
 * Enchantment - Saga
 * I, II - Put a +1/+1 counter on target creature you control with the greatest power.
 * III - Target creature you control with the greatest power gains flying, first strike, and lifelink until end of turn. */

/* Two-Headed Giant	|2|R|R
 * Creature - Giant Warrior 4/4
 * Whenever ~ attacks, flip two coins. If both coins come up heads, ~ gains double strike until end of turn. If both coins come up tails, ~ gains menace until end of turn. */

/* Untamed Kavu	|1|G
 * Creature - Kavu 2/2
 * Kicker |3
 * Vigilance, trample
 * If ~ was kicked, it enters the battlefield with three +1/+1 counters on it. */

/* Unwind	|2|U
 * Instant
 * Counter target noncreature spell. Untap up to three lands. */

/* Urgoros, the Empty One	|4|B|B
 * Legendary Creature - Specter 4/3
 * Flying
 * Whenever ~ deals combat damage to a player, that player discards a card at random. If the player can't, you draw a card. */

/* Urza's Ruinous Blast	|4|W
 * Legendary Sorcery
 * Exile all nonland permanents that aren't legendary. */

/* Urza's Tome	|2
 * Artifact
 * |3, |T: Draw a card. Then discard a card unless you exile a historic card from your graveyard. */

/* Valduk, Keeper of the Flame	|2|R
 * Legendary Creature - Human Shaman 3/2
 * At the beginning of combat on your turn, for each Aura and Equipment attached to ~, create a 3/1 |Sred Elemental creature token with trample and haste. Exile those tokens at the beginning of the next end step. */

/* Verix Bladewing	|2|R|R
 * Legendary Creature - Dragon 4/4
 * Kicker |3
 * Flying
 * When ~ enters the battlefield, if it was kicked, create Karox Bladewing, a legendary 4/4 |Sred Dragon creature token with flying. */

/* Karox Bladewing	""(token)
 * Legendary Creature - Dragon 4/4
 * Flying */

/* Vicious Offering	|1|B
 * Instant
 * Kicker-Sacrifice a creature.
 * Target creature gets -2/-2 until end of turn. If this spell was kicked, that creature gets -5/-5 until end of turn instead. */

/* Vodalian Arcanist	|1|U
 * Creature - Merfolk Wizard 1/3
 * |T: Add |C. Spend this mana only to cast an instant or sorcery spell. */

/* Voltaic Servant	|2
 * Artifact Creature - Construct 1/3
 * At the beginning of your end step, untap target artifact. */

/* Warcry Phoenix	|3|R
 * Creature - Phoenix 2/2
 * Flying, haste
 * Whenever you attack with three or more creatures, you may pay |2|R. If you do, return ~ from your graveyard to the battlefield tapped and attacking. */

/* Warlord's Fury	|R
 * Sorcery
 * Creatures you control gain first strike until end of turn.
 * Draw a card. */

/* Weatherlight	|4
 * Legendary Artifact - Vehicle 4/5
 * Flying
 * Whenever ~ deals combat damage to a player, look at the top five cards of your library. You may reveal a historic card from among them and put it into your hand. Put the rest on the bottom of your library in a random order.
 * Crew 3 */

/* Weight of Memory	|3|U|U
 * Sorcery
 * Draw three cards. Target player puts the top three cards of their library into their graveyard. */

/* Whisper, Blood Liturgist	|3|B
 * Legendary Creature - Human Cleric 2/2
 * |T, Sacrifice two creatures: Return target creature card from your graveyard to the battlefield. */

/* Wild Onslaught	|3|G
 * Instant
 * Kicker |4
 * Put a +1/+1 counter on each creature you control. If this spell was kicked, put two +1/+1 counters on each creature you control instead. */

/* Windgrace Acolyte	|4|B
 * Creature - Cat Warrior 3/2
 * Flying
 * When ~ enters the battlefield, put the top three cards of your library into your graveyard and you gain 3 life. */

/* Wizard's Lightning	|2|R
 * Instant
 * This spell costs |2 less to cast if you control a Wizard.
 * ~ deals 3 damage to any target. */

/* Wizard's Retort	|1|U|U
 * Instant
 * This spell costs |1 less to cast if you control a Wizard.
 * Counter target spell. */

/* Yargle, Glutton of Urborg	|4|B
 * Legendary Creature - Frog Spirit 9/3 */

/* Yavimaya Sapherd	|2|G
 * Creature - Fungus 2/2
 * When ~ enters the battlefield, create a 1/1 |Sgreen Saproling creature token. */

/* Yawgmoth's Vile Offering	|4|B
 * Legendary Sorcery
 * Put up to one target creature or planeswalker card from a graveyard onto the battlefield under your control. Destroy up to one target creature or planeswalker. Exile ~. */

/* Zahid, Djinn of the Lamp	|4|U|U
 * Legendary Creature - Djinn 5/6
 * You may pay |3|U and tap an untapped artifact you control rather than pay this spell's mana cost.
 * Flying */

/* Zhalfirin Void	""
 * Land
 * When ~ enters the battlefield, scry 1.
 * |T: Add |C. */

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int give_token_haste_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x22cb240; original cards/dominaria.c:241. */
int card_arvad_the_cursed(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  deathtouch_2022(player, card, event);
  lifelink(player, card, event);
  boost_creature_type(player, card, event, SUBTYPE_LEGEND, 2, 2, 0, BCT_CONTROLLER_ONLY);
  return 0;
}

/* ManalinkEh 2022: 0x22cbb50; original cards/dominaria.c:390. */
int card_baird_steward_of_argive(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  vigilance_2022(player, card, event);
  tax_attack(player, card, event);
  return 0;
}

/* ManalinkEh 2022: 0x22cbba0; original cards/dominaria.c:406. */
int card_befuddle(int player, int card, event_t event)
{
  int result;
  result = vanilla_instant_pump_2022(player, card, event, 2, 1 - player, -4, 0, 0x1000000, 0);
  return result;
}

/* ManalinkEh 2022: 0x22cbc00; original cards/dominaria.c:420. */
int card_benalish_marshal(int player, int card, event_t event)
{
  boost_subtype(player, card, event, ~SUBTYPE_INVALID, 1, 1, 0, 0, BCT_CONTROLLER_ONLY);
  return 0;
}

/* ManalinkEh 2022: 0x22cc4a0; original cards/dominaria.c:550. */
int card_broken_bond(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 0x44);
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = validate_target(player, card, &td, 0);
          if (result != 0)
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
            new_default_test_definition(&test, 1, "Select a land card.");
            new_global_tutor(player, player, 4, 4, 0, 1, &test);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_022cc513;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x44);
  get_card_instance(player, card);
LAB_022cc513:
  result = generic_spell(player, card, event, 1, &td, "DISENCHANT", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22ccaa0; original cards/dominaria.c:662. */
int card_cast_down(int player, int card, event_t event)
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
        td.required_subtype = 0x2001;
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
        goto LAB_022ccb03;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.required_subtype = 0x2001;
  td.special = 0x1000;
LAB_022ccb03:
  result = generic_spell(player, card, event, 0x81, &td, "Select target non-Legendary creature.", 1,
                         NULL);
  return result;
}

/* ManalinkEh 2022: 0x22cd560; original cards/dominaria.c:937. */
int card_divest(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  ec_definition_t this_definition;
  target_definition_t td;
  test_definition_t this_test;

  if (event != EVENT_CAN_CAST)
  {
    if (event != EVENT_CAST_SPELL)
    {
      if ((event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET) &&
          (event != EVENT_CHANGE_TARGET))
      {
        return 0;
      }
      default_target_definition(player, card, &td, 0);
      td.zone = 0x1000;
      instance = get_card_instance(player, card);
      if (event == EVENT_RESOLVE_SPELL)
      {
        result = valid_target(&td);
        if (result != 0)
        {
          new_default_test_definition(&this_test, 0x42, "Select target artifact or creature.");
          default_ec_definition(instance->targets[0].player, player, &this_definition);
          new_effect_coercion(&this_definition, &this_test);
        }
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_022cd5dd;
    }
    if (card != affected_card)
    {
      return 0;
    }
    if (player != affected_card_controller)
    {
      return 0;
    }
  }
  default_target_definition(player, card, &td, 0);
  td.zone = 0x1000;
  get_card_instance(player, card);
LAB_022cd5dd:
  result = generic_spell(player, card, event, 1, &td, "TARGET_PLAYER", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22cde70; original cards/dominaria.c:1122. */
int card_final_parting(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (event == EVENT_RESOLVE_SPELL)
  {
    new_default_test_definition(&this_test, 0xff, "Select a card to put into your hand.");
    this_test.no_shuffle = 1;
    new_global_tutor(player, player, 1, 1, 0, 1, &this_test);
    new_default_test_definition(&this_test, 0xff, "Select a card to put into your graveyard.");
    new_global_tutor(player, player, 1, 3, 0, 2, &this_test);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22ce170; original cards/dominaria.c:1210. */
int card_garna_the_bloodflame(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  boost_subtype(player, card, event, ~SUBTYPE_INVALID, 0, 0, 0, 4, BCT_CONTROLLER_ONLY);
  if (event == EVENT_ETB)
  {
    return_all_dead_this_turn_to_hand(player, 2);
  }
  if (event != EVENT_CAN_CAST)
  {
    if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
        (player == affected_card_controller))
    {
      ai_modifier = ai_modifier + -0x10;
      return 0;
    }
    return 0;
  }
  return 1;
}

/* ManalinkEh 2022: 0x22ce2b0; original cards/dominaria.c:1261. */
int card_goblin_chainwhirler(int player, int card, event_t event)
{
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  new_damage_all(player, card, 1 - player, 1, 0x23, NULL);
  return 0;
}

/* ManalinkEh 2022: 0x22ce330; original cards/dominaria.c:1280. */
int card_grow_from_the_ashes(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  kicker(player, card, event, 2, 0, 0, 0, 0, 0);
  if (event == EVENT_RESOLVE_SPELL)
  {
    new_default_test_definition(&this_test, 1, "Select a basic land card.");
    this_test.subtype = 0x2000;
    result = kicked(player, card);
    if (result != 0)
    {
      this_test.qty = 2;
    }
    new_global_tutor(player, player, 1, 4, 0, 5, &this_test);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22ca270; original cards/dominaria.c:1340. */
static int give_token_haste_legacy(int player, int card, event_t event)
{
  card_instance_t *instance;
  instance = get_card_instance(player, card);
  if (-1 < instance->damage_target_player)
  {
    instance = get_card_instance(player, card);
    haste((int)instance->damage_target_player, instance->damage_target_card, event);
  }
  return 0;
}

/* ManalinkEh 2022: 0x22c95e0; original cards/dominaria.c:1352. */
static void give_token_haste(token_generation_2022_t *token, int beast, int number)
{
  int card;
  card_instance_t *instance;

  card = create_targetted_legacy_effect(token->s_player, token->s_card, give_token_haste_legacy,
                                        token->t_player, beast);
  instance = get_card_instance(token->t_player, card);
  instance->token_status = instance->token_status | 0x10000;
  return;
}

/* ManalinkEh 2022: 0x22ce740; original cards/dominaria.c:1415. */
int card_invoke_the_divine(int player, int card, event_t event)
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
            gain_life_2022(player, 4);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_022ce7a6;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x44);
  get_card_instance(player, card);
LAB_022ce7a6:
  result = generic_spell(player, card, event, 1, &td, "DISENCHANT", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22ceec0; original cards/dominaria.c:1562. */
int card_josu_vess_lich_knight(int player, int card, event_t event)
{
  int result;
  token_generation_t token;
  check_legend_rule(player, card, event);
  kicker(player, card, event, 5, 1, 0, 0, 0, 0);
  menace(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    result = kicked(player, card);
    if (result != 0)
    {
      default_token_definition(player, card, 0x49af, &token);
      token.s_key_plus = 0x10000;
      token.qty = '\b';
      generate_token(&token);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22cf390; original cards/dominaria.c:1667. */
int card_karns_temporal_sundering(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  target_definition_t td1;
  if (event == EVENT_CAN_CAST)
  {
    result = check_battlefield_for_subtype(player, 2, 0x2001);
    if (result != 0)
    {
      return 1;
    }
    result = check_battlefield_for_subtype(player, 0x100, -1);
    return (unsigned int)(result != 0);
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
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    default_target_definition(player, card, &td1, 0x47);
    td1.illegal_type = 1;
    td1.allow_cancel = 1;
    new_pick_target(&td, "TARGET_PLAYER", 0, 0);
    result = can_target(&td1);
    if (result != 0)
    {
      new_pick_target(&td1, "TARGET_NONLAND_PERMANENT", 1, 0);
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET))
    {
      if (event == EVENT_CHANGE_TARGET)
      {
        default_target_definition(player, card, &td, 0);
        td.zone = 0x1000;
        default_target_definition(player, card, &td1, 0x47);
        td1.illegal_type = 1;
        td1.allow_cancel = 1;
        result = basic_spell(player, card, EVENT_CHANGE_TARGET);
        return result;
      }
      return 0;
    }
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    default_target_definition(player, card, &td1, 0x47);
    td1.illegal_type = 1;
    td1.allow_cancel = 1;
    if (event == EVENT_RESOLVE_SPELL)
    {
      instance = get_card_instance(player, card);
      time_walk_effect(instance->targets[0].player, card);
      result = validate_target(player, card, &td1, 1);
      if (result != 0)
      {
        instance = get_card_instance(player, card);
        result = instance->targets[1].card;
        instance = get_card_instance(player, card);
        bounce_permanent(instance->targets[1].player, result);
      }
      kill_card(player, card, KILL_EXILE);
      result = basic_spell(player, card, EVENT_RESOLVE_SPELL);
      return result;
    }
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22d0610; original cards/dominaria.c:2059. */
int card_lyra_dawnbringer(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  lifelink(player, card, event);
  boost_subtype(player, card, event, SUBTYPE_ANGEL, 1, 1, 0, 2, BCT_CONTROLLER_ONLY);
  return 0;
}

/* ManalinkEh 2022: 0x22c9680; original cards/dominaria.c:2131. */
static void memorial_to_glory_impl(int player, int card, target_definition_2022_t *td)
{
  generate_tokens_by_id_2022(player, card, 0x4986, 2);
  return;
}

/* ManalinkEh 2022: 0x22c94f0; original cards/dominaria.c:2143. */
static void memorial_to_unity_impl(int player, int card, target_definition_2022_t *td)
{
  int amount;
  int revealed_card;
  test_definition_2022_t this_test;

  amount = count_deck(player);
  if (amount != 0)
  {
    if (5 < amount)
    {
      amount = 5;
    }
    new_default_test_definition_2022(&this_test, 2, "Select a creature card.");
    this_test.create_minideck = (unsigned short)amount;
    this_test.no_shuffle = 1;
    revealed_card = new_global_tutor_2022(player, player, 1, 1, 0, 1, &this_test);
    if (-1 < revealed_card)
    {
      amount = amount + -1;
      reveal_card(player, card, player, revealed_card);
    }
    if (0 < amount)
    {
      put_top_x_on_bottom_in_random_order(player, amount);
      return;
    }
  }
  return;
}

/* ManalinkEh 2022: 0x22d0a60; original cards/dominaria.c:2186. */
int card_merfolk_trickster(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  default_target_definition(player, card, &td, 2);
  td.allowed_controller = 1 - player;
  td.allow_cancel = 0;
  instance = get_card_instance(player, card);
  if (event == EVENT_ETB)
  {
    result = can_target(&td);
    if (result != 0)
    {
      result = pick_target(&td, "TARGET_CREATURE_OPPONENT_CONTROLS");
      if (result != 0)
      {
        tap_card(instance->targets[0].player, instance->targets[0].card);
        humiliate_and_set_pt_abilities(player, card, instance->targets[0].player,
                                       instance->targets[0].card, 4, NULL);
      }
    }
  }
  result = flash(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22c96b0; original cards/dominaria.c:2248. */
static int muldrotha_legacy(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      instance = get_card_instance(player, card);
      if (instance->damage_target_player < '\0')
      {
        kill_card(player, card, KILL_EXILE);
      }
      result = check_status(player, card, 0x10000);
      if (result == 0)
      {
        add_status(player, card, 0x10000);
      }
    }
    instance = get_card_instance(player, card);
    if (instance->damage_target_player < '\0')
    {
      return 0;
    }
  }
  else
  {
    instance = get_card_instance(player, card);
    if (instance->damage_target_player < '\0')
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_022c9723;
    }
  }
  result = effect_follows_control_of_attachment(player, card, event);
  if ((result != 0) || (event != EVENT_CLEANUP))
  {
    return 0;
  }
LAB_022c9723:
  instance = get_card_instance(player, card);
  instance->targets[1].player = 0;
  return 0;
}

/* ManalinkEh 2022: 0x22d1880; original cards/dominaria.c:2513. */
int card_niambi_faithful_healer(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  check_legend_rule(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  new_default_test_definition(&test, 0xff, "Select a card named Teferi, Timebender.");
  test.id = 0x485c;
  result = new_special_count_grave(player, &test);
  if ((result != 0) && (result = new_global_tutor(player, player, 2, 1, 0, 5, &test), -1 < result))
  {
    return 0;
  }
  new_global_tutor(player, player, 1, 1, 0, 5, &test);
  return 0;
}

/* ManalinkEh 2022: 0x22d1c40; original cards/dominaria.c:2578. */
int card_on_serras_wings(int player, int card, event_t event)
{
  int result;
  result = generic_aura_2022(player, card, event, player, 1, 1, 0x20, 10, 0x2001, 0, 8);
  return result;
}

/* ManalinkEh 2022: 0x22d2320; original cards/dominaria.c:2694. */
int card_primevals_glorious_rebirth(int player, int card, event_t event)
{
  unsigned int uVar1;
  int result;
  test_definition_t test;
  if (event == EVENT_CAN_CAST)
  {
    result = check_battlefield_for_subtype(player, 2, 0x2001);
    uVar1 = 1;
    if (result == 0)
    {
      result = check_battlefield_for_subtype(player, 0x100, -1);
      uVar1 = (unsigned int)(result != 0);
    }
  }
  else
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      default_test_definition(&test, 0x47);
      test.subtype = 0x2001;
      new_reanimate_all(player, -1, player, &test, REANIMATE_DEFAULT);
      kill_card(player, card, KILL_DESTROY);
    }
    uVar1 = basic_spell(player, card, event);
  }
  return uVar1;
}

/* ManalinkEh 2022: 0x22d25b0; original cards/dominaria.c:2809. */
int card_run_amok(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    goto LAB_022d2624;
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
  LAB_022d26ae:
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
      if (event == EVENT_CHECK_PUMP)
        goto LAB_022d2738;
    }
    else
    {
      if ((event != EVENT_CHANGE_TARGET) && (event != EVENT_CHECK_PUMP))
      {
        return 0;
      }
      default_target_definition(player, card, &td, 2);
      if (event == EVENT_CHECK_PUMP)
      {
      LAB_022d2738:
        td.required_state = 2;
        td.preferred_controller = player;
        instance = get_card_instance(player, card);
        result = has_mana_to_cast_iid(player, EVENT_CAN_CAST, instance->internal_card_id);
        if (result == 0)
        {
          return 0;
        }
        result = can_target(&td);
        if (result == 0)
        {
          return 0;
        }
        pumpable_power[player] = pumpable_power[player] + 3;
        pumpable_toughness[player] = pumpable_toughness[player] + 3;
        return 0;
      }
      if (event == EVENT_CAST_SPELL)
        goto LAB_022d26ae;
    }
    if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
        (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
LAB_022d2624:
  td.required_state = 2;
  td.preferred_controller = player;
  result = vanilla_pump(player, card, event, &td, 3, 3, 0x80, 0);
  return result;
}

/* ManalinkEh 2022: 0x22d27d0; original cards/dominaria.c:2844. */
int card_saproling_migration(int player, int card, event_t event)
{
  int result;
  token_generation_t token;
  kicker(player, card, event, 4, 0, 0, 0, 0, 0);
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
      default_token_definition(player, card, 0x497d, &token);
      result = kicked(player, card);
      token.qty = (-(result == 0) & 0xfeU) + 4;
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

/* ManalinkEh 2022: 0x22d28e0; original cards/dominaria.c:2863. */
int card_seal_away(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  return_from_oblivion(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = 1 - player;
    td.allow_cancel = 0;
    td.required_state = 1;
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "Select target tapped creature an opponent controls.", 0, 0x81);
      if (result != 0)
      {
        instance = get_card_instance(player, card);
        obliviation(player, card, instance->targets[0].player, instance->targets[0].card);
        goto LAB_022d2922;
      }
    }
    if (player == 1)
    {
      ai_modifier = ai_modifier + -0x30;
    }
  }
LAB_022d2922:
  result = flash(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22d2e60; original cards/dominaria.c:2996. */
int card_shivan_fire(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 1 - player;
    result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, "TARGET_CREATURE", 1, NULL);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = 1 - player;
      if ((card == affected_card) && (player == affected_card_controller))
      {
        generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_CREATURE", 1, NULL);
        if (spell_fizzled != 1)
        {
          do_kicker(player, card, 4, 0, 0, 0, 0, 0);
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 1 - player;
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        result = kicked(player, card);
        damage_target0(player, card, (-(unsigned int)(result == 0) & 0xfffffffe) + 4);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td, 2);
  }
  return 0;
}

/* ManalinkEh 2022: 0x22ca9a0; original cards/dominaria.c:3187. */
static int song_of_freyalise_legacy(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  result = permanents_you_control_can_tap_for_mana_all_one_color(player, card, event, TYPE_CREATURE,
                                                                 0, COLOR_TEST_ANY_COLORED, 1);
  if ((event != EVENT_CAN_ACTIVATE) && (result = 0, event == EVENT_CLEANUP))
  {
    instance = get_card_instance(player, card);
    if (instance->targets[2].player < 1)
    {
      kill_card(player, card, KILL_EXILE);
      cant_be_responded_to = 1;
    }
    else
    {
      instance = get_card_instance(player, card);
      instance->targets[2].player = instance->targets[2].player + -1;
    }
  }
  return result;
}

/* ManalinkEh 2022: 0x22d37f0; original cards/dominaria.c:3264. */
int card_squee_the_immortal(int player, int card, event_t event)
{
  int result;
  check_legend_rule(player, card, event);
  if (event == EVENT_GRAVEYARD_ABILITY)
  {
    result = can_sorcery_be_played(player, EVENT_GRAVEYARD_ABILITY);
    if (result != 0)
    {
      result = get_original_internal_card_id(player, card);
      result = has_mana_to_cast_iid(player, EVENT_GRAVEYARD_ABILITY, result);
      if (result != 0)
      {
        return 2;
      }
    }
  }
  else if (event == EVENT_PAY_FLASHBACK_COSTS)
  {
    result = get_id(player, card);
    result = charge_mana_from_id(player, -1, EVENT_PAY_FLASHBACK_COSTS, result);
    return (unsigned int)(result != 0);
  }
  return 0;
}

/* ManalinkEh 2022: 0x22d3a30; original cards/dominaria.c:3319. */
int card_tatyova_benthic_druid(int player, int card, event_t event)
{
  landfall(player, card, event);
  check_legend_rule(player, card, event);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  gain_life_2022(player, 1);
  draw_a_card(player);
  return 0;
}

/* ManalinkEh 2022: 0x22d3aa0; original cards/dominaria.c:3338. */
int card_teferi_hero_of_dominaria_emblem(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  result = card_drawn_trigger(player, card, event, player, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  default_target_definition(player, card, &td, 0x47);
  td.allowed_controller = 1 - player;
  td.allow_cancel = 0;
  instance = get_card_instance(player, card);
  result = can_target(&td);
  if (result != 0)
  {
    result = new_pick_target(&td, "TARGET_PERMANENT", 0, 0);
    if (result != 0)
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x22caa60; original cards/dominaria.c:3358. */
static int teferi_hero_of_dominaria_legacy(int player, int card, event_t event)
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
        untap_lands(player, card, 2);
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

/* ManalinkEh 2022: 0x22d43f0; original cards/dominaria.c:3593. */
int card_thallid_soothsayer(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    draw_a_card(player);
  }
  result = generic_activated_ability(player, card, event, 0x200, 2, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22ca150; original cards/dominaria.c:3820. */
static int tiana_reanimation_legacy(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  result = duh_mode(player);
  result = eot_trigger_mode(player, card, event, 2, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (result != 0)
  {
    seek_grave_for_id_to_reanimate(player, card, instance->targets[0].player,
                                   (unsigned int)cards_data[instance->targets[0].card].id,
                                   REANIMATEXTRA_RETURN_TO_HAND2);
    kill_card(player, card, KILL_EXILE);
  }
  if (event == EVENT_CAN_SKIP_TURN)
  {
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x22ca230; original cards/dominaria.c:4107. */
static char *has_highest_power(int who_chooses, int player, int card, int targeting_player,
                               int targeting_card)
{
  int iVar1;
  int iVar2;
  char *pcVar3;
  iVar1 = get_power(player, card);
  iVar2 = get_highest_power(player);
  pcVar3 = NULL;
  if (iVar1 != iVar2)
  {
    pcVar3 = "must have the greatest power among creatures you control";
  }
  return pcVar3;
}

/* ManalinkEh 2022: 0x22d58d0; original cards/dominaria.c:4156. */
int card_unwind(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  counterspell_target_definition(player, card, &td, 0xff);
  td.illegal_type = 2;
  if (event == EVENT_RESOLVE_SPELL)
  {
    counterspell(player, card, EVENT_RESOLVE_SPELL, &td, 0);
    untap_lands(player, card, 3);
  }
  result = generic_spell(player, card, event, 2, &td, NULL, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22d5980; original cards/dominaria.c:4172. */
int card_urgoros_the_empty_one(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  unsigned int uVar3;
  unsigned int uVar4;
  int iVar5;
  int amount;
  unsigned int uVar6;
  int iStack_14;
  check_legend_rule(player, card, event);
  iVar1 =
      damage_dealt_by_me(player, card, event, DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER);
  if (iVar1 == 0)
  {
    return 0;
  }
  instance = get_card_instance(player, card);
  iVar1 = hand_count[0];
  iStack_14 = 0;
  uVar6 = instance->targets[1].player;
  instance->targets[1].player = 0;
  uVar4 = uVar6 >> 8 & 0xff;
  amount = 0;
  uVar6 = uVar6 & 0xff;
  if (uVar6 != 0)
  {
    uVar3 = 0;
    do
    {
      while (iVar1 == 0)
      {
        uVar3 = uVar3 + 1;
        amount = amount + 1;
        if (uVar6 == uVar3)
          goto LAB_022d5a1b;
      }
      uVar3 = uVar3 + 1;
      iStack_14 = iStack_14 + 1;
      iVar1 = iVar1 + -1;
    } while (uVar6 != uVar3);
  }
LAB_022d5a1b:
  iVar1 = 0;
  if (uVar4 != 0)
  {
    uVar6 = 0;
    iVar5 = hand_count[1];
    do
    {
      while (iVar5 == 0)
      {
        uVar6 = uVar6 + 1;
        amount = amount + 1;
        if (uVar4 == uVar6)
          goto LAB_022d5a39;
      }
      uVar6 = uVar6 + 1;
      iVar1 = iVar1 + 1;
      iVar5 = iVar5 + -1;
    } while (uVar4 != uVar6);
  }
LAB_022d5a39:
  new_multidiscard(0, iStack_14, 1, player);
  new_multidiscard(1, iVar1, 1, player);
  draw_cards(player, amount);
  return 0;
}

/* ManalinkEh 2022: 0x22c94c0; original cards/dominaria.c:4212. */
static int is_not_legendary_test(int unused1, int unused2, int player, int card)
{
  int result;
  result = is_legendary(player, card);
  return (unsigned int)(result == 0);
}

/* ManalinkEh 2022: 0x22d5c80; original cards/dominaria.c:4267. */
int card_verix_bladewing(int player, int card, event_t event)
{
  int result;
  check_legend_rule(player, card, event);
  kicker(player, card, event, 3, 0, 0, 0, 0, 0);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    result = kicked(player, card);
    if (result != 0)
    {
      generate_token_by_id(player, card, 0x4954);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22d5ef0; original cards/dominaria.c:4349. */
int card_whisper_blood_liturgist(int player, int card, event_t event)
{
  unsigned int uVar1;
  unsigned int uVar2;
  int result;
  test_definition_t test;
  test_definition_t sac_test;
  check_legend_rule(player, card, event);
  if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition(&test, 2, "Select target creature card to reanimate.");
  LAB_022d5f47:
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      return 0;
    }
    result = validate_target_from_grave_source(player, card, player, 0);
    if (result == -1)
    {
      return 0;
    }
    reanimate_permanent(player, card, player, result, REANIMATE_DEFAULT);
    return 0;
  }
  new_default_test_definition(&test, 2, "Select target creature card to reanimate.");
  if (event == EVENT_CAN_ACTIVATE)
  {
    result = can_sacrifice_type_as_cost(player, 2, TYPE_CREATURE);
    if (result == 0)
    {
      return 0;
    }
    result = generic_activated_ability_with_target_in_grave(player, card, EVENT_CAN_ACTIVATE,
                                                            GAA_UNTAPPED, 0, 0, 0, 0, 0, 0, 0, NULL,
                                                            NULL, TYPE_NONE, &test, 8);
    return (unsigned int)(result != 0);
  }
  if (event != EVENT_ACTIVATE)
    goto LAB_022d5f47;
  new_default_test_definition(&sac_test, 2, "Select a creature to sacrifice.");
  uVar1 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                        &sac_test);
  if (uVar1 == 0)
  {
    uVar2 = 0xffffffff;
  }
  else
  {
    uVar2 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                          &sac_test);
    if (uVar2 != 0)
      goto LAB_022d6000;
  }
  spell_fizzled = 1;
LAB_022d6000:
  if (-1 < (int)uVar1)
  {
    state_untargettable((int)uVar1 >> 0x10 & 0xff, uVar1 >> 0x18, 0);
  }
  if (-1 < (int)uVar2)
  {
    state_untargettable((int)uVar2 >> 0x10 & 0xff, uVar2 >> 0x18, 0);
  }
  generic_activated_ability_with_target_in_grave(player, card, EVENT_ACTIVATE, GAA_UNTAPPED, 0, 0,
                                                 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test, 8);
  if (spell_fizzled != 1)
  {
    kill_card((int)uVar1 >> 0x10 & 0xff, uVar1 >> 0x18, KILL_SACRIFICE);
    kill_card((int)uVar2 >> 0x10 & 0xff, uVar2 >> 0x18, KILL_SACRIFICE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x22d62a0; original cards/dominaria.c:4426. */
int card_wizards_retort(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_MODIFY_COST)
  {
    result = check_battlefield_for_subtype(player, -1, 0xd6);
    if (result != 0)
    {
      COST_COLORLESS = COST_COLORLESS + -1;
    }
  }
  result = counterspell(player, card, event, NULL, 0);
  return result;
}

/* ManalinkEh 2022: 0x22d6300; original cards/dominaria.c:4442. */
int card_yavimaya_sapherd(int player, int card, event_t event)
{
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  generate_token_by_id(player, card, 0x497d);
  return 0;
}

/* ManalinkEh 2022: 0x22d6360; original cards/dominaria.c:4455. */
int card_yawgmoths_vile_offering(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  test_definition_t test;

  if (event == EVENT_CAN_CAST)
  {
    result = check_battlefield_for_subtype(player, 2, 0x2001);
    if (result != 0)
    {
      return 1;
    }
    result = check_battlefield_for_subtype(player, 0x100, -1);
    return (unsigned int)(result != 0);
  }
  if (event != EVENT_CAST_SPELL)
  {
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      new_default_test_definition(&test, 0x102, "Select target creature or planeswalker card.");
      default_target_definition(player, card, &td, 0x102);
      instance = get_card_instance(player, card);
      if (event == EVENT_RESOLVE_SPELL)
      {
        result = validate_target_from_grave_source(player, card, instance->targets[1].player, 2);
        if (result != -1)
        {
          reanimate_permanent(player, card, instance->targets[1].player, result, REANIMATE_DEFAULT);
        }
        result = validate_target(player, card, &td, 0);
        if (result != 0)
        {
          kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
        }
        kill_card(player, card, KILL_EXILE);
      }
    }
    else
    {
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
      new_default_test_definition(&test, 0x102, "Select target creature or planeswalker card.");
      default_target_definition(player, card, &td, 0x102);
      get_card_instance(player, card);
    }
    goto LAB_022d63ed;
  }
  if (card != affected_card)
  {
    return 0;
  }
  if (player != affected_card_controller)
  {
    return 0;
  }
  new_default_test_definition(&test, 0x102, "Select target creature or planeswalker card.");
  default_target_definition(player, card, &td, 0x102);
  get_card_instance(player, card);
  result = new_special_count_grave(player, &test);
  if ((result == 0) || (result = graveyard_has_shroud(player), result != 0))
  {
    result = new_special_count_grave(1 - player, &test);
    if ((result != 0) && (result = graveyard_has_shroud(1 - player), result == 0))
      goto LAB_022d669e;
  }
  else
  {
  LAB_022d669e:
    select_target_from_either_grave(player, card, SFG_NO_SPELL_FIZZLED, 2, 2, &test, 1, 2);
  }
  result = generic_spell(player, card, EVENT_CAN_CAST, 0x20, &td, "TARGET_CREATURE_OR_PLANESWALKER",
                         1, NULL);
  if (result != 0)
  {
    generic_spell(player, card, EVENT_CAST_SPELL, 0x20, &td, "TARGET_CREATURE_OR_PLANESWALKER", 1,
                  NULL);
  }
LAB_022d63ed:
  result = basic_spell(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22cd9d0; original cards/dominaria.c:1061. */
int card_fight_with_fire(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *pcVar2;
  int iVar3;
  card_instance_t *pcVar4;
  target_t *ptVar5;
  target_definition_t td;
  target_definition_t tdk;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 2;
    default_target_definition(player, card, &tdk, 0x102);
    tdk.zone = 0x1200;
    get_card_instance(player, card);
    iVar1 = has_mana_multi(player, 7, 0, 0, 0, 2, 0);
    if ((iVar1 != 0) &&
        (iVar1 = generic_spell(player, card, EVENT_CAN_CAST, 1, &tdk, "TARGET_ANY", 1, NULL),
         iVar1 != 0))
    {
      return 1;
    }
    iVar1 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, "TARGET_CREATURE", 1, NULL);
    return iVar1;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = 2;
      default_target_definition(player, card, &tdk, 0x102);
      tdk.zone = 0x1200;
      pcVar2 = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        iVar1 = do_kicker(player, card, 5, 0, 0, 0, 1, 0);
        if (iVar1 == 0)
        {
          generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_CREATURE", 1, NULL);
        }
        else
        {
          iVar1 = 0;
          pcVar2->number_of_targets = '\0';
          while (iVar3 = new_pick_target(&tdk, "TARGET_ANY", iVar1, 1), iVar3 != 0)
          {
            iVar3 = pcVar2->targets[iVar1].card;
            if (iVar3 != -1)
            {
              pcVar4 = get_card_instance(pcVar2->targets[iVar1].player, iVar3);
              pcVar4->state = pcVar4->state | 0x200000;
            }
            iVar1 = iVar1 + 1;
            if (iVar1 == 10)
            {
              ptVar5 = pcVar2->targets;
              do
              {
                if (ptVar5->card != -1)
                {
                  pcVar4 = get_card_instance(ptVar5->player, ptVar5->card);
                  pcVar4->state = pcVar4->state & 0xffdfffff;
                }
                ptVar5 = ptVar5 + 1;
              } while (pcVar2->targets + 10 != ptVar5);
              iVar1 = basic_spell(player, card, EVENT_CAST_SPELL);
              return iVar1;
            }
          }
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 2;
    default_target_definition(player, card, &tdk, 0x102);
    tdk.zone = 0x1200;
    get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      iVar1 = kicked(player, card);
      if (iVar1 == 0)
      {
        iVar1 = valid_target(&td);
        if (iVar1 != 0)
        {
          damage_target0(player, card, 5);
        }
      }
      else
      {
        divide_damage(player, card, &tdk);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 2;
    default_target_definition(player, card, &tdk, 0x102);
    tdk.zone = 0x1200;
    get_card_instance(player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x22ce8a0; original cards/dominaria.c:1449. */
int card_jayas_immolating_inferno(int player, int card, event_t event)
{
  target_t *ptVar1;
  target_t *ptVar2;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
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
        default_target_definition(player, card, &td, 0x102);
        td.zone = 0x1200;
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
                  goto LAB_022ce980;
              }
              ptVar2 = instance->targets + iVar5;
              ptVar1 = instance->targets + iVar5;
              iVar5 = iVar5 + 1;
              damage_creature(ptVar1->player, ptVar2->card, instance->info_slot, player, card);
            } while (iVar5 < (int)(unsigned int)instance->number_of_targets);
          }
        LAB_022ce980:
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_022ce9e4;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x102);
  td.zone = 0x1200;
  get_card_instance(player, card);
LAB_022ce9e4:
  ptStack_68 = &td;
  iVar5 = generic_spell(player, card, event, 0x24, ptStack_68, "TARGET_ANY", 3, NULL);
  return iVar5;
}

/* ManalinkEh 2022: 0x22d16e0; original cards/dominaria.c:2475. */
int card_naru_meha_master_wizard(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  check_legend_rule(player, card, event);
  instance = get_card_instance(player, card);
  counterspell_target_definition(player, card, &td, 0x18);
  td.allowed_controller = player;
  if (event == EVENT_CAN_CAST)
  {
    result = twincast(player, card, 0x74, &td, NULL);
    if (result == 0)
    {
      instance->info_slot = 0;
      return 1;
    }
    instance->info_slot = 1;
    result = 99;
  }
  else
  {
    if (event == EVENT_CAST_SPELL)
    {
      if (((card == affected_card) && (player == affected_card_controller)) &&
          (instance->info_slot == 1))
      {
        result = counterspell(player, card, EVENT_CAST_SPELL, &td, 0);
        return result;
      }
      etb_ability_2022(player, card, EVENT_CAST_SPELL, RESOLVE_TRIGGER_MANDATORY);
    }
    else
    {
      etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
      if (((event == EVENT_ETB) &&
           (result = counterspell_validate(player, card, &td, 0), result != 0)) &&
          (instance->info_slot == 1))
      {
        copy_spell_from_stack(player, instance->targets[0].player, instance->targets[0].card);
        return 0;
      }
    }
    result = 0;
  }
  return result;
}

/* ManalinkEh 2022: 0x22c9d50; original cards/dominaria.c:3711. */
static int flame_of_keld_legacy(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  unsigned int uVar3;
  int result;

  pcVar1 = damage_being_dealt(event);
  pcVar2 = get_card_instance(player, card);
  if (pcVar1 != NULL)
  {
    uVar3 = get_color((int)pcVar1->damage_source_player, pcVar1->damage_source_card);
    if ((pcVar2->targets[2].card & uVar3) != 0)
    {
      pcVar1->info_slot_fn = pcVar1->info_slot_fn + 2;
    }
  }
  result = eot_trigger(player, card, event);
  if (result == 0)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22c9630; original cards/dominaria.c:2103. */
static void memorial_to_folly_impl(int player, int card, target_definition_2022_t *td)
{
  int grave_position;

  grave_position = validate_target_from_grave_source(player, card, player, 0);
  if (grave_position == -1)
  {
    return;
  }
  from_grave_to_hand(player, grave_position, 1);
  return;
}

/* ManalinkEh 2022: 0x22c95d0; original cards/dominaria.c:2119. */
static void memorial_to_genius_impl(int player, int card, target_definition_2022_t *td)
{
  draw_cards(player, 2);
  return;
}

/* ManalinkEh 2022: 0x22ca2c0; original cards/dominaria.c:2169. */
static void memorial_to_war_impl(int player, int card, target_definition_2022_t *td)
{
  int result;
  card_instance_t *instance;

  result = valid_target_2022(td);
  if (result == 0)
  {
    return;
  }
  instance = get_card_instance(player, card);
  result = instance->targets[0].card;
  instance = get_card_instance(player, card);
  kill_card(instance->targets[0].player, result, KILL_DESTROY);
  return;
}

/* ManalinkEh 2022: 0x22d0b60; original cards/dominaria.c:2217. */
int card_mox_amber(int player, int card, event_t event)
{
  int result;
  unsigned int uVar2;

  if (event != EVENT_RESOLVE_SPELL)
  {
    result = mana_producer(player, card, event);
    return result;
  }

  result = get_id(player, card);
  uVar2 = get_color_of_mana_produced_by_id(result, 0x3e, player);
  play_land_sound_effect_force_color(player, card, uVar2 & (int)(uVar2 ^ 0xffffffff) >> 0x1f);
  return 0;
}

/* ManalinkEh 2022: 0x22d6240; original cards/dominaria.c:4413. */
int card_wizards_lightning(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_MODIFY_COST)
  {
    result = check_battlefield_for_subtype(player, -1, 0xd6);
    if (result != 0)
    {
      COST_COLORLESS = COST_COLORLESS + -2;
    }
  }
  result = card_lightning_bolt(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22ca380; original cards/dominaria.c:7. */
static int is_historic(int iid, int unused, int player, int card)
{
  int result;

  result = is_what(player, card, 0x40);
  if (result == 0)
  {
    result = is_legendary(player, card);
    if (result == 0)
    {
      result = has_subtype(player, card, SUBTYPE_SAGA);
      return (unsigned int)(result != 0);
    }
  }
  return 1;
}

/* ManalinkEh 2022: 0x22cbc60; original cards/dominaria.c:428. */
int card_blackblade_reforged(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int result;
  card_instance_t *pcVar3;
  int p_mod;
  unsigned int uVar4;
  unsigned int uVar5;
  const char *prompt;
  target_definition_t *ptStack_c4;
  target_definition_t *ptStack_bc;
  target_definition_t td;
  target_definition_t td1;

  check_legend_rule(player, card, event);
  pcVar1 = get_card_instance(player, card);
  if (event - EVENT_POWER < 2)
  {
    pcVar3 = in_play(player, card);
    if ((pcVar3 != NULL) && (pcVar3 = is_equipping(player, card), pcVar3 != NULL))
    {
      result = count_subtype(player, 1, -1);
      p_mod = count_subtype(player, 1, -1);
      modify_pt_and_abilities((int)pcVar1->damage_target_player, pcVar1->damage_target_card, event,
                              p_mod, result, 0);
    }
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    default_target_definition(player, card, &td1, 2);
    td1.required_subtype = 0x2001;
    td1.allowed_controller = player;
    td1.preferred_controller = player;
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x100, 7, 0, 0, 0, 0, 0,
                                         0, &td, NULL);
      if (result != 0)
      {
        return 1;
      }
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x100, 3, 0, 0, 0, 0, 0,
                                         0, &td1, NULL);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {

      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 7, 0, 0, 0, 0, 0, 0,
                                         &td, NULL);
      uVar5 = 0;
      if (result != 0)
      {
        result = can_sorcery_be_played(player, EVENT_ACTIVATE);
        uVar5 = (unsigned int)(result != 0);
      }
      ptStack_bc = &td;
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 3, 0, 0, 0, 0, 0, 0,
                                         &td1, NULL);
      uVar4 = 0;
      if (result != 0)
      {
        result = can_sorcery_be_played(player, EVENT_ACTIVATE);
        uVar4 = (unsigned int)(result != 0);
      }
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "Normal equip",
                           uVar5, 1, "Legendary equip", uVar4, 5, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (result == 1)
      {
        prompt = "Select target creature you control.";
        result = 7;
      LAB_022cbfc2:
        result = generic_activated_ability(player, card, EVENT_ACTIVATE, 0x102, result, 0, 0, 0, 0,
                                           0, 0, ptStack_bc, prompt);
        return result;
      }
      if (result == 2)
      {
        prompt = "Select target legendary creature you control.";
        result = 3;
        ptStack_bc = &td1;
        goto LAB_022cbfc2;
      }
      goto LAB_022cbd2a;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    default_target_definition(player, card, &td1, 2);
  }
  ptStack_bc = &td;
  ptStack_c4 = &td1;
  td1.allowed_controller = player;
  td1.required_subtype = 0x2001;
  td1.preferred_controller = td1.allowed_controller;
  if (event != EVENT_RESOLVE_ACTIVATION)
    goto LAB_022cbd2a;
  result = pcVar1->info_slot;
  if (result == 1)
  {
    result = validate_target(player, card, ptStack_bc, 0);
    if (result == 0)
    {
      result = pcVar1->info_slot;
      goto LAB_022cc16a;
    }
  }
  else
  {
  LAB_022cc16a:
    if ((result != 2) || (result = validate_target(player, card, ptStack_c4, 0), result == 0))
      goto LAB_022cbd2a;
  }
  equip_target_creature(pcVar1->parent_controller, pcVar1->parent_card, pcVar1->targets[0].player,
                        pcVar1->targets[0].card);
LAB_022cbd2a:
  ptStack_bc = &td;
  result = generic_activated_ability(player, card, event, 0x102, 7, 0, 0, 0, 0, 0, 0, ptStack_bc,
                                     "Select target creature you control.");
  return result;
}

/* ManalinkEh 2022: 0x22cd150; original cards/dominaria.c:804. */
int card_danitha_capashen_paragon(int player, int card, event_t event)
{
  int result;

  check_legend_rule(player, card, event);
  vigilance_2022(player, card, event);
  lifelink(player, card, event);
  if ((((event == EVENT_MODIFY_COST_GLOBAL) && (affected_card_controller == player)) &&
       (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)) &&
      ((result = has_subtype(affected_card_controller, affected_card, SUBTYPE_EQUIPMENT),
        result != 0 || (result = has_subtype(affected_card_controller, affected_card, SUBTYPE_AURA),
                        result != 0))))
  {
    COST_COLORLESS = COST_COLORLESS + -1;
  }
  return 0;
}

/* ManalinkEh 2022: 0x22cd220; original cards/dominaria.c:838. */
int card_dauntless_bodyguard(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_RESOLVE_SPELL)
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    td.special = 0x4000;
    td.allow_cancel = 0;
    td.illegal_abilities = 0;
    result = can_target(&td);
    if (result != 0)
    {
      new_pick_target(&td, "TARGET_ANOTHER_CREATURE_CONTROL", 9, 1);
    }
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_022cd2d9:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_022cd2e2;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_022cd2d9;
  }

  instance = get_card_instance(player, card);
  result = instance->targets[9].player;
  instance = get_card_instance(player, card);
  card_00 = instance->targets[9].card;
  instance = in_play(result, card_00);
  if (instance != NULL)
  {
    pump_ability_until_eot_2022(player, card, result, card_00, 0, 0, 0, 0x800);
  }
LAB_022cd2e2:
  result = generic_activated_ability(player, card, event, 8, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22cd720; original cards/dominaria.c:989. */
int card_evra_halcyon_witness(int player, int card, event_t event)
{
  int result;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;

  check_legend_rule(player, card, event);
  if (event == EVENT_POWER)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (pcVar2 = get_card_instance(player, card), -1 < pcVar2->targets[1].card))
    {
      pcVar2 = get_card_instance(player, card);
      event_result = pcVar2->targets[1].card;
    }
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_022cd755:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_022cd75a;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_022cd755;
  }

  pcVar2 = get_card_instance(player, card);
  pcVar3 = in_play(pcVar2->parent_controller, pcVar2->parent_card);
  if (pcVar3 != NULL)
  {

    result = get_power(pcVar2->parent_controller, pcVar2->parent_card);
    pcVar3->targets[1].card = life[player];
    set_life_total(player, result);
  }
LAB_022cd75a:
  result = generic_activated_ability(player, card, event, 0, 4, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22cdf60; original cards/dominaria.c:1144. */
int card_firesong_and_sunspeaker(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *pcVar1;
  int result;
  unsigned int uVar3;
  unsigned int uVar4;
  card_instance_t *pcVar5;
  target_definition_t td;

  check_legend_rule(player, card, event);
  pcVar1 = in_play(player, card);
  if (event == EVENT_CAST_SPELL)
  {
    result = is_what(affected_card_controller, affected_card, 0x38);
    if (result != 0)
    {
      uVar3 = get_color(affected_card_controller, affected_card);
      uVar4 = get_sleighted_color_test(player, card, 0x20);
      if ((uVar3 & uVar4) != 0)
      {
        pcVar1->targets[9].player = affected_card_controller;
        pcVar1->targets[9].card = affected_card;
      }
    }
    result = trigger_gain_life_2022(player, card, EVENT_CAST_SPELL, player, RESOLVE_TRIGGER_MANDATORY);
    if (result == 0)
    {
      return 0;
    }
  }
  else
  {
    result = trigger_gain_life_2022(player, card, event, player, RESOLVE_TRIGGER_MANDATORY);
    if (result == 0)
      goto LAB_022cdfbd;
  }
  result = pcVar1->targets[9].player;

  if ((((-1 < result) && (card_00 = pcVar1->targets[9].card, card_00 != -1)) &&
       (pcVar5 = in_play(result, card_00), pcVar5 != NULL)) &&
      ((pcVar5->state & 2) != 0))
  {
    default_target_definition(player, card, &td, 0xff);
    td.illegal_type = 0x100;
    td.zone = 0x1200;
    td.allow_cancel = 0;
    result = can_target(&td);
    if ((result != 0) && (result = pick_target(&td, "TARGET_CREATURE_OR_PLAYER"), result != 0))
    {
      damage_target0(player, card, 3);
    }
    pcVar1->targets[9].player = -1;
    pcVar1->targets[9].card = -1;
  }
LAB_022cdfbd:
  if ((event == EVENT_STATIC_EFFECTS) && (stack_size == 0))
  {
    pcVar1->targets[9].player = -1;
    pcVar1->targets[9].card = -1;
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x22ce220; original cards/dominaria.c:1236. */
int card_ghitu_lavarunner(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if ((event - EVENT_POWER & 0xfffffffd) == 0)
  {
    instance = in_play(player, card);
    if (((instance != NULL) && (card == affected_card)) && (player == affected_card_controller))
    {
      result = count_graveyard_by_type(player, 0x38);
      if (1 < result)
      {
        if (event == EVENT_POWER)
        {
          event_result = event_result + 1;
        }
        else
        {
          haste(player, card, EVENT_ABILITIES);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22ce440; original cards/dominaria.c:1302. */
int card_grunn_the_lonely_king(int player, int card, event_t event)
{
  int result;
  int power;

  check_legend_rule(player, card, event);
  kicker(player, card, event, 3, 0, 0, 0, 0, 0);
  result = kicked(player, card);
  if (result != 0)
  {
    enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1, 5);
  }
  dat_ability(player, card, event, DAT_ATTACKS_ALONE | DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }
  result = get_toughness(player, card);
  power = get_power(player, card);
  pump_until_eot_merge_previous_2022(player, card, player, card, power, result);
  return 0;
}

/* ManalinkEh 2022: 0x22ce6c0; original cards/dominaria.c:1400. */
int card_in_bolass_clutches(int player, int card, event_t event)
{
  int result;
  target_definition_t td;

  check_legend_rule(player, card, event);
  default_target_definition(player, card, &td, 0x47);
  attached_is_additional_subtype(player, card, event, SUBTYPE_LEGEND);
  result = generic_stealing_aura(player, card, event, &td, "TARGET_PERMANENT");
  return result;
}

/* ManalinkEh 2022: 0x22cea50; original cards/dominaria.c:1476. */
int card_jhoiras_familiar(int player, int card, event_t event)
{
  int result;

  if ((((event == EVENT_MODIFY_COST_GLOBAL) && (affected_card_controller == player)) &&
       (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)) &&
      (((result = is_what(affected_card_controller, affected_card, 0x40),
         result != 0 ||
             (result = is_legendary(affected_card_controller, affected_card), result != 0)) ||
        (result = has_subtype(affected_card_controller, affected_card, SUBTYPE_SAGA),
         result != 0))))
  {
    COST_COLORLESS = COST_COLORLESS + -1;
  }
  return 0;
}

/* ManalinkEh 2022: 0x22ca320; original cards/dominaria.c:1594. */
static int is_land_or_legendary_by_internal_id(int iid, int unused, int unused1, int unused2)
{
  int iVar1;
  int iVar2;

  iVar1 = 1;
  if ((cards_data[iid].type & 1) == 0)
  {
    iVar2 = is_legendary(-1, iid);
    iVar1 = 0;
    if (iVar2 != 0)
    {
      return (unsigned int)((cards_data[iid].type & 0x47) != 0);
    }
  }
  return iVar1;
}

/* ManalinkEh 2022: 0x22d0040; original cards/dominaria.c:1932. */
int card_kwende_pride_of_femeref(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  check_legend_rule(player, card, event);
  if (event == EVENT_ABILITIES)
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if ((result == 0) && (affected_card_controller == player))
      {
        result = is_what(player, affected_card, 2);
        if (result != 0)
        {
          result = check_for_ability(affected_card_controller, affected_card, 0x100);
          if (result != 0)
          {
            event_result = event_result | 0x40000000;
          }
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22d1350; original cards/dominaria.c:2402. */
int card_multani_yavimayas_avatar(int player, int card, event_t event)
{
  target_t *ptVar1;
  target_t *ptVar2;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
  int iVar6;
  bool bVar7;
  int to_bounce[2];
  char msg[50];
  target_definition_t td;

  check_legend_rule(player, card, event);
  if (event - EVENT_POWER < 2)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        ((player == -1 ||
          (((-1 < player && (instance = in_play(player, card), instance != NULL)) &&
            (iVar6 = is_humiliated_for_ability(player, card, AL_STATIC), iVar6 == 0))))))
    {
      iVar6 = count_graveyard_by_type(player, 1);
      event_result = event_result + iVar6 + landsofcolor_controlled[player][7];
    }
    default_target_definition(player, card, &td, 1);
    td.allowed_controller = player;
    td.preferred_controller = player;
    td.illegal_abilities = 0;
    get_card_instance(player, card);
  }
  else
  {
    default_target_definition(player, card, &td, 1);
    td.allowed_controller = player;
    td.preferred_controller = player;
    td.illegal_abilities = 0;
    instance = get_card_instance(player, card);
    if (event == EVENT_GRAVEYARD_ABILITY)
    {
      iVar6 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 1, 0, 0, 1, 0, 0, 0,
                                        NULL, NULL);
      if ((iVar6 != 0) && (iVar6 = target_available(player, card, &td), 1 < iVar6))
      {
        return 4;
      }
    }
    else if (event == EVENT_PAY_FLASHBACK_COSTS)
    {

      charge_mana_for_activated_ability_2022(player, card, 1, 0, 0, 1, 0, 0);
      iVar6 = 0;
      do
      {
        iVar5 = iVar6 + 1;
        scnprintf(msg, 0x32, " Select a land you control (%d of 2)", iVar5);
        iVar4 = new_pick_target(&td, msg, iVar6, 0x80);
        if (iVar4 == 0)
          break;
        iVar4 = instance->targets[iVar6].card;
        *(int *)(msg + iVar5 * 4 + -0xe) = iVar4;
        state_untargettable(instance->targets[iVar6].player, iVar4, 1);
        iVar6 = iVar5;
      } while (iVar5 != 2);
      if (instance->number_of_targets != '\0')
      {
        iVar6 = 0;
        do
        {
          ptVar2 = instance->targets + iVar6;
          ptVar1 = instance->targets + iVar6;
          iVar6 = iVar6 + 1;
          state_untargettable(ptVar1->player, ptVar2->card, 0);
        } while (iVar6 < (int)(unsigned int)instance->number_of_targets);
        if (1 < instance->number_of_targets)
        {
          bVar7 = spell_fizzled == 1;
          instance->number_of_targets = '\0';
          if (bVar7)
          {
            return 0;
          }
          bounce_permanent(player, to_bounce[0]);
          bounce_permanent(player, to_bounce[1]);
          return 1;
        }
      }
      spell_fizzled = 1;
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22d33d0; original cards/dominaria.c:3154. */
int card_slinn_voda_the_rising_deep(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int (*paiVar4)[151];
  int card_00;
  unsigned char bVar5;
  int marked[2][151];

  bVar5 = 0;
  check_legend_rule(player, card, event);
  kicker(player, card, event, 1, 0, 1, 0, 0, 0);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if ((event == EVENT_ETB) && (iVar1 = kicked(player, card), iVar1 != 0))
  {

    paiVar4 = marked;
    for (iVar1 = 0x12e; iVar1 != 0; iVar1 = iVar1 + -1)
    {
      (*paiVar4)[0] = 0;
      paiVar4 = (int (*)[151])((int)paiVar4 + ((unsigned int)bVar5 * -2 + 1) * 4);
    }
    iVar1 = 0;
    do
    {
      if (0 < active_cards_count[iVar1])
      {
        card_00 = 0;
        do
        {
          instance = in_play(iVar1, card_00);
          if ((((instance != NULL) && (iVar3 = is_what(iVar1, card_00, 2), iVar3 != 0)) &&
               (iVar3 = has_subtype(iVar1, card_00, SUBTYPE_MERFOLK), iVar3 == 0)) &&
              (((iVar3 = has_subtype(iVar1, card_00, SUBTYPE_KRAKEN),
                 iVar3 == 0 &&
                     (iVar3 = has_subtype(iVar1, card_00, SUBTYPE_LEVIATHAN), iVar3 == 0)) &&
                ((iVar3 = has_subtype(iVar1, card_00, SUBTYPE_OCTOPUS),
                  iVar3 == 0 &&
                      (iVar3 = has_subtype(iVar1, card_00, SUBTYPE_SERPENT), iVar3 == 0))))))
          {
            marked[iVar1][card_00] = 1;
          }
          card_00 = card_00 + 1;
        } while (card_00 < active_cards_count[iVar1]);
      }
      iVar1 = iVar1 + 1;
    } while (iVar1 != 2);
    iVar1 = 0;
    if (0 < active_cards_count[0])
    {
      do
      {
        if (marked[0][iVar1] != 0)
        {
          bounce_permanent(0, iVar1);
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < active_cards_count[0]);
    }
    iVar1 = 0;
    if (0 < active_cards_count[1])
    {
      do
      {
        if (marked[1][iVar1] != 0)
        {
          bounce_permanent(1, iVar1);
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < active_cards_count[1]);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22d3750; original cards/dominaria.c:3247. */
int card_sporecrown_thallid(int player, int card, event_t event)
{
  int result;

  if ((((event - EVENT_POWER < 2) && (affected_card_controller == player)) &&
       (card != affected_card)) &&
      (((result = has_subtype(player, affected_card, SUBTYPE_FUNGUS),
         result != 0 ||
             (result = has_subtype(affected_card_controller, affected_card, SUBTYPE_SAPROLING),
              result != 0)) &&
        (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0))))
  {
    event_result = event_result + 1;
  }
  return 0;
}

/* ManalinkEh 2022: 0x22d38b0; original cards/dominaria.c:3292. */
int card_sylvan_awakening(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;

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

      if (0 < active_cards_count[player])
      {
        iVar3 = 0;
        do
        {
          while ((instance = in_play(player, iVar3),
                  instance == NULL || (iVar2 = is_what(player, iVar3, 1), iVar2 == 0)))
          {
            iVar3 = iVar3 + 1;
            if (active_cards_count[player] <= iVar3)
              goto LAB_022d39c8;
          }

          iVar2 = turn_into_creature(player, card, player, iVar3, 0x20, 2, 2);
          instance = get_card_instance(player, iVar2);
          iVar3 = iVar3 + 1;
          instance->targets[8].player = 0x400;
          instance = get_card_instance(player, iVar2);
          instance->targets[8].card = 0x804;
          instance = get_card_instance(player, iVar2);
          iVar2 = active_cards_count[player];
          instance->targets[9].card = 0x3a;
        } while (iVar3 < iVar2);
      }
    LAB_022d39c8:
      kill_card(player, card, KILL_DESTROY);
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  iVar3 = basic_spell(player, card, event);
  return iVar3;
}

/* ManalinkEh 2022: 0x22d3b80; original cards/dominaria.c:3375. */
int card_teferi_hero_of_dominaria(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if ((event - EVENT_RESOLVE_ACTIVATION < 2) || (event == EVENT_ACTIVATE))
  {

    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x47);
    td.illegal_type = 1;
    if (event == EVENT_ACTIVATE)
    {
      instance->number_of_targets = '\0';
      iVar1 = count_counters(player, card, COUNTER_LOYALTY);
      iVar3 = target_available(player, card, &td);
      iVar1 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017",
                          "Draw & untap lands", 1, 10, 1, "Tuck a nonland permanent", iVar3, 10,
                          0xfffffffd, "Emblem", 1, iVar1 * 5 + 0xf, 0xfffffff8, 0, 0, 0, 0);
      instance->number_of_targets = '\0';
      if (iVar1 == 2)
      {
        new_pick_target(&td, "TARGET_NONLAND_PERMANENT", 0, 1);
      }
    }
    else
    {
      iVar1 = target_available(player, card, &td);
      iVar1 = dialog_impl(player, card, event, "\001\002", "\001\017", "Draw & untap lands", 1, 10,
                          1, "Tuck a nonland permanent", iVar1, 10, 0xfffffffd, "Emblem", 1, 0,
                          0xfffffff8, 0, 0, 0, 0);
      if (event == EVENT_CAN_ACTIVATE)
      {
        if (iVar1 == 0)
        {
          return 0;
        }
      }
      else if (iVar1 == 2)
      {
        iVar1 = validate_target(player, card, &td, 0);
        if (iVar1 != 0)
        {
          put_permanent_under_the_first_x_card_of_its_owners_library(instance->targets[0].player,
                                                                     instance->targets[0].card, 2);
        }
      }
      else if (iVar1 == 3)
      {
        generate_token_by_id_2022(player, card, 0x485b);
      }
      else if (iVar1 == 1)
      {

        draw_a_card(player);
        iVar1 = create_legacy_effect(player, card, teferi_hero_of_dominaria_legacy);
        if ((current_turn == player) && (current_phase == 0x1f))
        {
          instance = get_card_instance(player, iVar1);
          instance->targets[1].player = 1;
        }
      }
    }
  }
  iVar1 = planeswalker(player, card, event, 4);
  return iVar1;
}

/* ManalinkEh 2022: 0x22d3ec0; original cards/dominaria.c:3455. */
int card_teferi_timebender(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if ((event - EVENT_RESOLVE_ACTIVATION < 2) || (event == EVENT_ACTIVATE))
  {

    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x42);
    if (event == EVENT_ACTIVATE)
    {
      instance->number_of_targets = '\0';
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017",
                           "Untap an artifact or creature", 1, 3, 2, "Gain 2 life & draw 2", 1, 8,
                           0xfffffffd, "Time Walk", 1, 10, 0xfffffff7, 0, 0, 0, 0);
      instance->number_of_targets = '\0';
      if ((result == 1) && (result = can_target(&td), result != 0))
      {
        new_pick_target(&td, "PUTREFY", 0, 0);
      }
    }
    else
    {
      result = dialog_impl(player, card, event, "\001\002", "\001\017",
                           "Untap an artifact or creature", 1, 3, 2, "Gain 2 life & draw 2", 1, 8,
                           0xfffffffd, "Time Walk", 1, 10, 0xfffffff7, 0, 0, 0, 0);
      if (event == EVENT_CAN_ACTIVATE)
      {
        if (result == 0)
        {
          return 0;
        }
      }
      else if (result == 2)
      {
        gain_life_2022(player, 2);
        draw_cards(player, 2);
      }
      else if (result == 3)
      {
        time_walk_effect(player, card);
      }
      else if (((result == 1) && (instance->number_of_targets == '\x01')) &&
               (result = valid_target(&td), result != 0))
      {
        untap_card(instance->targets[0].player, instance->targets[0].card);
      }
    }
  }
  result = planeswalker(player, card, event, 5);
  return result;
}

/* ManalinkEh 2022: 0x22d4190; original cards/dominaria.c:3529. */
int card_tempest_djinn(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (((event == EVENT_POWER) && (card == affected_card)) && (player == affected_card_controller))
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        result = get_hacked_subtype(player, card, 0x5002);
        result = count_basic_lands(player, result);
        event_result = event_result + result;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22d4300; original cards/dominaria.c:3574. */
int card_tetsuko_umezawa_fugitive(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  check_legend_rule(player, card, event);
  if ((((event == EVENT_ABILITIES) && (instance = in_play(player, card), instance != NULL)) &&
       (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)) &&
      ((affected_card_controller == player &&
        (result = is_what(player, affected_card, 2), result != 0))))
  {
    result = get_power(affected_card_controller, affected_card);
    if ((1 < result) &&
        (result = get_toughness(affected_card_controller, affected_card), 1 < result))
    {
      return 0;
    }
    special_abilities_2022(affected_card_controller, affected_card, EVENT_ABILITIES, 0x10, player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x22c9c40; original cards/dominaria.c:3604. */
static int the_antiquities_war_legacy(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;

  if (event == EVENT_CHANGE_TYPE)
  {
    default_test_definition(&this_test, 0x40);
    this_test.type_flag = 2;
    global_type_change(player, card, EVENT_CHANGE_TYPE, player, 2, &this_test, 5, 5, 0, 0, 0);
    return 0;
  }
  if (event - EVENT_POWER < 2)
  {
    result = is_what(affected_card_controller, affected_card, 0x40);
    if (result != 0)
    {
      set_base_pt(5);
    }
  }
  else if (event == EVENT_CLEANUP)
  {
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x22cab10; original cards/dominaria.c:3951. */
int time_of_ice_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;

  pcVar1 = in_play(player, card);
  if (pcVar1 != NULL)
  {
    pcVar1 = get_card_instance(player, card);
    if (-1 < pcVar1->damage_target_player)
    {

      pcVar1 = get_card_instance(player, card);
      does_not_untap((int)pcVar1->damage_target_player, pcVar1->damage_target_card, event);
      pcVar2 = in_play(pcVar1->targets[0].player, pcVar1->targets[0].card);
      if (pcVar2 == NULL)
      {
        kill_card(player, card, KILL_EXILE);
      }
      if (((event == EVENT_CARDCONTROLLED) && (pcVar1->targets[0].card == affected_card)) &&
          (pcVar1->targets[0].player == affected_card_controller))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22d5200; original cards/dominaria.c:4004. */
int card_torgaar_famine_incarnate(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  unsigned int uVar3;
  card_instance_t *instance;
  char sac[151];

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_td;

  check_legend_rule(player, card, event);
  iVar1 = duh_mode(player);
  etb_ability(player, card, event, (iVar1 != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {

    default_target_definition(player, card, &recovered_stack_td.target, 0);
    recovered_stack_td.target.zone = 0x1000;
    instance = get_card_instance(player, card);
    iVar1 = can_target(&recovered_stack_td.target);
    if ((iVar1 != 0) &&
        (iVar1 = new_pick_target(&recovered_stack_td.target, "TARGET_PLAYER", 0, 0), iVar1 != 0))
    {
      uVar3 = get_starting_life_total(instance->targets[0].player);
      uVar3 = round_down_value(uVar3);
      set_life_total(instance->targets[0].player, uVar3);
    }
  }
  else
  {
    if (event == EVENT_MODIFY_COST)
    {

      default_test_definition(&recovered_stack_td.test, 2);
      iVar1 = get_updated_casting_cost(player, card, -1, EVENT_MODIFY_COST, -1);
      iVar2 = max_can_sacrifice_as_cost(player, card, &recovered_stack_td.test);
      uVar3 = iVar1 + iVar2 * -2;
      iVar1 = has_mana_multi(player, uVar3 & (int)(uVar3 ^ 0xffffffff) >> 0x1f, 2, 0, 0, 0, 0);
      if (iVar1 == 0)
      {
        return 0;
      }
      null_casting_cost(player, card);
      return 1;
    }
    if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
        (player == affected_card_controller))
    {

      iVar1 = get_updated_casting_cost(player, card, -1, EVENT_CAST_SPELL, -1);
      iVar1 = has_mana_multi(player, iVar1, 2, 0, 0, 0, 0);
      iVar2 = card_torgaar_famine_incarnate(player, card, EVENT_MODIFY_COST);
      iVar1 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\016",
                          "Sacrifice creatures", iVar2, 5, "Cast Normally", iVar1, 10, 0, 0, 0, 0);
      if (iVar1 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      uVar3 = get_updated_casting_cost(player, card, -1, EVENT_CAST_SPELL, -1);
      if (iVar1 == 1)
      {

        round_up_value(uVar3);
        new_default_test_definition(&recovered_stack_td.test, 2, "Select a creature to sacrifice.");

        max_can_sacrifice_as_cost(player, card, &recovered_stack_td.test);
        iVar1 = mark_sacrifice(player, card, player, SAC_AS_COST | SAC_JUST_MARK,
                               &recovered_stack_td.test, sac);
        if (spell_fizzled != 1)
        {

          uVar3 = uVar3 + iVar1 * -2;
          charge_mana_multi(player, uVar3 & (int)(uVar3 ^ 0xffffffff) >> 0x1f, 2, 0, 0, 0, 0);
        }
        if (spell_fizzled != 1)
        {

          if (0 < active_cards_count[player])
          {
            iVar1 = 0;
            do
            {
              if (sac[iVar1] != '\0')
              {
                instance = get_card_instance(player, iVar1);
                instance->state = instance->state & 0xffefffff;
                kill_card(player, iVar1, KILL_SACRIFICE);
              }
              iVar1 = iVar1 + 1;
            } while (iVar1 < active_cards_count[player]);
          }
          iVar1 = get_id(player, card);
          return iVar1;
        }
        ai_modifier = ai_modifier + -99;
        return 0;
      }
      if (iVar1 == 2)
      {
        charge_mana_multi(player, uVar3, 2, 0, 0, 0, 0);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22d6b00; original cards/dominaria.c:4580. */
int card_zhalfirin_void(int player, int card, event_t event)
{
  int result;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    scry(player, 1);
  }
  result = plain_mana_producer(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int tiana_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x22c9df0; original cards/dominaria.c:3833. */
int tiana_effect(int player, int card, event_t event)
{
  char cVar1;
  uint16_t uVar2;
  int32_t iVar3;
  card_instance_t *pcVar4;
  unsigned int uVar5;
  int card_00;
  card_instance_t *pcVar6;
  int iVar7;
  unsigned int uStack_ec;
  test_definition_t test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar4 = get_card_instance(player, card);
      if ((-1 < pcVar4->damage_target_player) &&
          (iVar7 = get_id((int)pcVar4->damage_target_player, pcVar4->damage_target_card),
           iVar7 != 0x486b))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar4 = get_card_instance(player, card);
    if ((-1 < pcVar4->damage_target_player) &&
        (iVar7 = effect_follows_control_of_attachment(player, card, 0x4700), iVar7 != 0))
    {
      return 0;
    }
  LAB_022c9e27:
    uVar5 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar5 == 0)
      goto LAB_022c9e4c;
  }
  else
  {
    pcVar4 = get_card_instance(player, card);
    if ((-1 < pcVar4->damage_target_player) &&
        (iVar7 = effect_follows_control_of_attachment(player, card, event), iVar7 != 0))
    {
      return 0;
    }
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_022c9e27;

    pcVar4 = get_card_instance(player, card);
    cVar1 = pcVar4->damage_target_player;

    if ((((-1 < cVar1) && (pcVar4->damage_target_card == affected_card)) &&
         (cVar1 == affected_card_controller)) &&
        (pcVar6 = get_card_instance((int)cVar1, affected_card), pcVar6->kill_code != '\0'))
    {
      pcVar4->damage_target_card = -1;
      pcVar4->damage_target_player = -1;
      remove_status(player, card, 0x10000);
      if (pcVar4->targets[0xb].player < 0)
      {
        pcVar4->targets[0xb].player = 0;
      }
      *(uint8_t *)((int)&pcVar4->targets[0xb].player + 2) = 1;
    }
    default_test_definition(&test, 0x47);
    test.subtype = 0x4000;
    test.sub2 = 0x3001;
    test.subtype_flag = 3;
    count_for_gfp_ability_and_store_values(player, card, 0x77, player, 0, &test, 0xd0, 1);
    uVar5 = resolve_gfp_ability_2022(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (uVar5 == 0)
    {
      return 0;
    }
  }
  iVar7 = check_status(player, card, 0x10000);
  if (iVar7 == 0)
  {

    pcVar4 = get_card_instance(player, card);

    uStack_ec = (uVar5 & 0xff) + 1;
    if (10 < uStack_ec)
    {
      uStack_ec = 10;
    }
    if ((uVar5 & 0xff) != 0)
    {
      iVar7 = 1;
      do
      {
        if (pcVar4->targets[iVar7].player != -1)
        {

          card_00 = create_legacy_effect(player, card, tiana_reanimation_legacy);
          pcVar6 = get_card_instance(player, card_00);
          iVar3 = pcVar4->targets[iVar7].card;
          pcVar6->targets[0].player = pcVar4->targets[iVar7].player;
          pcVar6->targets[0].card = iVar3;
          uVar2 = cards_data[pcVar4->targets[iVar7].card].id;
          pcVar6->display_pic_csv_id = uVar2;
        }
        pcVar4->targets[iVar7].card = -1;
        pcVar4->targets[iVar7].player = -1;
        iVar7 = iVar7 + 1;
      } while (iVar7 < (int)uStack_ec);
    }
    kill_card(player, card, KILL_EXILE);
  }
LAB_022c9e4c:
  if ((event == EVENT_CLEANUP) && (iVar7 = check_status(player, card, 0x10000), iVar7 == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x22d4e30; original cards/dominaria.c:3892. */
int card_tiana_ships_caretaker(int player, int card, event_t event)
{
  uint16_t uVar1;
  int32_t iVar2;
  card_instance_t *pcVar3;
  unsigned int uVar4;
  int iVar5;
  card_instance_t *pcVar6;
  int iVar7;
  unsigned int uStack_ec;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      if (0 < active_cards_count[player])
      {
        iVar7 = 0;
        do
        {
          pcVar3 = in_play(player, iVar7);

          if (((pcVar3 != NULL) && (iVar5 = is_what(player, iVar7, 0x80), iVar5 != 0)) &&
              (pcVar3 = get_card_instance(player, iVar7), pcVar3->info_slot_fn == tiana_effect))
            goto LAB_022d4e68;
          iVar7 = iVar7 + 1;
        } while (iVar7 < active_cards_count[player]);
      }

      iVar7 = create_targetted_legacy_effect(player, card, tiana_effect, player, card);
      pcVar3 = get_card_instance(player, iVar7);
      pcVar3->number_of_targets = '\0';
      pcVar3->targets[0].player = player;
      pcVar3->targets[0].card = card;
      add_status(player, iVar7, 0x10000);
    }
  }
  else if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    default_test_definition(&test, 0x47);
    test.subtype = 0x4000;
    test.sub2 = 0x3001;
    test.subtype_flag = 3;
    count_for_gfp_ability_and_store_values(player, card, 0x77, player, 0, &test, 0xd0, 0);
  }
LAB_022d4e68:
  pcVar3 = get_card_instance(player, card);

  if ((pcVar3->kill_code == '\0') &&
      (uVar4 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY), uVar4 != 0))
  {

    pcVar3 = get_card_instance(player, card);
    uVar4 = uVar4 & 0xff;

    uStack_ec = uVar4;
    if (10 < uVar4)
    {
      uStack_ec = 10;
    }
    if (uVar4 != 0)
    {
      iVar7 = 0;
      do
      {
        if (pcVar3->targets[iVar7].player != -1)
        {

          iVar5 = create_legacy_effect(player, card, tiana_reanimation_legacy);
          pcVar6 = get_card_instance(player, iVar5);
          iVar2 = pcVar3->targets[iVar7].card;
          pcVar6->targets[0].player = pcVar3->targets[iVar7].player;
          pcVar6->targets[0].card = iVar2;
          uVar1 = cards_data[pcVar3->targets[iVar7].card].id;
          pcVar6->display_pic_csv_id = uVar1;
        }
        pcVar3->targets[iVar7].card = -1;
        pcVar3->targets[iVar7].player = -1;
        iVar7 = iVar7 + 1;
      } while (iVar7 < (int)uStack_ec);
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int saga(int player, int card, event_t event, int chapters);

/* ManalinkEh 2022: 0x22cabe0; original cards/dominaria.c:99. */
int saga(int player, int card, event_t event, int chapters)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  unsigned int uStack_18;
  int iStack_14;

  iStack_14 = chapters;
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    hack_silent_counters = hack_silent_counters + 1;
    add_counters_from_source(player, card, player, card, COUNTER_LORE, 1);
    hack_silent_counters = hack_silent_counters + -1;
    instance = get_card_instance(player, card);
    instance->eot_toughness = 1;
    iVar1 = is_humiliated_for_ability(player, card, AL_STATIC);
    if (iVar1 != 0)
    {
      iStack_14 = 0;
    }
  LAB_022cac53:
    if ((player != -1) && (card != -1))
    {
    LAB_022cac5d:
      iVar1 = beginning_of_phase(player, card, event, player, card, 0x14);
      if (iVar1 != 0)
        goto LAB_022cadbf;
    }
  }
  else
  {
    if (event == EVENT_PHASE_CHANGED)
    {
      if (current_phase != 0x14)
      {
        instance = get_card_instance(player, card);
        instance->eot_toughness = 0;
      }
      iVar1 = is_humiliated_for_ability(player, card, AL_STATIC);
      if (iVar1 == 0)
        goto LAB_022cac53;
      if ((player == -1) || (card == -1))
        goto LAB_022cac9a;
      iStack_14 = 0;
      goto LAB_022cac5d;
    }
    iVar1 = is_humiliated_for_ability(player, card, AL_STATIC);
    if (iVar1 != 0)
    {
      iStack_14 = 0;
    }
    if (event != EVENT_STATIC_EFFECTS)
      goto LAB_022cac53;
    iVar1 = count_counters(player, card, COUNTER_LORE);
    if (iStack_14 <= iVar1)
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
    if (((player == -1) || (card == -1)) ||
        (iVar1 = beginning_of_phase(player, card, EVENT_STATIC_EFFECTS, player, card, 0x14),
         iVar1 == 0))
      goto LAB_022cac9a;
  LAB_022cadbf:
    add_counters_from_source(player, card, player, card, COUNTER_LORE, 1);
    iVar1 = count_counters(player, card, COUNTER_LORE);
    if (iStack_14 <= iVar1)
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
  }
  if (((event == EVENT_COUNTERS) && (counters_added_type == 0x41)) &&
      ((card == affected_card &&
        ((player == affected_card_controller &&
          (iVar1 = is_humiliated_for_ability(player, card, AL_TRIG), iVar1 == 0))))))
  {

    if (counters_added == 1)
    {
      iVar1 = count_counters(player, card, COUNTER_LORE);
      uStack_18 = 1 << ((unsigned char)iVar1 & 0x1f);
    }
    else
    {

      iVar1 = count_counters(player, card, COUNTER_LORE);
      uStack_18 = 0;
      for (; iVar3 = count_counters(player, card, COUNTER_LORE), iVar3 - counters_added < iVar1;
           iVar1 = iVar1 + -1)
      {
        uStack_18 = uStack_18 | 1 << ((unsigned char)iVar1 & 0x1f);
      }
    }
    if ((uStack_18 & 2) != 0)
    {
      call_card_function(player, card, EVENT_CHAPTER1);
    }
    if ((uStack_18 & 4) != 0)
    {
      call_card_function(player, card, EVENT_CHAPTER2);
    }
    if (((uStack_18 & 8) != 0) && (2 < iStack_14))
    {
      call_card_function(player, card, EVENT_CHAPTER3);
    }
    if (((uStack_18 & 0x10) != 0) && (3 < iStack_14))
    {
      call_card_function(player, card, EVENT_CHAPTER4);
    }
  }
LAB_022cac9a:
  iVar1 = global_enchantment(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x22cb060; original cards/dominaria.c:198. */
int card_arbor_armament(int player, int card, event_t event)
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
        td.preferred_controller = player;
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {

            instance = get_card_instance(player, card);
            pump_ability_until_eot_2022(player, card, instance->targets[0].player,
                                   instance->targets[0].card, 0, 0, 0x400, 0);
            add_counters_from_source(player, card, instance->targets[0].player,
                                     instance->targets[0].card, COUNTER_P1_P1, 1);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_022cb0b8;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.preferred_controller = player;
LAB_022cb0b8:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22cd8b0; original cards/dominaria.c:1021. */
int card_fall_of_the_thran(int player, int card, event_t event)
{
  int result;
  unsigned int uVar2;
  unsigned int player_00;
  test_definition_t this_test;

  if (event == EVENT_CHAPTER1)
  {
    default_test_definition(&this_test, 1);
    new_manipulate_all(player, card, 2, &this_test, ACT_KILL_DESTROY);
  }
  else if ((event - EVENT_CHAPTER2 & 0xfffffeff) == 0)
  {

    uVar2 = 0;
    do
    {
      player_00 = current_turn ^ uVar2;
      result = graveyard_has_shroud(player_00);
      if (result == 0)
      {
        new_default_test_definition(&this_test, 1, "Select target land card.");
        this_test.qty = 2;
        new_global_tutor(player_00, player_00, 2, 4, 0, 1, &this_test);
      }
      uVar2 = uVar2 + 1;
    } while (uVar2 != 2);
  }
  result = saga(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x22ce5f0; original cards/dominaria.c:1374. */
int card_history_of_benalia(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if ((event & ~EVENT_SET_LEGACY_EFFECT_NAME) == EVENT_CHAPTER1)
  {
    generate_token_by_id_2022(player, card, 0x4958);
  }
  else if (event == EVENT_CHAPTER3)
  {
    new_default_test_definition(&test, 2, "");
    test.subtype = 0x67;
    pump_creatures_until_eot_merge_pt(player, card, player, 2, 1, &test);
  }
  result = saga(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x22cfb50; original cards/dominaria.c:1808. */
int card_kazarov_sengir_pureblood(int player, int card, event_t event)
{
  int result;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  card_instance_t *pcVar4;
  CardFunction p_Stack_70;
  int32_t iStack_68;
  target_definition_t td;

  result = is_humiliated_for_ability(player, card, AL_TRIG);
  if (result == 0)
  {

    if (event != EVENT_DEAL_DAMAGE)
    {
      pcVar2 = get_card_instance(player, card);
      if ((0 < pcVar2->info_slot) && (trigger_condition == 0xd7))
        goto LAB_022cfcc2;
      goto LAB_022cfbb0;
    }
    pcVar2 = get_card_instance(player, card);
    pcVar3 = get_card_instance(affected_card_controller, affected_card);
    if ((pcVar3->internal_card_id == damage_card) &&
        (((pcVar3->damage_target_card != -1 &&
           (result = is_what((int)pcVar3->damage_target_player, pcVar3->damage_target_card, 2),
            result != 0)) &&
          (iStack_68 = (int32_t)pcVar3->damage_target_player, iStack_68 == 1 - player))))
    {

      if (pcVar3->info_slot < 1)
      {

        pcVar4 = get_card_instance((int)pcVar3->damage_source_player, pcVar3->damage_source_card);
        if (pcVar4->targets[0x10].player < 1)
          goto LAB_022cfc99;
        iStack_68 = (int32_t)pcVar3->damage_target_player;
      }
      result = pcVar2->info_slot;
      if (result < 0)
      {
        result = 0;
        p_Stack_70 = (CardFunction)0x1;
      }
      else
      {
        p_Stack_70 = (CardFunction)(result + 1);
      }
      pcVar2->targets[result].player = iStack_68;
      pcVar2->targets[result].card = pcVar3->damage_target_card;
      pcVar2->info_slot_fn = p_Stack_70;
    }
  LAB_022cfc99:
    pcVar2 = get_card_instance(player, card);
    if (pcVar2->info_slot < 1)
    {
      return 0;
    }
    if (trigger_condition == 0xd7)
    {
    LAB_022cfcc2:
      if (((card == affected_card) && (player == affected_card_controller)) &&
          (reason_for_trigger_controller == player))
      {
        if (event == EVENT_TRIGGER)
        {
          event_result = event_result | 2;
          return 0;
        }
        if (event == EVENT_RESOLVE_TRIGGER)
        {

          pcVar2 = get_card_instance(player, card);
          add_counters_from_source(player, card, player, card, COUNTER_P1_P1, pcVar2->info_slot);
          pcVar2->info_slot = 0;
          return 0;
        }
      }
      goto LAB_022cfbb0;
    }
  }
  else
  {
  LAB_022cfbb0:
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
      goto LAB_022cfbce;
  }
  if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
  {
    return 0;
  }
LAB_022cfbce:
  default_target_definition(player, card, &td, 2);
  if ((event == EVENT_RESOLVE_ACTIVATION) &&
      (result = validate_target(player, card, &td, 0), result != 0))
  {
    damage_target0(player, card, 2);
  }
  result = generic_activated_ability(player, card, event, 4, 3, 0, 0, 0, 1, 0, 0, &td,
                                     "TARGET_CREATURE");
  return result;
}

/* ManalinkEh 2022: 0x22d0690; original cards/dominaria.c:2075. */
int card_marwyn_the_nurturer(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if ((((trigger_condition == 0xdb) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (reason_for_trigger_controller == player))
  {
    new_default_test_definition(&test, 0x47, "");
    test.subtype = 0x3c;
    test.not_me = 1;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_ETB)
  {
    add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
    result = 0;
  }
  else if (((event == EVENT_COUNT_MANA) || (event == EVENT_ACTIVATE)) ||
           ((event == EVENT_CAN_ACTIVATE && (player == 1))))
  {
    result = get_power(player, card);
  }
  else
  {
    result = 0;
  }
  result = mana_producing_creature(player, card, event, 0x30, COLOR_GREEN, result);
  return result;
}

/* ManalinkEh 2022: 0x22d1ca0; original cards/dominaria.c:2601. */
int card_phyrexian_scriptures(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_td;

  if (event == EVENT_CHAPTER1)
  {
    default_target_definition(player, card, &recovered_stack_td.target, 2);
    recovered_stack_td.target.preferred_controller = player;
    result = can_target(&recovered_stack_td.target);
    if (result != 0)
    {
      result = new_pick_target(&recovered_stack_td.target, "TARGET_CREATURE", 0, 1);
      if (result != 0)
      {

        instance = get_card_instance(player, card);
        add_counters_from_source(player, card, instance->targets[0].player,
                                 instance->targets[0].card, COUNTER_P1_P1, 1);
        turn_into_artifact(player, card, instance->targets[0].player, instance->targets[0].card, 0);
      }
    }
  }
  else if (event == EVENT_CHAPTER2)
  {
    default_test_definition(&recovered_stack_td.test, 2);
    new_manipulate_all(player, card, 2, &recovered_stack_td.test, ACT_KILL_DESTROY);
  }
  else if (event == EVENT_CHAPTER3)
  {
    rfg_whole_graveyard(1 - player);
  }
  result = saga(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x22d1e20; original cards/dominaria.c:2638. */
int card_precognition_field(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  uint32_t uVar4;
  unsigned int uVar5;
  uint32_t uStack_14;

  if (event == EVENT_CAN_ACTIVATE)
  {

    iVar1 = *deck_ptr[player];
    if (iVar1 != -1)
    {
      iVar3 = is_what(-1, iVar1, 0x38);
      if ((iVar3 != 0) &&
          (iVar3 = has_mana_to_cast_iid(player, EVENT_CAN_ACTIVATE, iVar1), iVar3 != 0))
      {
        iVar1 = can_legally_play_iid_now(player, iVar1, EVENT_CAN_ACTIVATE);
        return iVar1;
      }
      if (*deck_ptr[player] != -1)
      {
        return 1;
      }
    }
    iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 3, 0, 0, 0, 0, 0, 0,
                                      NULL, NULL);
    return (unsigned int)(iVar1 != 0);
  }
  if (event != EVENT_ACTIVATE)
  {
    if ((event == EVENT_RESOLVE_ACTIVATION) &&
        (instance = get_card_instance(player, card), instance->info_slot == 3))
    {
      rfg_top_card_of_deck(player);
    }
    goto LAB_022d1e49;
  }

  iVar1 = *deck_ptr[player];
  if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
  {
    uStack_14 = 0;
    if (iVar1 != -1)
      goto LAB_022d1e8f;
  LAB_022d1eaf:
    iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 3, 0, 0, 0, 0, 0, 0,
                                      NULL, NULL);
    uVar5 = 0;
  LAB_022d1f15:
    uVar4 = 10;
  }
  else
  {
    uStack_14 = 1;
    if (iVar1 == -1)
    {
      uStack_14 = 0;
      goto LAB_022d1eaf;
    }
  LAB_022d1e8f:
    iVar1 = is_what(-1, iVar1, 0x38);
    if ((iVar1 == 0) ||
        (iVar1 = can_legally_play_iid_now(player, *deck_ptr[player], EVENT_ACTIVATE), iVar1 == 0))
      goto LAB_022d1eaf;
    iVar3 = has_mana_to_cast_iid(player, EVENT_ACTIVATE, *deck_ptr[player]);
    uVar5 = (unsigned int)(iVar3 != 0);
    iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 3, 0, 0, 0, 0, 0, 0,
                                      NULL, NULL);
    if (iVar3 == 0)
      goto LAB_022d1f15;
    uVar5 = 1;
    uVar4 = 0xfffffff6;
  }
  iVar1 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016",
                      "Check top card of deck", uStack_14, 0, "Play top card", uVar5, 10,
                      "Exile top card", iVar1, uVar4, 0, 0, 0, 0);
  if (iVar1 == 0)
  {
    spell_fizzled = 1;
    return 0;
  }
  if (iVar1 == 1)
  {
    show_deck(player, deck_ptr[player], 1, "Here\'s the top card of your deck", 0,
              ENGINE_DONE_LABEL);
    spell_fizzled = 1;
  }
  else if (iVar1 == 2)
  {
    iVar1 = charge_mana_from_id(player, -1, EVENT_ACTIVATE,
                                (unsigned int)cards_data[*deck_ptr[player]].id);
    if (iVar1 == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      play_card_in_deck_for_free(player, player, 0);
      cant_be_responded_to = 1;
    }
  }
  else if (iVar1 == 3)
  {
    generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 3, 0, 0, 0, 0, 0, 0, NULL, NULL);
  }
LAB_022d1e49:
  iVar1 = global_enchantment(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x22d24e0; original cards/dominaria.c:2778. */
int card_rite_of_belzenlok(int player, int card, event_t event)
{
  int result;
  token_generation_t token;

  if ((event & ~EVENT_SET_LEGACY_EFFECT_NAME) == EVENT_CHAPTER1)
  {
    default_token_definition(player, card, 0x4916, &token);
    token.pow = 0;
    token.color_forced = '\x02';
    token.qty = '\x02';
    generate_token(&token);
  }
  else if (event == EVENT_CHAPTER3)
  {
    default_token_definition(player, card, 0x491a, &token);
    token.tou = 6;
    token.pow = 6;
    token.key_plus = 0xa0;
    token.special_infos = 0x42;
    generate_token(&token);
  }
  result = saga(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x22d29f0; original cards/dominaria.c:2910. */
int card_settle_the_score(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_t pw;
  target_definition_t td;
  target_definition_t tdpw;

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
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
            base_target_definition(player, card, &tdpw, 0x100);
            tdpw.preferred_controller = player;
            tdpw.allowed_controller = player;
            tdpw.allow_cancel = 0;
            result = can_target(&tdpw);
            if ((result != 0) && (result = select_target(player, card, &tdpw,
                                                         "Select a planeswalker you control.", &pw),
                                  result != 0))
            {
              add_counters_from_source(player, card, pw.player, pw.card, COUNTER_LOYALTY, 2);
            }
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_022d2a4b;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
LAB_022d2a4b:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22d3610; original cards/dominaria.c:3205. */
int card_song_of_freyalise(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;

  if ((event & ~EVENT_SET_LEGACY_EFFECT_NAME) == EVENT_CHAPTER1)
  {

    iVar3 = create_legacy_activate(player, card, song_of_freyalise_legacy);
    if (current_turn == player)
    {
      instance = get_card_instance(player, iVar3);
      instance->targets[2].player = 1;
    }
  }
  else
  {

    if ((event == EVENT_CHAPTER3) && (0 < active_cards_count[player]))
    {
      iVar3 = 0;
    LAB_022d365e:
      do
      {
        instance = in_play(player, iVar3);
        if (instance != NULL)
        {
          iVar2 = is_what(player, iVar3, 2);
          if (iVar2 != 0)
          {
            add_counters_from_source(player, card, player, iVar3, COUNTER_P1_P1, 1);
            iVar2 = iVar3 + 1;
            pump_ability_until_eot_2022(player, card, player, iVar3, 0, 0, 0x80, 0x808);
            iVar3 = iVar2;
            if (active_cards_count[player] <= iVar2)
              break;
            goto LAB_022d365e;
          }
        }
        iVar3 = iVar3 + 1;
      } while (iVar3 < active_cards_count[player]);
    }
  }
  iVar3 = saga(player, card, event, 3);
  return iVar3;
}

/* ManalinkEh 2022: 0x22d4480; original cards/dominaria.c:3619. */
int card_the_antiquities_war(int player, int card, event_t event)
{
  int result;
  int revealed_card;
  test_definition_t this_test;

  if ((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CHAPTER1)
  {
    if (event == EVENT_CHAPTER3)
    {
      create_legacy_effect(player, card, the_antiquities_war_legacy);
    }
    goto LAB_022d44a6;
  }

  result = count_deck(player);
  if (result == 0)
    goto LAB_022d44a6;
  if (5 < result)
  {
    result = 5;
  }

  new_default_test_definition(&this_test, 0x40, "Select an Artifact card.");
  this_test.create_minideck = (unsigned short)result;
  this_test.no_shuffle = 1;
  revealed_card = new_global_tutor(player, player, 1, 1, 0, 1, &this_test);
  if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
  {
    if (revealed_card < 0)
    {
      this_test.type = 0xff;
      revealed_card = new_global_tutor(player, player, 1, 1, 0, 1, &this_test);
      goto LAB_022d4574;
    }
  LAB_022d45a5:
    result = result + -1;
    reveal_card(player, card, player, revealed_card);
  }
  else
  {
  LAB_022d4574:
    if (-1 < revealed_card)
      goto LAB_022d45a5;
  }
  if (0 < result)
  {
    put_top_x_on_bottom_in_random_order(player, result);
  }
LAB_022d44a6:
  result = saga(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x22d4610; original cards/dominaria.c:3652. */
int card_the_eldest_reborn(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  char buffer[75];
  test_definition_t test;

  new_default_test_definition(&test, 0x102, buffer);
  if (event == EVENT_CHAPTER1)
  {
    scnprintf(buffer, 0x4b, "Select a creature or planeswalker to sacrifice");
    new_sacrifice(player, card, 1 - player, SAC_CAUSED | SAC_NO_CANCEL, &test);
  }
  else if (event == EVENT_CHAPTER2)
  {
    discard(1 - player, 0, player);
  }
  else if (event == EVENT_CHAPTER3)
  {
    scnprintf(buffer, 0x4b, "Select a creature or planeswalker card");
    result = new_special_count_grave(player, &test);
    if ((result == 0) || (result = graveyard_has_shroud(player), result != 0))
    {
      result = new_special_count_grave(1 - player, &test);
      if ((result == 0) || (result = graveyard_has_shroud(1 - player), result != 0))
        goto LAB_022d4662;
    }

    select_target_from_either_grave(player, card, SFG_0, 2, 2, &test, 0, 1);
    instance = get_card_instance(player, card);
    result = validate_target_from_grave_source(player, card, instance->targets[0].player, 1);
    if (result != -1)
    {
      reanimate_permanent(player, card, instance->targets[0].player, result, REANIMATE_DEFAULT);
    }
  }
LAB_022d4662:
  result = saga(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x22d4980; original cards/dominaria.c:3723. */
int card_the_flame_of_keld(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_CHAPTER1)
  {
    discard_all(player);
  }
  else if (event == EVENT_CHAPTER2)
  {
    draw_cards(player, 2);
  }
  else if (event == EVENT_CHAPTER3)
  {

    result = create_legacy_effect(player, card, flame_of_keld_legacy);
    instance = get_card_instance(player, result);
    result = get_sleighted_color_test(player, card, 0x10);
    instance->targets[2].card = result;
  }
  result = saga(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x22d4a40; original cards/dominaria.c:3741. */
int card_the_mending_of_dominaria(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if ((event & ~EVENT_SET_LEGACY_EFFECT_NAME) == EVENT_CHAPTER1)
  {
    mill(player, 2);
    new_default_test_definition(&test, 2, "Select target creature card.");
    result = count_graveyard_by_type(player, 2);
    if (result != 0)
    {
      result = graveyard_has_shroud(player);
      if (result == 0)
      {
        new_global_tutor(player, player, 2, 1, 0, 1, &test);
      }
    }
  }
  else if (event == EVENT_CHAPTER3)
  {
    reanimate_all(player, card, player, 1, 0, 0, 0, 0, 0, 0, 0, -1, 0, REANIMATE_DEFAULT);
    reshuffle_grave_into_deck(player, 0);
  }
  result = saga(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x22d50a0; original cards/dominaria.c:3966. */
int card_time_of_ice(int player, int card, event_t event)
{
  int result;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_td;

  if ((event & ~EVENT_SET_LEGACY_EFFECT_NAME) == EVENT_CHAPTER1)
  {

    default_target_definition(player, card, &recovered_stack_td.target, 2);
    recovered_stack_td.target.allowed_controller = 1 - player;
    recovered_stack_td.target.preferred_controller = recovered_stack_td.target.allowed_controller;
    pcVar2 = get_card_instance(player, card);
    result = can_target(&recovered_stack_td.target);
    if (result != 0)
    {
      result = pick_target(&recovered_stack_td.target, "TARGET_CREATURE");
      if (result != 0)
      {

        tap_card(pcVar2->targets[0].player, pcVar2->targets[0].card);
        result = create_targetted_legacy_effect(player, card, time_of_ice_effect,
                                                pcVar2->targets[0].player, pcVar2->targets[0].card);
        pcVar3 = get_card_instance(player, result);
        pcVar3->targets[0].player = player;
        pcVar3->targets[0].card = card;
        pcVar3->number_of_targets = '\x01';
      }
    }
    pcVar2->number_of_targets = '\0';
  }
  else if (event == EVENT_CHAPTER3)
  {
    default_test_definition(&recovered_stack_td.test, 2);
    new_manipulate_all(player, card, 2, &recovered_stack_td.test, ACT_BOUNCE);
  }
  result = saga(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x22d5730; original cards/dominaria.c:4114. */
int card_triumph_of_gerrard(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  char buffer[75];
  target_definition_t td;

  base_target_definition(player, card, &td, 2);
  td.preferred_controller = player;
  td.allowed_controller = player;
  td.special = 0x8000;
  td.extra = (int32_t)has_highest_power;
  td.allow_cancel = 0;
  if ((event == EVENT_CHAPTER3) || ((event & ~EVENT_SET_LEGACY_EFFECT_NAME) == EVENT_CHAPTER1))
  {

    result = can_target(&td);
    if (result != 0)
    {
      result = get_highest_power(player);
      scnprintf(buffer, 0x4b, "Select target creature with power %d", result);
      new_pick_target(&td, buffer, 0, 0x81);
    }
    instance = get_card_instance(player, card);
    if ((event & ~EVENT_SET_LEGACY_EFFECT_NAME) == EVENT_CHAPTER1)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        add_counters_from_source(player, card, instance->targets[0].player,
                                 instance->targets[0].card, COUNTER_P1_P1, 1);
      }
    }
    else
    {
      pump_ability_until_eot_2022(player, card, instance->targets[0].player, instance->targets[0].card,
                             0, 0, 0x120, 2);
    }
  }
  result = saga(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x22d5a90; original cards/dominaria.c:4215. */
int card_urzas_ruinous_blast(int player, int card, event_t event)
{
  unsigned int uVar1;
  int result;
  test_definition_t this_test;

  if (event == EVENT_CAN_CAST)
  {
    result = check_battlefield_for_subtype(player, 2, 0x2001);
    uVar1 = 1;
    if (result == 0)
    {
      result = check_battlefield_for_subtype(player, 0x100, -1);
      uVar1 = (unsigned int)(result != 0);
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
    else if (event == EVENT_RESOLVE_SPELL)
    {
      default_test_definition(&this_test, 1);
      this_test.type_flag = 1;
      this_test.special_selection_function = is_not_legendary_test;
      new_manipulate_all(player, card, 2, &this_test, ACT_KILL_REMOVE);
      kill_card(player, card, KILL_DESTROY);
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
    uVar1 = basic_spell(player, card, event);
  }
  return uVar1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int slimefoot_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x22ccc20; original cards/dominaria.c:685. */
int card_chainers_torment(int player, int card, event_t event)
{
  int argument3;
  int argument4;
  int result;
  unsigned int argument2;
  card_instance_t *instance;
  token_generation_t token;

  if ((event & ~EVENT_SET_LEGACY_EFFECT_NAME) == EVENT_CHAPTER1)
  {
    damage_player(1 - player, 2, player, card);
    gain_life_2022(player, 2);
  }
  else if (event == EVENT_CHAPTER3)
  {

    argument2 = round_up_value(life[player]);
    default_token_definition(player, card, 0x4969, &token);
    token.keep_track_of_tokens_generated = 10;
    token.pow = argument2;
    token.tou = argument2;
    generate_token(&token);
    result = 0;
    do
    {

      instance = in_play(player, card);
      if (instance != NULL)
      {
        argument3 = instance->targets[result].player;
        if ((argument3 != -1) && (argument4 = instance->targets[result].card, argument4 != -1))
        {
          damage_player(player, argument2, argument3, argument4);
        }
      }
      result = result + 1;
    } while (result != 9);
  }
  result = saga(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x22cd490; original cards/dominaria.c:899. */
int card_demonlord_belzenlok(int player, int card, event_t event)
{
  int result;
  int argument2;
  test_definition_t test;

  check_legend_rule(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }

  argument2 = 0;
  default_test_definition(&test, 1);
  test.type_flag = 1;
  do
  {
    result = reveal_card_until_you_reveal_test(player, player, &test, 0x228);
    if (result < 0)
      break;

    result = get_cmc(player, result);
    argument2 = argument2 + 1;
  } while (3 < result);
  damage_player(player, argument2, player, card);
  return 0;
}

/* ManalinkEh 2022: 0x22cf690; original cards/dominaria.c:1706. */
int card_karn_scion_of_urza(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  int32_t *piVar3;
  int *piVar4;
  int pos;
  unsigned int iid;
  int iStack_148;
  int leg;
  int idx;
  target_definition_t td;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_test;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    instance = get_card_instance(player, card);
    if (event != EVENT_ACTIVATE)
      goto LAB_022cf6fd;

    instance->number_of_targets = '\0';
    instance->info_slot = 0;
    result = exiledby_count(player, card, player);
    result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Reveal top 2", 1, 5,
                         1, "Return a card from exile", (unsigned int)(0 < result), 10, 0xffffffff,
                         "Make a construct", 1, 10, 0xfffffffe, 0, 0, 0, 0);
    if (result == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
    if (result != 2)
      goto LAB_022cf702;

    piVar3 = (int32_t *)exiledby_choose(player, card, 0x4809, EXBY_CHOOSE, 0, "Card", 0);
    if (piVar3 != NULL)
    {
      instance->targets[6].player = -1;
      instance->targets[6].card = *piVar3;
      goto LAB_022cf702;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_022cf702;
    default_target_definition(player, card, &td, 2);
    instance = get_card_instance(player, card);
  LAB_022cf6fd:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_022cf702;

    result = instance->info_slot;
    if (result == 1)
    {

      result = count_deck(player);
      if (result != 0)
      {
        iStack_148 = result;
        if (2 < result)
        {
          iStack_148 = 2;
        }

        leg = 0;
        idx = 0;
        show_deck(player, deck_ptr[player], iStack_148, "Cards revealed by Karn", 0,
                  ENGINE_DONE_LABEL);
        new_default_test_definition(&recovered_stack_test.test, 0xff, "Select a card.");
        recovered_stack_test.test.create_minideck = (unsigned short)iStack_148;
        recovered_stack_test.test.no_shuffle = 1;
        pos = new_select_a_card(1 - player, player, 1, 1, 3, -1, &recovered_stack_test.test);
        exiledby_remember(player, instance->parent_card, player, deck_ptr[player][pos], &leg, &idx);
        play_sound_effect(WAV_DESTROY);
        rfg_card_in_deck(player, pos);
        if (result != 1)
        {
          add_card_to_hand(player, *deck_ptr[player]);
          remove_card_from_deck(player, 0);
        }
      }
      goto LAB_022cf702;
    }
    if (result != 2)
    {
      if (result == 3)
      {
        default_token_definition(player, card, 0x4918, &recovered_stack_test.token);
        recovered_stack_test.test.message[0x1c] = '\0';
        recovered_stack_test.test.message[0x1d] = '\0';
        recovered_stack_test.test.message[0x1e] = '\0';
        recovered_stack_test.test.message[0x1f] = '\0';
        recovered_stack_test.test.message[0x18] = '\0';
        recovered_stack_test.test.message[0x19] = '\0';
        recovered_stack_test.test.message[0x1a] = '\0';
        recovered_stack_test.test.message[0x1b] = '\0';
        recovered_stack_test.test.message[0x2c] = 'B';
        recovered_stack_test.test.message[0x2d] = '\0';
        recovered_stack_test.test.message[0x2e] = '\0';
        recovered_stack_test.test.message[0x2f] = '\0';
        generate_token(&recovered_stack_test.token);
      }
      goto LAB_022cf702;
    }

    piVar4 = exiledby_find(player, instance->parent_card, instance->targets[6].card, NULL, NULL);
    if (piVar4 != NULL)
    {

      *piVar4 = -1;
      iid = instance->targets[6].card & 0x7fffffff;
      add_card_to_hand(player, iid);
      remove_card_from_rfg(player, (unsigned int)cards_data[iid].id);
      goto LAB_022cf702;
    }
  }
  spell_fizzled = 1;
LAB_022cf702:
  result = planeswalker(player, card, event, 5);
  return result;
}

/* ManalinkEh 2022: 0x22c9890; original cards/dominaria.c:3037. */
static int slimefoot_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  unsigned int uVar2;
  card_instance_t *pcVar3;
  int result;
  test_definition_t test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x484c))
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
  LAB_022c98c7:
    uVar2 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
      goto joined_r0x022c98f0;
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
      goto LAB_022c98c7;

    pcVar1 = get_card_instance(player, card);
    pcVar3 = in_play(affected_card_controller, affected_card);
    if ((pcVar3 != NULL) &&
        (result = is_this_dying(affected_card_controller, affected_card), result != 0))
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
      }
      default_test_definition(&test, 2);
      test.subtype = 0xa3;
      count_for_gfp_ability(player, card, 0x77, player, 0, &test);
    }
    uVar2 = resolve_gfp_ability_2022(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
    {
      return 0;
    }
  }
  result = check_status(player, card, 0x10000);
  if (result == 0)
  {

    uVar2 = uVar2 & 0xff;
    if (uVar2 != 0)
    {
      do
      {
        uVar2 = uVar2 - 1;
        damage_player(1 - player, 1, player, card);
        gain_life(player, 1);
      } while (0 < (int)uVar2);
    }
    kill_card(player, card, KILL_EXILE);
  }
joined_r0x022c98f0:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x22d3090; original cards/dominaria.c:3092. */
int card_slimefoot_the_stowaway(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  unsigned int uVar3;
  int iVar4;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      iVar2 = 0;
      if (0 < active_cards_count[player])
      {
        do
        {
          instance = in_play(player, iVar2);

          if ((((instance != NULL) && (iVar4 = is_what(player, iVar2, 0x80), iVar4 != 0)) &&
               (instance = get_card_instance(player, iVar2),
                instance->info_slot_fn == slimefoot_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_022d31ec;
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[player]);
      }

      iVar2 = create_targetted_legacy_effect(player, card, slimefoot_effect, player, card);
      instance = get_card_instance(player, iVar2);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar2, 0x10000);
    }
  LAB_022d31ec:
    instance = get_card_instance(player, card);
    if (instance->kill_code == '\0')
    {
    LAB_022d3218:

      uVar3 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
      if (uVar3 != 0)
      {
      LAB_022d323d:

        uVar3 = uVar3 & 0xff;
        if (uVar3 != 0)
        {
          do
          {
            damage_player(1 - player, 1, player, card);
            gain_life(player, 1);
            uVar3 = uVar3 - 1;
          } while (0 < (int)uVar3);
        }
      }
      goto LAB_022d30e2;
    }
  LAB_022d31fe:
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_022d30f2;
    }
  }
  else
  {
    if (event == EVENT_GRAVEYARD_FROM_PLAY)
    {
      default_test_definition(&test, 2);
      test.subtype = 0xa3;
      count_for_gfp_ability(player, card, 0x77, player, 0, &test);
      instance = get_card_instance(player, card);
      if (instance->kill_code != '\0')
      {
        return 0;
      }
      uVar3 =
          resolve_gfp_ability_2022(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
      if (uVar3 != 0)
        goto LAB_022d323d;
      goto LAB_022d31fe;
    }
    instance = get_card_instance(player, card);
    if (instance->kill_code == '\0')
      goto LAB_022d3218;
  LAB_022d30e2:
    if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
      goto LAB_022d31fe;
  LAB_022d30f2:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_022d30fc;
  }
  generate_token_by_id(player, card, 0x497d);
LAB_022d30fc:
  iVar2 = generic_activated_ability(player, card, event, 0, 4, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return iVar2;
}

/* ManalinkEh 2022: 0x22d4810; original cards/dominaria.c:3683. */
int card_the_first_eruption(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  test_definition_t test;

  default_test_definition(&this_test, 2);
  if (event == EVENT_CHAPTER1)
  {
    this_test.keyword = 0x20;
    this_test.keyword_flag = 1;
    result = 1;
  }
  else
  {
    if (event == EVENT_CHAPTER2)
    {
      produce_mana(player, 4, 2);
      goto LAB_022d4858;
    }
    if (event != EVENT_CHAPTER3)
      goto LAB_022d4858;
    new_default_test_definition(&test, 1, "Select a mountain to sacrifice");
    test.subtype = 0x5006;
    result = new_can_sacrifice(player, card, player, &test);
    if (result == 0)
      goto LAB_022d4858;
    result = new_sacrifice(player, card, player, SAC_NO_CANCEL, &test);
    if (result == 0)
      goto LAB_022d4858;
    result = 3;
  }
  new_damage_all(player, card, 2, result, 0, &this_test);
LAB_022d4858:
  result = saga(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x22d6710; original cards/dominaria.c:4499. */
int card_zahid_djinn_of_the_lamp(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  test_definition_t this_test;

  default_test_definition(&this_test, 0x40);
  this_test.state = 0x10;
  this_test.state_flag = 1;
  instance = get_card_instance(player, card);
  if (event == EVENT_MODIFY_COST)
  {
    result = has_mana_multi(player, 3, 0, 1, 0, 0, 0);
    if ((result != 0) &&
        (result = check_battlefield_for_special_card(player, card, player, 0, &this_test),
         result != 0))
    {
      null_casting_cost(player, card);
      instance->info_slot = 1;
      return 0;
    }

    instance->info_slot = 0;
    result = get_updated_casting_cost(player, card, -1, EVENT_MODIFY_COST, 4);
    result = has_mana_multi(player, result, 0, 1, 0, 0, 0);
    if (result != 0)
    {
      return 0;
    }
    infinite_casting_cost();
    return 0;
  }
  if (event != EVENT_CAST_SPELL)
  {
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
  result = played_for_free(player, card);
  if (result != 0)
  {
    return 0;
  }
  result = is_token(player, card);
  if (result != 0)
  {
    return 0;
  }
  if (instance->info_slot != 1)
  {
    return 0;
  }

  result = get_id(player, card);
  result = has_mana_to_cast_id(player, EVENT_CAST_SPELL, result);
  if (result == 0)
  {
  LAB_022d67e1:
    charge_mana_multi(player, 3, 0, 1, 0, 0, 0);
    if (spell_fizzled == 1)
      goto LAB_022d68a1;
    if (player == 1)
    {
      result = check_battlefield_for_special_card(1, card, 1, 2, &this_test);
      if (result == -1)
        goto LAB_022d6a94;
      tap_card(1, result);
    }
    else
    {
      default_target_definition(player, card, &td, 0x40);
      td.allowed_controller = player;
      td.preferred_controller = player;
      td.illegal_abilities = 0;
      td.illegal_state = 0x10;
      result = new_pick_target(&td, "Select an artifact to tap", 0, 0x81);
      if (result != 0)
      {
        tap_card(instance->targets[0].player, instance->targets[0].card);
      }
    }
  }
  else
  {
    result = has_mana_multi(player, 3, 0, 1, 0, 0, 0);
    if (((result != 0) &&
         (result = check_battlefield_for_special_card(player, card, player, 0, &this_test),
          result != 0)) &&
        (result = do_dialog(player, player, card, -1, -1,
                            " Play normally\n Pay 3U and tap an artifact\n Cancel", 1),
         result != 0))
    {
      if (result == 1)
        goto LAB_022d67e1;
    LAB_022d6a94:

      spell_fizzled = 1;
      goto LAB_022d68a1;
    }
    if (instance->info_slot == 1)
    {
      result = get_id(player, card);
      charge_mana_from_id(player, card, EVENT_CAST_SPELL, result);
    }
  }
  if (spell_fizzled != 1)
  {
    return 0;
  }
LAB_022d68a1:
  result = instance->targets[0].card;
  if (result != -1)
  {
    untap_card_no_event(instance->targets[0].player, result);
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int the_mirari_conjecture_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x22caf90; original cards/dominaria.c:163. */
int card_adeliz_the_cinder_wind(int player, int card, event_t event)
{
  test_definition_t wiz_test;

  check_legend_rule(player, card, event);
  haste(player, card, event);
  sp_ability_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  default_test_definition(&wiz_test, 2);
  wiz_test.subtype = 0xd6;
  pump_creatures_until_eot_merge_pt(player, card, player, 1, 1, &wiz_test);
  return 0;
}

/* ManalinkEh 2022: 0x22c9b90; original cards/dominaria.c:3762. */
int the_mirari_conjecture_legacy(int player, int card, event_t event)
{
  int result;

  sp_ability_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event == EVENT_SPA)
  {
    copy_spell_from_stack_2022(player, trigger_cause_controller, trigger_cause);
  }
  result = eot_trigger(player, card, event);
  if (result == 0)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x22d4ba0; original cards/dominaria.c:3774. */
int card_the_mirari_conjecture(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if ((event & ~EVENT_SET_LEGACY_EFFECT_NAME) == EVENT_CHAPTER1)
  {
    if (event == EVENT_CHAPTER1)
    {
      default_test_definition(&test, 0x30);
      test.type_flag = 2;
    }
    else
    {
      default_test_definition(&test, 8);
    }
    result = new_special_count_grave(player, &test);
    if (result != 0)
    {
      result = graveyard_has_shroud(player);
      if (result == 0)
      {
        new_global_tutor(player, player, 2, 1, 0, 1, &test);
      }
    }
  }
  else if (event == EVENT_CHAPTER3)
  {
    create_legacy_effect(player, card, the_mirari_conjecture_legacy);
  }
  result = saga(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x22ca3f0; original cards/dominaria.c:11. */
static void cast_historic_spell_2022(int player, int card, event_t event)
{
  int result;
  if ((trigger_condition == 0xd3) && (affected_card == card))
  {
    if (((affected_card_controller == player) && (player == reason_for_trigger_controller)) &&
        ((result = is_what(trigger_cause_controller, trigger_cause, 0x40),
          result != 0 || ((result = is_legendary(trigger_cause_controller, trigger_cause),
                           result != 0 || (result = has_subtype(trigger_cause_controller,
                                                                trigger_cause, SUBTYPE_SAGA),
                                           result != 0))))))
    {
      result = duh_mode(player);
      sp_ability_trigger(player, event, (result != 0) + 1, TYPE_ANY, NULL, player, card);
    }
    return;
  }
  return;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22cb200; original cards/dominaria.c:229. */
int card_artificers_assistant(int player, int card, event_t event)
{
  cast_historic_spell_2022(player, card, event);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  scry(player, 1);
  return 0;
}

/* ManalinkEh 2022: 0x22ceb10; original cards/dominaria.c:1491. */
int card_jhoira_weatherlight_captain(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  cast_historic_spell_2022(player, card, event);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  draw_a_card(player);
  return 0;
}

/* ManalinkEh 2022: 0x22d4230; original cards/dominaria.c:3548. */
int card_teshar_ancestors_apostle(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  cast_historic_spell_2022(player, card, event);
  if ((event == EVENT_SPA) && (spell_fizzled != 1))
  {

    new_default_test_definition(&this_test, 2, "Select a creature card with CMC 3 or less.");
    this_test.cmc = 4;
    this_test.cmc_flag = 3;
    result = new_special_count_grave(player, &this_test);
    if (0 < result)
    {
      result = graveyard_has_shroud(player);
      if (result == 0)
      {
        new_global_tutor(player, player, 2, 4, 1, 1, &this_test);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22d56b0; original cards/dominaria.c:4090. */
int card_traxos_scourge_of_kroog(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  comes_into_play_tapped(player, card, event);
  does_not_untap(player, card, event);
  cast_historic_spell_2022(player, card, event);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  untap_card(player, card);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22d1690; original cards/dominaria.c:2465. */
int card_naban_dean_of_iteration(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  if (event == EVENT_STATIC_EFFECTS)
  {
    card_counts[player][3] = card_counts[player][3] + '\x01';
  }
  return 0;
}

/* ManalinkEh 2022: 0x22d1980; original cards/dominaria.c:2535. */
int card_oath_of_teferi(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  target_definition_t td;

  check_legend_rule(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    default_target_definition(player, card, &td, 0x47);
    td.allowed_controller = player;
    td.allow_cancel = 0;
    td.special = 0x4000;
    instance = get_card_instance(player, card);
    iVar1 = can_target(&td);
    if ((iVar1 != 0) &&
        (iVar1 = new_pick_target(&td, "Select another target permanent you control.", 0, 0x80),
         iVar1 != 0))
    {
      remove_until_eot(player, card, instance->targets[0].player, instance->targets[0].card);
    }
    instance->number_of_targets = '\0';
    leaves_play_2022(player, card, EVENT_ETB, 0, 0);
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    iVar1 = get_id(player, card);
    iVar1 = count_cards_by_id(player, iVar1);

    if ((iVar1 == 1) && (0 < active_cards_count[player]))
    {
      iVar1 = 0;
      do
      {
        instance = in_play(player, iVar1);
        if ((instance != NULL) && (iVar3 = is_what(player, iVar1, 0x100), iVar3 != 0))
        {
          instance = get_card_instance(player, iVar1);
          instance->targets[9].card = instance->targets[9].card + 1;
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < active_cards_count[player]);
    }
    leaves_play_2022(player, card, EVENT_RESOLVE_SPELL, 0, 0);
  }
  else
  {
    leaves_play_2022(player, card, event, 0, 0);

    if ((((event == EVENT_LEAVES_PLAY_ABILITY) &&
          (iVar1 = is_humiliated_for_ability(player, card, AL_STATIC), iVar1 == 0)) &&
         (iVar1 = check_battlefield_for_id(player, 0x482a), iVar1 == 0)) &&
        (iVar1 = 0, 0 < active_cards_count[player]))
    {
      do
      {
        instance = in_play(player, iVar1);
        if ((instance != NULL) && (iVar3 = is_what(player, iVar1, 0x100), iVar3 != 0))
        {
          instance = get_card_instance(player, iVar1);
          instance->targets[9].card = instance->targets[9].card + -1;
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < active_cards_count[player]);
    }
  }
  iVar1 = global_enchantment(player, card, event);
  return iVar1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22ccd40; original cards/dominaria.c:748. */
int card_curators_ward(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int result;
  target_definition_t td;

  pcVar1 = get_card_instance(player, card);
  other_leaves_play_2022(player, card, (int)pcVar1->damage_target_player,
                         pcVar1->damage_target_card, event, 1, 1);
  if (event == EVENT_LEAVES_PLAY_ABILITY)
  {
    result = is_what((int)pcVar1->damage_target_player, pcVar1->damage_target_card, 0x40);
    if (((result != 0) ||
         (result = is_legendary((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
          result != 0)) ||
        (result = has_subtype((int)pcVar1->damage_target_player, pcVar1->damage_target_card,
                              SUBTYPE_SAGA),
         result != 0))
    {
      draw_cards(player, 2);
    }
  LAB_022ccdb8:
    if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) == EVENT_CAN_MOVE_AURA) ||
         (event - EVENT_POWER < 3)) ||
        (event == EVENT_RESOLVE_MOVING_AURA))
    {
    LAB_022cce23:
      if (event == EVENT_ABILITIES)
      {
        if (((pcVar1->damage_target_card == affected_card) &&
             (pcVar1->damage_target_player == affected_card_controller)) &&
            ((pcVar2 = in_play(player, card),
              pcVar2 != NULL &&
                  (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0))))
        {
          hexproof_2022((int)pcVar1->damage_target_player, pcVar1->damage_target_card, EVENT_ABILITIES);
        }
        goto LAB_022cce88;
      }
    }
    else if (((event != EVENT_CHANGE_TYPE) && (event != EVENT_SET_COLOR)) &&
             ((event != EVENT_GET_SELECTED_CARD && (event != EVENT_STATIC_EFFECTS))))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x47);
    if (event != EVENT_CAST_SPELL)
      goto LAB_022cceac;
  LAB_022ccf02:
    td.preferred_controller = player;
    if ((card == affected_card) && (player == affected_card_controller))
    {
      generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_PERMANENT", 1, NULL);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      result = get_cmc(pcVar1->targets[0].player, pcVar1->targets[0].card);
      ai_modifier = ai_modifier + result * 8;
      return 0;
    }
  }
  else
  {
    if (event != EVENT_CAN_CAST)
    {
      if (event == EVENT_CAST_SPELL)
      {
        if ((card != affected_card) || (player != affected_card_controller))
          goto LAB_022ccdb8;
        default_target_definition(player, card, &td, 0x47);
        goto LAB_022ccf02;
      }
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
        goto LAB_022cce23;
      if (event != EVENT_CHANGE_TARGET)
        goto LAB_022ccdb8;
    }
  LAB_022cce88:
    default_target_definition(player, card, &td, 0x47);
  }
LAB_022cceac:
  td.preferred_controller = player;
  result = targeted_aura(player, card, event, &td, "TARGET_PERMANENT");
  return result;
}

/* ManalinkEh 2022: 0x22d5bc0; original cards/dominaria.c:4245. */
int card_valduk_keeper_of_the_flame(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  token_generation_t token;

  check_legend_rule(player, card, event);
  iVar1 = beginning_of_combat_2022(player, card, event, player);
  if (iVar1 == 0)
  {
    return 0;
  }
  default_token_definition(player, card, 0x4926, &token);
  token.pow = 3;
  token.tou = 1;
  token.color_forced = '\x10';
  token.key_plus = 0x80;
  iVar1 = equipments_attached_to_me(player, card, 1);
  iVar2 = count_auras_enchanting_me(player, card);
  token.qty = (char)iVar2 + (char)iVar1;
  token.special_code_on_generation = haste_and_remove_token_at_end_of_next_turn;
  generate_token(&token);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int is_historic_by_internal_id(int iid, int unused, int player, int card);

/* ManalinkEh 2022: 0x22ca500; original cards/dominaria.c:3. */
static int is_historic_by_internal_id(int iid, int unused, int player, int card)
{
  int result;

  if ((cards_data[iid].type & 0x40) == 0)
  {
    result = is_legendary(-1, iid);
    if (result == 0)
    {
      result = has_subtype_by_id_2022(player, (unsigned int)cards_data[iid].id, SUBTYPE_SAGA);
      return (unsigned int)(result != 0);
    }
  }
  return 1;
}

/* ManalinkEh 2022: 0x22cc2b0; original cards/dominaria.c:514. */
int card_board_the_weatherlight(int player, int card, event_t event)
{
  int result;
  int revealed_card;
  test_definition_t this_test;

  if (event == EVENT_CAN_CAST)
    goto LAB_022cc2e8;
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
    goto LAB_022cc2e8;
  }
  if (event != EVENT_RESOLVE_SPELL)
  {
    if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
    goto LAB_022cc2e8;
  }

  result = count_deck(player);
  if (result != 0)
  {
    if (5 < result)
    {
      result = 5;
    }

    new_default_test_definition(&this_test, 0xff, "Select an historic card.");
    this_test.create_minideck = (unsigned short)result;
    this_test.special_selection_function = is_historic_by_internal_id;
    this_test.no_shuffle = 1;
    revealed_card = new_global_tutor(player, player, 1, 1, 0, 1, &this_test);
    if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
    {
      if (revealed_card < 0)
      {
        this_test.type = 0xff;
        revealed_card = new_global_tutor(player, player, 1, 1, 0, 1, &this_test);
        goto LAB_022cc3e8;
      }
    LAB_022cc425:
      reveal_card(player, card, player, revealed_card);
      result = result + -1;
    }
    else
    {
    LAB_022cc3e8:
      if (-1 < revealed_card)
        goto LAB_022cc425;
    }
    if (0 < result)
    {
      put_top_x_on_bottom_in_random_order(player, result);
    }
  }
  kill_card(player, card, KILL_DESTROY);
LAB_022cc2e8:
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22d5d40; original cards/dominaria.c:4310. */
int card_weatherlight(int player, int card, event_t event)
{
  int result;
  int revealed_card;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  result =
      damage_dealt_by_me(player, card, event, DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_BE_COMBAT_DAMAGE);
  if (result == 0)
    goto LAB_022d5d81;

  result = count_deck(player);
  if (result == 0)
    goto LAB_022d5d81;
  if (5 < result)
  {
    result = 5;
  }

  new_default_test_definition(&this_test, 0xff, "Select an historic card.");
  this_test.create_minideck = (unsigned short)result;
  this_test.special_selection_function = is_historic_by_internal_id;
  this_test.no_shuffle = 1;
  revealed_card = new_global_tutor(player, player, 1, 1, 0, 1, &this_test);
  if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
  {
    if (revealed_card < 0)
    {
      this_test.type = 0xff;
      revealed_card = new_global_tutor(player, player, 1, 1, 0, 1, &this_test);
      goto LAB_022d5e45;
    }
  LAB_022d5e75:
    reveal_card(player, card, player, revealed_card);
    result = result + -1;
  }
  else
  {
  LAB_022d5e45:
    if (-1 < revealed_card)
      goto LAB_022d5e75;
  }
  if (0 < result)
  {
    put_top_x_on_bottom(player, player, result);
  }
LAB_022d5d81:
  result = vehicle(player, card, event, 3);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static char *can_cast_historic_spell(int who_chooses, int player, int card, int targeting_player,
                                     int targeting_card);

/* ManalinkEh 2022: 0x22ce550; original cards/dominaria.c:1356. */
int card_helm_of_the_host(int player, int card, event_t event)
{
  enum
  {
    TOKEN2022_ACTION_ISNT_LEGENDARY = 256
  };

  int result;
  card_instance_t *instance;
  token_generation_2022_t token;

  result = beginning_of_combat_2022(player, card, event, player);
  if (result != 0)
  {
    instance = is_equipping(player, card);
    if (instance != NULL)
    {
      copy_token_definition_2022(player, card, &token, (int)instance->damage_target_player,
                            instance->damage_target_card);
      token.special_code_on_generation = give_token_haste;
      token.action = TOKEN2022_ACTION_ISNT_LEGENDARY;
      generate_token_2022(&token);
    }
  }
  result = basic_equipment(player, card, event, 5);
  return result;
}

/* ManalinkEh 2022: 0x22c97b0; original cards/dominaria.c:2728. */
static char *can_cast_historic_spell(int who_chooses, int player, int card, int targeting_player,
                                     int targeting_card)
{
  enum
  {
    EVENT_0 = 0
  };

  int result;
  char *pcVar2;

  result = is_what(player, card, 0x40);
  if (result == 0)
  {
    result = is_legendary(player, card);
    if (result == 0)
    {
      result = has_subtype(player, card, SUBTYPE_SAGA);
      if (result == 0)
      {
        return "must be historic";
      }
    }
  }
  result = get_id(player, card);
  result = has_mana_to_cast_id(who_chooses, EVENT_0, result);
  if (result == 0)
  {
    pcVar2 = "not enough mana";
  }
  else
  {
    result = get_id(player, card);
    result = can_legally_play_csvid(who_chooses, result);
    pcVar2 = "can\'t play this card";
    if (result != 0)
    {
      result = is_what(player, card, 1);
      pcVar2 = "not a spell";
      if (result == 0)
      {
        pcVar2 = NULL;
      }
    }
  }
  return pcVar2;
}

/* ManalinkEh 2022: 0x22d2410; original cards/dominaria.c:2740. */
int card_raff_capashen_ships_mage(int player, int card, event_t event)
{
  int result;
  const char *prompt;
  target_definition_t td;

  check_legend_rule(player, card, event);
  result = flash(player, card, event);
  if (result == 0)
  {
    if ((event - EVENT_RESOLVE_ACTIVATION < 2) || (result = 0, event == EVENT_ACTIVATE))
    {
      base_target_definition(player, card, &td, 0xff);
      td.preferred_controller = player;
      td.allowed_controller = player;
      td.special = 0x8000;
      td.extra = (int32_t)can_cast_historic_spell;
      td.zone = 0x100;
      if (event == EVENT_ACTIVATE)
      {
        prompt = "Select an historic card.";
      }
      else
      {
        prompt = NULL;
      }
      result = can_play_cards_as_though_they_had_flash(player, card, event, &td, prompt, 1);
    }
    return result;
  }
  return 1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22cfe60; original cards/dominaria.c:1888. */
int card_knight_of_grace(int player, int card, event_t event)
{
  unsigned int uVar1;
  unsigned int uVar2;
  int result;

  misc_hexproof(player, card, event);
  if (event == EVENT_PROT_MISC)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      uVar1 = get_color(attacking_card_controller, attacking_card);
      uVar2 = get_sleighted_color_test(player, card, 2);
      if ((uVar1 & uVar2) != 0)
      {
        event_result = event_result | 1;
      }
    }
  }
  else if (((event == EVENT_POWER) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    result = get_sleighted_color_test(player, card, 2);
    result = count_permanents_by_color(2, 0x47, result);
    if (result != 0)
    {
      event_result = event_result + 1;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22cff50; original cards/dominaria.c:1906. */
int card_knight_of_malice(int player, int card, event_t event)
{
  unsigned int uVar1;
  unsigned int uVar2;
  int result;

  misc_hexproof(player, card, event);
  if (event == EVENT_PROT_MISC)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      uVar1 = get_color(attacking_card_controller, attacking_card);
      uVar2 = get_sleighted_color_test(player, card, 0x20);
      if ((uVar1 & uVar2) != 0)
      {
        event_result = event_result | 1;
      }
    }
  }
  else if (((event == EVENT_POWER) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    result = get_sleighted_color_test(player, card, 0x20);
    result = count_permanents_by_color(2, 0x47, result);
    if (result != 0)
    {
      event_result = event_result + 1;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22d2be0; original cards/dominaria.c:2943. */
int card_shalai_voice_of_plenty(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  test_definition_t this_test;
  check_legend_rule(player, card, event);
  instance = in_play(player, card);
  if ((instance != NULL) &&
      (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0))
  {
    give_hexproof_to_player(player, card, event);
  }
  if (event == EVENT_ABILITIES)
  {
    instance = in_play(player, card);
    if ((((instance != NULL) &&
          (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)) &&
         (result = is_what(affected_card_controller, affected_card, 0x102), result != 0)) &&
        ((affected_card_controller == player && (affected_card != card))))
    {
      instance = get_card_instance(player, affected_card);
      instance->ext->sp_keywords = instance->ext->sp_keywords | (SP_KEYWORD_HEXPROOF);
    }
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_022d2c91:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_022d2c96;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_022d2c91;
  }
  default_test_definition(&this_test, 2);
  new_manipulate_all(player, card, player, &this_test, ACT_ADD_COUNTERS_BASE | ACT_KILL_BURY);
LAB_022d2c96:
  result = generic_activated_ability(player, card, event, 0, 4, 0, 0, 2, 0, 0, 0, NULL, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22cc670; original cards/dominaria.c:583. */
int card_cabal_stronghold(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uVar4;
  bool bVar5;
  unsigned int uStack_14;

  if (event == EVENT_RESOLVE_SPELL)
  {
    instance = get_card_instance(player, card);
    instance->info_slot = 1;
    play_land_sound_effect_force_color(player, card, 2);
    count_mana_2022();
  LAB_022cc73c:
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      get_card_instance(player, card);
    }
    else
    {
      if (event != EVENT_CLEANUP)
      {
      LAB_022cc6fb:
        iVar2 = mana_producer(player, card, event);
        return iVar2;
      }
      get_card_instance(player, card);
    }
    return 0;
  }
  if (event == EVENT_CHANGE_TYPE)
  {
    iVar2 = has_mana(player, 0, 4);
    if (iVar2 == 0)
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 1;
    }
    else
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 2;
    }
    goto LAB_022cc6fb;
  }
  if (event == EVENT_CAN_ACTIVATE)
    goto LAB_022cc6fb;
  if (event != EVENT_ACTIVATE)
    goto LAB_022cc73c;
  instance = get_card_instance(player, card);

  iVar2 = paying_mana();
  if ((iVar2 == 0) && (iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 4, 0,
                                                         0, 0, 0, 0, 0, NULL, NULL),
                       iVar2 != 0))
  {
    bVar5 = ai_is_speculating != 1;
    instance->info_slot = 2;
    if ((bVar5) && ((player != 1 || (((unsigned char)trace_mode & 2) != 0))))
    {
      iVar2 = get_hacked_subtype(player, card, 0x5009);
      iVar2 = count_basic_lands(player, iVar2);
      iVar3 = is_tapped(player, card);
      if ((iVar3 == 0) && ((iVar3 = is_animated_and_sick(player, card),
                            iVar3 == 0 && (iVar3 = can_produce_mana_2022(player, card), iVar3 != 0))))
      {
        iVar3 = has_mana(player, 0, 4);
        uStack_14 = (unsigned int)(iVar3 != 0);
      }
      else
      {
        uStack_14 = 0;
      }
      iVar3 = is_tapped(player, card);
      if ((iVar3 == 0) && (iVar3 = is_animated_and_sick(player, card), iVar3 == 0))
      {
        iVar3 = can_produce_mana_2022(player, card);
        uVar4 = (unsigned int)(iVar3 != 0);
      }
      else
      {
        uVar4 = 0;
      }
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "\001\016",
                          "Produce colorless mana", uVar4, 5, "Raid the Coffers", uStack_14,
                          (iVar2 < 4) - 1 & 10, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 == 1)
        goto LAB_022cc783;
      if (iVar2 != 2)
      {
        return 0;
      }
    }
    add_state(player, card, 0x10);
    charge_mana(player, COLOR_ANY, 3);
    if (spell_fizzled == 1)
    {
      remove_state(player, card, 0x10);
    }
    else
    {

      iVar2 = get_hacked_subtype(player, card, 0x5009);
      iVar2 = count_basic_lands(player, iVar2);
      produce_mana_tapped_impl(player, card, COLOR_BLACK, iVar2, 0, 1);
      ai_modifier = ai_modifier + ((3 < iVar2) - 1 & 0xffffff70) + 0x30;
    }
  }
  else
  {
    instance->info_slot = 1;
  LAB_022cc783:
    produce_mana_tapped_2022(player, card, COLOR_COLORLESS, 1);
  }
  return 0;
}

/* ManalinkEh 2022: 0x22cd050; original cards/dominaria.c:786. */
int card_damping_sphere(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_MODIFY_MANA_PROD)
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        multi_mana_transmutation[1][0] = 1;
        multi_mana_transmutation[0][0] = 1;
        multi_mana_transmutation[1][1] = 1;
        multi_mana_transmutation[0][1] = 1;
      }
    }
  }
  instance = in_play(player, card);
  if (instance != NULL)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if ((event == EVENT_MODIFY_COST_GLOBAL) && (result == 0))
    {
      result = is_what(affected_card_controller, affected_card, 1);
      if (result == 0)
      {
        result = get_specific_storm_count(affected_card_controller);
        COST_COLORLESS = COST_COLORLESS + (char)result;
      }
    }
  }
  return 0;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22cd3d0; original cards/dominaria.c:877. */
int card_deep_freeze(int player, int card, event_t event)
{
  int result;

  attached_set_pt(player, card, event, 0, 4);
  attached_is_additional_color(player, card, event, COLOR_TEST_BLUE);
  attached_is_additional_subtype(player, card, event, SUBTYPE_WALL);
  disable_attached(player, card, event, 0x17e);
  result = aura(player, card, event, NULL, NULL);
  disabling_aura_ai(player, card, event);
  return result;
}
#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22d04c0; original cards/dominaria.c:2011. */
int card_lichs_mastery(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  check_legend_rule(player, card, event);
  hexproof_2022(player, card, event);
  cannot_lose_the_game_2022(player, card, event, player);
  result = trigger_gain_life_2022(player, card, event, player, RESOLVE_TRIGGER_MANDATORY);
  if ((result != 0) && (instance = in_play(player, card), instance != NULL))
  {
    draw_cards(player, result);
  }
  if ((((trigger_condition == 0xd4) &&
        (result = is_humiliated_for_ability(player, card, AL_TRIG), result == 0)) &&
       (card == affected_card)) &&
      (((player == affected_card_controller && (trigger_cause == card)) &&
        ((trigger_cause_controller == player && (reason_for_trigger_controller == player))))))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result = event_result | 2;
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {
      suspend_cannot_lose_the_game = 1;
      lose_the_game_2022(player);
    }
    else if (event == EVENT_END_TRIGGER)
    {
      suspend_cannot_lose_the_game = 0;
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 Lichs Mastery life-loss helpers. */

typedef union
{
  char text[100];
  test_definition_2022_t test;
} lichs_mastery_storage_2022_t;

/* ManalinkEh 2022: 0x22ca570; original cards/dominaria.c:1949. */
static char *target_only_permanents_unless_in_hand_2022(int who_chooses, int player, int card)
{
  card_instance_t *instance;
  int result;
  instance = in_hand(player, card);
  if (instance == NULL)
  {
    result = is_what(player, card, 0x47);
    if (result == 0)
    {
      return (char *)EXE_STR(0x728F6C);
    }
  }
  return NULL;
}


/* ManalinkEh 2022: 0x22d0100; original cards/dominaria.c:1954. */
void lichs_mastery_lifeloss_trigger_2022(int player, int card, int amount)
{
  target_definition_2022_t td;
  lichs_mastery_storage_2022_t storage;
  default_target_definition_2022(player, card, &td, 0);
  td.zone = 0x300;
  td.allowed_controller = player;
  td.preferred_controller = player;
  td.illegal_abilities = 0;
  td.allow_cancel = 0;
  td.special = TARGET_SPECIAL_EXTRA_FUNCTION;
  td.extra = (int32_t)target_only_permanents_unless_in_hand_2022;
  int grave[amount];
  target_t tgts[amount + 1];
  card_instance_t *instance = get_card_instance(player, card);
  int num_tgts = 0;
  int i = 0;
  while (1)
  {
    int available = can_target_2022(&td);
    if ((!available && get_grave(player)[0] == -1) || amount <= i)
      break;
    int grave_available = get_grave(player)[0] != -1;
    int hand_or_play_available = can_target_2022(&td);
    int choice =
        dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\014", "\001\003",
                    "\001\026", "Exile from hand or play", hand_or_play_available, 5,
                    "Exile from graveyard", grave_available, 10, 0, 0, 0, 0);
    int next_count = num_tgts + 1;
    if (choice == 1)
    {
      scnprintf(storage.text, 100,
                "Select a permanent you control or a card from your hand to exile. (%d of %d).",
                next_count, amount);
      if (select_target_2022(player, card, &td, storage.text, &tgts[i]))
      {
        instance->number_of_targets = 0;
        state_untargettable(tgts[i].player, tgts[i].card, 1);
        num_tgts = next_count;
        grave[i] = 0;
      }
      ++i;
    }
    else
    {
      default_test_definition_2022(&storage.test, 0xff);
      scnprintf(storage.text, 100, "Select a card to exile (%d of %d).", next_count, amount);
      new_global_tutor_2022(player, player, TUTOR_FROM2022_GRAVE, TUTOR2022_RFG, 1, 3,
                            &storage.test);
      num_tgts = next_count;
      grave[i] = 1;
      ++i;
    }
  }
  for (i = 0; i < num_tgts; ++i)
    if (!grave[i])
      state_untargettable(tgts[i].player, tgts[i].card, 0);
  for (i = 0; i < num_tgts; ++i)
  {
    if (grave[i])
      continue;
    if (in_hand(tgts[i].player, tgts[i].card))
    {
      put_on_top_of_deck(tgts[i].player, tgts[i].card);
      rfg_top_card_of_deck(tgts[i].player);
    }
    else
      kill_card(tgts[i].player, tgts[i].card, KILL_EXILE);
  }
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22d2db0; original cards/dominaria.c:2976. */
int card_shanna_sisays_legacy(int player, int card, event_t event)
{
  int result;

  check_legend_rule(player, card, event);
  if (event - EVENT_POWER < 2)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = count_permanents_by_type(player, TYPE_CREATURE);
      event_result = event_result + result;
    }
  }
  else if ((((event == EVENT_PROT_MISC) && (card == affected_card)) &&
            (player == affected_card_controller)) &&
           ((((unsigned int)event_result >> 24 & 0xff) == '\x05' &&
             (attacking_card_controller != player))))
  {
    cannot_be_target_of_abilities(player, card, EVENT_PROT_MISC);
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x22ca5c0; original cards/dominaria.c:20. */
static int dominaria_memorial_2022_impl(int player, int card, event_t event, int x, int b, int u, int g,
                                 int r, int w, int target_in_grave, const char *label,
                                 target_definition_2022_t *td, char *prompt,
                                 void (*impl)(int, int, target_definition_2022_t *))
{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  card_instance_t *instance;
  unsigned int uVar6;
  const char *puStack_ec;
  test_definition_2022_t test;
  if ((event != EVENT_RESOLVE_ACTIVATION) &&
      (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE))
  {
  LAB_022ca5ef:
    iVar1 = mana_producer_tapped(player, card, event);
    return iVar1;
  }
  new_default_test_definition_2022(&test, 2, "Select target creature card.");
  iVar1 = landsofcolor_controlled[player][7];
  iVar2 = paying_mana();
  uVar6 = 0;
  if (iVar2 == 0)
  {
    if (target_in_grave == 0)
    {
      iVar2 = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE,
                                             (-(unsigned int)(td == NULL) & 0xfffffffc) + 0xd,
                                             x + 1, b, u, g, r, w, 0, td, prompt);
      uVar6 = (unsigned int)(iVar2 != 0);
    }
    else
    {
      iVar2 = generic_activated_ability_with_target_in_grave_2022(
          player, card, EVENT_CAN_ACTIVATE, GAA_SACRIFICE_ME | GAA_UNTAPPED, x + 1, b, u, g, r, w,
          0, NULL, prompt, TYPE_NONE, &test, 8);
      uVar6 = (unsigned int)(iVar2 != 0);
    }
  }
  iVar2 = paying_mana();
  iVar3 = mana_producer(player, card, EVENT_CAN_ACTIVATE);
  iVar4 = paying_mana();
  puStack_ec = "\001\026";
  if (iVar4 == 0)
  {
    puStack_ec = "\001\027";
  }
  iVar1 = dialog_impl(player, card, event, "\001\016", puStack_ec, "Produce mana", iVar3, iVar2,
                      label, uVar6, iVar1 + -6, 0, 0, 0, 0);
  if (event == EVENT_CAN_ACTIVATE)
  {
    return iVar1;
  }
  if (iVar1 == 0)
  {
    return 0;
  }
  if (iVar1 != 2)
    goto LAB_022ca5ef;
  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE)
  {
    (*impl)(player, card, td);
    return 0;
  }
  add_state(player, card, 0x10);
  charge_mana_for_activated_ability_2022(player, card, x, b, u, g, r, w);
  if (target_in_grave == 0)
  {
    if (td != NULL)
    {
      if (spell_fizzled == 1)
        goto LAB_022ca96e;
      instance = get_card_instance(player, card);
      if ((td->zone == 0x1000) && (td->allowed_controller == 1 - player))
      {
        instance->targets[0].player = 1 - player;
        instance->targets[0].card = -1;
        instance->number_of_targets = '\x01';
      }
      else
      {
        instance->number_of_targets = '\0';
        pick_target_2022(td, prompt);
      }
    }
  }
  else
  {
    if (spell_fizzled == 1)
      goto LAB_022ca96e;
    iVar1 = select_target_from_grave_source_2022(player, card, player, SFG_0, 2, &test, 0);
    if (iVar1 == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
  }
  if (spell_fizzled != 1)
  {
    kill_card(player, card, KILL_SACRIFICE);
    tapped_for_mana_color = -1;
    return 0;
  }
LAB_022ca96e:
  remove_state(player, card, 0x10);
  tapped_for_mana_color = -1;
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x22d07e0; original cards/dominaria.c:2109. */
int card_memorial_to_folly(int player, int card, event_t event)
{
  int result;
  result = dominaria_memorial_2022_impl(player, card, event, 2, 1, 0, 0, 0, 0, 1,
                                        "Return a creature to your hand", NULL,
                                        "Select target creature card.", memorial_to_folly_impl);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x22d0850; original cards/dominaria.c:2122. */
int card_memorial_to_genius(int player, int card, event_t event)
{
  int result;
  result = dominaria_memorial_2022_impl(player, card, event, 4, 0, 1, 0, 0, 0, 0, "Draw", NULL,
                                        NULL, memorial_to_genius_impl);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x22d08c0; original cards/dominaria.c:2134. */
int card_memorial_to_glory(int player, int card, event_t event)
{
  int result;
  result = dominaria_memorial_2022_impl(player, card, event, 3, 0, 0, 0, 0, 1, 0, "Create tokens",
                                        NULL, NULL, memorial_to_glory_impl);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x22d0930; original cards/dominaria.c:2160. */
int card_memorial_to_unity(int player, int card, event_t event)
{
  int result;
  result = dominaria_memorial_2022_impl(player, card, event, 2, 0, 0, 1, 0, 0, 0,
                                        "Dig for a creature", NULL, NULL, memorial_to_unity_impl);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x22d09a0; original cards/dominaria.c:2173. */
int card_memorial_to_war(int player, int card, event_t event)
{
  int result;
  target_definition_2022_t td;
  if (((unsigned int)(event - EVENT_RESOLVE_ACTIVATION) < 2) || (event == EVENT_ACTIVATE))
  {
    default_target_definition_2022(player, card, &td, 1);
  }
  result = dominaria_memorial_2022_impl(player, card, event, 4, 0, 0, 0, 1, 0, 0, "Destroy a land",
                                        &td, "TARGET_LAND", memorial_to_war_impl);
  return result;
}

#endif
