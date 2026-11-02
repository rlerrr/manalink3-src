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


The read-only filter-outlier follow-up rechecks 25 old owners, including
six already repaired in the earlier batch. Seven now have identical whole-frame
scalar events. Fifteen differ only by store sites proven mutually exclusive
between verified initializations of the same local. Revoker additionally repeats
the same known type_flag value across a path without an intervening call or
memory write. Silverquill's four readonly switch tables are included only after
checking their CMP/JA bounds, index registers, targets and guard predecessors.

The remaining two event differences are locally equivalent conditional
expressions: Stitch Together assigns AI mode1/2 for zero/nonzero has_threshold
returns; Mercurial Transformation assigns subtype68/134 and corresponding1/4
power/toughness for selector equal/not equal to1. All setting-store sites have
bounded expression witnesses. Argument/selector origin and complete function
semantics are separate; the expressions are proven given the same opaque input.

Thus all 19 previously outstanding discovery rows have classified field-level
explanations, with no new source repair required in this follow-up. The six
prior fixes remain checked. Evidence is in
[test-outlier-scope-review.json](recovery-evidence/test-outlier-scope-review.json),
[test-conditional-value-review.json](recovery-evidence/test-conditional-value-review.json)
and [test-outlier-coverage.json](recovery-evidence/test-outlier-coverage.json).
The source/build remain at the successful Mirror Match checkpoint; these are
independent read-only checks, not a new full build or whole-DLL comparison.

This review corrects the old discovery collector's missing SI/DI/BP/SP16 lanes
and invalidates all explicit/implicit register definitions in its own model.
Soul Spike had a concrete stale-ESI-zero inference after a partial SI load.
Historical collector files/receipts remain unchanged; broader old screens still
need replay with the corrected model. Equal unknown events, multi-frame/unbound
owners, indirect writes, input provenance, all18 aggregate token views and full
gameplay remain separate review queues. These classifications do not establish
complete behavioral equivalence or absence of other defects.


The aggregate-token pass restores 19 source definitions in17 original units:
17 card handlers, the sacrifice helper and Ghired's native inline helper. All
18 aggregate-token owners now use actual typed locals. Nineteen merged local
unions and one unused shared-storage typedef are removed. Native/rebuilt
geometry agrees for23 target definitions,21 tokens,five filters,two target pairs
andten fixed arrays. All 21 scoped token scalar byte-event multisets match
exactly, including callback identities. Unknown dynamic values remain unknown.

EDH now has its three600-byte deck buffers, two100-byte activation prompts and
separate filter/target/token scopes. Its two short error paths expand the same
existing dialog/lose/return tail inside their original buffer scopes. Grand
Ossuary's shared selection tail moves into its actual loop-local target/prompt
scope, preserving every original destination with an explicit fallthrough jump.
The sacrifice helper has an eight-byte selected target and scoped76-byte token;
its packed-choice expression assigns the result directly without a union alias.

Ghired now calls the already-recovered ghireds_special_populate helper. That
helper also regains the native separate AI/human filter, target and token scopes.
All original conditions, costs, callbacks, literal settings and dynamic values
are retained by the recorded projection, with the explicit structural changes
above. Build, all11 affected prior audits, focused checks, 15,169 fixed slots
and the full source/provenance audit pass. Seventeen linker objects change;
149 remain identical. Historical receipts replay through exact successors.
Evidence is in [token-aggregate-recovery.json](recovery-evidence/token-aggregate-recovery.json)
and [token-aggregate-coverage.json](recovery-evidence/token-aggregate-coverage.json).

The refreshed692-instance token screen uses the corrected partial-register and
implicit-definition model. It finds 688 exact scoped event matches and zero
aggregate-token cases. The four residual flags are again explained on this
build by the packed-field, mutually exclusive quantity-store and inline-owner
proofs. Their new input hashes and results are in
[token-scalar-followup-review.json](recovery-evidence/token-scalar-followup-review.json).
They remain independent bounded checks; argument/selector provenance, arbitrary
aliases and full behavior/gameplay are separate. No fresh whole-DLL score is claimed.

Five EDH array-name flags are cleared. The remaining array discovery queue is
fifteen flags across seven functions, all with same-geometry nested-array
candidates. Their scope/API roles still need review; geometry alone is not a
missing-code or behavior verdict. The broader multi-frame/unbound, local-enum,
global/dynamic and semantic-limit queues remain open.


The final named-array pass restores seven helpers in five original source units.
Six shared-storage union typedefs and two obsolete assertions are removed. The
helpers now have their separate prompt/diagnostic arrays, target definitions and
filter records. Checks cover 19 native fixed arrays, seven filter instances,
and all nine sacrifice-filter stores (208 incoming parameter bits). Existing
formats, limits, conditions, costs and expressions are retained by exact replay.

Build, focused checks, affected historical checks, 15,169 fixed jump slots
and the full source/provenance audit pass. Five linker objects changed; 161 are
identical. Historical union snapshots remain immutable; current typed locals
are checked through explicit successor receipts. Evidence:
[final-array-locals-recovery.json](recovery-evidence/final-array-locals-recovery.json)
and [final-array-locals-coverage.json](recovery-evidence/final-array-locals-coverage.json).

All 20 flags in the earlier named-array queue now have direct array-name
matches, including the five EDH arrays from the previous batch. This closes that
declaration queue, not every array or semantic issue in the DLL. The selector's
error-buffer formatter source reload remains unknown in the conservative model;
no proof is credited for it. Full dynamic value provenance, indirect/indexed
writes, multi-frame/unbound cases and gameplay remain open. No fresh whole-DLL
comparison score is claimed.

The next requested work is readability: reconstruct structured control flow,
replace remaining synthetic local unions with actual typed locals, and recover
useful names from DWARF. An initial screen finds 5,281 LAB jumps in 1,432 compiled
functions, 7,563 distinct per-function placeholder spellings in 2,148 functions,
and 98 local union declarations in 96 screened functions. These are review
candidates, not confirmed defects. Native local names are present for 2,615 of
the 2,642 screened functions. A role-based instance-name proposal narrows 348
initial candidates to 188 by rejecting additional assignment roles and address
or field uses. No readability source edits are included in this array batch.


The first readability batch renames 123 unambiguous card-instance pointer
locals across 54 source units, replacing 1,477 placeholder occurrences with
`instance`. Each local is assigned only `get_card_instance(player, card)`; there
is no destination collision, address-taking or additional assignment role. Native
DWARF supplies a matching pointer spelling/type, which is useful vocabulary
rather than proof of one-to-one identity after optimization.

The build passes. All 26 normalized runtime sections are identical to the
preceding audited build; only the export timestamp is masked, and debug sections
and debuglink are excluded. All 54 touched files replay exactly from checked
predecessors through the recorded source edits. The earlier full native setup
audit was not repeated for this identifier-only batch. Evidence:
[local-identifier-cleanup.json](recovery-evidence/local-identifier-cleanup.json)
and [local-identifier-cleanup-coverage.json](recovery-evidence/local-identifier-cleanup-coverage.json).

The pre-cleanup screen and its limits are preserved in
[readability-inventory.json](recovery-evidence/readability-inventory.json).
Sixty-five further clear instance-role candidates have earlier body-specific
receipts to update before renaming. The separate review queue retains their
source hashes and native names. LAB control-flow reconstruction and remaining
synthetic union removal are still open; this naming batch claims neither.


The typed-local cleanup removes 66 synthetic unions from 66 functions in
36 original source units (61 card handlers and five helpers). They regain
65 filters, 38 target definitions and 29 tokens, named from native DWARF and
declared in the lowest enclosing source compound containing their uses. One
duplicate native `td` spelling becomes `td2` to avoid shadowing an existing local.
Two hundred thirty-six writes through filter-message bytes and other overlapping
member accesses become 74 typed field assignments. Existing constants,
expressions and helper calls are retained by a deterministic checked projection.

Guild Summit now has structured chooser, trigger and draw checks in place of
seven LAB jumps and five labels, and its counter uses the native name `count`.
The unchanged chooser/trigger bodies are extracted from the typed predecessor;
368 abstract path cases compare the old label transitions with the new checks,
allowing those helper calls to change facts used by subsequent stages.

Build, focused checks, eight affected prior audits, 15,169 fixed jump slots
and the full source/provenance audit pass. Geometry agrees throughout. Of142
scalar-record comparisons, 139 match exactly and three doubled groups have
identical stores on CFG-proven mutually exclusive paths. All29 token records
match their native scoped scalar-byte events exactly. Thirty-six objects change;
130 remain identical. Historical union receipts and binaries remain intact;
explicit source successors and current typed-record bindings carry them forward.

Evidence: [disentangled-pair-locals.json](recovery-evidence/disentangled-pair-locals.json)
and [disentangled-pair-locals-coverage.json](recovery-evidence/disentangled-pair-locals-coverage.json).
Filter scalar checks exclude the100-byte prompt region, where optimized native
frame/scope reuse can include unrelated writes. Unknown dynamic values stay
unknown; full gameplay, all aliases, indexed stores and helper effects remain
separate. No fresh whole-DLL comparison score is claimed.

An independent C AST review accounts for all296 direct addresses of restored
records: each is an immediate call argument, with no direct pointer assignment
or return concealing a later source consumer. Parser typedef stubs establish
syntax only; callee-internal pointer retention is not inferred.

A fresh DWARF census without the old syntax prefilter finds 32 synthetic
union locals remaining across30 functions. Twenty-eight card functions use
merged record storage, the granted-activation helper has one216-byte storage
union, and generic_spell has three four-byte register-word unions. The timer's
two native LARGE_INTEGER unions are genuine and retained. Full results and
scope limits are in [remaining-local-union-review.json](recovery-evidence/remaining-local-union-review.json).
Further LAB control-flow reconstruction, placeholder names, local enums and the
broader semantic/global/frame queues remain open.


The remaining card-union batch removes 28 synthetic unions from 28 card
functions in 20 original units, restoring 83 typed locals: 42 filters,
33 target definitions and eight tokens. A conservative source CFG tracks
90 initializer sites and 342 record references. Every reference has a known
reaching constructor on all syntactic paths; no event-satisfiability assumption
is needed. Three native locals have several initializer paths. Fifteen typed
field rewrites replace overlapping-view stores, preserving the original values.

Najeela and Biomancer's Familiar now have structured event dispatch, removing
four LAB jumps and four labels. Biomancer's repeated leaves_play calls share
one call with the unchanged by-value event argument, preserving each path's
effect order. The24 dispatch cases are tied to seven distinct, equal native and
rebuilt event-enum values. Constructor/consumer blocks retain their expressions
and helper arguments; source projection records the explicit dispatch changes.

Build, focused checks, six affected earlier audits, 15,169 fixed jump slots
and the full source/provenance audit pass. Geometry agrees with the unchanged
baseline typed declarations plus all83 restored native common-frame records.
Their scalar store profiles give82 exact matches and one CFG-proven exclusive
compiled copy. Twenty objects changed; 146 remain identical. Earlier whole-frame
test receipts retain their snapshots and bind current typed filters only when
those filters actually share one checked frame offset.

Evidence: [multiphase-card-locals.json](recovery-evidence/multiphase-card-locals.json)
and [multiphase-card-locals-coverage.json](recovery-evidence/multiphase-card-locals-coverage.json).
Contained same-frame debug scopes are excluded from broader store profiles.
This is bounded corroboration, not unique variable identity or opaque-value
provenance. Five split cards already reuse other same-kind target locals across
native inline scopes; that baseline difference is not credited as restored.
Full aliases, indirect/indexed values, helper retention and gameplay remain
separate. No fresh whole-DLL comparison score is claimed.

The independent syntax review accounts for all196 record addresses as direct
call arguments, with no direct pointer assignment/return hiding a later source
consumer. A fresh whole-CU DWARF census finds zero union-valued locals in the
compiled card functions. Four synthetic union locals remain in two helpers:
the granted-activation storage object and generic_spell's three four-byte word
views. The timer's two genuine native LARGE_INTEGER unions remain intact.
See [remaining-helper-union-review.json](recovery-evidence/remaining-helper-union-review.json).
Other placeholder names, LAB flow, local enums and broader semantic/global/frame
reviews remain open.


The final helper-union batch removes all four remaining synthetic union
locals from `generic_spell` and `granted_generic_activated_ability` in
`functions/functions.c`, along with their two unused union typedefs. The
activation helper now uses separate target definitions, a filter and an output
target pair. Generic-spell storage now has distinct flags, limits, card-instance
pointers, player/card values and saved target counts. Its artificial two-word
temporary is replaced by `rv` and `min_targets`, and the player dialog saves a
typed `old_t0` plus `old_nt`. Native DWARF supplies names and type information.

This review also corrects a native behavior recovery error: the graveyard
player dialog must assign `td9.who_chooses = p` (offset 0), rather than assigning
`td9.player` (offset 72). Native DWARF places `p` at CFA-248 in the relevant
range, and the concrete native instruction window loads that word into EAX
and stores EAX to the chooser field. Caller-input provenance remains separate.

Build output, focused checks, the affected array/card checks and the full
source/provenance audit pass, including 15,169 fixed jump slots. All six
native record scalar profiles match exactly. Byte-lane CFG checks account for
23 union writes and 40 reads across six event classes without partial-word or
pointer/integer reinterpretation dependencies. Local liveness checks support
the discarded copies; 48 aura and 720 grave-search predicate classes preserve
helper-call order and dynamic loop-bound reloads. One linker object changed;
165 are identical to the preceding build. These checks do not establish full
function equivalence or gameplay correctness.

A fresh census of all concrete `cards/` and `functions/` CUs finds zero
synthetic union-valued locals. The only two remaining union locals are the
native Windows `LARGE_INTEGER` values `ticks` and `frequency` in `time_in_ms`.
Pointed-to unions, aggregate members and unbuilt configurations are outside
that count. Earlier remaining-union records are historical snapshots.

Evidence: [final-helper-locals.json](recovery-evidence/final-helper-locals.json),
[final-helper-locals-coverage.json](recovery-evidence/final-helper-locals-coverage.json)
and [remaining-helper-union-review-complete.json](recovery-evidence/remaining-helper-union-review-complete.json).
The remaining readability work includes LAB control flow and placeholder
names. Broader semantic, global, enum, frame and gameplay review remains open;
no fresh whole-DLL comparison score is claimed.


The next readability batch changes 72 definitions in their original units.
65 unambiguous card-instance pointers become `instance`, replacing 724
placeholder uses in 37 source units. Token-aware projection retains every
nonidentifier byte. The naming build has 26 identical normalized runtime
sections; only debug information changes in 37 linker objects. Native DWARF
provides the pointer spelling and 300-byte type vocabulary, rather than a
claim of unique optimized-variable identity. Earlier semantic receipts remain
intact and keep their validators. Their source guards accept this extension
only at the exact immediate pre-rename digest; no arbitrary predecessor graph
or virtual source replacement hides other edits.

Seven Throne of Eldraine adventure handlers now use structured event dispatch:
Curious Pair, Embereth Shieldbreaker, Foulmire Knight, Murderous Rider, Order
of Midnight, Realm-Cloaked Giant and Rimrock Knight. This removes 14 LAB
jumps, seven labels and six unused result locals. Common prefixes and helper
arguments remain unchanged. A parsed statement-CFG interpreter checks 252
cases: eight distinct native event values plus an other-event class, crossed
with both independent affected-card/player equality results. Every common
helper call and its arbitrary return value is retained. Prefix helpers may
change globals before dispatch; no callee purity assumption is introduced.
The second build changes only the Throne object; 165 remain identical.

Both builds, focused checks, affected older audits and the full
source/provenance audit pass, including 15,169 fixed jump-table slots.
Evidence: [additional-local-names.json](recovery-evidence/additional-local-names.json),
[spell-dispatch-cleanup.json](recovery-evidence/spell-dispatch-cleanup.json)
and [readability-cleanup-coverage.json](recovery-evidence/readability-cleanup-coverage.json).

The refreshed simple-pointer screen has zero candidates: all 188 previously
identified cases are handled across the two naming batches. This does not
finish placeholder cleanup. The current syntax screen still counts 5,256 LAB
jumps in 1,422 functions and 7,364 distinct per-function placeholder spellings
in 2,147 functions. A new read-only queue finds 1,673 call-result assignments
in 783 functions whose value appears to feed only the immediately following
zero/nonzero condition. Of these, 1,659 have a plain-int header declaration candidate.
Macro expansion, precise function/type binding, aliases and source successors
must be reviewed before applying that queue; none of those proposed rewrites
is credited as recovered source. Twenty-nine functions failed the syntactic
parser and five large bodies were skipped. The screen is scoped to ordinary
C definitions with concrete compiled owners, not every build configuration.
See [readability-followup-inventory.json](recovery-evidence/readability-followup-inventory.json).
Broader semantic/global/enum/frame review and gameplay remain open. No new
whole-DLL comparison score is claimed.


Routine comparison setup no longer runs the entire reconstruction history.
`python tools/reccmp_project.py setup` now validates current source-file hashes,
canonical name/ledger references, original binary hashes, rebuilt debuglink
pairing and all 15,169 built jump-table/COFF bindings. Historical source-edit
replay, archived builds, supporting proof inputs and the old per-batch native
audits remain available explicitly with `setup --audit-recovery` or
`python tools/recovery/canonicalize_helpers.py --verify`. The original full
`verify()` implementation is unchanged. The report labels routine results
`current_inputs`; they must not be described as a full recovery audit.

The timed full-history baseline passes and invokes 127 native audit entries
(126 distinct entrypoints), spending 412 seconds inside those audits. The
header-name audit alone takes 63 seconds. Its setup-log wall span is about 533
seconds. Routine setup completes in 68.3 seconds, generates the identical
CSV, preserves all mapping/exclusion/alias/stub results and checks all fixed
slots against the same unchanged binaries. Isolated checks reject changed
source text, an accepted ledger record and a projection recipe. The optional
full-history flag appears in command help. No product source or build artifact
changes in this pass.

The current [reccmp instructions](reccmp.md) describe the shorter workflow and
correct the old statement that the recovered Wish picker is still a stub.
Evidence: [audit-workflow-simplification.json](recovery-evidence/audit-workflow-simplification.json).
Focused behavior/representation checks remain appropriate when changing
recovery code. Replaying every archived experiment is no longer a prerequisite
for generating comparison metadata. The queued call-result cleanup is still
unapplied and remains the next source-recovery work.


The call-result cleanup inlines 1,515 throwaway assignments across 718
definitions in 110 original source units: 576 card handlers and 142 other
helpers. 131 unused local declarations disappear, removing 3,161 placeholder
identifier uses and 1,672 source lines. These are readability changes to existing
reconstructions; no new native function coverage is claimed.

Each selected local has private automatic storage, a unique nonvolatile plain
`int` declaration and no direct address escape. Its call assignment feeds the
sole adjacent zero/nonzero condition, and the stored result is dead at both
condition successors. The same checks run on GCC-expanded source, and actual
GCC original-tree call-expression types are signed 32-bit integers. This avoids
relying on abbreviated DWARF declarations, some of which omit return types.
Units using line-sensitive macros are deferred from this batch. Calls retain
their arguments and admission paths; native bugs and other control flow remain
as reconstructed.

The build and focused source/expanded-CFG/compiler-type checks pass. 716
edited definitions retain identical machine code. The only 176 differing
`.text` bytes are confined to Otherworld Atlas and Liliana's reanimation
helper. Their function extents, calls, branch destinations and argument values
are retained; GCC reschedules the same argument calculations. Twenty-four
normalized runtime sections are identical. `.text` and `.reloc` differences
are bounded to the two reviewed function ranges, preserve relocation target
words, and leave every function address unchanged. Instruction snapshots bind
the manual scheduling review; they do not constitute a general machine-code
equivalence proof.

Six affected older checkers pass after live cleanup validation. Full-history
mode gives old format-specific Python readers the checked per-function
pre-cleanup spelling, while preserving other current source. Files, compilers,
binaries and DWARF stay unchanged. Lazy imports and retained readers are
disabled/restored on scope exit, and an unreviewed live-source change is
rejected before the historical view starts. Routine setup remains on actual
current files and runs the fast `current_inputs` checks. Its 15,169 fixed slots
pass and its comparison CSV is identical. The complete historical suite was
not rerun for this batch.

Evidence: [call-condition-cleanup.json](recovery-evidence/call-condition-cleanup.json)
and [call-condition-cleanup-coverage.json](recovery-evidence/call-condition-cleanup-coverage.json).
158 candidate statements remain unapplied, primarily 154 statements in
units with line-sensitive assertions; three expanded forms and one stricter
flow case need further review. See
[call-condition-followup-queue.json](recovery-evidence/call-condition-followup-queue.json).
They are cleanup candidates, not 158 unrecovered native helpers. New compiler
artifacts and transaction snapshots were compressed losslessly to recover
disk space. Original inputs and earlier recovery archives remain intact.
Gameplay, other build configurations and broader native semantic review stay
open; no fresh whole-DLL similarity score is claimed.


The assertion-unit followup removes another 154 throwaway call-result
assignments in 63 functions across 15 original source files (15 card handlers
and 48 other definitions), plus 20 unused int locals. It removes 176 source
lines and 328 placeholder identifier uses. The cumulative adjacent-call cleanup
now covers 1,669 assignments in 781 definitions/125 source units, with 151 unused
locals removed. This adds no new native function coverage.

Each rewrite passes the raw and GCC-expanded statement CFG/liveness checks and
uses an actual signed 32-bit GCC call result. All 15 complete expanded source
units match their projected rewrites, allowing only whitespace and the verified
source-line literals in 45 ASSERT failure messages. Filenames, conditions,
stringified assertion text and other literals remain checked. Diagnostics use
the correct current source lines; no line padding or byte-matching workaround
was added. All 15 units also replay exactly from their original predecessor
archives through the ordered source edits. The combined historical reader view
reconstructs all 781 checked predecessor definitions without changing files.

The build, focused checker and routine metadata setup pass, including all
15,169 fixed slots; the comparison CSV is unchanged. The full historical suite
and whole-DLL byte comparison were not run. Four candidate statements remain:
one GNU statement-expression expansion in Torgaar, two expanded local-type or
offsetof forms in Karn, and one EXE_FN function-pointer call. These are parser
or binding followups, not a count of unrecovered helpers.

Evidence: [assertion-call-condition-cleanup.json](recovery-evidence/assertion-call-condition-cleanup.json),
[coverage](recovery-evidence/assertion-call-condition-cleanup-coverage.json), and
[remaining queue](recovery-evidence/call-condition-followup-queue.json).
Gameplay and broader retained-function/native semantic review remain open.


The original adjacent-call temporary queue is now resolved: 1,673 inlines
across 783 distinct definitions in 125 source units, with 151 unused locals
removed. The final four rewrites are in Torgaar, Karn and charge_spell_cost.
They add no new native function coverage.

Narrow offset-preserving analysis views handle Torgaar's pure GNU minimum
expression, Karn's compile-time offsetof and the engine's literal executable
callee casts. The actual product code and compiler inputs retain those original
constructs. The selected executable call still uses address 0x4302c0, an int
return and four int arguments. Raw/expanded CFG liveness, compiler result-type
checks and complete before/after GCC expansion comparisons pass. The combined
call-result checker passes with zero pending statements. All three edited units
replay exactly from the checked original predecessor archives. Historical source
views now unwind edits in reverse order, including the engine function that had
two successive cleanup receipts; all 784 receipts cover 783 distinct functions.

The build and routine reccmp setup pass, including all 15,169 fixed slots.
The comparison CSV is unchanged. The full historical suite and gameplay tests
were not run; broader native semantic review and control-flow cleanup remain
open. Evidence: [special-call-condition-cleanup.json](recovery-evidence/special-call-condition-cleanup.json),
[coverage](recovery-evidence/special-call-condition-cleanup-coverage.json), and
[resolved queue](recovery-evidence/call-condition-followup-queue.json).


The next unapplied cleanup queue has 1,924 direct call-result/return pairs in
1,519 definitions across 125 source units. The discovery screen requires a
plain-int caller, a unique private automatic nonvolatile int result local with
no direct address escape, and an adjacent return whose sole predecessor is the
call assignment. Actual GCC return types, macro-expanded code and predecessor
receipt succession still need review; these are candidates, not accepted edits.
See [call-return-candidate-queue.json](recovery-evidence/call-return-candidate-queue.json).
The simple forward-jump inversion screen found only one match, so it is not a
productive broad control-flow batch. Larger shared event dispatchers remain open.


The direct-call return batch removes 1,924 throwaway assignments in 1,519
functions across 125 original source units (1,421 card handlers and 98 other
definitions). 83 unused locals disappear, removing 2,081 source lines and
3,931 result-identifier uses. Both adjacent-call cleanup queues are resolved:
3,597 assignments in 1,834 distinct functions/140 source units, with 234 unused
locals and 3,934 lines removed. These are improvements to existing recovered
source, with no additional native function coverage claimed.

Raw and GCC-expanded statement CFGs bind a unique nonvolatile private automatic
int object to the immediately following return, with no alternative return
predecessor or direct address escape. Actual expanded caller and local types
are both int. A direct return therefore performs the same conversion as the
original assignment, even when the helper itself returns another scalar type.
The original call and arguments remain intact. All 125 complete expanded units
match their projected edits, allowing only whitespace and the independently
checked line literals in 38 ASSERT failure messages. Declaration removal is
allowed only when both raw and expanded views agree it is unused. All 125
before/after source pins follow the accepted canonical ledger.

The build and combined focused checks pass for all 1,924 current direct returns
and the earlier 1,673 conditional-result rewrites. Explicit historical Python
C-source readers can see checked predecessor text and byte inputs after live
validation; filesystem, compiler, binary and debug I/O remain physical/current.
Readers restore on scope exit, and an injected unreviewed source change is
rejected before a historical view starts. Routine reccmp setup passes all
15,169 fixed slots and retains the same comparison CSV. The full historical
suite, whole-DLL byte comparison and gameplay tests were not run.

The fresh source-only inventory of the 2,604 previously tracked definitions
still finds 5,256 LAB gotos in 1,422 functions and 7,204 placeholder spellings
across 2,142 functions (63,592 identifier occurrences). It finds no union
keywords within those bodies. Native/compiled-owner bindings in the original
DWARF census remain historical vocabulary; this scan does not establish new
optimized-local identities or count unrecovered native functions. Shared
control-flow reconstruction, reused-local roles and broader native semantic
review remain open.

Evidence: [call-return-cleanup.json](recovery-evidence/call-return-cleanup.json),
[coverage](recovery-evidence/call-return-cleanup-coverage.json),
[resolved queue](recovery-evidence/call-return-candidate-queue.json), and
[current source inventory](recovery-evidence/readability-source-snapshot-c119.json).


The native-vocabulary scalar pass names 164 locals in 163 functions across
76 original source units (119 card handlers and 44 other definitions), replacing
1,930 identifier uses. This includes 91 `i` and 48 `c` loop indices, counter
quantities, dialog choices, target-card values, CSV ids, CMC values and one power
value. These are naming changes to existing source, not new native coverage.

Each selected object is a unique ordinary function-scope int with no direct
address escape. Its writes retain a stable loop, helper-result or target-card
role in both raw and GCC-expanded source. The new spelling is absent in both
views before substitution, preventing capture; every other token, source line
and diagnostic literal remains unchanged. All 76 files pass both preflight and
actual whole-unit expanded comparison. Original debug information supplies 190
concrete signed-int32 local references under the owning native function, with
parameters and inlined helper scopes excluded. Names provide verified native
vocabulary matched to source roles; optimized-variable identity is not claimed.
Twelve weaker proposals remain unapplied because their initially chosen name
has no direct local in the owning function; different evidence may resolve them.

The build passes all 15,169 fixed slots. All 26 normalized runtime sections are
identical to the naming predecessor; debug metadata changes. Current naming,
source-pin succession and source-reader isolation checks pass. Exact prior-source
input hashes are checked for the 125-unit return batch and 18 earlier assertion
or special-case inputs without rerunning those older semantic checkers. Readers
restore and reject an unreviewed live edit before enabling historical views;
physical source/compiler/binary I/O stays current. Routine reccmp setup passes
and retains the same comparison CSV. The CSV writer now replaces its completed
output atomically after Windows rejected reopening the existing file. Earlier
raw debug snapshots were compressed transparently with unchanged byte hashes
to recover disk space; an interrupted evidence-pin write was finished without
reapplying source mutations. The full historical suite and gameplay tests were
not run.

The updated source-only screen of the 2,604 tracked definitions finds 7,050
placeholder spellings across 2,132 functions, totaling 61,716 identifier uses.
The 5,256 LAB gotos in 1,422 functions remain. No union keywords occur within
these tracked bodies. Native owner bindings in the original census remain
historical vocabulary; these counts are not remaining native-function counts.
Shared control flow, mixed-role locals and broader native semantic review remain
open.

Evidence: [role-local-names.json](recovery-evidence/role-local-names.json),
[coverage](recovery-evidence/role-local-names-coverage.json),
[rejected proposals](recovery-evidence/role-local-name-followup-queue.json), and
[current source inventory](recovery-evidence/readability-source-snapshot-c130.json).

The shared-spell dispatch pass structures 148 card handlers in 44 original
source files. Each now uses the existing `IS_GS_EVENT` guard, one copy of its
common setup, the unchanged resolution block and the unchanged tail return.
This removes 148 LAB gotos, 148 labels and 3,596 source lines. The 2020
`card_barals_expertise` provides a style reference; the accepted 2022 payloads,
arguments and event behavior are retained.

Focused checks bind the native owners and five event constants for all 148
handlers, compare the actual expansions of all 44 compilation units, and check
28,416 event/action traces, including affinity checks and early resolution
returns. In 83 handlers an added pointer capture is private and unread outside
resolution. The build passes all 15,169 fixed slots. Exact historical source
input hashes and reader restoration pass without replaying the older semantic
audits. Compiler output changes; byte matching is not a requirement. This
preserves the accepted reconstruction within the checked dispatch family, not
a full native or gameplay equivalence claim. No native function coverage is
added.

The source-only inventory now finds 5,108 LAB gotos in 1,274 of the 2,604 tracked
definitions. Placeholder counts remain 7,050 spellings and 61,716 uses; there
are no union keywords in these bodies. Routine setup continues to check current
inputs; the full historical audit remains an explicit opt-in.

Evidence: [shared-spell-cleanup.json](recovery-evidence/shared-spell-cleanup.json),
[coverage](recovery-evidence/shared-spell-cleanup-coverage.json), and
[current source inventory](recovery-evidence/readability-source-snapshot-c138.json).

The follow-up shared-spell pass structures another 22 card handlers in 15
original files, removing 23 outer LAB gotos, 22 labels and 521 source lines.
It accepts varied pure event/affinity branch layouts, preserves jumps contained
within the resolution block, and retains any straight-line common tail. All
constructor sequences must match in order, apart from checked private instance
captures. The source-identical payload must fall through directly to the tail.
Prefix behavior, including landcycling and split-card events, stays intact.

All 22 native owners and compiled event bindings pass, along with 4,224 parsed
action/guard trace cases and all 15 actual expanded compilation units. The build
passes all 15,169 fixed slots. Exact earlier input hashes are checked for the
44-unit shared-spell batch, 76-unit naming batch, 125-unit return batch and 18
assertion/special units; the older semantic checkers are not replayed. Routine
reccmp setup passes with all 18,725 comparison entries. Compiler code changes;
this is preservation of the accepted reconstruction, not new native coverage
or a full native/gameplay equivalence claim.

The source-only inventory now counts 5,085 LAB gotos in 1,262 tracked functions.
A separate unapplied screen finds 196 two-element comma call-result/zero-test
pairs across 163 functions and 61 units. Each selected int temporary has no
other identifier uses. Compiler return types, macro-expanded bindings and exact
expression boundaries remain to be checked before approving those candidates.

Evidence: [shared-spell extension](recovery-evidence/shared-spell-extension.json),
[coverage](recovery-evidence/shared-spell-extension-coverage.json),
[current source inventory](recovery-evidence/readability-source-snapshot-c147.json),
and [comma-result candidates](recovery-evidence/comma-call-result-queue.json).

The comma-result queue is now resolved: all 196 call-result/zero-test pairs in
163 functions across 61 original units use direct comparisons, removing 163
private int declarations and 169 source lines. This covers 139 card handlers
and 24 other functions. Every identifier reference to each selected object is
one of the admitted assignment/test operands; function argument lists are
excluded from comma-operator matching. Actual GCC assertions at all 196
expanded call sites prove exact int result compatibility, making the removed
conversions identities. Calls retain their evaluation count, arguments and
expression admission.

All 61 actual expanded units match the projection, including checked diagnostic
line changes at 12 ASSERT sites. The build passes all 15,169 fixed slots and all
26 normalized runtime sections are identical to the predecessor. Focused checks
replay the type assertions and validate exact predecessor input hashes without
rerunning earlier semantic checkers. Routine reccmp setup passes and produces
the identical 18,725-entry CSV. An interrupted metadata write was finished from
the checked source snapshots without reapplying edits. Old debug snapshots were
compressed losslessly with their paths and byte hashes retained; the failed,
unregistered full-unit GCC tree-dump experiment was pruned.

The current tracked bodies contain 6,887 placeholder spellings and 61,161 uses,
555 fewer identifier occurrences. The 5,085 LAB gotos in 1,262 tracked functions
are unchanged. A broader source-only screen finds 67 further comma-comparison
candidates in 48 functions/31 units, including other literal comparisons and
ordinary int result names. Those still require compiler and expansion checks.
There is no new native function coverage or gameplay validation in this pass.

Evidence: [comma-call cleanup](recovery-evidence/comma-call-cleanup.json),
[coverage](recovery-evidence/comma-call-cleanup-coverage.json),
[current source inventory](recovery-evidence/readability-source-snapshot-c157.json),
and [remaining comparison candidates](recovery-evidence/comma-comparison-queue.json).

The broader literal-comparison queue is also resolved: 67 pairs in 48 functions
across 31 original units now use direct calls, removing 48 private int locals
and 62 lines. The projector admits ordinary temporary names and side-effect-free
integer/character literals, including signed literals and relational operators.
Actual GCC assertions bind all 67 expanded call results to int. The earlier
196 raw and expanded projections still reproduce exactly with the generalized
projector. All 31 actual expanded units match, including diagnostic line changes
at 17 ASSERT sites. The build, current focused checks and predecessor-input
compatibility pass; earlier semantic audits are not replayed. All 26 normalized
runtime sections and the 18,725-entry comparison CSV remain identical.

The current source-only inventory has 6,879 placeholder spellings and 61,129
identifier uses. LAB counts remain 5,085 in 1,262 tracked functions. The next
control-flow screen identifies 24 retry/skip loops in 24 functions across 17
files. On failure, the inner loop increments the same private int and jumps
when the complement of the outer bound holds. Parsed CFGs show that normal
outer-loop exit and that jump reach the same action. This is discovery evidence;
integer bound types, expanded structure and a complete before/after CFG proof
remain to be checked before edits. The 2020 Sylvan Reclamation source confirms
the intended straightforward target-loop style; its 2022 payload is retained.
There is no new native function coverage or gameplay testing in these cleanups.

Evidence: [literal comparison cleanup](recovery-evidence/comma-comparison-cleanup.json),
[coverage](recovery-evidence/comma-comparison-cleanup-coverage.json),
[current source inventory](recovery-evidence/readability-source-snapshot-c165.json),
and [target-loop candidates](recovery-evidence/target-loop-queue.json).

The bounded retry-loop queue is resolved: 24 loops in 24 functions across 17
original units now use skip-and-continue branches in their existing outer
do/while loops. This removes 24 LAB gotos, 23 labels and 47 source lines across
21 card handlers and three helpers. The valid payloads, counter updates and
initial loop admission remain in place. A label shared with another jump in
`reveal_cards_from_your_hand` is retained.

Full before/after statement CFG comparisons cover 1,051 state pairs in both
raw and expanded source. Only the admitted integer exit guards are complemented;
all other expressions, calls, bound reads, branches and returns remain ordered.
Actual GCC assertions prove integer bound operands at all 24 sites, and all 17
actual expanded units match the projection, including checked diagnostic lines
at 12 ASSERT sites. Negative controls reject a break, altered increment, changed
valid action or wrong loop bound. The build passes all 15,169 fixed slots and
predecessor-input compatibility checks pass without replaying older semantic
audits. Compiler output changes; no byte matching or gameplay test is claimed.
Routine setup retains all 18,725 reccmp entries and mapping identities while
refreshing rebuilt COFF address annotations.

The tracked bodies now have 5,061 LAB gotos in 1,246 functions. Placeholder
spellings remain 6,879, with 61,116 identifier occurrences. A new source-only
screen identifies 36 private zero-initialized direction-state flags used in 49
stride expressions across 36 functions/31 units. Their expanded bindings and
surrounding operand types remain unchecked. Any fold must preserve unsigned
arithmetic, particularly around pointer-to-integer address calculations; the
surrounding clear/copy loops are not yet approved for library-call replacement.
No native function coverage is added by these readability changes.

Evidence: [bounded loop cleanup](recovery-evidence/target-loop-cleanup.json),
[coverage](recovery-evidence/target-loop-cleanup-coverage.json),
[current source inventory](recovery-evidence/readability-source-snapshot-c173.json),
and [direction-state candidates](recovery-evidence/direction-flag-queue.json).

The entry-zero direction-state pass removes 36 private byte locals and folds
49 stride expressions in 36 functions across 31 original units (26 card
handlers and ten helpers). This removes 72 source lines and 121 placeholder
identifier occurrences. Every selected flag is assigned zero at entry and has
no references except the recognized stride reads. Forward steps use unsigned
literals, including folded word-size factors, preserving unsigned address
arithmetic. The surrounding memory loops and their reads/writes remain in place.

All 49 actual expanded sites pass GCC checks for primitive nonvolatile flag
storage, constant values, old/new expression-type identity, integer/pointer bases
and the 32-bit model. All 31 actual expanded units match, including independently
checked diagnostic lines at 28 ASSERT sites. Unsafe-state controls reject later
writes, address escapes and volatile flags. The build passes all 15,169 fixed
slots, and predecessor-input compatibility passes without older semantic replay.
Raw executable sections are byte-identical to the predecessor. The only runtime
data differences are ten digits in eight ASSERT line-number strings; every
message and corresponding source assertion is unchanged. Routine setup retains
the identical 18,725-entry reccmp CSV. Evidence files use writable NTFS compression
without changing their contents or hashes.

The preceding inventory checkpoint contained 6,843 placeholder spellings and
60,995 identifier occurrences. Its 5,061 LAB gotos in 1,246 functions are
unchanged by the following batch. The four residual contexts are now resolved: eight more unsigned stride
expressions are folded, three obsolete bytes and four dead initializers removed,
and Bloodletter's later character byte uses its native unsigned-char name `ltr`.
Reaching-zero and liveness checks cover declaration initialization, character-byte
reuse and setup before initialization; the existing engine-call syntax adapter
leaves physical engine calls intact. All four actual expanded units, eight GCC
type/value probes and the build pass. All 26 normalized runtime sections and the
18,725-entry comparison CSV are unchanged. See
[residual direction cleanup](recovery-evidence/residual-direction-cleanup-coverage.json). No memory-library
replacement, new native function coverage or gameplay testing is claimed.

Evidence: [direction-state cleanup](recovery-evidence/direction-flag-cleanup.json),
[coverage](recovery-evidence/direction-flag-cleanup-coverage.json),
[preceding source inventory](recovery-evidence/readability-source-snapshot-c184.json),
and [resolved contexts](recovery-evidence/direction-flag-followup-queue.json).

The direct-return cleanup replaces 895 LAB gotos with their unchanged return
expressions in 474 functions (441 card handlers and 33 helpers), across 90 source
files. It removes 495 labels and 451 lines. Expression identifiers retain unique
outer bindings; string and compound literal objects are excluded. Whole-function
control-flow comparisons match 20,403 state pairs in both raw and expanded source.
All 90 actual expanded units, 21 ASSERT diagnostic sites, the build and all 15,169
fixed slots pass. Earlier source inputs remain compatible without replaying old
semantic audits. GCC code/layout changes; comparison metadata retains all 18,725
entries and mapping identities. The only CSV changes are 14 rebuilt public-entry
address annotations, verified by reconstructing the previous CSV byte for byte.
The tracked inventory now has 4,166 LAB gotos in 1,026 functions, down from 5,061
in 1,246. This is source readability recovery, not new native function coverage.
See [direct-return coverage](recovery-evidence/return-tail-cleanup-coverage.json),
[current inventory](recovery-evidence/readability-source-snapshot-c201.json), and
[remaining return contexts](recovery-evidence/return-tail-followup-queue.json).

The forward-guard cleanup replaces 31 conditional LAB jumps with guarded blocks
in 27 functions (21 card handlers and six helpers), across 23 source files. It
removes 11 labels and adds 20 lines for block structure. Conditions evaluate once;
logical negation swaps successors without assuming integer comparison operands.
No declarations move into narrower scopes, and all actions retain their order.
Whole-function CFG comparisons match 4,514 state pairs in both raw and expanded
source. All 23 complete actual expansions, nine ASSERT sites, the build and all
15,169 fixed slots pass. Earlier source inputs remain compatible without old
semantic audit replay. Routine setup retains all 18,725 comparison entries and
mapping identities; only 14 rebuilt public-entry annotations change, verified
by reconstructing the previous CSV exactly. The tracked inventory now contains
4,135 LAB gotos in 1,026 functions. No new native function coverage is claimed.
See [forward-guard coverage](recovery-evidence/forward-guard-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c210.json).

The scalar-lifetime pass separates 248 independent int value groups into 240
native-vocabulary bindings across 233 functions (201 card handlers and 32 helpers)
in 77 source files. It renames 926 placeholder occurrences, reuses one obsolete
placeholder declaration and adds 239 ordinary int declarations. Shared reads merge
predecessor values; uninitialized, opaque-write and address-escaping states remain
outside naming. Right-hand-side inputs retain their preceding value even when a
call result replaces the same old temporary. Compatible call/field roles and 336
direct, non-inline native int32 local references support the names; optimized
variable identity is not asserted. Merging the new bindings reproduces every
predecessor byte. Four unsafe-lifetime controls are rejected, and compatible
branch joins and old RHS inputs are preserved.

All 77 complete actual expansions, 30 ASSERT diagnostic sites, the build and all
15,169 fixed slots pass. Earlier source inputs remain compatible without old
semantic replay. Routine setup retains all 18,725 entries and mapping identities;
only 13 rebuilt public-entry annotations change, checked by reconstructing the
previous CSV exactly. Placeholder identifier uses fall from 61,038 to 60,112;
4,135 LAB gotos remain unchanged. One expanded statement-expression parser case
is deferred. This is readability recovery of accepted code; gameplay tests and
new native function coverage are outside this pass.
See [scalar-lifetime coverage](recovery-evidence/scalar-lifetime-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c218.json).

The getter/field follow-up separates 31 further int value lifetimes into 31
native-vocabulary bindings across 26 functions (15 card handlers and 11 helpers)
in 20 files. It renames 79 placeholder uses, reuses four old declarations and adds
27 ordinary int declarations. Additional getter roles distinguish original CSV
and internal card IDs, ownership and mana values. Existing member references
retain C's separate namespace, allowing a local `parent_card` beside
`instance->parent_card` without capturing ordinary identifiers. All 466 previous
raw/expanded lifetime projections and facts are unchanged by the shared-tool
extension, and exact inverse merging reproduces each new predecessor body.

All 20 complete actual expansions, 17 ASSERT sites, 39 direct native int32 local
references, the build and all 15,169 fixed slots pass. All 26 normalized runtime
sections are identical to the predecessor. Routine setup retains the identical
18,725-entry CSV and mapping results. Placeholder uses fall from 60,112 to 60,033,
with 4,135 LAB gotos unchanged. One expanded statement-expression parser case
remains deferred. No optimized-variable identity, new function coverage or
gameplay validation is claimed. Earlier native semantic audits were not rerun.
See [getter/field lifetime coverage](recovery-evidence/getter-lifetime-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c227.json).

The induction-lifetime pass separates 391 counter value groups into 315 native
bindings across 294 functions (242 card handlers and 52 helpers) in 87 files. It
renames 3,214 placeholder uses, reuses 20 old declarations and adds 295 ordinary
int declarations. Initialization, unit updates and loop comparisons support the
counter roles; player-argument uses support player-index names. Shared reads and
read/modify/write operations retain one scalar component, while opaque writes,
uninitialized paths and address escapes remain excluded. Exact inverse merging
reproduces each predecessor body. All 518 previous scalar/getter raw/expanded
projections and facts are unchanged by the shared-tool extension.

All 87 complete actual expansions, 36 ASSERT sites, 372 direct native int32 local
references, the build and all 15,169 fixed slots pass. Twenty-four normalized
runtime sections are unchanged; text and read-only data differences are recorded
without a byte-matching requirement. Routine setup retains the identical
18,725-entry CSV and mapping results. Placeholder uses fall from 60,033 to 56,819;
4,135 LAB gotos remain unchanged. One expanded statement-expression parser case
remains deferred. No optimized-variable identity, new function coverage or
gameplay validation is claimed.
See [counter-lifetime coverage](recovery-evidence/loop-lifetime-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c236.json).

Obsolete artifact replay is now retired at the user's request. The cleanup removes
2,224 superseded build/debug images, old compiler replay outputs, stale comparison
reports and obsolete progress-document copies. Release/analysis inputs, Ghidra
projects, raw decompilations, current build objects, implementation source and
source backups remain. The current batch's inputs and focused verification also
remain available. Full historical replay now reports its retirement explicitly;
current setup and builds continue to verify current inputs. Builds do not recreate
the retired initial binary backup. See the
[retirement record](recovery-evidence/historical-artifact-retirement.json).

The player-selection lifetime pass separates 120 player-valued components into
97 native bindings across 97 functions (62 card handlers and 35 helpers) in 53
files. It renames 740 placeholder uses, reuses 16 old declarations and adds 81
ordinary int declarations. Current-turn/player values, opposing-player transforms,
player fields and ownership lookups establish the value domain; explicit player
arguments or per-player array indices support the names. Toggles retain the same
scalar component, and later helper results remain separate. Four unsafe cases
are rejected; literal player one is not automatically called the opponent.
Exact inverse merging reproduces every predecessor body.

All 53 complete actual expansions, 12 ASSERT sites, 106 direct native int32 local
references, the build and all 15,169 fixed slots pass. Twenty-five normalized
runtime sections, including executable code, are identical; read-only data
differences are recorded. Routine setup retains the identical 18,725-entry CSV
and mapping results. Placeholder uses fall from 56,819 to 56,079; 4,135 LAB gotos
remain unchanged. No optimized-variable identity, new function coverage or
gameplay validation is claimed. Retired historical compiler/runtime replay was
not invoked.
See [player-selection coverage](recovery-evidence/player-lifetime-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c244.json).

The call-argument cleanup inlines 358 adjacent call results across 276 functions
(236 card handlers and 40 helpers) in 84 files. It removes 75 unused ordinary int
declarations and 440 source lines. Every result has one direct consumer argument
and no later live value. Actual GCC probes prove direct consumer function types,
nonvolatile unescaped integral sibling locals, stable local addresses and
compile-time constants. Mutable global siblings and side-effecting arguments
remain separate. All accepted producers return int, so no extra casts are needed.
Nested call chains retain every call and original argument value.

Raw and expanded CFG contraction checks cover 32,266 paired states. All 84
complete actual expansions, 30 ASSERT sites, five unsafe source controls, four
actual GCC controls, the build and all 15,169 fixed slots pass. Routine reccmp
setup preserves the identical 18,725-entry CSV and mapping results. Placeholder
uses fall from 56,079 to 55,288; 4,135 LAB gotos remain. The staged index is
unchanged. Superseded screening inputs (245 files, 375,139,717 bytes) were deleted;
accepted probes, native inputs, source backups and compact receipts remain.
Full historical replay remains retired. No new native function coverage or
gameplay validation is claimed.
See [call-argument coverage](recovery-evidence/call-argument-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c254.json).

The card-instance pointer pass separates 590 value lifetimes into 223 native
bindings across 216 functions (138 card handlers and 78 helpers) in 78 files.
It renames 2,994 placeholder references, reuses 183 old declarations and adds 40
typed pointer declarations. Direct native instance/parent names are selected by
their lookup roles and checked against 332 non-inline release DWARF references.
The source retains exact card_instance_t pointer types and all original pointer
assignments, including later null values. Seven unsafe source cases are rejected;
exact inverse merging reproduces every predecessor byte. Native optimized-local
identity is not assumed.

All 78 complete actual expansions and pointer-type probe units, 25 ASSERT sites,
the build and all 15,169 fixed slots pass. The type probes and native records
confirm 32-bit pointers to the 300-byte card-instance record. Routine reccmp
setup retains 18,725 CSV entries and all mapping identities; 0 rebuilt
public-entry address annotations change. Placeholder uses fall from 55,288 to
52,294; 4,135 LAB gotos remain. The staged index is unchanged. Historical replay
remains retired; no new native function coverage or gameplay validation is claimed.
See [pointer-lifetime coverage](recovery-evidence/instance-lifetime-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c266.json).

The special-keyword lookup is reconstructed as a readable card-ID switch covering
1,266 cards, 16 keyword bits and 47 keyword combinations. This is one existing
helper, has_sp_key_by_csvid_impl, rather than 1,266 new handlers. Its former pure
decision tree had 1,164 gotos, including 1,098 LAB jumps, and 215 labels. All are
removed, along with 2,979 source lines and three placeholder locals. The switch
uses checked CARD_ID and SP_KEYWORD names and retains arbitrary requested-mask
combinations through one bitwise intersection per matching card.

An acyclic integer interpreter builds canonical reduced ordered BDDs for both
32-bit inputs. It proves every output bit equals its requested bit intersected
with finite card membership, with no interactions between requested bits. All
3,016 reachable CFG nodes are covered; local definedness, shift counts and
signed arithmetic are checked. The original release machine function has no
calls or non-stack accesses other than seven read-only jump tables. A separate
32-bit checker maps that image without running its DLL entry point and compares
the native function, predecessor C and new switch on 9,231,550 input pairs. It
exhausts card IDs -65,536 through 65,535 for 70 request masks, adds signed32
boundaries and 50,000 full-width random pairs. Complete native32 input space and
gameplay are not exhaustively checked.

The complete actual compiler expansion, 1,282 enumerator bindings, integer ABI,
five unsafe-model controls, the build and all 15,169 fixed slots pass. Reccmp
retains all 18,725 mappings; only eight rebuilt public-entry address annotations
change. LAB jumps fall from 4,135 to 3,037 and placeholder uses from 52,294 to
52,062. The staged index is untouched, and historical replay remains retired.
See [keyword-lookup coverage](recovery-evidence/keyword-lookup-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c276.json).

The ability-query pass reconstructs seven helpers. has_an_activated_ability now
uses a named 112-card exclusion switch and named type/ability flags; Treasonous
Ogre retains its special mana-source result. Dependency analysis shows that all
132 ID-tree conditions observe only the low 16 bits of the former wide load.
Every uint16 ID is classified. The normal typed ID field replaces the aliased
32-bit load, and the bit-test tree's signed-shift idioms disappear. Adjacent
humiliation, activation, mana-production and special-keyword queries regain
structured flags and local names. Negative mode sentinels, AI-only mana
suppression, vigilance persistence and keyword suppression/indestructibility
ordering are preserved.

The comparison process rehosts all seven original branch bodies with five
explicit data-address relocations and three shared opaque callees. It compares
native, predecessor C and reconstruction on 33,213,200 cases (99,639,600 helper
invocations), checking results, callback arguments/order and all instance and
extension bytes. The classifier covers all 65,536 card IDs across all 256
relevant type/extra-ability/rules-result combinations, with poisoned upper ID
words and irrelevant bits. The small queries add 16,435,984 state/mode cases,
including negative/signed boundaries and sampled full-width state. This does
not exhaust every 32-bit query state or validate actual gameplay.

All 131 actual-unit enumerator/layout assertions, complete compiler expansion,
the build and all 15,169 fixed slots pass. Reccmp retains 18,725 mappings; only
eight rebuilt public-entry annotations change. The batch removes 516 lines,
203 placeholder uses and 16 placeholder names. Current uses fall to 51,859;
3,037 LAB gotos remain. The index is untouched and historical replay stays
retired. Superseded failed harness outputs were deleted; successful comparison
sources, binaries, logs and compact records remain.
See [ability-query coverage](recovery-evidence/ability-query-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c288.json).

The keyword-dispatch pass restructures special_abilities into a linear handler
sequence and cleans six adjacent workers: deathtouch, hexproof, vigilance,
indestructibility, must-attack and must-block handling. It removes 34 LAB gotos,
166 lines, 115 placeholder references and 13 placeholder names across seven
functions. Fresh affected-card/controller tests stay between handlers. The
initial instance pointer remains available for the unconditional shadow-hoser
update; later instance assignments, repeated getter calls and handler order are
preserved. Worker masks use the existing named ability, state and player flags.

A symbolic next-action comparison covers all 32 requested keyword bits and every
event category used by this routine. Its 47 product states preserve calls,
pointer-result stores, record updates and returns. Engine card/controller/turn/
forbid facts refresh arbitrarily after every matched call or record update;
private flag stores are silent and cannot capture those mutable facts. Raw and
actual expanded proofs match. A native isolated-dispatch comparison and a
native-dispatch-plus-six-real-workers comparison each pass 7,768,000 cases.
The composed comparison retains original private worker and humiliation-reader
bodies, binds 42 data operands to test globals, and supplies five controlled
external callees. Its callbacks mutate engine facts, player bits, ability-loss
modes, record state, keyword words and extension pointers. Complete call traces,
three instance/extension pairs and seven final engine words match. Full native32
state space and gameplay are not exhausted.

Actual enum/layout and ordinary-storage type probes, complete compiler expansion,
the build and all 15,169 fixed slots pass. Reccmp keeps all 18,725 mappings;
eight rebuilt public-entry annotations change. LAB gotos fall from 3,037 to
3,003; placeholder uses fall from 51,859 to 51,744. Two accumulated legacy audit
context chains now use ExitStack to avoid Python's static block-depth limit;
full historical replay remains retired and was not invoked. Superseded harness
outputs were pruned. The staged index is untouched; no new native function
coverage is claimed.
See [keyword-dispatch coverage](recovery-evidence/keyword-dispatch-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c299.json).

Obsolete artifact cleanup removes another 166 files: superseded full-ledger
rollback/candidate JSON files and completed-batch rebuilt DLL/debug snapshots.
They occupied 8.58 GB logically and freed 3.24 GB on disk. Current ledgers,
native release inputs, current build outputs, source snapshots, Ghidra projects
and reusable recovery tools remain. Historical compiler/runtime replay was
already retired; older receipts can still name deleted historical artifacts.
Protected file hashes and the staged index are unchanged. See the
[deletion record](recovery-evidence/obsolete-ledger-artifact-pruning.json).

The land-allowance scan, total_playable_lands, now uses one descending card loop
for both controller passes and named card IDs for permanent and temporary
bonuses. It reloads current_turn before choosing the second controller, retains
effect/humiliation short-circuiting, re-fetches custom legacy instances and
checks their callback identity. Storm Cauldron, Rites of Flourishing and
Ghirapur Orrery grant their bonus regardless of controller. The recount flag is
cleared only when the total remains one. This removes 30 gotos (20 LAB jumps),
147 source lines, 90 placeholder uses and five placeholder names.

The original release machine function, predecessor C and reconstruction agree
on 3,031,440 cases (9,094,320 invocations): every uint16 card ID with controller,
visibility, effect and humiliation variants; every uint16 legacy picture with
controller and callback/legacy-identity variants; signed32 random IDs; and
100,000 synthetic boards under four callback mutation modes. Results, exact
call traces, all instance bytes, predicates, RNG and relevant globals match.
Controlled engine callees can change current turn, counts, player bits, IDs,
legacy identity and record fields between calls. Players are valid and counts
stay within allocated test slots; full native32 state and gameplay are not
exhausted. The native branch body uses nine data-operand bindings and two
callback-identity bindings; no DLL initialization is run.

Actual compiler-unit expansion, storage/enum/layout probes, source succession,
the build and 15,169 fixed entries pass. Reccmp retains 18,725 mappings; only
eight public COFF address annotations change. The current tracked inventory has
2,983 LAB jumps and 51,654 placeholder uses. The staged index is unchanged.
See [land-allowance coverage](recovery-evidence/land-allowance-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c306.json).

Both mana-color helpers now use named card-ID switches in place of compiler
decision trees. get_color_of_mana_produced_by_id retains structured land scans
and Mox Amber's legendary-creature/planeswalker scan, including subtype filters,
recursive-producer exclusions, early color saturation and artifact-to-colorless
normalization. get_colors_of_mana_land_could_produce_ignoring_costs reads the
typed uint16 card ID instead of copying the adjacent word. Conservative bit
dependency support proves every wide-word use observes only bits 0..15.
Transmutation dispatch preserves cached versus freshly re-read colors and
callback-sensitive card-data fields. The batch removes 62 gotos (52 LAB jumps)
and 618 source lines across the two existing helpers.

Original primary, cold and land-query machine bodies run in composition in a
32-bit comparison process. Native, predecessor and reconstruction agree on
9,410,128 cases (28,230,384 outer helper entries): every uint16 ID for both
helpers under recorded configurations, 134 relevant IDs under 64 configurations
and ten info-slot boundary values for both players, 100,000 synthetic boards
under four callback mutation modes, and 50,000 full-width primary IDs. Exact
call traces, all record bytes, predicates, RNG and relevant globals match.
Twenty-nine data operands are rebound to controlled storage; six original
jump tables and the native primary-to-land helper call remain. Engine callees
use shared controlled stubs. Full native32 state/input space and gameplay are
not exhausted; no new native function coverage is claimed.

The actual complete compiler unit, ordinary-storage/layout probes, 151 enum
bindings, source succession, build and all 15,169 fixed entries pass. Reccmp
retains 18,725 mappings; only five public COFF address annotations change.
The current tracked inventory has 2,931 LAB jumps and 51,654 placeholder uses.
The staged index is unchanged. See
[mana-color coverage](recovery-evidence/mana-color-cleanup-coverage.json) and
[current inventory](recovery-evidence/readability-source-snapshot-c312.json).

The tapped-mana batch reconstructs produce_mana_tapped_impl,
produce_mana_tapped_multi_impl, produce_mana_tapped_all_different_colors_impl
and choose_mana_all_one_color. Structured branches and loops replace 36 gotos
(33 LAB jumps), 304 source lines, 198 placeholder uses and 17 placeholder names.
Named state/contamination/Deep Water/Quarum flags and payment roles replace raw
flags and compiler temporaries. Nonnegative quantity clamps replace signed
bit tricks. A single colored payment need still bypasses prompting. The
multi-color double override mapping, white/colorless bookkeeping, callback
order, fresh post-production reads, exact cancellation tests and unexpected
dialog-result paths remain as in the native routines. Dialog implementations
remain outside this reconstruction; their existing source calls are retained.

Original native branch bodies, predecessor and reconstruction agree on
2,570,688 cases (7,712,064 entries). Coverage includes all 128 low-seven-bit
availability masks, nine default masks, five chooser quantities, 128 payment
configurations, 729 six-color amount vectors, single/different-color amounts,
negative card sentinels and 50,000 random inputs under three mutation modes.
Shared controlled callees can mutate mana globals, report arrays, instances,
payment pointers and retained chooser buffers. Exact opaque-call traces and
all relevant external state match. The original private different-color entry
uses EAX/EDX/ECX for its first three arguments; its private chooser callback
uses an explicit regparm(3) bridge. Native rehosting binds 114 data operands,
one data-pointer literal and 65 external branch operands. Quantities and array
indices stay within the recorded valid domain; signed-overflow states, full
native32 state space and actual gameplay are not exhausted.

The actual complete compiler unit, ordinary-storage/array/layout probes,
17 enum bindings, 14 source succession links, build and all 15,169 fixed entries
pass. Reccmp retains all 18,725 mappings; only rebuilt public COFF address
annotations change. The tracked inventory now has 2,898 LAB jumps and 51,456
placeholder uses. The staged index is unchanged. See
[tapped-mana coverage](recovery-evidence/tapped-mana-cleanup-coverage.json) and
[current inventory](recovery-evidence/readability-source-snapshot-c318.json).

Six remaining mana helpers are cleaned up. produce_mana_tapped2_impl,
produce_mana_tapped_all_one_color_with_default_impl and
produce_mana_tapped_any_combination_of_colors_impl regain structured branches
and loops. The default-color helper keeps its second transmutation decode after
production, raw quantity reporting, devotion query, cancellation and unexpected
dialog-result paths. AI text uses the corresponding row of the existing
249-by-300 text table instead of pointer arithmetic beyond its first row.
Producer/multiplier locals and literal constants receive checked role/enum
spellings without changing their expressions, repeated queries or control flow.
The pure basic-subtype query becomes a five-case switch. The batch removes nine
LAB jumps, 122 lines, 149 placeholder uses and 16 placeholder names.

Three original native branch bodies, predecessor and reconstruction agree on
2,137,664 cases (6,412,992 entries). The configurations include both players,
negative card sentinels, all seven-bit availability masks, all two-color pairs,
16 amount pairs, cancellation/devotion/override cases and 40,000 random inputs
under three callback mutation modes. Exact calls, relevant globals, all instance
bytes, retained chooser-buffer mutations and seven AI text rows match. Rehosting
binds 86 data operands, one data-pointer literal and 50 external branch operands.
Native entries retain cdecl; the private chooser callback has its register ABI
bridge. Two other helper edits are exact inverse local/constant spelling
projections. The subtype helper's pure source CFGs agree over five singleton
subtypes and their complement, covering all 32-bit input patterns. These are
distinct verification scopes. Engine/dialog/text callees remain controlled
comparison stubs; no real UI, signed-overflow/out-of-bounds states, complete
native32 state space or gameplay is claimed.

The complete actual compiler unit, ordinary-storage/array/layout probes,
27 enum bindings, 12 source succession links, build and all 15,169 fixed entries
pass. Reccmp retains all 18,725 mappings; only rebuilt public COFF annotations
change. The entire produce_mana.c source now has zero LAB jumps and zero known
Ghidra placeholder identifiers. The tracked tree has 2,889 LAB jumps and 51,307
placeholder uses. The staged index is unchanged. See
[mana-helper coverage](recovery-evidence/mana-tail-cleanup-coverage.json) and
[current inventory](recovery-evidence/readability-source-snapshot-c324.json).

The counter-core batch reconstructs add_or_remove_counters_impl,
get_updated_counters_number_impl, copy_counters and increment_all_counters,
and gives clear_all_counters a checked local/status spelling. Seven assignable
byte slots and two dedicated +/-1 slots retain their native matching/allocation
order, capacity clamping, overflow promotion and power/toughness updates. Zero
adjustments still notify; replacement and addition triggers keep their distinct
save/restore behavior. Copy and increment share one pure snapshot utility that
reads ordinary record bytes into caller-local entries. Modifier cards use named
cases while preserving repeated count reads, short-circuit callback order and
the native target/controller quirks. This removes 37 gotos (36 LAB jumps),
469 lines within the five existing functions and 307 placeholder uses. Including
the new utility, the whole file shrinks by 442 lines.

Five original native branch bodies, predecessor and reconstruction agree on
8,938,472 cases (26,815,416 outer entries). Raw copies call the real native
updater in composition. The configurations cover every byte quantity, 512
counter-type values, signed32 amount boundaries, all uint16 modifier IDs,
activation-parent paths, player counters and 15,000 random inputs under three
callback mutation modes. Exact external-call traces, transient counter context,
all instance/card-data/player-counter bytes and relevant globals match. Original
COFF names independently bind the four counter globals. Rehosting uses 37 data
and 28 callee operands, retains the stat-dispatch jump table and explicitly
bridges private register ABIs. Modifier arithmetic uses bounded quantities;
invalid allocations, complete native32 state space and gameplay are not exhausted.

The complete actual compiler unit, ordinary-storage/byte-view/layout probes,
41 enum bindings, nine source succession links, build and all 15,169 fixed
entries pass. Reccmp retains all 18,725 mappings; only rebuilt public COFF
annotations change. The tracked tree has 2,853 LAB jumps and exactly 51,000
placeholder uses. The counter unit has 15 LAB jumps and 287 placeholder uses
remaining, principally in Quarry Hauler's chooser. The new snapshot utility is
a C refactoring helper, not additional native coverage. The staged index is
unchanged. See [counter-core coverage](recovery-evidence/counter-core-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c330.json).

The counter-tail batch reconstructs quarry_hauler_helper, remove_counters_impl
and move_counters with ordinary loops and named card/target constants. Quarry
retains slot order and the writes made for occupied invalid slots, builds the
nine-option dialog with the existing opaque DIALOG interface, and performs
chosen additions/removals in slot order. Native assembly at 0x026d5367 proves
the old recovered formatter arguments were wrong: destination, counter type
and 256 are passed to the private regparm(3) formatter, rather than destination
and target coordinates. The original C error is rejected by a negative control.

Native Quarry also reuses the name-loop index for AI priorities, then leaves
that index at two. At nine it reads the adjacent choices[0] stack word; shorter
lists can read an indeterminate entry unless a trailing occupied invalid slot
wrote it. The reconstruction models the defined choices[0] case explicitly
and documents the remaining native indeterminacy. It does not substitute the
user-selected index. Dialog implementation remains opaque and out of scope.
Suspend removal keeps the original instance pointer across activation-parent
redirection, refreshes its suspended ID between independent cases, and retains
target callbacks even for zero/negative removal counts where native does so.
Counter moves preserve Melira's fallthrough, repeated copy/clear operations,
fresh silence reads and zero-quantity predoubled calls.

The three original native branch bodies and reconstructed/reference bodies
agree on 552,736 comparisons (1,658,208 outer entries). Quarry's reference
explicitly corrects the formatter call and defines the adjacent index-nine
word; the actual predecessor is separately retained. Cases include every
human slot mask, defined AI priority layouts, all suspend card cases, callback
mutations, exact full record/global state and the visible dialog protocol.
Dependencies are controlled callees; no actual UI or gameplay runs, native
indeterminate-state equivalence or complete native32 coverage are claimed.

This removes 15 LAB jumps, 284 lines and 287 placeholder references. The entire
counters.c unit now has zero LAB jumps and zero known Ghidra locals. Full-unit
compiler expansion, ordinary-storage/layout/enum probes, five source succession
links, build and all 15,169 fixed entries pass. Reccmp keeps all 18,725 mappings;
only rebuilt public COFF annotations change. The tracked tree has 2,838 LAB
jumps and 50,713 placeholder uses. The staged index is unchanged. See
[counter-tail coverage](recovery-evidence/counter-tail-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c338.json).

The life-helper batch reconstructs life_total_can_change, can_lose_the_game_impl,
gain_life_impl, trigger_gain_life, cannot_lose_the_game_impl and lose_life.
Named card and trigger cases replace decompiler comparisons, and ordinary
loops replace the intertwined battlefield scans. Life loss snapshots its
simultaneous sources before resolution, retains live count reads while
resolving them, and keeps distinct owner/opponent trigger passes. Gonti's
Machinations still reacquires its instance after adding energy. Both-player
loss reads the current turn again after the first player's callbacks.

Life gain preserves context push/pop order, save/restore boundaries, negative
replacement into life loss, fresh trigger-enable checks and deferred loss
following draws. Loss prevention keeps continuous hidden life/poison debt,
the marker set while leaving play, callback-sensitive old-controller reads,
byte poison arithmetic and independent empty-library draw suppression. The
2020 source supplies structural context; the 2022 branch bodies determine
these reconstructions, including newer sources and humiliation checks.

Six native branch bodies, predecessor and reconstruction agree on 1,147,840
comparisons (3,443,520 outer entries), covering all uint16 loss-guard IDs,
all byte poison quantities, both-player loss with real native recursion,
18,000 random inputs and two callback-mutation modes. Exact external calls,
transient contexts, full instance/card-data/player-counter/deck state and
relevant globals match. Private register entries and the engine's two context
functions are explicitly bridged; COFF symbols bind the named globals.
Callees other than native self-recursion are controlled, not composed engine
execution. Arithmetic and allocated players/cards are bounded to valid game
states; full native32 coverage and gameplay are not claimed.

This removes 11 LAB jumps, 176 lines and 144 placeholder references. Full-unit
compiler expansion, ordinary-storage/layout/enum probes, ten source succession
links, build and all 15,169 fixed entries pass. Reccmp retains all 18,725 mappings;
only rebuilt public COFF annotations change. The tracked tree has 2,827 LAB
jumps and 50,569 placeholder references. The staged index is unchanged. See
[life-helper coverage](recovery-evidence/life-helpers-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c344.json).

The casting-cost batch reconstructs true_get_updated_casting_cost as 100 named
card cases and ordinary battlefield loops. It restores source-level validation,
explicit uint32 cost arithmetic, the cached card-definition pointer, fresh
metadata reads where native uses them, Trinisphere's final adjustment and
separate callback-context save/restore. The order-sensitive checked-player flag
still controls generic callbacks across both controllers. The 2020 switch
provides structure, while the 2022 branch body establishes the Aether Storm,
Monument, Hamza, Spectacle Mage and Myth Unbound cases missing from that switch.
Commander tax retains both counter reads and Myth Unbound's rounded half-count.
Pure infinite_casting_cost calls use the separately pinned six-byte writer.

The native 7,139-byte body, predecessor and reconstruction agree on 905,664
comparisons, including every uint16 modifier ID, 30,000 random inputs and two
callback-mutation modes. Another 1,536 comparisons cover 512 mixed-controller
layouts under three mutation modes, and 128 native/reconstruction comparisons
exercise INT_MIN and INT_MIN+1 original costs without evaluating the
predecessor's potentially overflowing signed Trinisphere subtraction. Exact
calls, assertion diagnostics, complete record/card-definition state, cached
pointer selection, mutable cost bytes and relevant globals match. Engine/card
callees remain controlled; native32 state and gameplay are not exhausted.

This removes 84 gotos (83 LAB jumps), 184 lines and 605 placeholder references.
Full-unit compiler expansion, ordinary-storage/layout/enum probes, two source
succession links, build and all 15,169 fixed entries pass. Reccmp keeps all
18,725 mappings; only rebuilt public COFF annotations change. The tracked tree
now has 2,744 LAB jumps and 49,964 placeholder references. The staged index is
unchanged. See
[casting-cost coverage](recovery-evidence/casting-cost-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c350.json).

The payment-helper batch reconstructs charge_mana_from_id_with_cost_mod,
charge_mana_for_activated_ability, has_mana_for_activated_ability,
attachment_granting_activated_ability and equipment_granting_activated_ability.
Ordinary branches replace payment labels, and named costs, recipient coordinates
and results replace decompiler temporaries. Reaper King's temporary path stays
separate from invisible temporary cards used for hybrid/Phyrexian payments;
each cleanup still returns the saved payment result. Fixed and X costs keep
their two charge passes, exact cancellation test, artifact channel selection
and untouched payment slots where native leaves them alone.

Activated payment/query callers reread the extension after cost modification
before converting colors for Celestial Dawn. Attachment targeting refreshes
saved recipient coordinates after protection lookup, and equipment preserves
its event-specific availability checks, fallback and resolution behavior.
The existing DIALOG boundary remains opaque; no dialog implementation was
recovered or executed.

Five native branch bodies, predecessor and reconstruction agree on 165,568
comparisons (496,704 outer entries), including all 32 activated color masks,
256 attachment/equipment event values, 12,000 random inputs and two callback
mutation modes. Exact calls and transient payment vectors, instance/extension
bytes, target definitions, cached card pointers and relevant globals match.
Other engine/helpers are controlled callees, not composed gameplay execution;
allocated players/cards and scalar color arithmetic are bounded, and arbitrary
pointer aliases, signed overflow and complete native32 state are excluded.

This removes six gotos (five LAB jumps), 188 lines and 152 placeholder references.
Full-unit compiler expansion, ordinary-storage/layout/enum probes, 12 source
succession links, build and all 15,169 fixed entries pass. One older local-name
receipt is linked through its exact token-only projector before following the
later semantic successors; historical binaries are not replayed. Reccmp keeps
all 18,725 mappings; only rebuilt public COFF annotations change. The tracked
tree has 2,739 LAB jumps and 49,812 placeholder references. The staged index is
unchanged through batch publication. See
[payment-helper coverage](recovery-evidence/payment-helpers-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c357.json).

Before committing the accumulated recovery work, the canonicalization ledger
was changed from JSON indent=2 to indent=0. Parsed data and field/array order
are identical, and indent=2 reproduces the original bytes exactly. The ledger
shrinks from 124,145,846 to 88,677,636 bytes. Subsequent batch writers should use
indent=0 to keep the ledger below the remote's large-file limit. See the
[format-only compaction receipt](recovery-evidence/ledger-format-compaction.json).
