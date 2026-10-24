// If any of these are implemented, they need to be put in
// functions/rules_engine.c:check_deck_legality()::banned_cards_vintage[] and banned_cards_legacy[], and in
// cards/vanguard.c:is_banned_from_edh().

#include "manalink.h"

/*********
 * White *
 *********/

/* Adorable // Kitten	|W
 * Host Creature - Cat 1/1
 * When this creature enters the battlefield,
 * //
 * roll a six-sided die. You gain life equal to the result. */

/* Aerial Toastmaster	|3|W
 * Artifact Creature - Cyborg Rigger 3/2
 * Flying
 * |3|W, Sacrifice another artifact: ~ assembles a Contraption. (Put the top card of your Contraption deck face up onto one of your sprockets.) */

/* Amateur Auteur	|1|W
 * Creature - Human 2/2
 * Sacrifice ~: Destroy target enchantment. */

/* By Gnome Means	|2|W
 * Enchantment
 * |1|W, Remove a counter from a permanent you control: Create a 1/1 colorless Gnome artifact creature token.
 * |1|W, Sacrifice an artifact: Choose any kind of counter a printed card refers to, then put one of that counter on target permanent. */

/* Chivalrous Chevalier	|4|W
 * Artifact Creature - Cyborg Knight 3/3
 * Flying
 * When ~ enters the battlefield, return a creature you control to its owner's hand unless you compliment an opponent. */

/* Do-It-Yourself Seraph	|4|W|W
 * Artifact Creature - Cyborg Angel 4/4
 * Flying
 * Whenever ~ attacks, you may search your library for an artifact card, exile it, then shuffle your library.
 * ~ has the text box of each card exiled with ~ in addition to its own. */

/* Gimme Five	|W
 * Instant
 * You gain 1 life for each person who high-fives you in the next thirty seconds. Each player in a silver-bordered game who high-fives you gains 1 life. (Offer high fives. Don't hit people.) */

/* GO TO JAIL	|W
 * Enchantment
 * When ~ enters the battlefield, exile target creature an opponent controls until ~ leaves the battlefield.
 * At the beginning of the upkeep of the exiled card's owner, that player rolls two six-sided dice. If they roll doubles, sacrifice ~. */

/* Half-Kitten, Half-	""
 * Creature - Cat 1/2
 * Whenever you're dealt damage,
 * Augment |2|W (|2|W, Reveal this card from your hand: Combine it with target host. Augment only as a sorcery.) */

/* Humming-	""
 * Creature - Bird 2/3
 * Flying
 * Whenever you attack with two or more creatures,
 * Augment |3|W (|3|W, Reveal this card from your hand: Combine it with target host. Augment only as a sorcery.) */

/* Jackknight	|1|W
 * Artifact Creature - Cyborg Knight 1/1
 * Whenever another artifact enters the battlefield under your control, put a +1/+1 counter on ~. If that artifact is a Contraption, ~ gains lifelink until end of turn. */

/* Knight of the Kitchen Sink A	|W|W
 * Artifact Creature - Cyborg Knight 2/2
 * First strike, protection from black borders (Nothing with a black border can block, target, deal damage to, or attach to this creature.) */

/* Knight of the Kitchen Sink B	|W|W
 * Artifact Creature - Cyborg Knight 2/2
 * First strike, protection from even collector numbers (Nothing with an even collector number can block, target, deal damage to, or attach to this creature.) */

/* Knight of the Kitchen Sink C	|W|W
 * Artifact Creature - Cyborg Knight 2/2
 * First strike, protection from loose lips (Nothing with an open mouth in its artwork can block, target, deal damage to, or attach to this creature.) */

/* Knight of the Kitchen Sink D	|W|W
 * Artifact Creature - Cyborg Knight 2/2
 * First strike, protection from odd collector numbers (Nothing with an odd collector number can block, target, deal damage to, or attach to this creature.) */

/* Knight of the Kitchen Sink E	|W|W
 * Artifact Creature - Cyborg Knight 2/2
 * First strike, protection from two-word names (Nothing with exactly two words in its name can block, target, deal damage to, or attach to this creature. Hyphenated words are one word.) */

/* Knight of the Kitchen Sink F	|W|W
 * Artifact Creature - Cyborg Knight 2/2
 * First strike, protection from watermarks (Nothing with a watermark can block, target, deal damage to, or attach to this creature.) */

/* Knight of the Widget	|2|W
 * Artifact Creature - Cyborg Knight 100/100
 * Vigilance
 * ~'s power and toughness are each equal to the number of Order of the Widget watermarks among permanents you control. */

/* Midlife Upgrade	|3|W
 * Sorcery
 * As an additional cost to cast this spell, sacrifice X Contraptions.
 * Assemble X plus one Contraptions. (To assemble a Contraption, put the top card of your Contraption deck face up onto one of your sprockets.) */

/* Oddly Uneven	|3|W|W
 * Sorcery
 * Choose one -
 * * Destroy each creature with an odd number of words in its name. (Hyphenated words are one word.)
 * * Destroy each creature with an even number of words in its name. */

/* Old Guard	|1|W
 * Artifact Creature - Cyborg Soldier 2/1
 * |W, |T: Tap target creature without reminder text. (Reminder text is still any italicized text in parentheses that explains rules you already know.) */

/* Ordinary // Pony	|2|W
 * Host Creature - Horse 2/3
 * When this creature enters the battlefield,
 * //
 * you may exile target non-Horse creature you control that wasn't put onto the battlefield with this ability this turn, then return it to the battlefield under its owner's control. */

/* Rhino-	""
 * Creature - Rhino 1/4
 * Whenever this creature blocks,
 * Augment |3|W (|3|W, Reveal this card from your hand: Combine it with target host. Augment only as a sorcery.) */

/* Riveting Rigger	|2|W
 * Artifact Creature - Cyborg Rigger 2/2
 * When ~ enters the battlefield, you may sacrifice another artifact. If you do, put two +1/+1 counters on ~ and it assembles a Contraption. (Put the top card of your Contraption deck face up onto one of your sprockets.) */

/* Rules Lawyer	|3|W|W
 * Artifact Creature - Cyborg Advisor 1/1
 * State-based actions don't apply to you or other permanents you control. (You don't lose the game due to having 0 or less life or drawing from an empty library. Your creatures aren't destroyed due to damage or deathtouch and aren't put into a graveyard due to having 0 or less toughness. Your planeswalkers aren't put into a graveyard if they have 0 loyalty. You don't put a legendary permanent into a graveyard if you control two with the same name. Counters aren't removed from your permanents due to game rules. Permanents you control attached or combined illegally remain on the battlefield. For complete rules and regulations, see rule 704.) */

/* Sacrifice Play	|2|W
 * Instant
 * A person outside the game chooses an attacking or blocking creature target opponent controls. That player sacrifices that creature. */

/* Shaggy // Camel	|3|W
 * Host Creature - Camel 3/3
 * When this creature enters the battlefield,
 * //
 * creatures you control get +1/+1 until end of turn. */

/* Side Quest	|W
 * Sorcery
 * Target player in a silver-bordered game you can see from your seat gains control of target creature you control until your next turn. At the beginning of your next upkeep, put two +1/+1 counters on that creature. (A creature is on the battlefield of only the game its controller is playing.) */

/* Success!	|W
 * Instant
 * Target creature gets +2/+2 until end of turn. If it's a host or has augment, it gains lifelink until end of turn. */

/* Teacher's Pet	|1|W
 * Creature - Cat Bird Scientist 2/1
 * |2|W, Sacrifice ~: Search your library for a card with augment, combine it with target host you control, then shuffle your library. */

/********
 * Blue *
 ********/

/* Animate Library	|4|U|U
 * Enchantment - Aura
 * Enchant your library
 * Enchanted library is an artifact creature on the battlefield with power and toughness each equal to the number of cards in it. It's still a library.
 * If enchanted library would leave the battlefield, exile ~ instead. */

/* Blurry Beeble	|U
 * Artifact Creature - Cyborg Beeble 1/1
 * Blurry (This creature can be blocked only if defending player was wearing glasses as it was cast.)
 * Whenever ~ deals combat damage to a player, you may draw a card. If you do, discard a card. */

/* Chipper Chopper	|3|U
 * Artifact Creature - Cyborg Rigger 1/1
 * Flying
 * When ~ enters the battlefield, you may sacrifice another artifact. If you do, put two +1/+1 counters on ~ and it assembles a Contraption. (Put the top card of your Contraption deck face up onto one of your sprockets.) */

/* Clocknapper	|3|U|U
 * Creature - Human Spy 2/2
 * When ~ enters the battlefield, choose beginning phase, precombat main phase, combat phase, postcombat main phase, or ending phase. Steal that phase from target player during their next turn. (That phase occurs as though it's your turn instead.) */

/* Crafty // Octopus	|2|U
 * Host Creature - Octopus Rigger 1/3
 * When this creature enters the battlefield,
 * //
 * this creature assembles a Contraption. (Put the top card of your Contraption deck face up onto one of your sprockets.) */

/* Crow Storm	|2|U
 * Sorcery
 * Create a 1/2 |Sblue Bird creature token with flying named Storm Crow.
 * Storm (When you cast this spell, copy it for each spell cast before it this turn.) */

/* Defective Detective	|2|U
 * Creature - Human Spy 2/1
 * ~ can't be blocked.
 * When ~ enters the battlefield, a person outside the game looks at target opponent's hand and chooses a card from it. That player reveals that card. */

/* Five-Finger Discount	|4|U|U
 * Sorcery
 * Put target nonland permanent into your hand. You may spend mana as though it were mana of any color the next time you cast that card. */

/* Graveyard Busybody	|4|U|U
 * Creature - Human Spy 100/100
 * All graveyards are also your graveyards.
 * ~'s power and toughness are each equal to the number of cards with flavor text in your graveyards. */

/* Half-Shark, Half-	""
 * Creature - Fish 3/3
 * At the beginning of your upkeep,
 * Augment |5|U (|5|U, Reveal this card from your hand: Combine it with target host. Augment only as a sorcery.) */

/* Incite Insight	|X|U|U
 * Sorcery
 * Assemble X Contraptions. (To assemble a Contraption, put the top card of your Contraption deck face up onto one of your sprockets.) */

/* Kindly Cognician	|1|U
 * Artifact Creature - Cyborg Artificer 1/3
 * Spells you cast that refer to artifacts or Contraptions in their rules text cost |1 less to cast. */

/* Magic Word	|2|U
 * Enchantment - Aura
 * Enchant creature
 * As ~ enters the battlefield, choose a word.
 * Whisper the chosen word: Tap enchanted creature. */

/* Mer // Man	|4|U
 * Host Creature - Human Fish 3/3
 * When this creature enters the battlefield,
 * //
 * you may draw a card. */

/* More or Less	|U
 * Instant
 * Add or subtract 1 or one from a number or number word on target spell or permanent until end of turn. */

/* Novellamental	|1|U
 * Creature - Elemental 2/1
 * Flying
 * ~ can block only creatures with flying. */

/* Numbing // Jellyfish	|3|U
 * Host Creature - Jellyfish 2/3
 * When this creature enters the battlefield,
 * //
 * roll a six-sided die. Target player puts the top X cards of their library into their graveyard, where X is the result. */

/* S.N.E.A.K. Dispatcher	|1|U
 * Creature - Human Spy 2/1
 * |2|U, |T: Look at the top card of target player's library. If it has an Agents of S.N.E.A.K. watermark, you may reveal it and put it into your hand. Otherwise, put it on the top or bottom of its owner's library. */

/* Socketed Sprocketer	|U
 * Artifact Creature - Cyborg Knight 1/1
 * |T: Uninstall all results from ~, then roll a six-sided die. Install the result on ~. (Put the die on this card.)
 * You may uninstall a result from ~ to use it for a die you rolled.
 * Uninstall a 6 from ~: Draw a card. */

/* Spell Suck	|2|U|U
 * Instant
 * Counter target spell, then assemble a Contraption. (Put the top card of your Contraption deck face up onto one of your sprockets.) */

/* Spy Eye	|2|U|U
 * Creature - Eye Spy 1/3
 * Flying
 * Whenever ~ deals combat damage to a player, you may draw a card from that player's library. */

/* Suspicious Nanny	|4|U
 * Creature - Human Spy Rigger 3/4
 * Whenever ~ deals combat damage to a player, it reassembles target Contraption that player controls. (Gain control of it and move it onto one of your sprockets.) */

/* Time Out	|4|U
 * Instant
 * Roll a six-sided die. Put target nonland permanent into its owner's library just beneath the top X cards of that library, where X is the result. */

/* Very Cryptic Command A	|1|U|U|U
 * Instant
 * Choose two -
 * * Switch target creature's power and toughness until end of turn.
 * * Target creature can't be blocked this turn.
 * * Draw a card. If that card's art is by Wayne England, you may reveal it and draw another card.
 * * Assemble a Contraption. */

/* Very Cryptic Command B	|1|U|U|U
 * Instant
 * Choose two -
 * * Untap two target permanents.
 * * Tap each permanent target player controls with exactly one word in its name.
 * * Discard all the cards in your hand, then draw that many cards.
 * * Return target instant or sorcery card from your graveyard to your hand. */

/* Very Cryptic Command C	|1|U|U|U
 * Instant
 * Choose two -
 * * Draw a card from an opponent's library.
 * * Copy target instant or sorcery spell. You may choose new targets for the copy.
 * * Until end of turn, target creature loses all abilities and becomes a |Sblue Frog with base power and toughness 1/1.
 * * Create a 1/1 colorless Gnome artifact creature token. */

/* Very Cryptic Command D	|1|U|U|U
 * Instant
 * Choose two -
 * * Return target permanent to its controller's hand.
 * * Draw two cards, then discard a card.
 * * Change the target of target spell with a single target.
 * * Turn over target nontoken creature. */

/* Very Cryptic Command E	|1|U|U|U
 * Instant
 * Choose two -
 * * Counter target black-bordered spell.
 * * Return target creature to its owner's hand.
 * * Untap each permanent you control with a watermark.
 * * Roll two six-sided dice. Target player puts the top X cards of their library into their graveyard, where X is the total of those results. */

/* Very Cryptic Command F	|1|U|U|U
 * Instant
 * Choose two -
 * * Scry 3.
 * * Create a 2/2 |Sblack Rogue creature token with menace.
 * * Add or subtract 1 or one from a number or number word on target spell or permanent until end of turn.
 * * Return all artifacts target player controls to their owner's hand. */

/* Wall of Fortune	|1|U
 * Artifact Creature - Wall 0/4
 * Defender
 * You may tap an untapped Wall you control to have any player reroll a die that player rolled. */

/*********
 * Black *
 ********/

/* Big Boa // Constrictor	|3|B
 * Host Creature - Snake 1/2
 * When this creature enters the battlefield,
 * //
 * roll a six-sided die. Target opponent loses life equal to the result. */

/* capital offense	|2|B|B
 * instant
 * target creature gets -x/-x until end of turn, where x is the number of times a capital letter appears in its rules text. (ignore reminder text and flavor text.) */

/* Dirty // Rat	|1|B
 * Host Creature - Rat 1/1
 * When this creature enters the battlefield,
 * //
 * target opponent discards a card. */

/* Extremely Slow Zombie	|1|B
 * Creature - Zombie 3/3
 * Last strike (This creature deals combat damage after creatures without last strike.) */

/* Finders, Keepers	|5|B
 * Sorcery
 * Destroy target creature, then assemble a Contraption. (Put the top card of your Contraption deck face up onto one of your sprockets.) */

/* Hangman	|B
 * Creature - Human Villain 1/1
 * As ~ enters the battlefield, secretly note a word with six to eight letters.
 * |1: Target player who doesn't control ~ guesses the noted word or an unguessed letter in that word. If they guess wrong, put a +1/+1 counter on ~. Any player may activate this ability.
 * When a player guesses the noted word or all of its letters, sacrifice ~. */

/* Hazmat Suit (Used)	|3|B
 * Enchantment - Aura
 * Enchant creature
 * Enchanted creature gets +2/+1 and has menace.
 * Whenever a player's skin or fingernail touches enchanted creature, that player loses 2 life. */

/* Hoisted Hireling	|2|B
 * Creature - Zombie 2/1
 * ~ has flying as long as it's being held above the battlefield. */

/* Inhumaniac	|1|B
 * Creature - Brainiac 1/1
 * At the beginning of your upkeep, roll a six-sided die. On a 3 or 4, put a +1/+1 counter on ~. On a 5 or higher, put two +1/+1 counters on it. On a 1, remove all +1/+1 counters from ~. */

/* Masterful Ninja	|2|B
 * Creature - Troll Ninja 1/1
 * Haste
 * Reveal ~ from your hand: ~ is on the battlefield and in your hand until end of turn.
 * |1|B: ~ gets +1/+1 until end of turn. */

/* Ninja	""
 * Creature - Ninja 1/0
 * You may activate ~'s augment ability any time you could cast an instant.
 * Whenever this creature deals combat damage to a player,
 * Augment |2|B (|2|B, Reveal this card from your hand: Combine it with target host. Augment only as-oh, nevermind.) */

/* Old-Fashioned Vampire	|3|B|B
 * Creature - Vampyre 3/3
 * Flying
 * ~ gets +2/+2 and has deathtouch as long as it's dark outdoors. */

/* Over My Dead Bodies	|4|B|B
 * Enchantment
 * Creature cards in graveyards can attack and block as though they were on the battlefield, can block or be blocked only by creature cards in graveyards, are Zombies in addition to their other types, and have undeathtouch. (If they would deal damage to a creature card, exile that creature card instead.)
 * Creature cards in your graveyard have haste. */

/* Overt Operative	|3|B
 * Creature - Human Ninja Rigger 2/3
 * Menace
 * Whenever ~ deals combat damage to a player, it assembles a Contraption. (Put the top card of your Contraption deck face up onto one of your sprockets.) */

/* "Rumors of My Death . . ."	|2|B
 * Enchantment
 * |3|B, Exile a permanent you control with a League of Dastardly Doom watermark: Return a permanent card with a League of Dastardly Doom watermark from your graveyard to the battlefield. */

/* Skull Saucer	|4|B|B
 * Creature - Zombie Head 4/1
 * Flying
 * When ~ enters the battlefield, destroy target creature and put your head on the table. Sacrifice ~ when your head stops touching the table. */

/* Sly Spy A	|2|B
 * Creature - Human Spy 2/2
 * Whenever Sly Spy deals combat damage to a player, that player reveals their hand. You choose a card from it with the longest name. That player discards that card. */

/* Sly Spy B	|2|B
 * Creature - Human Spy 2/2
 * Whenever Sly Spy deals combat damage to a player, you may destroy target creature facing left in its art. (Creatures without faces don't face anywhere.) */

/* Sly Spy C	|2|B
 * Creature - Human Spy 2/2
 * Whenever Sly Spy deals combat damage to a player, that player loses a finger until Sly Spy leaves the battlefield. (The finger is chosen by its owner and can't roll dice or touch cards.) */

/* Sly Spy D	|2|B
 * Creature - Human Spy 2/2
 * Whenever Sly Spy deals combat damage to a player, you may destroy target creature facing right in its art. (Creatures without faces don't face anywhere.) */

/* Sly Spy E	|2|B
 * Creature - Human Spy 2/2
 * Whenever Sly Spy deals combat damage to a player, that player reveals the top card of their library. Put that card into your hand and you lose life equal to its converted mana cost. */

/* Sly Spy F	|2|B
 * Creature - Human Spy 2/2
 * Whenever Sly Spy deals combat damage to a player, roll a six-sided die. That player loses life equal to the result. */

/* Snickering Squirrel	|B
 * Creature - Squirrel Advisor 1/1
 * You may tap ~ to increase the result of a die any player rolled by 1. */

/* Spike, Tournament Grinder	|2|PB|PB
 * Legendary Creature - Human Gamer 1/1
 * (|PB can be paid with either |B or 2 life.)
 * |PB|PB|PB|PB: Choose a card you own from outside the game that has been banned or restricted in a Constructed format, reveal that card, and put it into your hand. */

/* Squirrel-Powered Scheme	|2|B
 * Enchantment
 * Increase the result of each die you roll by 2. */

/* Steady-Handed Mook	|2|B
 * Creature - Human Rigger 1/1
 * Deathtouch
 * When ~ enters the battlefield, it assembles a Contraption. (Put the top card of your Contraption deck face up onto one of your sprockets.) */

/* Stinging // Scorpion	|4|B
 * Host Creature - Scorpion 3/2
 * When this creature enters the battlefield,
 * //
 * target creature an opponent controls gets -1/-1 until end of turn. */

/* Subcontract	|B
 * Sorcery
 * A person outside the game looks at target opponent's hand and chooses a nonland card from it. That player discards that card. */

/* Summon the Pack	|7|B
 * Sorcery
 * Open a sealed Magic booster pack, reveal the cards, and put all creature cards revealed this way onto the battlefield under your control. They're Zombies in addition to their other types. (Remove those cards from your deck before beginning a new game.) */

/* Zombified	""
 * Creature - Zombie 2/2
 * |4|B: Combine ~ from your graveyard with target host.
 * |2|B, Exile a creature card from your graveyard:
 * Augment |4|B (|4|B, Reveal this card from your hand: Combine it with target host. Augment only as a sorcery.) */

/*******
 * Red *
 *******/

/* The Big Idea	|4|R|R
 * Legendary Creature - Brainiac Villain 4/4
 * |2|BR|BR, |T: Roll a six-sided die. Create a number of 1/1 |Sred Brainiac creature tokens equal to the result.
 * Tap three untapped Brainiacs you control: The next time you would roll a six-sided die, instead roll two six-sided dice and use the total of those results. */

/* Box of Free-Range Goblins	|4|R|R
 * Sorcery
 * Roll a six-sided die. Create a number of 1/1 |Sred Goblin creature tokens equal to the result. */

/* Bumbling // Pangolin	|3|R
 * Host Creature - Pangolin Beast 2/2
 * When this creature enters the battlefield,
 * //
 * you may destroy target artifact. */

/* Common // Iguana	|1|R
 * Host Creature - Lizard 1/3
 * When this creature enters the battlefield,
 * //
 * you may discard a card. If you do, draw a card. */

/* The Countdown Is at One	|3|R|R
 * Sorcery
 * Players play a Magic subgame, starting at 1 life and using their libraries as their decks. For the rest of the main game, if a source would deal damage to a player who didn't win the subgame, it deals double that damage to that player instead. */

/* Feisty // Stegosaurus	|4|R
 * Host Creature - Dinosaur 2/1
 * When this creature enters the battlefield,
 * //
 * roll a six-sided die. This creature deals damage equal to the result to target creature an opponent controls. */

/* Garbage Elemental A	|4|R
 * Creature - Elemental 2/4
 * Frenzy 2 (Whenever this creature attacks and isn't blocked, it gets +2/+0 until end of turn.)
 * Garbage Elemental can't be blocked by wordy creatures. (A creature is wordy if it has four or more lines of rules text.) */

/* Garbage Elemental B	|4|R
 * Creature - Elemental Rigger 3/1
 * When Garbage Elemental enters the battlefield, it assembles a Contraption. (Put the top card of your Contraption deck face up onto one of your sprockets.)
 * Undying (When this creature dies, if it had no +1/+1 counters on it, return it to the battlefield under its owner's control with a +1/+1 counter on it.) */

/* Garbage Elemental C	|4|R
 * Creature - Elemental 3/2
 * Battle cry (Whenever this creature attacks, each other attacking creature gets +1/+0 until end of turn.)
 * When Garbage Elemental enters the battlefield, roll two six-sided dice. Create a number of 1/1 |Sred Goblin creature tokens equal to the difference between those results. */

/* Garbage Elemental D	|4|R
 * Creature - Elemental 3/3
 * Cascade (When you cast this spell, exile cards from the top of your library until you exile a nonland card that costs less. You may cast it without paying its mana cost. Put the exiled cards on the bottom of your library in a random order.)
 * When Garbage Elemental enters the battlefield, roll a six-sided die. Garbage Elemental deals damage equal to the result to target opponent or planeswalker. */

/* Garbage Elemental E	|4|R
 * Creature - Elemental 4/3
 * Unleash (You may have this creature enter the battlefield with a +1/+1 counter on it. It can't block as long as it has a +1/+1 counter on it.)
 * Each creature you control with any kind of counter on it has art menace. (They can't be blocked except by creatures with two or more visible figures in their art.) */

/* Garbage Elemental F	|4|R
 * Creature - Elemental 6/5
 * Last strike (This creature deals combat damage after creatures without last strike.)
 * Battalion - Whenever Garbage Elemental and at least two other creatures attack, target creature can't block this turn. */

/* Goblin Haberdasher	|2|R
 * Creature - Goblin Hatificer 2/2
 * Menace (This creature can't be blocked except by two or more creatures.)
 * Other creatures you control wearing hats in their art have menace. */

/* Half-Orc, Half-	""
 * Creature - Orc Warrior 3/1
 * Trample
 * At the beginning of each end step, if an opponent was dealt damage this turn,
 * Augment |1|R|R (|1|R|R, Reveal this card from your hand: Combine it with target host. Augment only as a sorcery.) */

/* Hammer Helper	|3|R
 * Sorcery
 * Gain control of target creature until end of turn. Untap that creature and roll a six-sided die. Until end of turn, it gains haste and gets +X/+0, where X is the result. */

/* Hammer Jammer	|3|R
 * Creature - Goblin Warrior 0/0
 * As ~ enters the battlefield, roll a six-sided die. ~ enters the battlefield with a number of +1/+1 counters on it equal to the result.
 * Whenever you roll a die, remove all +1/+1 counters from ~, then put a number of +1/+1 counters on it equal to the result. */

/* Hammerfest Boomtacular	|3|R|R
 * Enchantment
 * Whenever you cast a spell with a Goblin Explosioneers watermark, ~ deals 2 damage to any target. */

/* Infinity Elemental	|4|R|R|R
 * Creature - Elemental 100/5
 * (This creature has INFINITE POWER.) */

/* It That Gets Left Hanging	|5|R
 * Creature - Elemental Horror 5/4
 * When ~ enters the battlefield, ask a person outside the game to high-five you. If they won't, ~ gains haste until end of turn. */

/* Just Desserts	|1|R
 * Instant
 * ~ deals pi damage to target creature. (Pi is the ratio of a circle's circumference to its diameter. (It's a smidgen more than 3.)) */

/* Painiac	|2|R
 * Creature - Brainiac 0/3
 * At the beginning of your upkeep, roll a six-sided die. ~ gets +X/+0 until end of turn, where X is the result. */

/* Party Crasher	|4|R
 * Creature - Goblin Berserker 3/3
 * Haste
 * You can attack with ~ once each combat during each opponent's turn. */

/* Steamflogger of the Month	|3|R|R
 * Creature - Goblin Rigger 3/3
 * When ~ enters the battlefield, it assembles a Contraption for each Contraption you control. (To assemble a Contraption, put the top card of your Contraption deck face up onto one of your sprockets.) */

/* Steamflogger Temp	|1|R
 * Creature - Goblin Rigger 2/1
 * |6, |T: ~ assembles a Contraption. (Put the top card of your Contraption deck face up onto one of your sprockets.) */

/* Steamfloggery	|4|R|R
 * Instant
 * Roll a six-sided die. Assemble a number of Contraptions equal to the result. (To assemble a Contraption, put the top card of your Contraption deck face up onto one of your sprockets.) */

/* Super-Duper Death Ray	|2|R
 * Instant
 * Trample (This spell can deal excess damage to its target's controller.)
 * ~ deals 4 damage to target creature. */

/* Target Minotaur	|1|R
 * Creature - Minotaur Warrior 2/1
 * Prowess (Whenever you cast a noncreature spell, this creature gets +1/+1 until end of turn.) */

/* Three-Headed Goblin	|3|R|R
 * Creature - Goblin Mutant 3/3
 * Triple strike (This creature deals first-strike, regular, and last-strike combat damage.) */

/* Work a Double	|2|R|R
 * Sorcery
 * Assemble two Contraptions. (Put the top card of your Contraption deck face up onto one of your sprockets. Then repeat this process.) */

/* Wrench-Rigger	|R
 * Creature - Goblin Rigger 1/1
 * When ~ enters the battlefield, it assembles a Contraption. (Put the top card of your Contraption deck face up onto one of your sprockets.) */

/*********
 * Green *
 *********/

/* As Luck Would Have It	|G
 * Enchantment
 * Hexproof
 * Whenever you roll a die, put a number of luck counters on ~ equal to the result. Then if there are 100 or more luck counters on ~, you win the game. (Count both rolls if you reroll a die.) */

/* Beast in Show	|4|G|G
 * Creature - Beast 6/4
 * Trample */

/* Chittering Doom	|3|G
 * Enchantment
 * Whenever you roll a 4 or higher on a die, create a 1/1 |Sgreen Squirrel creature token. */

/* Clever Combo	|1|G
 * Sorcery
 * Search your library for a host card or a card with augment, reveal it, put it into your hand, then shuffle your library. */

/* Druid of the Sacred Beaker	|2|G
 * Creature - Deer Bird Ape Druid 2/2
 * |T: Add |G for each Crossbreed Labs watermark among permanents you control. */

/* Eager // Beaver	|2|G
 * Host Creature - Beaver 3/2
 * When this creature enters the battlefield,
 * //
 * you may untap target permanent. */

/* Earl of Squirrel	|4|G|G
 * Creature - Squirrel Advisor 4/4
 * Squirrellink (Damage dealt by this creature also causes you to create that many 1/1 |Sgreen Squirrel creature tokens.)
 * Creature tokens you control are Squirrels in addition to their other creature types.
 * Other Squirrels you control get +1/+1. */

/* First Pick	|3|G
 * Instant
 * Destroy target artifact or enchantment. Assemble a Contraption. (Put the top card of your Contraption deck face up onto one of your sprockets.) */

/* Ground Pounder	|1|G
 * Creature - Goblin Warrior 2/2
 * |3|G: Roll a six-sided die. ~ gets +X/+X until end of turn, where X is the result.
 * Whenever you roll a 5 or higher on a die, ~ gains trample until end of turn. */

/* Half-Squirrel, Half-	""
 * Creature - Squirrel -1/0
 * Whenever a nontoken creature enters the battlefield,
 * Augment |G (|G, Reveal this card from your hand: Combine it with target host. Augment only as a sorcery.) */

/* Hydradoodle	|X|X|G|G
 * Creature - Hydra Hound 0/0
 * As ~ enters the battlefield, roll X six-sided dice. ~ enters the battlefield with a number of +1/+1 counters on it equal to the total of those results.
 * Reach, trample */

/* Ineffable Blessing A	|1|G
 * Enchantment
 * As Ineffable Blessing enters the battlefield, choose Flavorful or Bland.
 * * Flavorful - Whenever a creature with flavor text enters the battlefield under your control, draw a card.
 * * Bland - Whenever a creature without flavor text enters the battlefield under your control, draw a card. */

/* Ineffable Blessing B	|1|G
 * Enchantment
 * As Ineffable Blessing enters the battlefield, choose an artist.
 * Whenever a creature with art by the chosen artist enters the battlefield under your control, draw a card. */

/* Ineffable Blessing C	|1|G
 * Enchantment
 * As Ineffable Blessing enters the battlefield, choose white-bordered or silver-bordered.
 * Whenever a creature with the chosen border enters the battlefield under your control, draw a card. */

/* Ineffable Blessing D	|1|G
 * Enchantment
 * As Ineffable Blessing enters the battlefield, choose a rarity.
 * Whenever a creature with the chosen rarity enters the battlefield under your control, draw a card. */

/* Ineffable Blessing E	|1|G
 * Enchantment
 * As Ineffable Blessing enters the battlefield, choose odd or even.
 * Whenever a creature with a collector number of the chosen value enters the battlefield under your control, draw a card. */

/* Ineffable Blessing F	|1|G
 * Enchantment
 * As Ineffable Blessing enters the battlefield, choose a number.
 * Whenever a creature with exactly the chosen number of words in its name enters the battlefield under your control, draw a card. (Hyphenated words are one word.) */

/* Joyride Rigger	|3|G
 * Creature - Goblin Rigger 3/3
 * When ~ enters the battlefield, it assembles a Contraption. (Put the top card of your Contraption deck face up onto one of your sprockets.) */

/* Monkey-	""
 * Creature - Monkey 2/2
 * Whenever a nontoken creature you control dies,
 * Augment |2|G (|2|G, Reveal this card from your hand: Combine it with target host. Augment only as a sorcery.) */

/* Mother // Kangaroo	|4|G
 * Host Creature - Kangaroo 1/1
 * When this creature enters the battlefield,
 * //
 * roll a six-sided die. Put a number of +1/+1 counters on this creature equal to the result. */

/* Multi-Headed	""
 * Creature - Hydra 4/4
 * At the beginning of each end step, if you rolled a die this turn,
 * Augment |4|G (|4|G, Reveal this card from your hand: Combine it with target host. Augment only as a sorcery.) */

/* Really Epic Punch	|1|G
 * Sorcery
 * Target creature you control gets +2/+2 if it's a host or has augment. Then it fights target creature you don't control. */

/* Selfie Preservation	|1|G
 * Sorcery
 * Search your library for a basic land card and reveal it. If there's a tree in its art, put it onto the battlefield tapped. Otherwise, put it into your hand. Then shuffle your library. */

/* Serpentine	""
 * Creature - Wurm 3/3
 * Whenever a land enters the battlefield under your control,
 * Augment |2|G (|2|G, Reveal this card from your hand: Combine it with target host. Augment only as a sorcery.) */

/* Shellephant	|1|G|G
 * Creature - Turtle and/or Elephant 100/100
 * |0: Choose one. You may activate this ability while ~ is in any zone.
 * * ~ has base power and toughness 1/4.
 * * ~ has base power and toughness 3/3. */

/* Slaying Mantis	|5|G|G
 * Creature - Insect Wrestler 6/6
 * Just a second (As long as this spell is on the stack, players can't move cards on the battlefield.)
 * ~ enters the battlefield by being thrown from a distance of at least three feet.
 * When ~ enters the battlefield, it fights each creature an opponent controls that it touched as it entered. */

/* Squirrel Dealer	|G
 * Creature - Raccoon Lizard Bird 1/1
 * When ~ enters the battlefield, ask a person outside the game "Do you like Squirrels?" If they do, create a 1/1 |Sgreen Squirrel creature token. */

/* Steamflogger Service Rep	|2|G
 * Creature - Goblin Rigger 1/1
 * Whenever another Goblin enters the battlefield under your control, you may pay |1. If you do, ~ assembles a Contraption. (Put the top card of your Contraption deck face up onto one of your sprockets.) */

/* Wild // Crocodile	|1|G
 * Host Creature - Crocodile 1/1
 * When this creature enters the battlefield,
 * //
 * search your library for a basic land card, reveal it, put it into your hand, then shuffle your library. */

/* Willing Test Subject	|2|G
 * Creature - Spider Monkey Scientist 2/2
 * Reach
 * Whenever you roll a 4 or higher on a die, put a +1/+1 counter on ~.
 * |6: Roll a six-sided die. */

/********
 * Gold *
 ********/

/* Baron Von Count	|1|B|R
 * Legendary Creature - Human Villain 3/3
 * ~ enters the battlefield with a doom counter on "5."
 * Whenever you cast a spell with the indicated numeral in its mana cost, text box, power, or toughness, move the doom counter one numeral to the left.
 * When the doom counter moves from "1," destroy target player and put that doom counter on "5." */

/* Better Than One	|G|W
 * Sorcery
 * A person outside the game becomes your teammate. (Choose any number of cards in your hand, on top of your library, or on the battlefield under your control. Those cards become your teammate's hand, library, and permanents, respectively.) */

/* Cramped Bunker	|4|R|G
 * Enchantment
 * At the beginning of each opponent's upkeep, that player moves a permanent they control to touch ~ and no other permanents. If the player can't, destroy each permanent they control that isn't touching ~, then sacrifice it. */

/* Dr. Julius Jumblemorph	|2|G|W
 * Legendary Creature 4/4
 * ~ is every creature type (even if this card isn't on the battlefield).
 * Whenever a host enters the battlefield under your control, you may search your library and/or graveyard for a card with augment and combine it with that host. If you search your library this way, shuffle it. */

/* The Grand Calcutron	|W|U
 * Legendary Artifact
 * When ~ enters the battlefield, each player's hand becomes a program (an ordered row of revealed cards).
 * Players can only play the first card of their program.
 * If a card would be put into a player's hand from anywhere, that player reveals it and places it anywhere within their program instead.
 * At the beginning of each player's end step, if that player's program has fewer than five cards, they draw cards equal to the difference. */

/* Grusilda, Monster Masher	|3|B|R
 * Legendary Creature - Zombie Villain 4/4
 * Combined, enchanted, and equipped creatures you control have menace.
 * |3|B|R, |T: Put two target creature cards from graveyards onto the battlefield combined into one creature under your control. (Its power is equal to their total power, its toughness is equal to their total toughness, and it has their names, mana costs, types, text boxes, etc.) */

/* Hot Fix	|4|W|U
 * Sorcery
 * You have ten seconds to look at and rearrange the cards in your library. At the end of those ten seconds, if you're touching one or more of those cards, shuffle your library. */

/* Ol' Buzzbark	|X|R|G
 * Legendary Creature - Goblin Warrior 3/3
 * When ~ enters the battlefield, roll X six-sided dice onto the battlefield from a height of at least X inches. For each die, put a number of +1/+1 counters equal to the result on each creature you control that die is touching. For each die, ~ deals damage equal to the result to each creature an opponent controls that die is touching. */

/* Phoebe, Head of S.N.E.A.K.	|1|U|B
 * Legendary Creature - Human Spy 2/3
 * ~, Head of S.N.E.A.K. can't be blocked by creatures with flavor text.
 * |2|U|B: ~ permanently steals target creature's text box. (That creature loses all rules text, flavor text, and watermarks. This creature gains them.) */

/* Urza, Academy Headmaster	|W|U|B|R|G
 * Legendary Planeswalker - Urza (4)
 * +1: Head to AskUrza.com and click +1.
 * -1: Head to AskUrza.com and click -1.
 * -6: Head to AskUrza.com and click -6. */

/* X	|U|B
 * Legendary Creature - Human Spy 2/2
 * As long as ~ is in ~'s owner's opponent's hand, ~'s owner may cast ~ and activate ~'s abilities. That opponent can't cast ~ and plays with their hand revealed.
 * |U|B, |T: Put ~ into target opponent's hand.
 * |3|U|B: You may play a card in the same hand as ~ without paying its mana cost. */

/**********
 * Hybrid *
 **********/

/* Mary O'Kill	|5|BR
 * Legendary Creature - Human Villain 5/5
 * |1|BR: Switch a Killbot or ~ in your hand with one on the battlefield. (If a creature is tapped, the switched creature is tapped. The same is true for untapped, attacking, blocking, enchanted, equipped, and targeted. Any counters on a creature are on the switched creature instead.) */

/************
 * Artifact *
 ************/

/* Angelic // Rocket	|8
 * Artifact Host Creature - Angel 4/4
 * Flying
 * //
 * When this creature enters the battlefield,
 * //
 * you may destroy target nonland permanent. */

/* Border Guardian	|4
 * Artifact Creature - Knight 1/1
 * Whenever you cast a silver-bordered spell, put a +1/+1 counter on ~.
 * Whenever you cast a black-bordered spell, ~ can't be blocked this turn.
 * Whenever you cast a white-bordered spell, ~ gains double strike until end of turn. */

/* Buzzing Whack-a-Doodle	|4
 * Artifact
 * As ~ enters the battlefield, you and an opponent each secretly choose Whack or Doodle. Then those choices are revealed. If the choices match, ~ has that ability. Otherwise, it has Buzz.
 * * Whack - |T: Target player loses 2 life.
 * * Doodle - |T: You gain 3 life.
 * * Buzz - |2, |T: Draw a card. */

/* Clock of DOOOOOOOOOOOOM!	|4
 * Artifact
 * |4, |T: Move the CRANK! counter to your Contraption deck's next sprocket and crank any number of that sprocket's Contraptions. */

/* Cogmentor	|1
 * Artifact Creature - Gnome Rigger 1/1
 * Flying
 * |4: Reassemble target Contraption you control. (Move it onto another one of your sprockets.) */

/* Contraption Cannon	|4
 * Artifact
 * |2, Sacrifice ~: It deals damage to any target equal to the number of Contraptions you control. */

/* Curious Killbot	|2
 * Artifact Creature - Killbot 2/1 */

/* Delighted Killbot	|2
 * Artifact Creature - Killbot 2/1 */

/* Despondent Killbot	|2
 * Artifact Creature - Killbot 2/1 */

/* Enraged Killbot	|2
 * Artifact Creature - Killbot 2/1 */

/* Entirely Normal Armchair	""
 * Artifact
 * During your turn, if ~ is in your hand, you may hide it on the battlefield.
 * |0: Return ~ to its owner's hand. Only any opponent may activate this ability and only if they see ~.
 * |2, Sacrifice ~: Destroy target attacking creature. */

/* Everythingamajig A	|5
 * Artifact
 * |2, |T: Move a counter from one permanent onto another. If the second permanent refers to any kind of counter, the moved counter becomes one of those counters. Otherwise, it becomes a +1/+1 counter.
 * |3, |T: Put a +1/+1 counter on target creature.
 * |4, |T: Proliferate. (You choose any number of permanents and/or players with counters on them, then give each another counter of a kind already there.) */

/* Everythingamajig B	|5
 * Artifact
 * |2, |T: Draw a card. Activate this ability only if you have no cards in hand.
 * |8, |T: You gain 10 life.
 * |4, |T, Sacrifice Everythingamajig: You may put a silver-bordered permanent card from your hand onto the battlefield. */

/* Everythingamajig C	|5
 * Artifact
 * |1: Flip a coin. If you win the flip, add |C|C. Activate this ability only any time you could cast an instant.
 * |3, |T: Target player discards a card. Activate this ability only during your turn.
 * |X: Everythingamajig becomes an X/X Construct artifact creature until end of turn. */

/* Everythingamajig D	|6
 * Artifact
 * |T: Add one mana of any color.
 * |1, |T: Say the flavor text on a card in your hand. Target opponent guesses that card's name. You may reveal that card. If you do and your opponent guessed wrong, draw a card.
 * |8, |T: Everythingamajig deals 12 damage to target creature. */

/* Everythingamajig E	|5
 * Artifact
 * Sacrifice a land: You gain 2 life.
 * Sacrifice a creature: Add |C|C.
 * |2, Discard a card: Search your library for a card that shares a complete word in its name with the name of the discarded card, reveal it, put it into your hand, then shuffle your library. */

/* Everythingamajig F	|8
 * Artifact
 * |1, |T: Scry 2.
 * |1, |T, Pay 1 life: Create a 0/1 |Swhite Goat creature token.
 * |7, |T, Sacrifice Everythingamajig: Choose target player. At the beginning of the next end step, exchange life totals with that player, exchange control of all permanents you and that player control, and exchange cards in your hands, cards in your libraries, and cards in your graveyards. */

/* Gnome-Made // Engine	|4
 * Artifact Host Creature - Construct 2/2
 * When this creature enters the battlefield,
 * //
 * create a 1/1 colorless Gnome artifact creature token. */

/* Handy Dandy Clone Machine	|3
 * Artifact
 * |2, |T: Create a 2/2 colorless Homunculus creature token. It must be represented by a unique hand and two fingers at all times, or it ceases to exist. */

/* Kindslaver	|5
 * Legendary Artifact
 * |5, |T, Sacrifice ~: A person outside the game controls target player during that player's next turn. Neither player may advise that person until the end of that turn. */

/* Krark's Other Thumb	|2
 * Legendary Artifact
 * If you would roll a die, instead roll two of those dice and ignore one of those results. */

/* Labro // Bot	|5
 * Artifact Host Creature - Hound 3/3
 * When this creature enters the battlefield,
 * //
 * return target host card or card with augment from your graveyard to your hand. */

/* Lobe Lobber	|2
 * Artifact - Equipment
 * Equipped creature has "|T: This creature deals 1 damage to target player or planeswalker. Roll a six-sided die. On a 5 or higher, untap it."
 * Equip |2 */

/* Mad Science Fair Project	|3
 * Artifact
 * |T: Roll a six-sided die. On a 3 or lower, target player adds |C. Otherwise, that player adds one mana of any color they choose. */

/* Modular Monstrosity	|7
 * Artifact Creature - Construct 3/3
 * Whenever an opponent casts a spell, you have five seconds to choose a keyword you haven't chosen for a card named ~ today that's been printed on a creature card. If you do, ~ gains that ability. Otherwise, ~ loses all keyword abilities. */

/* Proper Laboratory Attire	|1
 * Artifact - Equipment
 * Equipped creature gets +2/+1 and has protection from die rolls. (Nothing that lets a player roll a die can block, target, deal damage to, or attach to equipped creature.)
 * Equip |2 */

/* Robo-	""
 * Artifact Creature - Construct 1/1
 * At the beginning of each end step, if an artifact entered the battlefield under your control this turn,
 * Augment |2 (|2, Reveal this card from your hand: Combine it with target host. Augment only as a sorcery.) */

/* Split Screen	|4
 * Artifact
 * When ~ enters the battlefield, shuffle your library and deal it into four libraries. If anything refers to your library, choose one of your libraries for it.
 * Play with your libraries' top cards revealed.
 * When ~ leaves the battlefield, shuffle your libraries together. */

/* Staff of the Letter Magus	|3
 * Artifact
 * As ~ enters the battlefield, choose a consonant other than N, R, S, or T.
 * Whenever a player casts a spell, you gain 1 life for each time the chosen letter appears in that spell's name. */

/* Stamp of Approval	|3
 * Artifact
 * As ~ enters the battlefield, choose a watermark.
 * Creatures you control with the chosen watermark get +1/+1. */

/* Steam-Powered	""
 * Artifact Creature - Construct 0/4
 * |5:
 * Augment |4 (|4, Reveal this card from your hand: Combine it with target host. Augment only as a sorcery.) */

/* Steel Squirrel	|2
 * Artifact Creature - Squirrel 1/1
 * Whenever you roll a 5 or higher on a die, ~ gets +X/+X until end of turn, where X is the result.
 * |6: Roll a six-sided die. */

/* Sword of Dungeons & Dragons	|3
 * Artifact - Equipment
 * Equipped creature gets +2/+2 and has protection from Rogues and from Clerics.
 * Whenever equipped creature deals combat damage to a player, create a 4/4 gold Dragon creature token with flying and roll a d20 (a twenty-sided die). If you roll a 20, repeat this process.
 * Equip |2 */

/* Voracious // Vacuum	|3
 * Artifact Host Creature - Construct 1/1
 * When this creature enters the battlefield,
 * //
 * put a +1/+1 counter on target creature. */

/********
 * Land *
 ********/

/* Secret Base A	""(Order of the Widget watermark)
 * Land
 * |T: Add |C.
 * |T: Add one mana of any color. Spend this mana only to cast a spell that shares a watermark with Secret Base. */

/* Secret Base B	""(Agents of S.N.E.A.K watermark)
 * Land
 * |T: Add |C.
 * |T: Add one mana of any color. Spend this mana only to cast a spell that shares a watermark with Secret Base. */

/* Secret Base C	""(League of Dastardly Doom watermark)
 * Land
 * |T: Add |C.
 * |T: Add one mana of any color. Spend this mana only to cast a spell that shares a watermark with Secret Base. */

/* Secret Base D	""(Goblin Explosioneers watermark)
 * Land
 * |T: Add |C.
 * |T: Add one mana of any color. Spend this mana only to cast a spell that shares a watermark with Secret Base. */

/* Secret Base E	""(Crossbreed Labs watermark)
 * Land
 * |T: Add |C.
 * |T: Add one mana of any color. Spend this mana only to cast a spell that shares a watermark with Secret Base. */

/* Watermarket	""
 * Land
 * |T: Add |C|C. Spend this mana only to cast spells with watermarks. */

/***************
 * Contraption *
 ***************/

/* Accessories to Murder	""
 * Artifact - Contraption
 * Whenever you crank ~, target creature gets +X/+0 until end of turn, where X is the number of creatures you control. */

/* Applied Aeronautics	""
 * Artifact - Contraption
 * Whenever you crank ~, until end of turn, target creature gets +1/+0, gains flying, and becomes an artifact in addition to its other types. */

/* Arms Depot	""
 * Artifact - Contraption
 * Whenever you crank ~, put two +1/+1 counters on target creature. */

/* Auto-Key	""
 * Artifact - Contraption
 * Whenever you crank ~, until end of turn, target creature becomes an artifact in addition to its other types and gains "|T: You gain 3 life." */

/* Bee-Bee Gun	""
 * Artifact - Contraption
 * Whenever you crank ~, until end of turn, target creature gains "|2: This creature fights another target creature." */

/* Boomflinger	""
 * Artifact - Contraption
 * Whenever you crank ~, roll two six-sided dice. ~ deals damage to target player or planeswalker equal to the difference between those results. */

/* Buzz Buggy	""
 * Artifact - Contraption
 * Whenever you crank ~, target creature gets +2/+0 and gains trample until end of turn. */

/* Deadly Poison Sampler	""
 * Artifact - Contraption
 * Whenever you crank ~, until end of turn, target creature gains "Whenever this creature deals combat damage to a player, destroy target creature that player controls." */

/* Dictation Quillograph	""
 * Artifact - Contraption
 * Whenever you crank ~, until end of turn, target creature gains "Whenever this creature deals combat damage to a player, you may draw a card. If you do, discard a card." */

/* Dispatch Dispensary	""
 * Artifact - Contraption
 * Whenever you crank ~, create a 2/2 |Sblack Rogue creature token with menace. */

/* Division Table	""
 * Artifact - Contraption
 * Whenever you crank ~, target player loses 2 life. */

/* Dogsnail Engine	""
 * Artifact - Contraption
 * Whenever you crank ~, target player gains life equal to the greatest power among creatures you control. */

/* Dual Doomsuits	""
 * Artifact - Contraption
 * Whenever you crank ~, each time a source you control would deal damage this turn, it deals double that damage instead. */

/* Duplication Device	""
 * Artifact - Contraption
 * Whenever you crank ~, until end of turn, target creature becomes a copy of any creature on the battlefield, except it's an artifact in addition to its other types. */

/* Faerie Aerie	""
 * Artifact - Contraption
 * Whenever you crank ~, create two 1/1 |Sblue Faerie Spy creature tokens with flying, haste, and "Whenever this creature deals combat damage to a player, draw a card." Exile them at the beginning of the next end step. */

/* Genetic Recombinator	""
 * Artifact - Contraption
 * Whenever you crank ~, up to two target creatures each get +2/+2 until end of turn. */

/* Gift Horse	""
 * Artifact - Contraption
 * Whenever you crank ~, roll two six-sided dice. Create a number of 1/1 |Sred Goblin creature tokens equal to the difference between those results. */

/* Gnomeball Machine	""
 * Artifact - Contraption
 * Whenever you crank ~, create two 1/1 colorless Gnome artifact creature tokens. */

/* Goblin Slingshot	""
 * Artifact - Contraption
 * Whenever you crank ~, creatures you control get +2/+0 and gain trample until end of turn. */

/* Guest List	""
 * Artifact - Contraption
 * Whenever you crank ~, target creature gets -X/-X until end of turn, where X is the number of creature cards in its controller's graveyard. */

/* Hard Hat Area	""
 * Artifact - Contraption
 * Whenever you crank ~, roll two six-sided dice. ~ assembles a number of Contraptions equal to the difference between those results. (To assemble a Contraption, put the top card of your Contraption deck face up onto one of your sprockets.) */

/* Head Banger	""
 * Artifact - Contraption
 * Whenever you crank ~, target creature must be blocked this turn if able. */

/* Hypnotic Swirly Disc	""
 * Artifact - Contraption
 * Whenever you crank ~, target player puts the top X cards of their library into their graveyard, where X is the number of creatures you control. */

/* Inflation Station	""
 * Artifact - Contraption
 * Whenever you crank ~, target creature gets +3/+3 until end of turn. */

/* Insufferable Syphon	""
 * Artifact - Contraption
 * Whenever you crank ~, target player discards a card. */

/* Jamming Device	""
 * Artifact - Contraption
 * Whenever you crank ~, creatures target player controls get -1/-1 until end of turn. */

/* Lackey Recycler	""
 * Artifact - Contraption
 * Whenever you crank ~, put target creature card from your graveyard on top of your library. */

/* Mandatory Friendship Shackles	""
 * Artifact - Contraption
 * Whenever you crank ~, target creature gets -1/-1 until end of turn. */

/* Neural Network	""
 * Artifact - Contraption
 * Whenever you crank ~, gain control of target creature an opponent controls with power less than or equal to the number of creature cards in its controller's graveyard until end of turn. Untap that creature. It gains haste until end of turn. */

/* Oaken Power Suit	""
 * Artifact - Contraption
 * Whenever you crank ~, target creature gets +X/+X until end of turn, where X is the greatest power among creatures you control. */

/* Optical Optimizer	""
 * Artifact - Contraption
 * Whenever you crank ~, until end of turn, target creature becomes an artifact in addition to its other types and gains "|T: Draw a card." */

/* Pet Project	""
 * Artifact - Contraption
 * Whenever you crank ~, put target creature card from an opponent's graveyard onto the battlefield under your control. */

/* Quick-Stick Lick Trick	""
 * Artifact - Contraption
 * Whenever you crank ~, target creature gets +1/+1 and gains lifelink until end of turn. */

/* Rapid Prototyper	""
 * Artifact - Contraption
 * Whenever you crank ~, create an X/X colorless Construct artifact creature token, where X is the number of artifacts you control. */

/* Record Store	""
 * Artifact - Contraption
 * Whenever you crank ~, look at the top X cards of your library, where X is the number of artifacts you control. Put one of those cards into your hand and the rest on the bottom of your library in a random order. */

/* Refibrillator	""
 * Artifact - Contraption
 * Whenever you crank ~, return target creature card from your graveyard to your hand. */

/* Sap Sucker	""
 * Artifact - Contraption
 * Whenever you crank ~, add |G. Until end of turn, you don't lose this mana as steps and phases end. */

/* Sundering Fork	""
 * Artifact - Contraption
 * Whenever you crank ~, destroy target artifact. */

/* Targeting Rocket	""
 * Artifact - Contraption
 * Whenever you crank ~, target creature blocks this turn if able. */

/* Thud-for-Duds	""
 * Artifact - Contraption
 * Whenever you crank ~, roll two six-sided dice. ~ deals damage to target creature equal to the difference between those results. */

/* Top-Secret Tunnel	""
 * Artifact - Contraption
 * Whenever you crank ~, target creature can't be blocked this turn. */

/* Tread Mill	""
 * Artifact - Contraption
 * Whenever you crank ~, until end of turn, target creature gets +1/+2, gains vigilance, and becomes an artifact in addition to its other types. */

/* Turbo-Thwacking Auto-Hammer	""
 * Artifact - Contraption
 * Whenever you crank ~, target creature gains double strike until end of turn. */

/* Twiddlestick Charger	""
 * Artifact - Contraption
 * Whenever you crank ~, tap or untap target creature. */

/* Widget Contraption	""
 * Artifact - Contraption
 * Whenever you crank ~, it assembles a Contraption. (Put the top card of your Contraption deck face up onto one of your sprockets.) */

// Plains, Island, Swamp, Mountain, Forest.

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25c4930; original cards/unstable.c:107. */
static int fx_0(int player, int card, event_t event) { return 0; }

/* ManalinkEh 2022: 0x25c5700; original cards/unstable.c:442. */
int card_crow_storm(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = check_special_flags_2022(player, card, 0x40000);
      if (result == 0)
      {
        result = get_storm_count();
        if (0 < result)
        {
          generate_tokens_by_id(player, card, 0x48a, result);
        }
      }
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    generate_token_by_id(player, card, 0x48a);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int test_ineffable_blessing_a(int iid, int val, int player, int card);
static int test_words_in_name_mod_val(int iid, int val, int player, int card);

/* ManalinkEh 2022: 0x25c4b50; original cards/unstable.c:155. */
static int test_words_in_name_val(int iid, int val, int player, int card)
{
  int result;

  result = words_in_name(player, card);
  return (unsigned int)(result == val);
}

/* ManalinkEh 2022: 0x25c4b10; original cards/unstable.c:305. */
static int test_words_in_name_mod_val(int iid, int val, int player, int card)
{
  int result;

  result = words_in_name(player, card);
  return (unsigned int)(result % 2 == val);
}

/* ManalinkEh 2022: 0x25c53e0; original cards/unstable.c:309. */
int card_oddly_uneven(int player, int card, event_t event)
{
  int result;
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
        result =
            dialog_impl(player, card, event, "\001\002", "Odd", 1, 1, "Even", 1, 1, 0, 0, 0, 0);
        if (event == EVENT_RESOLVE_SPELL)
        {
          default_test_definition(&test, 0x47);
          test.value_for_special_selection_function = (int)(result == 1);
          test.special_selection_function = test_words_in_name_mod_val;
          new_manipulate_all(player, card, 2, &test, ACT_KILL_DESTROY);
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_025c54f4;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  dialog_impl(player, card, event, "\001\002", "Odd", 1, 1, "Even", 1, 1, 0, 0, 0, 0);
LAB_025c54f4:
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25c4970; original cards/unstable.c:1661. */
static int test_ineffable_blessing_a(int iid, int val, int player, int card)
{
  unsigned int uVar1;

  uVar1 = (unsigned int)(val != 1);
  if ((cards_ptr[cards_data[iid].id]->flavor_text != NULL) &&
      (*cards_ptr[cards_data[iid].id]->flavor_text != '\0'))
  {
    uVar1 = (unsigned int)(val == 1);
  }
  return uVar1;
}

/* ManalinkEh 2022: 0x25c7c10; original cards/unstable.c:1666. */
int card_ineffable_blessing_a(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  test_definition_t test;

  if (event == EVENT_RESOLVE_SPELL)
  {

    uVar3 = dialog_impl(player, card, EVENT_RESOLVE_SPELL, "\001\002", "\001\014", "\001\003",
                        "Flavorful", 1, 1, "Bland", 1, 1, 0, 0, 0, 0);
    instance = get_card_instance(player, card);
    instance->targets[5].player = uVar3 | 0x46550000;
    result = create_targetted_legacy_effect(player, card, fx_0, player, card);
    alternate_legacy_text(uVar3, player, result);
  }
  else if ((trigger_condition == 0xdb) && (event - EVENT_TRIGGER < 2))
  {

    instance = get_card_instance(player, card);
    uVar3 = instance->targets[5].player;
    if (((short)(uVar3 >> 0x10) != 0x4655) || (uVar3 = uVar3 & 0xffff, 1 < uVar3 - 1))
    {
      return 0;
    }
    default_test_definition(&test, 2);
    test.special_selection_function = test_ineffable_blessing_a;
    test.value_for_special_selection_function = uVar3;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  else if (event == EVENT_ETB)
  {
    draw_a_card(player);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static char *target_hasnt_reminder_text(int who_chooses, int player, int card, int targeting_player,
                                        int targeting_card);

/* ManalinkEh 2022: 0x25c4b80; original cards/unstable.c:342. */
static char *target_hasnt_reminder_text(int who_chooses, int player, int card, int targeting_player,
                                        int targeting_card)
{
  int result;
  char *pcVar2;

  result = has_reminder_text(player, card);
  pcVar2 = NULL;
  if (result != 0)
  {
    pcVar2 = "reminder text";
  }
  return pcVar2;
}

/* ManalinkEh 2022: 0x25c55c0; original cards/unstable.c:347. */
int card_old_guard(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 2);
    td.special = 0x8000;
    td.extra = (int32_t)target_hasnt_reminder_text;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.special = 0x8000;
    td.extra = (int32_t)target_hasnt_reminder_text;
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {

      instance = get_card_instance(player, card);
      tap_card(instance->targets[0].player, instance->targets[0].card);
    }
  }
  result = generic_activated_ability(player, card, event, 5, 0, 0, 0, 0, 0, 1, 0, &td,
                                     "TARGET_CREATURE");
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25c88d0; original cards/unstable.c:2183. */
int card_everythingamajig_c(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  bool bVar4;
  target_definition_t td;

  if ((event - EVENT_RESOLVE_ACTIVATION < 2) || (event == EVENT_ACTIVATE))
  {
    bVar4 = current_turn == player;
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    instance = get_card_instance(player, card);
    iVar2 =
        dialog_impl(player, card, event, "\001\002", "\001\023", &instance->targets[5].card, "Mana",
                    1, 1, "\001\020", 1, 0, 0, 0, 0, 0, "Discard", (unsigned int)bVar4, 1,
                    "\001\020", 3, 0, 0, 0, 0, 0, "\001\025", "\001\024", &td, "TARGET_PLAYER",
                    "Becomes Construct", 1, 1, "\001\020", 0xffffffff, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    if (event == EVENT_CAN_ACTIVATE)
    {
      if ((((unsigned int)land_can_be_played >> 16 & 0xff) & 4) == 0)
      {
        iVar3 = can_use_activated_abilities_2022(player, card);
        return (unsigned int)(iVar3 != 0 && iVar2 != 0);
      }
    }
    else if (event == EVENT_ACTIVATE)
    {
      if (iVar2 == 1)
      {
        iVar2 = flip_a_coin(player, card);
        if (iVar2 != 0)
        {
          produce_mana(player, 0, 2);
        }
        tapped_for_mana_color = -2;
      }
      else if (iVar2 == 3)
      {
        instance->info_slot = x_value;
      }
    }
    else if (event == EVENT_RESOLVE_ACTIVATION)
    {
      if (iVar2 == 2)
      {
        discard(instance->targets[0].player, 0, player);
        return 0;
      }
      if (iVar2 == 3)
      {
        artifact_animation(player, card, instance->parent_controller, instance->parent_card, 1,
                           instance->info_slot, instance->info_slot, 0, 0);
        return 0;
      }
    }
  }
  return 0;
}

/* October 2022 private .bss cache @ 0x2774140, original line 17. */
static const Watermark* watermarks = NULL;


/* ManalinkEh 2022: 0x25c49b0; original cards/unstable.c:1761. */
static int test_has_rarity_val(int iid, int val, int player, int card)
{
  const Rarities *pRVar1;

  pRVar1 = get_rarity((unsigned int)cards_data[iid].id);
  return (unsigned int)((unsigned int)pRVar1->default_rarity == val);
}


/* ManalinkEh 2022: 0x25c49f0; original cards/unstable.c:113. */
static watermark_t get_watermark_by_iid(int iid)
{
  const Rarities *pRVar1;
  watermark_t wVar2;

  pRVar1 = get_rarity((unsigned int)cards_data[iid].id);
  wVar2 = (watermark_t)pRVar1->watermark;
  if (wVar2 != WMARK_NONE)
  {
    if (watermarks == NULL)
    {
      watermarks = get_watermarks();
    }
    if (watermarks[wVar2].saturation != '\0')
    {
      return wVar2;
    }
  }
  return WMARK_NONE;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static char *target_has_league_watermark(int who_chooses, int player, int card,
                                         int targeting_player, int targeting_card);
static int test_has_watermark_val(int iid, int val, int player, int card);

/* ManalinkEh 2022: 0x25c4af0; original cards/unstable.c:150. */
static int test_has_watermark_val(int iid, int val, int player, int card)
{
  watermark_t wVar1;

  wVar1 = get_watermark_by_iid(iid);
  return (unsigned int)(val == wVar1);
}

/* ManalinkEh 2022: 0x25c5300; original cards/unstable.c:274. */
int card_knight_of_the_widget(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  unsigned int iid;
  watermark_t wVar3;
  int iVar4;
  int card_00;

  vigilance_2022(player, card, event);
  if ((((event - EVENT_POWER < 2) && (card == affected_card)) &&
       (affected_card_controller == player)) &&
      (player != -1))
  {
    iVar1 = is_humiliated_for_ability(player, card, AL_STATIC);
    if (iVar1 == 0)
    {

      iVar1 = 0;
      if (0 < active_cards_count[player])
      {
        card_00 = 0;
        do
        {
          instance = in_play(player, card_00);
          if (instance != NULL)
          {
            iid = instance->internal_card_id;

            if ((int)iid < 0)
            {
              iid = (unsigned int)instance->backup_internal_card_id;
            }
            wVar3 = get_watermark_by_iid(iid);
            if (wVar3 == WMARK_ORDER_OF_THE_WIDGET)
            {
              iVar4 = is_what(player, card_00, 0x47);
              iVar1 = (iVar1 + 1) - (unsigned int)(iVar4 == 0);
            }
          }
          card_00 = card_00 + 1;
        } while (card_00 < active_cards_count[player]);
      }
      set_characteristic_pt(iVar1);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25c4bb0; original cards/unstable.c:752. */
char *target_has_black_border(int who_chooses, int player, int card, int targeting_player,
                              int targeting_card)
{
  uint32_t csvid;
  border_t bVar1;
  char *pcVar2;

  csvid = get_id(player, card);
  bVar1 = get_border_color(csvid);
  pcVar2 = NULL;
  if (bVar1 != BORDER_BLACK)
  {
    pcVar2 = "black-bordered";
  }
  return pcVar2;
}

/* ManalinkEh 2022: 0x25c4a60; original cards/unstable.c:756. */
static int test_has_any_watermark(int iid, int val, int player, int card)
{
  card_instance_t *instance;
  unsigned int iid_00;
  watermark_t wVar2;

  instance = get_card_instance(player, card);
  iid_00 = instance->internal_card_id;

  if ((int)iid_00 < 0)
  {
    iid_00 = (unsigned int)instance->backup_internal_card_id;
  }
  wVar2 = get_watermark_by_iid(iid_00);
  return (unsigned int)(wVar2 != WMARK_NONE);
}

/* ManalinkEh 2022: 0x25c4aa0; original cards/unstable.c:1067. */
static char *target_has_league_watermark(int who_chooses, int player, int card,
                                         int targeting_player, int targeting_card)
{
  card_instance_t *instance;
  unsigned int iid;
  watermark_t wVar2;

  instance = get_card_instance(player, card);
  iid = instance->internal_card_id;

  if ((int)iid < 0)
  {
    iid = (unsigned int)instance->backup_internal_card_id;
  }
  wVar2 = get_watermark_by_iid(iid);
  if (wVar2 != WMARK_LEAGUE_OF_DASTARDLY_DOOM)
  {
    return "watermark";
  }
  return NULL;
}

/* ManalinkEh 2022: 0x25c6b60; original cards/unstable.c:1072. */
int card_rumors_of_my_death(int player, int card, event_t event)
{
  int result;
  target_t sel;
  target_definition_t td;
  test_definition_t test;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    base_target_definition(player, card, &td, 0x47);
    td.preferred_controller = player;
    td.allowed_controller = player;
    td.special = 0x8000;
    td.extra = (int32_t)target_has_league_watermark;
    new_default_test_definition(
        &test, 0x47, "Select target permanent card with a League of Dastardly Doom watermark.");
    test.special_selection_function = test_has_watermark_val;
    test.value_for_special_selection_function = 2;
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 3, 1, 0, 0, 0, 0, 0,
                                         &td, NULL);
      if (result == 0)
      {
        return 0;
      }
      result = new_special_count_grave(player, &test);
      if (0 < result)
      {
        result = graveyard_has_shroud(player);
        return (unsigned int)(result == 0);
      }
      return 0;
    }
    if (event == EVENT_ACTIVATE)
    {
      generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 3, 1, 0, 0, 0, 0, 0, NULL, NULL);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      result = select_target(
          player, card + -1000, &td,
          "Select a permanent you control with a League of Dastardly Doom watermark.", &sel);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      select_target_from_grave_source(player, card, player, SFG_0, 2, &test, 0);
      if (spell_fizzled != 1)
      {
        kill_card(sel.player, sel.card, KILL_EXILE);
        return 0;
      }
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      result = global_enchantment(player, card, event);
      return result;
    }
    base_target_definition(player, card, &td, 0x47);
    td.preferred_controller = player;
    td.allowed_controller = player;
    td.special = 0x8000;
    td.extra = (int32_t)target_has_league_watermark;
    new_default_test_definition(
        &test, 0x47, "Select target permanent card with a League of Dastardly Doom watermark.");
  }
  test.value_for_special_selection_function = 2;
  test.special_selection_function = test_has_watermark_val;
  if (event != EVENT_RESOLVE_ACTIVATION)
  {
    return 0;
  }
  result = validate_target_from_grave_source(player, card, player, 0);
  if (result < 0)
  {
    spell_fizzled = 1;
    return 0;
  }
  reanimate_permanent(player, card, player, result, REANIMATE_DEFAULT);
  return 0;
}

/* ManalinkEh 2022: 0x25c77a0; original cards/unstable.c:1456. */
int card_hammer_boomtacular(int player, int card, event_t event)
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

  if (((trigger_condition == 0xd3) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&recovered_stack_test.test, 0xff);
    recovered_stack_test.test.special_selection_function = test_has_watermark_val;
    recovered_stack_test.test.value_for_special_selection_function = 3;
    sp_ability_trigger(player, event, 2, TYPE_NONE, &recovered_stack_test.test, player, card);
  }
  if (event == EVENT_SPA)
  {

    default_target_definition(player, card, &recovered_stack_test.target, 0x102);
    recovered_stack_test.test.message[0xc] = '\0';
    recovered_stack_test.test.message[0xd] = '\x12';
    recovered_stack_test.test.message[0xe] = '\0';
    recovered_stack_test.test.message[0xf] = '\0';
    recovered_stack_test.test.message[0x44] = '\0';
    recovered_stack_test.test.message[0x45] = '\0';
    recovered_stack_test.test.message[0x46] = '\0';
    recovered_stack_test.test.message[0x47] = '\0';
    instance = get_card_instance(player, card);
    result = can_target(&recovered_stack_test.target);
    if (result != 0)
    {
      result = new_pick_target(&recovered_stack_test.target, "TARGET_ANY", 0, 0);
      if (result != 0)
      {
        damage_creature(instance->targets[0].player, instance->targets[0].card, 2, player, card);
      }
    }
    instance->number_of_targets = '\0';
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25c79a0; original cards/unstable.c:1573. */
int card_druid_of_the_sacred_beaker(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int iid;
  watermark_t wVar2;
  int iVar3;
  int card_00;
  int iVar4;

  if (event == EVENT_COUNT_MANA)
  {
    iVar4 = 0;
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_025c7a38;
  }
  else if ((event != EVENT_ACTIVATE) && ((event != EVENT_CAN_ACTIVATE || (player != 1))))
  {
    iVar4 = 0;
    goto LAB_025c7a38;
  }
  iVar4 = 0;
  card_00 = 0;
  if (0 < active_cards_count[player])
  {
    do
    {
      while (instance = in_play(player, card_00), instance == NULL)
      {
      LAB_025c79e0:
        card_00 = card_00 + 1;
        if (active_cards_count[player] <= card_00)
          goto LAB_025c7a38;
      }
      iid = instance->internal_card_id;

      if ((int)iid < 0)
      {
        iid = (unsigned int)instance->backup_internal_card_id;
      }
      wVar2 = get_watermark_by_iid(iid);
      if (wVar2 != WMARK_CROSSBREED_LABS)
        goto LAB_025c79e0;
      iVar3 = is_what(player, card_00, 0x47);
      iVar4 = (iVar4 + 1) - (unsigned int)(iVar3 == 0);
      card_00 = card_00 + 1;
    } while (card_00 < active_cards_count[player]);
  }
LAB_025c7a38:
  iVar4 = mana_producing_creature(player, card, event, 0x40, COLOR_GREEN, iVar4);
  return iVar4;
}

/* ManalinkEh 2022: 0x25c7e00; original cards/unstable.c:1715. */
int card_ineffable_blessing_c(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  test_definition_t test;

  if (event == EVENT_RESOLVE_SPELL)
  {

    uVar3 = dialog_impl(player, card, EVENT_RESOLVE_SPELL, "\001\002", "\001\014", "\001\003",
                        "White-bordered", 1, 1, "Silver-bordered", 1, 1, 0, 0, 0, 0);
    instance = get_card_instance(player, card);
    instance->targets[5].player = uVar3 | 0x46570000;
    result = create_targetted_legacy_effect(player, card, fx_0, player, card);
    alternate_legacy_text(uVar3, player, result);
  }
  else if ((trigger_condition == 0xdb) && (event - EVENT_TRIGGER < 2))
  {

    instance = get_card_instance(player, card);
    uVar3 = instance->targets[5].player;
    if (((short)(uVar3 >> 0x10) != 0x4657) || (uVar3 = uVar3 & 0xffff, 1 < uVar3 - 1))
    {
      return 0;
    }
    default_test_definition(&test, 2);
    test.special_selection_function = test_has_border_val;
    test.value_for_special_selection_function = uVar3;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  else if (event == EVENT_ETB)
  {
    draw_a_card(player);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25c7ff0; original cards/unstable.c:1767. */
int card_ineffable_blessing_d(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  test_definition_t test;

  if (event == EVENT_RESOLVE_SPELL)
  {

    uVar3 =
        dialog_impl(player, card, EVENT_RESOLVE_SPELL, "\001\002", "\001\014", "\001\003", "Common",
                    1, 1, "Uncommon", 1, 1, "Rare", 1, 1, "Mythic Rare", 1, 1, 0, 0, 0, 0);
    instance = get_card_instance(player, card);
    instance->targets[5].player = uVar3 | 0x46580000;
    result = create_targetted_legacy_effect(player, card, fx_0, player, card);
    alternate_legacy_text(uVar3, player, result);
  }
  else if ((trigger_condition == 0xdb) && (event - EVENT_TRIGGER < 2))
  {

    instance = get_card_instance(player, card);
    uVar3 = instance->targets[5].player;
    if (((short)(uVar3 >> 0x10) != 0x4658) || (uVar3 = uVar3 & 0xffff, uVar3 - 1 < 4))
    {
      return 0;
    }
    default_test_definition(&test, 2);
    test.special_selection_function = test_has_rarity_val;
    test.value_for_special_selection_function = uVar3;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  else if (event == EVENT_ETB)
  {
    draw_a_card(player);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25c8610; original cards/unstable.c:2127. */
int card_everythingamajig_b(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  test_definition_t test;

  if ((event - EVENT_RESOLVE_ACTIVATION < 2) || (event == EVENT_ACTIVATE))
  {
    iVar2 = hand_count[player];
    iVar1 = can_sacrifice_this_as_cost(player, card);
    iVar2 = dialog_impl(player, card, event, "\001\002", "Draw", (unsigned int)(iVar2 < 1), 1,
                        "\001\020", 2, 0, 0, 0, 0, 0, "\001\025", "Gain life", 1, 1, "\001\020", 8,
                        0, 0, 0, 0, 0, "\001\025", "Hand to battlefield", iVar1, 1, "\001\020", 4,
                        0, 0, 0, 0, 0, "\001\025", 0, 0, 0, 0);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar1 = can_use_activated_abilities_2022(player, card);
      return (unsigned int)(iVar1 != 0 && iVar2 != 0);
    }
    if ((event == EVENT_ACTIVATE) && (iVar2 == 3))
    {
      kill_card(player, card, KILL_SACRIFICE);
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      if (iVar2 == 2)
      {
        gain_life(player, 10);
        return 0;
      }
      if (iVar2 != 3)
      {
        if (iVar2 != 1)
        {
          return 0;
        }
        draw_a_card(player);
        return 0;
      }
      new_default_test_definition(&test, 0x47, "Select a silver-bordered permanent card.");
      test.special_selection_function = test_has_border_val;
      test.value_for_special_selection_function = 2;
      new_global_tutor(player, player, 4, 4, 0, 2, &test);
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int fx_ineffable_blessing_f(int player, int card, event_t event);

/* ManalinkEh 2022: 0x25c4c30; original cards/unstable.c:1825. */
static int fx_ineffable_blessing_f(int player, int card, event_t event)
{
  int result;

  result = txtfx(player, card, event, CARD_ID_INEFFABLE_BLESSING_F);
  if (result != 0x7fffffff)
  {
    sprintf(set_legacy_effect_name_addr, "%d", result);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25c8210; original cards/unstable.c:1832. */
int card_ineffable_blessing_f(int player, int card, event_t event)
{
  unsigned int uVar1;
  int result;
  card_instance_t *instance;
  test_definition_t test;

  if (event == EVENT_RESOLVE_SPELL)
  {

    if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
    {
      result = recorded_rand(player, 5);
      result = result + 1;
    }
    else
    {
      result = choose_a_number(player, "Choose a number.", 0xfff);
    }
    make_txtfx(player, card, fx_ineffable_blessing_f, CARD_ID_INEFFABLE_BLESSING_F, result);
  }
  else if ((trigger_condition == 0xdb) && (event - EVENT_TRIGGER < 2))
  {

    instance = get_card_instance(player, card);
    uVar1 = instance->targets[5].player;
    if ((short)(uVar1 >> 0x10) != 0x465a)
    {
      return 0;
    }
    default_test_definition(&test, 2);
    test.value_for_special_selection_function = uVar1 & 0xffff;
    test.special_selection_function = test_words_in_name_val;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  else if (event == EVENT_ETB)
  {
    draw_a_card(player);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25c4c80; original cards/unstable.c:2324. */
static int fx_staff_of_the_letter_magus(int player, int card, event_t event)
{
  int argument2;
  const char *pcVar1;

  argument2 = txtfx(player, card, event, CARD_ID_STAFF_OF_THE_LETTER_MAGUS);
  if ((argument2 != 0x7fffffff) && (argument2 - 0x42U < 0x19))
  {
    pcVar1 = strchr("AEIOUNRST", argument2);
    if (pcVar1 == NULL)
    {
      sprintf(set_legacy_effect_name_addr, "%c", (int)(char)argument2);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25c4d00; original cards/unstable.c:20. */
static int do_dialog_lots_of_choices(int player, int src_player, int src_card, char **choices,
                                     int num_choices)
{
  int iVar1;
  int iVar2;
  char *buffer;
  int iStack_bd0;
  char buf[3000];
  do
  {
    iStack_bd0 = 0;
    do
    {
      strncpy(buf, " (More)", 8);
      if (iStack_bd0 < num_choices)
      {
        iVar1 = sprintf(buf + 7, "\n %s", choices[iStack_bd0]);
        buffer = buf + iVar1 + 7;
        iVar1 = 1;
        do
        {
          if (num_choices - iStack_bd0 == iVar1)
            break;
          iVar2 = sprintf(buffer, "\n %s", (choices + iStack_bd0)[iVar1]);
          iVar1 = iVar1 + 1;
          buffer = buffer + iVar2;
        } while (iVar1 != 10);
      }
      iVar1 = do_dialog(player, src_player, src_card, -1, -1, buf, 0);
      if (iVar1 != 0)
      {
        return iStack_bd0 + -1 + iVar1;
      }
      iStack_bd0 = iStack_bd0 + 10;
    } while (iStack_bd0 < num_choices);
  } while (true);
}


/* ManalinkEh 2022: 0x25c4fd0; original cards/unstable.c:72. */
static watermark_t choose_a_watermark(int player, int src_player, int src_card)
{
  if (watermarks == NULL)
    watermarks = get_watermarks();
  int idxs[256];
  int num_idxs = 0;
  for (int i = 0; i < 255; ++i)
    if (watermarks[i].name != NULL && watermarks[i].saturation != '\0')
      idxs[num_idxs++] = i;
  if (num_idxs > 0)
  {
    int choice;
    if (ai_is_speculating == 1 || (player == 1 && ((unsigned char)trace_mode & 2) == 0))
    {
      choice = recorded_rand(player, num_idxs);
      if (ai_is_speculating != 1)
      {
        char buf[300];
        sprintf(buf, " %s\n", watermarks[idxs[choice]].disp);
        do_dialog(player, src_player, src_card, -1, -1, buf, 0);
      }
    }
    else
    {
      char *choices[num_idxs];
      for (int i = 0; i < num_idxs; ++i)
        choices[i] = watermarks[idxs[i]].disp;
      choice = do_dialog_lots_of_choices(player, src_player, src_card, choices, num_idxs);
    }
    return (watermark_t)idxs[choice];
  }
  return WMARK_NONE;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int fx_stamp_of_approval(int player, int card, event_t event);

/* ManalinkEh 2022: 0x25c4f50; original cards/unstable.c:2413. */
static int fx_stamp_of_approval(int player, int card, event_t event)
{
  unsigned int uVar1;

  uVar1 = txtfx(player, card, event, CARD_ID_STAMP_OF_APPROVAL);
  if ((uVar1 != 0x7fffffff) && (uVar1 < 0xff))
  {
    if (watermarks == NULL)
    {
      backtrace_and_abort("cards/unstable.c:2418\nwatermarks");
    }
    strcpy(set_legacy_effect_name_addr, watermarks[uVar1].disp);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25c8e10; original cards/unstable.c:2423. */
int card_stamp_of_approval(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int iid;
  watermark_t wVar3;
  watermark_t wVar4;

  if (event == EVENT_RESOLVE_SPELL)
  {

    wVar3 = choose_a_watermark(player, player, card);
    if (wVar3 != WMARK_NONE)
    {
      make_txtfx(player, card, fx_stamp_of_approval, CARD_ID_STAMP_OF_APPROVAL, wVar3);
    }
  }
  else if ((event - EVENT_POWER < 2) && (affected_card_controller == player))
  {
    result = is_what(player, affected_card, 2);
    if (result != 0)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {

        instance = get_card_instance(player, card);
        wVar3 = instance->targets[5].player;
        wVar4 = wVar3 & 0xffff;
        if (((short)(wVar3 >> 0x10) == 0x46b6) && (wVar4 < WMARK_NONE))
        {
          instance = get_card_instance(affected_card_controller, affected_card);
          iid = instance->internal_card_id;

          if ((int)iid < 0)
          {
            iid = (unsigned int)instance->backup_internal_card_id;
          }
          wVar3 = get_watermark_by_iid(iid);
          if (wVar4 == wVar3)
          {
            event_result = event_result + 1;
          }
        }
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25c8500; original cards/unstable.c:2053. */
int card_border_guardian(int player, int card, event_t event)
{
  uint32_t csvid;
  border_t bVar1;
  int ability;
  int sp_ability;
  int alternate_legacy_text_number;

  sp_ability_trigger(player, event, 2, TYPE_ANY, NULL, player, card);
  if (event == EVENT_SPA)
  {
    csvid = get_id(trigger_cause_controller, trigger_cause);
    bVar1 = get_border_color(csvid);
    if (bVar1 == BORDER_WHITE)
    {
      alternate_legacy_text_number = 2;
      sp_ability = 0;
      ability = 0x40000000;
    LAB_025c8595:
      pump_ability_until_eot_no_repeat_alt(player, card, player, card, ability, sp_ability,
                                           alternate_legacy_text_number);
      return 0;
    }
    if (bVar1 == BORDER_SILVER)
    {
      add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
    }
    else if (bVar1 == BORDER_BLACK)
    {
      alternate_legacy_text_number = 1;
      sp_ability = 0x10;
      ability = 0;
      goto LAB_025c8595;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25c4940; original cards/unstable.c:1229. */
static int test_has_been_banned_or_restricted(int iid, int val, int player, int card)
{
  enum
  {
    CPF_EVER_BANNED_OR_RESTRICTED = 1
  };

  return cards_ptr[cards_data[iid].id]->flags & CPF_EVER_BANNED_OR_RESTRICTED;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int roll_xd6_shim(int player, int card);

/* ManalinkEh 2022: 0x25c5150; original cards/unstable.c:199. */
int card_go_to_jail(int player, int card, event_t event)
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
    instance->number_of_targets = '\0';
  }
  else if ((trigger_condition == 0xc9) && (event - EVENT_TRIGGER < 2))
  {
    result = owner_of_obliviated_card(player, card);
    upkeep_trigger_ability(player, card, event, result);
  }
  else if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    roll_2d6t(current_turn, player, card, -1, -1, &td.who_chooses, &td.allowed_controller);
    if (td.who_chooses == td.allowed_controller)
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25c57c0; original cards/unstable.c:541. */
int card_time_out(int player, int card, event_t event)
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
            result = roll_d6t(player, player, card, instance->targets[0].player,
                              instance->targets[0].card);
            put_permanent_under_the_first_x_card_of_its_owners_library(
                instance->targets[0].player, instance->targets[0].card, result);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_025c5815;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
LAB_025c5815:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25c69d0; original cards/unstable.c:1005. */
int card_inhumaniac(int player, int card, event_t event)
{
  int result;

  upkeep_trigger_ability(player, card, event, player);
  if (event != EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    return 0;
  }

  result = roll_d6t(player, player, card, -1, -1);
  if (result - 3U < 2)
  {
    result = 1;
  }
  else
  {
    if (result < 5)
    {
      if (result != 1)
      {
        return 0;
      }
      remove_all_counters_from_source(player, card, player, card, COUNTER_P1_P1);
      return 0;
    }
    result = 2;
  }
  add_counters_from_source(player, card, player, card, COUNTER_P1_P1, result);
  return 0;
}

/* ManalinkEh 2022: 0x25c7090; original cards/unstable.c:1204. */
int card_sly_spy_f(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int amount;
  int iVar3;
  unsigned int uVar4;
  unsigned int uStack_20;
  int times_damaged[2];

  iVar1 = damage_dealt_by_me(player, card, event,
                             DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                                 DDBM_MUST_BE_COMBAT_DAMAGE);
  if (iVar1 == 0)
  {
    return 0;
  }

  instance = get_card_instance(player, card);

  uStack_20 = 0;
  uVar4 = instance->targets[1].player;
  instance->targets[1].player = 0;
  times_damaged[0] = uVar4 & 0xff;
  times_damaged[1] = uVar4 >> 8 & 0xff;
  do
  {
    uVar4 = uStack_20 ^ current_turn;

    iVar1 = times_damaged[uVar4];
    if (0 < iVar1)
    {
      iVar3 = 0;
      do
      {
        amount = roll_d6t(player, player, card, uVar4, -1);
        iVar3 = iVar3 + 1;
        lose_life_2022(uVar4, amount);
      } while (iVar3 != iVar1);
    }
    uStack_20 = uStack_20 + 1;
  } while (uStack_20 != 2);
  return 0;
}

/* ManalinkEh 2022: 0x25c73a0; original cards/unstable.c:1263. */
int card_squirrel_powered_scheme(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_STATIC_EFFECTS)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      add_to_die_rolls[player] = add_to_die_rolls[player] + 2;
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25c73f0; original cards/unstable.c:1308. */
int card_box_of_free_range_goblins(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    result = roll_d6t(player, player, card, -1, -1);
    generate_tokens_by_id_2022(player, card, 0x493a, result);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25c7510; original cards/unstable.c:1370. */
int card_garbage_elemental_d(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  cascade_2022(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }

  opponent_or_pw_target_definition(player, card, &td);
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

      result =
          roll_d6t(player, player, card, instance->targets[0].player, instance->targets[0].card);
      damage_creature(instance->targets[0].player, instance->targets[0].card, result, player, card);
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x25c7630; original cards/unstable.c:1422. */
int card_hammer_helper(int player, int card, event_t event)
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
            result = roll_d6t(player, player, card, instance->targets[0].player,
                              instance->targets[0].card);
            effect_act_of_treason_and_modify_pt_or_abilities(
                player, card, instance->targets[0].player, instance->targets[0].card, result, 0, 0,
                0);
          }
          kill_card(player, card, KILL_DESTROY);
          return 0;
        }
        goto LAB_025c767d;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
LAB_025c767d:
  result = aot_like_spell(player, card, event, &td, "TARGET_CREATURE", 0);
  return result;
}

/* ManalinkEh 2022: 0x25c7910; original cards/unstable.c:1497. */
int card_painiac(int player, int card, event_t event)
{
  int power;

  upkeep_trigger_ability(player, card, event, player);
  if (event != EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    return 0;
  }
  power = roll_d6t(player, player, card, -1, -1);
  pump_until_eot_merge_previous_2022(player, card, player, card, power, 0);
  return 0;
}

/* ManalinkEh 2022: 0x25c4be0; original cards/unstable.c:1641. */
static int roll_xd6_shim(int player, int card)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  result = roll_xd6t(player, instance->info_slot, player, card, -1, -1);
  return result;
}

/* ManalinkEh 2022: 0x25c7bb0; original cards/unstable.c:1645. */
int card_hydradoodle(int player, int card, event_t event)
{
  multi_x_spell(player, card, event, 2);
  enters_the_battlefield_with_counters_fn(player, card, event, COUNTER_P1_P1, roll_xd6_shim);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25c7a90; original cards/unstable.c:1599. */
int card_earl_of_squirrel(int player, int card, event_t event)
{
  int result;

  squirrellink(player, card, event);
  if (event == EVENT_CHANGE_TYPE)
  {
    if (affected_card_controller == player)
    {
      result = is_token(player, affected_card);
      if (result != 0)
      {
        result = is_what(affected_card_controller, affected_card, 2);
        if (result != 0)
        {
          result = is_humiliated_for_ability(player, card, AL_STATIC);
          if (result == 0)
          {
            add_a_subtype_2022(affected_card_controller, affected_card, SUBTYPE_SQUIRREL);
          }
        }
      }
    }
  }
  else if (((event - EVENT_POWER < 2) && (affected_card_controller == player)) &&
           (card != affected_card))
  {
    result = has_subtype(player, affected_card, SUBTYPE_SQUIRREL);
    if (result != 0)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        event_result = event_result + 1;
      }
    }
  }
  return 0;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25c7470; original cards/unstable.c:1355. */
int card_garbage_elemental_c(int player, int card, event_t event)
{
  int howmany;

  battle_cry_2022(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  howmany = roll_2d6t_difference(player, player, card, -1, -1);
  generate_tokens_by_id(player, card, 0x493a, howmany);
  return 0;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25c7170; original cards/unstable.c:1233. */
int card_spike_tournament_grinder(int player, int card, event_t event)
{
  int result;
  test_definition_2022_t test;

  phyrexian_casting_costs_2022(player, card, event, 2, 2, 0, 0, 0, 0, 0);
  if (event == EVENT_CAN_ACTIVATE)
  {
    result = has_phyrexian_mana_for_activated_ability_2022(player, card, 0, 4, 0, 0, 0, 0);
    if (result != 0)
    {
      result = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 0x800, 0, 0, 0, 0,
                                              0, 0, 0, NULL, NULL);
      return result;
    }
  }
  else if (event == EVENT_ACTIVATE)
  {
    result = charge_phyrexian_mana_for_activated_ability_2022(player, card, 0, 4, 0, 0, 0, 0);
    if (result != 0)
    {
      result = generic_activated_ability_2022(player, card, EVENT_ACTIVATE, 0x800, 0, 0, 0, 0, 0, 0,
                                              0, NULL, NULL);
      return result;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    new_default_test_definition_2022(
        &test, 0xff, "Select a card that has been banned or restricted in a Constructed format.");
    test.special_selection_function = test_has_been_banned_or_restricted;
    generic_wish_effect_2022(player, card, &test, 0x2b);
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x25c5920; original cards/unstable.c:576. */
int card_very_cryptic_command_b(int player, int card, event_t event)
{
  typedef enum
  {
    CHOICE_UNTAP = 1,
    CHOICE_TAP = 2,
    CHOICE_DISCARD_DRAW = 3,
    CHOICE_RAISE = 4
  } CommandChoices;
  CommandChoices CVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  card_instance_t *instance;
  unsigned int uVar6;
  CommandChoices *pCVar7;
  CommandChoices *pCVar8;
  int iStack_278;
  CommandChoices choices[2];
  target_definition_2022_t td_permanent;
  target_definition_2022_t td_player;
  test_definition_2022_t test_instant_sorcery;
  test_definition_2022_t test;

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
    else if ((event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET) &&
             (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  default_target_definition_2022(player, card, &td_permanent, 0x47);
  td_permanent.preferred_controller = player;
  iVar2 = target_available_2022(player, card, &td_permanent);
  default_target_definition_2022(player, card, &td_player, 0);
  td_player.zone = 0x1000;
  iVar3 = can_target_2022(&td_player);
  new_default_test_definition_2022(&test_instant_sorcery, 0x38,
                                   "Select target instant or sorcery card.");
  iVar4 = new_special_count_grave_2022(player, &test_instant_sorcery);
  uVar6 = 0;
  if (0 < iVar4)
  {
    iVar4 = graveyard_has_shroud(player);
    uVar6 = (unsigned int)(iVar4 == 0);
  }
  choices[0] = dialog_impl(player, card, event, "\001\005", choices + 1, "\001\015", "\001\002",
                           "Untap", (unsigned int)(1 < iVar2), 1, "Tap", iVar3, 1, "Discard, draw",
                           1, 1, "Graveyard to hand", uVar6, 1, 0, 0, 0, 0);
  if (event == EVENT_CAN_CAST)
  {
    return (unsigned int)(choices[0] != 0);
  }
  instance = get_card_instance(player, card);
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      pCVar8 = choices;
      instance->number_of_targets = '\0';
      iVar2 = 0;
      do
      {
        if (spell_fizzled == 1)
        {
          return 0;
        }
        CVar1 = *pCVar8;
        if (CVar1 == CHOICE_TAP)
        {
          iVar3 = iVar2 + 1;
          new_pick_target_2022(&td_player, "TARGET_PLAYER", iVar2, 1);
        }
        else if (CVar1 == CHOICE_RAISE)
        {
          iVar3 = iVar2 + 1;
          select_target_from_grave_source_2022(player, card, player, SFG_0, 2,
                                               &test_instant_sorcery, iVar2);
        }
        else
        {
          iVar3 = iVar2;
          if (CVar1 == CHOICE_UNTAP)
          {
            pick_up_to_n_targets_2022(&td_permanent, "TARGET_PERMANENT", 2);
            iVar3 = iVar2 + 2;
          }
        }
        pCVar8 = pCVar8 + 1;
        iVar2 = iVar3;
      } while (choices + 2 != pCVar8);
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    pCVar8 = choices;
    iStack_278 = 0;
    iVar2 = 0;
    pCVar7 = pCVar8;
    do
    {
      CVar1 = *pCVar7;
      if (CVar1 == CHOICE_TAP)
      {
        iVar3 = validate_target_2022(player, card, &td_player, iVar2);
        if (iVar3 != 0)
          goto LAB_025c5d0b;
      LAB_025c5d41:
        instance->targets[iVar2].player = -1;
        iVar2 = iVar2 + 1;
      }
      else if (CVar1 == CHOICE_RAISE)
      {
        iVar3 = validate_target_from_grave_source(player, card, player, iVar2);
        if (iVar3 < 0)
          goto LAB_025c5d41;
      LAB_025c5d0b:
        iVar2 = iVar2 + 1;
        iStack_278 = iStack_278 + 1;
      }
      else if (CVar1 == CHOICE_UNTAP)
      {
        iVar3 = validate_target_2022(player, card, &td_permanent, iVar2);
        if (iVar3 == 0)
        {
          instance->targets[iVar2].player = -1;
        }
        else
        {
          iStack_278 = iStack_278 + 1;
        }
        iVar3 = validate_target_2022(player, card, &td_permanent, iVar2);
        if (iVar3 == 0)
        {
          instance->targets[iVar2 + 1].player = -1;
        }
        else
        {
          iStack_278 = iStack_278 + 1;
        }
        iVar2 = iVar2 + 2;
      }
      pCVar7 = pCVar7 + 1;
    } while (pCVar7 != choices + 2);
    if (iStack_278 < 1)
    {
      spell_fizzled = 1;
    }
    else
    {
      iVar2 = 0;
      do
      {
        CVar1 = *pCVar8;
        if (CVar1 == CHOICE_DISCARD_DRAW)
        {
          iVar3 = hand_count[player];
          discard_all(player);
          draw_cards(player, iVar3);
        }
        else if (CVar1 < CHOICE_RAISE)
        {
          if (CVar1 == CHOICE_UNTAP)
          {
            iVar3 = instance->targets[iVar2].player;
            if (-1 < iVar3)
            {
              untap_card(iVar3, instance->targets[iVar2].card);
            }
            iVar3 = instance->targets[iVar2 + 1].player;
            if (-1 < iVar3)
            {
              untap_card(iVar3, instance->targets[iVar2 + 1].card);
            }
            iVar2 = iVar2 + 2;
          }
          else if (CVar1 == CHOICE_TAP)
          {
            if (-1 < instance->targets[iVar2].player)
            {
              default_test_definition_2022(&test, 0x47);
              test.special_selection_function = test_words_in_name_val;
              test.value_for_special_selection_function = 1;
              new_manipulate_all_2022(player, card, instance->targets[iVar2].player, &test,
                                      ACT_TAP);
            }
            goto LAB_025c5ba2;
          }
        }
        else if (CVar1 == CHOICE_RAISE)
        {
          if ((-1 < instance->targets[iVar2].player) &&
              (iVar3 = validate_target_from_grave_source(player, card, player, iVar2), -1 < iVar3))
          {
            from_grave_to_hand(player, iVar3, 0);
          }
        LAB_025c5ba2:
          iVar2 = iVar2 + 1;
        }
        pCVar8 = pCVar8 + 1;
      } while (pCVar8 != choices + 2);
    }
    kill_card(player, card, KILL_DESTROY);
    return 0;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x25c5f90; original cards/unstable.c:760. */
int card_very_cryptic_command_e(int player, int card, event_t event)
{
  typedef enum
  {
    CHOICE_COUNTER = 1,
    CHOICE_BOUNCE = 2,
    CHOICE_UNTAP = 3,
    CHOICE_MILL = 4
  } CommandChoices;
  CommandChoices CVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  card_instance_t *instance;
  target_definition_2022_t *td;
  CommandChoices *pCVar6;
  CommandChoices *pCVar7;
  int iStack_200;
  CommandChoices choices[2];
  target_definition_2022_t td_spell;
  target_definition_2022_t td_creature;
  target_definition_2022_t td_player;
  test_definition_2022_t test;

  if (event == EVENT_CAN_CAST)
  {
    counterspell_target_definition_2022(player, card, &td_spell, 0);
    td_spell.special = 0x8000;
    td_spell.extra = (int32_t)target_has_black_border;
    iVar2 = counterspell_2022(player, card, EVENT_CAN_CAST, &td_spell, -1);
    default_target_definition_2022(player, card, &td_creature, 2);
    iVar3 = can_target_2022(&td_creature);
    default_target_definition_2022(player, card, &td_player, 0);
    td_player.zone = 0x1000;
    iVar4 = can_target_2022(&td_player);
    iVar3 = dialog_impl(player, card, EVENT_CAN_CAST, "\001\005", choices + 1, "\001\015",
                        "\001\002", "Counter spell", iVar2, 1, "Creature to hand", iVar3, 1,
                        "Untap permanents", 1, 1, "Mill", iVar4, 1, 0, 0, 0, 0);
    if (iVar3 != 0)
    {
      return (-(unsigned int)(iVar2 == 0) & 0xffffff9e) + 99;
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      counterspell_target_definition_2022(player, card, &td_spell, 0);
      td_spell.special = 0x8000;
      td_spell.extra = (int32_t)target_has_black_border;
      iVar2 = counterspell_2022(player, card, EVENT_CAN_CAST, &td_spell, -1);
      default_target_definition_2022(player, card, &td_creature, 2);
      iVar3 = can_target_2022(&td_creature);
      default_target_definition_2022(player, card, &td_player, 0);
      td_player.zone = 0x1000;
      iVar4 = can_target_2022(&td_player);
      choices[0] = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\005", choices + 1, "\001\015",
                               "\001\002", "Counter spell", iVar2, 1, "Creature to hand", iVar3, 1,
                               "Untap permanents", 1, 1, "Mill", iVar4, 1, 0, 0, 0, 0);
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        if (player == 1)
        {
          ai_modifier = ai_modifier + -0x54;
        }
        instance->number_of_targets = '\0';
        pCVar7 = choices;
        iVar2 = 0;
        do
        {
          if (spell_fizzled == 1)
          {
            return 0;
          }
          CVar1 = *pCVar7;
          if (CVar1 == CHOICE_BOUNCE)
          {
            iVar3 = iVar2 + 1;
            new_pick_target_2022(&td_creature, "TARGET_CREATURE", iVar2, 1);
          }
          else if (CVar1 == CHOICE_MILL)
          {
            iVar3 = iVar2 + 1;
            new_pick_target_2022(&td_player, "TARGET_PLAYER", iVar2, 1);
          }
          else
          {
            iVar3 = iVar2;
            if (CVar1 == CHOICE_COUNTER)
            {
              iVar3 = iVar2 + 1;
              counterspell_2022(player, card, EVENT_CAST_SPELL, &td_spell, iVar2);
            }
          }
          pCVar7 = pCVar7 + 1;
          iVar2 = iVar3;
        } while (choices + 2 != pCVar7);
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL || event == EVENT_CAN_CHANGE_TARGET) ||
           (event == EVENT_CHANGE_TARGET))
  {
    counterspell_target_definition_2022(player, card, &td_spell, 0);
    td_spell.special = 0x8000;
    td_spell.extra = (int32_t)target_has_black_border;
    iVar2 = counterspell_2022(player, card, EVENT_CAN_CAST, &td_spell, -1);
    default_target_definition_2022(player, card, &td_creature, 2);
    iVar3 = can_target_2022(&td_creature);
    default_target_definition_2022(player, card, &td_player, 0);
    td_player.zone = 0x1000;
    iVar4 = can_target_2022(&td_player);
    choices[0] = dialog_impl(player, card, event, "\001\005", choices + 1, "\001\015", "\001\002",
                             "Counter spell", iVar2, 1, "Creature to hand", iVar3, 1,
                             "Untap permanents", 1, 1, "Mill", iVar4, 1, 0, 0, 0, 0);
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      pCVar7 = choices;
      iStack_200 = 0;
      iVar2 = 0;
      pCVar6 = pCVar7;
      do
      {
        CVar1 = *pCVar6;
        td = &td_creature;
        if ((CVar1 == CHOICE_BOUNCE) || (td = &td_player, CVar1 == CHOICE_MILL))
        {
          iVar3 = validate_target_2022(player, card, td, iVar2);
          if (iVar3 == 0)
            goto LAB_025c66e9;
        LAB_025c66b1:
          iVar2 = iVar2 + 1;
          iStack_200 = iStack_200 + 1;
        }
        else if (CVar1 == CHOICE_COUNTER)
        {
          iVar3 = counterspell_validate_2022(player, card, &td_spell, iVar2);
          if (iVar3 != 0)
            goto LAB_025c66b1;
        LAB_025c66e9:
          instance->targets[iVar2].player = -1;
          iVar2 = iVar2 + 1;
        }
        pCVar6 = pCVar6 + 1;
      } while (choices + 2 != pCVar6);
      iVar2 = 0;
      if (iStack_200 < 1)
      {
        spell_fizzled = 1;
      }
      else
      {
        do
        {
          CVar1 = *pCVar7;
          if (CVar1 == CHOICE_UNTAP)
          {
            default_test_definition_2022(&test, 0x47);
            test.special_selection_function = test_has_any_watermark;
            new_manipulate_all_2022(player, card, player, &test, ACT_UNTAP);
          }
          else
          {
            if (CVar1 < CHOICE_MILL)
            {
              if (CVar1 == CHOICE_COUNTER)
              {
                iVar3 = instance->targets[iVar2].player;
                if (-1 < iVar3)
                {
                  real_counter_a_spell(player, card, iVar3, instance->targets[iVar2].card);
                }
              }
              else
              {
                if (CVar1 != CHOICE_BOUNCE)
                  goto LAB_025c6211;
                iVar3 = instance->targets[iVar2].player;
                if (-1 < iVar3)
                {
                  bounce_permanent(iVar3, instance->targets[iVar2].card);
                }
              }
            }
            else
            {
              if (CVar1 != CHOICE_MILL)
                goto LAB_025c6211;
              iVar4 = roll_xd6t(player, 2, player, card, instance->targets[iVar2].player, -1);
              iVar3 = instance->targets[iVar2].player;
              if (-1 < iVar3)
              {
                mill(iVar3, iVar4);
              }
            }
            iVar2 = iVar2 + 1;
          }
        LAB_025c6211:
          pCVar7 = pCVar7 + 1;
        } while (choices + 2 != pCVar7);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x25c6ed0; original cards/unstable.c:1141. */
int card_sly_spy_a(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  const char *puVar4;
  unsigned int uVar5;
  int iVar6;
  int card_00;
  unsigned int uStack_118;
  int iStack_110;
  int times_damaged[2];
  ec_definition_t ec;
  test_definition_2022_t test;
  iVar1 = damage_dealt_by_me(player, card, event,
                             DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                                 DDBM_MUST_BE_COMBAT_DAMAGE);
  if (iVar1 != 0)
  {
    instance = get_card_instance(player, card);
    uStack_118 = 0;
    uVar5 = instance->targets[1].player;
    instance->targets[1].player = 0;
    times_damaged[0] = uVar5 & 0xff;
    times_damaged[1] = uVar5 >> 8 & 0xff;
    do
    {
      uVar5 = uStack_118 ^ current_turn;
      iStack_110 = 0;
      iVar1 = times_damaged[uVar5];
      if (0 < iVar1)
      {
        do
        {
          iVar6 = -1;
          card_00 = 0;
          if (0 < active_cards_count[uVar5])
          {
            do
            {
              instance = in_hand(uVar5, card_00);
              if ((instance != NULL) && (iVar3 = words_in_name(uVar5, card_00), iVar6 < iVar3))
              {
                iVar6 = iVar3;
              }
              card_00 = card_00 + 1;
            } while (card_00 < active_cards_count[uVar5]);
            if (-1 < iVar6)
            {
              default_ec_definition(uVar5, player, &ec);
              ec.effect = 1;
              new_default_test_definition_2022(&test, 0xff, "");
              test.special_selection_function = test_words_in_name_val;
              if (iVar6 == 1)
              {
                puVar4 = "";
              }
              else
              {
                puVar4 = "s";
              }
              test.value_for_special_selection_function = iVar6;
              sprintf(test.message, "Select a card with %d word%s in its name.", iVar6, puVar4);
              new_effect_coercion_2022(&ec, &test);
            }
          }
          iStack_110 = iStack_110 + 1;
        } while (iStack_110 != iVar1);
      }
      uStack_118 = uStack_118 + 1;
    } while (uStack_118 != 2);
    return 0;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x25c4e40; original cards/unstable.c:48. */
static char choose_a_consonant_not_nrst(int player, int src_player, int src_card)
{
  char cVar1;
  int result;
  char *choices[17] = {"B", "C", "D", "F", "G", "H", "J", "K", "L",
                       "M", "P", "Q", "V", "W", "X", "Y", "Z"};
  char buf[300];

  if ((ai_is_speculating != 1) && ((player != 1 || (((unsigned char)trace_mode & 2) != 0))))
  {
    result = do_dialog_lots_of_choices(player, src_player, src_card, choices, 0x11);
    return *choices[result];
  }
  result = recorded_rand(player, 0x11);
  if (ai_is_speculating == 1)
  {
    cVar1 = *choices[result];
  }
  else
  {
    sprintf(buf, " %s\n", choices[result]);
    do_dialog(player, src_player, src_card, -1, -1, buf, 0);
    cVar1 = *choices[result];
  }
  return cVar1;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x25c8c00; original cards/unstable.c:2331. */
int card_staff_of_the_letter_magus(int player, int card, event_t event)
{
  static const unsigned char native_name_fold[256] = {
      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
      0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
      0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c,
      0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
      0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
      0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
      0x7a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
      0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
      0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x2c, 0x66, 0x84, 0x85, 0x86,
      0x87, 0x5e, 0x25, 0x73, 0x3c, 0x8c, 0x8d, 0x7a, 0x8f, 0x90, 0x27, 0x27, 0x22, 0x22, 0x95,
      0x2d, 0x2d, 0x7e, 0x99, 0x73, 0x3e, 0x9c, 0x9d, 0x7a, 0x79, 0x5f, 0x21, 0x24, 0x24, 0x24,
      0x24, 0x7c, 0x25, 0x22, 0x40, 0x5e, 0x3c, 0x7e, 0x2d, 0x40, 0x7e, 0x5e, 0x2b, 0x32, 0x33,
      0x27, 0x75, 0x26, 0x2e, 0x2c, 0x31, 0x5e, 0x3e, 0x31, 0x32, 0x33, 0x3f, 0x61, 0x61, 0x61,
      0x61, 0x61, 0x61, 0xc6, 0x63, 0x65, 0x65, 0x65, 0x65, 0x69, 0x69, 0x69, 0x69, 0x64, 0x6e,
      0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x2a, 0x6f, 0x75, 0x75, 0x75, 0x75, 0x79, 0xde, 0xdf, 0x61,
      0x61, 0x61, 0x61, 0x61, 0x61, 0xe6, 0x63, 0x65, 0x65, 0x65, 0x65, 0x69, 0x69, 0x69, 0x69,
      0x64, 0x6e, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x2f, 0x6f, 0x75, 0x75, 0x75, 0x75, 0x79, 0xfe,
      0x79,
  };
  unsigned char bVar1;
  char cVar2;
  card_instance_t *instance;
  const char *pcVar4;
  int result;
  unsigned char *pbVar6;
  unsigned int uVar7;
  unsigned char bVar8;
  if (event == EVENT_RESOLVE_SPELL)
  {
    cVar2 = choose_a_consonant_not_nrst(player, player, card);
    make_txtfx(player, card, fx_staff_of_the_letter_magus, CARD_ID_STAFF_OF_THE_LETTER_MAGUS,
               (int)cVar2);
    sp_ability_trigger_2022(2, EVENT_RESOLVE_SPELL, 2, TYPE_ANY, NULL, player, card);
    return 0;
  }
  sp_ability_trigger_2022(2, event, 2, TYPE_ANY, NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  instance = get_card_instance(player, card);
  uVar7 = instance->targets[5].player;
  if ((short)(uVar7 >> 0x10) != 0x46b5)
  {
    return 0;
  }
  uVar7 = uVar7 & 0xffff;
  if (0x18 < uVar7 - 0x42)
  {
    return 0;
  }
  pcVar4 = strchr("AEIOUNRST", uVar7);
  if (pcVar4 != NULL)
  {
    return 0;
  }
  cVar2 = (char)uVar7 + ' ';
  result = get_card_name(trigger_cause_controller, trigger_cause);
  if (result == 0x45a5)
  {
    uVar7 = 0x4f;
    pbVar6 =
        (unsigned char *)"Our Market Research Shows That Players Like Really Long Card Names So We "
                         "Made this Card to Have the Absolute Longest Card Name Ever Elemental";
  }
  else
  {
    if (result == 0x45d5)
    {
      bVar8 = 0x68;
      pbVar6 = (unsigned char *)"The Ultimate Nightmare of Wizards of the Coast Customer Service";
      uVar7 = 0x54;
      goto LAB_025c8d64;
    }
    pbVar6 = (unsigned char *)get_card_name_by_id(result);
    uVar7 = (unsigned int)*pbVar6;
    if (*pbVar6 == 0)
    {
      return 0;
    }
  }
  bVar8 = pbVar6[1];
LAB_025c8d64:
  result = 0;
  while (true)
  {
    bVar1 = native_name_fold[uVar7];
    if (bVar1 == 0xdf)
    {
      if (cVar2 == 's')
      {
        result = result + 2;
      }
    }
    else if ((bVar1 & 0xdf) == 0xde)
    {
      if ((cVar2 == 't') || (cVar2 == 'h'))
      {
        result = result + 1;
      }
    }
    else if ((unsigned int)bVar1 == (int)cVar2)
    {
      result = result + (unsigned int)((unsigned char)(bVar1 + 0x9f) < 0x1a);
    }
    if (bVar8 == 0)
      break;
    uVar7 = (unsigned int)bVar8;
    bVar8 = pbVar6[2];
    pbVar6 = pbVar6 + 1;
  }
  if (0 < result)
  {
    gain_life_2022(player, result);
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x25c6880; original cards/unstable.c:939. */
int card_capital_offense(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  const char *pcVar3;
  char current_char;
  target_definition_2022_t td;
  char rules[3000];
  if (event != EVENT_RESOLVE_SPELL)
  {
    result = vanilla_instant_pump_2022(player, card, event, 2, 1 - player, -1, -1, 0, 0);
    return result;
  }
  default_target_definition_2022(player, card, &td, 2);
  result = valid_target_2022(&td);
  if (result != 0)
  {
    instance = get_card_instance(player, card);
    get_rules_text(rules, instance->targets[0].player, instance->targets[0].card, 1);
    current_char = rules[0];
    if (current_char != '\0')
    {
      result = 0;
      pcVar3 = rules;
      do
      {
        result = result + (unsigned int)((unsigned char)(current_char + 0xbfU) < 0x1a);
        pcVar3 = pcVar3 + 1;
        current_char = *pcVar3;
      } while (current_char != '\0');
      if (0 < result)
      {
        pump_until_eot_2022(player, card, instance->targets[0].player, instance->targets[0].card,
                            -result, -result);
      }
    }
  }
  kill_card(player, card, KILL_DESTROY);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x25c6aa0; original cards/unstable.c:1038. */
int card_old_fashioned_vampire(int player, int card, event_t event)
{
  bool bVar1;
  unsigned int uVar2;
  int result;
  unsigned int uVar4;
  if (event - EVENT_POWER < 3)
  {
    uVar2 = localtime_hour_minute();
    uVar4 = uVar2 >> 0x10;
    if (uVar4 == 5)
    {
      bVar1 = (uVar2 & 0xffff) < 0x1e;
    }
    else if (uVar4 == 0x12)
    {
      bVar1 = 0x1d < (uVar2 & 0xffff);
    }
    else
    {
      bVar1 = 0xd < uVar4 - 5;
    }
    if (bVar1)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        if (event == EVENT_ABILITIES)
        {
          deathtouch_2022(player, card, EVENT_ABILITIES);
        }
        else
        {
          event_result = event_result + 2;
        }
      }
    }
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x25c8380; original cards/unstable.c:1972. */
int card_hot_fix(int player, int card, event_t event)
{
  if (event != EVENT_RESOLVE_SPELL)
    return basic_spell(player, card, event);
  int32_t started;
  uint32_t needs_shuffle;
  if (ai_is_speculating == 1)
  {
    started = 0;
    goto rearrange;
  }
  if (player == 1 && (trace_mode & 2) == 0)
  {
    rearrange_top_x(1, 1, 500);
    started = 0;
    if (ai_is_speculating != 1)
      goto check_ai_clock;
    if ((trace_mode & 2) != 0)
      goto receive_result;
    goto finish;
  }
  started = time_in_ms();
rearrange:
  rearrange_top_x(player, player, 500);
  if (ai_is_speculating == 1)
    needs_shuffle = started < -10000;
  else
  {
  check_ai_clock:
    if (player == 1 && (trace_mode & 2) == 0)
      needs_shuffle = started < -10000;
    else
    {
      int32_t ended = time_in_ms();
      needs_shuffle = (int32_t)((uint32_t)ended - (uint32_t)started) > 10000;
    }
  }
  if ((trace_mode & 2) != 0)
  {
    if (player == 1)
      goto receive_result;
    EXE_DWORD(0x78fee8) = needs_shuffle;
    EXE_BYTE(0x78fee4) = 28;
    EXE_FN(int, 0x4a6bc0, int, int)(0, 28);
  }
check_shuffle:
  if (needs_shuffle != 0)
    shuffle(player);
finish:
  kill_card(player, card, KILL_DESTROY);
  return basic_spell(player, card, event);
receive_result:
  EXE_FN(int, 0x4a70c0, int, int)(1, 28);
  needs_shuffle = EXE_DWORD(0x78fee8);
  goto check_shuffle;
}

#endif
