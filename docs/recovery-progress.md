# October 2022 source recovery

Implemented directly in the `cards/` and `functions/` source files, starting
2026-10-17. Latest recovery pass: 2026-10-18.
Missing target set files were created with `#include "manalink.h"`; implementations
already present in the source were retained, with scoped repairs and lifecycle
fragments recorded separately.

## Recovered source

- **3,496 function implementations:** 2,815 functions named `card_*` and 681
  helpers, in **136 checked C source units**, including units containing only
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
- 21 previously missing C files were created. They include Strixhaven, Modern
  Horizons 1/2, Commander Legends, Throne of Eldraine, War of the Spark,
  Zendikar Rising, Commander 2020, and `functions/draft.c`.
- 421 recovered functions retain private/static linkage, including nine private
  implementation helpers whose names begin with `card_`. There are 269 recovered
  public helpers; shared declarations live in the existing `manalink.h`.
- Every function has its target address and original DWARF source unit/line
  recorded immediately above its implementation.
- Of the recovered symbols, 2,800 are referenced by the target registration
  records; 2,722 serve at least one record marked CODED. The remaining functions
  include callbacks and helper entry points. Counts are functions, not unique
  playable cards or completed mechanics. `cards/stubs.c::card_crab` preserves
  the target's no-op placeholder and is labeled accordingly.

Examples of recovered card handlers include Culling Ritual, Expressive Iteration,
Double Major, Curate, Environmental Sciences and Eureka Moment in
`cards/strixhaven.c`. Recovered shared helpers include `life_total_can_change`,
`how_much_life_can_i_pay`, `can_legally_target`, `choose_player`,
`cast_counter_triggered_ability`, `color_to_protection`, `get_aftermath`, and
`counter_triggered_target_definition`. Other helpers and legacy callbacks live
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
The canonical raw cache now contains 4,318 rows and 4,298 original names. Preparation
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
The Aura core also remains deferred: its target-definition struct is 84 bytes,
with a new `flags` field at offset 80, versus the existing 80-byte definition.
Its initialization and validation consumers need recovery together.

Some code retains decompiler-style temporaries and control flow. Numeric
constants are retained where a reliable symbolic reconstruction has not been
established. These are provisional reconstructed implementations, not claims of
original formatting, original macros or proven behavioral equivalence.

## Checks performed

- The accepted batch passed a combined 32-bit C syntax/interface check against
  the current `manalink.h` and `defs.h`. This also rejected missing dependencies,
  incompatible argument lists, unresolved identifiers and uninitialized locals
  reported by the checker.
- All 136 complete modified/new source units passed Clang's 32-bit C syntax
  checks. `functions/rules_engine.c` needs the checker-only Windows compatibility
  definition `strncasecmp=_strnicmp` for existing configuration parsing code.
  Its game source was not changed for that portability issue.
- Recovered string/character literals were checked against the raw exports.
- All 3,496 recorded raw-export hashes and source provenance markers were checked,
  along with supporting exports, twenty-two source fragments, twenty-six recovered global
  declarations, local enum values, array endpoint replacements and 1,441
  recovered literal arguments, plus two independently verified literal pointer
  stores, sixteen reconstructed literal references and twenty-nine manually
  reconstructed/qualified function body hashes.
- Reconstructed stack-storage body hashes were checked for all 83 affected
  functions, and the known cross-type local cast pattern is rejected.
- Compile-time layout checks confirm the 300-byte card instance and recovered
  member offsets, including `mode` at `0x37` and `targets` at `0x74`.
- Compile-time checks also confirm the card-count cache is 14 bytes with a
  seven-byte row stride. The pump specialization checks all nine common member
  offsets and the retained 36-byte source structure.
- The subtype cache checks confirm an eight-byte modifier record, its mode byte
  at offset four, a 128-byte player row and a 256-byte table.
- The query selector type is checked as four bytes.
- The resource-cache record is checked as 12 bytes with fields at offsets
  zero, four and eight.
- The dice selector is checked as four bytes and each live modifier array as
  eight bytes.
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

The final screening accepted no further functions under the current checks.
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
- `ExportRecovery.java`: exports requested functions with source provenance.
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
