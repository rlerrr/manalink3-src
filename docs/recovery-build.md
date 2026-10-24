# Building the recovery branch

The 2026-10-24 recovery build successfully compiles, assembles and links a
32-bit Manalink DLL using the downloaded GCC/G++ 9.3 toolchain. This verifies
native linking; it does not establish gameplay correctness or complete 2022
runtime integration.

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

The DLL still uses the repository's existing veneer/registration table and
contains retained legacy APIs alongside recovered 2022 interfaces. Further
integration remains necessary. The two user-directed Wish chooser stubs
still return no selection. No DLL/game execution or gameplay tests were run.
