# Recovery scope

The goal remains best-effort recovery of October 2022 card handlers and their
helpers into their original source units. Native linking, initializer coverage,
and downstream gameplay verification have separate evidence.

## Current chooser status

The initial 2026-10-22 deferral of the built-in Wish/card-picker UI is historical.
The later C++ recovery pass records that the user resumed this work. The current
[choose_a_card.cpp](../functions/choose_a_card.cpp) contains the picker logic,
dialog procedure, filtering, saved options and exact 990-byte/22-control dialog
template. Its public entry points call the recovered implementation.

[cpp-chooser-recovery.json](recovery-evidence/cpp-chooser-recovery.json) pins ten
authored C++ definitions and eight compiler/header ranges. It explicitly resolves
the earlier stubs, including `choose_a_card_test_ct` and
`choose_a_card_test_ct_allow_cancel`. The older manifest `source_stubs` and
`out_of_scope_source_units` fields retain their historical provenance; the
current verifier subtracts the resolved addresses when computing coverage.

The current code therefore does not simply return `-1` from those two APIs.
Compilation and native/source audits have passed. Interactive picker execution
and downstream gameplay correctness have not been established by those audits.

## Function and data coverage

The ordinary card/helper census covers contiguous top-level project DWARF
ranges, with a separate split-range helper census. Zero pending ranges in that
inventory is not a whole-DLL completion claim.

The COFF cross-check additionally accounts for 35 project entries without an
ordinary DWARF extent, including compiler-generated forwarding entries and the
legacy assembly call bridge. Five original card entry identities were missing
from source and are now restored; four registered veneers use those names.
CRT/library candidates and OS import thunks are reported separately from
authored card/helper source.

The initializer cross-check covers 296 direct-address project variable records,
including the resumed picker: 175 integer/enum scalars and arrays, plus 121
pointer/aggregate records. The latter compare all remaining bytes and 1,391
typed pointer words through explicit object identities or source-verified
readonly strings. Shared-pointer relationships also match. These are variable
records; some source descriptors share one physical object.

The linker-name pass restores 54 exact nonzero native fixed EXE bindings;
414 of the native 416 absolute-symbol names are now present with identical
values. The two absent entries are zero-address runtime-import bookkeeping,
explicitly excluded from source definitions. All previous 418 rebuilt absolute
symbols retain their values. The sealed linker-only build preserves the runtime
payload; subsequent C recovery can legitimately change the current payload.

The header pass restores 19 native enum names and corrects the Manalink crew
query from `0x4f00` to the native `0x1d00`. All three compiled consumers use
the native value. A normalized runtime delta check finds only those three
immediate-byte changes across the 26 runtime sections.

The header-name batch has now restored all 5,100 missing native card-ID names,
ten other missing names and Manalink's native double-faced-card symbol limit
of 11. All 5,111 compiled values match the original DWARF. The runtime sections
and function/absolute COFF entries are identical to the preceding build. Forty-eight
static object symbols keep their base names, offsets and types with a compiler
UID suffix increase of 5,110. The
older function/data reports retain their actual older input hashes; no fresh
full function comparison is claimed for this debug/header-only rebuild.

No enumerator names from the 65 named native header enums in this inventory
remain absent from source. This leaves anonymous/local enums, inline-only
project helpers and complete declaration ABI as separate audits; an empty
ordinary function queue does not prove those scopes complete.

Five original header-only APIs are now restored from their abstract DWARF
definitions and 21 caller-owned inline instances: the four untargeted dice
shortcuts and `is_split_card`. The latter returns a first-half card ID or -1.
Their build and source/provenance audit pass, and runtime-section payloads are
unchanged. Inline fragments have no invented callable entry addresses.

The next inline pass restores 20 additional definitions across 14 original
units, plus the local Party enum. Their 35 native inline instances and 24
complete native callers are recorded alongside 63 attributed code fragments.
Four native picker conversions were evaluated read-only for 8,224 input cases.
The build and source/provenance audit pass, with all 26 runtime sections
unchanged. Available caller domains constrain reconstruction; Boolean-only
uses do not uniquely establish a general helper's exact nonzero integer result.

The subsequent wrapper pass restores 23 original definitions across 15 source
units. Its 55 native inline instances, 91 attributed fragments and 35 complete
callers cover targeting, legacy effects, tokens, casting costs, text splitting
and stored card/toughness pairs. Callback pointer types are checked directly
against the native int(int,int,event-enum) shape. Per-helper notes record where
specialization prevents unique recovery of general parameter or unused-result
behavior. Build, fixed-slot checks and the source/provenance audit pass; all 26
runtime sections are unchanged from the preceding build.

The mechanics pass restores 14 more C definitions in 12 original units plus
the chooser's two bitwise macro instantiations. Eight native operator overloads
have their enum, reference and SafeBool layouts checked; six unused overloads
provided by the macro are not separately credited. C evidence includes 17 inline
instances, 49 fragments and 15 complete callers. Operator evidence includes
82 inline DIE instances, 42 nonempty fragments and two complete callers.

The existing set and unordered-map class declarations account for three
compiler-generated constructor/destructor names. Artificial native DWARF flags,
source declarations, member types/offsets, native/rebuilt sizes24/28 and complete
native caller witnesses are checked. No authored special-member bodies are
invented. Build, fixed-slot and full source/provenance audits pass, and all 26
runtime sections remain unchanged from the preceding build.

The card-mechanic pass restores17 additional source APIs, including a vowel
truth table recovered from the complete caller despite empty inline attribution.
It retains the native256-byte name-fold table and checks17 inline instances,
43 attributed fragments and16 complete callers. It also fixes a real omitted
AI token filter in Ghired: native type_flag6 at offset108 is now emitted by the
rebuilt caller. A control build preserves26 runtime sections; the corrected
build changes exactly one of166 linker objects. Code/layout/relocation deltas in
four runtime sections are recorded explicitly. Both builds, fixed slots and
the complete source/provenance audit pass.

The updated inline-name reconciliation leaves22 inline-only names for caller
analysis, with all eight ordinary native bindings reconciled. This remains a
source-definition inventory. Specialized argument and unused-result limitations
are explicit in each reconstruction's evidence.

Ghired's missing store predates source conversion: it is absent from the cached
raw decompiler output. The subsequent audit reviews all37 selected functions using native and rebuilt
DWARF frame bases, full216-byte test geometry and direct scalar parameter
stores. It restores43 missing native writes in24 handlers across16 source
units. This includes two missing value flows: Torgaar's signed minimum quantity
and Glissa's available-mana-plus1 CMC bound. Thirteen handlers already matched
this field check. Thirty-six now match exact byte-write multisets; Guild Summit
has two identical compiled stores that CFG analysis proves mutually exclusive.

Build, fixed slots, targeted field/value-flow checks, legacy Kykar receipt
compatibility and the complete source/provenance audit pass. Sixteen linker
objects change and150 remain byte-identical. Historical receipts are retained;
the later native-backed source correction explicitly supersedes Kykar's old
body pin.

This audit covers named scalar test parameters at offsets100..215, not complete
function semantics or every indirect/message-buffer write. A wider source
inventory identifies105 flat test-bearing unions, with68 outside the original
metadata-driven selection. The subsequent wider review now classifies all68:
12 handlers receive27 missing native stores across12 original source units,
and56 definitions are retained. Repairs include Turn's zero/one Weird fields,
two lost sleighted-color returns, Dual Nature's cached creature name and token
filter, and Slaughter's survivor-matrix pointer plus not_marked callback.

All68 native/rebuilt test layouts match. Sixty-three scalar byte-write multisets
match exactly; four callback-bearing cases match exact COFF identities after
relocation; Sai's two generated quantity stores are proven mutually exclusive
by CFG reachability. Four recovered dynamic values are checked by conservative
CFG reaching-value analysis in native and compiled code, including the302-byte
survivor matrix. Native quirks and the600-byte EDH mixed-role union are retained.
Build,15,169 slots, focused checks, old Rabblemaster receipt compatibility and
the complete source/provenance audit pass. Twelve expected linker objects
change and154 remain identical.

All105 identified flat test-bearing unions have therefore received the bounded
direct parameter-write review. This does not prove whole-function semantics or
exhaust indirect/message-buffer stores and other storage shapes. The inline
source-definition queue remains22 names.

The final inline pass restores the remaining22 private source definitions in
17 original units, covering23 native inline instances,79 attributed fragments
and20 complete native callers. An unused-definition control build preserves
all26 runtime sections. The original Prototype Portal worker then replaces its
old Soul Foundry delegation: native/rebuilt chooser identity is11338/0x2c4a,
and native chooser flags, event arguments, trailing arguments and the Creature
label are retained. Only cards/scars_of_mirrodin.o changes after the control.
Both builds,15,169 slots, focused evidence/identity checks, historical Portal
receipt compatibility and the full source/provenance audit pass.

The142-name source reconciliation now has zero missing inline definitions.
Six definitions retain explicit optimized-away semantics: four dead int returns
(store_iid_and_tou, resolve_an_adventure, assist, generate_tokens_impl), the
inferred mode0 mask clear in disable_all_but_sp_keywords, and unobserved modes
of ward_action. Existing callers keep their observed expansions. Other native
specializations and arbitrary-input limitations remain in per-helper notes.
An empty source-name queue is therefore not a general behavioral-equivalence
or whole-DLL completion claim.

The common-header declaration pass now restores card_data_t.name[35] rather
than the18-byte name plus17 reserved bytes. Its72-byte size and ID offset36
are unchanged. The synthetic card_ext_struct tag is replaced with native
struct card_ext_t, retaining192-byte storage. The compiled type/animation
verifier removes its old partition exception and requires exact native fields.

All99 named native types in the functions/functions.c CU are accounted:74
exact definitions,22 matches with explicit existing enum order/extra-name or
anonymous-union/raw-word views, and3 SDK aliases not emitted in that rebuilt
CU. Both C and C++ probes verify those aliases and the restored declarations.
Native enum names/values, member offsets/widths, fixed array extents and pointer
target identities are checked within this census. Build,15,169 slots, focused
type verification and the full source/provenance audit pass; all26 runtime
sections are byte-identical. This remains one common-CU census rather than a
complete all-CU declaration, full callback-signature or dynamic-storage audit.

The next shared-storage pass covers20 additional flat targeting/token unions
and2 shared-storage typedef uses. All22 native/rebuilt frame pairings and record
geometries are checked. Lord Windgrace's original40-byte first view had hidden
two native token writes beyond that extent: quantity6 at offset43 and green
color8 at offset40. Both are restored in the existing handler;21 other function
definitions remain unchanged.

After the repair,18 cases match exact scalar byte-write multisets,2 compare
exact relocated callbacks, and2 use explicit value-flow proofs. Mechanized
Production sets both controller fields to player at every consumer despite
compiler duplication. The sacrifice helper preserves all32 packed result bits
(count16/player8/card8) under the native0x40 guard, with all other storage writes
matching outside the proved packing windows. Build,15,169 slots, focused checks
and the full source/provenance audit pass. Only commander_2018.o changes;165
other linker inputs remain identical. Direct scalar stores and these value
flows do not cover all indirect/string/SSE stores or complete function behavior.

The direct target-flag census finds5357 functions with fixed-frame target
locals and eight flag-writing functions. Native/rebuilt32-bit expressions
match: constant5, prior flags OR3/OR5 or the flags formal. The two rebuilt
Basic Equipment stores are mutually exclusive in the complete direct CFG.
The57 omitted target declarations have no emitted location, so no frame
storage or additional defect is inferred. A further shared sacrifice test
has all nine native parameter stores and208 argument bits, with native
200-byte prompt/216-byte test aliases correctly represented by one216-byte
source union. Allnine definitions are retained.

The local-enum review checks486 existing native declarations and1233 values
against actual GCC9.3 DWARF. Eight Odyssey source-owner spellings and the
abstract Memorial worker are resolved. The777-declaration census leaves290
missing declarations/730 names and one same-value native/source name mismatch
(CHOICE_UNPRINTED/CHOICE_TOKEN_ONLY, value4). This is a restoration queue,
not a count of behavioral defects. Focused checks,15169 fixed slots and the
full source/provenance audit pass without C/C++ or build-artifact changes.

The ordinary-test follow-up restores seven native settings in five card
handlers and the Miracle helper: tapped lands, token/Reflection filtering,
self exclusion, nonland AI counting, Goblin filtering and Saheeli's tutor
storage card. Allsix direct parameter-store multisets match native code;
Saheeli's storage argument also has an all-direct-CFG-path provenance proof.
Old receipts remain immutable through six exact supersession chains. Build,
15169 slots, focused checks and the full source/provenance audit pass. Six
expected linker objects change and160 remain identical.

The wider3299-pair screen is discovery, not full semantic verification. After
3189 equal scalar-event multisets and85 callback relocation cases,25 cases
need further classification; this repair closes six. The remaining19 include
known compiler clones. Multiple/no-current typed frames and nonstandard stack
prologues remain separate, as do unknown dynamic values and indirect stores.
Of47 initially unmatched typed-frame owners,41 are the Odyssey spelling alias
and six need separate analysis. The130 native entries without a CSV binding
are excluded from the paired screen, including static_graveyard_abilities.

The scoped-storage pass restores four original216-byte test locals across
Engineered Explosives, Grand Ossuary and static_graveyard_abilities. The original
28/100/100-byte payment/prompt arrays remain for their own uses, with no casts
passing them to full test APIs. Native lexical lifetimes are disjoint at the
same CFA-236. Explosives restores saved-CMC/nonland filtering; the graveyard
engine restores toughness6/less-than filtering for Kozilek's Return's AI count.

Allfour test layouts and scoped scalar parameter-event multisets match. An
independent direct-CFG proof follows the exact card-instance result and its
info_slot into Explosives' CMC. Ownership follows actual compiled DWARF: the
before graveyard helper is inlined; the repaired compiler outlines it. Earlier
Grand Ossuary receipts are preserved through exact supersession chains. Build,
15169 slots, focused checks and the full source/provenance audit pass; three
expected linker objects change and163 remain identical. Remaining casts and
array-iteration cases are separately inventoried, not assumed defective.

The next array-role pass restores three native int[2][100] matrices and one
char[2][151] matrix in Soldevi Sage, Krovikan Sorcerer, Aminatou and Liliana.
Their genuine216-byte filters remain separate. The Mimeoplasm's37-byte prompt
uses its message field directly; Ib Halfheart and Bolas's Citadel use typed
2/10-entry sacrifice loops. Native capacities and flag quirks are retained.

Actual DWARF and definite machine effects verify array geometry,800/800/302/40
clear spans,37 copied bytes, draw rows0/400 and chooser input, and Liliana's
302-byte matrix/callback operand. Source replay covers every edited byte;
this does not prove all dynamic indexed values or full function behavior.
Build,15169 slots, focused checks and the full source/provenance audit pass.
Six expected linker objects change and160 remain identical. Remaining target
casts and native prompt/marker storage have a separate discovery inventory.

The target-role pass restores 13 card definitions across 12 source units:
Eye of Doom's 1,208-byte marker matrix, three 100-byte prompts, four 84-byte
record copies, four typed array endpoints and four integer roles. Strive's
already typed helper remains unchanged. Native/rebuilt layouts and definite
machine effects verify the clear, formatter bounds and copies. Ovinomancer's
chooser pointer-spill provenance remains outside the conservative analysis;
its typed source operand is retained. Build, 15,169 slots, focused checks and
the full source/provenance audit pass; 12 expected linker objects change and
154 remain identical. Dynamic indexed values, arbitrary aliases and complete
function/gameplay equivalence are not inferred from these bounded checks.

The ability-storage pass restores four card definitions: Dark Impostor's
1,200-byte name matrix, scoped IID arrays, twelve dialog labels and correctly
based selected-ability load; Brain in a Jar's separate 500-byte prompt and
16-entry CMC clear endpoint; and three signed integer roles across Dark
Impostor, Ajani and Serra's Hymn. Native/rebuilt DWARF and direct-CFG operands
verify the extents, eleven explicit row pointers, twelve weights, formatter
and initializer operands. Affine direct-CFG analysis verifies the signed
16-bit IID load as stolen_abilities[dialog_result - 2]. Row zero's pointer
spill and the dynamically indexed strcpy destination remain unknown to the
effect model; exact source replay covers those typed operand changes.

Etali's adjacent prompt read is native behavior and is retained. Build,
15,169 fixed slots, focused checks and the full source/provenance audit pass.
Four expected linker objects change and 162 remain identical. The raw-array inventory identified Dihada's split 151-by-2 marked matrix;
that case is addressed by the following selection-storage pass. Other raw
arrays remain a discovery queue. These checks do not establish full gameplay equivalence.

The selection-storage pass restores Dihada's 1,208-byte marked[151][2]
matrix, original player/card columns and reverse row traversal, plus two
typed adventure target-slot source paths. Separate p/c scalar and td2 target
locals replace the prior source union. The old target-field byte-event proof
is replayed against the actual td2 local; original receipts remain immutable
through exact supersession guards. Native layouts verify the matrix and
target geometry. Direct-CFG tag analysis verifies instance provenance, an
eight-byte slot advance and exact owner/IID result stores at offsets 116/120.
Chosen indices, dynamic matrix values and arbitrary aliasing remain outside
that bounded model; source replay preserves the original search/control paths.

Build, 15,169 fixed slots, focused checks, prior target/inline audits and the
full source/provenance audit pass. Two linker objects change and 164 remain
identical. The earlier six inline semantic limits, including the adventure
helper's dead integer return, remain explicitly unresolved. No gameplay or
fresh whole-function comparison result is claimed.

The global-index pass repairs 53 expressions in 20 source functions across
17 units (13 card handlers and seven helpers). Typed player_bits and
stack_data indexing restores four-byte strides, replacing erroneous 32/512-
byte strides from whole-array pointer arithmetic. Native/rebuilt declarations
and absolute addresses match: two four-byte player-bit elements and 32
four-byte stack records with signed 16-bit IID at offset zero. Selectors,
masks, event conditions and all other source bytes are retained.

Nineteen active bodies have normalized native/before address-event matches.
After repair, 18 have exact native read/write address-event multisets. Worms
of the Earth has a separate proof of identical OR-8/AND-not-8 effects with
mutually unreachable duplicated OR blocks and no repeated direct-CFG effect
sites. Opaque/piecewise selector roots do not establish parameter identity,
arbitrary aliasing or complete gameplay equivalence. The inline-only cycling
helper retains its original Nimble Obstructionist native witness and exact
source provenance. Build, 15,169 fixed slots, focused/prior inline checks and
the full source/provenance audit pass; 17 objects change and 149 remain
identical. No fresh whole-function score or gameplay result is claimed.

The reverse-array pass repairs Pia's Revolution, its private legacy effect
and Out of Time. Typed cursors use their actual native-capacity 22/20-entry
target arrays and 151-by-2 integer matrix rather than neighboring scalar
addresses. Source guards skip the final unused decrement without changing
collection order, dialogs, ownership, flags or counter counts. Rebuilt DWARF
restores emitted locations with exact 160/176/1,208-byte extents and layouts;
the missing before locations are not proof of absent physical storage.

Direct-CFG row analysis checks eight-byte origins/reverse steps and four
same-row player/card consumer pairs, with correct fields and no pointer
register changes between paired loads. It does not establish actual indices,
bounds, all array contents, arbitrary aliases or complete gameplay behavior.
Build, 15,169 fixed slots, focused checks and the full source/provenance audit
pass. Two expected linker objects change and 164 remain identical. No fresh
whole-function score or gameplay result is claimed.

The fixed-array census follow-up repairs six original definitions. Multani
stores selected land IDs in its actual two-int array instead of relative to
the prompt; direct-CFG selected-card/index correspondence and both bounce
operands match native. A rebuilt pointer spill uses the typed instance local's
actual fixed-frame DWARF location at its reload. Generic calls still kill
tracked memory; arbitrary aliases and index bounds are not proven.

Faerie Dragon restores its 301-byte prompt. put_card_on_stack and resolve_trigger
restore separate scoped logging500/prompt300 arrays, with five formatter and
two prompt-consumer operands checked in native and rebuilt code. Reanimation
restores two int[2][500] matrices with unchanged 4,000-byte capacities; exact
source replay preserves every row/index address expression and other behavior.
Dromoka's Command restores its two-element anonymous enum choice array with
native unsigned32 elements and values1..4. All other source bytes are retained.

Build, 15,169 fixed slots, focused checks and the full source/provenance audit
pass. Six expected linker objects change and 160 remain identical. Trace-call
operands, matrix contents, arbitrary aliases, full-function equivalence and
gameplay remain outside these checks. No fresh whole-function score is claimed.

The scoped-array follow-up reconstructs six functions in five original units.
Tezzeret, multicascade and Inniaz restore separate 500/1,000/150-byte prompts,
with four formatter and three prompt-consumer operands checked. Seven scoped
test locals retain native parameter-event multisets; two token records retain
definite constant stores and API pointers. Tevesh restores rv[3] and two array-
word commander consumers. Nevinyrral restores totals[2][2], its sixteen-byte
zero initialization and four direct-CFG count/CMC updates with eight-byte rows.
Go to Jail restores r[2], both dice output pointers and their comparison.

Native/rebuilt lexical ranges verify twelve disjoint lifetime pairs for reused
allocations. Exact source replay preserves all other code. The old shared-test
audit uses actual scoped tests and retains its immutable historical artifacts.
Build, focused/prior checks, 15,169 fixed slots and the full source/provenance
audit pass. Five objects change and 161 remain identical. Opaque values, index
bounds, arbitrary aliases, full behavior and gameplay are not established.

Read-only follow-up of 59 earlier name/owner flags finds six repaired arrays,
fourteen present in canonical helper owners and nine with other local names.
Thirty require further aggregate/owner review, not necessarily source changes.
All eleven old no-current-function array flags have native-size counterparts
under canonical helper names; that reconciliation is limited to declarations
and emitted geometry.

The card-local array pass restores ten card definitions in ten units,
replacing nine merged unions with native scoped arrays, eleven tests and four
tokens. It restores two complete 100-byte message initializers and two one-IID
display arrays. Native byte-event and callback comparisons preserve existing
settings and identify two genuinely omitted Kaya's Guile token writes: flying32
and black/white color34, now present in rebuilt code. Cursor API pointers, array
dimensions, emitted extents and disjoint reused scopes have bounded evidence.

Build, focused checks, six affected prior audits, 15,169 slots and the full
source/provenance audit pass. Ten objects change and 156 remain identical.
Historical receipts are immutable. Unknown REP STOSD counts remain unknown;
current Windgrace/Reign pointer-spill operands are not credited as definite
frame pointers. Typed source replay and actual layouts cover those edits.
Dynamic values, index bounds, arbitrary aliases and complete gameplay remain
outside these checks.

Twenty array-name flags across eight functions remain for aggregate/owner
review. The next read-only token-local inventory contains 692 native fixed
instances in 677 functions, with 691 instances paired to current owners. It
does not establish missing writes or complete token behavior; further scope-
filtered scalar/callback comparison is required.

The token-scope pass restores four functions with seven actual token locals
and two tests. Scoped scalar events and API pointers match native; two Circle
vigilance writes use the proper token field. One reccmp callback mapping now
uses its verified native-address/source binding instead of a colliding name.
Build, prior/focused checks,15169 slots and full source/provenance audit pass;
four objects change and162 remain identical.

The broad read-only token-write screen has669 exact instances,18 aggregate
supersets,three value/scope cases,one multiplicity case andone unbound/inline
case. It exposes Mirror Match's wrong legacy callback as a confirmed next
repair; this checkpoint records preparation only. Packing, cloned stores,
opaque values and full gameplay remain separate evidence.

Dynamic locations, EXE-owned storage, constructor effects, runtime state and
complete global declaration ABI remain separate evidence. Gameplay tests
remain deferred by the user.

The Mirror Match follow-up fixes its token callback to the actual native
shared Innistrad end-of-combat exile helper. The old local callback destroyed
the token instead. Exact source replay changes one expression; callback type,
native address/source identity and all other scalar token writes match. Build,
focused checks, 15,169 fixed slots and the full source/provenance audit pass.
One linker object changes;165 remain identical. Evidence is in
[token-callback-recovery.json](recovery-evidence/token-callback-recovery.json)
and [token-callback-coverage.json](recovery-evidence/token-callback-coverage.json).

The refreshed692-instance token screen reports670 exact scalar-event matches,
18 aggregate views and four further flags. Independent read-only proofs resolve
all four: Vivien and specialized Fractal pack identical32-bit argument values;
the two native custom-token quantity stores are mutually exclusive and write
the same low8 bits as the single rebuilt store; the private copy_token clone
contains the expected token in an actual helper-inline scope. Both copy APIs
receive that local. No source edits were needed for these four cases.

The independent proofs pin native/rebuilt code, debug and tool hashes separately
from the full source audit. Eighteen aggregate views, dynamic values, indirect
writes, nonfixed locations, transitive helper behavior and gameplay remain
outside these targeted conclusions. The earlier checkpoint below correctly
records Mirror Match as prepared at that time; this pass applies the repair.

