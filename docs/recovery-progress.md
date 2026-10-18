# October 2022 source recovery

Implemented directly in the `cards/` and `functions/` source files on 2026-10-17.
Missing target set files were created with `#include "manalink.h"`; implementations
already present in the source were preserved.

## Recovered source

- **3,255 function implementations:** 2,647 functions named `card_*` and 608
  helpers, in **130 checked C source units**, including units containing only
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
- 18 previously missing C files were created. They include Strixhaven, Modern
  Horizons 1/2, Commander Legends, Throne of Eldraine, War of the Spark,
  Zendikar Rising, Commander 2020, and `functions/draft.c`.
- 398 recovered functions retain private/static linkage, including six private
  implementation helpers whose names begin with `card_`. There are 216 recovered
  public helpers; shared declarations live in the existing `manalink.h`.
- Every function has its target address and original DWARF source unit/line
  recorded immediately above its implementation.
- Of the recovered symbols, 2,636 are referenced by the target registration
  records; 2,559 serve at least one record marked CODED. The remaining functions
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

These declarations and adapters do not complete engine integration. The newer
ability check expects bitmask writers rather than the legacy nibble counters;
new event constants still need their dispatch pathways; the power/toughness
globals need the newer dispatcher updates. The existing shared `EVENT_CANT_CREW`
value also still differs from the target release. Twenty-two recovered string
arguments use the newer `DLGIMPL_MODE` dialog control (`0x1b`); its parser support
is not present in the existing dialog implementation. The extension pointer at
`0x2c` remains unprojected, and no allocation/ownership behavior was invented.
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
- All 130 complete modified/new source units passed Clang's 32-bit C syntax
  checks. `functions/rules_engine.c` needs the checker-only Windows compatibility
  definition `strncasecmp=_strnicmp` for existing configuration parsing code.
  Its game source was not changed for that portability issue.
- Recovered string/character literals were checked against the raw exports.
- All 3,255 recorded raw-export hashes and source provenance markers were checked,
  along with supporting exports, six source fragments, recovered global
  declarations, local enum values, array endpoint replacements and 1,304
  recovered literal arguments.
- Compile-time layout checks confirm the 300-byte card instance and recovered
  member offsets, including `mode` at `0x37` and `targets` at `0x74`.
- Compile-time checks also confirm the card-count cache is 14 bytes with a
  seven-byte row stride. The pump specialization checks all nine common member
  offsets and the retained 36-byte source structure.
- Source whitespace was checked, and original implementations were retained.

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
layouts, particularly the newer `card_instance_t` extension pointer and storage.
Others require new types/constants, changed helper signatures, C++ recovery,
or dependencies that were not ready for promotion. The spell/Magecraft core now
has a narrow flag-accessor adaptation, but broader extension-backed helpers
such as `inst_info` remain deferred. Source compatibility does not validate
copied-spell trigger delivery or complete the ability-mask writer migration.

The final screening accepted no further functions under the current checks.
Continue by resolving those interfaces and layouts, then rerun promotion to
recover their dependent card groups. This does not require a whole-program diff.
The tools preserve source-unit placement and retain unresolved work for the
next pass. Static helpers with repeated names must be resolved in their own
source unit, and optimized split/ranged functions need their full range lists.

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
The prior [recovery plan](recovery-plan.md) and
[registration trace](registration-trace.md) contain the input identities and
registration details.
