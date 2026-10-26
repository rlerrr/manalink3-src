# Building the recovery branch

The 2026-10-25 recovery build successfully compiles, assembles and links a
32-bit Manalink DLL using the downloaded GCC/G++ 9.3 toolchain. This verifies
native linking; it does not establish gameplay correctness or complete 2022
runtime integration.

The current source uses one canonical October 2022 definition for each target,
test, token and pump record (84/216/76/48 bytes). Versioned typedefs alias those
definitions. Original initializer/helper names share recovered bodies; 371
pairs are checked as 368 same-address aliases and three typed const-return
tail adapters. Native DWARF also verifies seven restored shared stack unions.
The source/ABI check verifies every recovered record field and token action
value. Historical pre-migration sizes in earlier pass notes describe archived
inputs, not layouts emitted by the current build.

The preceding mana/activation/token batch consolidated **65 further pairs**. Its nine
reviewed duplicate candidates and 56 runtime bindings preserve all accepted
recovery records and declarations. The native verifier also follows the
canonical entry's direct edge to GCC's 442-byte mana-production `.part.0`
worker, validating both ranges without treating the outlined worker as an
independent recovery or interpreting its public wrapper as the complete body.

The next batch consolidates **44 more pairs**, including the three generic
activation entry points and 41 life-loss, pump, damage, targeting and ability
helpers. The activation prompts have checked `const char *` source annotations:
only their declarations change; every executable expression and accepted native
recovery record remains unchanged. Both names resolve to the same actual COFF
address. The full GCC build, 143-C-unit/one-stub syntax and ABI checks, and native
verification pass. Archived retained-body checks replay the recorded preimages.

The reanimation/spell/Aura/tutor batch shares **40 further pairs** and removes
one unused private predecessor. The original reanimation, generic-spell flag,
tutor destination and tutor origin type/constant names now use native 2022
values, with the versioned spellings aliasing them. Source checks pin every
value, signed reanimation operations, typedef identity and both native callback
types. The five changed C units reproduce exactly from their recorded preimages;
accepted recovery records and header declarations remain unchanged.

From the repository root in PowerShell:

```powershell
.\tools\build-recovery.ps1
python tools/recovery/verify_native_build.py
```

If local script execution is restricted, invoke the same script with:

```powershell
powershell -ExecutionPolicy Bypass -File tools/build-recovery.ps1
```

The default toolchain is `C:\bin\gcc-9.3.0-no-debug`; override it with
`-Toolchain`. The script uses the repository's make and yasm executables,
selects `-m32`, generates i386 import libraries using `--32` assembler flags,
and writes objects and logs under `.recovery/build-gcc93/`. Existing root
binary/debug/import artifacts are backed up there before being replaced.
It does not deploy files to a game installation.

Outputs are `ManalinkEh.dll` and `ManalinkEh.dbg` in the repository root. The
successful build uses 165 C files, two C++ files, 20 linked assembly objects
and the root veneer object. Verification checks PE32/i386 format, the image
base `0x02000000`, `DllEntry` at RVA `0x1000`, `CodeSectionEx` at `0x1006`,
nonempty DWARF data and the new batch's public symbols. It also checks i386
import members, source/object freshness and coverage of the existing C units
in both release compilation-unit inventories. Artifact hashes and details
are recorded in [native-build.json](recovery-evidence/native-build.json).

The pass-73 build includes twelve recovered reanimation helpers and five new
card handlers. All fourteen public additions are present in the rebuilt debug
symbols. Pass 74 adds Command the Dreadhorde, Dance of the Manse, and Eerie Ultimatum;
all three new card symbols are present. Pass 75 adds four protection helpers,
Tanazir Quandrix and Shanna, with all six public symbols verified. The recovery
ledger records 4,316 functions after pass 84 adds Ral, Tuvasa,
Thousand-Year Storm and eight storm/hand-count helpers. All eleven public
symbols and the retained private rules-card array are verified. Legacy
collection/reset integration remains pending. Pass 85 reconstructs both
inlined routines as source fragments, keeping the function/symbol counts
unchanged. Their old dispatcher wiring is retained; native trap and Rules
Engine ID differences are documented. Pass 86 brings the ledger to 4,323
functions, adding Firemind Vessel, Stream of Acid, Prismatic Circle and four
public mana/target helpers. All seven new public symbols are verified; the
existing distinct-color worker and legacy helper bodies remain unchanged.
Pass 87 raises the ledger to 4,334 functions with ten Embalm card handlers and
their private native worker. All ten new public symbols and the private
worker's local COFF linkage are verified. Its legacy helper/callback remain
unchanged; native token, target and test layouts pass the source checks.
Pass 88 raises the ledger to 4,340 functions with the five Memorial lands
and their private worker. Five existing resolver interfaces are upgraded to
native layouts and APIs, with exact predecessor records retained. The five
new public symbols and all six private worker/resolver symbols are verified.
Pass 89 raises the ledger to 4,345 functions with spell-cost payment,
Improvise worker/frontend, Inspiring Statuary and Skyclave Shade. One accepted
cost modifier gains native targeting. Four new public symbols and both private
Improvise helpers are verified, including the casting worker's GCC
`.constprop.0` symbol. Native signed-byte, snapshot and rollback oddities are
preserved and documented; those runtime paths have not been executed.
Pass 90 raises the ledger to 4,346 functions with the private Expertise
free-cast helper, whose local executable COFF symbol is verified. Fifteen
existing card handlers gain native helper bindings with eighteen native call
sites and exact predecessor history. The public-symbol count stays unchanged.
Pass 91 raises the ledger to 4,347 functions with Saheeli's Directive.
Its complete native VLA/selection/restore frame and public symbol are verified;
all prior records remain unchanged. The explicit zero-count, Cage and failed-add
paths are preserved for downstream gameplay validation.
Pass 92 raises the ledger to 4,357 functions with seven card handlers and
three shared helpers. A single full build verifies nine new public symbols,
the private Escape worker and the eight-byte private targeting result in BSS.
All prior recovery records and C token prefixes remain unchanged.
Pass 93 raises the ledger to 4,387 functions with 28 card handlers across
fourteen set files and two private Glittering callbacks. All 28 new public
symbols and both private callbacks are verified. Three EXE-owned combat-AI
accumulator bindings resolve the shared blocker; their native initialization
and all 95 consumer memory operands are audited without running the game.
Pass 94 raises the ledger to 4,402 functions with eight card handlers and
seven native helpers. All twelve new public symbols and two private worker
symbols are verified. GCC inlines Amplify into Aven Warhawk; its concrete
213-byte inline range, amount-one location, abstract origin, parent bounds and
eight call sites are checked in DWARF. Existing helper bodies remain unchanged.
The native 28-byte coercion definition is checked at compile time alongside
the existing test/target domains.
Pass 95 raises the ledger to 4,415 functions with seven card handlers and
six graveyard/pump/target helpers. All twelve new public symbols and the private
pump lifetime callback are verified. Source/ABI checks now cover 141 C units,
including the first recovered implementation in `functions/deadbox.c`. Two
qualified native-private predicates receive shared C linkage for Timecrafting;
the earlier static bodies remain unchanged.
Pass 96 raises the ledger to 4,430 functions with eleven card handlers and
four public Devil, Overload and creature-mana helpers. All fifteen new public
symbols are verified. Source/ABI checks now cover 142 C units, including
Return to Ravnica. Native packed fields, overlapping stack phases, fixed
initializers, prompt copies and dependency operands pass provenance checks.
Pass 97 raises the ledger to 4,445 functions with ten card handlers and five
permanent-copy/exile-play helpers. All fourteen new public symbols and the
private copy callback are verified. Native stack/local arrays, flag words,
fixed prompt copying, mana-override rows and event gates pass the source and
provenance checks. The two helper families retain their older bodies under the
existing interfaces; broader caller migration remains downstream.
Pass 98 raises the ledger to 4,454 functions with five card handlers and four
private graveyard-selection/combat-exile callbacks. Five public symbols and all
four private callbacks are verified. Private verification now uses the DWARF
owning compilation unit and concrete entry address, distinguishing the existing
end-of-combat callback homonym and Breach's compiler-outlined `.part.0` body.
Three two-row selection VLAs, Genesis's saved-ID VLA, Breach's conditional buffer
and Gyrus's formatting/token call frames pass source and native evidence checks.
Pass 99 raises the ledger to 4,461 functions with Hogaak and six native
Convoke/Delve/formatting helpers. All six new public symbols and the private
predicate are verified. The seven-argument optimized interface, four VLAs,
three hybrid constant frames, formatting operand, nine-pair reverse target
shift and native trial/payment edge cases pass the source and evidence checks.
Both older formatter/chooser bodies remain unchanged.
Pass 100 raises the ledger to 4,480 functions with fifteen card handlers and
four new helpers, plus a native-layout migration of the triggered-ability
eligibility helper. The build verifies all seventeen new public symbols, both
private callbacks, both names of the migrated/retained helper and the public
zero-initialized cycling word. Typed stack/target/info-slot views, Command enum
storage and endpoints, phase-specific Shadrix token storage, the one-argument
EXE boundary and native Scouting Trek extra-entry read are recorded separately
in the source/provenance evidence. Earlier caller integration and gameplay
validation remain downstream. Reccmp metadata is refreshed with 18,380 pairs
and 4,347 ledger mappings; all twenty processed bodies have explicit mappings.
Pass 101 raises the ledger to 4,494 functions with ten card handlers and four
activated-ability counter actions. All fourteen public symbols are verified.
The root Drawcardlib import definition gains the existing `get_config_data`
export needed by Symbol Status; the regenerated i386 archive has eleven
members and the output DLL's import is checked. Three local modal enums and
two-choice endpoints, nine full 84-byte target copies, exact subtype tables,
Lutri's partial-cleared VLA and the 198-byte expansion bitmap pass the source
and native evidence checks. Native uninitialized/out-of-bounds cases are
preserved and documented for downstream review; gameplay remains untested.
Reccmp metadata is refreshed with 18,392 pairs and 4,363 ledger mappings.
Thirteen additions map directly; the new raw-counter entry is a verified
five-byte jump to its retained compatibility body without a concrete DWARF
range, and is omitted by the current name matcher.
Pass 102 raises the ledger to 4,504 functions with seven library/token handlers
and three native Proliferate helpers. All ten new public symbols are verified.
Six dynamic arrays, fourteen selection/formatting/casting frames, exact prompt
copies, packed choice bytes and the shared 76-byte token phase pass the source
checks. A compound statement makes the token declaration legal after its label
in GCC 9. Native ownership, queue extra reads, Cage gates and early exits are
preserved for downstream review. The three legacy Proliferate bodies and
existing Drawcardlib imports remain unchanged.
Reccmp metadata has 18,401 pairs and 4,375 ledger mappings. Nine additions
map directly; the manual Proliferate helper is omitted by the name matcher
because its verified public entry and private `.part.0` body share a DWARF
name. Both concrete ranges are checked against their owning CU and COFF symbols.
Pass 103 raises the ledger to 4,512 functions with six target/hand-array
handlers and two native X-payment helpers. All eight new public symbols are
verified. Seven integer VLAs, four target displays, the full eighteen-word
Metzali dialog, independent hand/discard sizes, native extra row reads and
Mass Manipulation's target-player view pass the evidence and source checks.
Both legacy payment bodies, older selector-storage module, previous interfaces
and Drawcardlib imports are retained. Gameplay validation remains downstream.
Reccmp metadata has 18,407 pairs and 4,383 ledger mappings; all eight
additions have direct mappings and current input hashes are verified.
Pass 104 raises the ledger to 4,521 functions with Spark Fiend, Togglodyte,
Now I Know My ABC's, Staff of the Letter Magus and five private helpers.
All four public card symbols and five local helper symbols are verified.
Two 256-byte native character-fold tables, the 26-byte alphabet bitmap,
seventeen consonant pointers, long-name literals and the three-register
text-effect compiler part pass the reconstruction and provenance checks.
Reccmp metadata now has 18,419 pairs and 4,395 ledger mappings, including
all nine additions. Current DLL/DBG/CSV hashes are verified. The older HTML
report is unchanged; gameplay validation remains downstream.
Pass 105 raises the ledger to 4,524 functions with Punctuate, Capital Offense
and the shared rules-text normalizer, plus the repaired Bloodletter table
and native dependency bindings. All three new public symbols are verified.
The character VLA, two 300-byte next-card back-face patterns, native token and
reminder cursor behavior, punctuation bytes and `strpbrk` import pass the
reconstruction/provenance checks. Bloodletter's exact predecessor is retained;
the remaining previous bodies, globals, imports and GUI stubs are unchanged.
Reccmp metadata has 18,422 pairs and 4,398 ledger mappings. All three additions
and repaired Bloodletter map directly, with current input hashes verified.
Pass 106 raises the ledger to 4,532 functions with five Windows date/time/delay
helpers, Elvish House Party, Old-Fashioned Vampire and Snow Mercy. All eight
new public symbols, the persistent eighteen-byte private date buffer and four
Windows API imports are verified. Native field offsets, time packing,
performance-counter scaling/division, animation delays and both two-word redraw
frames pass the source and provenance checks. All earlier bodies and records
remain unchanged; gameplay validation remains downstream.
Reccmp metadata is refreshed with 18,431 pairs and 4,407 ledger mappings.
All eight additions map directly, and the retained private Globe-counter
callback now has a mapping. Current DLL/DBG/CSV hashes are verified.
Pass 107 raises the ledger to 4,537 functions with five pack utilities and all
198 native booster/starter descriptions in the restored `functions/packs.c`.
Source checks now cover 143 C units, and the build contains 165 C objects.
All three new public symbols and 44 private table objects are verified,
including exact rarity bytes and every relocated booster/starter pointer.
Two unreferenced private workers remain source-only until their generator
caller is recovered. Thirty native expansion enumerators extend the original
header without changing earlier values. Pack generation, its card consumers
and the selection GUI remain pending; gameplay validation is downstream.
Reccmp metadata has 18,434 pairs and 4,410 ledger mappings. The three public
utilities map directly; two unreferenced private workers remain omitted until
their caller is recovered. Current DLL/DBG/CSV hashes are verified.
Pass 108 raises the ledger to 4,540 functions with the complete weighted pack
generator, public wrapper and original self-check entry point. All 18 new
weight tables match the native 7,024 bytes. The generator and both previously
unused private workers are now emitted, and the self-check's four-byte static
once gate is verified through DWARF and its local BSS symbol. All source,
ABI, provenance and compiler-part checks pass, as does the GCC 9.3 PE32 build.
The terminal empty switch arm has an explicit null statement for GCC 9.
The self-check and game code have not been executed. The pack GUI and card
consumers remain pending; gameplay validation remains downstream.
Reccmp metadata now has 18,439 pairs and 4,415 ledger mappings. All three
additions and both previously unused workers map directly; current DLL/DBG/CSV
identities are verified.
Pass 109 raises the ledger to 4,545 functions with the pack dialog, exact template,
public chooser, Booster Tutor and Stocking Tiger. All four new public symbols,
the private stdcall callback, 294 template bytes, seven persistent local objects
and 28 drawing/Win32 imports are verified. Source checks cover the native
argument/owner-draw/text/rectangle layouts and three card VLAs. The new
`blt_expansion_symbol` import brings the drawing import library to twelve i386
members. Full source/ABI, provenance and compiler-part checks and the GCC 9.3
PE32 build pass. The Wish chooser stubs retain their scope. No dialog or game
code was executed; gameplay integration and validation remain downstream.
Reccmp metadata now has 18,444 pairs and 4,420 ledger mappings. All five
additions map directly; current release/build DLL/DBG and CSV hashes are verified.
Pass 110 raises the ledger to 4,555 functions with As Foretold, Fires of
Invention, Jodah, Abandoned Sarcophagus, Lurrus, Muldrotha and four qualified
graveyard-replacement helpers. All ten new public symbols are verified.
DWARF confirms both the four older 208-byte filter APIs and the four new
216-byte APIs, plus the six two-row integer array shapes. GCC omits three
hand-size bounds from DWARF; source/native allocation evidence checks them.
Native legality/payment/state quirks and Muldrotha's full 27-word dialog
are retained. All source/ABI, provenance and compiler-part checks pass,
as does the GCC 9.3 PE32 build. No game code was executed; gameplay validation
and runtime integration remain downstream.
Reccmp metadata has 18,452 pairs and 4,432 ledger mappings. All ten additions
and the now-emitted Cycling/Muldrotha callbacks map directly. The four helper
RVAs use the explicit native-interface names, and current input hashes are verified.
Pass 111 raises the ledger to 4,560 functions with Endless Sands, Izzet Chemister,
Livio, Dauthi Voidwalker and its private Void-counter constructor. All four new
public symbols, the private constructor and the retained Aegis/Void callbacks
are verified. DWARF checks Livio's 100-by-2 table, Dauthi's 31-by-3 table,
Chemister's VLA and its two distinct 216-byte filters. Six full dialog calls
retain 193 argument words. Native packed-owner/ID rules, counter/image metadata,
iteration order and unchecked bounds/filter quirks remain explicit in the
evidence. All source/ABI, provenance and compiler-part checks and the GCC 9.3
PE32 build pass. Headers, import definitions and the Wish chooser stubs remain
unchanged; gameplay validation and runtime integration remain downstream.
Reccmp metadata has 18,459 pairs and 4,439 ledger mappings. All five additions
and both now-emitted callbacks map directly; current release/build DLL/DBG and
CSV hashes are verified.
Pass 112 raises the ledger to 4,566 functions with Ormos, Pelt Collector,
Blaze of Glory, Dispossess, Hot Fix and Mise. All six public symbols, Ormos's
fixed/two-row variable arrays, Pelt's variable power array and three 216-byte
filters are verified. Native packed power slots and low-byte counters, signed
phase/clock rules, packet 28 network arguments and the runtime EXE legacy global
are recorded in the evidence. The DLL named-card wrapper is verified as the
NULL-filter specialization of the existing typed chooser boundary. Dispossess
and Mise still require the deferred interactive chooser UI; no dialog internals
are counted as recovered. All source/ABI, provenance and compiler-part checks
and the GCC 9.3 PE32 build pass. Five missing card-handler names remain;
gameplay integration remains downstream. Reccmp metadata has 18,465 pairs and
4,445 ledger mappings, with all six additions and current input hashes verified.
Pass 113 raises the ledger to 4,571 functions with the two Nicol Bolas cards,
Tamiyo, God-Eternal Kefnet and Aryel. All five public symbols, three fixed arrays,
two VLAs, three 216-byte filters and Aryel's 76-byte token/special-keyword word
are verified. The native seven dialogs retain 173 argument words; Dragon-God's
five indirect handler calls and full nested chooser are preserved. Native
capacity/filter/removal quirks remain explicit in the evidence. All source/ABI,
provenance and compiler-part checks and the GCC 9.3 PE32 build pass. The name
inventory now finds zero missing card-handler names; helper and modification
coverage remain separate work. Tamiyo retains the deferred chooser dependency.
Reccmp metadata has 18,470 pairs and 4,450 ledger mappings, with all five
additions and current input hashes verified. Gameplay integration remains downstream.
Pass 114 raises the ledger to 4,580 functions with four modified `_2022` card
handlers and five legacy/selection helpers for Bruna, Personal Incarnation,
Toshiro and Seht's Tiger. Five public symbols and four private helpers are
verified, together with both Toshiro two-row VLAs and Bruna's distinct 84-byte
target/216-byte filter locals. Native physical worker parameters, control/death
state, proxy-legacy construction, damage/protection callbacks and retry/cleanup
quirks are recorded. All source/ABI, provenance and compiler-part checks and the
GCC 9.3 PE32 build pass. Original card entry points and deferred chooser stubs
are retained; downstream dispatch and gameplay integration remain separate.
Reccmp has 18,475 pairs and 4,459 ledger mappings, with all nine native addresses
mapped to the recovered implementations and current input hashes verified.
Pass 115 raises the ledger to 4,589 functions with Havengul Lich, five private
helpers and three public commander/viewer/fixed-mana wrappers. Four public
symbols, five private helpers, the eight-IID array and both 216-byte filters
are verified. The retained Havengul attachment callback is newly emitted and
checked separately. Original physical specialization signatures, eighteen-pair
borrowed-ability storage, grave-source identity and unchecked native behavior
are recorded. The viewer/preparation EXE calls and four-argument mana fallback
are verified in the rebuilt binary. All source/ABI, provenance and compiler-part
checks and the GCC 9.3 PE32 build pass. Original entry/helper bodies, linker
bindings and deferred chooser stubs remain; downstream dispatch/gameplay
integration remains separate. Reccmp has 18,482 pairs and 4,469 ledger mappings,
with all nine additions and the attachment callback mapped directly and current
input hashes verified.
Pass 116 raises the ledger to 4,596 functions with seven public stack/activation,
trigger-response and deck/opponent helpers. Both stack phases, payment/failure
cleanup and full EXE argument frames are reconstructed. Binary/DWARF checks
verify the seven symbols, 500-byte prompt, 300-byte trace, dynamic opponent
candidate array, 4,776-byte opponent layout, seventeen deck-limit table values
and all stack/trigger EXE targets. The no-opponent path terminates at the native
abort boundary. Source/ABI, provenance and compiler-part checks and the GCC 9.3
PE32 build pass. Reccmp has 18,489 pairs and 4,476 ledger mappings, with the seven
new addresses mapped directly and current input/CSV hashes verified. Original
EXE stack linker bindings and existing helper bodies remain; the two recovered
stack entry points use qualified names pending downstream migration. The
inventory has zero missing card-handler names and 21 source-absent helper/artifact
candidates requiring review. No game code was executed.
Pass 117 raises the ledger to 4,603 implementations with seven combat helpers:
five public interfaces and two private workers. Native legality returns a
minimum or maximum blocker requirement, including the signed -1 result.
Human selection and final blocker-group enforcement retain event/selection
timing and native state. Speculative setup restores the 4,800-byte EXE table,
three ability accumulators, full activating-player word and COP state. All
source/ABI, provenance and compiler-part checks and the GCC 9.3 PE32 build pass.
Binary/DWARF checks verify five public symbols, the private legality worker,
the enforcer's concrete inline instance, its live 151-element count array,
two 84-byte targets, the 16-byte record, table clear and typed EXE boundaries.
Reccmp has 18,492 pairs and 4,482 ledger mappings, with six additions/upgrades
mapped directly; the inlined enforcer has no independent rebuilt range and is
excluded from the CSV. Input/CSV hashes are verified. The larger AI search
routines and downstream caller/dispatch/gameplay integration remain recovery
work. The function inventory retains zero missing card-handler names and
eighteen source-absent helper/compiler candidates requiring investigation.
Pass 118 raises the ledger to 4,607 implementations with four combat-search
helpers: unblocked-power adjustment, recursive blocker assignment/scoring,
qualified public blocker assignment and attacker-combination search. The GCC
9.3 PE32 build and all source/ABI, provenance and compiler-part checks pass.
Binary/DWARF checks verify two new private workers, both public entries, all
23 local array objects, the integer VLA, twelve/eight/sixteen-byte record layouts,
signed 64-bit division and typed EXE calls. The retained minimum-blocker enforcer
and two sort callbacks are now emitted independently due to their new callers;
their bodies and implementation counts are unchanged. Reccmp has 18,498 pairs
and 4,489 ledger mappings, including the four additions and three retained
workers/callbacks, with input/CSV hashes verified. Native unchecked selection
indexing, rampage lifetime and assassin-ranking behavior remain intact. The
inventory retains zero missing card-handler names and fifteen source-absent
helper/compiler candidates requiring review. Modified old bodies and downstream
caller/dispatch/gameplay integration remain recovery work. No game code was run.
Pass 119 raises the ledger to 4,614 implementations with seven display helpers:
instance/extension copy, keyword/legacy text, full-card renderer worker/frontend
and two stdcall window hooks. The GCC 9.3 PE32 build and all source/ABI,
provenance and compiler-part checks pass. Binary/DWARF checks verify the seven
functions, seven local arrays, 492-byte composite record, two private 128-byte
keyword tables, four-byte expansion static, both decorated hook symbols and
their sixteen-byte callee stack cleanup, typed EXE boundaries and three new
Drawcardlib imports. Previous import-definition hashes remain verified through
the new append. Reccmp has 18,505 pairs and 4,496 ledger mappings, including all
seven new functions, with input/CSV hashes verified. Native formatting and paint
quirks remain intact; the deferred Wish dialog stubs remain unchanged. Eight
source-absent helper/compiler candidates and modified older bodies need review;
downstream caller/dispatch/gameplay integration remains recovery work. No game
or UI code was run.
Pass 120 raises the ledger to 4,619 implementations with the modified 2022
draft handler, its deck writer, both K'rrik-aware mana checks and a private
life-payment clone. All source/ABI, provenance and compiler-part checks and the
GCC 9.3 PE32 build pass. Binary/DWARF checks verify all five symbols, both new
private workers, eleven arrays including five VLAs, mana-byte store coverage and
typed EXE boundaries. The retained picker is now emitted without changing its
body/count. Reccmp has 18,508 pairs and 4,502 ledger mappings, with current
input/CSV hashes verified. One earlier Improvise caller's mana-check migration
is pinned explicitly as pending; its accepted source and earlier evidence remain
unchanged. Native draft/scoring/deck-format behavior and error paths are retained.
Six source-absent C++ helper/library candidates and modified older bodies still
need review. Downstream dispatch/caller/gameplay integration remains recovery
work; no game, UI or recovered deck-writing code was executed.
Pass 121 adds the complete mana-payment core and seven multi-color/resolving
frontends, bringing the ledger to 4,627 implementations. Source/ABI, provenance,
compiler-part and GCC 9.3 PE32 build checks pass. Native verification checks all
eight public symbols, four local arrays, four saved pointers, twenty EXE call
boundaries, six/seven-word request stores and five cleanup/result sequences.
Reccmp has 18,508 pairs and 4,510 ledger mappings with current input/CSV hashes
verified. Older engine/frontends and three earlier charging fragments remain
unchanged. Five C++ candidates are verified GCC library instantiations and add
no implementation counts; the small ID-conversion method remains pending.
Native X, K'rrik, tap-trigger, rollback and cleanup ordering is retained.
Downstream caller/dispatch/gameplay integration and modified older bodies remain
work. No game, UI or recovered payment code was executed.
Pass 122 adds nine stack/activation helpers, bringing the ledger to 4,636
implementations. Source/ABI, provenance, compiler-part and GCC 9.3 PE32 build
checks pass. Native verification checks eight public symbols and a private
stack worker, extension-copy calls, packed stack/extension layouts, native
qualified constants, Replicate-byte access, spell dependencies, being-cast bit
operations and thirteen EXE boundaries. GCC folds the identical response body
into an exact tail jump to the retained implementation; its entry and target
are verified. Reccmp has 18,509 pairs and 4,518 ledger mappings, including eight
of this batch's bodies. The folded response thunk has no independent DWARF
range and is explicitly excluded. Current input/CSV hashes are verified.
Storm/Gravestorm/Replicate precedence, cast/replacement sequencing, Snow sound,
forced-mana tap triggers and native cleanup behavior are preserved. Older APIs
and chooser stubs remain unchanged; downstream caller/dispatch/gameplay
integration and modified older bodies remain work. No recovered code was run.
Pass 123 adds nine public engine/state, legacy, mana and subtype-reset helpers
plus the four-byte override_xx global, bringing the ledger to 4,645 functions
and 122 recovered globals. Source/ABI, provenance, compiler-part and GCC 9.3
PE32 build checks pass. Native verification checks all nine symbols, BSS/type
storage, four arrays, three saved pointers, added-color/override reads, player
masks, subtype filtering, recalculation dependencies and nine EXE boundaries.
The full recalculation body preserves four earlier fragments and their raw
export. GCC folds the asterisk helper and both legacy constructors into exact
verified tail jumps to their retained implementations. Reccmp maps the six
independent new bodies and explicitly excludes those three folded entries;
metadata has 18,506 pairs and 4,524 ledger mappings with verified input/CSV
hashes. Native X-format override precedence, subtype rebuild, tap-trigger and
state-restoration behavior are retained. Older APIs and deferred chooser stubs
remain unchanged. Downstream caller/dispatch/gameplay integration and modified
older bodies remain work; no recovered code was executed.
Pass 124 adds eight public turn/combat/display/death/color/phasing helpers,
bringing the ledger to 4,653 implementations. Source/ABI, provenance,
compiler-part and GCC 9.3 PE32 build checks pass. Native verification checks all
eight symbols, four local arrays, the fourteen-entry Shimmer table, byte poison
reads, display masks, mark/complete mode pairs, dependencies and twelve EXE
boundaries. Untap preserves full skip scanning and attachment-driven phasing;
upkeep omits the obsolete EXE flag reset and keeps native AI speculation.
The earlier untap fragment/export and all previous records remain unchanged.
Attack/display routines fold into exact verified public tail jumps. Reccmp maps
six new bodies and explicitly excludes those two entries without independent
DWARF ranges; metadata has 18,504 pairs and 4,530 ledger mappings with verified
input/CSV hashes. Original APIs and chooser stubs remain available. Downstream
caller/dispatch/gameplay integration and modified older bodies remain work.
No recovered phase, game or UI code was executed.
Pass 125 adds eight mana-drain and AI phase/choice helpers, bringing the
ledger to 4,661 implementations. Source/ABI, provenance, compiler-part and
GCC 9.3 PE32 build checks pass. Native verification checks seven public symbols,
two arrays, both snapshot-row restores, timer arithmetic, dependencies,
fourteen EXE boundaries and the activation pointer cached across a loop. The
choice worker is fully inlined into both public callers; both DWARF instances,
value parameter and record/replay calls are verified. Two non-draining public
helpers fold into exact verified tail jumps. Reccmp maps five new bodies and
explicitly excludes those three entries without independent ranges; metadata
has 18,503 pairs and 4,535 ledger mappings with verified input/CSV hashes.
The native one-row save/two-row restore remains partially initialized, with no
zero-fill fix or stack-byte equivalence claim. Existing bodies, records,
globals, fragments and chooser stubs remain unchanged. Modified older bodies
and downstream caller/dispatch/gameplay integration remain work; no recovered
code was executed.
Pass 126 adds eight complete AI snapshot/save/support helpers and two private
counter-type scalars, bringing the ledger to 4,669 implementations and 124
recovered globals. Source/ABI, provenance, compiler-part and GCC 9.3 PE32 build
checks pass. Native verification checks seven public symbols and the private
worker, all 46 snapshot storage objects, four bulk-copy regions and counter-type
access pairs, all 23 unconditional ordered save/load frames, enum values and
three EXE boundaries. Both EXE banks and sixteen card-data slots are retained;
so are assertion continuation, autotap settings and lethal-blocking callbacks.
All earlier records, bodies, globals, 24 overlapping snapshot fragments and
the five existing snapshot/save raw exports remain unchanged. Reccmp maps all
eight bodies. Metadata has 18,503 pairs and 4,543 ledger mappings with verified
input and CSV hashes.
Modified older bodies and downstream caller/dispatch/gameplay integration
remain work; no recovered code was executed.
Pass 127 adds fifteen mana color-choice, availability, production and aura
helpers, bringing the ledger to 4,684 implementations. Source/ABI, provenance,
compiler-part and GCC 9.3 PE32 build checks pass. Native verification checks
eleven public and four private symbols, five local arrays, the 84-byte target
definition, enum values, dependencies, unsigned division, eight exact strings
and five EXE boundaries. The full inlined color-choice logic, positive-amount
compiler workers, cancellation/chosen-color bookkeeping, Mana Flare fallback
and two-mode Wild Growth behavior are retained. All previous records, bodies,
globals, fragments and chooser stubs remain unchanged. GCC folds the multi-color
public helper into an exact verified jump to its retained counterpart. Reccmp
maps fourteen new bodies and explicitly excludes that entry without an
independent range. Metadata has 18,506 pairs and 4,557 ledger mappings with verified
input and CSV hashes.
Modified older bodies and downstream caller/dispatch/gameplay integration
remain work; no recovered code was executed.
Pass 128 adds thirteen public land/creature mana source/control helpers,
bringing the ledger to 4,697 implementations. Every bounded function entry in
the native `produce_mana.c` unit is now represented; downstream integration
remains separate. Source/ABI, provenance, compiler-part and GCC 9.3 PE32 build
checks pass. Native verification checks thirteen public symbols, four 84-byte
target objects, the 72-byte card-data layout and adjacent ID-word load, enums,
dependencies, signed AI division, six strings and six EXE boundaries. It follows
the final bit-count result through the single-color helper to the direct
producer's amount argument. Native replacement/subtype events, the full ID
decision tree, overrides/transmutation, creature restrictions, tap/control rules
and mixed-controller Aura quirks are retained. All earlier bodies, records,
globals, fragments and chooser stubs remain unchanged. Two old Memorial call
bindings are explicitly recorded; its auditor replays its historical target
catalog without migrating those callers. Reccmp maps all thirteen new bodies.
Metadata has 18,506 pairs and 4,570 ledger mappings with verified input and CSV hashes.
Modified older bodies and downstream caller/dispatch/gameplay integration
remain work; no recovered code was executed.
Pass 129 adds eleven public color/subtype and sleight helpers, bringing the
ledger to 4,708 implementations. Source/ABI, provenance, compiler-part and GCC
9.3 PE32 build checks pass. Native verification checks all eleven symbols,
seven static-local views, three distinct 500-byte text buffers, shared color-
word table storage, all table values, byte-field/enum values, three fixed
variadic words and first access at EBP+20, and the 30,000-byte EXE word-table
view. COLOR_ANY maps to all five colored bits; other inputs preserve native
shift behavior. Formatting, subtype domains, mask retention and AI cache/flag
ordering are retained. All previous records, bodies, globals, fragments and
chooser stubs remain unchanged. GCC folds four helpers into exact verified
jumps to retained counterparts. Reccmp maps seven new bodies and explicitly
excludes those four entries without independent ranges. Metadata has 18,503 pairs and 4,577 ledger mappings with verified input and CSV hashes.
Modified older bodies and downstream caller/dispatch/gameplay integration
remain work; no recovered code was executed.
Pass 130 adds twenty-three color-core helpers, bringing the ledger to 4,731
implementations. Source/ABI, provenance, compiler-part and GCC 9.3 PE32 build
checks pass. Native verification checks twenty-one public symbols, three local
array types, two private scanner inline instances and their ranges/string calls,
callback/dependency bindings, enums, the EXE word-table view and chooser boundary.
Full timestamp/controller/humiliation precedence, split-card lookup, printed-
color exceptions, no-repeat color-change effects, masks/counts and deck/chooser
behavior are retained. All previous bodies, records, globals, fragments, chooser
stubs and the two prior scanner exports remain unchanged. Seven public entries
fold into verified thunks. A separate sixteen-byte shared-color COFF body equals
the retained helper but lacks a concrete DWARF range. Reccmp maps thirteen new
bodies and explicitly excludes the seven thunks, two fully inlined workers and
that body without its own concrete range. Metadata has 18,493 pairs and 4,590 ledger mappings with verified input and CSV hashes.
Modified older bodies and downstream caller/dispatch/gameplay integration
remain work; no recovered code was executed.
Pass 131 adds twenty name/action/legacy/type-text helpers, bringing the ledger
to 4,751 implementations. Source/ABI, provenance, compiler-part and GCC 9.3
PE32 build checks pass. Native verification checks sixteen public symbols,
two private callbacks, two private inline workers, five local storage views,
exact dependency/callback bindings, enums, EXE text tables and both returning
assertion paths. Assembly confirms both shared-name counting loops check
in_play despite Ghidra's omitted first check. Ten-attempt random naming,
216-byte tests, both-player damage, original-ID/token exceptions, legacy
labels and sorted type formatting are retained. All earlier records, bodies,
globals, fragments and chooser stubs remain unchanged. Four public helpers
fold into verified thunks; a separate eighteen-byte equivalent body lacks
concrete DWARF. The 101-byte generated exile-check part aliases the bounded
retained part. Reccmp maps thirteen new bodies and excludes seven entries
without independent ranges. Metadata has 18,490 pairs and 4,603 ledger mappings
with verified input and CSV hashes. Modified older bodies and downstream
caller/dispatch/gameplay integration remain work; no recovered code was executed.
Pass 132 adds twenty tap/untap, disabling and AI helpers, bringing the ledger
to 4,771 implementations. Source/ABI, provenance, compiler-part and GCC 9.3
PE32 build checks pass. Native verification checks nineteen public symbols,
one private targeting predicate, five storage views, the 84-byte target layout,
59 enum values/source alias, dependencies, callback field and the complete
eighteen-word Twiddle dialog frame. Ability-event suppression, label-only entry,
full activated-ability lookup, signed AI ratings and own-life draw quirks,
retargeting, optional untap state/negative target guards and both sickness bits
are retained. Five public entries fold into verified thunks. The new upkeep
caller makes the earlier type-text worker an 875-byte outline; the earlier
source/record stays exact, and its verifier now checks both callers and the
bounded array layout. All other earlier tooling, records, bodies, globals,
fragments and chooser stubs remain unchanged. Reccmp maps fifteen new bodies
plus the earlier newly outlined worker and excludes the five thunks. Metadata
has 18,486 pairs and 4,619 ledger mappings with verified input and CSV hashes.
Modified older bodies and downstream caller/dispatch/gameplay integration
remain work; no recovered code was executed.
Pass 133 adds twenty type/animation/copy helpers and the 2022 generic
animated-land callback, bringing the ledger to 4,792 implementations. All
source/ABI, provenance, compiler-part and GCC 9.3 PE32 build checks pass. Native
verification checks nineteen public symbols, two private callbacks, both local
views, 72/216/84-byte layouts, 43 enums, exact dependency/callback bindings,
typed indirect dispatch and three fifteen-word activation frames. Subtype/color
and expiry modes, base PT, dynamic-IID reuse, no-repeat creature effects,
malformed global filter slots, invisible uncounterability cleanup, clone/legend
handling and manland activation are retained. The existing unused database-name
partition and the death worker's bounded attached=0 specialization are explicitly
verified. Three public entries and one private callback fold into verified
thunks. Every previous record, source prefix, global, fragment, import, linker
binding and chooser stub remains unchanged. Reccmp maps seventeen new bodies
and excludes those four thunks. Metadata has 18,482 pairs and 4,636 ledger
mappings with verified input and CSV hashes. Modified older bodies and downstream
caller/dispatch/gameplay integration remain work; no recovered code was executed.
Pass 134 adds twenty-seven card-query helpers, bringing the ledger to 4,819
implementations. All source/ABI, provenance, compiler-part and GCC 9.3 PE32
build checks pass. Native verification checks twenty-five public symbols,
two private workers and all twenty-seven concrete ranges, three local storage
views, accessed database fields, 22 enums, exact dependency bindings, signed
CMC locals, three callback-context restores, both X-spell calls and four inline
flag queries with their immediate result consumers. Printed/current type flags,
Grist's creature/base-1/1 exceptions, generic recursive split-card mana values,
token/DFC/X handling, callback restoration, unchecked seventeen-bin histograms
and exact counting rules are retained. The retained-body audit anchors real
definitions instead of earlier conditional calls. Every previous record,
source prefix, global, fragment, import, linker binding and chooser stub remains
unchanged. Reccmp maps all twenty-seven new bodies. Metadata has 18,482 pairs
and 4,663 ledger mappings with verified input and CSV hashes. Modified older
bodies and downstream caller/dispatch/gameplay integration remain work;
no recovered code was executed.
Pass 83 added Windscouter, Wall of
Junk, Quicksilver Behemoth, Rune of Protection: Artifacts and two shared public
callbacks. All six public symbols are verified. Pass 82 added War Room, Hellkite
Courser and two commander helpers. The qualified public command-zone wrapper
preserves an older private Commander 2015 helper; all three new public symbols
are present. Pass 81 added Zombie Musher,
Hallowed Ground and two native snow helpers. The private nonsnow callback
is verified with local COFF linkage. Pass 80 added Vivien and four
casting-as-flash helpers. Three private callbacks and two private BSS event
variables are checked in the rebuilt debug symbols. Pass 79 added the five
Disciple handlers and their private native worker. Pass 78 added Indomitable Creativity
and its reveal/destruction helpers. Verification also checks the new private
callback's local COFF linkage. Pass 77 added native area damage, Arahbo,
Plague Reaver and Noxious Field. Pass 76 added the Ozolith and upgraded
two already accepted leaves-play helpers. Verification checks both new native
helper names and their retained compatibility symbols. The reccmp CSV has
been refreshed against this build.

`Makefile-common` now allows warnings by default. `WERROR=-Werror` restores
fatal warnings for make users.

The wrapper also handles Windows PowerShell 5.1's redirected-stderr error
records, allowing GCC warnings to reach the log while using make's native exit
code to determine success. Verification reads both legacy UTF-16 and newer
UTF-8 build logs.

The first helper-unification pass replaces fifteen retained bodies with GCC
aliases: `is_what`, `is_token` and all thirteen extension-flag APIs. Full
source/ABI, provenance, compiler-part, GCC 9.3 PE32 and native checks pass.
All fifteen public name pairs have identical actual COFF addresses; their
recovered bodies and implementation records remain unchanged. Historical
retained bodies are archived and hash-checked separately. Seven additional
query callers now compile as verified JMPs to identical retained bodies.
The existing constprop death-trigger worker's 899-byte range, constant
parameter and fourteen outgoing calls are verified. Reccmp records 48 folded
helpers separately from its 18,470 full-body pairs and 4,701 ledger mappings.

The next unification batch shares 92 more core helpers, bringing the total to
107 same-address public name pairs. All 4,871 recovered implementation records
remain unchanged; only the 92 archived body-to-alias replacements are applied
across three C files. Source/ABI, provenance, compiler-part and the complete
GCC/native checks pass. Seven enum widths and the common 300/72/152-byte
pointer record views are checked. The two private color scanners now have
254-byte emitted bodies shared with retained counterparts; their exact COFF
addresses, complete DWARF ranges, signed-int parameter types and three string
calls are verified. Reccmp has 18,493 full-body pairs and 4,724 ledger mappings,
plus 27 verified compiler thunks and two same-address private compiler aliases.

The stricter recovery checks remain available:

```powershell
python tools/recovery/check_source.py
python tools/recovery/audit_source.py
python tools/recovery/audit_compiler_parts.py
```

`Makefile-excluded` names 24 historical C files absent from both the 2020 and
2022 DLL CU inventories. These include obsolete aggregate card files whose
handlers already live in set-specific units. Ten old assembly wrappers also
duplicate active C implementations. Their files remain available; they are
omitted from this DLL's link. `functions/in_play.asm` conditionally omits its
C-replaced entries while preserving its unique `is_not_dead` entry. The
linker script loads `ManalinkEh.ao` first, and make builds it as a prerequisite
without passing a second copy when an object directory is selected.

Two recovered functions needed null statements after terminal labels for GCC
9 C grammar: Brainstone and Kenrith. Their exact prior records are preserved;
the earlier Kenrith life-callee proof is replayed against the predecessor.
The header's tentative global definitions became extern declarations.
`card_coded` is supplied by the EXE address in `manalink.lds`; the DLL-owned
legacy-title pointer has one zero-initialized definition in `functions/functions.c`.

Pass 135 adds twenty-eight public card-state, land and mana-cost helpers,
bringing the ledger to 4,847 implementations. The source/ABI, provenance,
compiler-part and GCC 9.3 PE32 checks pass. Native proofs pin 2,721 bytes and
nine recovered-callee branch sites, including a verified two-byte tail jump.
Rebuilt checks verify all 28 symbols and return types, four accessed layouts,
18 enums, exact dependencies, 22 concrete bodies and six compiler thunks with
independent target ranges. Three extension-flag inline queries occupy four
DWARF spans; two immediate result consumers are checked separately. The
16-ID no-mana-cost list is independently decoded over the 16-bit CSVID domain
and signed boundary cases. Bulk state-filter asymmetry, raw X-spell masks,
pointer results, tap/untap dispatch, printed graveyard types, death sentinel
99 and exact unsigned backup-ID handling remain as in the native code.
All earlier records and source prefixes remain unchanged. Reccmp maps the
22 standalone additions and excludes six thunks. Metadata has 18,476 pairs
and 4,685 ledger mappings; the CSV and all four binary/debug input hashes
are verified. Gameplay and downstream caller/dispatch integration remain work.

Pass 136 adds twenty-four damage-prevention/Fog helpers, bringing the ledger
to 4,871 implementations. Source/ABI, provenance, compiler-part and GCC 9.3
PE32 checks pass. Proofs pin 5,108 native bytes, eighteen recovered-callee
branches and eleven callback references. Rebuilt checks verify twenty public
symbols, four private symbols, 22 concrete bodies, two compiler thunks, native
return contracts, three accessed record views, 38 enums, exact dependencies
and callbacks, signed 16-bit capacity, byte redraw updates, the shield title
and the six-word source-aware counter API. The private worker has three
physical arguments; its callers retain the optimized-away event gate. Its
flag-getter DWARF spans and immediate result consumer are checked separately.
The new text caller outlines the earlier alternate-text worker; its 125-byte
range, both callers, exact strings and two returning assertion continuations
are verified. No earlier source or recovery record changes. Reccmp maps the
22 new concrete bodies and that earlier newly outlined worker, excluding the
two thunks. Metadata has 18,477 pairs and 4,708 ledger mappings; all
binary/debug inputs and the CSV hash are verified. Native prevention and
source/target-pairing quirks remain intact. Runtime/gameplay and downstream
caller/registration integration remain unverified.

The DLL still uses the repository's existing veneer/registration table and
contains retained legacy APIs alongside recovered 2022 interfaces. Further
integration remains necessary. The two user-directed Wish chooser stubs
still return no selection. No DLL/game execution or gameplay tests were run.
