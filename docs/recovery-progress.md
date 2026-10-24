# October 2022 source recovery

Implemented directly in the `cards/` and `functions/` source files, starting
2026-10-17. Latest recovery pass: 2026-10-24.
Missing target set files were created with `#include "manalink.h"`; implementations
already present in the source were retained, with scoped repairs and lifecycle
fragments recorded separately.

The built-in Wish/card-picker dialog UI is [temporarily out of scope](recovery-scope.md)
by user request. Two typed chooser stubs return no selection and are tracked
separately from recovered implementations. Surrounding Wish helpers and card
handlers remain in scope; interactive selection awaits UI integration.

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

- **4,589 function implementations:** 3,328 functions named `card_*` and 1,261
  helpers, in **143 checked C source units**, including units containing only
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
- The fifty-fifth pass added **thirteen card handlers**: Zara, Flower // Flourish,
  The Binding of the Titans and ten Aether Revolt handlers. Fourteen native
  operand sites establish the packed energy costs, unsigned player-energy reads
  and opponent-target activation flags. Existing implementations are retained.
- The fifty-sixth pass added **eight implementations: four helpers and four
  card handlers**. The specific enters-play worker and three guarded frontends
  use the native filter layout. Packed activation operands unblock Gonti's
  Aether Heart, Desiccated Naga, Scrying Glass and Urborg Mindsucker.
- The fifty-seventh pass added **twelve implementations: three card handlers
  and nine helpers**. Native life-gain replacement/notification and loss-prevention
  APIs, two state globals and a verified executable counter alias unblock
  Exquisite Archangel, Lich's Mastery, Skyshroud Cutter and False Cure's legacy
  effect. Exact native calls bind 141 earlier recovered functions to these APIs.
- The fifty-eighth pass added **five helpers**: the specialized cannot-lose
  worker, two public wrappers, Lich's Mastery's life-loss callback and its private
  target predicate. Four earlier native callers now use the recovered protection
  APIs. The callback has verified dynamic array bounds and shared text/test storage.
- The fifty-ninth pass added **two public helpers**: native life-loss dispatch
  and Oath of Lim-Dul's callback. Fifty-seven earlier recovered callers gain
  native loss bindings; the gain worker's two loss calls and the loss engine's
  Oath callback are coordinated with the recovered implementations.
- The sixtieth pass added **eight implementations: six `card_*` functions and
  two public helpers**. Native Vow and Lace APIs, plus three retained helpers
  exposed with verified public linkage, unblock Vow of Torment, Moonlace,
  Kari Zev, Metallic Mimic, Glinting Creeper and Radiant Epicure. The retained
  helper bodies are not counted again.
- The sixty-first pass added **15 implementations: six `card_*` functions and
  nine helpers**. Four missing private token-cleanup callbacks, three private
  constructors and two public native API consumers unblock Chance for Glory,
  Sapphire Charm, Planebound Accomplice, Tilonalli's Summoner, Hornet Cannon
  and Norritt. The existing cantrip callback becomes public without counting
  its retained body again.
- The sixty-second pass added **six implementations: two `card_*` functions
  and four helpers**. The temporary-exile worker and its two public wrappers
  preserve meld pairs, ownership, picture fields and discard-on-return modes.
  Two retained Origins helpers gain public linkage, unblocking Nicol Bolas,
  the Ravager and Conqueror's Galleon with its private combat-end callback.
- The sixty-third pass added **six implementations: two `card_*` functions
  and four helpers**. Native spell-copy frontends and the forced-target worker
  unblock Zada and Ink-Treader Nephilim. Fifteen earlier recovered callers now
  use the native copy APIs. Zada's native loop with no normal exit is preserved
  and explicitly annotated.
- 21 previously missing C files were created. They include Strixhaven, Modern
  Horizons 1/2, Commander Legends, Throne of Eldraine, War of the Spark,
  Zendikar Rising, Commander 2020, and `functions/draft.c`.
- The sixty-fourth pass added **eight implementations: seven `card_*` functions
  and one private helper**. It recovers Kamahl's Will, Kamahl, Heart of Krosa,
  Daughter of Autumn, Hazduhr the Abbot, Barbed-Back Wurm, Flowering Field and
  Verdant Field, with the shared white-creature damage predicate in Homelands.
- The sixty-fifth pass added **three implementations: one `card_*` function
  and two public helpers**. Native color/land-word replacement separates the
  acting card from the target, preserves chained substitutions and unlocks
  Balduvian Shaman.
- The sixty-sixth pass added **two `card_*` implementations**, Ajani, Adversary
  of Tyrants and Mu Yanling, Sky Dancer. It also binds 72 earlier recovered
  functions to the existing native token frontends, with 77 verified call sites
  across 27 source units.
- The sixty-seventh pass added **six implementations: four `card_*` functions
  and two public helpers**. Filter-aware Wish/card-list APIs unlock Granted,
  Mastermind's Acquisition, Vivien, Arkbow Ranger and Spike, Tournament Grinder.
  Two built-in dialog entry stubs are tracked separately and excluded from
  these counts; the UI implementation is out of scope by user request.
- The sixty-eighth pass added **one `card_*` implementation**, Karn, the Great
  Creator. It also corrects one earlier ability-filter helper interface and
  migrates Blood Sun and the Revel in Silence effect to the native filter
  layout. These three corrections do not add to the implementation total.
- The sixty-ninth pass added **nine implementations: three `card_*` functions
  and six public helpers**. Native library reveal/selection APIs unlock Vivien
  Reid, Turntimber Symbiosis and Explore the Vastlands. The existing EXE deck
  viewer remains an external call; no additional dialog code is recovered.
- The seventieth pass added **25 implementations: 15 `card_*` functions and
  ten helpers**. Scoped flag-word recovery and four native phasing functions
  unlock more handlers. The first successful GCC 9.3 32-bit DLL/debug build is
  verified separately; two earlier terminal labels receive null statements
  solely to satisfy GCC C grammar.
- The seventy-first pass added **23 implementations: 18 `card_*` functions and
  five helpers**. Caller-specific flag masks and the verified native attacker
  query unlock combat, equipment and other handlers. Two recovered Partner
  helpers supply Akroma's keyword count; Defiling Tears and Tin Street Dodger
  follow their recovered callbacks. The full GCC 9.3 build succeeds again.
- The seventy-second pass added **ten implementations: three `card_*` functions
  and seven helpers**. It recovers Legion Warboss, Tidal Wave and Chandra,
  Acolyte of Flame, plus the native haste/delayed-removal family. Four earlier
  token callbacks and four earlier consumers migrate to the native token API;
  these eight corrections are excluded from the new-implementation count.
- The seventy-third pass added **17 implementations: five `card_*` functions
  and twelve helpers**. The native reanimation family unlocks Entreat the Dead,
  Salvaging Station, Claim // Fame, Revival // Revenge, and Calix, Destiny's Hand.
  The full GCC 9.3 build and all recovery checks pass.
- The seventy-fourth pass added **three `card_*` functions**: Command the
  Dreadhorde, Dance of the Manse, and Eerie Ultimatum. Native packed-byte
  accesses, a missed variadic argument and two dynamic arrays resolve the
  remaining cached reanimation callers. All recovery and build checks pass.
- The seventy-fifth pass added **six implementations: two `card_*` functions
  and four helpers**. Tanazir Quandrix uses a verified creature snapshot, and
  four miscellaneous protection frontends reconstruct the existing native
  source-wrapper guard. The helpers unlock Shanna, Sisay's Legacy.
- The seventy-sixth pass added **one `card_*` function**, the Ozolith. Two
  previously accepted leaves-play helpers move to the native 216-byte test API,
  retaining their legacy bodies as compatibility fragments. Three existing
  card callers migrate with exact predecessor and historical-proof replay;
  those five upgrades are excluded from the new-function count.
- The seventy-seventh pass added **four implementations: three `card_*`
  functions and one helper**. Arahbo, Plague Reaver and Noxious Field use
  verified native target/test storage. The native area-damage helper supplies
  Noxious Field's 216-byte test interface and changed filtering behavior.
- The seventy-eighth pass added **four implementations: one `card_*` function
  and three helpers**. Indomitable Creativity now uses the native reveal and
  destruction-tracking family, with two reconstructed dynamic arrays and
  verified packed counters. All recovery and build checks pass.
- The seventy-ninth pass added **six implementations: five `card_*` functions
  and one private helper**. All five Disciple handlers use their native
  four-argument color-mask worker, with verified tail jumps, native target
  definitions and corrected controller-preference store ordering.
- The eightieth pass added **five implementations: one `card_*` function
  and four helpers**. Vivien, Champion of the Wilds and native casting-as-flash
  use independently verified private event contexts and mana callbacks, with
  native target/test definitions and a reconstructed signed deck-count local.
- The eighty-first pass added **four implementations: two `card_*` functions
  and two helpers**. Zombie Musher and Hallowed Ground use the shared native
  snow-land predicate and private nonsnow validator. Native linkage, target
  layout and the engine subtype-error pointer are verified.
- The eighty-second pass added **four implementations: two `card_*` functions
  and two helpers**. War Room uses the native both-commanders color
  specialization; Hellkite Courser uses the native command-zone wrapper and
  an existing recovered delayed callback. All build/recovery checks pass.
- The eighty-third pass added **six implementations: four `card_*` functions
  and two public helpers**. Windscouter, Wall of Junk, Quicksilver Behemoth
  and Rune of Protection: Artifacts use verified shared native callbacks.
  Native linkage and operands distinguish the bounce callback from its
  different-signature private homonym. All recovery/build checks pass.
- The eighty-fourth pass added **eleven implementations: three `card_*`
  functions and eight public helpers**. Ral, Tuvasa and Thousand-Year Storm
  use the recovered native storm readers and shared hand-type counter.
  The native increment routine, shifted mode domain and backing-state layout
  are recovered; collector/reset integration remains downstream work.
- The eighty-fifth pass reconstructed **two inlined storm routines** as source
  fragments. The standalone function count stays unchanged. Collector/reset
  code preserves native event timing, copied-spell exclusion, Werewolf history
  and the newer counter, trap and Rules Engine identifier meanings.
- The eighty-sixth pass added **seven implementations: three `card_*` functions
  and four public helpers**. Firemind Vessel, Stream of Acid and Prismatic
  Circle use recovered artifact-mana frontends and shared target predicates.
  Native worker specialization, targeting, error-pointer and color-mask behavior
  are verified, with earlier helper bodies retained.
- The eighty-seventh pass added **eleven implementations: ten `card_*`
  functions and one private helper**. The native Embalm specialization unlocks
  Trueheart Duelist, Unwavering Initiate, Aven Initiate, Labyrinth Guardian,
  Tah-Crop Skirmisher, Vizier of Many Faces, Heart-Piercer Manticore, Honored
  Hydra, Aven Wind Guide and Temmet. All sixty caller constants are verified.
- The eighty-eighth pass added **six implementations: five `card_*` functions
  and one private helper**. All five Memorial lands use the corrected native
  worker signature. Five existing resolvers are migrated to native target/test
  interfaces, with exact previous records and reversible source views retained.
- The eighty-ninth pass added **five implementations: two `card_*` functions
  and three helpers**. Native spell-cost payment and the Improvise worker/frontend
  unblock Inspiring Statuary and Skyclave Shade. The accepted Improvise cost
  modifier gains native targeting; signed-byte/index/EXE-call evidence and two
  post-initializer controller stores are verified.
- The ninetieth pass added **one private helper** and corrected helper bindings
  in **fifteen existing card handlers**. Four Expertise handlers now call the
  two-register native free-cast specialization with the correct limit; eleven
  handlers use the native Improvise frontend. Eighteen native call/tail sites
  and exact reversible history are verified; migrations are not new functions.
- The ninety-first pass added **one `card_*` implementation**, Saheeli's
  Directive. Complete native range coverage and concrete DWARF recover its
  variable-length reveal array, formatted prompt and selection/restore/mill
  loops, with native event, Cage, zero-count and failure paths retained.
- The ninety-second pass adds **ten implementations: seven `card_*` functions
  and three helpers**. One specialized Escape helper unlocks five handlers;
  the counter-entry and subtype-target helpers unlock Renata and Fugitive Druid.
  One combined check/build verifies the entire batch, including thirty Escape
  caller constants and the private eight-byte targeting result.
- The ninety-third pass adds **30 implementations: 28 `card_*` functions
  and two private callbacks** across fourteen set files. Three native combat-AI
  accumulators resolve the shared blocker; 95 exact global operands, 115 native
  calls and eight callback operands are verified in one combined check/build.
- The ninety-fourth pass adds **fifteen implementations: eight `card_*`
  functions and seven helpers**. Spectacle, Amplify, counter-based boosting,
  hand reveal, coercion/discard and filtered top-deck exile are recovered as one
  batch. Native signatures, 24 caller cost constants, fixed hand arrays and
  the formatted prompt frame are checked; the combined GCC build passes.
- The ninety-fifth pass adds **thirteen implementations: seven `card_*`
  functions and six helpers**. Graveyard recovery, sustained pumps, Suspend
  targeting, both hand-reveal flags and a fixed historic-permanent prompt are
  recovered together. `functions/deadbox.c` becomes the 141st checked C unit;
  the full source/ABI/provenance and GCC build checks pass.
- The ninety-sixth pass adds **fifteen implementations: eleven `card_*`
  functions and four public helpers**. Devil creation, Overload and creature
  mana production support a batch with native packed-field, prompt, fixed-table
  and overlapping-stack recovery. Return to Ravnica becomes the 142nd checked
  C unit, and all fifteen new public symbols pass the combined build check.
- The ninety-seventh pass adds **fifteen implementations: ten `card_*`
  functions and five helpers**. Permanent copying and exile-play effects are
  recovered with Kess, Echoing Equation, both Valakut cards and six other
  handlers. Fourteen public symbols and the private copy callback pass the
  combined source/ABI/provenance/compiler-part and GCC build checks.
- The ninety-eighth pass adds **nine implementations: five `card_*`
  functions and four private helpers**. Underworld Breach, Chainer, Liliana,
  Genesis Ultimatum and Gyrus recover native graveyard/deck selection with four
  reconstructed VLA frames. The complete batch passes source/ABI/provenance,
  compiler-part and GCC build checks; private symbols are resolved by source
  ownership to distinguish a callback homonym and GCC outlining.
- The ninety-ninth pass adds **seven implementations: one `card_*`
  function and six helpers**. Hogaak's Convoke/Delve family, native mana-text
  formatting and shared Convoke color chooser are recovered together. The
  optimized predicate has seven physical parameters and one nonphysical source
  parameter; four VLAs and all three hybrid-cost call frames are verified.
  Six public symbols and the private predicate pass the combined build checks.
- The hundredth pass adds **19 implementations: 15 `card_*` functions and four
  helpers**, and upgrades one previously accepted counter helper to its native
  target layout while retaining its exact predecessor record and legacy body.
  Nimble Obstructionist, the two Hourglass bodies, Command handlers and other
  flag/target consumers are recovered with their callback dependencies.
- The hundred-and-first pass adds **14 implementations: ten `card_*`
  functions and four helpers**. Three modal handlers, full target-definition
  copies, packed limb/damage state, Lutri's deck test and native subtype/expansion
  tables are recovered with the activated-ability counter actions.
- The hundred-and-second pass adds **ten implementations: seven `card_*`
  functions and three helpers**. Six library-selection VLAs and fourteen
  outgoing selection/formatting/casting frames are restored alongside Planewide
  Celebration and the native Proliferate family. Native queue, ownership,
  cancellation and Cage-gate quirks are preserved.
- The hundred-and-third pass adds **eight implementations: six `card_*`
  functions and two helpers**. Four target-display VLAs, Metzali's random
  candidates and complete dialog frame, Aether Tide's hand/discard arrays and
  both multiple-X payment interfaces are recovered together.
- The hundred-and-fourth pass adds **nine implementations: four `card_*`
  functions and five private helpers**. Spark Fiend, Togglodyte, Now I Know My
  ABC's and Staff of the Letter Magus regain their state/name helpers, native
  character-fold tables, alphabet bitmap and consonant choices.
- The hundred-and-fifth pass adds **three implementations: two `card_*`
  functions and one shared helper**. Punctuate and Capital Offense use the
  recovered rules-text normalizer. Bloodletter's native character folding and
  sacrifice/damage dependencies are repaired without counting it again.
- The hundred-and-sixth pass adds **eight implementations: three `card_*`
  functions and five shared helpers**. Elvish House Party, Old-Fashioned Vampire
  and Snow Mercy regain the native Windows date/time/delay helpers and exact
  two-argument redraw boundary.
- The hundred-and-seventh pass adds **five helper implementations** and all
  198 native pack descriptions in the newly restored `functions/packs.c`.
  Availability, sizing, announcements and two private choice/pool workers are
  recovered together; their generator and card callers remain pending.
- The hundred-and-eighth pass adds **three pack-generation helpers**, including
  the native weighted tables and qualified 2022 wrapper.
- The hundred-and-ninth pass adds **five pack-chooser implementations**, including
  Booster Tutor, Stocking Tiger and the native dialog template/callback.
- The hundred-and-tenth pass adds **ten implementations: six `card_*` functions
  and four qualified graveyard-replacement helpers**.
- The hundred-and-eleventh pass adds **five implementations: four `card_*`
  functions and the private Void-counter constructor**.
- The hundred-and-twelfth pass adds **six `card_*` functions**, restoring Ormos,
  Pelt Collector, Blaze of Glory, Dispossess, Hot Fix and Mise.
- The hundred-and-thirteenth pass adds **five `card_*` functions**, restoring both
  Nicol Bolas cards, Tamiyo, God-Eternal Kefnet and Aryel. The named card-handler
  inventory now has no missing names; modified existing bodies, helpers and
  deferred interactive chooser integration remain separate work.
- The hundred-and-fourteenth pass adds **nine implementations: four modified
  `card_*` functions and five legacy/selection helpers**. Bruna, Personal
  Incarnation, Toshiro and Seht's Tiger receive qualified `_2022` versions beside
  their retained original handlers; these are four existing card identities.
- The hundred-and-fifteenth pass adds **nine implementations: one modified
  `card_*` function and eight helpers**. Havengul Lich receives its permission,
  selection and borrowed-ability helpers, alongside commander-state,
  expansion-viewer and fixed-mana wrappers. Its original handler/helpers remain.
- 593 recovered functions retain private/static linkage, including nine private
  implementation helpers whose names begin with `card_`. There are 677 recovered
  public helpers; shared declarations live in the existing `manalink.h`.
- Every function has its target address and original DWARF source unit/line
  recorded immediately above its implementation.
- Of the recovered symbols, 3,308 are referenced by the target registration
  records; 3,219 serve at least one record marked CODED. The remaining functions
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

The fifty-fifth pass adds the three handlers unblocked by the native global
tutor: Zara, Renegade Recruiter; Flower // Flourish; and The Binding of the
Titans. It also recovers Aetherwind Basker, Consulate Turret, Glint-Sleeve
Siphoner, Gonti's Machinations, Greenbelt Rampager, Lightning Runner, Peema
Aether-Seer, Shielded Aether Thief, Shipwreck Moray and Tezzeret's Simulacrum
in `cards/aether_revolt.c`. Packed energy costs had resembled executable
addresses to the conservative screen. Native outgoing argument stores at stack
offset 40 identify the cost word passed to `generic_activated_ability_2022`;
existing `GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, n)` macros preserve those exact
bits. Four unsigned byte comparisons use the energy field at offset 2 with a
four-byte player stride. Tezzeret's activation mode is a verified combination
of untapped and opponent-target flags. Consulate Turret's apparent pointer local
is the integer cost word, with zero and three-energy alternatives. Its prompt
retains the native mutable pointer interface. No address-range guard was relaxed.

All 139 source units and layout checks pass. Every earlier implementation record
and C token prefix is preserved; headers, definitions and linker script are
byte-identical to the preceding batch. Ten manually reconstructed bodies have
hashes, full native function hashes, replayable operand changes and canonical
callee proofs. Four recovered dialog arguments are checked against read-only
bytes. Gonti's Aether Heart remains deferred pending recovery of the native
`new_specific_cip` aggregate interface. The canonical export cache remains at
4,625 rows and 4,568 original names; native linking and gameplay remain pending.

The fifty-sixth pass recovers the private `new_specific_cip_impl_2022` worker,
plus `specific_cip_2022`, `new_specific_cip_2022` and
`new_specific_cip_global_2022`, in `functions/functions.c`. DWARF proves the
worker's first three arguments use EAX/EDX/ECX and its remaining four arguments
use stack words. The public wrappers retain their enters-play trigger guard;
the worker retains reason/controller checks, optional source/type filtering,
the 16-bit `not_me` field, trigger mode resolution, target slot 1 stores and
power/toughness recalculation bits. The scalar frontend constructs the full
216-byte test and truncates its five flag inputs to their native 16-bit fields.
Unused `t_player` parameters remain unused, as in the target. All earlier source
interfaces and implementations are retained.

Gonti's Aether Heart now uses that recovered aggregate interface and an exact
eight-energy cost. Desiccated Naga, Scrying Glass and Urborg Mindsucker use native
opponent-target activation flags, with the latter also retaining sacrifice and
sorcery-speed bits. Five outgoing argument stores prove these four handlers'
cost/mode values and their native activated-helper destination. Sixteen native
sites separately cover the enters-play worker's filtering/context and wrapper
guards/calls. All eight new bodies have native hashes, source hashes and callee
evidence. The three public declarations are added to `manalink.h`; definitions,
linker script, earlier implementation records and source token prefixes are
preserved. All 139 source units and ABI checks pass. The canonical cache now has
4,628 rows and 4,571 original names. Native linking and gameplay remain pending.

The fifty-seventh pass recovers eight native life helpers in
`functions/functions.c`: the private gain-life and can-lose workers; ordinary
and lifelink gain frontends; gain-as-cost permission; public loss permission;
the game-loss frontend; and the gain-life trigger handler. DWARF proves EAX for
the private loss worker and EAX/EDX/ECX for the gain worker. Both-player routing,
replacement callbacks, saved trigger globals, negative replacement results,
life/trap updates, gain notifications and the post-notification draw/loss paths
retain their native order. Context push/pop calls retain the existing executable
void signatures. The game-loss frontend honors the suspension flag and the
native special-card/speculation branches before reaching the executable hook.

The target uses `0x01000600` for life-gain replacement; the old source's
Shandalar-only declaration uses `0x01000800`. A separate
`XTRIGGER2022_REPLACE_GAIN_LIFE` constant preserves the native value without
changing that earlier declaration. `life_to_lose_on_gain_life` and
`suspend_cannot_lose_the_game` are reconstructed as external four-byte BSS state,
with COFF addresses, linkage and DWARF integer types recorded. A two-word
executable counter at `0x4ef538` controls the draw-on-gain path; its new source
name, `life_gain_draw_count_2022`, is explicitly inferred. It is a linker alias,
not separately allocated storage. New state lifecycle and broader integration
remain pending.

These APIs and declarations unblock Exquisite Archangel, Lich's Mastery,
Skyshroud Cutter and False Cure's private legacy effect in their original set
files. In addition, 141 earlier recovered functions in 41 source units now call
the native public life APIs, with exact native destinations, original records,
reversible identifier bindings and before/after body hashes retained. Eighty-two
earlier manual bodies remain deferred for coordinated component migration.
Twenty-nine native sites pin the core's flags, context calls, state restoration,
replacement event, executable counter and public/private routes. All 139 source
units, ABI checks and provenance checks pass. Earlier C token prefixes survive
after reversing the recorded bindings; prior fragments, globals, interface
components and stack records are preserved. The canonical cache has 4,631 rows
and 4,574 original names. Native `lose_life`, the specialized cannot-lose
suppression worker, linking and gameplay verification remain pending; recovered
handlers do not imply completed card mechanics.

The fifty-eighth pass recovers `cannot_lose_the_game_impl_2022` and both public
protection wrappers in `functions/functions.c`. DWARF gives `for_life` the
constant value 1 and no storage: the worker has six physical arguments, with
three in EAX/EDX/ECX and three stack words. Restoring that specialization fixes
the decompiled poison/draw arguments. The general wrapper enables both; the
life-only wrapper disables both. Life suppression is unconditional in this
native worker. Poison-byte reads use the verified four-byte player stride.
Hidden life/poison debt, protected-player changes, leaving-play marker 17 and
empty-library draw suppression retain their native behavior. Four recovered
callers, including Exquisite Archangel and Lich's Mastery, gain exact native
public-callee bindings; their earlier records and reversible body hashes remain.

The same pass reconstructs `lichs_mastery_lifeloss_trigger_2022` and its private
hand/permanent predicate in `cards/dominaria.c`. DWARF and the allocation code
prove `int grave[amount]` and `target_t tgts[amount + 1]`. The 100-byte prompt
buffer and 216-byte test have the same stack location; a union preserves that
storage, with the prompt occupying `test.message` at offset zero. The target
definition occupies a separate 84 bytes. Eighteen outgoing dialog argument
stores, nine read-only text values and the VLA/selection/exile instructions are
recorded independently. The predicate preserves a writable runtime executable
error-buffer address; it does not invent a static error string. Target-selection
failure still advances the array index without advancing the successful count;
the grave tutor return remains ignored, and all selected targets are unmarked
before exiling. Arrays are not silently initialized or capped. Native hand
exile still goes through top-of-library removal; permanent exile uses kill mode 4.

All 139 source units, ABI checks and provenance checks pass. Earlier source
tokens survive after reversing four documented callee bindings; prior globals,
fragments, interface components and stack registry entries are preserved.
Headers add only three public prototypes. The canonical cache has 4,634 rows and
4,577 original names. Native `lose_life` and its routing to the new callback,
broader integration, linking and gameplay verification remain pending.

Ghidra briefly could not save because the C: drive was nearly full. Removing
generated compiler scratch copies allowed the export to save successfully.
Forty-eight older baseline scratch snapshots (passes 7 through 54) were then
compressed into `.recovery/cache-archives/`; every file was compared byte-for-byte
before its expanded copy was removed. This reclaimed about 852 MiB. The archive
index records ZIP hashes, and its README gives restoration instructions for old
one-time prefix verifiers. Raw exports, source files, Ghidra projects and the
current baseline snapshots were retained.

The fifty-ninth pass recovers `lose_life_2022` in `functions/functions.c`.
DWARF identifies a fixed `int marked[2][151]` array at frame offset -1228; the
native zeroing sequence clears 302 four-byte words. The reconstructed body uses
`memset` for precisely those 1208 bytes and retains the remaining native control
flow. All-player recursion, invalid-player/nonpositive-amount returns, life-total
permission, pre-loss callback-ID snapshots, trigger humiliation checks,
life/trap/sound updates and player-before-opponent callbacks are retained. The
engine now calls the recovered gain worker and Lich's Mastery callback. Both
loss calls in the earlier recovered gain worker use the new routine, with the
earlier record and reversible body hash retained; its native evidence gains only
those two proven call sites.

`oath_of_lim_dul_lifeloss_trigger_2022` is recovered in `cards/ice_age.c` from
the original callback's verified control flow, using the 84-byte target APIs and
native `amount + 1` target VLA. Failed selection still advances the index without
advancing the successful count. All targets are unmarked before discarding hand
cards or sacrificing permanents. Its unchanged private predicate is verified
against the separate native entry and supporting export, rather than counted as
a newly recovered implementation. The original callback and predicate remain
unchanged. The native life-loss engine's one Oath call now uses the recovered
callback, with its earlier stage record, reversible identifier binding and exact
native destination retained. The selection prompt is checked against native
read-only bytes.

Fifty-seven previously recovered functions in 36 source units now use the
native life-loss entry with exact direct-call evidence; 78 earlier manual bodies
await coordinated component migrations. All 139 source units, ABI checks and
provenance checks pass. Earlier source token prefixes survive after reversing
the recorded public-callee and gain-worker bindings. Existing fragments, globals
and stack evidence remain intact; the earlier life component changes only for
the two coordinated loss call proofs. Headers add two public prototypes, with
definitions and the linker unchanged. The canonical cache has 4,637 rows and
4,579 original names. Broader lifecycle/dispatch integration, earlier manual
consumers, native linking and gameplay verification remain pending.

The sixtieth pass recovers `vow_2022` (`0x23e1320`) in `cards/commander.c`
and `lace_2022` (`0x24bd8f0`) in `cards/unlimited.c`. Vow checks the abilities
event, the affected attached creature and its opposing controller before calling
the native cannot-attack helper. It uses the recovered aura API. Lace retains
the native spell-event routing and two 84-byte target definitions at DWARF frame
offsets -188 and -104. Its resolution path writes the boolean target-validation
result into the four-byte info slot at offset 56. The original source contains
assignments in those conditions; the native XOR/TEST/SETNE and dword stores
confirm that behavior, which is preserved rather than corrected.

The existing `converge` and `is_creature_type` bodies now have public linkage.
Converge is a five-byte tail jump to the retained sunburst storage wrapper;
the private color-payment worker and wrapper are preserved and hash-verified.
The creature predicate accepts exactly subtypes 1 through 4095, matching the
native unsigned comparison. The retained Innistrad exile callback also becomes
public, as `exile_at_end_of_combat_2022`; its definition and local token callback
reference are renamed to avoid the separate private Commander 2015 helper.
Its body remains unchanged. None of these three retained implementations is
added to the recovery count.

These helpers unlock six handlers: Vow of Torment, Moonlace, Kari Zev, Skyship
Raider, Metallic Mimic, Glinting Creeper and Radiant Epicure. Eight native operand
sites bind the six cards to their shared helpers, including Kari Zev's immediate
callback pointer to `0x2073940` and Radiant Epicure's recovered life-loss/gain
calls. All 4,123 earlier implementation records and all prior interface
components remain unchanged. Earlier source token prefixes survive after
reversing only the three linkage edits and the Innistrad helper rename.
Headers gain five prototypes; definitions, linker script, fragments, globals
and stack registry are unchanged. All 139 source units, ABI checks and
provenance checks pass; the separate Commander 2015 source also passes syntax
checking with its original private callback intact. The canonical export cache
now has 4,639 rows and 4,581 original names. Native linking and gameplay
verification remain pending.

The sixty-first pass recovers four missing private delayed-token callbacks in
their original units: `effect_sacrifice_at_next_eot` (`0x2377110`, Mirage),
`effect_destroy_at_next_eot` (`0x212c130`, Stronghold), `remove_eot_unless_cb`
(`0x218f5b0`, Rivals of Ixalan) and `sac_at_eot` (`0x2335b80`, Modern Horizons).
They retain the native end-step guard, attachment coordinates, countdown in
`targets[1].player`, mandatory-trigger flag and phased-out checks. Tilonalli's
cleanup also checks the controller's city's blessing. Despite its name, the
destroy callback passes `KILL_SACRIFICE`; the native outgoing mode is 3, and
that behavior is retained. Constructors for sacrifice, destroy and the
city's-blessing exception are also recovered as private helpers in those units.

`last_chance_legacy_2022` (`0x21b8dc0`) and `effect_nettling_imp_2022`
(`0x24bc200`) use the recovered game-loss and attack APIs. Their earlier source
helpers remain untouched. The existing `cantrip_legacy` (`0x23eb810`) body
matches the native callback and is exposed with public linkage, without adding
another recovered implementation. Norritt's native target definitions occupy
84-byte locals at frame offsets -188 and -104. Its color-text facade returns a
`const char*`, while the native activation API accepts `char*`; an explicit
qualifier cast is confined to that prompt argument. The native helper return
in EAX is stored unchanged into outgoing argument 12 before the activation
call. The source facade, forwarding wrapper and activation worker body hashes
are recorded as supporting interface evidence.

The six handlers are Chance for Glory, Sapphire Charm, Planebound Accomplice,
Tilonalli's Summoner, Hornet Cannon and Norritt. Nine native immediate operands
verify the direct helper calls and callback pointers of these cards and their
three constructors. All 4,131 earlier implementation records, interface
components, fragments, globals and stack evidence remain unchanged. Every
earlier source token prefix survives after reversing the single cantrip linkage
edit. The header gains three prototypes; definitions and the linker script are
unchanged. All 139 source units, ABI checks, provenance checks and compiler-part
call checks pass. The canonical cache retains 4,639 rows and 4,581 original
names. Native linking and gameplay verification remain pending.

The sixty-second pass recovers `remove_until_upkeep_impl` (`0x2685a40`) in
`functions/functions.c`, followed by `remove_until_upkeep` (`0x2685c10`) and
`remove_until_upkeep_discard_cards_equal_to_toughness` (`0x2685c50`). All three
are public in the target symbols. The worker's six arguments have concrete
stack locations. Its malformed packed read is reconstructed from the adjacent
unsigned 16-bit `card_data_t.id` and `reserved2` fields, preserving the original
32-bit value. Native addressing confirms a 72-byte card-data stride and ID
offset 36; the picture ID and image number use two-byte stores at instance
offsets 100 and 102.

The worker retains the native three meld-ID branches and two-element `csvids`
array at DWARF frame offset -28. It exiles the original target, creates one or
two return legacies as appropriate, preserves ownership from state bit 12,
records whose upkeep to use and writes the native discard marker, either -1 or
66. The wrappers pass exactly 0 and 1 in outgoing argument 5. The constructor's
callback pointer is bound to the already recovered `remove_until_upkeep_legacy`
at `0x269f000`; that implementation remains unchanged.

The existing Origins `return_to_bf_transformed_impl` (`0x20d9140`) and
`exile_and_return_to_bf_transformed` (`0x20d9fd0`) bodies match the native
exports and gain public linkage. Neither body is counted again. The newly
recovered private `transform_at_end_of_combat` (`0x22128b0`) preserves its live
attachment and phased-out checks, controller-preserving blink callback and
legacy removal. These dependencies unlock Nicol Bolas, the Ravager and
Conqueror's Galleon in their original card-set units. Five native operands
verify the two wrappers and the card-to-callback chains, alongside the two
outgoing wrapper mode stores.

All 4,146 earlier implementation records and interface components remain
unchanged. Earlier source token prefixes survive after restoring only the two
Origins static qualifiers; both retained body hashes match. The header gains
five prototypes, with definitions, linker script, fragments, globals and stack
registry unchanged. All 139 source units, ABI checks, provenance checks and
compiler-part checks pass. The canonical cache now contains 4,640 rows and
4,582 original names. Native linking, deeper blink/transform comparison and
gameplay verification remain pending.

The sixty-third pass recovers private `real_copy_spell_2022` (`0x2668490`) in
`functions/play_spell_for_free.c`. Its six physical arguments use EAX, EDX and
ECX plus three stack words. The optional sixth argument supplies a forced
`target_t`; the worker writes that target and sets the one-byte target count
to 1. The ordinary clone path preserves the info slot, the four-byte
end-of-turn toughness word and all 19 target slots. It combines the source's
three native special-flag words with the requested flags and the copied-spell
marker through the recovered extension APIs. Its null-source path retains
the native X-value assignment and distinct flag behavior.

The public frontends are `copy_spell_2022` (`0x2668690`),
`copy_spell_from_stack_2022` (`0x26686d0`) and the newly available
`copy_spell_from_stack_set_target0` (`0x2668720`). The forced-target frontend
adds `SF_TARGETS_ALREADY_SET`, producing the native `0x200c0000` flag word;
the other two pass `0xc0000` and no forced target. DWARF places the target
pair at frame offset -16. The existing private `free_spell_player` and
`free_spell_card` globals retain their original declarations and -1
initializers. The worker preserves stack insertion, fallback placement, land
bookkeeping and card-state cleanup. A fresh export with the existing
address-matched EXE prototype recovers the three-argument `dispatch_event`
call to `0x4359b0`; the native outgoing event is `EVENT_SPELL_COPIED`
(`0x5c00`). The older cached zero-argument spelling was incomplete analysis.

Zada, Hedron Grinder and Ink-Treader Nephilim now use the forced-target
frontend and recovered spell-trigger API. Their fixed snapshots are verified
as `int[151]` at frame offset -624 and `int[2][151]` at -1228, respectively.
Ink-Treader retains its two per-player counters, legal-target filtering and
native player/reverse-target iteration. Zada has a confirmed native control-flow
oddity: after selecting the last saved target, the call at `0x2455a49` is
followed by the unconditional `EB E0` branch at `0x2455a4e`, returning to
`0x2455a30`. That loop has no normal exit or target-index advance. The
reconstruction preserves it with a source comment and manifest annotation.
The target registration points to this handler and marks it CODED; recovery
does not imply that the handler is suitable for gameplay without a later fix.

Fifteen earlier recovered functions in 11 source units now call the native
copy frontends with exact call/jump evidence; ten earlier manual bodies await
coordinated migration. All 4,152 earlier records survive except for these
explicit identifier bindings, each retaining its complete prior record.
Earlier source token prefixes survive after reversing the bindings. Original
five-argument copy source and its public frontends, prior interface components,
fragments, globals and stack evidence remain unchanged. The header adds three
public prototypes; definitions and the linker script are unchanged. All 139
source units, ABI checks, provenance checks and compiler-part checks pass. The
canonical cache has 4,642 rows and 4,584 original names. Native linking and
gameplay verification remain pending.

The sixty-fourth pass recovers Kamahl's Will (`0x25a9da0`) and Kamahl, Heart
of Krosa (`0x25aa4f0`) in `cards/commander_legends.c`. Their decompiler CPU-flag
expressions resolve to signed phase comparisons: the threshold is `PHASE_MAIN1`
(20) on the controller's turn and `PHASE_BEFORE_BLOCKING` (22) on the other
turn. Native `CMP`/`SETLE` instructions and the decrement/mask/add sequence
verify an AI adjustment of +20 through the threshold and -20 afterward. The
Will handler retains its separate iteration increment, two 84-byte target
definitions and the `int[151]` damage-source snapshot at frame offset -624.
The Heart handler retains its target definition at frame offset -104. Existing
global bindings and signed 32-bit phase/AI storage are checked separately.

The private five-argument `damage_dealt_to_white_creature` predicate
(`0x2120600`) returns null for a suitable damage object or the native color-aware
error text. It remains static in `cards/homelands.c`. Daughter of Autumn
(`0x2124160`) and Hazduhr the Abbot (`0x2125a30`) each have two verified native
pointer stores selecting that predicate. This helper unlocked Hazduhr without
additional manual reconstruction. No public header declaration was needed.

Four handlers also regain their native prompt boundaries: Daughter of Autumn,
Barbed-Back Wurm (`0x23897c0`), Flowering Field (`0x2602b50`) and Verdant Field
(`0x2607b60`). Local `char*` casts preserve the prompt pointer where the source
facade uses a const-qualified result. The predicate uses the same local bridge
for its return value. Exact native calls/tail jumps and outgoing four-byte
pointer stores are recorded; the Field handlers retain their literal and null
prompt alternatives. Existing text and activation helper bodies are unchanged
and their hashes remain pinned. Balduvian Shaman was screened but remains
pending: its native four-argument text-changing helper is incompatible with
the retained two-argument source API and needs separate recovery.

All 4,158 earlier implementation records, interface components and C token
prefixes survive unchanged. Header, definitions, linker script, fragments,
globals and the stack registry are unchanged. All 139 source units, ABI checks,
provenance checks and compiler-part checks pass. The canonical cache retains
4,642 rows and 4,584 original names. Native linking and gameplay verification
remain pending.

The sixty-fifth pass recovers the public four-argument
`replace_all_instances_of_one_color_word_with_another_2022` (`0x267f930`) and
`replace_all_instances_of_one_basic_land_type_word_with_another_2022`
(`0x267fdc0`) in `functions/functions.c`. Each receives acting player/card and
target player/card separately. The acting player controls the chooser and AI
selection path; target identity supplies the text-mask cache, native extension
flag and six-byte substitution map. Previously substituted words are offered
as choices, and the final loop updates all entries mapping to the old word.
The raw native AI priorities, dialog controls, literal labels and cache writes
are preserved, including the combined rules/type mask for basic-land words.

Both decompilations incorrectly render the final chooser result as a partial
stack-word assignment. Native `MOV [EBP-44], EAX` stores the complete four-byte
result after `dialog_impl`; subsequent reads and mapping writes use only its
low byte. The source restores that complete unsigned-word assignment and
retains the byte casts at both consumers. DWARF independently records each
option array as `int[6]`; native indexed stores place its base at `EBP-36`,
separate from the saved result at `EBP-44`. The source retains distinct scalar
and array storage. The existing two-argument helpers and three retained mask-extraction
helper bodies are unchanged and hash-pinned. The native zero-valued `SF3_0`
enumerator is added to the existing flag type without changing other values.

Balduvian Shaman (`0x23f08c0`) is recovered in `cards/ice_age.c` with its native
target/activation interfaces and the new four-argument color-word helper.
Its target definition is verified at frame offset -104. Exact native operands
bind the private cumulative-upkeep predicate and `effect_basha` callback. A
local pointer-qualification cast at activation prompt argument 12 preserves
the color-text helper's pointer bits; the return, outgoing store and callee
instructions are recorded. Existing prompt/activation source helpers and both
callbacks retain their body hashes. Mind Bend, Sleight of Mind and Magical
Hack still use their original source bodies; their native four-argument
consumer paths have been exported for separate recovery.

All 4,166 earlier implementation records, interface components and C token
prefixes survive unchanged. The header gains two public prototypes, and
definitions gain only `SF3_0`; linker script, fragments, globals and stack
registry are unchanged. All 139 source units, ABI checks, provenance checks
and compiler-part checks pass. The canonical cache now contains 4,644 rows
and 4,586 original names. Native linking and gameplay verification remain
pending.

The sixty-sixth pass completes the pipeline mapping for the four already
recovered public token ID/copy frontends. These were previously recovered
without general automatic caller translation. New candidates now select
`generate_token_by_id_2022`, `generate_tokens_by_id_2022`,
`generate_reserved_token_by_id_2022` and `copy_token_2022` when those native
helpers are available. Seventy-two earlier recovered functions in 27 source
units receive identifier-only migrations, verified against 77 exact native
call/jump destinations. Each keeps its complete previous manifest record.
Fifty-three earlier manual bodies remain pending coordinated migration.
No additional helper implementations are counted for this wiring.

The single-token frontend returns an integer, whereas the older source helper
returns void. Correcting caller selection unlocks Ajani, Adversary of Tyrants
(`0x2206a90`) in `cards/m19.c` and Mu Yanling, Sky Dancer (`0x23982c0`) in
`cards/m20.c`. At both emblem-creation sites, the native EAX token result
passes unchanged to outgoing argument 1 of `get_card_instance`. Ajani passes
emblem ID `0x49bb` and writes its sleighted color mask at offset 128.
Mu Yanling passes `0x4f99`, writes its hacked subtype at offset 124 and copies
the one-byte Island substitution at offset 278. Exact ID stores, calls,
returned-ID stores and emblem word/byte stores are recorded independently.

DWARF verifies Ajani's 84-byte target definition at frame offset -320 and
216-byte resurrection test at -236. Mu Yanling has its target definition at
-104 and 76-byte token definition at -180. Both handlers preserve the native
target selection, planeswalker modes and remaining event control flow. The
source uses the recovered target/test/token APIs without record-layout casts.

The Wish investigation also traced the four-argument native filter interface
through `new_card_from_list` to the newer C++ card-selection UI. That path
still needs recovery; its chooser wrappers are not replaced with the older
source chooser. Karn's native ability-suppression filter also needs coordinated
layout migration with its existing callers. Those findings do not add to the
recovered-implementation count.

All 4,169 earlier records survive except for the 72 documented identifier
bindings. Earlier source token prefixes survive after reversing only those
bindings. Prior helper bodies and interface components, fragments, globals and
stack evidence are unchanged. Header, definitions and linker script are
byte-identical. All 139 source units, ABI checks, provenance checks and
compiler-part checks pass. The canonical cache retains 4,644 rows and 4,586
original names. Native linking and gameplay verification remain pending.

The sixty-seventh pass applies the user's scope change for the built-in
Wish/card-picker dialog. `functions/choose_a_card.cpp` now provides two typed
C-linkage stubs, `choose_a_card_test_ct` (`0x26ef820`) and
`choose_a_card_test_ct_allow_cancel` (`0x26ef860`). Both ignore their arguments
and return -1, including the mandatory-selection entry point. The dialog UI
and its dialog-specific internals remain out of scope. These are explicitly
marked placeholders, not recovered native bodies, and are excluded from the
implementation/helper totals. The manifest has separate stub and scope ledgers,
the provenance audit pins their source bodies, and automatic promotion honors
the source-unit exclusion. The normal makefile already discovers C++ source
units; the new stub unit passes a separate 32-bit C++ syntax check.

`new_card_from_list_2022` (`0x268ecf0`) in `functions/functions.c` preserves the
native 216-byte test filter, type/color masks, mandatory/cancel chooser branch
and optional CSVID-to-IID conversion. The C/C++ analysis type-name conflicts
are projected onto the typed stub boundaries without record-layout casts;
the constraints pointer is not dereferenced by either stub. The existing
CSVID lookup preserves the -1 no-selection sentinel. Exact native calls at
`0x268ed5c` and `0x268ed83` bind the two dialog entries. Original type-only
card-list source remains unchanged.

`generic_wish_effect_2022` (`0x20590c0`) in `cards/judgement.c` preserves the
native filter-aware Wish logic: Death Wish life loss, the human card-list path,
AI selection from its filtered library with fallback card ID, hand addition,
reveal and rules-engine bookkeeping. It uses the recovered selection and
life-loss APIs. Interactive card selection remains stubbed; the native
surrounding flow is not replaced by a guessed dialog implementation. The
original type-only Wish helper remains unchanged.

Granted (`0x25d04e0`), Mastermind's Acquisition (`0x2190ed0`), Vivien, Arkbow
Ranger (`0x239a660`) and Spike, Tournament Grinder (`0x25c7170`) now call the
filter-aware helper from their original set files. Four exact native call
destinations and the 216-byte source filter types are checked separately.
Their records explicitly carry the stubbed-dialog limitation. Karn remains
pending its ability-suppression filter migration. The library-reveal chooser
investigation also remains pending; no library-reveal bodies were accepted
during the Wish scope pivot.

All 4,171 earlier implementation records, interface components and C token
prefixes survive unchanged. The header adds two helper prototypes and two
stub-boundary declarations. Definitions, linker script, fragments, globals
and stack registry are unchanged. All 139 C source units, the separate C++
stub unit, ABI checks, provenance checks and compiler-part checks pass. The
canonical cache now contains 4,646 rows and 4,588 original names. Native
linking, interactive UI integration and gameplay verification remain pending.

The sixty-eighth pass corrects the earlier recovery of
`remove_abilities_by_test` (`0x26dda70`). Its native record now identifies
`remove_abilities_by_test_2022` in `functions/abilities.c`, accepting a
216-byte test definition and calling `new_make_test_in_play_2022`. The
previous 208-byte compatibility implementation retains its body and old
prototype; it is explicitly labeled as compatibility source rather than the
native entry. This is an interface correction at an already counted address,
not an additional helper implementation.

The helper retains its native event gates, optional null filter, affected-card
controller selection and ability-loss mask merge. Exact instructions verify
the four-byte mask at instance offset 256, the signed negative-sentinel test,
conditional OR and final store. DWARF records the four physical arguments at
frame offsets 0, 4, 8 and 12. The source ABI check verifies the 208/216-byte
filter distinction, four-byte ability-loss type and mask slot.

Blood Sun (`0x2191920`) and `ris_legacy` (`0x23c7630`, the Revel in Silence
effect) now use the 216-byte filter, matching initializer and corrected helper.
Their native test locals are recorded at frame offsets -236 and -232.
Four exact native calls cover the filter initializer and ability-suppression
dependencies. Changes are limited to three recorded identifier substitutions.
Each earlier record is retained in its migration evidence. Blood Sun's prior
entry-trigger binding remains unchanged and is checked by reversing only the
new substitutions to recover its exact predecessor body before replaying that
older proof. The previous interface component is not rewritten.

This unlocks Karn, the Great Creator (`0x21cfde0`) in
`cards/war_of_the_spark.c`. The native static-ability filter uses the corrected
helper; animation, exile retrieval, planeswalker modes and filter-aware Wish
fallback retain their exported flow. Exact native filter and Wish calls are
recorded separately. Its Wish branch still carries the explicitly stubbed
interactive-dialog limitation from the preceding pass.

All 4,177 earlier records survive except the documented helper-interface
correction and two caller migrations. Earlier C token prefixes survive after
reversing only those caller bindings. Header gains one public prototype;
definitions, linker script, fragments, globals and stack registry are
unchanged. The C++ dialog stubs and scope ledger are unchanged. All 139 C
units, the separate C++ stub unit, ABI checks, provenance checks and
compiler-part checks pass. The canonical cache retains 4,646 rows and 4,588
original names. Native linking and gameplay verification remain pending.

The sixty-ninth pass recovers six native library reveal/selection helpers in
`functions/tutors.c`, with `_2022` APIs retaining the original three source
bodies. `reveal_top_cards_of_library_2022` (`0x26526c0`) clamps the count and
uses the existing Magic.exe deck viewer. The shared worker
`reveal_top_cards_of_library_and_choose_fn_who_chooses_2022` (`0x2654d60`)
restores the chosen-IID VLA, 216-byte filter, chooser routing, destination
switches, remaining-card disposition and delayed graveyard/battlefield entry.
The ordinary, type-based, callback and explicit-chooser frontends are
`0x2655580`, `0x26555e0`, `0x2655740` and `0x26557a0` respectively.

DWARF's array upper bound is a reference to an artificial variable. Decoding
that variable's location expression proves `num_to_pick - 1`; its DIE offset
must not be treated as a constant array length. Native allocation scales the
count by four and probes/subtracts that size from the stack. The display
buffer is 100 bytes, while four lexical diagnostic buffers are 200 bytes;
all five share frame offset -220. A 200-byte union preserves those views.
The type frontend restores the two compiler-inlined string-length scans.

Six read-only title/diagnostic values and nine materialization sites restore
the worker's complete variadic arguments, including the source filename and
native line numbers 884, 902, 945 and 999. The title reads `card_ptr_t.name`
at offset eight after `get_id`; DWARF distinguishes `full_name` at offset
four. The ABI check caught and corrected an initial mistaken field choice.
No target binary was executed. Fifty-six native helper call sites are pinned,
along with the VLA, argument-frame, callback and return instruction ranges.

Native behavior is retained even where surprising: the callback frontend
ignores its `fn` argument and passes NULL; the standalone reveal helper uses
a 50-byte local with a formatting bound of 100; the remaining-card
`TUTOR2022_TPLAYER_HAND` arm gives cards to `1 - t_player`. A single ordinary
battlefield entry returns the created card slot, while attacking entry and
graveyard batching return counts. Ownership, played-from-deck flags and the
callback-before-entry ordering follow the native worker.

This unlocks Vivien Reid (`0x220c4d0`) in `cards/m19.c`, Explore the Vastlands
(`0x23dfc10`) in `cards/strixhaven.c` and Turntimber Symbiosis (`0x230f420`) in
`cards/zendikar_rising.c`. Four exact library-selection calls are recorded.
Turntimber's pointer to `three_counters_for_mv_less_than_three` is verified
separately, along with the native wrapper that discards it; this recovery
does not silently repair that behavior.

`show_deck_exe` (`Magic.exe:0x479360`) remains the existing six-argument engine
boundary. No deck-viewer dialog procedure is decompiled or counted. The
Wish/card-picker stubs and their scope exclusion are unchanged. All 4,178
previous records and C token prefixes survive unchanged; the header gains
one callback typedef and six prototypes. Definitions, linker, fragments,
globals and the shared stack registry are unchanged. All 139 C units, the
separate C++ stub unit, ABI, provenance and compiler-part checks pass. The
canonical cache now has 4,649 rows and 4,591 original names. Native linking
and gameplay verification remain pending.

The seventieth pass recovers 21 implementations through caller-specific
DWARF enums and exact native scalar/global operands, then four more dependent
card handlers. Named scalar values remove ambiguous-address screening failures
without relaxing the address checks. Thirty-five source-expression changes and
64 native helper call sites are recorded in `native_word_cards_2022`.

Narset, Parter of Veils reads the opponent's two-byte `cards_drawn_this_turn`
element at `0x4ef1f4`, using native opponent-index arithmetic. Approach of the
Second Sun and Fierce Guardianship restore four-byte `player_bits` indexing at
`0x4ef1a4`, including byte-two flag tests. Native Approach history uses bits
22/23; distinct `PB2022_` names preserve those values without changing the older
Shandalar-only declarations. Each used enum is checked in its caller's CU;
COFF absolute symbols establish the engine array addresses and DWARF establishes
their types. Source ABI checks confirm both array strides and flag-word widths.

Defender, regeneration-mode, regeneration-target, phased-state and sacrifice
restriction masks unlock Portcullis Vine, Arcades, Teyo, Soldevi Sentry,
Furnace Brood, Medicine Bag, Jungle Troll and Mischievous Poltergeist. The
remaining initial helpers are Lukka's legacy, Xathrid's legacy, the trample/
regeneration effect, the Oubliette pair and the control-loss sacrifice effect.
Native `phase_in_impl`, `phase_in`, `phase_out_impl` and `phase_out` are recovered
under `_2022` names in `cards/mirage.c`; old source bodies remain unchanged.
These preserve recursive attachment handling, the new cannot-phase-in flag,
source-aware counters, target layout and the native detach/return behavior.

The follow-up handlers are Lukka, Wayward Bonder, Spinal Embrace, Out of Time
and Run Wild. Their four native helper/callback operands are pinned separately.
The batch adds one checked C source unit, `cards/arabian_nights.c`, and raises
the canonical cache to 4,652 rows and 4,594 original names. All 4,187 previous
records survive except two documented C-grammar portability changes, each
retaining its exact previous record. Brainstone and Kenrith receive only null
statements after terminal labels; Kenrith's earlier life-callee proof is checked
against the exactly restored predecessor body without rewriting that component.

The user also requested a complete build. [The build procedure](recovery-build.md)
now succeeds with GCC/G++ 9.3 and `-m32`, with warnings allowed. It compiles 164 C
and two C++ units, assembles 20 linked helper/card objects plus the fixed veneer,
links a PE32 DLL and separates its debug file. The veneer is loaded exactly once
by the linker script. Twenty-four historical C files absent from both release
CU inventories and ten duplicate assembly wrappers are excluded; the original
files remain in place. The unique `is_not_dead` assembly entry remains linked.
Header globals become extern declarations: `card_coded` remains an absolute EXE
symbol, while the DLL-owned legacy-title pointer has one C definition. Import
archives are explicitly generated as i386 using a 32-bit assembler.

The [build verification record](recovery-evidence/native-build.json) checks the
PE32 machine, base `0x02000000`, entry/exports at RVAs `0x1000`/`0x1006`, nonempty
DWARF data, all 21 new public symbols, both import archives and the selected
source/object inputs. All 140 recovery C units, the separate C++ stub unit, ABI,
provenance and compiler-part checks pass. Native linking now succeeds. Runtime
integration, the existing veneer/registration table, remaining legacy API
migrations and gameplay verification still need work; neither DLL nor game was
executed. Wish dialog stubs remain unchanged and out of scope.

The seventy-first pass recovers caller-specific keyword, regeneration,
graveyard-ability, Aura movement and Kodama entry masks. Every named mask is
checked against its owning compilation unit's DWARF enumeration and exact
full-width native immediate operands. Yixlid Jailer's two indexed player-word
updates retain their native addresses and disable-graveyard-abilities bit.
The new graveyard-ability and Kodama flag names are confined to the Manalink
configuration. The existing EXE legal-attacker boundary at `0x434be0` gains a
typed inline interface; seven indirect calls are checked against adjacent
register loads and the already verified native EXE body. This interface is
not counted as another recovered implementation.

The new card functions are Akroma, Vision of Ixidor; Kodama of the East Tree;
Chromium, the Mutable; Embercleave; Shadowspear; Yixlid Jailer; Dowsing Dagger;
Towering Titan; Matopi Golem; Aura Finesse; Aethershield Artificer; God-Pharaoh's
Gift; Majestic Myriarch; Neyith of the Dire Hunt; Okaun, Eye of Chaos; Zndrsplt,
Eye of Wisdom; Defiling Tears; and Tin Street Dodger. Native control flow,
including the coin-flip loops and each card's individual combat gate, is
retained rather than replaced with a shared trigger template.

Three private helpers recover the Spitting Drake predicate, Defiling Tears
effect and Tin Street Dodger callback. Two public helpers in `cards/vanguard.c`
recover the expanded Partner predicate and Partner-with lookup. The new
`general_has_partner_2022` preserves all 56 native switch cases while retaining
the old 15-case static helper unchanged. `get_partner_with_id` preserves the
unsigned range check and signed 16-bit reads from the exact 118-byte native
table: 59 entries, including eleven nonnegative mappings. The other entries
and out-of-range IDs return the native `-1` result.

All 4,212 previous function records and existing C token prefixes survive
unchanged. The two newly available card callbacks have independently checked
native pointer operands. No source globals, engine fragments, stack/mask
records or Wish stubs change. Legion Warboss's token callback migration and
Tidal Wave's defender-mask reconstruction are completed in pass 72 below;
Tidal Wave's callback was already migrated. No pointer cast was introduced to
bypass those interfaces. This pass raises the canonical cache to 4,653
rows and 4,595 original names.

All source, ABI, provenance and compiler-part checks pass, and the full GCC 9.3
PE32 build succeeds. The build wrapper now lets Windows PowerShell 5.1 log
nonfatal GCC warnings while preserving the native process's failure exit code;
the verifier accepts that shell's UTF-16 log as well as PowerShell 7's UTF-8.
All twenty newly added public symbols are present in the DLL/debug pair.
Gameplay verification remains deferred.

The seventy-second pass corrects four previously recovered native token
callbacks: `give_token_haste`, `haste_and_must_attack_until_eot`,
`add_hofry_legacy` and `ugins_token_processing`. Their accepted consumers,
Helm of the Host, Ugin the Ineffable, Hofri Ghostforge and the Hofri effect,
now use the recovered 76-byte token definition, constructors and generator.
Per-unit DWARF confirms all 22 members, including `special_flags3` at offset
72, the callback pointer at offset 64 and the packed Hofri payload at offset
52. Callback parameters retain their original three stack slots. No record
casts are introduced. Other legacy test/target APIs in these earlier
consumers remain separate migration work.

All eight prior records are stored exactly in their migration evidence. The
auditor reverses the identifier/type changes to replay two earlier scalar
callee proofs, Hofri's stack-storage proof and Helm's local enum proof. The
earlier stack registry and interface components remain unchanged. The new
typed bodies and native constructor/generator/callback operands are checked
separately. These corrections do not increase the implementation count.

Seven new implementations in `functions/functions.c` recover the private
haste/kill worker, its generic effect callback, exile/sacrifice frontends,
the delayed-effect constructor and two token callback frontends. Versioned
names preserve all seven existing source bodies and legacy declarations.
The private worker's physical interface uses EAX/EDX/ECX for its first three
arguments and one stack argument for the kill mode. Its native countdown,
controller and phased-out guards are preserved. The constructor's native
destroy/bury/bounce branch discards the created effect ID and subsequently
uses `get_card_instance(player, -1)`; the reconstruction retains that behavior.

Legion Warboss binds its typed haste/must-attack callback and exact EXE
attacker-query load/call. Tidal Wave's token keeps the verified defender bit
and its existing typed sacrifice callback. Chandra, Acolyte of Flame uses the
new native haste/sacrifice token frontend, with a separately verified native
callback-pointer store. The promotion and formatting tools now understand
the public delayed-removal API names for subsequent callers.

All 4,227 other earlier records and all prior C token prefixes remain
unchanged after reversing the eight recorded migrations and two forward
declarations. Seven focused exports raise the canonical cache to 4,660 rows
and 4,602 original names. The source/ABI, provenance, compiler-part and GCC
9.3 PE32 build checks all pass. All eight new public symbols are present in
the rebuilt DLL/debug pair. Wish stubs, source fragments and source globals
remain unchanged; gameplay verification remains deferred.

Some code retains decompiler-style temporaries and control flow. Numeric
constants are retained where a reliable symbolic reconstruction has not been
established. These are provisional reconstructed implementations, not claims of
original formatting, original macros or proven behavioral equivalence.

The seventy-third pass recovers twelve native reanimation helpers in
`functions/functions.c`, under `_2022` compatibility names. These include the
single-card worker and four frontends, the mass worker and two frontends,
Unearth's effect, the upkeep-exile callback, the upkeep-trigger worker and the
Erebos's Titan trigger helper. All twelve legacy bodies and the retained
`empty` callback are independently hashed and remain unchanged. The two
callback interfaces are distinct: a three-argument `CardFunction` effect and a
four-integer before-ETB callback. Nine public declarations accompany the three
private helpers.

`ApplyRecoveryReanimation.java` gives the private mass worker its five physical
arguments: player/card/target-player in EAX/EDX/ECX and test/action on the stack.
DWARF's other three parameters are `DW_OP_GNU_parameter_ref` source references,
not physical arguments. The single-card worker receives eight physical
arguments. The separate signed 32-bit action enum retains all 21 native values,
including the negative extra actions and the new attach-to-reanimator flag.
The existing action enum and APIs remain available to legacy callers.

DWARF proves two 4,000-byte work arrays and their 500-integer row stride.
The recovered mass worker represents each as one 1,000-integer array so its
native cross-row pointer loops stay within the same C array. Two apparent Aura
array initializers were actually the saved EAX and stack-probe return address
before allocation. Only those prologue artifacts are removed, with instruction
and stack-offset evidence. The recovery retains native indexing/controller
quirks, the mixed Aura/non-Aura cleanup arrays, signed negative-action clamp,
and wrappers that test a returned card index against `0x800`.

Five consumers are recovered into their original set files: Entreat the Dead,
Salvaging Station, Claim // Fame, Revival // Revenge, and Calix, Destiny's Hand.
Five prompt copies preserve 348 initialized bytes, including exact native
zero-fill extents. Calix's eight-byte meld-result array shares stack storage
with a 216-byte test object in separate lifetimes; the source gives it a
separate typed local. Both native ID loads use the first meld-result element,
including the second loop iteration, and that behavior is retained.

All 4,245 previous records and C token prefixes remain unchanged. Eleven new
canonical export rows bring the cache to 4,671 rows and 4,613 original names.
All 140 C units, the C++ stub unit, ABI/provenance/compiler-part checks and the
GCC 9.3 PE32 build pass. All fourteen new public symbols are present in the
rebuilt debug data. The reccmp CSV now contains 18,174 pairs, including 4,100
recovery-ledger mappings; the earlier HTML comparison report has not been
regenerated. No game or DLL was executed, and gameplay tests remain deferred.

The seventy-fourth pass finishes the three remaining cached reanimation
consumers. Command the Dreadhorde uses the four-byte integer `info_slot` for
cost accumulation, then its low byte as damage and its second byte as an APNAP
selection split. Native loads/stores at instance offsets `0x38` and `0x39`
prove these views. The source uses unsigned-character aliasing of the existing
integer member, preserving full-word additions and the native ten-selection
cap versus nine-slot resolution loop.

Dance of the Manse's multi-card prompt has two `%d` conversions. Native code
loads the mana bound into EAX and stores it at ESP+12 before placing the card
limit at ESP+8 and calling `sprintf`. Ghidra omitted the second variadic
argument; the scoped reconstruction restores it. The private `dotm_selection`
callback remains in its original unit and its immediate native reference is
checked.

Eerie Ultimatum uses a `2 x 151` snapshot and two variable-length integer arrays
sized by `num_chosen`. Their DWARF upper bounds are references to artificial
variables expressing count minus one; the large reference offsets previously
printed as array dimensions are not capacities. Native stack probes and
allocation instructions confirm four bytes per selected card. The recovered
source restores the hidden graveyard cards, sorts a copied position array,
reanimates each result, marks it untargetable, then clears those marks. The
zero-selection branch still sorts a zero-length GNU VLA. It preserves the
native temporary-Aura validation, original-source flag destination and final
`basic_spell` call after destroying the resolving spell.

The native selection predicate reads four slots from the initially
uninitialized snapshot row, and neither a 151-entry selection guard nor a
failed-reanimation guard is present. These limitations are recorded rather
than repaired during recovery. Prior implementations, shared headers, fragments,
globals, stack/mask registries and the Wish stubs remain unchanged. All 140 C
units, the C++ stub unit, provenance/ABI/compiler-part checks and the GCC 9.3
build pass. The refreshed reccmp CSV has 18,178 pairs and 4,104 ledger mappings;
its earlier HTML report remains unchanged.

The seventy-fifth pass recovers `protection_from_type`,
`protection_from_colorless`, `protection_from_monocolored` and
`cannot_be_target_of_abilities` in `functions/abilities.c`. GCC inlined the
public `get_protection_source` wrapper into these callers, leaving a signed
`attacking_card` guard followed by the private three-register worker call.
The recovery restores the existing public call after checking its source body,
native signed-byte forwarding, guard, private-worker destination and both
four-byte output locals. The guarded wrapper leaves the outputs untouched when
`attacking_card` is negative; no initial values or warning suppressions are
introduced. All strict candidate diagnostics pass.

Tanazir Quandrix's target definition and 151-integer `marked` array have the
same DWARF frame offset in separate lifetimes. The source gives the array a
separate typed local, preserves descending collection into the snapshot and
reverse consumption, and fetches Tanazir's current toughness then power for
every target. This retains effects of earlier pumps during the same loop rather
than caching source P/T once or iterating the changing battlefield directly.
Its ETB branch uses the native source-aware counter API and 84-byte target
interface; five dialog literals are resolved without recovering dialog internals.

The helpers unlock Shanna, Sisay's Legacy. Its opponent-only protection branch
has a separately checked direct native operand to
`cannot_be_target_of_abilities`. All 4,265 previous records and C token prefixes
remain unchanged, as do definitions, earlier interfaces, fragments, globals,
stack/mask registries and dialog stubs. Four declarations are added to the
native-only shared-header block. Source/ABI/provenance/compiler-part checks and
the GCC 9.3 PE32 build pass, including all six new public symbols. The refreshed
reccmp CSV contains 18,185 pairs and 4,111 ledger mappings; the earlier HTML
comparison report is unchanged.

The Ozolith remains deferred: two earlier recovered leaves-play frontends
still expose the legacy test layout. Its migration must preserve compatibility
bodies and reconcile the already accepted Nadier, Nadier's Nightblade and
Soulherder callers with the native 216-byte test API. Those existing native
addresses must not be counted twice.

The seventy-sixth pass upgrades the already accepted `test_leaves_play_mode`
and `test_leaves_play` address records to `_2022` implementations with the
native 216-byte test interface. Both earlier 208-byte source bodies remain
verbatim under their original names and declarations, recorded as two
uncounted compatibility fragments. The helper-address count is unchanged.
Per-unit DWARF verifies the test and target sizes, callback fields and physical
parameters; native call operands bind the mode worker to the recovered test
predicate and the public frontend to that worker. Trigger suppression,
duplication counts, controller filters and arbitrary-card dispatch remain as
in the native code.

Nadier's Nightblade, Nadier, Agent of the Duskenel, and Soulherder migrate by
identifier/type substitutions only. Their complete predecessor records and
reversible body hashes are retained. The earlier Nightblade life-callee and
Nadier token-frontend proofs replay against those predecessors. Soulherder's
216-byte storage union now uses an 84-byte target view and a 216-byte test view,
while its earlier stack proof remains independently reproducible. All three
callers' native API operands are checked. No previously accepted address is
counted again.

The Ozolith is recovered in `cards/ikoria.c` with its native test predicate,
leaves-play callback delivery and source-aware counter movement. Eight native
call/reference sites are checked, including the private `has_counter` predicate
in the same source unit. Its combat query uses the previously verified EXE
boundary. No chooser or dialog internals are added.

All 4,266 other previous records remain unchanged. Earlier C token prefixes
survive after reversing only the three recorded caller migrations; definitions,
earlier interface components, globals, stack/mask registries and Wish stubs
are unchanged. The fragment registry grows from forty to forty-two. All 140 C
units, the C++ stub unit, ABI/provenance/compiler-part checks and the GCC 9.3
PE32 build pass. Verification checks the new card symbol, both migrated native
helper symbols and both retained compatibility symbols. The refreshed reccmp
CSV contains 18,187 pairs and 4,113 ledger mappings; its earlier HTML
report has not been regenerated.

The seventy-seventh pass recovers `new_damage_all_2022` in
`functions/manipulate_and_damage_all.c`, retaining the older implementation
and its 208-byte test API. The native six-argument helper accepts 216-byte
tests, rejects nonpositive damage, and uses distinct creature/planeswalker mode
bits when no test is supplied. With a test, the native predicate decides which
permanents receive damage. Source exclusion, player damage, regeneration/exile
flags, APNAP turn rereads and result accumulation remain as decoded; a negative
damage-object index skips its flag writes but still contributes the damage
amount to the accumulated result. No gameplay corrections are inferred.

Arahbo, Roar of the World, Plague Reaver and Noxious Field have target/test
objects sharing DWARF frame offset -236. Each receives a 216-byte union with
native 84-byte target and 216-byte test members. Fresh parameter/local evidence,
per-unit types and native helper operands validate the views. These three
native unions have their own component; the historical 88-function stack
registry remains unchanged. Arahbo binds the already verified EXE legal-attacker
query through its EAX load/indirect call and uses the recovered declaration and
pump APIs. Plague Reaver retains the private effect callback in its original
unit. Noxious Field uses the native damage helper and the already recovered
84-byte Aura prompt helper.

All 4,272 earlier records and C token prefixes remain unchanged. One native
declaration is added to `manalink.h`; definitions, earlier interfaces,
fragments, globals, stack/mask registries and Wish stubs are unchanged.
One focused export raises the canonical cache to 4,672 rows and 4,614 original
names. Source/ABI/provenance/compiler-part checks and the GCC 9.3 PE32 build
pass, including all four new public symbols. The refreshed reccmp CSV contains
18,191 pairs and 4,118 ledger mappings; its initial HTML report is
unchanged.

Indomitable Creativity remains pending. Its shared target/test object is
accompanied by a dynamic original-target list, and the newly exported native
reveal helper has a second dynamic deck-copy frame. The destruction-tracker
constructor and its callback were also exported; the callback needs packed
halfword/byte reconstruction at instance offset 76. These support drafts are
outside the accepted count and have not replaced their legacy source bodies.

The seventy-eighth pass finishes Indomitable Creativity and three support
functions in their original source units. The public reveal and destruction
constructor use `_2022` names, as does the private destruction callback. All
three earlier support bodies remain unchanged under their legacy names.

The callback's destroyed-count halfword is at instance offset 78; per-player
halfwords are at offsets 196 and 198. Native 16-bit increments and independent
halfword sums are represented with unsigned word masks/addition, avoiding
`short*` aliases of integer fields. The native transfer's two independent byte
additions at offsets 78 and 79 remain independent, including their lack of
carry. Re-entry flags, target compaction, effect merging, original-card
redispatch and cleanup follow the native control flow. The constructor still
creates another tracking effect when the target count reaches eighteen.

The reveal helper's deck-copy VLA uses `count_deck(player)`. DWARF expresses
its upper bound directly as EAX minus one; native count/shift/probe/allocation
instructions confirm four bytes per card. Flag `0x4000` can select the other
deck pointer without changing that allocation size. The helper retains the
16-bit quantity and byte-split bounds, independent movement/free-cast/exile
flags, source/recipient mismatches and two-part return packing. It does not
repair negative added-card handling, result initialization, skipped deck
positions after removals or the final scanned-minus-removed return behavior.
Both native reveal-prompt operands and the engine Done label remain checked.

Creativity's test and target locals share the native frame offset in separate
lifetimes; the C reconstruction gives them separate 216-byte and 84-byte
objects. Its original-target VLA is sized from the target-count byte, with a
DWARF referenced bound of count minus one. Native writes prove zero-based
indices despite Ghidra's generated pointer arithmetic. Player placeholders
remain IDs `0x615a` and `0x6161`; the target-change prompt is recovered from its
read-only bytes. Packed low/high controller counts, original spell branches,
retargeting, destruction accumulation and the unchanged `0x144` reveal flags
are preserved. No built-in dialog implementation is added.

All 4,276 previous records and C token prefixes remain unchanged. Definitions,
earlier interface components, fragments, globals, stack/mask registries and
Wish stubs are unchanged; two native declarations are added to `manalink.h`.
Three focused exports raise the canonical cache to 4,675 rows and 4,617 original
names. All 140 C units, the C++ stub unit, ABI/provenance/compiler-part checks
and the GCC 9.3 PE32 build pass. Verification finds all three new public
symbols and the private callback with local COFF linkage. Three prompt loads
are checked separately in the reconstruction component, outside the generic
literal-argument tally below. Reccmp now has 18,192 pairs and 4,122 ledger
mappings; its earlier HTML report remains unchanged.

## Disciple family (pass 79)

Ana, Ceta, Dega, Necra and Raka Disciple now use the recovered native
`ap_disciples_2022` worker in `cards/apocalypse.c`. Three existing wrappers
are updated in place, retaining their Oracle comments and SHANDALAR calls;
Necra and Raka receive missing definitions. The original five-argument worker
remains unchanged. These are five recovered card implementations, including
three modified existing handlers, rather than five newly introduced cards.

The worker receives player/card/event in EAX/EDX/ECX and one stack color
mask. All five native 25-byte tail wrappers prove their color combinations.
Three distinct 84-byte target definitions and their frame offsets are
checked against DWARF. Ghidra incorrectly hoisted the flying target's
controller preference before its initializer; two native stores prove that
it belongs after initialization in both event paths. The reconstruction
restores that ordering. Seven dialog arguments, including both control
strings and the Prevent damage choice, are recovered from read-only bytes;
the `_rdata` alias is pinned to its actual immediate operand, not the
section base. Activation priorities, mana handling and resolution branches
follow the native worker.

All 4,280 previous records are unchanged. Removing the new definitions and
reversing the three scoped wrapper substitutions restores the previous
source token stream. Headers, prior interfaces, fragments, globals, shared
stack/mask registries and Wish stubs are unchanged. Four new canonical raw
entries raise the cache to 4,679 rows and 4,621 original names. Source/ABI,
provenance, compiler-part and GCC 9.3 PE32 build checks pass. Reccmp now has
18,194 pairs and 4,128 ledger mappings; its earlier HTML report is unchanged.

## Vivien and casting as though a card had flash (pass 80)

Vivien, Champion of the Wilds, its flash activation effect and two private
mana-check callbacks are recovered alongside the native
`can_play_cards_as_though_they_had_flash_2022` helper. The legacy public helper
and its callback remain unchanged. The native helper accepts the 84-byte
target definition; its declaration is added to `manalink.h`, with the usual
recovery-tool mapping for future native callers.

The two same-named native callbacks are distinct. Vivien's callback checks
the card ID directly, while the general flash helper's callback first
converts that ID to an internal ID. Each callback reads a separate private
event variable. DWARF locations, local COFF storage, BSS zero initialization,
all six variable accesses and both callback operands verify those bindings.
The reconstruction gives both callbacks and variables distinct names, with
private linkage in their owning source files.

Vivien's card keeps its native selection fallback and face-down exile
branches, using the already recovered vigilance/reach and look-at-card
callbacks. A Ghidra local used for both a target pointer and an integer count
is split into distinct variables. DWARF records the count as `int`; native
signed comparisons cap it at three before writing the test's 16-bit
`create_minideck` member at offset 194. The 84-byte target and 216-byte test
have independent native frame offsets. The original fallback, controller
choices, prompt text and bottom-of-deck behavior are preserved, including
native failure handling. No dialog internals are added.

All 4,286 previous function records and C token prefixes remain unchanged.
The two private event declarations raise the global registry to 56; fragments,
prior interfaces, shared stack/mask registries and Wish stubs are unchanged.
One new canonical export raises the cache to 4,680 rows and 4,622 original
names. Source/ABI, provenance, compiler-part and GCC 9.3 PE32 build checks
pass. Build verification finds both new public symbols, all three private
callbacks and both private BSS variables. Reccmp now has 18,200 pairs and
4,135 ledger mappings; its earlier HTML report is unchanged. The next
ordinary recovery screen accepts no additional functions automatically.

## Snow predicates and consumers (pass 81)

Zombie Musher and Hallowed Ground are recovered with the native public
`control_a_snow_land_2022` predicate and private `target_is_nonsnow` callback.
The old file-local snow-land helper remains unchanged. Native COFF linkage
proves that the newer shared predicate can be called across compilation
units, resolving Zombie Musher's missing dependency. Its two physical stack
arguments, reverse scan, subtype test and exact untapped-only selector
are preserved. Native block-legality, snow-mana cost and regeneration paths
remain as compiled in the release.

Hallowed Ground uses a distinct 84-byte target definition and a private
five-argument validator. Both native callback operands are checked. The
validator's bit-selected return is reconstructed as a conditional pointer to
`EXE_STR(0x73964c)`, the engine's subtype-error buffer. The complete compare,
mask and return instructions prove that binding. This address lies in the
engine's runtime data region; no on-disk string content is assumed or
invented. Native bounce and activation ordering remain unchanged.

All 4,291 previous records and C token prefixes remain unchanged. One shared
snow-predicate declaration is added to `manalink.h`; earlier interfaces,
fragments, globals, shared stack/mask registries and Wish stubs are unchanged.
The canonical cache remains at 4,680 rows and 4,622 original names. All
source/ABI, provenance, compiler-part and GCC 9.3 PE32 build checks pass,
including the three public symbols and private nonsnow callback. Reccmp
now has 18,203 pairs and 4,139 ledger mappings; its earlier HTML report is
unchanged. The ordinary recovery screen accepts no further implementations.

## Commander consumers (pass 82)

War Room and Hellkite Courser are recovered with their remaining commander
helpers. The native color-identity routine is the private compiler clone
`get_color_identity_of_commander.constprop.0`. Its abstract prototype has two
arguments, but the concrete DWARF parameter fixes `which_commander` to `3`.
Only player survives in EAX. The reconstruction gives this specialization a
one-argument interface named `get_color_identity_of_all_commanders_2022`.
Its native loop scans commander codes one and two; both War Room call sites
load player into EAX and call this exact entry point. The phantom second
argument and its generated dead assignment are removed using scoped proof.
Native color/remainder-text merging and the selected-color slot remain intact.

The public `put_commander_from_command_zone_to_2022` wrapper delegates to the
already recovered private command-zone core with selector zero and a null
result pointer. The native tail wrapper proves the register arguments and
zero stack operand. The qualified public name preserves the older private
Commander 2015 helper with the same original name; that old body is also
hash-checked. The full build caught the initial name collision, and it is
resolved without changing legacy gameplay code. Hellkite Courser uses the
native destination and existing delayed callback after its ETB transition.

War Room keeps its native activation availability cost of four, payment cost
of three, color-count life-cost arguments, mode byte and mana-producer
branches. The separate life adjustment subtracts the Boolean expression
`color_count > 5`; native compare/set/subtract/store instructions verify this
behavior. Its placement after the dialog and human/AI fall-through conditions
are preserved rather than rewritten from intended card behavior.

All 4,295 prior records and C token prefixes remain unchanged. One qualified
wrapper declaration is added to `manalink.h`; fragments, globals, previous
interfaces, stack/mask registries and Wish stubs are unchanged. The cache
remains at 4,680 rows and 4,622 original names. Source/ABI, provenance,
compiler-part and GCC 9.3 PE32 build checks pass, including the two card
symbols and qualified public wrapper. Reccmp now has 18,209 pairs and
4,145 ledger mappings; its earlier HTML report is unchanged. The next
ordinary recovery screen accepts no further implementations automatically.

## Shared public card callbacks (pass 83)

Windscouter, Wall of Junk and Quicksilver Behemoth are recovered with the
public `bounce_at_end_of_combat_2022` effect callback in `cards/prophecy.c`.
DWARF and COFF distinguish its three stack arguments and external linkage
from the private four-argument Stronghold wrapper at `0x212ace0`. Both card
callback operands point to the public effect at `0x25fb3e0`; the separate
Quicksilver-to-Windscouter call is also checked. Qualification is scoped to
those native bindings. The effect preserves its native combat trigger,
oubliette-state guard, bounce and matched end-trigger cleanup.

The public `damage_from_artifact_source_2022` predicate and Rune of
Protection: Artifacts are also recovered. The predicate keeps the native
three-argument callback interface and error returns. It checks the damage
object, player-only recipient, artifact-source type and planeswalker attack
flag. When the source is absent, the saved artifact bit in target slot three
supplies the type fallback. The Rune uses an 84-byte target definition and
two verified predicate operands, retaining its cycling event range, native
activation/prevention flags, validation and damage-amount lookup.

Three legacy bodies remain unchanged: the Stronghold wrapper and its
`wot_effect`, and Antiquities' old private artifact predicate. The Stronghold
native homonym remains outside this batch's recovered count and has an
address-qualified deferral. Native evidence pins its bytes, private linkage
and four-argument interface so it cannot be confused with the public callback.

All 4,299 previous records and C token prefixes remain unchanged. Three public
declarations are added to `manalink.h` for the two callbacks and Windscouter;
previous interfaces, fragments, globals, stack/mask registries and Wish stubs
are unchanged. The canonical cache remains at 4,680 rows and 4,622 original
names. Source/ABI, provenance, compiler-part and GCC 9.3 PE32 build checks pass,
including all six new public symbols. Reccmp now has 18,215 pairs and 4,151
ledger mappings; its earlier HTML report is unchanged. A serial refresh
succeeded after the parallel attempt exhausted memory while reading DWARF.
The ordinary recovery screen accepts no further implementations automatically.

## Native storm family and consumers (pass 84)

Ral, Storm Conduit, Tuvasa the Sunlit and Thousand-Year Storm are recovered
with eight public helpers: the native storm increment routine, six storm
readers and the shared hand-type counter. They use qualified `_2022` names;
the older storm APIs, private hand counter and event collector/reset paths
remain unchanged. Integrating the newer collection modes and reset behavior
into the legacy Rules Engine event path remains downstream work.

Native DWARF, counter operands and mode tests establish these assignments:

| Count | Native counter | Native mode bit |
| --- | --- | --- |
| All spells | Vortex (38) | Always incremented |
| Creature spells | Blood (19) | `0x40` |
| Enchantment spells | Devotion (32) | `0x80` |
| Instants and sorceries | Charge (123) | `0x100` |

The new `storm_mode_2022_t` preserves native color bits two through thirty-two
and gives the three type modes their native values. The older
`storm_mode_t` and `STORM_CREATURE == 1` remain intact. Compile-time assertions
check the new mode width, four counter IDs, the two rules-card slots and every
member offset in the existing 152-byte `rules_engine_globals_t` layout.
The native rules-card array is eight initialized bytes containing two `-1`
values; the rebuilt DLL's private data symbol and contents are verified too.

The native increment routine retains its source-aware counter additions,
silent-counter scope, total-spells byte and per-color trap increments. The
existing trap handlers are reused only for this routine's fixed add-one
operations and the color reader's five nibble reads. Both native callee
entries, all six register/stack argument setups, backing offsets, source
body hashes and byte/nibble saturation domains are checked. This does not
count the legacy trap handlers as newly recovered implementations.
The color reader has an explicit unsigned range guard matching the native
comparison, including rejection of colorless and noncolor values.

Ral uses an 84-byte target definition, the hand-type helper for dialog
priority and the already recovered Doublecast callback. Tuvasa queries the
native enchantment count and preserves its first-spell test and power/toughness
flow. Thousand-Year Storm retains the native live count query in each copy
iteration and uses the recovered spell-copy API. Eleven legacy bodies remain
verbatim, including collection/reset, trap support and the private hand
counter in Mercadian Masques.

All 4,305 previous records and C token prefixes remain unchanged. Eight public
declarations and a separate mode domain are added; existing globals, fragments,
earlier interfaces, shared stack/mask registries and Wish stubs are unchanged.
Five focused exports raise the canonical cache to 4,685 rows and 4,627 original
names. Source/ABI, provenance, compiler-part and GCC 9.3 PE32 build checks
pass, including all eleven new public symbols and the retained rules-card
state. Native event collection/reset integration and gameplay remain pending.
DWARF locates both missing routines as inline instances inside
`card_rules_engine` at `0x26c9140`; their range-list offsets are recorded for
the next reconstruction step.

Reccmp now has 18,220 pairs and 4,161 ledger mappings; its earlier HTML report
is unchanged. GCC splits the rebuilt hand-type helper into a public entry
and a private `.part.0` body sharing its DWARF name, so that one function is
conservatively excluded from name-based comparison. Its native source body
and public rebuilt symbol still pass the recovery/build checks. The ordinary
recovery screen accepts no further implementations automatically.

## Inlined storm collection and reset (pass 85)

The native `keep_storm_count` and `reset_storm_count` have no standalone
machine entries. Concrete DWARF inline instances lie inside
`card_rules_engine` at `0x26c9140`. The collector has eighteen nonempty ranges
covering 1,307 bytes; reset is nested within it and has thirteen ranges
covering 855 bytes. These overlap and are not added together as independent
machine coverage. Both routines are reconstructed as private `_2022`
helpers in `functions/rules_engine.c`, with dedicated source-fragment markers
and a pinned copy of the full dispatcher export. They add two source
fragments, bringing that registry to 44, and add no standalone function count.

The collector retains the human-only guard, spell-trigger conditions,
copied-spell exclusion and native evaluation order. It snapshots the card's
type byte, counts noncreature spells in native trap `0x408`, combines the
three newer type modes and color bits, handles Peer/Reach traps and calls
the recovered increment routine. Reset runs on `EVENT_CAN_SKIP_TURN`, rather
than the legacy cleanup path. Its 84-byte target local is distinct from the
parent dispatcher's reused diagnostic buffer; DWARF and the subtype store
prove its frame location and required-subtype offset.

Reset preserves both native paths. Without a Werewolf target, it visits the
two recorded Rules Engine slots. With one, it snapshots both players' Vortex
counts before clearing, scans all in-play slots, marks previously unstamped
Werewolves and writes their two history values. The marker write precedes
the history writes as in native code. Each encountered native Rules Engine
then has five color traps and all four storm counter kinds cleared. All ten
zero-set trap calls and eight source-aware removals are checked, including
their source player/card and the native loop behavior.

Integration must account for two identifier changes. Native
`TRAP_NONCREATURES_PLAYED` is `0x408`; the old header uses
that slot for `TRAP_COT_FLAG`, which moved to `0x40b` in 2022. Native
`CARD_ID_RULES_ENGINE` is 24922 (`0x615a`), while the retained header gives it
19800. The reconstructed routines qualify their native constants locally.
Existing dispatcher calls, legacy routines and headers remain unchanged;
event-path, trap-domain and Rules Engine ID integration remain downstream.

All 4,316 existing function records and prior C source prefixes survive
unchanged. Prior fragments, interfaces, globals, shared stack/mask registries
and Wish stubs are unchanged. Source/ABI, provenance, compiler-part and
GCC 9.3 PE32 build checks pass. The standalone ledger remains at 4,316 and
the canonical export cache remains at 4,685 rows and 4,627 original names;
the new parent export is pinned separately as fragment support. Reccmp
remains at 18,220 pairs and 4,161 ledger mappings, with fresh input hashes;
the earlier HTML report remains unchanged.

## Artifact mana and shared target predicates (pass 86)

Seven native public entries are recovered in their original source units:
Firemind Vessel, Stream of Acid and Prismatic Circle, plus the one-color and
distinct-color artifact-mana frontends and two shared targeting callbacks.
The four helpers use qualified `_2022` names and public declarations; the
three older same-named helper bodies remain unchanged.

Both artifact frontends retain native tap, animated-sickness and mana checks,
sacrifice eligibility, AI costs, count-mana declarations and the
`spell_fizzled != 1` sacrifice gate. The one-color frontend's count query
really calls its own entry; instruction evidence verifies this recursion.
The distinct-color frontend reuses the already accepted private worker at
`0x2670e00`. DWARF and both call sites establish five physical parameters:
player, card, available colors, amount and prompt. Its abstract sixth `mca`
parameter is specialized to one and has no physical argument. Firemind
Vessel retains its humiliation-aware enters-tapped behavior and requests two
different colors through this frontend.

Stream of Acid's predicate accepts lands first, then creatures outside the
caster's sleighted black mask. Both native callback references and the
sleight-aware prompt are retained, along with resolution's return through
the generic spell handler. Prismatic Circle's callback distinguishes a live
damage source's current color from the damage object's saved initial color,
preserves the planeswalker rejection and uses the engine-owned error buffer
at `0x739060`. No on-disk message is invented for that runtime pointer.
Its chosen-color shift uses an explicit unsigned 32-bit value with the native
five-bit shift-count mask. Both card target locals are verified as 84 bytes.

All 4,316 earlier records and C token prefixes survive unchanged. Earlier
interfaces, fragments, globals, shared stack/mask registries and the two Wish
stubs are unchanged. The ledger reaches 4,323 implementations: 3,158 `card_*`
functions and 1,165 helpers. Registration coverage is 3,144 symbols, including
3,059 with a CODED record. The canonical cache contains 4,686 rows and 4,628
original names. Source/ABI, provenance, compiler-part and GCC 9.3 PE32 build
checks pass, including all seven new public symbols. Reccmp metadata is refreshed
against this build with 18,225 pairs and 4,169 ledger mappings; the earlier
HTML report is unchanged. Gameplay integration remains downstream.

## Native Embalm specialization and consumers (pass 87)

The private native `_embalm.constprop.0` at `0x22d74b0` is recovered as
`embalm_2022_impl` in `cards/amonkhet.c`. Concrete DWARF puts player, card and
event in EAX/EDX/ECX and places card ID, generic cost and blue/green/red/white
costs in six stack words. Black mana is a constant zero with no location.
Applying all ten abstract source parameters had shifted the costs and created
phantom register/stack arguments in its consumers. A verified nine-parameter
Ghidra signature removes those artifacts before fresh export. All six
card-ID/cost stack constants in each of the ten callers are independently
checked against native instruction operands.

The worker preserves graveyard query/payment/resolution events, sorcery-speed
eligibility, zero black mana, the `spell_fizzled != 1` gate and exile-payment
result. Its 76-byte token local is verified in DWARF at frame offset -96.
The native combined store writes white color at token offset 40 and
`no_sleight=1` at offset 41; a second store sets the embalmed/eternalized flag
at offset 72. This native direct-flag path is recovered as written. The older
ten-argument Embalm helper and its callback remain unchanged.

Ten handlers now use the recovered worker: Trueheart Duelist, Unwavering
Initiate, Aven Initiate, Labyrinth Guardian, Tah-Crop Skirmisher, Vizier of
Many Faces, Heart-Piercer Manticore, Honored Hydra, Aven Wind Guide and Temmet,
Vizier of Naktamun. Existing native calls and callback references are checked,
including Temmet's token-only predicate and beginning-of-combat helper.
Manticore uses the native 216-byte sacrifice test and 84-byte target object;
its packed sacrifice result, power snapshot, sacrifice and damage sequence
are retained. Temmet also uses the native 84-byte target definition, local
selected target and temporary pump/unblockability call.

All 4,323 previous records and C token prefixes survive unchanged. Headers,
earlier interfaces, source fragments, globals, shared stack/mask registries
and Wish stubs are unchanged. The ledger reaches 4,334 functions: 3,168 named
`card_*` and 1,166 helpers. There are 556 private functions, including 547
private helpers, and 619 public helpers. Registration coverage reaches 3,154
symbols, including 3,069 with a CODED record. The canonical cache stays at
4,686 rows and 4,628 original names. Source/ABI, provenance, compiler-part and
GCC 9.3 PE32 build checks pass. All ten new public card symbols and the private
worker's local COFF linkage are verified. Reccmp metadata is refreshed with
18,235 pairs and 4,180 ledger mappings against this build; its earlier HTML
report is unchanged. Gameplay integration remains downstream.

## Memorial lands and native resolver interfaces (pass 88)

The native private `_dominaria_memorial.isra.0` at `0x22ca5c0` has fourteen
physical parameters: three entry registers and eleven stack words. Its
abstract source `ai` parameter exists only as a GNU parameter-reference
expression, with no physical entry storage. Applying all fifteen abstract
parameters had shifted the target, prompt and resolver callback. The verified
Ghidra signature now omits that nonphysical parameter and produces a typed
callback dispatch with the original player, card and target pointer. Native
instruction evidence also establishes the AI priority as the player's land
count minus six.

The shared helper and Memorial to Folly, Genius, Glory, Unity and War are
recovered in `cards/dominaria.c`. They retain the native mana/ability dialog,
extra mana eligibility check, costs, tap-before-payment sequence, graveyard
selection, target selection, sacrifice/fizzle gates and untap cleanup paths.
Two conditional dialog-control strings are recovered from their exact
read-only bytes and immediate pointer stores; the ordinary dialog API remains
an external call. War's target initialization uses an explicitly unsigned event
range matching the native `cmp`/`jbe` pair. Its local target object is 84 bytes;
the helper's local creature test is 216 bytes.

The five already accepted resolvers now take native target pointers. Unity
also uses the native test initializer and global tutor; War validates through
the native targeting API. These upgrades are not counted as new functions.
Each resolver retains its complete predecessor record and an exact reversible
source view. Earlier scalar-call evidence for Glory is replayed against that
view, so its previously verified token API binding remains independently
checked. The resolvers retain their native control flow and linkage.

The ledger reaches 4,340 functions: 3,173 named `card_*` and 1,167 helpers.
Of the 4,334 previous records, 4,329 remain unchanged and five have verified
interface migrations. Reversing those five token substitutions recovers all
prior source prefixes. Headers, earlier interface components, source fragments,
globals, shared stack/mask registries and Wish stubs are unchanged. There are
557 private functions, including 548 private helpers, and 619 public helpers.
Registration coverage reaches 3,159 symbols, including 3,074 with a CODED
record. The canonical cache remains at 4,686 rows and 4,628 original names.
Source/ABI, provenance, compiler-part and GCC 9.3 PE32 build checks pass,
including the five new public card symbols and all six private worker/resolver
symbols. Reccmp metadata is refreshed against this build with 18,246 pairs and
4,191 ledger mappings; the earlier HTML report is unchanged. Gameplay
integration remains downstream.

## Native spell-cost payment and Improvise (pass 89)

`charge_spell_cost_2022` is recovered at `0x26af860` in
`functions/engine.c`. Its three native stack parameters are card data, player
and card. DWARF and compile-time assertions establish the 72-byte card-data
layout, type byte at offset 40 and X-cost byte at offset 44. Six signed EXE
cost bytes feed 32-bit mana words; native operand algebra establishes the
32-byte mana-pool row and the local eight-int snapshot at frame offset -52.
Typed Ghidra EXE boundaries restore four arguments to `0x4302c0`, two to
`0x4301a0` and one pointer to `0x430260`. Their actual EXE bodies and DLL
materializations are pinned in the evidence.

The native payment routine contains unusual bookkeeping that is preserved
and annotated in source. It copies seven 32-bit mana-paid words into the
snapshot region but subsequently reads eight signed bytes from that region.
Its summary-byte pointer advances by player with a one-byte stride, while
the integer mana pool uses the separate 32-byte row stride. On rollback,
the native code sign-extends the snapshot's first byte, passes that integer
word as a pointer, and calls the EXE routine that dereferences a seven-int
array. The reconstruction preserves that exact argument. AI/X selection,
maximum restoration, byte wrap behavior, fizzle handling and native
return-register values are retained. Runtime integration must review these
observed paths; the build checks do not execute them.

The private `cast_spell_with_improvise_2022_impl` and public `improvise_2022`
are recovered in `cards/aether_revolt.c`. Native instructions put the allowed
controller store after each of two target initializers; the decompiler had
hoisted one store before the branch. Both stores are restored to initialized
84-byte targets. The worker retains its native fifteen-int artifact array,
selection/cancellation, temporary untargetability, cleanup, mana/X payment,
tapping and flag behavior. The already accepted cost modifier is migrated
to the native target initializer and availability API through reversible token
substitutions, with its exact predecessor record retained. Older Improvise
helper bodies remain unchanged.

Inspiring Statuary retains native global-cost and cast-event routing through
those helpers. Skyclave Shade preserves its kicker/counter behavior,
land-play/current-turn graveyard eligibility and payment result through the
recovered spell-cost routine. All direct native calls and callback references
in the six affected entries are checked.

The ledger reaches 4,345 implementations: 3,175 named `card_*` and 1,170
helpers. Of the 4,340 prior records, 4,339 remain unchanged and one is migrated
with exact reversible history. Restoring that migration recovers every prior
C token prefix. Two public declarations are added; earlier interface components,
fragments, globals, shared stack/mask registries and Wish stubs are unchanged.
There are 558 private functions, including 549 private helpers, and 621 public
helpers. Registration coverage reaches 3,161 symbols, including 3,076 with a
CODED record. The canonical cache stays at 4,686 rows and 4,628 original names.
Source/ABI, provenance, compiler-part and GCC 9.3 PE32 build checks pass,
including four new public symbols and both private Improvise helpers. GCC
specializes the rebuilt casting worker as `.constprop.0`; its local executable
COFF symbol is explicitly verified. Reccmp metadata is refreshed against this
build with 18,253 pairs and 4,198 ledger mappings; the earlier HTML report is
unchanged. Gameplay integration remains downstream.

## Expertise specialization and recovered helper callers (pass 90)

The native private `_av_free_spell.isra.0.part.0` at `0x20bc560` takes only
player in EAX and the mana-value limit in EDX. Its abstract source `card`
parameter has no physical location. The earlier three-parameter Ghidra import
had treated the limit as a card slot and invented a third argument. Applying
the concrete two-register signature yields the recovered private
`av_free_spell_2022(player, max_cmc)` in `cards/aether_revolt.c`.

The native helper uses a 216-byte test to select a nonland card from hand
with mana value below `max_cmc + 1`, then casts the selected hand slot for
free. DWARF and native word stores establish its test object, flags and cost
bound. The four native Expertise callers already guard their positive hand
counts; the native part has no additional hand-count guard. The older source
helper and both legacy Improvise bodies remain unchanged.

Kari Zev's Expertise, Rishkar's Expertise, Sram's Expertise and Yahenni's
Expertise now pass the native limits 2, 5, 3 and 3 to the new helper. Each
EDX constant and EAX setup is pinned with its direct call. Four fresh caller
exports are retained separately from their original canonical exports and
confirm the two-argument calls. The unnecessary third arguments are removed;
other source operations remain unchanged.

Eleven accepted handlers now call `improvise_2022`: Enraged Giant, Fen Hauler,
Foundry Assembler, Freejam Regent, Herald of Anguish, Maverick Thopterist,
Metallic Rebuke, Reverse Engineer, Sly Requisitioner, Sweatworks Brawler and
Whir of Invention. Fourteen native Improvise calls/tail jumps and the four
Expertise calls are verified, including multiple event paths and Foundry
Assembler's tail jump. No broad name-only replacement changes legacy callers.

All fifteen changed records retain complete predecessor records and exact
reversible source substitutions. Earlier native pump, enters-play and death
trigger evidence is replayed against those historical views. Of 4,345 prior
records, 4,330 remain unchanged and fifteen are migrated; these migrations
add no function count. Restoring them and removing the scoped private forward
declaration recovers all previous C token prefixes. Headers, earlier interface
components, fragments, globals, shared stack/mask registries and Wish stubs
remain unchanged.

The ledger reaches 4,346 functions: 3,175 named `card_*` and 1,171 helpers.
There are 559 private functions, including 550 private helpers, and 621 public
helpers. Registration coverage stays at 3,161 symbols, including 3,076 with a
CODED record. The canonical cache remains at 4,686 rows and 4,628 original
names. Source/ABI, provenance, compiler-part and GCC 9.3 PE32 build checks
pass, including the new worker's private executable COFF symbol. Reccmp
metadata is refreshed against this build with 18,253 pairs and 4,199 ledger
mappings; the earlier HTML report is unchanged. Gameplay integration remains
downstream.

## Saheeli's Directive variable-length reveal buffer (pass 91)

`card_saheelis_directive` at `0x2564cc0` is recovered in
`cards/commander_2018.c`. Eight contiguous native ranges cover all 1,028 bytes.
Concrete DWARF gives `deck_copy` an integer element type and a
`DW_FORM_exprloc` upper bound of `DW_OP_breg2: -1`, establishing an EDX-counted
`int[max]` array. The native code computes `max * 4`, probes/subtracts that
allocation, uses the resulting buffer after its 32-byte outgoing call area,
and restores the saved stack before destroying the source. The range proof
retains the complete routing, allocation, selection and cleanup paths.

Resolution reads the card's X value after querying deck length and uses the
signed minimum as the reveal count. A zero count goes directly to the common
generic-spell call and skips explicit self-destruction. With a nonzero count,
the native Cage query precedes revealing the cards; a nonzero Cage result
skips extraction and selection, then destroys the source. These native paths
are preserved. On the selection path, the revealed cards are copied out of
the deck, a 100-byte prompt is formatted and a native 216-byte test is built.
The lost fourth `scnprintf` argument is recovered from its ESP+12 store, and
the prompt's exact read-only bytes are checked against its immediate operand.
The prompt and mana-value filter use the initial reveal count, including when
the available deck is shorter than X.

All eight selector arguments are recovered from the dynamic call frame.
Each selected artifact retains the ignored creature-type query, hand insertion,
played-from-deck flag and put-into-play sequence. No guard is invented for a
failed hand insertion. The selected row is removed by left compaction, and
the remaining count drives the next selection. Cancelling restores the
remaining IDs to the top in ascending buffer order, then mills that count;
selecting every row leaves no remainder to mill. The buffer's lexical scope
matches native stack restoration before source destruction.

Casting and fallback events use the recovered native Improvise frontend.
The matching cast path calls Improvise and the X-spell handler, checks fizzle,
and preserves the signed AI adjustment: -48 through X=5 and twelve times X
above that. Nonmatching casts and other fallback events return through
Improvise; can-cast, target-change and resolve paths retain their native
generic-spell routing. Three native Improvise calls and two generic-spell calls
are verified alongside the other recovered native helper calls.

All 4,346 previous records and C token prefixes remain unchanged. Headers,
earlier interfaces, fragments, globals, shared stack/mask registries and Wish
stubs are unchanged. The ledger reaches 4,347 functions: 3,176 named `card_*`
and 1,171 helpers. Registration coverage reaches 3,162 symbols, including
3,077 with a CODED record. The canonical cache remains at 4,686 rows and 4,628
original names. Source/ABI, provenance, compiler-part and GCC 9.3 PE32 build
checks pass, including the new public card symbol. Reccmp metadata is refreshed
against this build with 18,254 pairs and 4,200 ledger mappings; the earlier
HTML report is unchanged. Gameplay integration remains downstream.

## Escape and shared-helper consumer batch (pass 92)

Ten implementations are recovered together in their original source units:
Loathsome Chimera, Ox of Agonas, Underworld Charger, Voracious Typhon, Woe
Strider, Renata, Called to the Hunt and Fugitive Druid, with three shared
helpers. The complete native bodies cover 1,847 bytes.

Concrete DWARF specializes `escape_with_counters` to nine physical arguments:
player/card/event in EAX/EDX/ECX and six stack words. Blue and white mana are
constant zero without physical locations. Applying that interface before
export removes invented caller arguments; all thirty cost/exile/counter
constants are checked against native stack stores. The helper calls the
accepted private Escape worker directly and applies entry counters only when
the native Escape flag is present. Ox's discard/draw, Charger's blocking
restriction and Woe Strider's Goat/sacrifice/scry paths are retained, including
its unsigned event-range gate.

`test_enters_the_battlefield_with_counters_2022` retains the affected-player
guard, native 216-byte test API and direct counter-entry tail call. Renata's
other-creature filter, devotion power and static-ability gate are preserved.
The subtype-target trigger helper checks spell-cast events, target/controller,
land/effect exclusions and subtype, preserving its optional/mandatory trigger
mode and unsigned trigger-event range. Its eight-byte native BSS result is
recovered as a private `target_t`, allowing Fugitive Druid's Aura-trigger draw.
The existing Morningtide helper and earlier Escape bodies remain unchanged.

All 4,347 prior function records and C token prefixes remain unchanged. The
ledger reaches **4,357 implementations: 3,183 `card_*` and 1,174 helpers**.
There are 560 private functions, including 551 private helpers, and 623 public
helpers. Registration coverage reaches 3,169 symbols, including 3,084 with a
CODED record. The raw cache remains at 4,686 rows and 4,628 original names.
One combined source/ABI, provenance, compiler-part and GCC 9.3 PE32 build
passes; all nine public additions, the private Escape worker and private BSS
result are verified. Reccmp metadata is refreshed against this build with
18,263 pairs and 4,210 ledger mappings; the earlier HTML report remains unchanged.
Gameplay validation remains downstream.

## Combat-AI accumulator consumer batch (pass 93)

The native combat initializer identifies three EXE-owned 32-bit lvalues:

| Binding | Address | Native role |
| --- | --- | --- |
| `check_abilities_power_2022` | `0x739b50` | Power estimate |
| `check_abilities_toughness_2022` | `0x7a371c` | Toughness estimate |
| `check_abilities_keywords_2022` | `0x78fef0` | Keyword estimate |

Five checked machine ranges in `setup_ai_combat_data` seed these slots from
`EVENT_POWER`, `EVENT_TOUGHNESS` and `EVENT_ABILITIES`, dispatch
`EVENT_CHECK_ABILITIES`, then accumulate the results into the per-card combat
record. The initializer is supporting machine evidence and remains unrecovered.
The bindings use existing `EXE_DWORD` lvalues without adding DLL-owned storage.
The old power-address comment is corrected from `0x789b50` to `0x739b50` to
match the native operand; no existing source behavior changes with that comment.

These bindings recover **28 handlers and two private callbacks together**:
Rhet-Crop Spearmaster, Elvish Berserker, Goblin Snowman, Soul Kiss, Urborg
Phantom, Ferocity, Brushwagg, Leering Gargoyle, Phyrexian Prowler, Norwood
Warrior, Town Sentry, Zhao Zilong, Tiger General, Fault Riders, Glittering Lion,
Glittering Lynx, Crown of Flames, Flailing Drake, Flowstone Giant, Flowstone
Salamander, Tahngarth's Rage, Mantis Engine, Retaliation, Kyscu Drake, Mob
Mentality, Raging Gorilla, Dwarven Berserker, Avizoa and Nature's Kiss.
Glittering Lion/Lynx retain their private activation callbacks.

The full bodies cover 17,467 native bytes. All 95 accumulator memory operands
are checked for exact address and four-byte width. Native call/callback targets,
216-byte tests, 84-byte targets and both unsigned Aura event-range guards are
verified. Existing oddities remain intact, including AI-only estimates, unused
queries, Glittering callback scan limits/flag calculations and Raging Gorilla's
power-only AI estimate. No gameplay changes are inferred from printed card text.

All 4,357 prior records and C token prefixes remain unchanged. The ledger
reaches **4,387 implementations: 3,211 `card_*` functions and 1,176 helpers**,
with 562 private functions (553 private helpers) and 623 public helpers.
Registration coverage reaches 3,197 symbols, including 3,112 with a CODED
record. The raw cache remains at 4,686 rows and 4,628 original names.
Source/ABI, provenance, compiler-part and GCC 9.3 PE32 build checks pass,
including all 28 new public symbols and both private callbacks. Two preexisting
Invasion format-extra-argument diagnostics are suppressed only around the old
prefix in the scratch compilation; new bodies retain strict format checks and
the actual source remains unchanged. Reccmp metadata is refreshed against this
build with 18,297 pairs and 4,244 ledger mappings; the earlier HTML report is
unchanged. Gameplay validation remains downstream.

## Spectacle, Amplify and native filtering batch (pass 94)

Fifteen implementations are recovered together in four original source units.
The handlers are Drill Bit, Light Up the Stage, Rix Maadi Reveler, Skewer the
Critics, Spawn of Mayhem, Spikewheel Acrobat, Aven Warhawk and Zegana, Utopian
Speaker. Seven helpers provide specialized Spectacle and Amplify workers,
native hand reveal, counter-based boosts, coercion, its private discard worker
and filtered top-deck exile. The complete native bodies cover 6,065 bytes.

Concrete DWARF gives Spectacle six physical arguments: three registers and
three stack words for colorless, black and red mana. Blue, green and white are
constant zero without physical locations. Eight call/tail frames verify all
24 caller cost constants, including the EBP-relative incoming slots used by
tail calls. Native life-loss trap, payment choice, free/token gates, fizzle,
return values and `info_slot` bit operations remain intact. Spawn of Mayhem's
separate upkeep-event branches and Rix Maadi Reveler's flag reset are preserved.

Amplify's part uses player/card/amount in EAX/EDX/ECX. Its abstract `event`
parameter has a GNU parameter reference without a physical argument; Aven's
resolve-spell guard and ECX amount one are verified in native instructions.
The formatted prompt uses four actual words and one `%s`; the invented fifth
argument is removed with the specialized snprintf wrapper and caller stores
checked. The existing subtype callback and native 216-byte test are retained.

The hand-reveal helper has two native `int[100]` arrays and separate manual/auto
paths. Coercion has two `int[50]` arrays and two 216-byte tests. Their DWARF
stack offsets identify a decompiler end pointer formed from an adjacent array;
reconstruction expresses that same address as `ai_hand + count - 1`, keeping C
pointer arithmetic within its owning array. The 28-byte coercion definition and
all seven member offsets are verified against DWARF and checked at compile
time. Native cancellation loops, hand compaction, chooser behavior and return
values remain intact. Filtered exile preserves the target-player deck, optional
test, source-name prompt and last-effect return. The boost helper retains its
unsigned three-event gate and native special-ability call.

All 4,387 prior records, six legacy helper bodies and C token prefixes remain
unchanged. Earlier fragments, globals, interfaces and dialog stubs are retained.
The ledger reaches **4,402 implementations: 3,219 `card_*` functions and 1,183
helpers**, with 565 private functions (556 private helpers) and 627 public
helpers. Registration coverage reaches 3,205 symbols, including 3,120 with a
CODED record. Four fresh helper exports raise the raw cache to 4,690 rows and
4,632 original names. Source/ABI, provenance, compiler-part and GCC 9.3 PE32
build checks pass. All twelve public additions and two private standalone
workers have symbols. GCC inlines Amplify into Aven Warhawk: its 213-byte
concrete DWARF inline range, amount-one location expression, abstract origin,
parent bounds and eight callees are verified instead of requiring a standalone
symbol. Reccmp metadata is refreshed with 18,306 pairs and 4,259 ledger
mappings. Fourteen additions have explicit CSV mappings; the inlined Amplify
worker has no standalone mapping. The earlier HTML report remains unchanged.
Gameplay validation remains downstream.

## Graveyard, sustained-pump and target-consumer batch (pass 95)

Thirteen implementations are recovered together in their original source units.
The seven handlers are Thrilling Encore, Cry of the Carnarium, Flowstone Armor,
Endoskeleton, Timecrafting, Wandering Eye and Thran Temporal Gateway. Six helpers
supply the current-turn deadbox scan, graveyard ID lookup, sustained-pump
constructor/lifetime callback and two Suspend/time-counter predicates. The
complete native bodies cover 3,862 bytes.

The deadbox scan walks 36 DWORD cells between instance offsets `0x74` and
`0x104`. Reconstruction indexes the first eighteen `target_t` pairs directly,
using their player/card fields, preserving four-byte steps, sign/type checks,
optional planeswalker exclusion, every reload and the increment-before-call
order. Concrete DWARF verifies the 300-byte instance, nineteen-element target
array and eight-byte pair layout. The helper keeps the separate acting-player
and target-player parameters. Its ID lookup searches the selected graveyard
backward and preserves native reanimation, return-to-hand, leave-in-place and
exile-only modes. Cry's APNAP passes and existing private end-step callback are
retained; neither native handler is rewritten from printed card text.

The pump constructor uses the verified native 48-byte definition and creates
its private lifetime callback. The callback uses native leaves-play notification
and optional untapped-source removal, exiling both effects in the native order.
Three unsigned activation-event gates are restored in Flowstone Armor,
Endoskeleton and Thran Temporal Gateway. Existing target preferences, costs and
source/target references remain intact.

Both target predicates retain their original Time Spiral source unit and old
static bodies. Their qualified variants receive shared C declarations for
Timecrafting's native cross-unit callback references, despite the native COFF
symbols having private linkage. The localized EXE-owned error pointer remains
`EXE_STR(0x728f6c)`; no on-disk message is invented. Wandering Eye sets the native
`PB_HAND_REVEALED` bit in both existing player-bit array elements. Gateway's
alignment-dependent copy becomes the exact 32-byte prompt and eighteen zero
bytes in a 50-byte buffer, followed by the native 216-byte test and historic
predicate. The original wording, `Select an historical permanent.`, is retained.

All 4,402 prior records, six legacy helper bodies, C token prefixes, earlier
interfaces, fragments, globals and dialog stubs remain unchanged. The ledger
reaches **4,415 implementations: 3,226 `card_*` functions and 1,189 helpers**,
with 566 private functions (557 private helpers) and 632 public helpers.
Registration coverage reaches 3,212 symbols, including 3,127 with a CODED
record. Three fresh helper exports raise the raw cache to 4,693 rows and 4,635
original names. All 141 C units pass source/ABI checks; provenance,
compiler-part and GCC 9.3 PE32 build checks pass, including twelve public symbols
and the private pump callback. Reccmp metadata is refreshed with 18,316 pairs
and 4,273 ledger mappings. Twelve additions have explicit CSV mappings; the
base Suspend predicate has a public symbol plus a private compiler-outlined
`.part.0` body, so the current strict DWARF name matcher leaves it unmapped.
The earlier HTML report remains unchanged. Gameplay validation remains downstream.

## Packed-field, filtering and Overload batch (pass 96)

Fifteen implementations are recovered together in their original source files:
eleven handlers and four shared helpers covering 8,893 native bytes. The cards
are Zurzoth, Chaos Rider; Search for Azcanta; Rampaging Ferocidon; Cavalier of
Gales; Secret Salvage; Umori, the Collector; Vivien, Monsters' Advocate;
Incubation Druid; Winds of Abandon; Break the Ice; and Damn. The helper variants
restore Devil creation, Overload eligibility/payment and creature mana production
while retaining all four older helper bodies.

Concrete DWARF verifies the 300-byte card instance and its four-byte anonymous
union at offset 56. Incubation Druid retains separate byte-zero selection and
byte-one activation state; Overload payment temporarily saves, clears and
restores the entire word around the normal-cast eligibility query. Three
Overload consumers keep the native bit-two test. The mana helper retains its
unsigned two-event rating gate and calls the previously recovered seven-argument
worker. These implementations preserve the native decisions and return paths.

Cavalier's native 216-byte test and later graveyard position share a stack
address. Reconstruction gives the two nonoverlapping phases distinct typed
locals rather than reading a test buffer as an integer. Umori's seven-entry
initializer is verified byte for byte as `{2, 4, 8, 48, 64, 256, 9}`; its native
random-choice bound and scan expressions remain unchanged. Three Secret Salvage
branches copy the same 32 bytes, including the terminating NUL, and clear the
remaining 68 bytes of the prompt buffer. Its card-name address is expressed
through the verified 72-byte card-data record and name member at offset one.

Vivien retains its two previously recovered private callbacks and packs a
one-counter quantity in the low word with the chosen counter ID in the high
word of the 76-byte token definition's action argument at offset 52. Ferocidon's
life-gain locks use the existing two-element player-bit array and the native
`PB_CANT_GAIN_LIFE` value. Azcanta keeps the accepted deck-view boundary's
32-bit pointer representation for its final button-label argument. All 71
calls/tail jumps and six callback references to accepted or new implementations
are checked against native instruction operands and source names.

All 4,415 prior records, C token prefixes, four legacy helper bodies, earlier
interfaces, globals, fragments and dialog stubs remain unchanged. The ledger
reaches **4,430 implementations: 3,237 `card_*` functions and 1,193 helpers**,
including 566 private functions (557 private helpers) and 636 public helpers.
Three fresh raw exports bring the canonical cache to 4,696 rows and 4,638
original names. Return to Ravnica becomes the 142nd checked C unit.
All 142 C units pass source/ABI checks, and provenance, compiler-part and GCC
9.3 PE32 build checks pass with all fifteen new public symbols verified.
Registration coverage reaches 3,223 symbols, including 3,138 with a CODED
record. Reccmp metadata is refreshed with 18,331 pairs and 4,292 ledger mappings;
all fifteen additions have explicit CSV mappings. The earlier HTML report is
unchanged. Gameplay validation remains downstream.

## Permanent-copy, exile-play and field-consumer batch (pass 97)

Fifteen implementations, covering 10,428 native bytes, are recovered in their
original source units. Ten handlers are Kess, Dissident Mage; Valakut
Exploration; Echoing Equation; Kaervek's Purge; Hidden Retreat; Valakut
Awakening; Hall of Gemstone; Vectis Gloves; Rise and Shine; and Mirrorwing
Dragon. Five helpers supply the permanent-copy constructor/private lifetime
callback, token-characteristic copy and exile-play constructor/callback.
The native variants use distinct names; all five older helper bodies remain.

The permanent-copy family preserves repeat-effect lookup and removal, token
characteristic forwarding, optional artifact type, temporary-token flags,
copyable special information, source/target references and the sequence of
characteristic recalculations. Its private callback retains native static
linkage. Both the constructor and callback use the existing verified native
extension flag words. Native enum evidence gives the missing legend-rule mask
as `0x01000000`; the callback retains the native call to remove that mask from
the first flag word after checking the third word. The constructor's unusual
scan comparisons and special-information source are preserved as decoded.
Echoing Equation marks creatures before applying the recovered constructor.

The exile-play family keeps face-down viewing, mana payment modes, land and
spell eligibility, turn/cleanup lifetimes, the reentrancy sentinel, optional
requirements and ownership handling. Its dialog alias is resolved from the
actual native pointer operand to `Look at card`; the deck viewer retains the
live localized EXE `OK` label. Its activation-event interval is explicitly
unsigned. Valakut Exploration constructs the native effect, identifies it by
its callback and retained tag, and preserves the end-step scan/damage count.
Earlier accepted callers outside this batch retain their earlier helper
bindings; broader runtime migration remains downstream.

Kess's two `int[500]` arrays and frame locations are verified against concrete
DWARF. The spurious `positions[499] = 0x228fa90` write is removed: that address
is the return site of the native stack-probe call, which precedes stack
allocation, rather than a source initializer. Two separate `int[151]` marked
arrays are verified for Echoing Equation and Rise and Shine. The latter uses
the already recovered native Overload helpers. Valakut Awakening's prompt is
reconstructed as exactly 52 copied bytes including the NUL and 48 zero bytes.
Its native conditional draw and selection behavior remain intact.

Hall of Gemstone writes seven columns of each `int[2][8]` mana-override row.
The native base, 32-byte row stride, four-byte stores and seven-iteration bound
are verified; column seven remains untouched. Kaervek's Purge reads the upper
word of `eot_toughness`; Hidden Retreat retains a byte-zero write to `info_slot`.
Vectis Gloves tests the existing player-bit word with the verified
`PB_NONSTANDARD_LANDWALK_DISABLED` mask. Mirrorwing preserves both full-word
slot copies, eighteen target-pair copies, native stack handling and the
unsigned trigger-event gate. All 59 calls/tail jumps and seven callback
references to accepted or new implementations are checked against native
operands and source names.

All 4,430 prior records, C token prefixes, earlier interfaces, globals,
fragments and dialog stubs remain unchanged. The ledger reaches **4,445
implementations: 3,247 `card_*` functions and 1,198 helpers**, including 567
private functions (558 private helpers) and 640 public helpers. Three fresh
exports bring the raw cache to 4,699 rows and 4,641 original names. Source/ABI
checks pass for all 142 C units; provenance, compiler-part and GCC 9.3 PE32
build checks pass, including fourteen new public symbols and the private copy
callback. Registration coverage reaches 3,233 symbols, including 3,147 with a
CODED record. Reccmp metadata is refreshed with 18,343 pairs and
4,309 ledger mappings; all fifteen additions have explicit CSV
mappings. The earlier HTML report remains unchanged. Gameplay validation
remains downstream.

## Graveyard-selection and dynamic-stack family (pass 98)

Nine implementations, covering 6,655 native bytes, are recovered together in
their original source units. Five handlers are Underworld Breach; Chainer,
Nightmare Adept; Liliana, Untouched by Death; Genesis Ultimatum; and Gyrus,
Waker of Corpses. Four private helpers implement the three graveyard casting
selectors and Gyrus's end-of-combat token exile callback.

Chainer, Liliana and Gyrus use two-row variable-length integer arrays. Their
first rows contain candidate card IDs; their second rows contain corresponding
graveyard positions. Concrete allocation instructions establish eight bytes per
graveyard entry, four-byte elements and the row stride. DWARF confirms the
array ranks, fixed first dimension, dynamic bound forms and local locations.
Reference-form and expression-form DWARF bounds are not interpreted as enormous
constant array sizes. Source uses the native count-driven VLAs, including the
GNU zero-size case, without invented selection caps or cancellation behavior.
Chainer and Liliana retain their legality, mana and timing checks, graveyard
pointer reloads and original prompts.

Chainer pays for the selected ID, then plays its stored graveyard position and
removes the permission effect. Liliana's native final call instead passes
`play_card_in_grave_for_free(player, selected, 1)` despite constructing a
position row. Its three outgoing arguments and call target are verified
directly. This oddity is retained for downstream integration rather than
silently changed to the apparent intended behavior. The original guards,
cleanup and return paths are preserved.

Breach's activation allocates a 3,000-byte text buffer and uses a separate fixed
`int opts[100][2]` table. The native 3,000/2,999 debug constants, allocation,
buffer base, formatter capacity and table location resolve the misleading
2,968-byte Ghidra buffer. Synthetic return-address writes are removed only
after each value is matched to a concrete native call return site. The
player-bit byte test becomes the verified `PB_GY_ABILITIES_DISABLED` word mask;
the chosen option's byte test becomes its second word's `0x2000` bit. All three
Escape calls bind to the already recovered private `escape_part_2022` worker
at their actual native target. Fake-card creation/recycling, Cage checks,
graveyard source lookup, sorcery override lifetime and native bounds remain.

Genesis Ultimatum retains a variable-length saved-ID array capped at five
looked-at cards. It removes all chosen permanents before putting any into play,
then takes the remaining cards from the live deck after those effects, and
preserves the final spell destruction/basic-spell call sequence. Gyrus restores
the missing `%d` formatting argument from the concrete outgoing stack store.
Its typo `creatrue`, repeated power queries, signed base-power comparison,
minus-one array initialization and gate on all scanned graveyard cards remain.
The token's no-sleight/legacy bytes, native attacking action and private combat
exile callback are verified against the 76-byte token definition and stores.

All 4,445 prior records, C token prefixes, headers, interfaces, globals,
fragments and dialog stubs remain unchanged. The ledger reaches **4,454
implementations: 3,252 `card_*` functions and 1,202 helpers**, including 571
private functions (562 private helpers) and 640 public helpers. One fresh
callback export brings the raw cache to 4,700 rows and 4,642 original names.
All 142 C units pass source/ABI checks; provenance, compiler-part and GCC 9.3
PE32 build checks pass with five new public card symbols and four private
callback symbols. Private callback checks pair executable local COFF symbols
with their DWARF owning source unit, distinguishing the end-of-combat homonym
and Breach's compiler-outlined `.part.0` implementation. All 36 calls/tail jumps
and six callback references to accepted or new implementations are verified.
Registration coverage reaches 3,238 symbols, all five additions marked CODED,
for 3,152 CODED handler symbols overall. Reccmp metadata is refreshed with
18,351 pairs and 4,317 ledger mappings. Seven additions have explicit
CSV mappings; the current strict global-name matcher excludes Breach's split
body and the private end-of-combat homonym. Both are verified in the build by
source ownership. The earlier HTML report remains unchanged. Gameplay
validation remains downstream.

## Hogaak Convoke/Delve family (pass 99)

Seven implementations, covering 5,144 native bytes, are recovered together:
Hogaak, Arisen Necropolis; its Convoke predicate, eligibility check, hybrid
Convoke payment and Delve casting helpers; the native mana-text formatter; and
the shared creature/color chooser. Each lives in its original Modern Horizons,
Ravnica or functions source unit. The older formatter and private Ravnica
chooser bodies remain unchanged; native variants receive qualified names.

Concrete DWARF proves that the predicate takes player, generic cost and black
cost in EAX/EDX/ECX, then blue, green, red and white in four stack words. Its
source `card` parameter has only a GNU parameter reference and no physical
location. A focused Ghidra interface removes that ghost argument before all
callers are exported. All five caller targets are verified, and the three
eligibility frames are decoded independently as two black, black/green and two
green, with the other colored costs zero. Source retains the native generic
cost clamping and the distinction between a successful predicate result of one
and the creature-using result of two.

The predicate and payment helper each allocate a tapped-card VLA and a
five-column card/color history VLA. Four-byte and twenty-byte allocation
strides, dynamic DWARF bounds and zero/minus-one initializations are verified.
Their fifty-attempt loops retain colored-cost priority, generic fallback,
history across attempts, temporary tapped-state marks, removal on failed trials
and actual tap calls on payment. Live card-count reloads remain distinct from
the counts used for allocation. These trial algorithms are reconstructed as
decoded rather than replaced with an ideal Convoke solver.

Hybrid payment keeps six mana fields, ten option words, six-bit-per-color
encoding and a 500-byte prompt. The missing payment-label `%s` argument is
restored from its concrete fourth outgoing stack word. The formatter uses the
native 216-byte test domain and preserves its X/40 handling, color order and
100-byte text buffer. The public Ravnica chooser preserves single-color
priority, multicolor dialog choices and native color masks. Human payment
retains cancellation, target reset, fixed player-zero tap/rollback calls and
the native lack of a final residual-cost check when no target remains. No
synthetic fallback option or extra bounds check is added to native exceptional
paths; their downstream gameplay validation remains pending.

Delve casting retains the reverse search for the source ID, temporary graveyard
face-down/face-up calls, selection limits, payment order and post-payment exile
loop. Its overlapping pointer loop becomes an explicit reverse copy of nine
eight-byte target pairs, preserving slot zero for Convoke targeting. Both the
from-hand and from-grave card paths use the recovered helpers and retain their
native fizzle/return behavior. All eighteen calls/tail jumps to accepted or new
implementations are checked against native operands and source names.

All 4,454 prior records, C token prefixes, earlier interfaces, globals,
fragments and dialog stubs remain unchanged. Five helper declarations are
added to the existing header. The ledger reaches **4,461 implementations:
3,253 `card_*` functions and 1,208 helpers**, including 572 private functions
(563 private helpers) and 645 public helpers. One fresh formatter export brings
the raw cache to 4,701 rows and 4,643 original names. All 142 C units pass
source/ABI checks; provenance, compiler-part and GCC 9.3 PE32 build checks pass,
including six new public symbols and the private predicate. Registration
coverage reaches 3,239 symbols, including 3,153 marked CODED. Reccmp metadata
is refreshed with 18,357 pairs and 4,324 ledger mappings; all seven
additions have explicit CSV mappings. The earlier HTML report remains unchanged.
Gameplay validation remains downstream.

## Native flag/target consumers (pass 100)

This batch adds **19 implementations: 15 card handlers and four helpers** in
their original source units. The cards are Nimble Obstructionist, Orcish Healer,
Scouting Trek, Energy Vortex, Seedling Charm, Time and Tide, Samite Ministration,
Infinite Hourglass, Shadrix Silverquill, Commander's Plate, Path of Ancestry,
Quandrix Command, Desperate Gambit, Fungus Elemental and Emergence Zone.
The helpers are the public activated-ability eligibility function and
attached-Aura predicate, and the private Hourglass and flash-land callbacks.
The triggered-ability eligibility helper is also reconstructed with the native
84-byte target definition; its earlier accepted record is retained verbatim in
`previous_recovery` and is not counted as a new implementation. Both older
counter helper bodies and Urza's Legacy's older private Aura predicate remain
unchanged. Trickbind still binds the retained legacy helper; migration of that
older caller remains downstream.

Nimble's cycling path uses the verified four-byte stack record, eight-byte
player/card pairs and the disabled-graveyard-abilities bit in the player word.
Its two counter eligibility dependencies use the native target layout and typed
stack fields. The four-byte, public, zero-initialized `card_is_being_cycled`
word is restored in `functions/functions.c` from DWARF and release COFF/BSS
evidence at `0x2775470`. Native unsigned event ranges are restored for Nimble,
Orcish Healer, Commander's Plate, the activated-ability helper and the flash
callback. Time and Tide calls the already recovered native phase workers.
Commander's Plate binds the accepted all-commanders specialization with its
one physical argument rather than an invented stack argument.

Quandrix's local `CommandChoices` enum comes from its owning DWARF lexical
block: five values from zero through four, four-byte storage and a two-entry
array. Its cast/resolution loops terminate at `choices + 2`; the preceding
can-cast phase reuses the low two bytes of the first word as separate flags.
Shadrix's 100-byte text buffer shares a native stack location with a later
76-byte token object. Separate C objects restore those phases, with verified
2/1 stats, flying, forced color 34 and the chosen controller. Its two label
pointers and Emergence Zone's stored `SELECT_CARD` pointer are checked against
exact read-only native bytes. Desperate Gambit's callback variable has the
three-argument handler type; native byte writes to the info slot remain byte
writes. Orcish Healer's format call has four physical arguments; the fifth
argument in the raw decompilation is stale outgoing storage from the preceding
color-text call and is removed using the exact instruction sequence.

Scouting Trek restores its deck-sized VLA and select/remove/shuffle/put-back
sequence. **The native put-back loop reads `count + 1` entries**, including
one uninitialized entry beyond those selected; selecting the whole deck can
also read beyond the array. This behavior is retained and called out in source
for downstream review. No inferred gameplay fix is mixed into the recovery.
Energy Vortex's indirect call is retained as a typed, one-argument cdecl EXE
boundary at `0x499010`; its 56-byte entry returns zero. The EXE function itself
is not counted as recovered. Hourglass and its callback access the live
four-byte EXE control word at `0x736808`.

The evidence covers 20 native bodies totaling **20,557 bytes**, 167 direct
canonical calls and 15 callback references. All 4,460 unaffected predecessor
records and all earlier C token prefixes survive unchanged; the one migrated
record retains its exact history and compatibility body. Earlier interfaces,
57 existing globals, fragments, storage/mask registries and dialog stubs are
unchanged. Three public helper declarations and the cycling extern are added.
The ledger reaches **4,480 functions: 3,268 `card_*` functions and 1,212 helpers**,
including 574 private functions (565 private helpers) and 647 public helpers.
The raw cache has 4,704 rows and 4,646 original names. Registration coverage
reaches 3,254 symbols, including 3,167 marked CODED. Source/ABI checks pass for
all 142 C units and the dialog-stub C++ unit; compiler-part and GCC 9.3 PE32
build checks pass, including 17 new public symbols, two private callbacks,
both migrated/retained counter symbols and the public cycling BSS word.
Reccmp metadata is refreshed against this DLL/DBG pair with 18,380 pairs and
4,347 ledger mappings. All 20 processed bodies have explicit mappings and the
metadata/input hashes are verified; the earlier HTML report is unchanged.
Gameplay validation remains downstream.

## Modal and packed-state consumers (pass 101)

This batch adds **14 implementations: ten card handlers and four public
helpers** in their original source units. The handlers are Verdant Command,
Very Cryptic Command variants B and E, Inscription of Abundance, Penance,
Lutri the Spellchaser, Vision Charm, The Fallen Apart, Symbol Status and Sly
Spy variant A. The helpers are native cast, validate, raw-counter and resolve
operations for activated abilities at `0x2692e80`, `0x2692ef0`, `0x2692fc0` and
`0x2692ff0`. Qualified names retain all four older compatibility bodies.
Verdant uses these helpers and the eligibility function recovered in pass 100;
other earlier callers remain bound to their existing interfaces.

Three function-local `CommandChoices` enums are restored from their owning
DWARF lexical blocks, with four-byte storage and exact labels/values. Each
handler has two choices, immediately before the next native stack object;
all loop endpoints become `choices + 2`. Verdant's graveyard AI priority is
an integer during the earlier phase, separated from the later choice pointer
that reuses storage. The native uninitialized human-priority inputs are
retained. Very Cryptic Command B's two validations of the same target index
are also retained and checked against both call frames. Inscription's nine
21-word copies become full 84-byte target assignments, preserving trailing
flags and the original order of controller overrides. Its eight-byte
information accessor remains the verified EDX:EAX interface.

Lutri's source-level dynamic `int` array is supported by DWARF's
`DW_OP_breg3 -1` upper bound and the native four-times-slot-count allocation.
**The native zero fill clears only `available_slots` bytes**, leaving much of
the integer array uninitialized. That behavior is retained in source. The
scan uses unsigned 16-bit card IDs and stops at the deck sentinel or 500
entries; the can-cast flag uses a full zero-extended information-slot write.
Penance retains its single-byte flag write. The Fallen Apart and Sly Spy
retain signed limb bytes and packed damage counts, with singular/plural
label pointers restored from exact native read-only bytes.

Vision Charm's local subtype arrays contain exactly five and thirteen
integers at `0x2723acc` and `0x2723ae0`. **Native indexing uses the shifted
bit masks, which can exceed those array bounds.** The original table sizes,
contents and indexing are preserved and annotated for downstream review.
Its phase-out call binds the previously recovered native worker. Symbol
Status restores the 198-byte expansion bitmap, the three partial-word clear
bytes and the array-end sentinel. The imported rarity record's first-byte
expansion read is checked against the native load.

Symbol Status also needs Drawcardlib's existing `get_config_data` export.
The release IAT entry at `0x27b5370` and both three-argument call frames
support a typed import for the two integer queries; this is a dependency
boundary, not a recovered Drawcardlib implementation. The existing root
`Drawcardlib.def` gains only that export name. The build regenerates an i386
import archive with eleven members, and the output DLL's import is verified.

The evidence covers **13,856 native bytes**, 131 canonical calls and nine
callback references. All 4,480 previous records, C token prefixes, earlier
interfaces, 58 globals, fragments, storage/mask registries and dialog stubs
remain unchanged. The header adds four helper declarations and the import.
The ledger reaches **4,494 functions: 3,278 `card_*` functions and 1,216 helpers**,
including 574 private functions (565 private helpers) and 651 public helpers.
Four focused helper exports bring the raw cache to 4,708 rows and 4,650
original names. Registration coverage reaches 3,264 symbols, including 3,176
marked CODED. All 142 C units and the separate dialog-stub C++ unit pass
source/ABI checks; provenance, compiler-part and GCC 9.3 PE32 build checks
pass, including all fourteen new public symbols and the Drawcardlib import.
Reccmp metadata is refreshed with 18,392 pairs and 4,363 ledger mappings.
Thirteen additions have direct mappings. GCC emits the new raw-counter entry
as a five-byte tail jump to the retained compatibility body; both public
symbols and the exact jump target are verified. Its declaration has no concrete
DWARF range, so the current reccmp name matcher omits that one entry. Current
DLL/DBG/CSV hashes are verified; the earlier HTML report remains unchanged.
Gameplay validation remains downstream.

## Library-selection VLAs and Proliferate (pass 102)

This batch adds **ten implementations: seven card handlers and three public
helpers** in their original source units. The cards are Hazoret's Undying
Fury, Sunbird's Invocation, Gishath, Sun's Avatar, Niv-Mizzet Reborn, Kamahl's
Druidic Vow, Emergent Ultimatum and Planewide Celebration. The native automatic,
manual and top-level Proliferate helpers receive qualified interfaces;
all three older helper bodies and their earlier callers remain unchanged.

Six dynamic arrays are reconstructed from native allocation/stride evidence
and DWARF type/bound/location records. Sunbird uses a two-row integer table:
one row preserves the revealed IIDs, while the second substitutes the draw-card
placeholder for ineligible choices. After removing the revealed cards, it
returns the unchosen entries to the top in reverse order, casts the selected
IID and moves the remaining count to the bottom. Live deck reads after the
legality check and the original count adjustment are retained. The other
arrays hold shrinking Hazoret cast candidates, Gishath/Niv/Vow library copies
and Emergent's final casting queue. Emergent's dynamic upper bound is absent
from DWARF; its native allocation and surrounding frame supply the evidence.
Large reference-valued bounds in the other DIEs are recorded as references,
never interpreted as enormous static arrays.

Fourteen native outgoing frames are saved: seven eight-argument zone selectors,
three formatting calls and four free-casting calls. Hazoret casts using
`1 - player` as exile owner; Emergent uses `player`. Niv retains an extra
format argument even though its prompt has no conversion. Kamahl's formatted
CMC limit and test threshold use the clamped revealed-card count. Native
Grafdigger's Cage gates skip the complete Gishath/Kamahl selection blocks.
Cancellation restores remaining copies in their native forward order before
random bottoming or milling. Early returns, including Emergent's zero-chosen
and one-card cases, retain their original cleanup behavior.

**Hazoret and Emergent compact their queues with an extra read at the current
count**, potentially outside the array. Both reads are preserved and annotated
for downstream review. Emergent also initializes a second test object while
passing the first to the cast chooser; that native detail is retained. Its two
message updates copy exactly 55 and 31 bytes, including each terminator.
All restored prompt bytes and copy extents are checked against the release.

Planewide stores four choices in the four bytes of its information slot, then
reuses the native choice-array stack location for a 76-byte token object.
The phases become separate C objects with verified 2/2 stats, forced color 62
and `no_sleight = 1`. The declaration following a native label is enclosed in
a compound statement for GCC 9's C grammar. Proliferate's native target uses
the 84-byte layout; its two helpers retain the original counter-kind chooser
and player-counter behavior. Poison, experience and energy use offsets zero,
one and two in the verified four-byte player row.

The evidence covers **8,440 native bytes**, 52 canonical calls and four callback
references. All 4,494 previous records, C token prefixes, earlier interfaces,
58 globals, fragments, storage/mask registries, Drawcardlib imports and dialog
stubs remain unchanged. Three helper declarations are added. The ledger reaches
**4,504 functions: 3,285 `card_*` functions and 1,219 helpers**, including 574
private functions (565 private helpers) and 654 public helpers. Three focused
helper exports bring the raw cache to 4,711 rows and 4,653 original names.
Registration coverage reaches 3,271 symbols, including 3,183 marked CODED.
All 142 C units and the dialog-stub C++ unit pass source/ABI checks; the GCC 9.3
PE32 build and all ten new public symbols pass verification. Reccmp metadata
is refreshed with 18,401 pairs and 4,375 ledger mappings. Nine additions map
directly. GCC outlines the manual Proliferate helper into a public entry and
private `.part.0` body sharing a DWARF name; both COFF symbols and owning-CU
DWARF ranges are verified, while the current name matcher omits that ambiguous
entry. All DLL/DBG/CSV identities are checked; the earlier HTML report is
unchanged. Gameplay validation remains downstream.

## Target-display and hand-selection arrays (pass 103)

This batch adds **eight implementations: six card handlers and two public
helpers** in their original source units. The cards are Force of Vigor, Mass
Manipulation, Release the Gremlins, Builder's Bane, Metzali, Tower of Triumph
and Aether Tide. Native multiple-X and double-X charging at `0x268db60` and
`0x268dc30` receive qualified interfaces, retaining both older helper bodies.
The double-X wrapper passes the constant multiplier two to the new native
multiple-X implementation. Original human/speculative branches, `hack_xx`
restoration, integer division, total-payment return and cancellation remain
unchanged.

Seven integer VLAs replace corrupted stack/pointer expressions. The first four
are target displays sized from the target-count byte. Each reads the original
player/card pair, obtains a permanent's IID or the native self/opponent player
placeholder (`0x615a`/`0x6161`), then passes the live fill count through the
six-word `show_deck` frame. Each caption is checked against release bytes.
Mass Manipulation's target-player expression becomes `targets[i].player`:
the native address arithmetic is `64 + (i + 14) * 8 - 60`, equal to the verified
116-byte target base plus an eight-byte row stride.

Metzali restores an availability-sized candidate array initialized to -1. It
scans permanents belonging to `current_turn`, retains those passing the native
target predicate, and selects using a fresh `target_available` result as the
random bound. **The native bound is not replaced by the number of filled
entries.** Its decompilation had lost the entire dialog tail. The restored
18-word call preserves both activated-ability results, two control strings,
three option triples and four terminators. The destruction priority is -100
on the controller's turn, and either 15 or zero on the other turn depending
on target availability. Ability conditions and mana parameters remain native.

Aether Tide snapshots original IIDs and hand positions in a two-row VLA. A
second hand-count read, when the native scan runs, independently sizes its
discard array. The eight-word chooser frame retains mode three and cancel
value -1. Cancellation before paying all required discards sets `spell_fizzled`
and returns. **Compaction reads one extra entry from each snapshot row**;
those reads are retained and annotated. The later mana charge uses the native
32-bit target-count-minus-information-slot result, and actual discards occur
only if payment did not fizzle. Native selection, temporary untargetability,
copy/free-cast gates and cleanup order remain unchanged.

Spell-specific behavior is preserved rather than normalized across the four
related display handlers: Force's pitch-cost/retarget path and destruction
loop, Mass Manipulation's human/AI X handling, Gremlins' token quantity from
the original target count and Builder's Bane's post-kill life calculation.
Six unused synthetic return-PC temporaries and their stand-alone constant
assignments are removed only after verifying that no remaining reads exist.
The earlier selector-storage recovery and its provenance remain unchanged.

The evidence covers **10,063 native bytes**, 120 canonical calls and two
callback references. Seven outgoing frames record four displays, the discard
chooser, the dialog and the random call. All 4,504 earlier records, C token
prefixes, interfaces, 58 globals, fragments, storage/mask registries,
Drawcardlib imports and dialog stubs survive. Two helper declarations are
added. The ledger reaches **4,512 functions: 3,291 `card_*` functions and
1,221 helpers**, including 574 private functions (565 private helpers) and
656 public helpers. Two focused exports bring the raw cache to 4,713 rows and
4,655 original names. Registration coverage reaches 3,277 symbols, including
3,188 marked CODED. All 142 C units and the dialog-stub C++ unit pass source/ABI
checks; provenance, compiler-part and GCC 9.3 PE32 build checks pass, including
all eight new public symbols. Reccmp metadata is refreshed with 18,407 pairs
and 4,383 ledger mappings; all eight additions have explicit mappings, and
current DLL/DBG/CSV identities are verified. The earlier HTML report is
unchanged. Gameplay validation remains downstream.

## Letter-name, dice and toggle handlers (pass 104)

Recovered four card handlers and five private helpers in the original
`cards/unglued_unhinged.c` and `cards/unstable.c` source files:

- Spark Fiend and Togglodyte, including their private effect-name callbacks.
- Now I Know My ABC's and its permanent-name alphabet collector.
- Staff of the Letter Magus and its seventeen-consonant chooser.
- The event-specialized `txtfx.part.0` compiler body, represented as the private
  three-argument `txtfx_name_2022` helper. The existing four-argument public
  `txtfx` body and its earlier callers remain unchanged.

The callbacks pass player, card and marker in EAX, EDX and ECX. Applying that
interface before exporting removes the fictitious fourth stack argument and
incorrect event operand. Both callbacks retain their native event gates.
Spark Fiend retains its initial/upkeep dice paths, packed marker/point state
and `Don't roll` sentinel. Togglodyte retains its spell-triggered toggle,
attack/block restrictions and combat-damage prevention.

Two native indexed loads use the complete 256-byte table at `0x273f300`.
Local read-only copies retain byte-exact uppercase/accent folding, AE ligature
handling for the alphabet collector, and sharp-s/thorn rules for the Staff.
The collector's 26-byte bitmap begins at EBP-0x22: the store at
EBP+character-0x83 corresponds to `letter[character - 'a']`. Its reduction
checks the first byte followed by 25 remaining bytes, ending at EBP-8.
Fictitious pre-array storage and unused return-PC locals are removed.

The chooser copies seventeen native pointers from `0x27211c0`, preserving
the order B, C, D, F, G, H, J, K, L, M, P, Q, V, W, X, Y, Z. Human, AI,
speculation and trace-mode paths retain native unchecked choice indexing.
Staff's two long-name special cases preserve exact spelling, capitalization
and first/second-byte fast paths. Ordinary name lookup continues using the
existing shared source implementation.

All 4,512 previous records and source token prefixes survive unchanged,
as do headers, 58 globals, earlier interfaces, storage/mask registries,
Drawcardlib imports and both out-of-scope chooser stubs. The nine new bodies
cover 2,539 native bytes, 22 canonical dependency operands and five callback
references. The ledger reaches **4,521 functions: 3,295 `card_*` functions and
1,226 helpers**, including 579 private functions (570 private helpers) and
656 public helpers. Refreshed raw exports leave the cache at 4,713 rows and
4,656 distinct names. Registration coverage reaches 3,281 symbols, including
3,192 marked CODED.

All 142 C units, the dialog-stub C++ unit, source/ABI/provenance and
compiler-part checks pass. The GCC 9.3 PE32 build succeeds; all four new public
card symbols and five local helper symbols are verified. Gameplay validation
remains downstream. This pass identified Bloodletter's earlier empty-string
indexed-load artifact, which uses the same native fold table. Pass 105 repairs
that previous recovery and retains its exact predecessor record and body.

Reccmp metadata is refreshed with 18,419 pairs and 4,395 ledger mappings.
All nine additions map directly, including the explicitly named compiler
part; resolving that name also makes three earlier ledger entries eligible.
Current DLL/DBG/CSV identities are verified. The earlier HTML report is
unchanged.

## Rules-text parsing and consumers (pass 105)

Recovered the shared `get_rules_text` helper in `cards/unglued_unhinged.c`,
Punctuate in that file, and Capital Offense in `cards/unstable.c`. The helper
retains its empty-text special case, `strlen(original) + 1` character VLA,
exact original-text copy and native `\n--FLIP--\n` truncation. Its two 300-byte
back-face patterns are built from the following card's name: the native load
uses `0x73bb04 + csvid * 4`, four bytes after the `cards_ptr` base. Patterns
end in ` - ` and `)` and are applied in the original order.

The pipe-token walker preserves the raw native control flow and cursor
movements for numeric, colored, hybrid, half, Phyrexian, escaped-pipe and
unknown codes. Optional reminder removal stops at the first closing `)`;
an unmatched opening parenthesis terminates the output. No bounds checks or
nested-parenthesis behavior are added. The suspicious `|/` path is preserved:
native code writes the word `or`, advances the output cursor by one, then
overwrites the second byte on the next write. This remains a downstream
behavior question, rather than an inferred source repair.

Punctuate uses the verified `msvcrt.dll!strpbrk` thunk at `0x2701da8`, through
IAT slot `0x27b5514`. Its exact eighteen punctuation bytes include Windows
dash and quotation characters; octal C escapes preserve those bytes. Every
match advances the search pointer by one, and damage remains half the match
count. Capital Offense requests reminder removal, counts ASCII uppercase
bytes and applies the native negative pump. Its current character is restored
as a register local instead of fictitiously writing each byte over `rules[0]`.
Both handlers retain their original target, resolution and cleanup paths.

Bloodletter's indexed first-character load now uses the complete native
256-byte table at `0x273f300`. Its sacrifice and mass-damage calls bind to
the recovered native helpers. All other control flow is retained. Its exact
previous manifest record and source body are stored in the component evidence
and `previous_recovery`; the other 4,520 previous entries remain unchanged.
The source-prefix check allows only this explicit predecessor replacement.

Three new implementations bring the ledger to **4,524 functions: 3,297
`card_*` functions and 1,227 helpers**, including 579 private functions
(570 private helpers) and 657 public helpers. Registration coverage reaches
3,283 symbols, including 3,194 marked CODED. Four refreshed exports retain
4,713 raw rows and 4,656 distinct names. The recovered/repaired bodies cover
1,991 native bytes and thirteen canonical dependency operands.

All 142 C units, the dialog-stub C++ unit, source/ABI/provenance checks and
compiler-part audit pass. The GCC 9.3 PE32 build and all three new public
symbols are verified. Headers gain only the shared rules-text declaration;
globals, earlier components, storage/mask registries, Drawcardlib imports and
both out-of-scope chooser stubs remain unchanged. Gameplay validation remains
downstream.

Reccmp metadata is refreshed with 18,422 pairs and 4,398 ledger mappings.
All three additions and repaired Bloodletter map directly. Current DLL/DBG/CSV
identities are verified; the earlier HTML report remains unchanged.

## Native Windows clock helpers and consumers (pass 106)

Recovered `localtime_date`, `localtime_hour_minute`, `localtime_hour`,
`time_in_ms` and `sleepms` in their original `functions/windows.c` source file,
alongside Elvish House Party, Old-Fashioned Vampire and Snow Mercy in their
original set files. The two earlier display-copy helpers and the rest of the
Windows source remain unchanged.

The Windows `SYSTEMTIME` object is sixteen bytes. Native DWARF and compile-time
assertions agree on year/month/day/hour/minute offsets 0/2/6/8/10. The date
formatter retains `%02d/%02d/%d` and returns the same persistent eighteen-byte
function-local static buffer on every call. Its rebuilt DWARF array bound,
address expression, local COFF symbol and BSS allocation are verified. This
local static is separate from the 58 recovered file-scope global declarations.
The packed hour/minute helper returns the hour in the high word and minute in
the low word, both zero extended; the hour-only helper returns the native
24-hour value.

The millisecond helper calls `QueryPerformanceFrequency` before
`QueryPerformanceCounter`. Its lowered arithmetic is
`((ticks * 32 - ticks) * 4 + ticks) * 8`, a wrapped 64-bit multiply by 1000,
followed by the signed `__divdi3` quotient and the returned low 32 bits.
The recovered source expresses that operation with an unsigned multiply and
signed division, preserving native truncation and wrap behavior. The two
eight-byte Windows counter objects are checked. Native ignored API status
returns and absence of division/failure guards are retained. `sleepms` remains
a direct Windows `Sleep` call with the original 32-bit argument conversion.

Elvish House Party uses the twelve-hour value, with zero converted to twelve,
for the affected card's power/toughness events. Old-Fashioned Vampire retains
its 18:30-inclusive to 05:30-exclusive night interval and original event gates,
including the absence of an affected-card check in that handler. No cached
clock or inferred gameplay correction is introduced.

Snow Mercy retains the ten-entry damage-source queue, immediate Globe-counter
overflow, trigger collection/reset and mass-tap callback. Its temporary state
changes, four tap/untap iterations, sound choices, 350 ms delay, final tapped
state and speculation gates are preserved. Both `redisplay_all()` artifacts
actually call the existing executable boundary `0x437ec0` with two stack words,
0 and 255. Those exact DLL frames and the matching executable prefix are
verified; no redraw or dialog implementation is decompiled here.

The ledger reaches **4,532 functions: 3,300 `card_*` functions and 1,232 helpers**,
including 579 private functions (570 private helpers) and 662 public helpers.
All 4,524 previous records and C token prefixes survive unchanged. Five header
declarations are added; previous globals, fragments, interfaces, storage/mask
registries, Drawcardlib imports and out-of-scope chooser stubs remain unchanged.
Eight refreshed exports retain 4,713 raw rows and 4,656 distinct names.
Registration coverage reaches 3,286 symbols, including 3,197 marked CODED.
The new bodies cover 1,709 native bytes, fifteen canonical dependency operands
and one callback reference.

All 142 C units, the dialog-stub C++ unit, source/ABI/provenance and compiler-part
checks pass. The GCC 9.3 PE32 build succeeds. All eight new public symbols,
the private date buffer and all four Windows API imports are verified.
Gameplay validation remains downstream.

Reccmp metadata is refreshed with 18,431 pairs and 4,407 ledger mappings.
All eight additions map directly; Snow Mercy's new use also makes the earlier
private Globe-counter callback visible to the matcher. Current DLL/DBG/CSV
identities are verified. The earlier HTML report remains unchanged.

## Pack utility foundation and native descriptions (pass 107)

The original `functions/packs.c` file was absent. It now contains five recovered
helpers: `is_valid_pack`, `pack_size`, `announce_expansion_choice`, and the
private `add_to_csvid_choices_2022` and `add_card_to_pack_2022` workers. This
is a backend foundation for Booster Tutor and Stocking Tiger. The full pack
generator, its remaining helpers, the selection dialog and both card handlers
are still pending. No substitute generator or additional GUI stub is added.

Native DWARF bounds and structure layouts identify 43 immutable rarity-description
arrays and the 198-entry `pack_contents` table. All 44 objects are reconstructed
from 4,536 native data bytes, preserving every count, sentinel, pointer alias and
subarray offset. Rarity rows are eight bytes; pack records are twelve bytes.
The rebuilt DLL's 43 array byte sequences match the native input exactly, and
all 396 booster/starter pointers are verified against their relocated objects.
The separate `no_boosters` and `no_starters` objects retain pointer identity.

`Expansions.h` already contained identifiers through value 169. All those
values match native DWARF and remain unchanged. Thirty missing enumerators
extend that existing enum: zero, values 170 through 197, and the `LAST` alias.
There are 199 native enumerator names representing 198 distinct values.
The public utilities use the existing `EXPANSION_t` type. The private draft
rarity enum and record types stay in the original pack source unit.

The two `.isra.0` workers have field-split interfaces. The append worker takes
the count address, pool-pointer address and card ID in three registers. The
pack-add worker takes three register arguments and six stack words, including
both original field addresses. Ghidra receives these interfaces before export;
the typed C helper names are qualified to distinguish their physical shapes.
The clone importer gains backward-compatible support for an `int **` parameter.

Append allocation starts at 200 entries, doubles at native boundaries and
retains the original 3,200-entry stop and fatal messages. Pack-add keeps the
local pool copy, native random draws, basic/snow-land duplicate exceptions,
last-entry removal, forced-duplicate fallback and diagnostic behavior.
Diagnostics use the verified native `fprintf` import and stderr, whose runtime
shim computes `_iob + 2 * 32`. Both exact format strings are restored.

The validity predicate retains its native unsigned range and excluded sets.
The sizing caller keeps the original expansion operand in ECX across that
predicate, which never writes ECX; the decompiler's fictitious clobbered
register local is removed. Conditional rows retain the maximum-of-two size
calculation and native assertions. Announcement keeps its speculation/player
gates, text-file offsets and all three native dialog-control literals.

The ledger reaches **4,537 functions: 3,300 `card_*` functions and 1,237 helpers**,
including 581 private functions (572 private helpers) and 665 public helpers.
All 4,532 previous records and C token prefixes survive unchanged. The 44 new
table objects bring the file-scope global registry to 102 declarations; earlier
globals, fragments, interfaces, storage/mask registries and chooser stubs remain
unchanged. Raw exports retain 4,713 rows and 4,656 distinct names. The five
bodies cover 1,416 native bytes and six canonical dependency operands.

All 143 C units and the dialog-stub C++ unit pass source/ABI checks. Provenance,
compiler-part and GCC 9.3 PE32 build checks pass. The build now includes 165 C
objects, two C++ objects and twenty linked assembly objects. All three public
utility symbols and all 44 table objects are verified. GCC removes the two
unreferenced private workers until their generator caller is recovered;
their source bodies and native provenance are retained. Gameplay validation
remains downstream.

A fresh inventory finds **23 missing `card_*` handler names**. The 2022 DWARF
inventory is checked against accepted addresses, current C definitions and
rebuilt executable COFF function symbols so macro-generated handlers are
recognized. This count excludes helper dependencies and unverified changes
within retained 2020 handlers. Dispossess and Mise depend on the chooser UI
already set aside by the user.

Reccmp metadata is refreshed with 18,434 pairs and 4,410 ledger mappings.
All three public additions map directly. The two unreferenced private workers
are absent from the rebuilt function table and are correctly omitted by the
matcher. Current DLL/DBG/CSV identities are verified; the earlier HTML report
remains unchanged.

## Weighted pack generator and native wrapper (pass 108)

The full pack generator is now recovered in `functions/packs.c`, together with
its public wrapper and the release's original `test_packs` entry point. The
private generator is named `make_pack_normal_2022`; the wrapper is
`make_pack_2022`, preserving the unrelated two-argument draft helper already
present in `cards/draft.c`. This batch recovers three helpers covering 4,570
native code bytes and 33 canonical dependency operands.

Eighteen immutable count/card tables restore all 878 weighted rows from 7,024
native data bytes. Their mixed `-1` and `0` terminators remain exact. A local
nineteen-entry initializer preserves all eighteen pointers and the final null
entry; its native copy spans 152 bytes. The generator retains 44 choice pools,
the three contraption weights `{16, 7, 2}`, the set-specific rarity/land/DFC
branches, native random draw order, duplicate fallback and testing-mode retry
limits. Native quirks remain unchanged rather than being corrected to card rules.

The typed reconstruction restores the five-byte `Rarities` header followed by
expansion-rarity bytes and the seven-entry subtype array ending at record offset
146. Compile-time assertions check both layouts. The public wrapper keeps its
output pointer in ECX across `is_valid_pack`, which does not write ECX. Its
temporary allocation, normal/test mode choice, failure diagnostics and free
sequence retain the release behavior. The final empty switch arm has an explicit
null statement for GCC 9 C grammar, recorded as a scoped reconstruction.

`test_packs` retains its four-byte static once gate, expansion loop, timing
conditions and native 32-bit elapsed subtraction before float conversion.
The `fprintf`, `fflush` and stderr boundaries are checked against native
imports/shims. This recovers the original entry point as source; it has not
been executed.

The ledger reaches **4,540 functions: 3,300 `card_*` functions and 1,240 helpers**,
including 582 private functions (573 private helpers) and 667 public helpers.
All 4,537 preceding records and C token prefixes survive unchanged. The global
registry now contains 120 declarations, including the eighteen weight arrays.
The 143 C units, C++ stub unit, provenance/compiler-part audits and GCC 9.3
PE32 build pass. All eighteen rebuilt weight arrays match native bytes; the
generator, both previously unused private workers and the once gate are
verified in the rebuilt debug data.

The pack selection GUI and its card consumers remain pending. The inventory
still contains 23 missing card-handler names; this batch closes their shared
generator dependency. Gameplay validation remains downstream.

Reccmp metadata now contains 18,439 pairs and 4,415 ledger mappings. All three
additions and the two previously omitted private workers map directly. The
current release/build DLL/DBG and CSV hashes are verified; the older HTML report
is unchanged.

## Pack chooser, Booster Tutor and Stocking Tiger (pass 109)

The remaining three functions in the original `functions/packs.c` are recovered:
`dlgproc_choose_a_pack`, `generate_choose_a_pack_dlg_template` and `choose_packs`.
Booster Tutor and Stocking Tiger are recovered in their original set files,
using the native generator and chooser. This batch covers 4,466 native code
bytes, 24 canonical dependency operands, one callback reference and 41 direct
or register-load IAT references.

The complete 294-byte dialog template is restored byte for byte with four-byte
alignment. Its eight controls are six pack comboboxes (IDs 1101–1106), OK and
Cancel; the template uses 10-point MS Sans Serif and a 186-by-116 dialog area.
The 64-byte argument structure contains six eight-byte expansion/booster rows.
The private callback retains its stdcall ABI and `_dlgproc_choose_a_pack@16`
COFF symbol. Its background image, expansion names, brush, pens and argument
pointer retain their function-local static lifetime. The public chooser's
counter starts at -1 and increments with native 32-bit wrapping.

Correct Win32 stdcall import signatures remove the decompiler's accumulating
stack-pointer artifacts. The typed reconstruction restores the original
279-byte path buffer, 300-byte item buffers, 48-byte owner-draw structure,
56-byte ANSI text metrics and 16-byte rectangles. Native painting retains
palette handling, the double-buffer/critical-section sequence, focus colors,
expansion symbols, dragging, cancellation rules and object cleanup. All
template styles, control coordinates, class ordinals and strings are decoded
in the evidence. The native `blt_expansion_symbol` Drawcardlib import is added;
the import library now contains twelve i386 members. The import-definition
audit checks this exact append against its predecessor, including the earlier
configuration import, rather than discarding the prior definition hash.

Booster Tutor restores two integer VLAs sized by `pack_size`; Stocking Tiger
restores one. The native stack-probe sites and four-byte element widths are
recorded. Both retain their distinct AI expansion choices, packet tag 26 and
explicit `(0, 26)` send / `(1, 26)` receive frames. Booster Tutor uses the
216-byte native filter and recovered selector/show-deck APIs. Its display and
selection bound remains the maximum pack capacity, even if generation returns
fewer cards, and selection retries retain the native no-cancel loop. Stocking
Tiger keeps the `0x3f7c` high-word expansion marker, clears it before generation
and adds the generated contents after qualifying combat damage. Its marker
storage remains at card-instance offset `0x90`.

Other native quirks remain explicit: a nonzero `DialogBoxIndirectParamA`
result, including -1, is treated as success; the expansion-name guard checks
`num_textlines > expansion + 6` before an `expansion + 7` lookup. These are
source recovery choices, not gameplay fixes.

The ledger reaches **4,545 functions: 3,302 `card_*` functions and 1,243 helpers**,
including 583 private functions (574 private helpers) and 669 public helpers.
All 4,540 preceding records and C token prefixes survive unchanged. The template
brings the global registry to 121 declarations. All 143 C units and the C++ stub
unit pass syntax/ABI checks; provenance, compiler-part and GCC 9.3 PE32 build
checks pass. Native verification checks the complete template bytes, callback
linkage, seven persistent local objects and all 28 drawing/Win32 imports.
The Wish/card-picker stubs and their scoped deferral remain unchanged. The
fresh native-name/accepted-ledger/executable-COFF inventory finds **21 missing
card-handler names**, excluding changes within retained 2020 handlers. No game
or dialog was executed; gameplay validation remains downstream.

Reccmp metadata now contains 18,444 pairs and 4,420 ledger mappings. All five
additions map directly, including the private callback through its DWARF name.
Current release/build DLL/DBG and CSV identities are verified. The older HTML
report remains unchanged.

## Alternate hand/graveyard casting and replacement filters (pass 110)

Six card handlers are recovered in their original set files: As Foretold,
Fires of Invention, Jodah, Archmage Eternal, Abandoned Sarcophagus, Lurrus of
the Dream-Den and Muldrotha, the Gravetide. Four shared graveyard-replacement
helpers are recovered in `functions/functions.c` under qualified `_2022`
names, using the native 216-byte test filter. The four existing 208-byte
filter APIs and their bodies remain intact. The batch covers 8,856 native
code bytes, 33 canonical dependency operands and four private callback
references.

Each card restores a two-row integer VLA. The first row contains internal
card IDs and the second their original hand or graveyard positions. Three
arrays use `hand_count[player]`; three use `count_graveyard(player)`. Native
stack-probe frames confirm eight bytes per candidate capacity, and every
array has its own native allocation. All six card filters retain the 216-byte
layout and the selector uses the qualified native interface. Existing
`has_cycling` and `muldrotha_legacy` callback bodies are retained, with their
native pointer operands recorded.

As Foretold preserves upkeep counters, the `targets[3].player` once-use marker
and cleanup reset. Fires of Invention retains the storm-count/turn gates,
land-count ceiling, infinite-cost restriction and 100-byte prompt buffer.
Both preserve the native 99/1 legality branches and the repeated sorcery-timing
predicate calls. Jodah pays WUBRG before constructing candidates, keeps original
hand positions and retains the AI rule that uses this path only when normal
payment is unavailable. It tests `spell_fizzled != 1`, as the binary does.

Abandoned Sarcophagus preserves its cycling exclusion and native enable mask
`0xA`; the older header's Milled-enable value is different, so it is not used
to replace that literal. Its numeric trigger gate `8` remains unchanged.
The four replacement helpers restore the library, nonlibrary and battlefield
paths, native filter calls, trigger/resolve events, exile flags, sounds and
replacement ordering. The newly typed APIs keep 216-byte pointers throughout
instead of passing them through the older 208-byte interface.

Lurrus keeps its 500-entry companion deck scan and the native active-card query
checks, including rejection of active entries with mana value at most two.
That unusual condition is verified against the branch instructions and is
not changed to match card rules. The turn, humiliation and once-use checks
remain in `EVENT_CAN_ACTIVATE`; activation pays, marks usage and plays the
stored graveyard position in the native order.

Muldrotha finds or creates its matching attachment legacy and stores used types
there when present. Its `-1` mask is normalized only during the native
can-activate path. The full 27-word type chooser includes five control strings,
the Artifact/Creature/Enchantment/Land/Planeswalker order, each availability and
weight operand, and four trailing zero words. Its type-state update precedes
playing the stored graveyard position. Activation returns the saved zero
confirmed at the native epilogue, resolving misleading pointer-valued output
from the decompiler.

The ledger reaches **4,555 functions: 3,308 `card_*` functions and 1,247 helpers**,
including 583 private functions (574 private helpers) and 673 public helpers.
All 4,545 previous records and C token prefixes survive unchanged; global,
fragment, storage/mask and dialog-stub registries retain their previous data.
All 143 C units and the C++ stub unit pass source/ABI checks. Provenance,
compiler-part and GCC 9.3 PE32 build checks pass. Binary verification checks
all ten new public symbols, both generations of the four filter APIs and the
six two-row array shapes. GCC omits the three hand-size bounds in DWARF; those
bounds are checked through source and original native allocation evidence.
The inventory now finds **15 missing card-handler names**. No game code was
executed; gameplay integration and validation remain downstream.

Reccmp metadata now contains 18,452 pairs and 4,432 ledger mappings. All ten
additions and both previously unused private callbacks map directly. The four
replacement-helper RVAs now use explicit `_2022` mappings instead of their
earlier same-name matches to retained 2020 bodies. Current release/build DLL/DBG
and CSV identities are verified; the older HTML report remains unchanged.

## Exile/return records, Aegis and Void counters (pass 111)

Endless Sands, Izzet Chemister, Livio, Oathsworn Sentinel and Dauthi Voidwalker
are recovered in their original set files, together with the private
`create_dauthi_voidwalker_legacy` constructor. The five functions cover 12,582
native code bytes, 43 canonical dependency operands and five callback
references. Existing Aegis/Void legacy callback bodies remain intact.

The packed exile-list value is restored as a 32-bit word: bit 31 is the owner
and the low 31 bits are the internal ID. The decompiler's wrapped pointer
arithmetic resolves to 72-byte card-data records, with the ID at byte 36 and
short name at byte 1. Sands retains its parent-linked exile count, token path,
AI regeneration, differing capability/payment costs and detached-list return
loop. Remembered cards enter under the activating player, while the encoded
owner identifies the exile zone to remove them from.

Chemister restores its variable-length integer list using the recorded exile
amount, with the original four-byte stack-probe allocation. Its two 216-byte
filter objects are distinct: the native code initializes the second one with
the play-for-free prompt but passes the first, exile-selection filter to the
chooser. That mismatch is preserved. Its compaction loop also retains the
final `ic[k + 1]` read through the old count. Native owner checks, casting from
`1 - player`, repeated can-cast probes, payment/detach/sacrifice ordering and
conditional detached cleanup remain unchanged. The full 17-word activation
dialog is restored, including the decompiler's missing ability arguments.

Livio restores its two activation choices and complete 21-word consent dialog,
including controller, target display, availability and AI weights. The Aegis
legacy records original ID, owner, display image and native `0x300` text flags.
Its return path uses the native **100-by-2 integer table**, sharing a native
stack region with the separate 100-byte prompt buffer. It collects in descending
active-player/nonactive-player order and processes the list in reverse. No
additional capacity guard is introduced; the source retains the release's
behavior. The controller's consent remains necessary before exiling.

Dauthi restores the **31-by-3 integer table**, all 93 zero-initialized words,
owner/ID/effect rows and both full **59-word dialog pages**. Each page has fifteen
card choices, navigation and four trailing zero words. Card values are evaluated
in native descending order, including inactive zero rows; native navigation gates
and weights are retained. The original one-based fill can write index 31 despite
the 31-row declaration, and selection adds no new row guard. Those native bounds
quirks are documented rather than corrected. The private constructor's entry
parameters occupy EAX/EDX/ECX according to its initial DWARF location-list ranges;
the recovered source keeps the same three logical arguments and private ownership.
Void and Aegis counter values are respectively 170 and 173.

The six restored dialog calls contain **193 argument words** in total. Native
literals, control opcodes, callback pointers, filter/target layouts, local array
bounds and the Chemister allocation/compaction/filter operands are replayed by
the provenance audit. The ledger reaches **4,560 functions: 3,312 `card_*`
functions and 1,248 helpers**, including 584 private functions (575 private
helpers) and 673 public helpers. All 4,555 previous records and C token prefixes
survive unchanged, along with headers, globals, fragments, masks and chooser
stubs. All 143 C units, the C++ stub unit, source/ABI/provenance/compiler-part
checks and the GCC 9.3 PE32 build pass. Binary verification checks all four new
public symbols, the private constructor and both callbacks, the fixed tables,
VLA shape and distinct filters. The inventory now finds **11 missing card-handler
names**. No game code was executed; gameplay integration remains downstream.

Reccmp metadata now contains 18,459 pairs and 4,439 ledger mappings. All five
additions and the now-emitted Aegis/Void callbacks map directly. Current
release/build DLL/DBG and CSV identities are verified; the older HTML report
remains unchanged.


## Selection/storage and deferred chooser callers (pass 112)

Six handlers are recovered in their original source files: Ormos, Archive Keeper;
Pelt Collector; Blaze of Glory; Dispossess; Hot Fix; and Mise. This adds 4,566
native instruction bytes and raises the ledger to **4,566 functions: 3,318
`card_*` functions and 1,248 helpers**. The 584 private functions, 575 private
helpers, 673 public helpers, 143 checked C units and 121 recovered globals are
unchanged. All 4,560 previous records and C token prefixes survive unchanged,
along with headers, imports, shared registries, chooser stubs and the Git index.

Ormos retains its two-by-three sentinel array, two-row hand VLA, distinct-card
callback and native choice/payment/discard order. Its CAN_ACTIVATE path stores
the first distinct IID in slot one, and its selection loop compares zone indices
with saved hand positions. These native indexing quirks are preserved.
Pelt Collector retains 21 power slots split between target player/card fields,
the pending byte at offset 204, resolved byte at offset 208, the four-byte power
VLA and native sorting. It increments the pending low byte before looking for
a free slot and clears only nine target pairs afterward. No bounds guard or
extra cleanup is introduced.

Hot Fix retains the AI/speculation branches, wrapped 32-bit clock subtraction,
packet 28 send/receive arguments and EXE tag/payload globals. Blaze of Glory
reads the runtime EXE legacy global at `0x728368` and keeps its signed phase
limit and generic-spell dispatch after destruction. Dispossess retains its
fourteen-word dialog and unchecked no-artifact fallback.

The native Dispossess call targets the DLL's three-argument `choose_a_card`
wrapper at `0x26ef720`, not the EXE binding with that name. Both native thin
wrappers call the same implementation with identical options; the three-argument
entry supplies a NULL filter. Its recovered caller therefore delegates to the
existing typed `choose_a_card_test_ct` stub with NULL. Mise uses that typed
entry with its 216-byte filter. These two card bodies are recovered, but their
interactive named-card selection still depends on the deferred dialog UI.
The stub continues to return -1; native unconditional uses are retained.
No dialog internals are added or counted as recovered helpers.

All source/ABI, provenance and compiler-part checks and the GCC 9.3 PE32 build
pass. Binary verification checks all six public symbols, Ormos's fixed/VLA
arrays, Pelt's VLA and three distinct 216-byte filter locals. A fresh inventory
against native names, accepted records and rebuilt executable COFF symbols
finds **five missing card-handler names**: Nicol Bolas, God-Pharaoh; Tamiyo,
Collector of Tales; Nicol Bolas, Dragon-God; God-Eternal Kefnet; and Aryel,
Knight of Windgrace. This inventory does not measure modified existing 2020
handlers or remaining helpers. No game code was executed.

Reccmp metadata now contains **18,465 pairs and 4,445 ledger mappings**. All six
additions map directly; current release/build DLL/DBG and CSV hashes are verified.
The older HTML report remains unchanged.


## Final missing named card handlers (pass 113)

Nicol Bolas, God-Pharaoh; Tamiyo, Collector of Tales; Nicol Bolas, Dragon-God;
God-Eternal Kefnet; and Aryel, Knight of Windgrace are recovered in their original
three set files. Their 9,796 native instruction bytes include 61 canonical
calls, five indirect handler dispatches and seven full dialogs containing
173 argument words. The ledger reaches **4,571 functions: 3,323 `card_*`
functions and 1,248 helpers**. All 4,566 previous records and C token prefixes,
headers, imports, globals, fragments, shared registries, chooser stubs and the
Git index survive unchanged.

God-Pharaoh retains its count-deck-sized exile VLA, AP/NAP reveal, unchecked
activation target and native mode-9 exile permission. Tamiyo retains a 501-entry
array of eight-byte count/IID pairs, the first maximum count and its forward
removal loop, which advances after compacting the deck. In the human branch it
initializes `test2` but passes the distinct `test` to the named-card chooser.
The native mismatch is preserved. That chooser is still the existing deferred
UI stub; Tamiyo's interactive naming remains pending alongside Mise/Dispossess.

Dragon-God retains all five indirect calls to other card handlers. Its native
14-IID table still checks `ac > 14`, potentially allowing a fifteenth write;
no capacity repair is substituted. All fourteen names, including inactive zero
entries, are looked up in reverse order before the 53-word nested chooser.
The selection index remains unchecked. The first scan sets/clears player bit
`0x40000`; the nested scan saves/restores the complete player words. The stored
mode is byte `0x52`, and the selected IID is word `0x4c`.

Kefnet retains its separate replacement/drawn triggers, repeated instance reads,
all native mana-availability branches, cost reduction, copy/payment calls and
cleanup timing. The branches cover artifacts, Phyrexian mana, both hybrid forms,
Reaper King and the graveyard creature adjustment. The dialog deliberately uses
`EVENT_RESOLVE_ACTIVATION` inside the draw-trigger resolution, matching native code.
Aryel retains its target-count-sized tap VLA, repeated availability queries and
payment/untargettable/tap/cancellation order. The 75-byte formatted prompt and
76-byte token are distinct source objects. The native token write is the
four-byte special-keyword word at offset 36, verified by a source assertion,
the original instruction and rebuilt DWARF.

All 143 C units, the C++ stub unit, source/ABI, provenance and compiler-part
checks and the GCC 9.3 PE32 build pass. Rebuilt debug checks cover the five new
public symbols, three fixed arrays, two VLAs, three 216-byte filters and Aryel's
token layout. The VLA upper bounds use the native DWARF expression
`DW_OP_breg0 -1`; they are not interpreted as constant array lengths.

The [function inventory](recovery-evidence/function-inventory.json), reproduced
with `python tools/recovery/inventory_missing_functions.py --output .recovery/function-inventory.json`, compares the native range/name inventory,
accepted addresses and rebuilt executable COFF symbols. It finds **zero missing
card-handler names across 15,066 native ranges and 15,064 distinct names**.
This does not establish complete 2020-to-2022 modification coverage or gameplay
correctness. At pass 113, the 116 unmatched non-card ranges needed separate review: 63 already
have source bodies whose code emission is absent, 15 belong to the deferred
chooser UI unit, and 38 lack a body under that name in the named source unit.
The last group includes compiler/C++ artifacts and possible aliases as well as
helper candidates; it is not a count of proven missing implementations.

Reccmp metadata contains **18,470 pairs and 4,450 ledger mappings**. All five
additions map directly, and current release/build DLL/DBG and CSV hashes are
verified. No game code was executed; runtime integration remains downstream.


## Modified card/legacy families (pass 114)

Four existing card identities now have recovered 2022 versions beside their
original implementations: Bruna, Light of Alabaster; Personal Incarnation;
Toshiro Umezawa; and Seht's Tiger. The `_2022` entry points and five recovered
legacy/selection helpers add **nine function implementations**, covering 4,854
native instruction bytes, 34 canonical calls and seven callback references.
The ledger reaches **4,580 functions: 3,327 `card_*` functions and 1,253 helpers**,
including 588 private functions (579 private helpers) and 674 public helpers.
All 4,571 previous accepted records, C token prefixes and the Git index survive
unchanged. The older card entry points and two retained helper bodies are
hash-pinned; downstream dispatch is not migrated by this recovery batch.

Bruna's native `.isra.0` worker has two live register arguments, EAX and EDX.
DWARF describes the original event argument through a parameter reference, but
it is absent from the physical worker. The fresh export uses the verified
two-argument signature. Separate source objects represent the native reused
stack slot's 84-byte target and 216-byte filter lifetimes. The existing
`check_targets_for_bruna` implementation is retained, including the zero-versus
negative-result behavior used by the native loops; hand/grave tutor ordering
and human cancellation are preserved.

Personal Incarnation gains the public `effect_personal_incarnation` legacy and
private `is_damaging_stored_creature` predicate. Its native ownership-transfer
path creates a temporary card under the other player, creates the EXE activated
legacy using the runtime word at `0x739d5c`, stores the callback and original
creature coordinates, then obliterates the temporary card. The positive-life
death gate, inclusive active-card-count scan, repeated owner/state reads,
separate attached/stored damage predicates and redirection rules are retained.
One declaration is appended to `manalink.h`; existing declarations, other
headers, import definitions, globals and shared registries are preserved.

Toshiro gains its native attached legacy and qualified card handler together.
The legacy follows control, detaches when its source dies, clears status
`0x10000` and writes byte 206 in the pending-trigger state. The legacy masks its
resolved count to the low byte; the parent handler uses the full result. Both
selection loops retain two-row, four-byte VLAs, repeated candidate scans,
failed-payment retries, cancellation and the post-cast candidate decrement.
The native no-candidate retry and unchecked storage behavior are preserved.

Seht's Tiger gains its protection legacy with player bit `0x04000000`, the
high-byte protection-event tag 6 and expiry at turn start. Its card handler
retains cleanup of the info slot and the x86 color shift masked to five bits.
The original color-choice UI is used; the deferred Wish/card-picker unit is
unchanged.

All 143 C units and the C++ stub unit pass source/ABI checks. Provenance and
compiler-part audits pass, as does the GCC 9.3 PE32 build. Binary inspection
verifies five new public symbols, all four private helpers, both Toshiro VLAs
and Bruna's separate target/filter layouts. No game code was executed.
Reccmp contains **18,475 pairs and 4,459 ledger mappings**; all nine native
addresses map directly to their recovered implementations, with input and CSV
hashes verified. The current function inventory still has zero missing
card-handler names. Unmatched non-card ranges fall from 116 to 111: 63 existing
source bodies without emitted code, 15 deferred chooser UI ranges and 33
source-absent helper/compiler-artifact candidates requiring further review.


## Havengul permissions and utility boundaries (pass 115)

Havengul Lich's 2022 handler and five private helpers are recovered together,
alongside three public helpers for commander state, forced-expansion viewing
and fixed mana production. The batch adds **nine implementations** covering
4,025 native bytes, 27 canonical call sites, five callback references and five
indirect handler-dispatch sites. The ledger reaches **4,589 functions: 3,328
`card_*` functions and 1,261 helpers**, including 593 private functions (584
private helpers) and 677 public helpers. All 4,580 previous records, C token
prefixes and the Git index survive unchanged. The old Havengul card, its older
helpers and the already-recovered attachment callback are hash-pinned.

Two compiler-specialized interfaces are verified before fresh export. The
creature-in-grave query has only player/card in EAX/EDX and checks both
graveyards; its logical `t_player` parameter is absent. The attached-legacy
finder has player/card/target-player in EAX/EDX/ECX and a target-card stack word;
its logical function-pointer argument is fixed to `havengul_lich_legacy`.
The qualified source workers preserve those physical signatures. The new
four-argument selection predicate uses its callback player directly, rather
than reading the older implementation's saved target controller.

The permission legacy records the graveyard owner, source ID, position hint,
IID, source creature and mode, then relocates the grave entry by source ID
before activation. It snapshots state before destroying itself and retains
native payment, play/exile/control and borrowed-ability storage behavior.
The can-use helper scans only card words in target pairs 1 through 18. The main
handler scans both words in those pairs into the native **eight-IID array**,
without adding a capacity guard. The differing scans and unchecked capacity
remain explicit. Borrowed activation state uses tag `0x80000000`, excludes
that exact sentinel and resolves through native IID mask `0x0fffffff`.
The original missing-position/played-card fallback behavior is also retained.

`is_commander_in_command_zone_2022` checks the nonnegative commander-state word
and bits 0/8. `show_deck_force_expansion_2022` prepares the viewer, sets the
expansion, calls the existing six-argument EXE viewer at `0x479360`, then resets
the expansion global to zero. `mana_producer_fixed_2022` preserves added-color,
land-transmutation and per-color override precedence. Its fallback is the
existing four-argument EXE entry at `0x47b6c0`, verified from the original tail
jump; the initial zero-argument decompile is not used. Three declarations are
appended to `manalink.h`; existing EXE/linker bindings, globals, imports, shared
registries and deferred chooser stubs remain unchanged. The viewer wrapper
uses the established UI boundary without recovering dialog internals.

All 143 C units, the C++ stub unit, source/ABI, provenance and compiler-part
checks and the GCC 9.3 PE32 build pass. Binary verification checks four new
public symbols, five private helpers, the eight-IID array, both 216-byte filter
locals and the viewer/mana EXE targets. The newly referenced retained
`havengul_lich_legacy` is now emitted and checked as a private callback; it is
not counted as a new implementation. No game code was executed.

Reccmp contains **18,482 pairs and 4,469 ledger mappings**. All nine additions
and the retained attachment callback map directly; current inputs and CSV
hashes are verified. The function inventory still finds zero missing card-handler
names. Unmatched non-card ranges fall to 105: 62 existing source bodies without
emitted code, 15 deferred chooser UI ranges and 28 source-absent helper/compiler
candidates requiring review. Downstream dispatch and gameplay integration remain
separate work.

## Checks performed

- The accepted batch passed a combined 32-bit C syntax/interface check against
  the current `manalink.h` and `defs.h`. This also rejected missing dependencies,
  incompatible argument lists, unresolved identifiers and uninitialized locals
  reported by the checker.
- All 143 complete modified/new source units passed Clang's 32-bit C syntax
  checks. `functions/rules_engine.c` needs the checker-only Windows compatibility
  definition `strncasecmp=_strnicmp` for existing configuration parsing code.
  Its game source was not changed for that portability issue.
- Recovered string/character literals were checked against the raw exports.
- All 4,589 recorded raw-export hashes and source provenance markers were checked,
  along with supporting exports, forty-four source fragments, 121 recovered global
  declarations, local enum values, array endpoint replacements and 1,908
  recovered literal arguments, plus two independently verified literal pointer
  stores, nineteen reconstructed literal references and 2,240 manually
  reconstructed/qualified function body hashes.
- The two out-of-scope dialog stubs are source-hash checked and compiled as one
  32-bit C++ syntax unit. They are tracked independently of recovered native
  implementation counts and native-behavior claims.
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

The latest ordinary screening accepts no additional functions. The cached
reanimation callers are now recovered; further progress requires focused
repairs to other machine-state/layout and helper-interface groups. Resolve
those interfaces and layouts, then rerun promotion to recover their dependent
card groups. This does not require a whole-program diff.
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
- `migrate_token_callback_records.py`: migrates four native token callbacks
  and four earlier consumers with exact predecessor replay, preserving earlier
  scalar, stack and local enum evidence.
- `recover_token_delays.py`: recovers the native haste/delayed-removal family,
  its physical private-worker interface and token callback frontends, retaining
  the original APIs and the native negative-ID constructor branch.
- `recover_callback_token_cards.py`: recovers Legion Warboss and Tidal Wave
  with typed native callbacks, defender-mask evidence and the EXE query binding.
- `recover_combat_mask_cards.py`: verifies caller-specific flag masks, the
  native legal-attacker boundary, Partner switch tables and signed Partner-with
  lookup, then recovers their card consumers. Its follow-up mode records the
  Defiling Tears and Tin Street Dodger callback operands separately.
- `ApplyRecoveryReanimation.java` and `recover_reanimation.py`: recover the
  concrete five-argument mass worker, eight-argument single worker, typed
  callbacks, signed action domain and native array storage; retain old APIs.
- `recover_reanimation_cards.py`: recover five callers with exact prompt
  copy/zero-fill evidence and Calix's separate meld-result lifetime, binding
  native calls and private callbacks.
- `recover_reanimation_tail.py` and `reanimation_vla_body.py`: recover the
  three remaining cached reanimation callers with exact packed-byte and
  variadic-argument sites, referenced DWARF VLA bounds, allocation/loop ranges
  and recorded native selection/state quirks.
- `recover_protection_frontends.py`: reconstruct four inlined public-wrapper
  guards, verify their private worker/parameter/local storage, and recover
  Tanazir's separate snapshot lifetime and reverse iteration. Its follow-up
  mode records Shanna's native protection-frontend call.
- `recover_leaves_test_native.py`: move two accepted helper records to the
  native test API while recording exact compatibility fragments; migrate
  three callers with reversible type/API substitutions and replay earlier
  scalar/stack proofs. Its follow-up mode binds the Ozolith's native operands.
- `recover_native_aggregate_consumers.py`: recover native area damage and
  three consumers with 216-byte shared target/test unions; verify per-unit
  layouts, native calls, Arahbo's EXE boundary and Plague Reaver's private
  callback while retaining the historical stack registry and legacy APIs.
- `recover_saheeli_directive.py` and `saheeli_directive_body.py`: reconstruct
  the complete native VLA/variadic call frame with eight full range proofs,
  concrete array-bound DWARF, native prompt bytes and preserved event,
  Cage/zero-count, selection/compaction and restoration/mill paths.
- `recover_aer_helper_callers.py`: recover the two-register Expertise helper
  using `ApplyRecoveryCloneInterfaces.java`, pin four fresh caller exports and
  bind fifteen accepted handlers through eighteen verified call/tail sites.
  Preserve predecessor records and replay earlier scalar proofs through exact
  reversible source views.
- `ApplyRecoverySpellCost.java` and `recover_spell_cost.py`: restore three
  typed EXE boundaries and signed byte views, recover spell-cost payment and
  Improvise/card consumers, and verify local/global index algebra, native odd
  rollback/snapshot behavior, post-initializer stores and the modifier migration.
- `ApplyRecoveryMemorial.java` and `recover_memorial_family.py`: apply the
  concrete fourteen-argument native Memorial signature, recover five lands and
  migrate five existing resolver interfaces. Verify callback dispatch, AI priority,
  dialog bytes, target/test layouts, unsigned event guard and reversible history.
- `recover_embalm_family.py`: verify and prepare the nine-argument Embalm
  specialization, recover the worker and ten consumers, and check sixty caller
  constants, native token/test/target layouts, events, flags and preserved legacy
  helper/callback bodies. `verify_native_build.py` checks its private COFF symbol.
- `recover_mana_predicate_cards.py`: recover two artifact-mana frontends,
  two shared targeting callbacks and three consumers; verify specialized
  worker arguments, native calls/callbacks, target layouts, engine error
  pointer, unsigned color shift and retained legacy bodies.
- `recover_storm_inlines.py` and `storm_inline_bodies.py`: reconstruct two
  logical native routines from nested DWARF inline ranges; verify event gates,
  trap/identifier meanings, native calls, Werewolf stores and preserved
  dispatcher boundaries without inventing standalone machine entries.
- `recover_storm_family.py`: recover the native increment/readers, hand-type
  counter and three card consumers; verify mode/counter domains, retained
  trap-support call arguments, packed backing state and legacy collector scope.
- `recover_public_card_callbacks.py`: recover the shared bounce and artifact
  callbacks plus four consumers; verify scoped native bindings, public/private
  homonym signatures, cross-unit calls, target layout and retained legacy bodies.
- `recover_commander_consumers.py`: recover War Room, Hellkite Courser,
  the both-commanders compiler specialization and the public command-zone
  wrapper; verify constant/physical arguments, native calls, legacy private
  compatibility and the compiled life-adjustment behavior.
- `recover_snow_consumers.py`: recover the shared snow predicate, private
  nonsnow validator, Zombie Musher and Hallowed Ground; verify public/private
  linkage, cross-unit calls, target layout and the exact EXE error pointer.
- `recover_flash_casting.py`: recover Vivien and native casting-as-flash,
  distinguish two private same-named mana callbacks and their event globals,
  and reconstruct the signed deck-count local from DWARF and native stores.
- `recover_disciples.py`: recover the private color-mask worker and five
  Disciple tail wrappers; verify native parameter locations, target layouts,
  controller-store ordering, dialog operands and exact previous wrapper bodies.
- `recover_reveal_destroy.py` and `reveal_destroy_bodies.py`: recover the
  native reveal helper, destruction tracker/constructor and Creativity; verify
  physical parameters, two DWARF VLA bound forms, count-sized allocation sites,
  mixed byte/halfword arithmetic, read-only prompts and unchanged legacy bodies.
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
For Expertise exports, run `recover_aer_helper_callers.py --prepare` to verify
the two-register signature and write `.recovery/aer-free-spell-clone.tsv` and
its five export requests. Apply `ApplyRecoveryCloneInterfaces.java` with that
TSV before exporting the helper and four callers. The caller exports are pinned
separately to preserve the earlier canonical raw hashes.
For spell-cost exports, apply `ApplyRecoverySpellCost.java` before exporting
payment and its consumers. It types the three EXE boundaries and two signed
byte views; the source reconstruction verifies their DLL materializations and
actual EXE bytes, local snapshot layout and address/index algebra.
For Memorial exports, apply `ApplyRecoveryMemorial.java` to the imported
abstract signature before `ExportRecovery.java`. It removes the nonphysical
`ai` parameter while retaining native target and resolver types. The recovery
script verifies concrete DWARF register/stack locations and the GNU reference.
For the Hogaak family, run `recover_hogaak_family.py --prepare`, apply
`ApplyRecoveryCloneInterfaces.java` with `.recovery/hogaak-clone-interface.tsv`,
and export `.recovery/export-request-hogaak-family.tsv`. The companion
`hogaak_bodies.py` expresses the typed trial/payment reconstruction. `--apply`
checks the physical worker interface, three hybrid call frames, native array
ranks/strides, formatting operand, target shift and dependency operands before
compiling and appending the seven functions.
For the letter-name/text-effect family, run `recover_letter_effects.py --prepare`,
apply `ApplyRecoveryCloneInterfaces.java` with `.recovery/letter-effects-clone.tsv`,
then export the nine entries in `.recovery/export-request-letter-effects.tsv`.
The private `txtfx.part.0` body has three physical register arguments and is
named `txtfx_name_2022`. `--apply` checks the complete native folding and
consonant tables, exact long-name literals, bitmap addressing, callback
operands and original linkage before appending the complete batch.
For rules-text consumers, run `recover_rules_text_consumers.py --prepare` and
export its four requests with `ExportRecovery.java`. The companion
`rules_text_bodies.py` restores the typed VLA and back-face pattern setup.
`--apply` compiles the helper, both new cards and repaired Bloodletter together,
verifies native literals/imports/field addresses and dependency operands, and
retains the repaired card's exact previous record and body.
For clock/redraw consumers, run `recover_clock_cards.py --prepare` and export
its eight requests. `--apply` checks Windows structure/buffer DWARF, the four
native API imports, signed runtime divider, date format and both redraw frames
before compiling and appending the helper/card batch. The shared audit replays
the reconstruction; the native build verifier also checks the date buffer's
private BSS symbol and the rebuilt Windows imports.
For pack utilities, run `recover_pack_utilities.py --prepare`, apply
`ApplyRecoveryCloneInterfaces.java` with `.recovery/pack-utility-clones.tsv`,
then export its five requests. `pack_native_tables.py` decodes native array
bounds, typed records and pointer identities. `--apply` compiles the complete
new source unit against the extended expansion header and checks every table,
helper interface, diagnostic and native dependency before appending the batch.
For weighted pack generation, run `recover_pack_generator.py --prepare` and
export its three requests after applying the pack utility clone interfaces.
`pack_generator_tables.py` checks all weighted arrays, choice-pool bounds,
native initializer pointers and local once storage. `--apply` restores the
complete generator/wrapper/self-check batch, compiles it and records native
layout, diagnostic/import, dependency and scoped reconstruction evidence.
For the pack chooser/card consumers, run `recover_pack_chooser.py --prepare`.
Apply `ApplyRecoveryPackDialogInterfaces.java` with the generated
`pack-dialog-imports.tsv` and `ApplyRecoveryRuntime.java` with
`pack-dialog-exe.tsv`, then export the five requests. `pack_chooser_bodies.py`
contains the typed dialog and VLA projections. `--apply` checks native template
bytes, storage, literals, packet frames, IAT/dependency operands and all source
bodies before appending the batch. The shared provenance audit replays this
projection; native-build verification checks the emitted callback, resource,
local statics and imports.
For alternate hand/graveyard casting, run `recover_alternate_casting.py --prepare`
and export the ten requests with `ExportRecovery.java`. The companion
`alternate_casting_bodies.py` restores the six typed VLA handlers. `--apply`
checks native allocations, filter layouts, retained older APIs/callbacks,
exact prompts/control bytes and all dependency operands before compiling and
appending the batch. The shared audit replays this projection, while native
build verification checks both filter generations and all six array shapes.
For the exile/return handlers, run `recover_exile_return_cards.py --prepare`
and export its five requests. `exile_return_card_bodies.py` restores the typed
packed words, array/record views, separate Chemister filters and complete dialog
calls. `--apply` checks original DWARF/native storage, all six modal argument
counts, literals, allocation/compaction/filter operands and private callbacks
before compiling and appending the batch. The shared provenance audit replays
the projection; native-build verification checks the three private symbols,
fixed/VLA shapes and both Chemister filters.
For the Havengul/utility batch, run `recover_havengul_utility_helpers.py --prepare`,
apply `ApplyRecoveryCloneInterfaces.java` with `.recovery/havengul-utility-clones.tsv`,
and export `.recovery/export-request-havengul-utility-helpers.tsv` into
`.recovery/decompiled-havengul-utility-helpers`. The companion
`havengul_utility_bodies.py` restores the typed projections. `--apply` validates
physical parameters, fixed storage, filter layout, legacy/state operands,
callback identities, utility EXE boundaries and retained bodies before compiling
and appending the nine implementations. The shared provenance audit replays the
proof; native-build checks cover private symbols, storage/filter sizes, EXE
calls and the retained attachment callback. Original Havengul entry/helper
bodies and the EXE mana binding remain available beside the qualified versions.
For modified Bruna/Personal Incarnation/Toshiro/Seht's Tiger families, run
`recover_legacy_helper_families.py --prepare`, apply
`ApplyRecoveryCloneInterfaces.java` with `.recovery/legacy-helper-family-clones.tsv`,
and export `.recovery/export-request-legacy-helper-families.tsv` into
`.recovery/decompiled-legacy-helper-families`. The companion
`legacy_helper_family_bodies.py` restores the typed bodies. `--apply` verifies
native linkage, physical arguments, VLA bounds/allocations, packed state,
legacy/protection operands and retained dependencies before compiling and
appending the nine implementations. The shared provenance audit replays the
proof; the native build verifier checks private symbols, VLA shapes and Bruna
layouts. Existing card implementations retain their names; recovered versions
use `_2022` until downstream dispatch migration.
For the graveyard-selection and dynamic-stack family, run
`recover_graveyard_selection_family.py --prepare` and export the nine requests
in `.recovery/export-request-graveyard-selection-family.tsv`. The companion
`graveyard_selection_bodies.py` expresses the typed VLA reconstructions.
`--apply` verifies the native ranks, allocation/call frames, formatting operand,
probe artifacts, flag views and callback operands before compiling the batch;
the shared provenance audit replays the same reconstruction.
For permanent-copy/exile-play and field-consumer exports, run
`recover_copy_exile_consumers.py --prepare` and export the fifteen requests
in `.recovery/export-request-copy-exile-consumers.tsv`. Its `--apply` mode
checks both helper families, native linkage, physical/local type evidence,
stack-probe artifact, fixed prompt, field/table widths and dependency operands
before compiling and appending the complete batch.
For packed-field/filter/Overload exports, run
`recover_packed_filter_consumers.py --prepare` and export the fifteen requests
in `.recovery/export-request-packed-filter-consumers.tsv`. Its `--apply` mode
verifies the native field layouts, stack phases, fixed table, prompt copies,
packed token argument and native dependency operands before appending the batch.
For graveyard/pump/target consumer exports, run
`recover_grave_pump_consumers.py --prepare` and export the thirteen requests in
`.recovery/export-request-grave-pump-consumers.tsv`. Its `--apply` mode validates
the native iterator, layouts, callbacks, prompt and event ranges, then appends
the complete batch and registers any newly checked source unit.
For Spectacle/Amplify and filtering exports, run
`recover_casting_reveal_family.py --prepare`, apply
`ApplyRecoveryCloneInterfaces.java` with `.recovery/casting-reveal-clones.tsv`,
and export `.recovery/export-request-casting-reveal-family.tsv`. The `--apply`
mode validates all fifteen functions, native helper dependencies, fixed arrays,
coercion layout and caller frames before appending the complete batch.
For combat-AI consumer exports, run `recover_combat_ai_consumers.py --prepare`
and apply `ApplyRecoveryCombatGlobals.java` before exporting the thirty entries
in `.recovery/export-request-combat-ai-consumers.tsv`. Its `--apply` mode checks
the initializer and every consumer operand, compiles all fourteen candidate
source units together and appends the functions to their original set files.
For Escape/counter/target-consumer exports, run
`recover_card_helper_batch.py --prepare` and apply
`ApplyRecoveryCloneInterfaces.java` with `.recovery/escape-clone-interface.tsv`
before exporting the ten requests in `.recovery/export-request-card-helper-batch.tsv`.
`recover_card_helper_batch.py --apply` compiles and audits the complete batch
before appending it; the shared provenance audit subsequently replays the proof.
For Embalm exports, run `recover_embalm_family.py --prepare` to validate the
black-mana-zero specialization and create `.recovery/embalm-clone-interface.tsv`
and its request list. Apply `ApplyRecoveryCloneInterfaces.java` with that TSV
before exporting the worker and ten consumers. The physical signature has
three register parameters and six stack words; no black-mana argument exists.
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
