# October 2022 source recovery

Implemented directly in the `cards/` and `functions/` source files, starting
2026-10-17. Latest recovery pass: 2026-10-21.
Missing target set files were created with `#include "manalink.h"`; implementations
already present in the source were retained, with scoped repairs and lifecycle
fragments recorded separately.

A [2020-to-2022 modification pilot](recovery-comparison-pilot.md) now compares
30 shared functions using a saved Ghidra Version Tracking session: 13 confirmed
code/interface changes, 11 compiler/layout candidates, five matching local
instruction streams and one refactor needing worker comparison. This does not
change the recovered-implementation count below. It establishes a path for
finding modifications in existing source alongside recovery of new functions.

A separate [random sample](recovery-random-sample.md) freezes 15 shared card
handlers and 15 helpers before decoding. It finds five card and seven helper
code/API changes, 14 compiler/layout candidates and four matching local
instruction streams. The population-weighted observed rate is about 36% within
the eligible identity frame, with wide sampling uncertainty; it is not an
estimate of remaining implementations or changed gameplay.

## Recovered source

- **4,083 function implementations:** 3,033 functions named `card_*` and 1,050
  helpers, in **139 checked C source units**, including units containing only
  recovered engine/AI fragments.
- The second pass added **340 implementations: 273 `card_*` functions and 67
  helpers**. The first pass contributed 1,066 implementations.
- The third pass added **912 implementations: 713 `card_*` functions and 199
  helpers**, including one manually reconstructed interface adapter.
- The fourth pass added **202 implementations: 170 `card_*` functions and 32
  helpers**, including five focused counter API implementations.
- The fifth pass added **268 implementations: 237 `card_*` functions and 31
  helpers**, including a focused reconstruction of overlapping stack objects.
- The sixth pass added **92 implementations: 83 `card_*` functions and 9
  helpers**, including five focused compatibility/specialization reconstructions.
- The seventh pass added **30 implementations: 23 `card_*` functions and 7
  helpers**, plus the token generator's counter-action fragment. Four compiler
  clones received concrete interfaces; four Command handlers received local enums
  and verified array bounds.
- The eighth pass added **96 implementations: 81 `card_*` functions and 15
  helpers**, including the source-aware counter-removal API and the Study-counter
  legacy constructor. Existing Monarch helper bodies were exposed for callers;
  those linkage changes are not counted as recovered implementations.
- The ninth pass added **36 implementations: 29 `card_*` functions and 7
  helpers**, including the pair-return repeat-effect lookup and the newer
  split-card wrapper. The existing Suspend helper was exposed without counting
  its retained body as a recovered implementation.
- The tenth pass added **35 implementations: 22 `card_*` functions and 13
  helpers**, including four focused reconstructions for Amass, landcycling,
  commander lookup and the all-player color-count clone.
- The eleventh pass added **28 implementations: 17 `card_*` functions and 11
  helpers**, including the historic-spell clone, a private Thopter factory,
  rarity predicates and watermark lookup. Three existing drawing-library APIs
  now have typed import declarations and import-library exports.
- The twelfth pass added **22 implementations: 14 `card_*` functions and eight
  helpers**, including Surveil, the paged chooser and watermark selection.
  A verified variadic `sprintf` import interface also unblocked more callers.
- The thirteenth pass added **75 implementations: 48 `card_*` functions and 27
  helpers**, including the alternate-text keyword pump, five-argument leaves-play
  dispatcher and end-turn helpers. The card-count cache also has five recovered
  reset/AI-copy fragments and three recovered global declarations; these are
  counted separately from function implementations.
- The fourteenth pass added **53 implementations: 37 `card_*` functions and 16
  helpers**, including three incompatible helper APIs recovered under distinct
  names for Cascade, beginning of combat and another permanent leaving play.
- The fifteenth pass added **21 implementations: nine `card_*` functions and 12
  helpers**, including player-aware subtype lookup, out-of-play subtype modifiers
  and subtype comparison by internal card ID. Five reset/AI-copy fragments and
  three global declarations recover the subtype cache separately from functions.
- The sixteenth pass added **47 implementations: 39 `card_*` functions and eight
  helpers**, using verified missing enum values and the query-selector type.
  It also corrected a previously recovered protection wrapper whose private
  worker had been confused with its public entry point.
- The seventeenth pass added **30 implementations: 22 `card_*` functions and eight
  helpers**, including source-aware counter copying/movement, generalized death
  triggers and bounded name-based exile. A cleanup fix also recovers writable
  byte views and const-qualified borrowed target lists correctly.
- The eighteenth pass added **eight `card_*` implementations** through scoped
  integer union-field hints. It also corrected overlapping local storage in
  **60 earlier implementations**, separately from the new-function count.
- The nineteenth pass added **34 implementations: 32 `card_*` functions and two
  helpers**, including choice/count field hints, verified stack-member views,
  the shared token-target predicate and a set-private basic-land predicate.
- The twentieth pass added **ten implementations: four `card_*` functions and
  six helpers**, including the resource-text cache and Party helper. Six private
  global declarations restore their cache storage.
- The twenty-first pass added **38 implementations: 21 `card_*` functions and
  17 helpers**, including the dice worker, wrappers and random/sort dependencies.
  Six global declarations and five reset/AI-copy fragments restore dice-modifier
  storage. Two existing sort callbacks regain their DWARF const qualifiers.
- The twenty-second pass added **17 implementations: ten `card_*` functions and
  seven helpers**, including triggered-ability countering, the Devour selector
  and refreshed text-loader callers. Four accepted functions use verified scoped
  state-mask names; thirteen expressions in nine candidates were prepared.
- The twenty-third pass added **36 implementations: 23 `card_*` functions and
  13 helpers**, including extension lifecycle and the complete eight-function
  information/stored-ID API. Eight declarations and six scoped fragments restore
  storage, creation binding, AI snapshots and the ownership-transfer copy.
- The twenty-fourth pass added **47 implementations: 28 `card_*` functions and
  19 helpers**, using the extension's two keyword words and a five-value keyword
  enum verified against DWARF. It separately corrected three calls whose private
  protection worker shared a debug name with the public wrapper. New handlers
  include Dress Down, Blood Sun, Rebbec, Gor Muldrak, Nevinyrral, Giver of Runes,
  Veil of Summer and Haktos. The extension-backed getter/remover have distinct
  `_2022` names; original accessors remain available. Keyword refresh, remaining
  legacy keyword readers/writers and native/game integration remain unfinished.
- The twenty-fifth pass added **16 implementations: ten `card_*` functions and
  six helpers**, including Heroic Intervention, Shalai, Blacksmith's Skill,
  Dream Trawler and Palladia-Mors. Scoped keyword-mask names are verified against
  each caller's DWARF enums and machine operands. The protection core, its public
  wrapper and two initial-result event dispatchers are recovered with a separate
  six-value 2022 protection-mode enum; the old API and enum retain their values.
- The twenty-sixth pass added **eight implementations: four `card_*` functions
  and four helpers**. It recovers the full card-ID keyword lookup, both versioned
  keyword queries, the specialized Replicate worker, Reconstruct History,
  Angrath's Rampage, Inscription of Insight and Inscription of Ruin. Two previously
  recovered literal card-ID queries now use the matching 2022 API. Packed
  information reads and an array endpoint use verified layouts rather than
  depending on C local placement.
- The twenty-seventh pass added **14 implementations: two `card_*` functions
  and twelve helpers**. All thirteen extension-backed flag APIs are recovered,
  including replacement of one earlier compatibility adaptation. **218 flag
  calls in 166 recovered functions** now use verified 2022 callees or an explicit
  inline-read proof. Garth One-Eye and Serra's Emissary are recovered with two
  private writable tables and verified native table-copy operands.
- The twenty-eighth pass added **51 implementations: 26 `card_*` functions
  and 25 helpers**. It recovers the mana-count pass, seven tapped-mana cores
  and seven public front ends, plus their newly available callers. Twelve
  declarations and four AI snapshot fragments recover the shared mana modifier
  tables separately. Thirteen mana calls in eleven earlier recovered functions
  now use verified 2022 APIs.
- The twenty-ninth pass added **13 implementations: eleven `card_*` functions
  and two helpers**. The versioned mana-color chooser retains the native dialog
  tail call, while a private compiler-part helper unlocks Naked Singularity and
  Reality Twist. Six declarations and six reset/AI fragments recover per-player
  card and counterability flags. Two existing life-upkeep callback bodies were
  exposed without counting them as new implementations.
- The thirtieth pass added **nine implementations: seven `card_*` functions
  and two helpers**. Independent game-mode storage and three verified pregame
  fragments unlock the counterspell precheck and six callers. Yorion and its
  private ownership predicate are also recovered, including its exact 302-byte
  selection matrix initialization. The existing native singleplayer-mode type
  and import are shared without changing their values, address or storage.
- The thirty-first pass added **16 implementations: seven `card_*` functions
  and nine helpers**. Eight versioned token-core functions preserve the target's
  76-byte definition and sixteen action values, followed by Eternalize and its
  seven card callers. A four-byte instance keyword view preserves the original
  upkeep-byte names; a typed native alias supplies the dynamic token-type pool.
- The thirty-second pass added **ten implementations: three `card_*` functions
  and seven helpers**. Verified token/test/target union views recover Varina,
  Urza's Saga and Kiora Bests the Sea God, with Urza's printed-cost predicate.
  Six token ID/copy entry-point helpers preserve four public APIs, two private
  compiler parts and the newer integer result from single-token creation.
- The thirty-third pass added **13 implementations: one `card_*` function
  and twelve helpers**. Three extension subtype mutators unlock subtype-removal
  wrappers, Aeve, Progenitor Ooze and callbacks for Figure of Destiny, Warden of
  the First Tree and Ageless Sentinels. Their native array boundary reads and
  unsigned creature-type range check are preserved explicitly.
- The thirty-fourth pass added **seven helpers** and corrected one earlier
  target initializer recovery. A separate 84-byte target definition preserves
  all twenty legacy fields and adds the native flags word. Six initializers
  and the public/private protection getter pair retain their native calls;
  the older 80-byte targeting APIs remain available.
- The thirty-fifth pass added **fourteen helpers**: four private target
  validation/availability workers and ten public entry points using the 2022
  layout. Nine compiler-inlined string appends now use verified typed buffer
  operations. The earlier protection dispatcher also calls the recovered
  private getter at its confirmed native destination.
- The thirty-sixth pass added **seventeen helpers**: two private selection
  workers and fifteen public selection/picking APIs. A shared 480-byte storage
  view preserves the sixty-entry AI candidate array and overlapping human error
  buffers. The variable-size multi-target prompt and its native calls are
  reconstructed, and the existing `choose_player` body now uses the full
  84-byte API without changing its public signature.
- The thirty-seventh pass added **eleven helpers** and corrected **three
  earlier Aura wrappers**. Three private compiler parts replace erroneous
  self-call bindings. Eight test constructors/scorers/matchers use a separate
  216-byte definition with the native special-keyword fields and shifted later
  members. Newly screened callers use the verified test/target APIs; callers
  needing older consumer signatures or stack-layout migration remain deferred.
- The thirty-eighth pass added **eighteen helpers** and corrected **six existing
  implementations**. Twelve test consumers cover zone/graveyard selection and
  tutoring restrictions; three counterspell helpers use the full target layout.
  Three private compiler parts repair five callers. The native to-array wrapper
  is migrated without counting it again, with a separate legacy compatibility
  body preserving its older test interface.
- The thirty-ninth pass added **thirty-six sacrifice helpers and one card**.
  Six private workers/callbacks and thirty public helpers preserve native cost
  restrictions, optional selection, marking/cleanup and result reporting. A
  separate flag enum retains the new optional bit and shifted target-reporting
  bit. This unblocks Plumb the Forbidden's optional creature-sacrifice cost.
- The fortieth pass added **seventy-nine implementations: seventy-three card
  handlers and six helpers**. The reconstructed generic-spell core and native
  deck-display frontend unblock the Aura casting worker, three Aura frontends
  and their dependent card handlers. Native spell flags and reused stack words
  have verified source representations alongside the retained legacy domain.
- The forty-first pass added **twenty-two helpers**, repaired two compiler-part
  wrappers and the native Aura attachment binding, and migrated thirty earlier
  scalar callers. Highest-power sacrifice, ninjutsu and eighteen Aura helpers
  now have distinct native implementations. No additional card handlers are
  counted in this pass.
- The forty-second pass added **twenty-five implementations: one card handler
  and twenty-four helpers**. Native keyword writers and private compiler parts,
  lure/flanking helpers, the 48-byte pump constructor and pump/animation Auras
  unblock Talruum Piper and its ability-filtering blocker helper. The attached
  keyword helper and 144 earlier native callers now use the recovered writers.
- The forty-third pass added **twenty-nine helpers** for native pump effects,
  repeat reuse, mass pumps, subtype/color and instant-pump frontends, cleanup,
  humiliation and Spirit Link. Two earlier native implementations and three
  subtype bindings were upgraded; another 276 earlier functions now call native
  pump/cleanup helpers. This pass adds no card handlers.
- The forty-fourth pass added **nineteen implementations: four card handlers
  and fifteen helpers**. The granted/generic activation core, cost/mana helpers,
  permission queries, attachment/equipment wrappers, creature-pump frontend and
  shade amount helper unblock Uvilda, Dizzying Gaze, Barbed Field and Archery
  Training, Uvilda's callback and a mana-cost helper. Eighty earlier native
  callers now use verified permission/cost callees.
- The forty-fifth pass added **seven implementations: three card handlers
  and four helpers**. Specialized sacrifice, graveyard and tap-subtype activation
  costs and the multi-graveyard selector unblock Pestilent Cauldron, Erebos's
  Intervention and Fortitude. Two dynamic integer arrays and overlapping scratch
  storage have native evidence; the sacrifice report-count bit gains its name.
- The forty-sixth pass added **fourteen implementations: five card handlers
  and nine helpers**. The attacker storage/resolution engine and its test and
  self-pump frontends, plus Support, Battle Cry, Renown and Retrace, unblock
  Captured by Lagacs, Generous Patron, Throes of Chaos, Constable of the Realm
  and Garbage Elemental's Battle Cry variant. The existing native attack
  dispatcher and eleven earlier native callers now use the recovered helpers.
- The forty-seventh pass added **eight implementations: three card handlers
  and five helpers**. Native self/global entry-trigger dispatch, Evolve and
  Exploit's private/public pair unblock Gluttonous Slug, Scurry Oak and
  Loathsome Curator. Another 266 earlier recovered functions now use the native
  entry-trigger helpers, with exact call-target and body-preservation evidence.
- The forty-eighth pass added **seven implementations: three card handlers
  and four helpers**. Correct physical signatures for the Level Up and Buddy
  private compiler parts and their public frontends unblock Hexdrinker,
  Sadistic Skymarcher and Daring Buccaneer. Both parts retain private linkage;
  the original public/legacy interfaces remain available.
- The forty-ninth pass added **thirty-four implementations: fourteen card
  handlers and twenty helpers** after exporting 61 missing dependency entries.
  Damage/target predicates, temporary-effect callbacks and a deflection helper
  unblock Doom Foretold, Liliana's Contract, Mizzium Meddler, Trickbind, Dovin,
  Hand of Control and nine other handlers. Nineteen helpers retain private
  linkage and their original source ownership.
- The fiftieth pass added **five implementations: three card handlers and
  two helpers**. The native protection-Aura recursion guard and its initialized
  static target pair unblock Pledge of Loyalty, Cho-Manno's Blessing, Ward of
  Lights and their shared Ward frontend. One eight-byte static global restores
  the guard state in the original Aura source.
- The fifty-first pass added **seven implementations: K'rrik, Son of Yawgmoth
  and six Phyrexian-cost helpers**. Two globals and three scoped mana-engine
  blocks restore the life-payment path. Two existing native spell-trigger
  entries now use 216-byte tests; their earlier 208-byte source bodies remain
  as compatibility fragments and do not add to the implementation count.
- The fifty-second pass added **fifteen implementations: four card handlers
  and eleven helpers**. Native counter proliferation, legend-rule suppression,
  delayed return and the death-trigger counting/resolution core unlock Maulfist
  Revolutionary, Skyship Plunderer, Ilharg and Miara. Four helpers retain private
  linkage; 96 earlier recovered callers gain verified scalar API bindings.
- The fifty-third pass added **twelve implementations: four card handlers
  and eight helpers**. Native action decoding/application, mass manipulation,
  random permanent choice and copy helpers unblock Last One Standing, Sakashima
  of a Thousand Faces, Sakashima's Will and Pretender's Claim. The action worker
  includes both hot/cold ranges and retains an unsigned 32-bit action domain.
- The fifty-fourth pass added **three public helpers**: native global tutoring,
  its scalar wrapper and control transfer. The tutor uses a quantity-sized VLA
  and distinct 2022 destination values; earlier source interfaces are retained.
- 21 previously missing C files were created. They include Strixhaven, Modern
  Horizons 1/2, Commander Legends, Throne of Eldraine, War of the Spark,
  Zendikar Rising, Commander 2020, and `functions/draft.c`.
- 520 recovered functions retain private/static linkage, including nine private
  implementation helpers whose names begin with `card_`. There are 539 recovered
  public helpers; shared declarations live in the existing `manalink.h`.
- Every function has its target address and original DWARF source unit/line
  recorded immediately above its implementation.
- Of the recovered symbols, 3,019 are referenced by the target registration
  records; 2,937 serve at least one record marked CODED. The remaining functions
  include callbacks and helper entry points. Counts are functions, not unique
  playable cards or completed mechanics. `cards/stubs.c::card_crab` preserves
  the target's no-op placeholder and is labeled accordingly.

Examples of recovered card handlers include Culling Ritual, Expressive Iteration,
Double Major, Curate, Environmental Sciences and Eureka Moment in
`cards/strixhaven.c`. Recovered shared helpers include `life_total_can_change`,
`how_much_life_can_i_pay`, `can_legally_target`, `choose_player`,
`cast_counter_triggered_ability`, `color_to_protection`, `get_aftermath`, and
`counter_triggered_target_definition_2022`. Other helpers and legacy callbacks live
in their original card-set files.

The second pass includes Pest and Inkling token generation, Afterlife, Escape,
Ephemerate, Hard Evidence, Ragavan, Hydroid Krasis, Witch's Oven and Cauldron
Familiar. Other additions include aura power/toughness helpers, protection,
combat-damage prevention, deflection and graveyard-selection helpers. These
are source recoveries; their mechanics are not yet validated in the game.

The third pass adds Learn, Adventure, the generic Adventurer handler, the
double-faced card mapping tables, back-face land handlers, attack-trigger
handling, attached-object keyword helpers, and additional mana producers.
Examples include Beanstalk Giant, Murderous Rider and Retriever Phoenix.

The fourth pass adds the Saga helper and handlers including History of Benalia,
Phyrexian Scriptures, The Eldest Reborn, The Antiquities War and Time of Ice.
Other additions include Riot, Mentor, The Great Henge, Yorvo, Korvold,
Sword of Hearth and Home, Tourach and Sparring Regimen. Opponent-activated
legacy effects now have their shared creation helper and additional callers.

The fifth pass adds the Modern Horizons 2 evoke helper and handlers for Endurance,
Solitude, Fury and Grief. Other additions include Explore, multicascade,
Codie, Valentin/Lisette, Rowan, Gilded Goose, Trail of Crumbs and the Eldraine
Castle cycle. More legacy callbacks and older card handlers were recovered too.

The sixth pass recovers the spell-trigger and Magecraft core plus handlers for
Professor Onyx, Archmage Emeritus, Sedgemoor Witch, Storm-Kiln Artist,
Witherbloom Apprentice, Clever Lumimancer and other Magecraft cards. The Spirit
specialization also unlocks Illuminate History, Quintorius and Spirit Summoning.

The seventh pass adds the Fractal and Elemental token factories, Body of Research,
Fractal Summoning, Mascot Exhibition, Serpentine Curve, the Adapt helper,
Pteramander, Growth-Chamber Guardian, Crackling Drake and Ral, Izzet Viceroy.
Lorehold, Prismari, Silverquill and Witherbloom Command are also recovered.

The eighth pass adds Encore and its 15 card handlers, Kianne/Imbraham, Kasmina,
Polukranos, Unchained, Heart of Kiran, Treasure Map, Primal Amulet,
Jace, Mirror Mage and several Monarch cards, including the Commander Legends
Court cycle. Targeting-trigger cards include Thunderbreak Regent, Shield Mare,
Leyline of Combustion and Cephalid Illusionist.

The ninth pass adds the Theros Beyond Death God helper and handlers for Erebos,
Heliod, Klothys, Nylea, Purphoros and Thassa. New Suspend callers include
Gaea's Will, Glimpse of Tomorrow, Profane Tutor, Resurgent Belief and
Sol Talisman. Split-card additions include Expansion/Explosion, Find/Finality,
Response/Resurgence, Consecrate/Consume, Depose/Deploy, Thrash/Threat and
Warrant/Warden. Countering-triggered-ability helpers are also recovered.

The tenth pass adds Amass and handlers including Lazotep Reaver, Angrath,
Enter the God-Eternals and Gleaming Overseer. Commander helpers unlock the
Commander 2018 Storm cycle, Tevesh Szat, Captain Vargus Wrath and additional
legacy effects. The Invasion Djinn cycle, Barrin's Unmaking and Tsabo's Assassin
now have their shared color-count helper. New landcycling callers include
Landscaper Colos, Mental Journey and Orchard Strider.

The eleventh pass adds Jhoira, Weatherlight Captain, Teshar, Ancestor's Apostle,
Traxos, Scourge of Kroog, Artificer's Assistant, Filigree Crawler and Maverick
Thopterist. Border, rarity and watermark helpers also unlock Rare-B-Gone,
Spatula of the Ages, Knight of the Widget, Druid of the Sacred Beaker and
additional Unstable handlers.

The twelfth pass adds Surveil callers including Doom Whisperer, Mission
Briefing, Thought Erasure, Sinister Sabotage, Dakkon, Shadow Slayer,
Dragon's Rage Channeler and Master of Death. Other additions include Ecological
Appreciation, Ineffable Blessing's mana-value variant, Stamp of Approval and
the counter-activated-ability resolution helper with an optional mana payment.

The thirteenth pass adds Panharmonicon, Yarok, the Desecrated, Teysa Karlov,
Scrap Trawler, Soulherder, Colfenor, the Last Yew, Anax, Hardened in the Forge,
Naban, Dean of Iteration and additional Sliver handlers. End-turn additions
include Time Stop, Glorious End, Obeka, Brute Chronologist, Sundial of the
Infinite and Day's Undoing. Alternate-text keyword pumps unlock Silverquill
Pledgemage, Armored Guardian, Shrouded Serpent and Border Guardian.

The fourteenth pass adds the Loyal cycle, Brudiclad, Telchor Engineer, Luminarch
Aspirant, Aurelia, Exemplar of Justice, Valduk, Keeper of the Flame, Ardenn,
Intrepid Archaeologist and Volrath, the Shapestealer. Cascade additions include
The First Sliver, Imoti, Celebrant of Bounty, Bloodbraid Marauder, Meteoric Mace,
Ethersworn Sphinx and several Commander Legends cards. Other additions include
Quicksmith Rebel/Spy, Golden Guardian, Ugin, the Ineffable and additional Bestow
helpers and leaves-play callbacks.

## Method

This pass prioritizes source reconstruction rather than behavioral diffs.
The verified October 2022 runtime payloads were combined with their matching
DWARF/COFF metadata in an analysis-only PE. Every section payload was checked
against its original input. Ghidra imported 200 CUs, about five million DIEs,
19,054 functions and 22,543 data types including derived types.

The executable's memory sections and 260 absolute symbol interfaces were initially
exported for analysis. A later audit separated 196 typed interfaces from 64
name-only function declarations; see the fifth-pass correction below.
Function-entry location expressions identified 516 GCC register
calling conventions; these were applied before the second decompilation pass.
This matters: optimized private helpers can use EAX/EDX/ECX even though their
source declaration is ordinary C. Interpreting those as stack arguments produced
incorrect initial decompilation, which was not promoted.

The initial batch decompiled 4,287 function entries (4,269 distinct DWARF names), selected
using symbol membership and source-unit information. Compiler clones were
excluded from source promotion unless their canonical entry could be identified.
Only C implementations with resolved machine state, supported layouts, available
dependencies and compatible declarations were promoted in this batch. String
and character literals were preserved. The generated raw exports remain under
ignored `.recovery/`; the recovered implementations are in the actual source.

The second pass verified the anonymous `card_instance_t` unions at offsets
`0x38` and `0x4c` directly against DWARF. `defs.h` now exposes `info_slot_fn`
and the four `eot_toughness` bytes alongside the original scalar names. These
aliases preserve the existing 300-byte x86 instance and its field offsets.
The new extension pointer at `0x2c` was not projected onto old storage.

GCC sibling calls needed a separate correction: Ghidra was following terminal
jumps into other functions and emitting their bodies inside small wrappers.
The exporter now bounds each function to its DWARF extent and marks direct,
unconditional jumps to exact external function entry points as `CALL_RETURN`.
Re-exporting 2,857 deferred entries recorded 414 new flow overrides. For example,
`sp_ability_trigger` and `magecraft_trigger` now decompile as calls to their
shared implementation rather than copies of its body. The sixth pass recovers
their shared core using a verified flag-accessor adaptation described below.
One additional private deflection callback was exported from its
own source unit, bringing the cached export set to 4,288 entries/4,270 names.

The screening tool now checks existing source signatures, private linkage and
references to callbacks, and excludes string/character literals from identifier
screening. Three existing helper bodies (`reinforce`, `pump_when_blocked`, and
`real_play_spell_for_free`) were made public to match their 2022 COFF linkage
and support callers in newer sets. Their implementations were retained; this
does not claim recovery of newer behavior inside those pre-existing helpers.
Details are in [the interface evidence](recovery-evidence/interface-recovery.json).

The third pass corrected two overly broad screening rules: actual `in_play`,
`in_hand` and `in_play_and_attacking` calls were being mistaken for unresolved
registers, and `DAT_*` flags were being mistaken for unnamed data addresses.
Parameter/local bindings now prevent an `iid` variable from becoming a false
dependency on the C++ method of that name. Larger resolved lookup functions
were admitted with a 30,000-character screening limit.

Literal recovery is restricted to direct arguments at known string-taking
positions in `dialog_impl` and `new_default_test_definition`. Pointers must
refer to NUL-terminated ASCII text or recognized two-byte dialog controls in
the DLL's read-only `.rdata` section. Ambiguous `.rdata` aliases are rejected;
13 callers had a unique alias address in their instruction operands. Each
accepted replacement records its address and exact bytes in the manifest.
The audit checks all 1,084 recovered literal arguments against the binary.

DWARF verified the `mode` byte at `0x37`, the new Manalink-only event names,
ability-loss masks, keyword zero values and two `events.c` integer globals.
The latter retain their binary initial value of 1. Fixed-width opaque scalar
body values are represented with `uint8_t`/`uint16_t`/`uint32_t`/`uint64_t`;
missing signature types, unsized pointers, aggregates and register state remain
deferred. Two byte expressions were projected onto verified integer globals.

The mode-aware 2022 `is_humiliated` wrapper and worker were reconstructed as
`is_humiliated_for_ability` in `functions/abilities.c`. Recovered three-argument
calls use this name; the existing two-argument function remains in place.
`get_sleighted_hacked_suppressed_ability` also became public to match its 2022
COFF linkage, retaining its existing body.

The fourth pass identifies 16 C runtime functions through both their COFF names
and their indirect jumps to matching `msvcrt.dll` imports. Applying signatures
for `memcpy`, `memset`, allocation and string functions before re-exporting 1,860
deferred entries resolves previously unnamed calls. DWARF also confirms that
Ghidra's generated callback type `int (*)(int, int, event_t)` is `CardFunction`.
The screening tool now distinguishes the legitimate parameter `func_ptr` from
an unresolved `func_0xADDRESS` call. Localized Done/OK labels retain their engine
addresses and use the existing integer final argument of `show_deck`.

Five focused implementations in `functions/counters.c` recover the newer
source-aware counter placement core and its quantity calculation helpers.
Six-argument calls in newly recovered code use `add_counters_from_source`,
preserving both source and recipient. Existing four-argument functions remain
in place. The canonical quantity wrapper and its same-named register-call worker
have distinct source names. Byte-wide counter-slot pointer views were normalized;
the core still retains the target's control flow. `counters_added_type` has its
DWARF type and binary initial value of -1. Fifteen shared enum values used by
this core were compared against compiler-resolved source values and agree.

The target counter replacement, +1/+1 and -1/-1 trigger numbers and enable masks
differ from the legacy engine, with some numeric collisions. Their exact target
values have distinct private `COUNTERS_2022_*` names in the recovered core.
Engine dispatchers and enable-mask writers still require migration before
gameplay integration; these source additions do not complete that migration.

The fifth pass corrects an analysis import error: 64 absolute DWARF subprogram
DIEs provide names without return types, parameter children or a prototype flag.
They had been interpreted as zero-argument `void` functions. Existing source
declarations supply signatures for 52 of them, with every address independently
matched between `manalink.lds` and target COFF. Clang resolves the declarations
and checks their scalar/pointer ABI widths. Ghidra now recovers arguments and
return values for calls such as `has_mana`, `do_dialog` and `dispatch_event`.
The exporter keeps name-only declarations out of the typed-interface table;
12 remain unresolved. Interface application validates the complete input before
mutation, and the decompiler refuses exports after an incomplete application.

The target's `___chkstk_ms` body was checked byte for byte. It preserves general
registers and probes pages; its caller performs the stack allocation. The Ghidra
model accounts for the return-address pop without changing the allocation size.
This helps distinguish compiler machinery from source logic, but variable-length
stack arrays can still need focused reconstruction.

For Wrenn and Six's emblem, DWARF proves that `positions[500]` and `test_hand`
share a stack location in different branches. Ghidra emitted indexed writes
through `test_hand.message`; the recovered C instead declares the separate
500-element integer array and uses it for those accesses. The array size,
element type and shared location are recorded in the manifest. An experiment
removing local type hints ran with `-readOnly`; no exports from that experiment
were promoted, and the persistent array/struct hints were preserved.

The sixth pass traces `card_ext_t` to fixed `card_ext[2][150]` and
`disp_card_ext[2][150]` arrays, with 192-byte elements. The target constructor
binds the instance's `0x2c` pointer to its bank element and copies a verified
extension template. Layout, global addresses and template bytes are recorded
as evidence. The extension is still not projected into the existing instance:
the old eight-byte `mana_to_untap` slot has native readers/writers, and extension
copy/display/save behavior also needs recovery.

The spell-trigger core's only extension access is the copied-spell bit.
`SF2_COPIED_FROM_STACK` is bit 31 in both releases, so the recovered core uses
the existing `get_special_flags2_by_instance` accessor. This is an explicit
source compatibility adaptation: the legacy accessor maps its -1 sentinel to
no flags, while target extension storage has no such sentinel. Related helpers
use the existing flag accessors/storage too. No extension allocation or pointer
fallback was invented. The two-argument callback type used by the counter-entry
helper is exposed as `CardPairFunction`, verified as `int (*)(int, int)`.

The concrete DWARF for the Spirit factory gives its `amount` parameter a constant
value of 1. Its actual interface contains only `player` and `card` in EAX/EDX.
The private `make_one_spirit_strixhaven` implementation preserves that fixed base
quantity; it does not invent an arbitrary-quantity API. Correcting the analysis
signature and re-exporting six entries removes bogus third arguments in its
callers.

The seventh pass verifies the Fractal factory's concrete parameter order:
`player`, `card`, and counter quantity are in EAX/EDX/ECX; token quantity is
constant 1 and has no argument storage. `make_one_fractal` preserves this
specialization. The new `TOKEN_ACTION_ADD_COUNTERS` value is `1 << 20` in both
the card and token-generator DWARF units. Its consumer decodes counter type from
the upper 16 bits of `action_argument` and quantity from the lower 16 bits.
Only that action was added to the existing `functions/token_generation.c`:
after `put_into_play`, it nests `hack_silent_counters`, calls the recovered
source-aware counter API, and restores the nesting before blocking processing.
This is a recorded source fragment, not a claim to have recovered or integrated
the entire 2022 token generator. Other new token actions and replacement behavior
remain downstream work.

Three further clones have concrete two-register interfaces. The Elemental
factory has constant quantity 1. The zone-count helper drops its unused player
argument and receives type/zone in EAX/EDX. The Adapt predicate drops its `fx`
argument; its concrete body has no output-pointer write. The latter removed
parameters have `DW_OP_GNU_parameter_ref` descriptions rather than ABI storage.
The recovered names distinguish these clones from generic source interfaces;
existing helper bodies remain in place.

The four recovered Command handlers require function-local `CommandChoices`
enums. Their separate DWARF scopes provide exact names, values and four-byte
sizes, recorded in `recovery-evidence/local-enums.json`. The decompiler used an
adjacent target/token object's address as the endpoint of each choice loop.
Frame offsets prove that object starts eight bytes after `choices[2]`; all eight
comparisons now use `choices + 2`. This preserves the recovered bound without
depending on source compiler stack placement. Quandrix Command remains deferred
because other partial stack values are unresolved.

The eighth pass recovers `remove_counters_from_source`,
`remove_all_counters_from_source`, and their two private workers into
`functions/counters.c`. Existing removal APIs/bodies remain intact. The clear-all
clone takes only the recipient pair in EAX/EDX; three abstract source parameters
have no ABI storage. Counter removal's target definition and token definition
share a DWARF frame location in separate branches. They are reconstructed as
separate C objects: the byte at `target_definition_t.extra + 3` is really
`token_generation_t.qty` at offset 43 in the token branch. Counter enum values
and the source token offset were checked at compile time. The 57 refreshed
entries add four supporting function entries, giving 4,292 cached export rows
and 4,272 original names.

The Study-counter legacy constructor only consumes the low 16 bits of a combined
card-data load. It now reads the existing 16-bit `id` field directly; the unused
upper half is omitted. `COUNTER_STUDY` was verified as 107 in the target debug
data. This unlocks both Dean handlers without projecting extension storage.

Two pre-existing Monarch helpers changed from private to public linkage in the
target COFF symbols. Removing `static` and adding matching header declarations
makes their existing bodies available to newer callers. This does not recover
the target Monarch worker or its new wrapper's Jared Carthalion guard, and does
not validate delivery of the newer Monarch event. Those behavior differences
remain integration work. A separate, narrowly matched `check_state` mask
normalization identifies `0x800000` as the verified `STATE_OUBLIETTED` value;
this also unblocks Encore without treating a state flag as an absolute address.

Seventeen missing target card IDs were recovered from enum DIEs in two CUs.
Their values are introduced only inside the affected functions, with evidence
in `recovery-evidence/local-card-ids.json`; the live global ID table and jump
table are unchanged. Two Eternalize callers still need their shared mechanic.
The prototype scanner now recognizes multiline signatures, including private
Encore. Locals receiving borrowed target lists preserve the existing APIs'
`const target_t*` return type; attempted writes remain compiler errors.

The ninth pass verifies that `event_flags` and `land_can_be_played` are four-byte
integers in DWARF, with executable addresses matching `manalink.lds`. Their
decompiler byte/halfword reads now use unsigned shifts and masks. This recovers
the Theros God helper's second-pass guard without changing flag writers.
`stack_data_t` is four bytes with `generating_event` at offset two, so the load
at `stack_size * 4 + 0x50962e` is the generating-event byte of
`stack_data[stack_size - 1]`. The enclosing positive-size check remains intact.
These views and the source layouts were checked with the 32-bit compiler.

`find_repeat_legacy_effect_alt` returns an eight-byte `target_t` by value, with
player in EAX and card in EDX. Its earlier analysis interface retained only a
pointer-sized value. DWARF fields and exact success/failure instruction sequences
now establish both return values. Ghidra's explicit join storage is
`EDX:4,EAX:4`; the resulting field assignments and a caller's use of `.card`
were checked against those instructions. The two timestamp calls are represented
by the existing `new_timestamp` wrapper, which calls the same executable
addresses with matching arguments. The corrected lookup also unlocks `make_txtfx`.
The pump caller remains deferred because its newer argument struct has additional
members; the pair-return fix alone does not implement those members.

The target split-card worker was already recovered in `cards/invasion.c`.
`generic_split_card_2022` supplies its missing nine-argument public wrapper,
using the existing worker once, while the legacy sixteen-argument API remains.
Refreshed callers remove the arity mismatch and unlock the hybrid split helper
and additional cards. Both split-half event values were verified against DWARF
and the source enum. The cached export set now contains 4,293 rows and 4,273
original names.

`suspend_only` has matching source/target signatures and public target COFF
linkage. Only its existing source linkage and header declaration changed; no
target-body recovery is claimed for that change.

The tenth pass reconstructs Amass's token and targeting objects separately.
Their DWARF scopes share one frame slot: the apparent target-definition writes
at offsets 48/52 are actually `token.action`/`token.action_argument` in the token
branch. They now use the previously recovered `TOKEN_ACTION_ADD_COUNTERS`
consumer. `SUBTYPE_ARMY` is verified as 254 and declared only inside the helper;
the global live subtype/registration tables remain unchanged.

The newer public `basic_landcycling` and `locate_edh_card` entries collide with
older private helpers in other source units. Their full target bodies are
recovered in `cards/modern_horizons_2.c` and `cards/vanguard.c` as
`basic_landcycling_2022` and `locate_edh_card_2022`. Only callers of those target
entries are renamed during analysis. Existing accepted exports and older private
helper bodies are preserved. The two supplemental entries bring the cached
export set to 4,295 rows and 4,275 original names.

`most_common_color` is a private compiler clone with a constant `player=2`
parameter and no machine arguments. It is recovered as
`most_common_color_all_permanents(void)`. DWARF identifies six integer counters;
machine initialization and indexed accesses confirm their bounds. The raw
decompilation incorrectly overwrites `counts[3]` and `counts[5]` during maximum
selection. The verified instruction range `0x2488862..0x24888f0` only updates
registers and has no explicit memory writes. Reconstructed C therefore keeps
the maximum in a separate scalar and leaves the count array intact. The final
comparison preserves all ties, including all five colors on an empty board.
The helper's machine-byte hash and relevant instruction bytes are recorded with
its reconstruction evidence.

The eleventh pass gives the concrete `cast_historic_spell` clone three register
arguments. Its abstract `trigger_mode` parameter has no ABI storage; the body
computes its trigger mode with `duh_mode`. It is recovered as the private
`cast_historic_spell_2022` helper in `cards/dominaria.c`. The private
`make_thopter` in `cards/aether_revolt.c` is a separate body from the existing
same-name Kaladesh helper. Its supplemental export brings the cache to 4,296
rows and 4,276 original names.

`get_rarity`, `get_watermarks` and `get_border_color` already exist in the
drawing library. Their signatures and producer exports were checked against
the repository and their IAT slots against the target DLL. The analysis image
had untyped slots rather than external functions; those slots now reference
typed external functions. No function bodies were invented at IAT addresses.
The existing root import definition and `manalink.h` now expose these APIs,
preserving the source library's const return types.

DWARF and 32-bit layout checks confirm the watermark-table stride of 16 bytes,
`saturation` at offset 15 and rarity/watermark bytes at offsets 1/2. Unstable's
private `const Watermark*` cache is reconstructed from its zero-initialized
`.bss` symbol. The rarity predicate uses unsigned subtraction/comparison to
match the machine's `SUB`/`CMP`/`SETBE` sequence: only rarities 3 and 4 qualify.
The raw decompiler omitted that cast after the narrow member's C integer
promotion, which would have incorrectly accepted lower rarities.
The variable-length choice array in `choose_a_watermark` required a focused
reconstruction, completed in the twelfth pass below.

The twelfth pass reconstructs Surveil's variable-length integer array and its
scoped test object using DWARF and machine instructions. The array capacity is
the initial minimum of library size and requested count; the remaining count
decreases only after selecting a card. Selected cards retain the target's
hand/top-of-library reinsertion sequence before milling, and only the remainder
is reordered. A positive requested count dispatches `EVENT_SURVEIL` even when
the library is empty. The target's test-member offset is 194; the existing C
object and its source consumers use offset 186. The reconstruction uses the
named field with those C APIs, without projecting newer stack offsets onto the
older structure. Broader selection and event-engine integration remain pending.

Surveil's prompt is passed through an outgoing stack slot that obscures the
literal in raw decompilation. Its DWARF call-site expression, exact argument
store, relative call and read-only string bytes are recorded. The provenance
auditor now checks this explicit reconstruction against both code and data.

The C-runtime thunk at `0x2701df0` is COFF `_sprintf`; its `FF25` destination
matches the target's `msvcrt.dll!sprintf` import. Applying its two fixed arguments
and variadic signature refreshed 31 previously unaccepted exports. Candidate
compilation now treats format errors as failures. This rejected a missing
format argument in Dance of the Manse, which remains deferred. Ghidra's fixed
string-copy intrinsic is rendered using the standard C `strncpy` interface.

The paged chooser's writable string pointer is an alias into its local
3,000-byte buffer. Watermark selection keeps the 256-entry index array, scans
indices 0 through 254, and allocates one display pointer per eligible watermark.
The target's AI/speculation path uses `recorded_rand`; its confirmation message
passes the selected watermark's display text as `sprintf`'s third argument.
That argument was absent from raw decompilation and is reconstructed from the
explicit machine store to `[esp+8]`. The private watermark cache remains in
`cards/unstable.c`, shared with the already recovered lookup helper.

The thirteenth pass recovers `pump_ability_until_eot_no_repeat_alt` using the
existing pump initializer's nine common fields. The target wrapper always sets
the appended subtype, color and disabling fields to zero. Its specialized
source reconstruction retains the target's neutral `targets[8]` writes and
immediate ability recalculation, verified against the worker's instructions.
The existing 36-byte pump structure and older callback body remain. This is a
recovery of that wrapper, not the complete newer pump worker or its generalized
subtype/color behavior. The newer worker is retained as a supporting export.

The newer `leaves_play` has five arguments, including suppression and duplication
controls. Its full target body is recovered as `leaves_play_2022`, while the
existing three-argument API/body is preserved. A refreshed group of 49 exports
now calls the correct entry. Its duplication count uses `card_counts[player][5]`.

DWARF identifies `card_counts` as `uint8_t[2][7]`; COFF locates its zero-initialized
`.bss` storage and two private AI snapshots. The cache is defined in the existing
engine source and declared in `manalink.h`. The target reset uses three 32-bit
stores and a final 16-bit store, clearing exactly 14 bytes before recalculation.
Both AI backup/restore pairs copy the same 14 bytes; their source additions use
separate snapshots. Those five small fragments have independent source and raw
export hashes. Their surrounding engine/AI bodies are retained.

The target also serializes this cache among several other new fields in its
supplemental save record. Save-format migration remains deferred; the existing
`BACKUP_AND_SAVE_LIST` is unchanged. The cache recovery does not claim a complete
engine, save-file or trigger-dispatch upgrade. New count-writing card handlers
and their consuming trigger helpers are now available for downstream integration.

`end_the_turn_legacy` has two separate 32-bit OR operations at `0x4ef1a4` and
`0x4ef1a8`. The existing executable bindings and DWARF array type establish these
as `player_bits[0]` and `[1]`; the decompiler's overlapping scalar aliases were
replaced accordingly. Its newer ability-disable writes are retained, with the
broader ability-mask migration still pending. Recovering this callback and the
five-argument leaves-play dependency unlocks `end_the_turn` and its callers.
Eight supplemental entries bring the cache to 4,304 rows and 4,284 original names.

The fourteenth pass recovers `cascade_2022`, `beginning_of_combat_2022` and
`other_leaves_play_2022` with the target's three-, four- and seven-argument
interfaces. The existing four-, five- and five-argument source APIs and their
bodies remain. Cascade delegates to the recovered `multicascade` with a count
of one; the other-leaves-play wrapper delegates to `other_leaves_play_mode`
with mandatory resolution. Fifty-three previously unaccepted entries were
exported again after the analysis names were separated; fifty caller/helper
implementations passed promotion after the three shared APIs were recovered.

The combat helper calls a native executable predicate at `0x434be0`. Its
matching root `Magic.exe` was read from the same October 2022 release archive.
The instructions establish one stack argument, integer 0/1 results and a plain
`RET`, preserving caller stack cleanup. Its call to native `0x434c30` follows
the patched jump through `0x200b42f` to COFF `_can_attack` at `0x26ab730`.
The recovery records the executable hash, instruction bytes and jump route,
and uses the verified `EXE_FN(int, 0x434be0, int)` interface in the source.
The helper retains its target event/flag behavior; full event-engine integration
is still separate. Three supplemental helper entries bring the raw cache to
4,307 rows and 4,287 original names.

The fifteenth pass recovers `get_subtypes_by_csvid_2022` and
`has_subtype_by_id_2022` with their player argument, preserving the old lookup
interfaces and bodies. DWARF and instructions establish a 26-entry target report
buffer, seven natural subtype slots, and a two-player table of sixteen eight-byte
modifier records. Only fifteen records per player are reset and populated. The
record contains a four-byte subtype and a byte-sized add/force mode at offset
four; padding and the last record are preserved. The global has zero-initialized
storage, and both AI backup/restore pairs copy all 256 bytes into separate private
snapshots. Reset and copy fragments were placed in the existing engine/AI bodies.
The newer supplemental save record still needs downstream migration.

The lookup adapters use the first 26 entries of the existing 100-entry source
buffer. The ID predicate deliberately retains the target's repeated modifier
pass and its final scan of only twenty entries. Machine instructions verify
those different bounds and the unsigned creature-subtype range checks.
`shares_creature_subtype_by_iid` retains the target's changeling handling and
two 26-element rows; its two unresolved stack endpoints were replaced using
the verified frame offsets and 104-byte row stride. Battlefield lookup still
uses the existing `get_subtypes` body; full extension-backed subtype handling
remains deferred.

Eighteen dependent implementations passed promotion after the two lookup APIs
and cache were recovered. They include Muxus, Goblin Grandee; Kaheera, the
Orphanguard; Kaalia, Zenith Seeker; Quandrix Cultivator; Armored Skyhunter;
Board the Weatherlight; Weatherlight; Commune with Dinosaurs; and Acclaimed
Contender, plus selection predicates and callbacks. The final comparison helper
brings this pass to 21 functions. Two supplemental lookup entries bring the raw
cache to 4,309 rows and 4,289 original names.

The sixteenth pass checks missing constant names against explicit enumeration
DIEs in each caller's own compilation unit. Forty-seven distinct values were
prepared for 66 function scopes; 39 newly recovered functions use 31 of those
values. Constants without a unique matching declaration remain unresolved.
These aliases live inside their recovered functions, preserving the existing
engine enums and the separate conditional Shandalar values. This recovers target
counter IDs, trigger values, token actions and flags; it does not establish
their downstream dispatcher, display or token-generator support. Forty-three
implementations passed promotion after these declarations were restored.
They include Helm of the Host, Bruvac, the Grandiloquent, Doomed Artisan,
Chitterspitter, Luminous Broodmoth, Athreos, Shroud-Veiled, and the Zendikon helper
with four card handlers.

DWARF also supplies the four-byte `event_query_t` and its five explicit selector
values for Adventure, Companion, plain mana producers, Storm and Gravestorm.
Restoring the type in `defs.h` enables the query dispatcher, the Adventure
predicate and Edgewall Innkeeper. The dispatcher temporarily stores its selector
in `attacking_card`, calls the appropriate card function and restores that global.

An audit of an earlier protection helper found a name collision: public
`get_protection_source` and its private GCC `.part.0` worker were both imported
under the same C name. The wrapper's decompilation consequently appeared to call
itself. The instructions instead pass a signed byte in EAX and two pointers in
EDX/ECX, then jump to the distinct worker. COFF verifies private worker linkage.
The recovered `get_protection_source_impl` follows damage, activation and legacy
sources, and the public wrapper now calls it. Its combined ID/reserved-field load
is represented by the ID field alone because all uses observe only the low
sixteen bits. Both refreshed exports, instruction checks and the correction's
previous raw hash are recorded. This adds one worker implementation and corrects
one already-counted wrapper. Source-prefix verification confirms that this is
the only pre-existing function body changed during the sixteenth pass.

The seventeenth pass separates the newer seven-argument counter copy,
six-argument predoubled placement and eight-argument counter movement interfaces
from the older APIs. The new calls preserve the card responsible for the effect,
the recipient and the counter donor. The copy helper snapshots up to nine
counter types before placement; DWARF verifies its local record as an eight-byte
pair of `counter_t` and `int`. Placement uses the previously recovered source-aware
counter core. Nesting Grounds, Scrounging Bandar, Arcbound Fiend and Steel
Dromedary now have recovered handlers using these interfaces.

The generalized dies-trigger core adds a required kill-code argument and uses
the recovered card-count cache for duplicate triggers. Its new callback is
recovered alongside it under a distinct name. The core reads the exile flag
through the existing flag accessor; the target flag value is verified as
`0x10000000`, without projecting the extension pointer onto old instance storage.
The lowered target-array copy is reconstructed as a 152-byte `memcpy`, using the
verified nineteen eight-byte target entries. The callback retains its player-bit
check and trigger-mode byte. Sacrifice/exile wrappers and Daemogoth Woe-Eater
are recovered through this core. The five-argument `lobotomy_effect_2022` also
retains the target's maximum-card limit, enabling Test of Talents, Unmoored Ego
and Cranial Extraction with its private implementation helper.

Seventeen focused exports supply the changed interfaces and callers, including
six supplemental entries. The raw cache now contains 4,315 rows and 4,295
original names. Six implementations were placed with explicit reconstruction
evidence; eleven more passed ordinary promotion after those helpers were present.

The cleanup tool had incorrectly made every local `char*` read-only in many
functions. It now retains writable pointers for direct byte stores and known
buffer-output calls. It also restores const qualifiers on borrowed lists from
`becomes_target_of_spell` and on addresses of members in borrowed target lists.
The compiler continues to reject writes through those const pointers. These
corrections unlock thirteen more handlers, including Pirs Whim, Kindred Charge,
Saheeli the Gifted, Bonecrusher Giant, Fblthp, Myriad Construct and Cowardice.
Formatting checks confirm code tokens are unchanged in all earlier additions,
and every previous source prefix remains byte-for-byte intact in this pass.

The eighteenth pass resolves `info_slot` as a signed four-byte integer at eight
specific accesses, using Ghidra's union-facet mechanism. DWARF identifies the
original `amt` locals as integers and the attacker lists as unsigned-byte
pointers. Instructions independently verify the count loads at instance offset
`0x38` and byte-indexed attacker reads beginning at `0x84`. The union hints apply
only to these functions; genuine callback uses and the shared union remain.
The refreshed decompilations recover Master Symmetrist, Mila, Tenured Inkcaster,
Anointer of Valor, Keleth, Cavalcade of Calamity, Dreadhorde Invasion and Sarkhan
the Masterless.

Tenured Inkcaster exposed another importer problem: different source locals
sharing a stack offset had been represented by one small C declaration, with
casts to larger structures. Such code could pass syntax checks while an
initializer wrote beyond the declared local. A broader scan identified sixty
already accepted functions with this pattern, including casts between target,
test and token structures. DWARF verifies the shared frame offsets and each
original local type in all sixty cases and in the new Tenured handler.

Those functions now use explicit union members for their verified typed views,
plus byte storage sized to the exact target extent. Calls use the appropriate
member address. This preserves decoded field/byte accesses while allocating
enough space for each overlapping view. Gustha's Scepter also receives its
verified 100-byte prompt buffer instead of formatting into a scalar local.
The changes cover 62 storage objects across 61 functions. Their original and
corrected body hashes, frame offsets and DWARF references live in
[`recovery-evidence/stack-storage.json`](recovery-evidence/stack-storage.json).
The audit checks the corrected hashes and rejects the known cross-type local
cast pattern; promotion now holds new occurrences for storage reconstruction.

This storage repair does not upgrade aggregate APIs. Source checks preserve the
existing 208-byte test, 80-byte target and 72-byte token structures, verifying
the common view offsets against DWARF. The target versions are 216, 84 and 76
bytes respectively. Its test structure inserts special-keyword fields at offsets
140/144, shifting subsequent fields; full support needs the initializer and
selection consumers migrated together. The existing source APIs and named fields
remain the downstream compatibility boundary. Previous source prefixes were
verified unchanged after accounting for only the sixty recorded corrections.

The nineteenth pass extends scoped union hints to fourteen choice/count accesses
in twelve handlers. Three Flailing creature handlers also contain genuine legacy
callback comparisons in the same word. Their distinct accesses are selected as
function pointers, with instruction addresses and comparisons to the exact legacy
entries recorded. Reproducible hint requests now select individual instructions,
so a scalar correction cannot replace the callback interpretation elsewhere.
Nature's Wrath, Blind Seer, Hungering Hydra, Morophon and the three Flailing
creatures are among the recovered handlers. Two bit-preserving damage-word copies
also have direct load/store instruction evidence, enabling Spiteful Sliver.

Thirty-one deferred handlers received verified shared-stack view preparations.
Where a synthetic partial field has exactly one matching member at the same
offset and width, it is replaced by that typed member. Ambiguous views and fields
requiring aggregate API migration remain deferred. Standalone four-byte stores
retain their bit pattern; pointer members keep their source callback types so
the compiler can check assignments. Integer reads preserve the unsigned slice
value. Byte reads of the four-byte `event_result` and `trigger_cause_controller`
globals also have matching DWARF/COFF types and linker addresses.

Nineteen prepared stack handlers passed the remaining screening, contributing
26 verified partial-member replacements. They include Hofri, Geyadrone Dihada,
Ayula, Bramble Sovereign, Hapatra, Tolsimir, Wicked Wolf, Ghired and The Birth of
Meletis. Geyadrone's otherwise missing Corruption-counter constant is recovered
as a function-local value, 163, from its own DWARF unit. Other newly recovered
handlers include Devastating Mastery, Mirror March, Incubation/Incongruity and
Syr Konrad. The normalizer screens code after excluding comments; reconstruction
comments previously caused false unresolved-C++ matches.

The shared token-target predicate is recovered in its original Amonkhet unit as
`target_is_token_too_2022`. Its distinct name avoids collision with older
set-private predicates. The borrowed diagnostic return and local pointer are
const-qualified, preserving the pointer ABI and literal bytes. The basic-land
predicate remains private to Theros Beyond Death, matching its COFF linkage and
four-argument selection-callback interface. These helpers enable Team Pennant,
Ghired and The Birth of Meletis. Five focused predicate/caller exports add two
entries, bringing the raw cache to 4,317 rows and 4,297 original names.

All previous source prefixes were preserved in this pass. The cumulative storage
audit now checks eighty function bodies; the new preparations continue to use
the existing source aggregate APIs and do not complete the engine migrations
described above.

The twentieth pass recovers `functions/load_text.c`: the private resource parser,
main and file-specific lookup functions, and the copy wrapper `load_text_2022`.
The existing native `load_text` interface remains available to older callers.
The parser retains its original 12-byte records, five private zero-initialized
caches, in-place escape decoding, retained raw/line buffers and 250-entry empty
sentinel. Full DWARF pointer chains establish `const char *` keys and
`const char **` returned lines; the earlier ABI exporter omits these qualifiers.
The 500-byte path uses the verified one-buffer native helper at `0x466020`.
Twenty-five CRT import thunks now have verified interfaces, nine added here.

The reconstructed allocator growth uses truncation after multiplication by 1.5,
matching the x87 control-word changes. Two empty-string pointer assignments have
read-only byte checks and exact immediate-MOV evidence, separately from string
arguments. The public fatal formatter is appended to its original
`functions/show_backtrace.c`, using a 2,000-byte buffer and the Microsoft
`_vsnprintf` contract. All five manually reconstructed bodies have audit hashes.
The released code's quirks are preserved: the file-cache miss uses the main
cache's count, and the copy wrapper caps at 250 rows despite the native
`text_lines` declaration containing 249 rows. These need downstream review;
recovery does not silently repair them or add resource cleanup.

A sixth private global restores Zendikar Rising's 64-byte bit-count table.
Its DWARF `int[16]` type and private COFF `.bss` address enable `party_size`,
Veteran Adventurer and Zagras, Thief of Heartbeats. The promotion check now supplies
verified private declarations only to candidates in their owning unit. It also
recognizes identical public signatures repeated in historical source files,
while keeping conflicting signatures ambiguous. Exposing the existing Crucible
of Worlds and Bribery declarations enables Ancient Greenwarden and Inevitable
Betrayal without counting their retained helper bodies as new implementations.
All 131 previous source prefixes remain intact; the existing backtrace source
only gains `stdarg.h` and the appended formatter.

The twenty-first pass reconstructs the private `roll_xdy` worker in its original
`functions/dice.c`, with its file-local presentation enum. DWARF identifies the
1,500-byte prompt buffer and a dynamically bounded `int` array. The apparent
huge array bound in earlier exports is a misread DIE reference. Machine code
confirms allocation of four bytes per adjusted die count, the stack probe and
array alignment. Explicit typed calls restore the outgoing arguments obscured
by this variable-length allocation. Fourteen symbolic string references have
verified read-only bytes and immediate materialization instructions; their
literal order remains identical to the refreshed export.

The worker preserves extra-dice sorting, pre-modifier natural results, per-die
adjustments, total/difference presentation and the final two-die output. The
native random function at `0x44e070` takes player and upper bound and returns
the recorded result; the dialog at `0x471b60` uses the existing six-argument
source binding. The released assertion only checks a positive dice count,
despite mentioning positive sides in its message. Prompt selection uses
`ai_is_speculating != 0`, while most logging/dialog checks compare against one;
highest-dice headings are formatted even during speculation. These details
remain intact. Unsigned word arithmetic preserves x86 wraparound where needed,
and signed formatting arguments preserve the original bit patterns.

`network_rand` and two descending-sort wrappers are recovered in AI and tutor
units. The CRT import table proves the callback-aware `qsort` binding; its two
previously recovered callbacks now have the original `const void *` parameters.
The dice wrappers enable Hydradoodle, Squirrel-Powered Scheme, Chicken Egg,
Inhumaniac, Painiac, Hammer Helper, Season's Beatings and other handlers.

The two eight-byte modifier arrays have verified `.bss` storage, four exact
reset stores and two complete pairs of AI snapshots. Every copied word is
traced from its source load to its destination store. Six global declarations
and five scoped fragments implement those paths. Serialization is recorded as
part of the target's larger supplemental save record; save-format migration
remains deferred. The 130 unaffected previous source prefixes are byte-for-byte
preserved. Removing the new cache declarations/fragments and restoring the two
callback signatures reproduces the other three prior prefixes.

The twenty-second pass distinguishes verified state masks from executable
addresses. Only numeric masks applied directly to the four-byte `state` member
qualify. Each caller's DWARF supplies the selected enumerators and member offset;
the corresponding AND/OR/TEST instructions confirm the masks. Composite names
preserve every bit and operator. The normal unresolved-address screen remains
in place. Four accepted functions use these substitutions, and their complete
body hashes are audited. New helpers include `on_stack`, `is_being_cast` and
protection from colored spells, enabling Sakashima's Protege and God-Eternal Bontu.

Seven refreshed callers now resolve to `load_text_2022`. String formatters pass
`text_lines[0]`, the same native address with the correct character-pointer type.
This enables Blazing Sunsteel, Serra's Hymn and Remedy. The stack's signed card-ID
word is recovered as `stack_data[stack_size - 1].internal_card_id`, with matching
absolute bindings and four-byte record layout. The validation/resolution helpers
for countering triggered abilities enable Tale's End, Diplomatic Escort and
Repudiate/Replicate, retaining the released validation logic.

The private Devour selector regains two grammatical suffix pointers from exact
immediate loads. Its type-description call passes zero; the missing enum spelling
was synthetic decompiler output, so the native zero argument is retained. This
enables the shared Devour wrapper and Caprichrome. Syr Carah, the Bold uses an
explicit read view of the engine's live prefix buffer at `0x738f34`; the recovery
preserves the pointer without inventing literal text or initialization.
All 134 previous C source prefixes remain byte-for-byte intact in this pass.

The extension investigation records three exact record layouts, eight global
storage objects/template/snapshots and seven refreshed lifecycle helpers in
`recovery-evidence/extension-foundation.json`. Machine references identify creation,
display-copy start/end, duel startup, both AI snapshot pairs and serialization.
The 192-byte extension has six eight-byte information records and two fixed
150-card player banks. `inst_info` returns an eight-byte value in EDX:EAX; its
old pointer-shaped decompilation is unsuitable for promotion. Creation installs
the extension pointer, while instance copying preserves the destination's own
extension storage. This is preparation for a coherent migration, not a recovered
implementation count or a completed extension-storage change.

The twenty-third pass implements the extension's C storage and lifecycle in the
original source units. The instance still occupies 300 bytes, with its extension
pointer at `0x2c` and the unused word at `0x30`. A byte view preserves the existing
zero initializer; it is no longer presented as runtime untap-cost storage. The
192-byte extension, its eight-byte information records and four-byte stored-ID
records have every member offset and width checked against DWARF. The complete
signed `CardFromWhere` enum is recovered with the types.

Creation binds each instance to its fixed `card_ext[2][150]` slot and copies the
original extension template. Duel startup binds all live/displayed slots. Display
start temporarily selects live extension records, while display end compares and
copies the 57,600-byte bank, restores displayed-bank pointers and leaves the
existing display critical section. Both AI snapshot pairs copy the complete live
bank. The ownership-transfer fragment uses `memcpy_inst_and_ext`, which copies the
192-byte extension and 300-byte instance while retaining the destination's own
extension pointer. Other legacy shallow copies remain as in the release; they
are not indiscriminately converted. No heap fallback or new ownership policy is
introduced. The matching executable already disables the old untap-cost setup
routine; the retained legacy code is recorded separately from the C migration.

The reconstructed extension initializer compiles to exactly the release's 192
bytes. The existing instance initializer also matches all 300 release bytes after
the pointer-layout change. These are compilation/data-provenance checks, without
linking or running the game. All 131 unaffected previous source prefixes remain
byte-for-byte intact. Removing the recorded declarations/fragments and restoring
initializer braces reproduces the other three prior prefixes. The pre-existing
Windows source is preserved before its appended helpers.

`inst_info` now returns the actual eight-byte value through EDX:EAX. Its lookup
stops at the first empty record and overwrites slot five when full, whereas
`set_inst_info` searches all six entries before an empty slot and can fail. Both
increment functions preserve their two information reads; the 16-bit and 32-bit
increments retain native wraparound. Clearing compacts matching records but only
zeros the final ID. Stored-ID helpers preserve twelve signed 16-bit ID pairs and
their `-1` sentinel. Packed return-word reads are reconstructed from the exact
`id`/`data2` fields; a neighboring damage/counter halfword load also has an explicit
word representation with checked offsets and widths.

Thirty-four refreshed API callers unlock Grist, Lonis, Murktide Regent, Omnath,
Lord Windgrace, Jeska, Dina, Draconic Intervention, Dramatic Finale, Silverquill
Silencer, Laboratory Drudge, Arcane Adaptation, Conspiracy, Nykthos Paragon,
Agadeem's Awakening, Shatterskull Smashing, Skyclave Apparition and other handlers.
At that pass the canonical raw cache contained 4,318 rows and 4,298 original names. Preparation
tools discard stale unaccepted hints after re-exporting while retaining accepted
proofs. Native hook/jump-table integration, complete supplemental serialization
and extension-backed flag/keyword reader/writer migration remain separate work.

These declarations and adapters do not complete engine integration. The newer
ability check expects bitmask writers rather than the legacy nibble counters;
new event constants still need their dispatch pathways; the power/toughness
globals need the newer dispatcher updates. The existing shared `EVENT_CANT_CREW`
value also still differs from the target release. Twenty-two recovered string
arguments use the newer `DLGIMPL_MODE` dialog control (`0x1b`); its parser support
is not present in the existing dialog implementation. Extension storage and its
information API are now recovered, while the remaining native hooks and flag
consumers still require integration.

The twenty-fourth pass projects `sp_keywords` and `sp_keywords2` through the
verified extension pointer, only when the decompiler declares the base as a
`card_instance_t *`. Twenty projected field accesses in ten newly accepted
functions are recorded with their offsets and widths. Other extension fields
remain unresolved. The four-byte `sp_keyword2_t` enum is recovered from concrete
DWARF, and all five values are checked at compile time. These changes unlock
`special_abilities2`, the attached-object wrapper, Squirrellink, trample over
planeswalkers, miscellaneous hexproof and blocking-limit helpers, plus their
card handlers and private protection effects.

The private `misc_protection.part.0` worker receives only player/card in EAX/EDX
and sets bit 0 of the extension's second word. Its event-gated public wrapper
tail-jumps to it. Giving them separate Ghidra identities fixes a spurious recursive
call in the recovered public wrapper and wrong calls in two recovered protection
helpers. Their recorded machine branches and refreshed raw hashes are checked.
The separately named extension-backed getter preserves the target's Vigilance
write and ability-loss bitmask handling; the remover clears the supplied mask.
At that pass, the large card-ID keyword lookup and remaining legacy keyword
migration were deferred. All 136 previously checked source prefixes were preserved,
allowing exactly these three call repairs and their static forward declaration.
The canonical cache now has 4,320 export rows and 4,300 original names.

The twenty-fifth pass names twelve keyword-mask expressions in eleven accepted
functions. Substitution is restricted to known special-keyword argument positions
and the typed first extension keyword word. Each bit agrees with the caller's
DWARF enumerators, a matching immediate operand in the release and compile-time
assertions against the current header. The general address filter remains in
place. These functions and Palladia-Mors's dependent handler add Heroic
Intervention, Hapatra's Mark, Destined/Lead, Shalai, Fire Covenant, Trench Behemoth,
Blacksmith's Skill, Dream Trawler and Lazotep Plating.

The 2022 protection modes are damage=1, aura attachment=2, equipment attachment=3,
blocking=4, targeting=5 and query=6. The old source assigns different values to
targeting, damage and blocking, so these are recovered as
`is_protected_from_mode_2022_t`/`IPF2022_*` with a distinct
`is_protected_from_2022` wrapper. The core reads the second extension keyword
word and selects direct-card or broadcast protection events. Its apparent
`0x05000000` pointer is an integer initial result: DWARF identifies an `int`
local, and machine instructions set the high byte to the mode. Player protection
reads the verified high byte of the native player-bit word. Both event wrappers
retain the existing native context push/pop bindings and independently save and
restore the trigger condition. All 137 prior C source prefixes remain intact.
At that pass the cache contained 4,321 export rows and 4,301 original names. Native
dispatch integration and replacement of the remaining old protection/keyword
consumers were deferred, including the card-ID lookup recovered in the next pass.

The twenty-sixth pass recovers the complete 4,250-instruction card-ID keyword
lookup in `functions/abilities.c`. It has no external calls; seven bounded
read-only jump tables have their bytes, bounds and internal destinations
recorded. Its decompiler output reported unsettled type propagation and exposed
two `CONCAT11` expressions. The corresponding machine operations are `OR BH,1`;
the reconstruction preserves the full accumulator with `| 0x100U`. Fifty-nine
one-bit shifts use unsigned operands to retain x86 bit patterns. The remaining
integer control flow is retained, with a recorded body hash and a focused
whole-source check; the automatic function-length limit remains unchanged.

`get_special_abilities_2022` and `check_for_special_ability_2022` preserve both
paths: player `-1` queries by card ID, and live instances use extension storage.
The original APIs remain available. Two recovered literal card-ID callers,
Cairn Wanderer and the private M13 predicate, now use the 2022 query after their
machine call targets were verified. Other earlier consumers remain pending.

The Replicate specialization has five physical arguments: player/card/generic
cost in EAX/EDX/ECX and blue/red costs on the stack. DWARF makes black/green/white
constant zero; the event has no physical argument storage. The recovered private
worker keeps the original dialog, payment/cancellation loop and byte counter
wraparound at extension offset 159. The old Replicate implementation is retained;
caller migration and engine copy handling remain integration work. Plumb the
Forbidden remains deferred: its `SAC_OPTIONAL=128` flag collides with the old
`SAC_SET_TARGETS` value and requires the newer sacrifice consumer.

Packed information-value reads reconstruct all eight bytes from the verified
`id`, `data2` and `data` fields. This unlocks Inscription of Insight and Ruin.
Abundance remains deferred because its target-definition copies span 84 bytes,
including an unrecovered trailing flags word; promotion now screens that copy
pattern explicitly. Reconstruct History's five-element type-array endpoint is
verified from DWARF stack offsets and the loop's machine instructions, then
expressed as `types + 5`. The callee's concrete enum also establishes the empty
`TYPETEXT_0=0` flag, retaining the source's other flag values. All 137 previous C
prefixes remain intact apart from the two recorded query repairs. The raw cache
now contains 4,323 export rows and 4,303 original names.

The twenty-seventh pass recovers the thirteen flag APIs for the three four-byte
extension words at offsets 180, 184 and 188. The selector getter, per-instance
getters, masked checks, setters and removers retain their native single-card and
all-in-play behavior, including the both-players selector. The APIs have `_2022`
names. The original target-slot APIs remain available. One earlier getter had
been reconstructed as a legacy compatibility adaptation; its recovered identity
now points to the actual extension-backed getter. The compatibility function is
retained separately and is not counted as another recovered implementation.
Four enum-expanded full-word masks are reduced to bitwise complements only
after their DWARF enumerators are verified to cover all 32 bits.

Across 45 source units, 218 calls in 166 previously recovered functions now use
the extension-backed flag APIs. Each has a verified canonical call or tail jump;
the transformed-entry helper's getter has a separate proof for its inline
extension-pointer load and bit-9 test. Arguments and control flow are retained.
Two existing stack-storage reconstructions retain their layouts and receive
updated body hashes. Original 2020 functions and scoped engine fragments are not
migrated. Remaining original consumers, flag refresh and native hook binding
still require integration.

Garth One-Eye's seven card IDs and Serra's Emissary's nine type selectors are
copied from two private writable arrays totaling 64 bytes. Their original DLL
tables are in writable `.data`, so the recovery preserves mutability and the
per-call local copies. DWARF supplies the local dimensions; the binary supplies
the table bytes, source addresses, copy counts and `REP MOVSD` operands. Serra's
native player-bit access is expressed as `player_bits[player]` after verifying
the four-byte indexing and word load/OR/store. All earlier C prefixes are intact
apart from the 218 recorded callee renames and one corrected provenance comment.
The raw cache now contains 4,335 export rows and 4,315 original names.
The Aura core also remains deferred: its target-definition struct is 84 bytes,
with a new `flags` field at offset 80, versus the existing 80-byte definition.
Its initialization and validation consumers need recovery together.

The twenty-eighth pass recovers four live mana modifier tables: added colors,
color overrides, quantity overrides and multi-mana replacements. Their twelve
declarations include both private AI backup sets. All storage begins zeroed;
the mana-count pass supplies the identity and sentinel resets. Four snapshot
fragments each copy the complete 152-byte state, with two 64-byte `REP MOVSD`
blocks and six scalar words independently traced in the binary. The arrays also
appear in supplemental serialization, whose full format migration remains
deferred.

`count_mana_2022` restores the two-row variable-length source list, sized by the
larger active-card count. It preserves modifier reset, per-player production
events, special battlefield/hand/graveyard paths and deferred source dispatch.
Separate native aliases expose the target's 51-entry mana list and 11-entry
spend-as-color list per player; the old 50-entry declaration remains intact.
The corresponding linker addresses and 204-/44-byte row strides are checked.

Seven tapped-mana cores and seven public `_2022` front ends retain the modifier
logic and physical interfaces. The distinct-color core takes five arguments;
its abstract sixth argument is a constant one with no physical argument slot.
Borrowed prompt strings are read-only. The special flag operand `0x800000` is
verified as `SF2_QUARUM_TRENCH_GNOMES` in this call context. Existing recovered
callers receive thirteen proven callee renames across eleven functions in eight
units. The matching executable's jump at `0x472400`, DLL stub at `0x20103e9`
and final target at `0x26a9b30` establish the native mana-count route. Original
2020 callers and rebuilt native hook integration remain pending.

New callers include Nyxbloom Ancient, Damping Sphere, Fabled Passage, Prismatic
Vista, Spire of Industry, Cabal Stronghold, the five Hour of Devastation sacrifice
Deserts and the five Mercadian Masques storage lands. All 137 previous C token
prefixes are preserved after reversing only the documented callee renames and
removing the new storage/snapshot fragments. `defs.h` is byte-identical to the
previous pass. The raw cache contains 4,341 export rows and 4,321 original names.

The twenty-ninth pass restores `choose_mana_all_one_color_2022`. Its raw export
had absorbed the native color dialog and its UI/network paths after a sibling
jump. A verified Ghidra call boundary at the exact native entry `0x4721c0`
recovers the helper's own algorithm without those foreign stack temporaries.
The outgoing five argument slots, epilogue, jump bytes, complete native dialog
bytes and read-only prompt interface are retained as evidence. The earlier
recovered producer now calls this versioned chooser; the original static source
chooser stays intact. School of the Unseen, Archaeological Dig, Nomadic Elf and
Celestial Prism are recovered as callers. Archaeological Dig retains its target
DWARF's unused fourth parameter.

The per-player `card_flags` words and counterability-color bytes occupy eight
and two bytes respectively. Their four private AI buffers preserve both complete
snapshot pairs. Recalculation resets the card flags; untap entry resets both
counterability bytes before begin-turn dispatch. Every scalar copy is traced
with its original two- or four-byte width, totaling ten bytes per snapshot.
Supplemental serialization of both arrays is confirmed; its full format remains
deferred. These globals unlock Masako the Humorless, Archelos, Lagoon Mystic
and Archon of Coronation. The counterspell precheck remained deferred at that
pass pending the separate `other_game_mode` state, recovered below.

The target exports `can_pay_1_life_thunk` and `lose_1_life_thunk` publicly, while
the old source keeps their matching bodies private to Weatherlight. Their
verified interfaces are now shared, retaining the existing bodies exactly and
allowing Dystopia and Decomposition to be recovered. They add no helper-body
count. A separate private `basic_subtype_to_color_part_2022` retains the target's
`.part.0` function. Its two card callers keep the first three basic-land cases
inline, with nine independently verified calls to the remaining partial helper.
Their mana override writes and special flags are preserved.

All 137 prior C token prefixes are retained after reversing the one documented
chooser rename, two linkage changes and six storage/six lifecycle additions.
`defs.h` and `manalink.lds` are byte-identical to the preceding pass. The raw
cache contains 4,344 export rows and 4,324 original names.

The thirtieth pass recovers the four-byte independent game-mode flag word at
`0x27b3728` and all five enum values from DWARF. It remains separate from the
existing native singleplayer mode. Its target producer is pregame: reset outside
solo duel, then OR the EDH bit when the EDH avatar is found in either the deck or
already-drawn cards. These three writes are inserted in the corresponding
existing control-flow locations, with avatar ID, branch operands and complete
native pregame bytes recorded. No producers for other mode bits are invented.

`pre_counterspell` now retains the target's color-based protection, Commander
zone choice, exile/cast replacement, event dispatch, sound and trap paths. Its
callers include Divide by Zero, Fold into Aether, Spelljack, Force of Negation,
Subtlety and Narset's Reversal. It uses the previously recovered flag storage
and extension-backed accessors; original counterspell code remains available.

Yorion's private predicate takes all five physical target-callback arguments.
It compares ownership to the targeting player, rather than using
the older Antiquities predicate with the same name. The existing target
dispatcher already supplies this five-argument interface. DWARF establishes the
`char[2][151]` selection matrix. The native alignment-aware zero fill is
reconstructed as one `memset` of all 302 bytes after checking both two-byte
edge paths, the 300-byte aligned path and `REP STOSD` operands. Literal auditing
replaces only this exact proven raw block before comparing the remaining source
literals. The selection/exile paths and native companion deck-size conditions
are preserved.

The old source's native singleplayer-mode enum and extern declaration were
private to `rules_engine.c`; they now live in shared headers unchanged. Their
four values, four-byte width and native address `0x4e95a8` are verified. All 137
prior C token prefixes are preserved after reversing this declaration move and
removing the new global/three fragments. The linker file is byte-identical to
the previous pass. The raw cache contains 4,346 rows and 4,326 original names.

The thirty-first pass recovers `token_generation_2022_t` as a separate 76-byte
record, retaining the old 72-byte type. Its first 21 member offsets match the
legacy prefix; the added `special_flags3` word is at offset 72. Both callback
signatures are verified in DWARF. The separate `token_action_2022_t` retains all
sixteen native values: adding Equip Any shifted Haste from 8 to 16, Don't Copy
Token Source from 16 to 32 and Convert into Artifact from 32 to 64. Existing
action names and values remain unchanged for original callers.

The recovered versioned component includes default/copy initialization,
dynamic IID resolution, per-instance setup, the guarded special-info part
helper, token generation and characteristic-setting/generic token handlers.
The creation path retains Academy Manufactor, Chatterfang and Divine Visitation
replacement behavior, action dispatch, callbacks, source tracking, counter
actions and trap updates. The concrete quantities table is eight integer words
for four records, proven by its native stores and loop bound; no huge allocation
is inferred from an optimized DWARF bound. The focused core initializes the new
`special_flags3` field but has no explicit read of it; no extra consumer is
invented during reconstruction.

The resolver uses the native sixteen-entry dynamic type pool, with a typed
four-byte view of its start index at `0x7a5380`. A lowered 18-word copy becomes a
72-byte `card_data_t` assignment after checking the native count and copy
instruction. A packed ID/reserved-word load uses the verified two halfwords.
The token keyword mirror at instance offset `0x124` is exposed through an
anonymous union over the four original upkeep bytes. All old byte names,
offsets and the 300-byte instance size remain intact; native load/store operands
and the extension keyword write are preserved.

Eternalize now uses the versioned initializer and generator with the full
definition, allowing Adorned Pouncer, Proven Combatant, Steadfast Sentinel,
Champion of Wits, Dreamstealer, Timeless Dragon and Timeless Witness to be
recovered. Fresh token candidates use the verified new type/actions/APIs.
Varina, Urza's Saga and Kiora Bests the Sea God remained deferred at that pass
pending the overlapping-stack layout update completed below.
Original source/token consumers, callbacks and native/card dispatch bindings
remain subject to downstream migration and gameplay validation.

All 137 earlier C token prefixes and all earlier implementation/fragment body
records remain intact. Two existing citations now point to a separately
preserved exact supporting export; their original hashes remain unchanged when
the canonical generator export is refreshed. The old header definitions remain
intact beneath the new union view, ABI declarations and native alias append.
The raw cache contains 4,353 rows and 4,333 original names.

The thirty-second pass revalidates all three cards' shared stack locations
directly against DWARF. Their explicit union views use the full 76-byte token
type, with 216-byte backing for token/test sharing and 84 bytes for token/target
sharing. The existing older test/target interfaces and their common prefix
fields remain available. Four groups of lowered byte writes become verified
token word stores: Urza's power/toughness zero and special-info value 66, and
Varina's tapped-token action. Each is matched to a four-byte native store at the
corresponding frame/member offset.

Kiora's apparent token `s_player` and `special_flags2` assignments belong to
the target-definition view in chapter three: `allowed_controller` at offset four
and `allow_cancel` at offset 68. Varina's prompt initialization copies exactly
24 bytes from the native literal and clears the remaining 76 bytes of its
100-byte buffer. These scoped block replacements retain separate raw hashes,
instruction bytes and literal/byte-value checks. Urza's callback checks the
printed mana cost using the native packed ID load, excludes lands and cards
without mana costs, and checks each colored requirement. Its existing shared
`has_no_mana_cost` dependency receives a declaration without counting its
retained body as another implementation.

The versioned token ID/copy APIs preserve the public negative-source and count
guards. Native copying has a public wrapper and a private `.part.0` worker;
they have distinct recovered names, with the canonical short tail jump verified
so the wrapper cannot accidentally recurse. The private generation part retains
all five physical arguments. `generate_token_by_id_2022` returns the target's
integer result while the old void interface remains intact. This includes the
native ordinary-token `-1` result and reserved-token return path; quantity
narrowing is also retained. Existing callers and native/card dispatch bindings
still need downstream integration.

All 137 prior C token prefixes, earlier implementation/fragment/global records
and previous stack proofs are unchanged. The only shared-header additions are
one existing helper declaration and four versioned API prototypes. `defs.h` and
`manalink.lds` are byte-identical to the preceding pass. The raw cache contains
4,358 rows and 4,337 original names.

The thirty-third pass recovers the extension-backed add/remove subtype
mutators in `functions/has_creature_type.c`. DWARF and native operands establish
twelve two-byte subtype entries at extension offset eight, followed by the
information array at offset 32. Addition's pointer endpoint is expressed as
`subtype + 12`. Both removal loops retain their original final lookahead at
index twelve through the adjacent `inst_info_data[0].id` member. The compaction
sequence and loop bounds remain intact, with no extra terminator introduced.

Creature-type removal decrements a zero-extended word in a 32-bit register and
uses an unsigned range comparison. Its C expression now makes that unsigned
arithmetic explicit, preserving the native range 1 through `0xfff` and skipping
the empty subtype value zero. The recovered code uses the existing typed
extension pointer at instance offset 44 and retains the affected-card fallback
for a null instance argument.

The three mutators allow the direct and permanent subtype-removal APIs and
their legacy callback to be recovered. Other callers include Aeve's token
legendary-subtype removal and the subtype-changing callbacks for Figure of
Destiny, Warden of the First Tree and Ageless Sentinels. Existing original
subtype queries/writers and engine refresh/dispatch paths still require
downstream migration; this pass does not change their source bodies.

All 137 earlier C token prefixes and prior implementation/fragment/global
records are unchanged. The newly checked Fate Reforged source unit retains its
complete HEAD token prefix. Original header tokens remain beneath five helper
declarations; `defs.h` and `manalink.lds` are byte-identical to the prior pass.
The canonical export cache remains at 4,358 rows and 4,337 original names.

The thirty-fourth pass recovers the targeting initializer component in its
original `functions/targets.c`. The native DWARF record has twenty-one four-byte
members: the earlier twenty fields retain their offsets and `flags` is at 80.
`target_definition_2022_t` therefore occupies 84 bytes, alongside the retained
80-byte `target_definition_t`. Six separately named 2022 initializers use this
new type. Four inlined constructors explicitly zero the final word; the
opponent/planeswalker and triggered-ability constructors call the new default
initializer. Every store and direct call has runtime instruction evidence.

The protection getter's private compiler part and public entry point share a
DWARF name. COFF linkage and the public tail jump establish their identities.
The recovered public getter and default initializer retain their guards before
calling `get_protections_from_part_2022`. That private body follows activation
parents, computes protection masks from type/color and updates the existing
source cache. The new names prevent an accidental self-call or a call into the
older getter. The previously accepted triggered-ability initializer is corrected
to the new type without counting it twice; its old 80-byte compatibility body
and API remain intact, with a separate preservation hash.

Target readers remain pending: validator, availability and selection workers
must all carry the new flags word before Aura or generic-spell callers can be
migrated. A preparatory Ghidra correction removes an abstract constant parameter
with no physical location from the availability wrapper; its twenty-two actual
arguments now align in the focused exports. This analysis change is not counted
as another source implementation. All earlier source bodies and records are
preserved apart from the explicitly corrected initializer record and marker.
The canonical raw cache still has 4,358 rows and 4,337 original names.

The thirty-fifth pass completes the target validator and availability component
as separately named 2022 APIs. Four private functions recover type matching,
target validation, availability/counting and the forced AI fallback. Ten public
functions recover the executable-facing validation/availability adapters,
`can_target_2022`, `target_available_2022` and the arbitrary/numbered target
validation variants. Their raw bodies retain the native flags, preference
save/restore, damage-source/recipient modes, target count and fallback behavior.
The availability specialization has twenty-two physical arguments plus an
abstract constant zero with no argument slot; that constant is supplied only
when it calls the underlying worker. The validator has twenty-three physical
arguments. Existing original APIs and callers remain intact.

DWARF locates the validator's 100-byte formatting buffer immediately before its
200-byte error buffer. The frame expressions, prologue, byte initialization and
native string operations establish their separate identities. Nine lowered
appends are reconstructed with `strcat`, including both optional comma prefixes,
the literal effect suffix, bounded native literal copies and the five-word
self-target error copy. Instruction ranges, copy lengths and literal bytes are
recorded for each reconstruction. The formatter still writes to its own
100-byte buffer, and the final result still omits the leading error comma.
Executable-localized messages retain their original live addresses through
`EXE_STR`; they are not frozen as English literals.

The extra predicate has five integer arguments and a borrowed `const char*`
result, verified from its local DWARF typedef and indirect native call. The
owner bit becomes an explicit boolean. The type matcher preserves a native
four-byte read across the adjacent two-byte card ID/reserved fields using a
typed unsigned composition. Other card ID comparisons retain their two-byte
width. Player protection bits use the existing two-word array, and the native
chooser flag uses the existing executable word at `0x60e9fc`.

The validator calls the versioned protection modes, extension-backed ability
checks and extension flag accessor. The already recovered protection dispatcher
is also bound to its native private getter at `0x265d2e0`, with a forward
declaration and a recorded body-hash update. This is one existing-body correction,
separate from the fourteen new implementations. All other earlier function
records and bodies, source fragments, globals and header definitions remain
unchanged. Target selection/UI workers and Aura/generic-spell migration remain
pending. Twelve focused entries are new to the canonical raw export list,
bringing it to 4,370 rows and 4,349 original names.

The thirty-sixth pass completes the target selection and picking family in
`functions/targets.c`. The two private selector workers retain all twenty-three
physical parameters, including the flags word and cancellation controls.
The public family includes the executable-facing adapter, auto-target chooser,
ordinary/next/arbitrary target pickers, counted and marked multi-target pickers,
player/opponent helpers and the auto-selection adapter. Their prompts retain
the read-only qualification established by DWARF. AI fallback still changes the
preferred controller and applies the native success penalty.

DWARF places the 208-byte validation error, a separate 200-byte multiblocker
error and sixty eight-byte AI candidates at the same frame offset. An explicit
480-byte union provides these mutually exclusive views. Another 200-byte human
message buffer and the temporary target remain separate. Candidate stores,
player candidates, the stop-after-58-cards comparison and selected-target loads
have native operand evidence. The engine's packed selection word retains its
player bit, low card byte and `0x4000` marker through an explicit unsigned-byte
composition. Existing UI globals and localized error strings keep their native
addresses; the prompt destination remains a writable engine buffer.

The `fscanf` thunk is verified as `msvcrt.dll!fscanf` through the PE import table
and its `FF25` instruction. A two-fixed-argument variadic signature recovers all
eight calls and their four exact scanset formats. The auto-target loader reuses
the original two 500-word tables and read-once flag, with unchanged native file
names, scan behavior and controller save/restore paths.

The counted picker uses a variable-length prompt buffer of `strlen(prompt) + 50`
bytes. Its native stack probe, four-byte allocation rounding, initial zero byte,
six formatter arguments and five selector arguments are recorded. The retained
readable source pattern supplies the reconstruction, checked against the native
control flow, format-word stores, resource operands, duplicate-player retry,
target-marker set/clear, Done/Cancel branch and return-count instructions.
The recovery keeps the caller's target count and cancellation behavior; it does
not introduce a new selection limit or file-parser policy.

The earlier `choose_player` recovery has the same four-argument public API, but
now allocates the complete 84-byte definition and calls the versioned constructor,
validator, availability and selector. This is one corrected existing body,
separate from the seventeen added implementations. All other earlier C token
prefixes and function/fragment/global records remain intact; the original
definition/header tokens and linker bindings are preserved. The canonical raw
cache contains 4,387 rows and 4,366 original names. Aura/generic-spell and card
caller migration remains pending.

The thirty-seventh pass traces the Aura casting dependency through `generic_spell`.
That worker also depends on the native 216-byte test object. The target adds
`sp_keyword` at offset 140 and `sp_keyword_flag` at 144; all subsequent existing
members move by eight bytes. The recovered `test_definition_2022_t` retains the
complete native layout and four-integer predicate callback. The older 208-byte
definition and its original APIs remain intact. Compile-time assertions check
every native field and every retained legacy field's original offset/width.

Eight separately named functions recover both constructors, both score functions,
in-play and internal-ID matchers, and their plain-argument wrappers. The new
matchers preserve native special-ability tests, owner checks, callback arguments,
power/toughness handling and all native selection branches. Two optimized
four-byte flag loads discard their upper half before comparison; the C view uses
the actual signed 16-bit flag member. The internal-ID matcher composes the native
four-byte card-ID read from its two adjacent typed 16-bit fields. Native operand
and callback evidence accompanies these scoped reconstructions.

While tracing Aura dependencies, three earlier public wrapper recoveries were
found to call themselves. Their DWARF names had been shared with distinct private
compiler parts. The private power/toughness pump, base-stat setter and disabling
AI worker are now recovered under separate names; COFF linkage, physical
parameters and native calls/tail jumps establish each binding. The public guards
and signatures remain unchanged, and their only body change is the corrected
callee. These three corrections are separate from the eleven new implementations.

The recovery cleaner can now project new candidates onto the verified 216-byte
test and 84-byte target domains with recorded API bindings. Their aliases are
registered with the native stack-layout metadata. Existing accepted stack views
are retained; prepared candidates using the old aggregate names stay deferred
until their storage is explicitly migrated. A fresh screen accepts no additional
card handlers: generic-spell and sacrifice/graveyard consumers still need their
matching native interfaces and bodies. The Aura casting core remains pending;
it has not been connected to incompatible older consumers.

All earlier C token prefixes and prior implementation/fragment/global records
are preserved apart from the three verified wrapper repairs and their private
forward declarations. Original header definitions and declarations remain;
the linker file is byte-identical. Native stack layouts and existing storage
records are unchanged, with only the two verified source aliases added. The
canonical raw cache contains 4,395 rows and 4,374 original names.

The thirty-eighth pass completes a first group of test consumers needed by
`generic_spell`: graveyard counting, zone-value averaging, human/AI zone selection,
library/graveyard/exile/hand dispatch, and the single/either/multiple-graveyard
selection frontends. The larger test object remains explicit throughout these
calls. Tutoring denial preserves the native restriction-card checks, and a
separate private worker preserves the opponent-search penalty. The picker resets
the native deck-display expansion word and retains the live localized Done label.

The AI picker's variable-length integer array is reconstructed from the native
`zone_count * 4` allocation and each outgoing argument store. Its branch structure,
candidate scoring, random selection and mana checks are preserved. Only decompiler
return-address bookkeeping and unused high bytes of boolean spills are removed.
The hand dispatcher's 150-entry array and exact one-past end are checked against
DWARF. These are scoped storage reconstructions, not a rewrite of its AI policy.
Three counterspell APIs now use the 84-byte target definition, including validation
with restored controller preference and the optional payment resolution path.

A wider compiler-part audit found other public names shared by private entries.
The recovered private Whirlpool Rider, escape and Aminatou's Augury legacy parts
now receive five existing callers' native calls. This fixes the recursive Rider,
escape and legacy bindings, and ensures Whirlpool Drake/Warrior call the shared
part rather than re-entering Rider's public event wrapper. The public guards and
signatures are unchanged. Promotion now defers raw self-calls when their native
destination is a different entry with the same DWARF name.

The previously recorded `select_multiple_cards_from_graveyard_to_array` wrapper
had the same problem. Its recovered native identity is now versioned and calls
the newly recovered private part. The legacy 208-byte API gets an explicitly
uncounted copy of that selection/restoration algorithm using the existing legacy
constructors and selection APIs. This avoids record conversion, preserves filter
mutations and accepts an independently allocated result array without adding the
19-target card-instance limit. The native wrapper's migration is one correction,
not a new implementation or a claim that all old callers have been migrated.

All prior source token prefixes and manifest records are retained except the five
callee repairs and the to-array migration/compatibility body. `defs.h`, the linker
file and the stack registry are byte-identical to the prior pass. There is one new
native UI global; the rules-engine sorcery gate reuses its existing definition.
The canonical cache contains 4,408 rows and 4,387 original names. A fresh screen
accepts no more card handlers. The native sacrifice family still blocks generic
spell/Aura migration; the highest-power-sacrifice and ninjutsu compiler parts also
remain to be reconstructed with their matching test/target interfaces.

The thirty-ninth pass recovers all remaining native entries in
`functions/sacrifices.c`: thirty-six helpers supplement the previously recovered
`types_cannot_sac` restriction scanner. The family includes cost/cause checks,
minimum-value and cannon-fodder AI selection, three permanent pickers, the shared
sacrifice worker, marking and plain-argument wrappers, imposed sacrifices, altar
helpers, damage-prevention activation and value reporting. Their constructors,
matchers and selectors use the native test/target layouts. Original legacy APIs
and bodies remain available in the same source unit.

The flag domain changes materially: the native `SAC_OPTIONAL` is 128 and
`SAC_SET_TARGETS` is 256; the older `SAC_SET_TARGETS` remains 128. The separate
`sacrifice_2022_t` exposes all nine verified native values under `SAC2022_*` names.
New screening uses that domain and the recovered public APIs together, so a
request for optional sacrifice does not accidentally invoke old target-reporting
behavior. No legacy flag value or existing caller is silently rewritten.

Concrete DWARF also resolves two optimized signatures. The private cause checker
takes only the recipient player in EAX; its source player parameter exists solely
as a GNU parameter-reference expression. The prompt helper takes prompt/type/
invert in EAX/EDX/ECX; source `maxlen` is the constant 200 without physical storage.
Applying those interfaces before fresh exports removes the phantom caller stack
arguments and separates the private cause checker from its public wrapper.

The main worker preserves its full selection and cleanup control flow, quantity
clamp, optional/all-or-none handling, previous target restoration and native
target-reporting cases. Its integer result, temporary target and 76-byte token
definition share the same frame location; an explicit union represents all three.
The token controller, quantity two and haste-action stores are checked against
the native offsets and instructions. The 151-byte marked clear/output copy is
reconstructed from its exact aligned word loops and byte tails. The packed result
keeps the count in its low word and player/card in its high bytes. A second union
represents the special picker's overlapping 200-byte prompt and 216-byte test.
Both readonly predicates and the separate native test callback pointer retain
their original physical interfaces and storage identity.

The resulting screen adds Plumb the Forbidden, including its 151-creature limit,
optional sacrifice cost and extension-backed replication count. Formatting now
replays the recorded test/target/sacrifice interfaces when checking code-token
identity. No earlier implementation, fragment or global record changes in this
pass; all prior C token prefixes and legacy definition/header tokens are retained.
The linker bindings and stack registry are byte-identical. The canonical cache
contains 4,442 rows and 4,420 original names. Generic-spell's remaining variable
array/partial-register reconstruction and Aura integration are still pending,
alongside the two outstanding compiler parts noted above.

The fortieth pass reconstructs `generic_spell_2022` in `functions/functions.c`.
It connects the recovered test, targeting, graveyard, counterspell and sacrifice
families without passing native records through the older aggregate APIs. Its
sixteen verified spell flags have a separate `GS2022_*` domain: the new
graveyard-from-graveyards flag occupies 8192, shifting multiple-target handling
to 16384 and permanent-sacrifice costs to 32768. The old values remain intact.
The private redirect flag spelling is recovered from its native enum, and every
existing extension-flag value used by the core is checked against that enum.

The native control flow is retained rather than substituting the older readable
implementation. Three four-byte union slots represent successive pointer,
integer and low-byte lifetimes. A 216-byte union represents the test definition,
temporary target definition and two error buffers sharing one DWARF frame
location; targeted-player state has its own two-integer array. Typed target
pointers replace the decompiler's misleading instance-member address arithmetic.
The native retarget base of one, source/new target storage, minimum target count,
old target restoration and base-plus-selected cleanup bound are retained exactly.
These native bounds are not inferred from the older implementation's loop.

The Spellskite target-choice VLA is reconstructed from its four-byte allocation,
post-incremented array stores and complete outgoing argument frame. Its player
placeholder IDs and byte-exact native prompt are retained, with the localized
Done label left live. Two error-format calls also had a variadic string argument
missing from raw C: independent native stack stores establish both the live format
and reason pointers. `show_deck_2022` now resets the native expansion word before
the existing engine frontend, preserving the source's integer-address label ABI.

The resulting promotion adds `aura_impl`, `aura`, `aura_literal_prompt` and
`aura_pref_controller`, using the full target definition and reconstructed spell
core. Seventy-three card handlers follow, including all four recovered Cartouches,
Deep Freeze, Journey to Eternity, Kasmina's Transmutation, Canopy Cover, Skyblade's
Boon, World-Weary and numerous older Auras. These are recovered handler functions;
their cards are not necessarily new to the release.

All earlier C token prefixes and implementation/fragment/global records remain
unchanged. Original definition/header tokens, linker bindings and the stack
registry are preserved. New native batches and aggregate declarations are kept
in the Manalink configuration, and the formatter replays the new spell interface.
Sensitive native spell flags without a recovered consumer remain deferred.
The canonical cache contains 4,444 rows and 4,422 original names. Further targeted,
stealing and animating Aura migration, earlier caller migration and the two
outstanding compiler-part bindings remain.

The forty-first pass closes those two compiler-part bindings. Highest-power
sacrifice now calls its three-register private implementation after the native
sacrifice restriction guard, using the recovered 216-byte battlefield test.
Ninjutsu has its complete private selector, native 84-byte custom-target wrapper
and ordinary frontend. The native unsigned hand-event range is retained. The
old 80-byte custom-target API has a separately labeled compatibility body; it
is not counted as another native implementation.

Eighteen more native helpers in `functions/auras.c` cover targeting, disabling,
attachment, variable power/toughness, control changes, enchantment protection,
player enchantment and the death-return legacy callback. Their declarations
preserve the 84-byte target and 216-byte test domains. The recovered Aura casting
worker now calls `attach_aura_to_target_2022`, with verified native call sites
and extension-backed flags. A direct call in the disabling-Aura frontend binds
to the private AI implementation rather than the similarly named public wrapper.

Thirty earlier recovered native functions now call the corresponding scalar
2022 helpers, including ninjutsu, vanilla/disabling Auras and attachment. Each
migration retains its earlier manifest record and body hash, records the new
body hash, and verifies the direct native call/jump destinations. Original 2020
functions remain unchanged. Aggregate callers and the shared-stack Zareth
handler await coordinated interface migrations. The broad candidate scan added
no further handlers in this pass.

All other earlier function records and C token prefixes, all fragment/global
records, definition tokens, linker bindings and the stack registry are preserved.
The compiler-part audit finds no remaining direct same-name private-entry
misbindings among recovered functions with source self-calls; this is a focused
check rather than a complete call-graph audit. The canonical cache now contains
4,464 rows and 4,442 original names.

Animation and pump helpers are the next Aura dependency group. Fresh exports
show a 48-byte native pump record with subtype, color and disabling fields after
the retained 36-byte prefix. The first-word special-ability writer also needs
its own native implementation and downstream call verification before those
consumers can be accepted. The fresh exports are analysis inputs only.

The forty-second pass reconstructs that dependency group. `special_abilities_2022`
writes the first extension keyword word and retains the native event, controller,
removal and granted-ability behavior. Deathtouch, hexproof, indestructible,
vigilance and attack-if-able each have a two-argument private worker plus their
public event-filtering wrapper. The debug event parameters of the private workers
exist only as abstract parameter references; corrected Ghidra signatures remove
the misleading caller values and uninitialized event expressions. Granted
flanking similarly has four physical private parameters and five public ones.

The recovered nine-argument lure worker preserves the additional blocker-ability
filter, target selection and temporary trigger suppression. Its original
eight-argument implementation remains available. The public filtering helper
`ability_must_block_me` and `card_talruum_piper` are newly accepted source. The
first keyword word, player-bit byte projections and unsigned two-event range
checks have native instruction evidence. The sixteen deferred scalar callers
already have separate manual or shared-storage reconstruction evidence, so their
future migrations must update those records together.

`pump_ability_2022_t` is a separate 48-byte record with all twelve fields checked
against DWARF and compiled offsets. Its constructor initializes the subtype,
color and disabling fields as well as the nine common prefix fields. The legacy
36-byte record remains unchanged. `pump_attached_2022`, `generic_aura_2022` and
four animation workers/frontends use native records, attachment and ability
writers. Native direct calls in the pump helper bind to the separate private
power/toughness workers. `attached_gets_kw` now calls `special_abilities_2022`.

Another 144 already recovered functions use verified scalar native ability/Aura
callees. These are counted as migrations, not additional implementations. Every
changed caller has its earlier record/body hash, new body hash, exact call/jump
destinations and source-preservation checks. All other earlier function records
and C token prefixes, fragments, globals, linker bindings and the stack registry
are unchanged. The canonical cache contains 4,486 rows and 4,457 original names.
The candidate screen now rejects already recovered native addresses explicitly,
including entries whose recovered names differ from their original debug names.

The full pump-effect engine and earlier aggregate callers still need native
record/API migration. The newly recovered Auras preserve the binary's control
flow; no gameplay execution or native DLL linkage has been performed.

The forty-third pass reconstructs the pump-effect engine in
`functions/functions.c`: the plain power/toughness callback and constructor,
full ability-effect callback, 48-byte constructor, repeat lookup consumers,
mass/test-filtered pumps, subtype/color wrappers and vanilla instant-pump
frontends. The earlier neutral-field no-repeat adapter is replaced by the full
native implementation under the same scalar public name. The merged plain-pump
helper also now references the recovered native callback and constructor.
These two replacements are not counted again.

The native constructor serializes subtype and color to `targets[8].player/card`,
then requests an ability recalculation on an in-play recipient. It does not
serialize the record's disabling field. The callback retains expiration,
source/recipient leave-play handling, untap notifications, optional target
removal, subtype/color events and returned effect values. Native instructions
confirm an apparent release bug: the color branch tests `targets[8].card` but
loads its value from `targets[8].player`. The reconstruction preserves that read
and labels it in source. Two 21-word target copies become typed 84-byte record
assignments, with the native copy counts and instructions recorded separately.

Cleanup now uses recovered native ability-removal writers in
`functions/abilities.c`. They set/clear category bits rather than decrementing
the older packed removal counters. Native humiliation, its 216-byte test
frontend and its legacy callback retain keyword clearing, power/toughness,
color and subtype handling. `cards/legends.c` gains the native Spirit Link
frontend and private implementation; its private event parameter is abstract,
while player, card and the life recipient occupy EAX/EDX/ECX. The analysis
signature now reflects those three physical parameters.

Two native subtype frontends in `functions/has_creature_type.c` use the recovered
twelve-slot extension mutators. Two attached-subtype helpers and the animation
worker now call those frontends. Another 276 earlier functions use verified
scalar pump/cleanup callees, with exact machine destinations and before/after
body hashes. Forty-four callers retain earlier manual/storage reconstructions
that require coordinated evidence updates. All other prior implementation
records and C token prefixes, fragment/global records, definition tokens, linker
bindings and the stack registry are preserved. No definition layout changes
were required in this pass. The canonical cache now has 4,514 rows and 4,484
original names.

The candidate scan added no further handlers. Pump frontends that depend on the
full granted activated-ability or attacker-trigger families, the shade amount
helper and earlier aggregate callers remain to be reconstructed or migrated.
Native DLL linkage, lifecycle/dispatch integration and gameplay verification
remain pending.

The forty-fourth pass reconstructs `granted_generic_activated_ability_2022` and
its generic, attachment and equipment frontends. It preserves activation checks,
cost payment, temporary target/sacrifice marking and restoration, saved targets,
counter and life costs, once-per-turn flags and cancellation behavior. Its
216-byte native scratch area is an explicit union containing the 84-byte target,
216-byte test and eight-byte selected pair. The three views share DWARF frame
offset -236. Selected player/card reads use their named pair fields rather than
misleading target-controller field names.

The native permission queries use the newer category masks in the common
300-byte card instance. Activation mana/cost functions retain the native colored
cost overrides, X handling and stack wrapper used while resolving. The existing
eight-word engine payment array at `0x4ef400` gains its debug-verified declaration;
the original linker binding is unchanged. The established activation flag values
are preserved, and two previously unnamed bits gain their native names:
`GAA_DURING_COMBAT` (4096) and `GAA_MANA_ABILITY` (2097152). All native flag values
are checked against the compiled header.

The creature-pump frontend and shade amount helper now use recovered activation
and pump dependencies. The candidate scan adds the four named handlers plus
`uvilda_legacy` and `charge_mana_from_id_with_cost_mod`. Eighty earlier functions
use verified native scalar permission/cost callees or the unchanged card-instance
pointer layout, with exact call/jump destinations and before/after body hashes.
Thirty-one callers retain earlier manual/storage evidence requiring coordinated
updates. Original test/target record APIs remain available.

All other earlier function records and C token prefixes, fragment/global records,
linker bindings and the stack registry are preserved. Aside from the two named
flag bits and new declarations, original definition/header tokens are retained.
At the end of that pass the canonical cache had 4,527 rows and 4,497 original
names, with focused exports prepared for specialized activation costs.

The forty-fifth pass reconstructs the sacrifice, graveyard and tap-subtype cost
wrappers in `functions/functions.c`, and
`select_multiple_cards_from_either_graveyard_2022` in `functions/tutors.c`.
Sacrifice results use a four-byte integer report in `info_slot`; the decompiler's
function-pointer interpretation was incorrect. `SARV_REPORT_COUNT` names the
existing bit 8. Native report overwrites and count handling are retained.

The graveyard wrapper shares a 100-byte text buffer with an 84-byte target
definition at DWARF frame offset -552, represented by an explicit union.
The tap wrapper's `to_tap` array and selector's face-down-ID array have four-byte
integer elements and DWARF bounds derived from their runtime counts. Outgoing
call-frame artifacts are replaced with their native logical arguments, including
the missing count arguments to both selector prompt formats. The multi-graveyard
selector retains packed player/index values, the 19-target limit, three retries,
temporary face-down filtering and the card-minus-1000 cancellation convention.

The tap wrapper preserves the release's fixed target slot 9, cleanup bound from
`number_of_targets`, and uninitialized color-format buffer passed to the inlined
color helper. The helper's unchanged source body, native color-word table,
prompt bytes and relevant instructions are recorded and checked. These native
quirks have not been corrected during recovery.

The scan adds Pestilent Cauldron, Erebos's Intervention and Fortitude to their
original set files. All earlier implementation records and C token prefixes,
source fragments/globals, existing interface evidence, linker bindings and the
stack registry are preserved. The canonical cache now has 4,530 rows and 4,500
original names. Earlier aggregate callers and lifecycle/dispatch/native DLL
integration remain pending.

The forty-sixth pass reconstructs the attacker-trigger family in
`functions/functions.c`: `store_attackers_2022`,
`resolve_declare_attackers_trigger_2022`, both declaration wrappers and
`when_attacks_pump_self_2022`. Native scalar counts occupy `info_slot`,
`targets[1].player` or `targets[3].player`, at offsets 56, 124 and 140. The tracked
attacker list starts at offset 132 and contains up to 64 unsigned bytes.
The decompiler's function-pointer interpretations become typed integer pointers
and byte access. The separate-trigger path retains its native use of the count
as the byte index before decrementing. The native Battalion, two-or-more and
test-quantity gates, attack-player/planeswalker checks and trigger-category
suppression are retained. The original flag values are checked without changing
the definitions.

Support is recovered in `cards/oath_of_the_gatewatch.c`, Battle Cry in
`cards/mirrodin_besieged.c`, Renown in `cards/origins.c` and Retrace in
`cards/eventide.c`. Support's debug-declared temporary contains 152 target pairs
(1,216 bytes), but the native save/restore operations copy exactly 152 bytes.
Both copy extents, the saved one-byte target count, selection flags and prompt
are verified. Its selection loop continues through failed choices as the binary
does. Battle Cry uses the recovered test and pump interfaces; Renown uses the
extension-backed flag API; Retrace uses the native 216-byte test and selector.

The previously recovered `dat_ability` dispatcher now calls the native storage
and resolution helpers, with its remaining body preserved. Eleven earlier
functions gain verified scalar helper bindings; seven manual callers still need
coordinated migrations. The five new handlers are placed in their original set
files. All other earlier implementation records and C token prefixes, source
fragments/globals, definition tokens, interface components, linker bindings and
the stack registry are preserved. The canonical cache has 4,539 rows and 4,509
original names. Focused exports also prepared the remaining Evolve/Exploit and
entry-trigger dependencies. At that point Level Up and the Buddy private
compiler part still needed physical-parameter recovery before promotion.

The forty-seventh pass recovers `etb_ability_2022` and
`etb_ability_global_2022` in `functions/functions.c`. Both retain native trigger
suppression, optional/mandatory resolution and repeated `EVENT_ETB` dispatch.
Their repetition calculations use the existing two-player, seven-byte-row
card-count cache and the previously recovered Harmonic Prodigy helper. The
native subtype branches, cache columns, trigger-cause marking and global
test/not-me filtering are preserved. The test filter uses the 216-byte native
record, with its two-byte `not_me` field at offset 170.

`evolve_2022` is recovered in `cards/gatecrash.c`, retaining the power/toughness
comparisons, AI modifier and native entry-trigger behavior. `exploit_2022` and
its private `exploit_impl_2022` are recovered in `cards/dragons_of_tarkir.c`.
Concrete DWARF locations prove four private parameters: player/card/event in
EAX/EDX/ECX and the resolution mode in a stack word. The public wrapper's native
tail jump, private linkage, native test local, sacrifice flags and helper calls
are checked.

The scan adds Gluttonous Slug, Scurry Oak and Loathsome Curator to their original
set files. Another 266 earlier native functions in 44 source units gain exact
scalar helper bindings; 83 manually reconstructed callers remain deferred for
coordinated evidence updates. All other earlier implementation records, C token
prefixes, fragments/globals, definitions, interface components, linker bindings
and the stack registry are preserved. The canonical cache now contains 4,544
rows and 4,514 original names. Native linkage, remaining caller migrations and
gameplay validation remain pending.

The forty-eighth pass recovers Level Up's private effects worker and public
activation frontend in `cards/rise_of_the_eldrazi.c`, and Buddy's private
reveal/pay-cost worker and public cost modifier in `cards/lorwyn.c`. Concrete
DWARF lists thirteen physical parameters for the Level Up worker. The two base
power/toughness parameters are retained only as `DW_OP_GNU_parameter_ref`
expressions, so they are omitted from the private C interface while the public
21-parameter interface remains intact. Re-exporting with that signature removes
the false uninitialized arguments and gives the level thresholds, stats and
keyword arguments their correct names. Native behavior sets base power/toughness
through `set_base_pt`, combines middle/upper-level keyword masks at the upper
threshold, applies native ability and activation APIs, and preserves AI scoring.

Buddy has five physical private parameters. Its source debug type says `int`,
but its only direct native caller discards the result and explicitly returns
zero; the optimized worker leaves an unusable residue after void calls. The
private C reconstruction therefore uses `void`. The sole call site, jump to the
zero-return path and all executable-section direct call/jump encodings are
checked. The native public interface still returns `int`. Buddy's 100-byte pay
prompt shares the 216-byte test's leading message storage at frame offset -236;
the dialog receives that explicit message view. Variable colorless surcharges,
reveal/payment choices, AI adjustment and cancellation are preserved.

The scan adds Hexdrinker, Sadistic Skymarcher and Daring Buccaneer in their
original set files. No earlier recovered callers needed migration in this group.
All earlier implementation records, C token prefixes, fragments/globals,
definitions, interface components, linker bindings and the stack registry are
preserved. The canonical cache has 4,548 rows and 4,517 original names. Native
linkage and gameplay validation remain pending.

The forty-ninth pass broadens the missing-dependency scan and exports 61
previously uncached canonical entries, including private callbacks whose names
are reused in different source units. Two screening passes recover 32 and two
implementations; a final scan accepts no further candidates from this cache.
The private helpers stay in their original owning files, including independent
`is_already_damaged` predicates in Tempest and Weatherlight. The public
`deflect_away_from_me` helper is recovered in `functions/functions.c`.

New handlers are Varchild's Crusader, Wandering Mage, Heavenly Blademaster,
Kestia, the Cultivator, Shivan Sand-Mage, Silver Wyvern, Mizzium Meddler,
Liliana's Contract, Doom Foretold, Trickbind, Hungry Hungry Heifer, Dovin,
Hand of Control, Fatal Blow and Opportunist. Their dependencies include
damage-to-player/planeswalker filters, Aura/Equipment and enchantment predicates,
counter and target-change filters, ability re-enabling, temporary damage
prevention and deflection.

New native binding evidence records 89 direct calls/jumps and 19 immediate
callback/function-address references to recovered targets. Each reference is
checked against instruction bytes, the target function's identity and its
private source owner where applicable. This supplements the raw-export,
literal and source checks; it is not a complete whole-program call-graph audit.
All earlier function records, original C token prefixes, fragments/globals,
definitions, interface components, linker bindings and the stack registry are
preserved. The canonical cache now has 4,609 rows and 4,552 original names.
Remaining unresolved bodies/interfaces and native/game integration are pending.

The fiftieth pass recovers `this_effect_doesnt_remove_this` in
`functions/auras.c`. Its C++-style decompiler name is a function-local static
variable, not a class: concrete DWARF identifies an eight-byte `target_t` at
`0x2721d6c`, and the DLL initializes both words to -1. The recovered private
`ward_recalculation_2022` pair uses those values; all seven native four-byte
references are checked. The function retains its native card-only recursion
check during static effects, the player/card check during ability calculation,
and both guard resets around target validation.

The helper recalculates abilities with the native regeneration-context bit
`0x8000000` and an 84-byte target definition whose flags word is ORed with 3.
It retains state-based Aura removal when the attached permanent disappears or
fails validation. The scan adds the private `ward_enchantment` frontend in
`cards/unlimited.c`, Pledge of Loyalty in `cards/invasion.c`, Cho-Manno's Blessing
in `cards/mercadian_masques.c` and Ward of Lights in `cards/mirage.c`.
All earlier function records, C token prefixes, fragments/globals, definition
tokens, interface components, linker bindings and the stack registry are
preserved. The canonical cache remains 4,609 rows and 4,552 original names.

The fifty-first pass recovers the six-function Phyrexian-cost family in
`cards/new_phyrexia.c`: private payment, availability, activated availability,
casting payment, activated payment and the casting-cost event wrapper. The
private payer has eight physical parameters, including three register arguments.
Its six-entry color-word table and public zero-initialized payment-mode flag are
recorded with their native linkage, locations, bytes and references. Two earlier
native scalar callers now bind to these recovered payment helpers.

Three scoped blocks in `functions/engine.c::charge_mana` restore K'rrik's
life-for-black-mana selection and settlement. The mode flag prevents a second
K'rrik prompt inside Phyrexian payment. Native behavior is retained: the displayed
minimum is not enforced, cancellation returns before the usual cleanup, and
life settlement is not gated on `spell_fizzled`. The remainder of the original
charger is retained; this is not a full recovery of the native mana engine.

The two existing spell-trigger entries are upgraded under `_2022` names to use
216-byte tests, the 16-bit `not_me` field at offset 170 and raw copied-spell bit
31 in the extension word at offset 184. Their previous 208-byte implementations
remain under their earlier names, with separate hashed compatibility fragments.
These upgrades do not increase the recovered implementation count. Their native
suppression, controller/type filtering, optional copied-spell handling and
Harmonic Prodigy repetition are preserved. The recovered K'rrik handler in
`cards/commander_2019.c` uses the new payment and trigger APIs.

The batch preserves prior implementation records except the two proved scalar
callee bindings and two native spell-trigger upgrades. All original source C
tokens survive outside those bindings and the three recorded engine fragments;
prior interfaces, globals, fragments, definitions, linker bindings and stack
storage evidence remain intact. The canonical export cache now has 4,612 rows
and 4,555 original names. Fresh Wish-selector exports expose a separate C++
chooser calling-convention dependency; Spike remains deferred. Native linkage
and gameplay verification remain pending.

The fifty-second pass recovers `increment_all_counters` in
`functions/counters.c`, `disable_legend_rule` in `functions/functions.c` and
`bounce_at_eot` in `cards/war_of_the_spark.c`. The counter helper enumerates the
native nine counter slots, skips invalid labels, follows activation parents and
preserves the source/recipient arguments of counter addition. For players it
increments existing energy, poison and experience counters using the verified
four-byte player-counter stride and byte offsets 2, 0 and 1. Legend suppression
writes the native `0x8000000` bit to the selected player words. The delayed
return callback preserves its countdown and phased-out check. These helpers
unlock Maulfist Revolutionary, Skyship Plunderer, Ilharg, the Raze-Boar and
Sakashima's private callback.

Miara's private optimized worker has two physical arguments, player and amount,
in EAX/EDX. The apparent `card` argument has only a DWARF parameter-reference
expression and occupies no register or stack word. Applying that concrete
signature corrects both callers in fresh Ghidra exports. The worker retains
its repeated mana/life payment and draw sequence, including the native
`spell_fizzled == 1` check. Its callback and card handler remain private/public,
respectively, in `cards/commander_legends.c`.

Five additional helpers in `functions/functions.c` recover native death-trigger
counting, stored values and resolution under `_2022` names. They retain token,
owner/controller and replacement filtering, the 216-byte test interface,
packed per-player byte counts at `targets[11].player`, previous counts in
`targets[11].card`, and cached death-trigger multipliers from column 5 of the
seven-byte card-count rows. DWARF confirms a three-integer meld-ID array,
including its native indexing; that storage is retained. The earlier 208-byte
counting APIs remain available. This completes the dependencies needed for
Miara, Thorn of the Glade and her callback.

Another 96 earlier recovered functions in 41 source units now call the native
scalar resolver or callback-based counter API, with exact call destinations and
reversible identifier-only changes recorded. The audit verifies each native
body/call and restores the previous source body hash by reversing its recorded
bindings. Thirty-two earlier manual reconstructions remain deferred for a
coordinated migration. No older original source body is changed by this pass.
All earlier fragments, globals, definitions, interface components, linker
bindings and stack records are preserved. The canonical cache now has 4,617 rows
and 4,560 original names. Native linkage and gameplay verification remain
pending.

The fifty-third pass recovers eight native consumers in their original source
units. `functions/manipulate_and_damage_all.c` gains the private action worker
and decoder, single-card action, mass action and the scalar mass-action
frontend. The worker's concrete DWARF has two ranges: 847 primary bytes and a
five-byte cold abort call. Its seven physical parameters use EAX/EDX/ECX plus
four stack words. Correcting that interface removes bogus argument/spill state
from the mass-action export. The cold call is tied to the verified `msvcrt.dll`
`abort` import through its native IAT thunk.

DWARF gives native `actions_t` unsigned four-byte storage (encoding 7), and the
worker uses an unsigned `JA` comparison. The recovered APIs therefore use
`actions_2022_t`, an explicit `uint32_t` alias, while legacy APIs keep their
existing enum. The high-byte parameter mask is checked as a 32-bit bit pattern.
Native action return values, source-aware counter addition/removal, target
marking and Aura ordering are retained. The native `marked[2][100]` and `mc[2]`
arrays keep their actual DWARF bounds; this pass does not resize them.

The already recovered `counter_number_2022` wrapper is exposed for the new
private action decoder. Its body is unchanged, and its original COFF symbol is
public. This linkage change is recorded separately and does not add a recovered
implementation. One earlier native caller, Beledros Witherbloom, now binds to
the recovered mass-action API through a verified identifier-only change.

`cards/battlebond.c` gains native random permanent selection with its 151-entry
integer array. Its network branch retains the packet tag at `0x628c0c` and the
four-byte result word at `0x628c10`. Two zero-argument, void network interfaces
have verified DWARF declarations and direct executable call targets. New
`_2022` linker aliases preserve those signatures while retaining the existing
tentative declarations. No new storage is allocated for these packet fields.

`functions/functions.c` gains native mass shapeshift with its two-by-151-byte
mark array, plus the entry-copy targeting wrapper using an 84-byte target
definition. Together with the action helpers, these APIs unblock Last One
Standing, Sakashima of a Thousand Faces, Sakashima's Will and Pretender's Claim.
All 139 checked source units compile. Prior source tokens survive after
reversing the one scalar callee binding and the documented static-to-public
exposure; prior fragments, globals, interface components and stack records are
preserved. The only definition addition is the unsigned action type, and the
linker appends the two network aliases. The canonical cache has 4,623 rows and
4,566 original names. The fresh global tutor export still contains dynamic
stack-array state and remains deferred for a full reconstruction; native
linkage and gameplay checks remain pending.

The fifty-fourth pass reconstructs `new_global_tutor_2022`,
`global_tutor_2022` and `gain_control_impl_2022` in the original tutor/control
source files. Native stack allocation and DWARF establish that the tutor's
selection array has `test.qty` elements, read from the 16-bit field at offset
174. Separate native tutor domains preserve the destination values shifted by
one from the old source and the new automatic-deck origin. The body retains
Opposition Agent routing, shuffle stages, cancellation results, Cage handling,
ownership changes and the target's unusual hand-ID indexing behavior. Control
transfer preserves timestamp-array updates, target retargeting and the native
tail behavior when allocation fails. These three bodies have native instruction
and replay evidence; earlier implementations and evidence remain unchanged.
All 139 source units and ABI checks pass. The canonical cache now has 4,625 rows
and 4,568 original names. Earlier consumers still need deliberate migration to
the shifted destination values; native linkage and gameplay checks remain pending.

Some code retains decompiler-style temporaries and control flow. Numeric
constants are retained where a reliable symbolic reconstruction has not been
established. These are provisional reconstructed implementations, not claims of
original formatting, original macros or proven behavioral equivalence.

## Checks performed

- The accepted batch passed a combined 32-bit C syntax/interface check against
  the current `manalink.h` and `defs.h`. This also rejected missing dependencies,
  incompatible argument lists, unresolved identifiers and uninitialized locals
  reported by the checker.
- All 139 complete modified/new source units passed Clang's 32-bit C syntax
  checks. `functions/rules_engine.c` needs the checker-only Windows compatibility
  definition `strncasecmp=_strnicmp` for existing configuration parsing code.
  Its game source was not changed for that portability issue.
- Recovered string/character literals were checked against the raw exports.
- All 4,083 recorded raw-export hashes and source provenance markers were checked,
  along with supporting exports, forty source fragments, fifty-two recovered global
  declarations, local enum values, array endpoint replacements and 1,588
  recovered literal arguments, plus two independently verified literal pointer
  stores, sixteen reconstructed literal references and 1,460 manually
  reconstructed/qualified function body hashes.
- Reconstructed stack-storage body hashes were checked for all 88 functions in
  the shared storage registry, and the known cross-type local cast pattern is
  rejected. The selector union and counted-picker VLA are checked separately.
- Compile-time layout checks confirm the 300-byte card instance and recovered
  member offsets, including `mode` at `0x37` and `targets` at `0x74`.
- Compile-time checks also confirm the card-count cache is 14 bytes with a
  seven-byte row stride. The pump specialization checks all nine common member
  offsets and the retained 36-byte source structure.
- The subtype cache checks confirm an eight-byte modifier record, its mode byte
  at offset four, a 128-byte player row and a 256-byte table.
- The query selector type is checked as four bytes.
- The separate 2022 protection-mode type is checked as four bytes with all six
  values verified; the native player-bit array is checked as two four-byte words.
- The resource-cache record is checked as 12 bytes with fields at offsets
  zero, four and eight.
- The dice selector is checked as four bytes and each live modifier array as
  eight bytes.
- Mana checks verify all four modifier table sizes/strides, both native list
  views, event values, the producer flag mask, snapshot copy instructions and
  the executable-to-DLL mana-count jump route.
- Per-player card/counterability checks verify four-byte and one-byte elements,
  full reset/copy ranges, instruction bytes and owning-function hashes. The
  mana chooser's native dialog boundary and all nine partial color-helper
  calls are checked, along with the retained life-upkeep callback bodies.
- Independent and native singleplayer mode widths/values and pregame selectors
  are checked. Yorion's callback slots, matrix dimensions and complete zero-fill
  instruction sequence are verified against DWARF and the runtime bytes.
- Token checks verify both definition sizes, every new member and retained
  prefix offset, all sixteen action values, the preserved legacy action values,
  keyword word/byte views, 72-byte card data and native dynamic-pool binding.
- Token stack checks decode native word-store destinations and values, verify
  the exact prompt copy/clear and validate direct calls plus near/short tail
  jumps for the ID/copy entry points.
- Target initializer checks verify both structure sizes, all twenty-one native
  member offsets/widths, the twenty retained legacy members, flags-zero stores,
  private/public call destinations and the retained compatibility body.
- Target validation checks verify physical argument counts, native local-buffer
  locations, the five-argument predicate, every reconstructed append range,
  literal-copy size/content, live engine-string operands, two-word player bits
  and the adjacent ID-word view. Versioned call destinations and the protection
  getter binding are checked against the native instruction bytes.
- Selection checks verify the 480-byte union and all three shared views, native
  candidate operands/bounds, packed target encoding, live engine operands,
  auto-target cache shapes, variadic import identity and VLA allocation/call/
  cleanup ranges. The player chooser's migration preserves its public signature.
- Test-core checks verify the 208/216-byte layouts, all member offsets and widths,
  the four-argument callback, added-field operands, flag-word and ID-word views,
  and versioned query destinations. Aura compiler-part checks verify private
  ownership, physical arguments and the three corrected public call targets.
- Test-consumer checks replay the recorded VLA call-frame transformations, verify
  native function/call bytes and the hand-array bound, and check the deck expansion
  and existing sorcery-gate words. Five more compiler-part callee bindings, the
  migrated to-array entry and the legacy compatibility body's hash are verified.
- Sacrifice checks verify all nine native flags and the retained old flag value,
  two optimized physical interfaces, both shared stack unions, callback/global
  identity, exact clear/copy ranges, packed result bytes and token member stores.
  Native call bindings and whole-body hashes cover all thirty-six added helpers.
- Generic-spell checks verify its eight-argument physical interface, all sixteen
  native flags and retained legacy values, extension masks, shared scratch/array
  locations, VLA and variadic argument stores, cleanup arithmetic and native
  callees. The two core/display body hashes and their recorded transformations
  are checked; subsequent Aura/card API bindings are recorded per function.
- Extension subtype checks verify the twelve-word array, adjacent two-byte
  information ID, instance pointer offset and exact scan/compaction/unsigned
  range instructions, along with the corresponding C reconstructions.
- The stack card-ID word is checked at offset zero in a four-byte record;
  native text storage is checked as 249 rows of 300 bytes. Selected source state
  masks are checked against the verified target values.
- The stack-storage checks verify the retained source aggregate sizes and common
  target/test/token member offsets used by the reconstructed views.
- Extension checks verify all three record sizes, every member offset/width,
  both 57,600-byte banks and the pointer/unused-word positions. Separate object
  probes verify both the 192-byte and 300-byte initializers against release data.
- Source whitespace was checked; existing bodies were retained apart from
  recorded scoped repairs and lifecycle fragments.

**No tests were added or run, as requested. No native DLL build, game execution,
link verification or gameplay validation was performed.** Syntax success is
not downstream integration or behavior validation. The live jump-table source,
card-data IDs and existing engine ABI have not been upgraded to the complete
2022 release; recovery of implementations is the current scope. Some named event
values changed between releases, and numeric card IDs still refer to the target
release. Syntax compatibility does not resolve those downstream differences.

## What remains

The per-function manifest is
[`recovery-evidence/recovered-functions.json`](recovery-evidence/recovered-functions.json).
It lists accepted functions with source locations, linkage, signatures and raw
export hashes, plus deferred functions with reasons.

The largest remaining group needs resolution of machine state or changed
layouts, particularly target-definition flags and extension-backed keyword/flag
consumers. The extension storage and packed information API now have source.
Others require new types/constants, changed helper signatures, C++ recovery,
or dependencies that were not ready for promotion. The spell/Magecraft core now
has a narrow flag-accessor adaptation; broader flag storage and reader/writer
migration remains. Source compatibility does not validate
copied-spell trigger delivery or complete the ability-mask writer migration.

The latest screening accepted seventy-seven dependent functions after the
generic-spell core was reconstructed.
Continue by resolving those interfaces and layouts, then rerun promotion to
recover their dependent card groups. This does not require a whole-program diff.
The tools preserve source-unit placement and retain unresolved work for the
next pass. Static helpers with repeated names must be resolved in their own
source unit, and optimized split/ranged functions need their full range lists.

Further scalar `info_slot` values are still inferred through the union's
function-pointer member in some choice and damage-count handlers. The attacker
count group is now recovered; the remaining accesses need the same focused
instruction/type verification. Real callback comparisons, such as those in the
Flailing creature handlers, must retain function-pointer interpretation at their
own accesses.

## Recovery tooling and cached analysis

- `tools/recovery/prepare_analysis.py`: builds and verifies the analysis PE.
- `ImportRecoveryDwarf.java`: imports the matching debug types/signatures.
- `export_abi.py`: exports executable sections, absolute interfaces,
  entry-register evidence and the reproducible export-request list.
- `ApplyRecoveryAbi.java`: applies the executable interface/ABI information.
- `export_runtime.py` and `ApplyRecoveryRuntime.java`: verify CRT import thunks
  and apply their signatures before refreshing deferred exports.
- `export_engine_interfaces.py`: matches existing source declarations to target
  absolute addresses, separating untyped names from usable prototypes.
- `ApplyRecoveryStackProbe.java`: verifies and models the target stack probe.
- `recover_stack_aliases.py`: reconstructs the DWARF-proven Wrenn emblem array.
- `recover_spell_trigger_core.py` and `recover_flag_helpers.py`: reconstruct
  focused helpers through the existing flag storage APIs.
- `recover_spirit_specialization.py` and `ApplyRecoverySpiritSpecialization.java`:
  verify the constant quantity and recover the specialized Spirit interface.
- `recover_fractal_specialization.py` and `ApplyRecoveryFractalSpecialization.java`:
  verify the Fractal clone and recover the token counter-action fragment.
- `recover_clone_helpers.py` and `ApplyRecoveryCloneInterfaces.java`: restore
  concrete interfaces for the Elemental, zone-count and Adapt predicate clones.
- `recover_local_enums.py`: restores scoped Command enums and verifies choice
  array endpoints against DWARF frame offsets; `--apply-bounds` repairs the
  newly recovered handlers once.
- `recover_counter_api.py`: reconstructs the focused source-aware counter API
  with explicit compatibility names and a complete-unit compiler check.
- `recover_counter_removal.py` and `ApplyRecoveryCounterRemoval.java`: verify
  removal clone arguments, restore the token stack object, and recover the
  source-aware removal APIs and workers.
- `recover_monarch_interfaces.py`: verifies public COFF linkage/signatures and
  exposes the existing source helpers; also records the phased-state mask.
- `recover_study_helper.py`: reconstructs the masked card-ID load in the Study
  legacy constructor and verifies the counter value.
- `recover_card_id_constants.py`: recovers otherwise missing target ID constants
  into function-local scope, without replacing the live ID table.
- `recover_engine_views.py`: verifies engine-word and stack-record views and
  exposes the existing Suspend helper after checking public target linkage.
- `recover_pair_return.py` and `ApplyRecoveryPairReturn.java`: verify both return
  registers and restore the by-value repeat-effect lookup and its callers.
- `recover_split_helpers.py` and `ApplyRecoverySplitInterfaces.java`: recover
  the nine-argument wrapper for the already recovered private split worker.
- `recover_batch10_helpers.py` and `ApplyRecoveryBatch10Interfaces.java`: verify
  and recover Amass, the two shared helper aliases, and the color-count clone,
  checking the maximum-selection instructions rather than copying its faulty
  aggregate-hinted decompilation.
- `recover_subtype_cache.py`: verifies the modifier layout, zero-initialized
  globals, field-only reset and four complete AI copies before placing fragments.
- `recover_subtype_interfaces.py` and `ApplyRecoverySubtypeInterfaces.java`:
  separate the player-aware lookup APIs and verify their distinct loop bounds.
- `recover_subtype_comparison.py`: restores the two 26-entry array endpoints
  using DWARF and machine frame offsets.
- `recover_target_enums.py`: recovers missing constant declarations from each
  caller's own DWARF unit and keeps the aliases local to that function.
- `recover_query_type.py`: restores the five query selectors and their type.
- `recover_protection_worker.py` and `ApplyRecoveryProtectionWorker.java`:
  distinguish the protection wrapper from its register-call worker, recover
  the worker and repair the earlier wrapper reconstruction.
- `recover_batch17_interfaces.py`, `recover_batch17_wrappers.py` and
  `ApplyRecoveryBatch17Interfaces.java`: verify the changed counter, death-trigger
  and bounded exile interfaces, recover the local counter snapshot and target
  copy, and retain distinct APIs for old callers.
- `ApplyRecoveryScalarInfoSlots.java` and `recover_scalar_info_slots.py`: apply
  per-access scalar union hints and verify original local types, count loads
  and byte-indexed attacker accesses before merging refreshed exports.
- `recover_stack_storage.py`: verifies shared DWARF frame offsets and aggregate
  layouts, repairs undersized local allocations with explicit typed union views,
  and records hashes for subsequent audits.
- `prepare_stack_storage_candidates.py` and `resolve_stack_partials.py`: prepare
  deferred handlers' verified stack views, resolve unique offset/width member
  matches and leave normal dependency/compiler screening in control of promotion.
- `recover_scalar_choices.py`: verifies scalar choices and distinct callback
  comparisons, and regenerates instruction-specific hints for a fresh project.
- `record_scalar_word_views.py`: verifies two engine-word bindings and direct
  four-byte info-slot-to-target copies, with persistent source layout checks.
- `recover_token_predicate.py` and `ApplyRecoveryTokenPredicate.java`: recover
  the public token predicate with a separate name and export the private
  basic-land predicate in its own set unit.
- `recover_text_loader.py`, `ApplyRecoveryTextInterfaces.java` and
  `ApplyRecoveryTextLocals.java`: recover qualified resource-cache interfaces,
  the parser's typed table local, native path binding and fatal formatter;
  verify allocator rounding and Microsoft formatter behavior.
- `recover_party_cache.py`: verifies the set-private 16-entry bit-count cache
  before normal promotion of the Party helper and card callers.
- `recover_dice_helpers.py` and `ApplyRecoveryDiceInterfaces.java`: restore the
  VLA worker's typed calls, private enum/linkage, native random/dialog interfaces
  and CRT sort callback contract, with instruction and literal provenance.
- `recover_dice_caches.py`: verifies six globals, four reset stores and sixteen
  source-to-destination word copies before recovering both AI snapshot pairs.
- `recover_state_masks.py`: prepares per-function mask substitutions with DWARF
  enum/member proofs and matching machine operands, keeping address screening.
- `recover_keyword_masks.py`: prepares special-keyword argument/field masks with
  per-unit enum proofs, matching machine operands and retained address screening.
- `recover_keyword_lookup.py` and `recover_keyword_id_callers.py`: recover the
  complete card-ID lookup and versioned query wrappers, then repair literal-ID
  callers after checking their machine callees.
- `recover_replicate_worker.py`: verifies the specialization's physical argument
  locations and constants, prepares focused Ghidra interfaces and recovers the
  original dialog/payment loop with its extension byte counter.
- `recover_type_text_zero.py` and `recover_reconstruct_bounds.py`: recover the
  empty text flag and the five-element array endpoint from concrete type/stack
  declarations and loop operands.
- `recover_extension_flags.py` and `migrate_recovered_flag_calls.py`: recover
  the three extension flag-word APIs, correct the earlier compatibility getter
  and migrate proven flag callees in recovered functions with body/stack hashes.
- `recover_card_table_initializers.py` and `InspectRecoveryCardArrays.java`:
  inspect and recover Garth/Serra's writable copied tables with exact data,
  dimensions, copy operands and native player-word access evidence.
- `recover_mana_caches.py` and `recover_mana_counter.py`: recover the four live
  modifier arrays, both AI snapshot pairs, the complete count pass and two typed
  native list aliases, checking reset instructions, VLA shape and native strides.
- `recover_mana_production.py` and `ApplyRecoveryDifferentMana.java`: recover
  seven producer cores/front ends with the distinct-color specialization's five
  physical arguments and verified modifier operands.
- `migrate_recovered_mana_calls.py`: verifies canonical producer/count callees
  and the native jump route before updating recovered callers and body hashes.
- `recover_mana_chooser.py` and `ApplyRecoveryManaChooser.java`: restore the
  typed native dialog tail-call boundary and recover the newer shared chooser.
- `expose_life_upkeep_thunks.py`: matches two retained Weatherlight callbacks
  to their public target entries and exposes their unchanged bodies.
- `recover_engine_flags.py`: verifies per-player card/counterability flag
  storage, complete recalculation/untap resets and both AI snapshot pairs.
- `recover_basic_color_part.py`: preserves the partial basic-land color helper
  and recovers its two card callers with canonical call-site evidence.
- `recover_game_mode.py`: recovers the independent mode word, verified enum and
  the three original pregame writes in their existing control-flow locations.
- `recover_yorion.py`: recovers the five-argument ownership predicate, Yorion's
  two-row selection storage/zero fill and the shared native singleplayer import.
- `recover_token_core.py`: recovers the versioned token layout/actions and eight
  creation/copy helpers, preserving prior exact supporting exports and checking
  the native pool, card-data copy and keyword mirror before applying.
- `recover_token_stack_cards.py`: revalidates three cards' shared DWARF locals,
  token word stores and prompt initialization before recovering their unions
  and the printed-cost callback.
- `recover_token_frontends.py`: recovers six ID/copy entry points with distinct
  public/private identities, physical argument evidence and canonical callees.
- `recover_extension_subtypes.py`: recovers the three mutators with exact
  array endpoints, typed adjacent-word reads and unsigned range arithmetic.
- `recover_protection_dispatch.py`: recovers the integer initial-result local,
  versioned protection modes/API and native-context event wrappers, checking both
  complete owning source units before appending.
- `recover_devouring_type.py` and `recover_damage_choice_label.py`: recover
  grammatical suffix pointers and a native live-string view with exact operand
  checks before appending the private helper and card handler.
- `inspect_extension_foundation.py`: records exact extension layouts, globals,
  templates, lifecycle exports and machine references for the pending migration.
- `recover_extension_foundation.py`, `extension_bodies.py` and
  `ApplyRecoveryInstInfoReturn.java`: recover typed storage, creation/display/AI
  lifecycle, ownership-copy behavior and the eight-byte information return;
  compare both compiled initializers against the release before applying.
- `recover_increment_info.py`: completes the 32-bit information-data increment
  while preserving the two original reads and unsigned wraparound.
- `recover_extension_keywords.py`, `ApplyRecoveryKeywordWorker.java` and
  `recover_keyword_enum.py`: recover extension-backed accessors, separate the
  private two-argument protection worker from its public wrapper and recover
  the second keyword-word enum. Promotion projects only the two verified words
  through typed instance pointers; other extension fields remain deferred.
- `ExportRecovery.java`: exports requested functions with source provenance.
- `recover_target_initializers.py`: recovers the 84-byte targeting type, six
  versioned initializers and the distinct protection compiler part/public
  entry, preserving the older APIs. `ApplyRecoveryTargetAvailability.java`
  removes the abstract constant parameter from the analysis wrapper's physical
  signature before focused target-worker exports.
- `recover_target_validation.py` and `target_validation_bodies.py`: recover the
  matching validator, availability workers and ten public APIs, reconstructing
  only the recorded native buffer operations and typed storage views. The
  protection dispatcher is connected to the recovered private getter.
- `recover_target_selection.py`, `target_selection_bodies.py` and
  `ApplyRecoveryFscanfInterface.java`: recover shared human/AI selector storage,
  selection frontends, auto-target file loading and the counted-picker VLA;
  verify import identity, stack arguments, native cleanup and source templates
  before applying. The compatible player chooser is migrated in place.
- `recover_test_core.py`: recovers the full 216-byte test definition and eight
  versioned constructors/scorers/matchers, retaining the old domain and verifying
  native field operands, callback shape and typed word views.
- `recover_aura_parts.py`: separates three private compiler parts from their
  public wrappers and repairs the three earlier self-call bindings with native
  call-target and body-preservation evidence.
- `recover_test_consumers.py`: recovers the typed graveyard/zone selection and
  counterspell group, three card-set compiler parts, the native to-array wrapper
  and its retained legacy interface. Records VLA argument/allocation evidence,
  private linkage, native calls, live UI bindings and every scoped source repair.
- `recover_sacrifice_core.py`, `sacrifice_core_bodies.py` and
  `ApplyRecoverySacrificeParts.java`: recover the full sacrifice family with its
  separate native flag domain, physically correct private calls, shared stack
  views, exact marked-buffer operations and native callback/global bindings.
- `recover_generic_spell.py` and `generic_spell_body.py`: reconstruct the native
  spell core, shared words/scratch, target-choice VLA, live variadic operands and
  deck-display frontend, while retaining the original flag and aggregate domains.
- `recover_activation_costs.py`, `activation_cost_bodies.py` and
  `audit_activation_costs.py`: reconstruct specialized activation costs and
  multi-graveyard selection, checking native scalar reports, shared scratch,
  dynamic array bounds, outgoing call arguments and preserved selection quirks.
- `recover_attacker_core.py`, `attacker_core_bodies.py` and
  `audit_attacker_core.py`: recover integer/byte attacker tracking, trigger
  resolution and keyword frontends; verify Support's native target copies and
  preserve the existing dispatcher's body while binding its native callees.
- `recover_entry_triggers.py` and `audit_entry_triggers.py`: recover native
  self/global entry dispatch, Evolve and Exploit's private/public pair, verifying
  cache addressing, test filtering, physical parameters and native call targets.
- `ApplyRecoveryLevelBuddy.java`, `recover_level_buddy.py` and
  `audit_level_buddy.py`: recover both private compiler parts and public
  frontends, checking physical/nonphysical parameters, Buddy's discarded return,
  shared prompt prefix and native call bindings.
- `record_dependency_bindings.py`: records direct calls and immediate callback
  references to recovered targets for a promoted batch; verifies source names
  and private ownership before attaching native instruction/range evidence.
- `recover_ward_guard.py`: restores the initialized static recursion pair and
  native protection-Aura helper, verifying DWARF layout, native data bytes,
  public/private linkage, word references and the 84-byte target interface.
- `recover_phyrexian_costs.py`, `phyrexian_engine_fragments.py` and
  `audit_phyrexian_costs.py`: recover native payment/query/casting helpers,
  color words and mode storage, plus the three K'rrik payment blocks, checking
  parameters, data linkage, native instructions, prompts and unchanged engine
  source outside those blocks.
- `recover_spell_trigger_native.py` and `audit_spell_trigger_native.py`: upgrade
  native spell-trigger entries to the 216-byte test interface while retaining
  the earlier source bodies as independently hashed compatibility fragments;
  check copy filtering and test-field operand widths/offsets.
- `ApplyRecoveryMiara.java`, `recover_dependency_repairs.py` and
  `audit_dependency_repairs.py`: recover the optimized two-argument Miara worker
  and three blocked counter/legend/return helpers, checking physical parameters,
  native references, player counter bytes, mask stores and private linkage.
- `recover_death_trigger_core.py` and `audit_death_trigger_core.py`: recover the
  native counting/storage worker, three public frontends and resolver, checking
  test offsets, packed count storage, native cache reads, meld IDs and wrappers.
- `ApplyRecoveryActionPart.java`, `recover_action_core.py` and
  `audit_action_core.py`: restore the action worker's concrete register/stack
  ABI and disjoint ranges, native unsigned actions, action/copy/random-choice
  consumers, source-aware counter-number exposure and exact network interfaces.
  `ExportRecovery.java` accepts optional extra DWARF ranges with exclusive
  endpoints, preserving the existing four-column request format.
- `ExportRecoveryStrings.java`: resolves uniquely identified `.rdata` aliases
  from caller operands; `recover_literals.py` validates and renders their bytes.
- `promote_recovery.py`: screens candidates and, with `--apply`, appends them
  to their original source units and updates shared declarations.
- `check_source.py`: checks all recovered source units and fixed x86 member
  offsets without linking a DLL or running tests.
- `audit_source.py`: verifies hashes, markers and literal bytes against inputs.
- `format_recovery.py`: restores spacing in unstaged recovery additions from
  the raw exports, checks code-token identity and preserves staged content.
  It uses binary formatter pipes to avoid Windows newline conversion.

The persistent Ghidra project is `.recovery/ghidra/RecoveryFull.gpr`, containing
`2022-ManalinkEh.analysis.dll`. Current raw exports are in
`.recovery/decompiled-abi/`. The export request is
`.recovery/export-request.tsv`; all of these are ignored analysis artifacts.

To screen newly resolved exports, run from the repository root:

```powershell
python tools/recovery/promote_recovery.py --max-characters 30000
```

Inspect `.recovery/promotion-report.json` and `.recovery/promotion-compiler.log`;
then apply the source additions with:

```powershell
python tools/recovery/promote_recovery.py --max-characters 30000 --apply
python tools/recovery/format_recovery.py --apply
python tools/recovery/check_source.py
python tools/recovery/audit_source.py
```

Existing source implementations are skipped, and the accepted-function manifest
is cumulative. Full regeneration of the raw exports uses the saved Ghidra
project with `-noanalysis`, followed by `ApplyRecoveryAbi.java`,
`ApplyRecoveryRuntime.java` (using both `export_runtime.py` and
`export_engine_interfaces.py` outputs), `ApplyRecoveryStackProbe.java`, and
`ApplyRecoverySpiritSpecialization.java`, `ApplyRecoveryFractalSpecialization.java`,
and `ApplyRecoveryCloneInterfaces.java` for specialized callers, then
`ApplyRecoveryCounterRemoval.java` before exporting removal callers, followed by
`ApplyRecoveryPairReturn.java` and `ApplyRecoverySplitInterfaces.java` for their
focused callers, then `ExportRecovery.java`. The pair-return preparation writes
`.recovery/timestamp-interfaces.tsv` for `ApplyRecoveryRuntime.java` to apply the
two source-verified timestamp interfaces. The clone script takes `.recovery/clone-interfaces.tsv`,
written by `recover_clone_helpers.py --prepare` after checking concrete DWARF.
For action-worker exports, apply `ApplyRecoveryActionPart.java` after the base
ABI import and request both concrete DWARF ranges. `ExportRecovery.java` accepts
a fifth request column of comma-separated `start:end` extra ranges, using
exclusive endpoints; the worker's cold abort range must remain part of the same
recovered implementation.
For Miara exports, apply `ApplyRecoveryMiara.java` after the base ABI import and
before `ExportRecovery.java`; the private worker takes player and amount in two
registers, while its abstract source `card` parameter has no physical location.
Both known direct callers are checked before applying that signature.
For sacrifice exports, apply `ApplyRecoverySacrificeParts.java` after the base
interfaces and before `ExportRecovery.java`; this retains the verified one- and
three-parameter private signatures instead of source-only abstract parameters.
`recover_spirit_specialization.py --prepare` writes
the focused request list; its `--apply` mode appends the verified helper once.
The focused counter exports and adaptations are recorded
in `recovery-evidence/interface-recovery.json`; `recover_counter_api.py` uses
those cached raw exports and refuses to append the API twice.
For information-API callers, apply `ApplyRecoveryInstInfoReturn.java` after the
base ABI import and before exporting. Its explicit EDX:EAX result storage must
not be replaced by a pointer-shaped or hidden-result prototype. The foundation
tool checks the whole source units and both compiled initializer objects before
placing its declarations, fragments and helper bodies.
The prior [recovery plan](recovery-plan.md) and
[registration trace](registration-trace.md) contain the input identities and
registration details.
