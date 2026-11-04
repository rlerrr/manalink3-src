# Recovery scope

The goal remains best-effort recovery of October 2022 card handlers and their
helpers into their original source units. Native linking, initializer coverage,
and downstream gameplay verification have separate evidence.

The C751 follow-up in the same receipt restores 535 loose comments across 65
source files to their original neighboring definitions, including skipped-card
notes and color/section headings. The reorderer now treats these as placement
items instead of collecting them in the file header. Original file headers and
forward-declaration documentation stay put. Complete code token streams,
function order and conditional guards match C750, and the repair is idempotent;
C750 remains the last compiled/debug artifact receipt.

The [C750 source-layout cleanup](recovery-evidence/source-layout-restoration.json.gz)
restores relative function order in 136 source units and adds or repositions
5,600 card descriptions. Existing body comments, standalone card slots and
historical source take precedence over the pinned 2026-11-04 Scryfall Oracle
fallback. Current Oracle wording documents the cards; it does not alter their
implementation. The local bulk cache is excluded from Git.

Original 2022 line annotations are ordering anchors, not exact line targets.
File-local data, types, macros and dependencies constrain some placement; 72
functions use explicitly recorded baseline/neighbor interpolation. The pass
removes 1,898 excessive top-level blank-line runs, retains spacing inside
function bodies and removes 16 orphan markers from empty conditional wrappers.
Generic token, animated-form and engine callbacks receive no guessed printed
card text; their remaining identities are listed in the receipt.

All changed function tokens, literals and enclosing conditional branches match
commit `1864623`; file-scope tokens match except eight forward declarations.
The grouped GCC 9.3 build passes all 15,169 fixed jump slots and the current
source/name pin check. Historical comment-heading variants are preserved without
new duplicate descriptions. This is organization/documentation work: no new
native semantic review, gameplay test or full reccmp score run was needed.
C751 is the latest source-layout receipt; C750 is the latest build receipt.
C736/C735 remain the full-score snapshot.
Older sections below describe their own batch's build and comparison state.

The [C742 constant/event review](recovery-evidence/reccmp-constant-and-shared-event-review.json.gz)
checks 50 small handlers/helpers selected from unmatched instruction constants,
skipping earlier queue hints. Native/current event sets and inspected arguments
agree: most leads are aura or double-faced-card dispatch folding. Native public
and private experience-counter entries confirm that the source's positive-count
guard remains required. No game-source edit or rebuild is indicated. Scoped
cursors end in `progress-c742.json`: 301 priority observations and 518 named-call
observations; these are not full-function equivalence or missing-body counts.
C740 remains the latest source/build receipt and C736/C735 the full-score snapshot.

The [C741 shared-dispatch review](recovery-evidence/native-spell-hand-and-graveyard-dispatch-review.json.gz)
inspects generic spell handling, hand/graveyard/exile activations, static
graveyard abilities and Glissa's autotap logic: 2,089 source lines across four
large bodies. A fifth scoped review covers the tutor's reveal-call lead.
Native instructions confirm Ghidra-omitted test-field stores and sprintf
arguments; apparent missing calls come from shared/duplicated compiler tails.
No game-source change or rebuild is indicated. These are scoped observations,
not full-function or gameplay proofs. C740 remains the latest source/build
receipt; named-call cursors end in `progress-c741.json` with 518 observations
and 474 pending leads. Pending leads are not missing functions.

The [C740 interval and shared-caller batch](recovery-evidence/native-attacker-slot-wrap-and-interval-review.json.gz)
adds opt-in 32-bit field checks and callback-address normalization to
`tools/recovery/rank_interval_leads.py`. On 5,673 selected non-exact functions,
170 raw interval leads reduce to 26 after 144 public callback-address-only
leads are filtered by unique function identity. Eighteen residual dispositions
retain their preceding source hashes; eight new interval-site reviews include
two additional private callback relocations. Another 27 small-caller/shared-cost
reviews bring this batch to 35 new scoped manual observations. These counts
are review scope, not missing bodies or full-function equivalence proofs.

Four attacker-list handlers now perform their initial slot subtraction in
`uint32_t`, then convert to `int32_t` before the sign check: Caught in the
Brights, Lost in the Woods, Marchesa and Stonehoof Chieftain. This preserves
the native wrapping subtraction instead of allowing signed-overflow
simplification. The differing input was INT32_MIN, outside ordinary attacker
counts; this is a fidelity correction, not a newly fixed normal-gameplay case.
One grouped build passes all 15,169 fixed slots; all four selected rebuilt
partitions now agree with native instructions. The scanner passes 32 focused
checks. Full comparison scores remain the historical C736/C735 snapshot.
The scoped queue cursors now end in `progress-c740.json`. Staging is preserved.

The preceding [C739 call-argument and mana review](recovery-evidence/native-call-argument-and-mana-review.json.gz)
records 17 scoped observations without source changes. The [C738 interval
triage](recovery-evidence/reccmp-interval-triage.json.gz) preserves the initial
byte/word/return scanner results. The [C737 shared-helper fix](recovery-evidence/native-death-trigger-kill-range.json)
restores the native death-counter kill-code interval to 1..3. Their own
evidence and build receipts remain historical.

The preceding [C735 batch](recovery-evidence/native-caller-and-operand-triage.json)
reviewed 67 candidates and corrected both Kiora tutor-success predicates; Kiora
is exact in the subsequent C736 report. Earlier [C734](recovery-evidence/native-imprint-and-shared-query-flow.json),
[C733](recovery-evidence/native-attacker-and-instance-query-placement.json),
[C732](recovery-evidence/native-aura-callback-snapshots.json) and
[C731](recovery-evidence/native-tutor-predicate-and-scan-roles.json) retain their
own build and selected-site evidence. C736 comparison scores describe the C735
build and are historical after the C737 guard correction.

The preceding source work is the [C730 batch](recovery-evidence/native-query-placement-and-structured-continuations.json):
119 new scoped candidate reviews, 14 refined definitions across 13 source units.
Brutal Hordechief, Star Compass and Elite Scaleguard restore native instance-query
placement. Unsubstantiate and the scry-like helper make native evaluation order
explicit; their previous GCC output already used that order. Nine other handlers
replace duplicated dispatch/returns or decompiler loops with structured source.
One grouped build passes all 15,169 fixed entry slots. Selected instruction sites
confirm the query changes; per-function strict CFG checks are deferred. The saved
call-lead cursor now has 352 scoped observations/corrections and 640 pending.
These are prioritization counts, not missing bodies or full-function closures.

The latest [C736 full comparison](recovery-evidence/reccmp-behavior-priority-refresh.json.gz)
describes the C735 build: **18,837 ranges: 12,790 exact, 287 effective and 5,760
different**. Since C730, five source batches refine 32 definitions across 25
source units. Twelve ranges become exact and two become effective; thirty
scores/categories change, with no previous exact/effective category regressing.
All fourteen comparator-source hashes are unchanged. The fresh priority packet
`.recovery/reccmp/current-priority-c736.json` has 3,044 candidates in 2,359 textual
groups. It retains all differences while lowering priority for 1,715 unchanged
historical textual deltas and 245 unchanged recent scoped source observations;
1,077 source-owned candidates have neither hint. These are ordering hints with
overlap, not renewed equivalence proofs. No game-source changes, rebuild or
historical semantic-audit replay occurred during the refresh. Gameplay testing
remains downstream. Earlier [C730 comparison](recovery-evidence/reccmp-structured-source-priority-refresh.json.gz)
counts and priorities are historical.

The preceding [C725 full comparison](recovery-evidence/reccmp-indexed-arrays-and-public-call-refresh.json.gz)
of C724 covers **18,837 ranges: 12,768 exact, 284 effective and 5,785 different**.
Categories are unchanged from C721/C720; five different-to-different scores
change, with no added or removed ranges. The intervening batches refine 51
existing definitions across 34 units: 42 exact literal/enum-only changes and
nine native-supported refinements. No new bodies or header/ABI edits. The C724
build passes all 15,169 fixed entry slots; no game source changes or rebuilds
during this refresh. All 14 comparator-source hashes and existing automatic
review models remain unchanged.

Manual renewal retains 1,177 identical-context observations, four through exact
card-ID projections. Two callback/linker relocations have fresh identities; nine
source refinements have fresh strict entry packets and retain their differences.
The optional spell checker examines 1,156 remaining eligible cards and adds two
scoped observations: Traitorous Instinct and Spell Pierce. It binds plain public
C helpers only when pinned native/current entry traces overwrite arithmetic
flags before any use or call, and keeps the existing argument/register/width
and control-flow checks. Return-only, logical-operation, flag-reading and
unknown entries do not receive that additional contract. Callee/data/ordinary
C ABI/memory assumptions remain; no callee-implementation or gameplay proof.

The standard wider queue has **2,607 entries: 1,692 annotated and 915 without
site observations**, down from 917. All **575 tiny candidates** have scoped
observations. The separate 40-instruction-per-side queue has 3,042 entries,
1,721 annotated and 1,321 pending. Thresholds and raw comparison scores are
unchanged. Twelve comparison-mapping follow-ups remain separate. These are
review leads, not missing-source or gameplay-bug counts.

The preceding game-source work is the [C729 batch](recovery-evidence/native-cycling-and-aura-query-flow.json):
74 functions reviewed together, with six corrected handlers across four units.
Gaea's Embrace and Ghostly Touch restore fresh attached-instance queries.
Resounding Scream, Silence and Wave initialize targets on both target-change
events; Wave also queries targets before reading the live disable flag.
Chandra's Revolution restores the release's zero-return CAN_CHANGE_TARGET path,
keeping actual CHANGE_TARGET selection. One incremental build passes all 15,169
fixed slots and four exact source projections. Selected native/rebuilt query,
constructor and call-order sites are recorded; all six strict entry checks still
retain event, operand or branch differences. Actual Merrow instructions preserve
a color-field store omitted by Ghidra, and Hatred already snapshots life before
its attack-power query; both sources stay unchanged. Sixty-eight scoped read-only
notes advance the worklist to 233 observed/corrected packet leads and 759 pending.
These are not full-range closures, bug counts or missing-body counts. C725 remains
a historical C724 comparison; gameplay verification stays downstream.

The preceding game-source work is the [C728 batch](recovery-evidence/native-spell-event-and-convoke-flow.json):
73 functions reviewed together, including 67 call-difference leads and six related
convoke callers. Fifteen definitions across eight source units are refined:
Resounding Thunder restores target setup on both target-change events, Felidar
Umbra queries before activation dispatch/validation, and Stasis Cell reloads its
attached instance. Eight convoke callers use defined unsigned high-bit predicates;
four handlers consolidate duplicated event continuations. One grouped build
passes all 15,169 fixed slots. Felidar Umbra, Endless Obedience and Tin Street
Dodger pass strict caller CFG checks under the existing identity/callee/data
assumptions; other functions retain differences. Selected raw sites confirm the
three event/query changes and all eight native/rebuilt high-bit predicates.
Fifty-eight read-only topology notes and the new worklist cursor avoid repeating
this review: 160 packet leads observed/corrected, 832 pending. These are scoped
observations, not full comparison closures or missing-body counts. C725 remains
the C724 comparison snapshot; gameplay verification stays downstream.

The preceding game-source work is the [C727 batch](recovery-evidence/native-attachment-query-and-argument-snapshots.json):
71 call-difference leads reviewed together, with eight corrections across eight
source units. Time to Feed, Burn Away, Searing Blood and The Brute restore fresh
attachment queries before event filtering; Roots restores its card snapshot then
fresh player query. Traveler's Cloak keeps the pre-validation instance through
the color chooser and still queries afresh for the aura continuation. Constricting
Sliver snapshots its attachment before the AI-mode helper; Creature Bond removes
an extra pre-count getter while retaining a fresh query per damage iteration.
One incremental build passes all 15,169 fixed slots. Four strict CFG checks now
match (the three death legacies and Constricting Sliver); four retain compiler
layout/ordering differences. These remain scoped caller checks, not callee or
gameplay proof. The other 63 leads have read-only topology notes. A compact
worklist cursor records 88 C726/C727 observations/corrections and 904 remaining
leads from the historical C725 call packet; these are not missing-body counts.
No full comparison refresh, staging or publication in this batch.

The preceding game-source work is the [C726 batch](recovery-evidence/native-spell-instance-query-placement.json):
Volcanic Awakening, Tendrils of Agony, Astral Steel, Ground Rift and Bitter Ordeal
restore the native instance query on every accepted GS event, before resolution
and validation. Early storm-query and unaffected-cast guards remain. Lobotomy
re-reads its final target player after enumeration/selection callbacks rather
than using the hand scan's saved target. One grouped build passes all 15,169
entry slots, four exact source projections and selected getter/field checks:
twelve native and twelve compiled getter sites, including the final Lobotomy
getter-to-effect target load. Six strict checks retain event/operation/register
differences. Eleven further leads have scoped read-only notes. Actual random-hand
packet instructions store payload before kind, matching existing source; cached
Ghidra statements reverse those independent stores. No network I/O is executed.
No new bodies or broad gameplay proof. C725 queues remain C724 snapshots and
are now explicitly labeled historical after this build.

The preceding game-source work is the [C724 batch](recovery-evidence/native-type-worker-and-color-query-order.json):
Amaug's private worker uses seven indexed stored-card slots and separate IID,
type-mask, legality, count, selection and dialog roles. CAN-only initialization,
early 99, the TYPE_EFFECT tribal surrogate, live post-dialog mask writes and
fresh final playable-array IID remain. Phyrexian Boon, Serra's Boon and Chandra
Ablaze sequence four source color/sleight pairs explicitly; six native and six
compiled physical sites confirm color-before-sleight ordering. Chandra's parent
fields are read after the color query, and priority legality uses a fresh grave
IID after the callbacks. One grouped build passes all 15,169 entry slots and
four exact source projections. Four strict entry checks retain frame/local or
aura-event-order differences; no new body or gameplay fix is claimed. Nicol
Bolas's fourteen name queries match the existing descending label loop and need
no source edit. C721 remains a C720 comparison snapshot.

The preceding game-source work is the [C723 batch](recovery-evidence/native-indexed-meld-and-cascade-flow.json):
the two large Colfenor implementations use indexed compact/clear/meld rows,
removing flat int-pointer walks across inner arrays. Slot starts zero and one,
16-bit death snapshots, untouched column zero, repeated ID queries and sticky
test exclusions remain distinct. Multi-cascade uses explicit deck-tail and
compaction indices, removing stack-address cancellation and reused scalar roles.
Native instructions confirm the destination is saved before RNG and deck_ptr is
loaded afterward. The unusual reveal stop check, two land queries, 500 limit,
last shift read, unconditional post-play deck count and reverse Aurora scan are
preserved. One grouped build passes all 15,169 entry slots and two exact source
file projections. Three strict comparisons retain register/branch/frame changes;
no new body, native invalid-boundary repair or gameplay proof is claimed. C721
remains a C720 comparison snapshot.

The preceding game-source work is the [C722 batch](recovery-evidence/named-card-ids-and-activation-local-roles.json):
44 existing definitions across 31 units, comprising 42 mechanical-only changes
and two native-supported activation local-role refinements. Sixty-two card-ID
literals use exact existing header names in known CSV-ID arguments or typed
test fields; reversal reproduces every original token. The scanner passes three
positive and 19 negative controls, including declaration-before-use and shadow
cases. Asmoran separates cost predicates, packed Food choices and creature
coordinates; mana battery separates availability, dialog choice, amount and
prompt card ID. Native callback order, cancellation and counter snapshots remain.
One grouped build passes all 15,169 fixed slots and 31 exact file projections.
Two strict entry checks retain frame/register differences. No new body or
behavior fix is claimed. C721 reports and queues remain C720 snapshots and are
explicitly labeled historical after this build.

The preceding game-source work is the [C720 batch](recovery-evidence/fixed-argument-and-selection-loop-review.json):
Descent into Madness, Orcish Librarian and Development/Research have disjoint
selection/count/prompt scopes and restored native local roles. Six numeric card
IDs use their verified header names. Native quirks remain: Descent carries its
reduced amount into the second player, Librarian's eight-card branch makes two
random picks then exiles positions one and zero, and Research retains its second
IID conversion. One grouped build passes all 15,169 fixed entry slots; 4,056
isolated before/after Librarian traces match. These tests use stubs, not the
original DLL or gameplay. Three strict checks retain stack/register differences.
A read-only scan of 5,693 functions produced no conflicting singleton scalar
argument leads under its narrow straight-line public-API model; this does not
cover arbitrary values, joins or complete caller/ABI behavior. No new bodies or
behavioral fixes are claimed. C721 subsequently refreshes this C720 build.

The preceding [C719 batch](recovery-evidence/exclusive-int-predicate-restoration.json):
111 explicit numeric predicates across 31 existing definitions in 24 files.
Private int temporaries used only by adjacent helper/test pairs are removed;
numeric-only comma RHSs, all-use accounting, helper int contracts and supported
macro inventory prevent changing callback/read order or hidden local uses.
Call syntax/order and exact source byte projections are checked. One grouped
incremental build passes all 15,169 fixed entry slots. This is mechanical source
cleanup, not new bodies, native behavior fixes or fresh CFG/gameplay validation.
Seven selected call-surplus observations explain duplicated native branches and
loop entry/latch sites; their scopes remain separate. C718 scores at that checkpoint describe C717; C721 above supersedes that
comparison with the C720 build.

The preceding [C717 batch](recovery-evidence/native-live-fields-and-shared-calls.json):
30 existing definitions across 22 source units plus one shared-helper declaration.
Legion's Initiative restores native conditional card-ID re-queries; Shape of the
Wiitigo restores fresh attachment coordinates after callbacks; Bogbrew Witch
calls the existing deck matcher instead of duplicating its scan. Two mana
helpers receive local-role refinements, and 25 other definitions receive only
mechanical cleanup of exclusive int predicate temporaries. The completed-batch
build passes all 15,169 entry slots. The reused check_deck_for_test helper passes
strict native/current CFG comparison (42 states); five refined handlers retain
instruction differences. Source/header byte projections are verified after an
interrupted application was resumed atomically; the preliminary partial-state
build is not final verification.

The preceding [C716 batch](recovery-evidence/native-protection-trigger-and-network-flow.json)
refined 34 existing definitions, including planar-die query order and structured
Hot Fix flow. [C715](recovery-evidence/native-predicates-and-local-roles.json)
refined 205 definitions, including 166 mechanical-only cleanups. None of these
batches adds new bodies. The C718 report now includes all three accumulated source batches.

All **575 tiny candidates** in 421 groups have scoped observations (364 at the
default score cutoff). The wider queue has **2,607 entries** in 1,938 groups:
**1,530 annotated and 1,077 without site observations** (468 at the default
score cutoff). These are current C718/C717 review leads, not missing-source or
gameplay-bug counts. Queues are `.recovery/reccmp/current-tiny-reviewed-c718.json`
and `.recovery/reccmp/current-wide-reviewed-c718.json`. A separate fresh packet
contains 128 lexical native/source call-surplus leads with cached native output;
inlining, branch duplication and unreachable arms can also create these signals.
Twelve comparison-mapping follow-ups remain separate.

The helper representation scan accounts for 1,609 source-owned private native
entries through emitted/inline code, an exact COFF-only body or an existing
native equivalence. This is representation accounting, not complete semantic
coverage. The historical 240-handler C697 entry-fragment proofs remain separate;
they do not establish full-function equivalence on the latest build. Earlier
coverage categories below retain their historical scope. See [reccmp](reccmp.md)
for current queues and batching cadence.

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

The targeting-core batch reconstructs validate_target_impl, target_match_type,
target_available_impl, get_protections_from_part and has_protection_from_impl.
The validator's layered checks return to readable source order with the
original named failure epilog and enchanted-loop escape, while all LAB labels
are removed. It retains three instance lookups during initial validation,
aura/equipment protection modes, Kaya's player-hexproof exception, the zero
counter-byte meaning all counters, and the 2022 required/illegal state rules.
Two local recovery names describe unconfirmed native flag spellings; their
actual bit tests and callback order are preserved. The stack-restriction bit
still reads the top stack entry before checking stack_size, matching native.

Availability still tests players first, chooses its first controller from the
native preference rule, caches each controller pass's maximum slot limit and
refreshes that limit for the next pass. Damage target/source modes and the
engine's chooser override retain their argument conventions. The type matcher
keeps its cached IID, full legacy-ID word and later fresh reads. Protection
queries retain activation-parent traversal, backup-IID fallback, cached keyword
bits, humiliation checks, local/global dispatch routing and low-24-bit results.

The native validator, predecessor and reconstruction agree on 185,649 cases;
the four supporting native bodies agree on 200,832 more, totaling 386,481
three-way comparisons and 1,159,443 outer entries. Checks cover every uint16
power/toughness requirement and legacy ID, every byte card type, controller
selection, all four damage scan modes, exact diagnostic output, callback order,
full record/extension/stack/global state and mutation cases. Native private
register entries are explicitly bridged. Query/dispatch callees and 33 engine
resource strings are controlled substitutes; actual localized error resources,
complete native32 state, UI and gameplay are not claimed. Activation chains
must be acyclic and stack-restriction inputs must have an allocated nonempty
stack. Protection-parent tests mutate globals without mutating parent topology.

This removes 82 gotos (80 LAB jumps), 409 lines and 383 placeholder references.
Full-unit compiler expansion, ordinary-storage/layout/enum probes, seven source
succession links, build and all 15,169 fixed entries pass. Reccmp retains all
18,725 mappings; only rebuilt public COFF annotations change. The tracked tree
now has 2,659 LAB jumps and 49,429 placeholder references. The ledger retains
compact indent=0 formatting. The staged index is unchanged. See
[targeting-core coverage](recovery-evidence/targeting-core-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c366.json).

The target-selection batch reconstructs eleven remaining selection helpers and
callers: select_target_impl, real_select_target, real_validate_target,
real_target_available, validate_arbitrary_target, validate_target, valid_target,
choose_default_target, mark_up_to_n_targets_noload, can_legally_target and
opponent_is_valid_target. The original source helps recover readable loops,
controller preference handling, cancellation paths and meaningful local names.
All changes remain in functions/targets.c.

Selection keeps raw dialog return values, separate -1/-3 cancellation outcomes,
multiblocker rejection and diagnostic retry behavior. The AI stops card collection
at 58, then appends eligible player entries, preserving native index recording
and callback reads. Default targeting retains both target-file scans, its full
500-entry lookup, player candidates beginning at card -1 and native preference
restoration timing. Marking retains live scan bounds, cleanup of premarked
entries, and cancellation behavior. Spellskite probing and opponent validation
retain their original borrowed-target/state restoration order.

All eleven native bodies, predecessor and reconstruction agree on 67,712 cases
(203,136 outer calls), including 128 explicit 58-card plus two-player candidate
fixtures with an asserted random bound of 60. Callback arguments/order, return
values, fixture output/global/record state, changing scan counts and controller
preferences are checked. Callback-boundary state is summarized with hashes;
final fixture snapshots are compared byte for byte. Native and reconstruction
also agree on borrowed GUI-local observations during cleanup. The predecessor
comparison excludes these local observations because its decompiled cancellation
assignment is synthetic: native writes result registers directly to the caller.
Queries, GUI callbacks and bounded stdio streams are controlled substitutes;
native dialog implementation, invalid file inputs and gameplay remain out of scope.

This removes 16 gotos (11 LAB jumps), 174 lines and 231 placeholder references.
The complete targets.c unit now has zero LAB jumps and zero known placeholder
identifiers. Full-unit compiler projection, storage/layout/enum checks, 21 source
succession links (including an exact earlier array-scope bridge), build and all
15,169 fixed entries pass. Reccmp retains all 18,725 mapping identities; only
rebuilt public COFF annotations change. The tracked census now has 2,648 LAB
jumps and 49,198 placeholder references. The staged index is unchanged. See
[target-selection coverage](recovery-evidence/target-selection-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c372.json).

The tutor-selection batch reconstructs get_average_value_from_zone,
select_card_from_zone, new_select_a_card,
select_multiple_cards_from_graveyard_to_array_part and
select_multiple_cards_from_either_graveyard in functions/tutors.c.
Readable loops replace the decompiled label graph and local names describe
zone counts, AI candidates, controller choices and temporary hidden-card state.

The average-value and card selectors retain their distinct CMC/value modes,
mana affordability checks, random early choice and average-threshold candidate
selection. Human selection retains the direct engine chooser call, cached Done
label and expansion reset. Zone enumeration keeps the 2022 tutoring-denial and
Aven Mindcensor checks, cached zone pointer, live hand scans, minideck count
behavior, hardcoded child test type and original-ID hand lookup. Library-search
permanent side effects still run after an own-library attempt.

Multi-graveyard selection retains the strict greater-than eligibility threshold,
targeted versus untargeted graveyard modes, bounded rechoose behavior, original
source-ID encoding and temporary face-down/restoration sequence. Caller-provided
filters are initialized or rejected exactly as native, including diagnostic text.
The private one-graveyard worker retains descending result sorting and the native
cancellation/count return conventions. No _2022 aliases or local unions are added.

The five native bodies, predecessor and reconstruction agree on 123,136 cases
(369,408 outer calls), including 256 complete 19-target selections. Checks include
callback order and arguments, live record/zone/global/filter mutations, output
arrays, diagnostic exits, sorting calls, AI modes and selection cancellation.
Callback traces use hashes for compound state/arguments; final declared fixture
snapshots are compared byte for byte. Child queries, selection, constructors,
diagnostics and sorting are controlled substitutes. The native stack probe is
retained unmodified; the private selector uses regparm(3), and the permanent
side-effect compiler part uses regparm(1). Bounded zone storage, positive AI VLA
sizes and nonoverflowing arithmetic are required. Negative private-worker target
counts are checked only through a controlled qsort stub, which records the native
unsigned count without attempting an invalid-size sort. Native UI and composed
gameplay remain outside these checks.

This removes 28 LAB jumps, 429 lines and 396 placeholder references. The entire
tutors.c unit now has zero LAB jumps and zero known placeholder identifiers.
Actual compiler-unit projection, layout/storage/enum probes, 11 source succession
links, build and all 15,169 fixed entries pass. Reccmp retains all 18,725 mappings;
the average-value helper now has one rebuilt range and uses an ordinary ledger
mapping. Its 234-byte body matches native exactly except the three verified
external call displacements. Other changed annotations are rebuilt public COFF
addresses. The tracked census now has 2,620 LAB
jumps and 48,802 placeholder references. The staged index is unchanged. See
[tutor-selection coverage](recovery-evidence/tutor-selection-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c378.json).

The aura/attachment batch reconstructs disable_attached, attached_set_pt_part,
aura_impl, aura_pref_controller, end_of_stealing_aura_effect,
generic_animating_aura_impl and
when_enchanted_permanent_dies_return_aura_to_hand in functions/auras.c.
Ten further helpers change only masked function-local identifier spellings:
attached_is_additional_color, attached_is_forced_color, disabling_aura_ai_part,
targeted_aura_impl, aura_with_variable_pt_boost_depending_on_condition,
gain_control_and_attach_as_aura, cannot_be_enchanted_granted, enchant_player,
pump_attached and this_effect_doesnt_remove_this. Their source and actual expanded
token projections agree exactly; compiler macro-column padding is ignored,
while literals and token order are preserved. They claim no additional native
comparison cases. No local unions or _2022 aliases are added.

The aura worker retains event filtering, cast targeting and AI preference bias,
attachment validation, shroud handling, illegal-attachment cleanup and aura-move
controller/type restrictions. Native callback order, cached coordinates and
later fresh record reads remain visible in the source. AI bias keeps native
unsigned wrapping, and selected-card encoding uses an unsigned shift rather
than a signed negative shift. Animation keeps the native strictly positive
base-PT rule, zero meaning no dynamic PT override, CMC-derived -1 values,
16-bit PT writes, cached dynamic type, added subtype behavior and move reset.
Control restoration still scans for another stealing aura before enabling an
older effect and rereads attachment coordinates around control callbacks.
Death handling keeps separate instance lookups around owner/original-IID
queries and creates the same deferred legacy callback.

Seven native bodies, predecessor and reconstruction agree on 265,664 cases
(796,992 outer calls). Both event-entry filters cover all 65,536 uint16 values;
explicit event/configuration crosses and 192 dynamic-type creation cases cover
animation boundaries, zero/-1 PT, type-data/table mutations and CMC callbacks.
Required callback families are asserted to have executed, including type
creation, both status paths, subtype addition, control restoration and legacy
creation. Queries and child target/attachment/control/ability/legacy callbacks
are controlled substitutes; native gameplay and UI composition are not claimed.
Callback-boundary state and compound arguments use hashes; final declared
fixture snapshots are compared byte for byte. Three private native entries and
two child adapters use regparm(3). Storage/layout/enum probes constrain the
ordinary globals, records and dynamic type fields.

This removes 14 LAB jumps, 215 lines and 285 placeholder references. The entire
auras.c unit now has zero LAB jumps and zero known placeholder identifiers.
Full compiler-unit projection, ten expanded local-name projections, 37 source
succession links (four exact earlier rename bridges), build and all 15,169 fixed
entries pass. Reccmp retains all 18,725 mappings; rebuilt public COFF annotations
are refreshed. The tracked census now has 2,606 LAB jumps and 48,517 placeholder
references. The staged index is unchanged. See
[aura/attachment coverage](recovery-evidence/aura-attachment-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c384.json).

The token-generation batch reconstructs copy_all_tokens_both_players,
generate_token, default_token_definition, resolve_token_iid and
get_copyable_special_infos in functions/token_generation.c. Four more helpers
change only masked local names: set_token, copy_token_definition,
generate_token_by_id_impl and copy_token_characteristics_for_clone. Those four
source and actual expanded-token projections are verified independently;
literals/token order are preserved and CPP column-padding whitespace is ignored.

The initializer also repairs a recovery error. Native 0x266553a subtracts 901,
compares with 7, and uses unsigned JA to reject values outside IDs 901..908.
The predecessor's signed promoted-uint16 subtraction mistakenly included IDs
0..900 and redirected their source coordinates as if they were legacy effects.
All 65,536 uint16 IDs are checked. Exactly 901 predecessor cases differ; the
test verifies their old coordinates, correct native coordinates and byte-for-byte
agreement in every other fixture field. Only the source-player/source-card output
fields are normalized for that explicitly documented predecessor comparison.
Native and reconstruction agree directly in every case. Initial negative source
coordinates and activation-parent handling are also exercised.

Generation keeps both-player doublers, Academy Manufactor's native quantity
behavior, Chatterfang's extra group, Divine Visitation's lookups/overrides and
descriptor resets for each secondary-group token. It preserves cached counts,
fresh recipient reads, actions/equipment, generation callbacks, pre/post-entry
tracking, silent counters, blocker triggers and special-info-99 duplicate cleanup.
Token copying retains its fresh current_turn read between player passes and
reverse copy order. Dynamic IID resolution retains its 16-entry reuse search,
300-instance bitmap scan, backup/dummy IID rules, current engine-range checks,
72-byte type copy, narrowing PT writes, color changes and artifact/legend flags.
The special-info whitelist includes the native Sculpture case and preserves its
ID-specific 66/67 rules. No local unions or _2022 aliases are introduced.

Five native bodies and reconstruction agree on 212,096 cases (636,288 outer
calls); 211,195 also agree unchanged with the predecessor, alongside the 901
verified range repairs. The copy-info whitelist covers every uint16 backup ID
with both 66 and 67; explicit cases cover replacement effects, missing effect
sources, generation diagnostics, duplicate tokens and required action callbacks.
Four naming-only helpers claim no added native cases. Child queries, generation,
allocation and game effects are controlled substitutes. No native UI or composed
gameplay claim is made. Callback traces hash descriptor/type data, first 16
record/extension slots and current generated/legacy records; every allocated
record/extension and all declared output fixtures are byte compared at return.
Storage is bounded to allocated 150-slot banks with valid IIDs, finite quantities
and safe action arguments. Two private entries use regparm(2); copy/set adapters
use regparm(3). The read-only native switch table retains 56 verified targets.

The whole-file check also found generate_tokens_impl, an inline-only helper
omitted from the prior native-function census. Its old duplicated creation block
is replaced by an exact source/expanded-token projection from the verified
parent block, binding iid/n as its original parameters and adding its return of
the created slot. It remains in its original location and claims no new native
entry or comparison cases. The inline helper is now included in the census.

Together these ten helpers remove 15 gotos (14 LAB jumps), 455 lines and 744
placeholder references.
The entire token_generation.c unit now has zero LAB jumps and zero known
placeholder identifiers. Actual compiler-unit projection, four expanded naming
projections, layout/storage/enum probes, 21 source succession links (one exact
earlier rename bridge), build and all 15,169 fixed entries pass. Reccmp retains
all 18,725 mapping identities. copy_token now has a single rebuilt range,
so its annotation uses the ordinary recovery ledger instead of public-entry
disambiguation; its source definition is unchanged. The tracked census now has 2,593 LAB jumps and 47,925
placeholder references. The staged index is unchanged. See
[token-generation coverage](recovery-evidence/token-generation-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c393.json).

The event-core batch reconstructs resolve_trigger and get_abilities in
functions/events.c. The two dispatch wrappers
dispatch_event_arbitrary_to_one_card_with_initial_event_result and
dispatch_event_with_attacker_and_initial_event_result change only the saved
trigger-condition local's name; exact source and expanded-token projections
are checked, with only CPP column-padding whitespace ignored.

Trigger resolution now expresses its live-source, missing-source and proxy-card
paths directly. It retains trace/dialog ordering, triggering-state cleanup,
reserved-card/trigger eligibility, the 300-byte proxy copy, timestamp handling,
response windows, proxy stack metadata, cause remapping and repeated context
restoration around EVENT_TRIGGER_RESOLVED. Native high state-bit guards and
quiet-versus-resolved cleanup remain intact. The actual dialog and engine stack
operations remain controlled opaque callbacks in these checks.

Ability calculation retains the activation-parent walk, cached versus recalculated
characteristics, effective-event changes for P/T swapping, base/characteristic PT,
protection/landwalk changes, extension resets, two-pass type dispatch, damage/death
processing and cached return values around affected-stack restoration. Counter
types/counts are unpacked from their seven native storage slots with ordinary
byte/word operations; all eleven grants, player restrictions and humiliation
queries retain their order. Mana overrides use a low-byte cache: the decompiled
adjacent-word reconstruction contributes no additional low bits. Callback changes
to mana bytes, override tables, records, type data and extension pointers are
included. The native inclusive aura scan bound and change-type early return
without raw-base restoration are preserved. No local unions or _2022 aliases
are added.

Both native bodies, predecessor and reconstruction agree on 162,368 cases
(487,104 outer calls). Checks cover all 65,536 uint16 event values for ability
calculation, every uint16 trigger-source card ID, all 256 counter-type bytes in
each of seven slots with zero/nonzero counts, event/configuration crosses and
forced proxy replacement paths. Required callback families and every ability
counter query are asserted to execute. Native entries use cdecl. Engine function
pointer literals/direct calls are explicitly rebound, and the eleven-entry native
switch table remains read-only with internal targets verified. Memcpy is observed
without injecting mutations and copies exactly 300 bytes. Callback-boundary state
uses hashes; declared fixture outputs are compared byte for byte. Comparisons
require initialized allocated records, acyclic activation parents, bounded stack
and type storage, bounded strings and distinct source/proxy records. Raw -1 type
results use an allocated table prefix under the target 32-bit compiler behavior;
portable abstract-C behavior and complete native engine memory are not claimed.
Child callbacks and UI/gameplay composition remain outside these focused checks.

This removes 28 gotos (21 LAB jumps), 277 lines and 415 placeholder references.
The entire events.c unit now has zero LAB jumps and zero known placeholder
identifiers. Actual compiler-unit projection, layout/storage/enum probes, two
expanded naming projections, five source succession links, build and all 15,169
fixed entries pass. Reccmp retains all 18,725 mapping identities. The tracked
census now has 2,572 LAB jumps and 47,510 placeholder references. The staged index
is unchanged. See [event-core coverage](recovery-evidence/event-core-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c399.json).

The shared-helper local-name batch replaces 126 decompiler local spellings in
54 already structured helpers in functions/functions.c, removing 953 placeholder
references. The affected functions include shade/husk activation, countering and
deflection, death/leaves-play/ETB triggers, attack tracking, temporary effects,
transformation, graveyard replacement and mana text. Names describe observed use;
they are not claimed to be recovered original DWARF spellings. Locals reused for
several purposes retain appropriately general names. Existing unused declarations
remain explicitly named as unused where applicable.

Every rename is a reversible masked-identifier substitution, with one checked
local declaration, no collisions with existing function tokens, and exact
compiler-expanded token projection. All 54 expanded functions have zero ASSERT
literal exceptions. The entire expanded compiler unit changes only these bodies;
types, literals, expressions, statements and control flow are retained. GCC syntax
preflight, 93 source succession links and six exact older local-name bridges pass.
The rebuilt DLL has identical runtime section layouts and bytes, excluding debug
sections/debuglink and masking only the export-directory timestamp. This proves
this naming batch preserves the predecessor's compiled runtime; it does not add
native semantic coverage or claim existing engine behavior is fully recovered.

Build and all 15,169 fixed veneers pass. Reccmp's 18,725-row mapping CSV remains
byte-identical. The tracked inventory has 2,572 LAB jumps and 46,557 placeholder
references. The staged index is unchanged. See
[shared-helper local-name coverage](recovery-evidence/helper-local-names-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c403.json).

The damage-helper batch reconstructs damage_effects, damage_creature, effect_damage
and after_damage in functions/damage_effects.c, removing 23 LAB jumps, 233 lines
and 862 placeholder references. The whole unit now has no LAB jumps or known
placeholder identifiers. Review covers callback order, cached versus live reads,
activation/legacy source handling, redirected targets, card-specific damage
tracking, prevention, poison/wither, planeswalker overflow, death precedence,
deferred Archangel counters and the separate after-damage dispatch pass. The
unusual native scan through the record prefix before targets[0] is retained.

This batch adopts a lighter readability workflow: review, target syntax preflight,
one build and one reccmp refresh. It adds no native comparison cases or gameplay
equivalence claim. Build and all 15,169 fixed veneers pass; reccmp retains 18,725
mapping identities. The tracked census has 2,549 LAB jumps and 45,695 placeholder
references. The staged index is unchanged. See
[damage-helper coverage](recovery-evidence/damage-helpers-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c407.json).

The shared-control batch reconstructs 25 temporary-effect, pump, trigger and
selection helpers in functions/functions.c, removing 46 LAB jumps, 460 lines
and 674 placeholder references. Review retains callback ordering, cached/live
record reads, reverse iteration, current-turn-dependent pass order, distinct
pump-test exclusion orders, regeneration targeting, aura-return failure cleanup,
spell/ETB trigger repetition, sleight/hack mappings and clone-effect expiry.
Existing unusual zero-count dispatch and selection behavior is retained. Dialog
calls remain opaque; their caller-side arguments and ordering are preserved.

The lighter workflow uses review, target syntax preflight, one build and current
reccmp setup. No new native comparison cases or gameplay-equivalence claim are
added. Build and all 15,169 fixed veneers pass; reccmp retains all 18,725 mapping
identities. This unit is down to 12 functions with LAB jumps (67 jumps and 1,446
placeholder references). The tracked tree has 2,503 LAB jumps and 45,021
placeholder references. The staged index is unchanged. See
[shared-control coverage](recovery-evidence/shared-control-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c411.json).

The reanimation-control batch reconstructs nine helpers in functions/functions.c:
lobotomy_effect, both sacrifice/grave-target activation wrappers,
reanimate_permanent_impl, new_reanimate_all_impl,
count_for_gfp_ability_and_store_values_impl, shapeshift_all,
fx_permanents_destroyed_this_way_accumulate and new_effect_coercion. It removes
37 decompiler jumps (34 LAB jumps), 272 lines and 1,019 placeholder references.
Review retains callback ordering, cache/live field distinctions, aura attachment,
sacrifice reporting, graveyard choices, coercion retries and array removal,
mass-copy marking/application passes, packed death counters and delayed polling.
Mass reanimation retains its existing unusual array indexes and exile/ownership
choices; behavioral changes there require separate native evidence.

The lighter workflow uses review, target syntax preflight, one build and one
mapping refresh, without adding native comparison cases or gameplay-equivalence
claims. Build and all 15,169 fixed veneers pass; reccmp retains all 18,725 mapping
identities. This source unit now has three functions with LAB jumps: generic_spell,
granted_generic_activated_ability and legacy_effect_pump_ability_until_eot
(33 jumps and 427 placeholder references). The tracked census has 2,469 LAB jumps
and 44,002 placeholder references. The staged index is unchanged. See
[reanimation-control coverage](recovery-evidence/reanimation-control-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c415.json).

The spell-control batch reconstructs generic_spell,
granted_generic_activated_ability and legacy_effect_pump_ability_until_eot in
functions/functions.c, removing 36 decompiler jumps (33 LAB jumps), 95 lines
and 427 placeholder references. The entire unit now has zero LAB jumps and zero
known placeholder identifiers. Review preserves availability/equip queries,
redirection UI arguments, X setup, both sacrifice selectors, retarget storage and
bounds, duplicate marks, aura rejection, counter/graveyard order, packed costs,
payment/rollback order and zero-choice cancellation cleanup. The pump legacy
retains reciprocal source/target leaves-play behavior, live flag reloads,
expiration countdowns, end-of-combat/source/untap removal and final result clamp.

The lighter workflow uses review, target syntax preflight, one build and current
mapping refresh. No new native comparison cases or gameplay-equivalence claim
are added. Build and all 15,169 fixed veneers pass; reccmp retains all 18,725
mapping identities. The tracked census has 2,436 LAB jumps and 43,575 placeholder
references. The staged index is unchanged. See
[spell-control coverage](recovery-evidence/spell-control-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c419.json).

The pack-control batch reconstructs nine helpers in functions/packs.c, including
the inline get_csvid_choices omitted by the prior inventory. It removes 88
decompiler jumps (82 LAB jumps), 957 placeholder references and 790 net lines.
The whole unit now has zero LAB jumps and zero known placeholder identifiers.
get_csvid_choices is restored as the shared pool builder called by
make_pack_normal, eliminating the duplicated pool code. Both the predecessor
inline/parent blocks and the reconstructed inline/parent blocks are token-identical;
the pre-outlining parent is retained in the batch evidence.

Review preserves all pack recipes/copy tables, weighted rarity and special-sheet
pools, Legends/DFC/contraption multiplicities, random draw order, basic/snow-land
duplicate exceptions, shortage fallback, conditional recipe choices, retry limits,
the DFC-count requirement and sticky testing-mode duplicate status. The existing
test_packs utility is reconstructed but not executed as a gameplay test. The
lighter workflow uses review, compiler preflight, a final build and mapping refresh.
No native comparison cases or gameplay-equivalence claim are added. Build and all
15,169 fixed veneers pass; reccmp retains all 18,725 mapping identities.

The current census now includes 2,606 definitions, with 2,387 LAB jumps and 42,900
placeholder references. Its delta counts only the eight previously tracked
functions; the old inventory omitted the inline helper's 33 LAB jumps and 282
placeholder references. The staged index is unchanged. See
[pack-control coverage](recovery-evidence/pack-control-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c426.json).

The rules-control batch reconstructs check_card_for_rules_engine,
get_storm_count and static_graveyard_abilities in functions/rules_engine.c,
removing 82 decompiler jumps (79 LAB jumps), 479 lines and 787 placeholder
references. The whole unit now has zero LAB jumps and known placeholders.
Review preserves classification masks and post-query ID/class refreshes,
event-stage routing, callback order, cached/live trigger context, grave-slot
rereads, per-card payment/return/exile differences, and direct/wrapped xtrigger
snapshots. Opaque dialog calls remain out of scope.

The lighter workflow uses review, target syntax preflight, one build and current
mapping refresh. No new native comparison cases or gameplay-equivalence claim
are added. Build and all 15,169 fixed veneers pass; reccmp retains all 18,725
mapping identities. The tracked census has 2,308 LAB jumps and 42,113 placeholder
references. The staged index is unchanged. See
[rules-control coverage](recovery-evidence/rules-control-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c430.json).

The Aether Revolt readability batch updates 49 definitions (43 card handlers and
six local helpers): 19 exact alpha-only token substitutions and 30 reviewed
control-flow reconstructions. It removes 60 decompiler jumps (58 LAB jumps),
770 lines and 580 placeholder references. Event qualification/rechecks, callback
order, live/cached reads, native deck/target quirks, cancellation cleanup,
death aggregation and opaque dialog behavior are retained. The prompt and
alignment-specific buffer initialization are reconstructed as ordinary arrays.

One target build and all 15,169 fixed veneers pass. No new native comparison
cases are claimed. Reccmp refresh is deliberately deferred to a larger
checkpoint; its existing setup still describes the preceding build. The source
census is now 2,250 LAB jumps and 41,533 placeholder references. Aether Revolt
has 48 LAB jumps and 591 placeholder references remaining. The index is
unchanged. See [Aether Revolt coverage](recovery-evidence/aether-control-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c434.json).

The final nine Aether Revolt definitions are now structured: Pia's Revolution
and its effect, Unbridled Growth, Scrap Trawler and its legacy, Exquisite
Archangel, Indomitable Creativity, the improvise casting helper and Release the
Gremlins. This adds 49 removed jumps (48 LAB jumps), 628 removed lines and 591
removed placeholder references. Review preserves packed death records and
reverse resolution, native slot/player lookup quirks, cached/live context,
fixed/X payment routes, retarget previews and cancellation cleanup.

The cumulative Aether Revolt batch covers 58 definitions (49 card handlers and
nine helpers), removing 106 LAB jumps, 1,171 placeholder references and 1,398
lines. The whole file now has zero LAB jumps and known placeholders. The final
build and all 15,169 fixed veneers pass. Reccmp refresh remains deferred; no new
native semantic cases are claimed. The census is 2,202 LAB jumps and 40,942
placeholder references. See the updated
[Aether Revolt coverage](recovery-evidence/aether-control-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c438.json).

The Mirage readability batch updates 61 definitions (52 card handlers and nine
helpers): 34 exact alpha-only substitutions and 27 reviewed control-flow
reconstructions. It removes 99 decompiler jumps (98 LAB jumps), 775 lines and
1,154 placeholder references. The entire file now has zero LAB jumps and known
placeholder identifiers. Review preserves phasing recursion and detachment,
repeated queries, native stored-target and color-refresh quirks, aura trigger
and flash-expiry behavior, sacrifice scoring, mana filtering and Builder's
Bane's distinct payment, retargeting and destruction/life-loss paths.

One target build and all 15,169 fixed veneers pass. No new native comparison
cases are claimed. Reccmp refresh remains deferred to a larger checkpoint;
its existing setup still describes the preceding checkpoint build. The source
census is now 2,104 LAB jumps and 39,788 placeholder references. The staged index
is unchanged. See [Mirage coverage](recovery-evidence/mirage-control-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c442.json).

The Eldraine readability batch updates 57 definitions (47 card handlers and ten
helpers), with 19 exact alpha-only substitutions and 38 structured or extracted
definitions. It removes 80 decompiler jumps (77 LAB jumps), 575 placeholder
references and 992 net source lines. Review preserves adventure legality,
free/token/copied modes, targeting/retarget behavior, transformation rollback,
query/callback ordering, packed death counters, fresh context and native quirks.
The existing private resolve_an_adventure helper now supplies the caller's
resolution body; expanding the call reproduces the reviewed caller tokens.

One initial batch build and the final shared-helper build pass, including all
15,169 fixed veneers. No new native semantic cases are claimed. Reccmp refresh
remains deferred. The prior census omitted the inline helper's three LAB jumps
and 28 placeholder references; its clean definition is now tracked. The census
has 2,607 definitions, 2,030 LAB jumps and 39,241 placeholder references. Eldraine
has eight definitions left with 18 LAB jumps and 230 placeholder references.
The staged index is unchanged. See
[Eldraine coverage](recovery-evidence/eldraine-control-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c448.json).

The final eight Eldraine definitions are now structured: Steelclaw Lance,
Lochmere Serpent, Kenrith, Gilded Goose, Golden Egg, Trail of Crumbs, Fabled
Passage and Dance of the Manse. This removes 19 jumps (18 LAB jumps), 159 lines
and 230 placeholder references. Review preserves validation-driven mode reloads,
distinct costs and tap-state rollback, duplicate legacy detection, Fabled
Passage's repeated activation calls and mode markers, and Dance's selection-test
reset, cached X, CMC/payment maxima and AI payment retry.

The cumulative Eldraine batch covers 65 definitions (55 card handlers and ten
helpers), removing 95 LAB jumps, 805 placeholder references and 1,151 net lines.
The entire file has zero goto statements and known placeholder identifiers.
The final build and all 15,169 fixed veneers pass. Reccmp refresh remains
deferred; no new native semantic cases are claimed. The census is 2,012 LAB
jumps and 39,011 placeholder references. The index is unchanged. See
[Eldraine coverage](recovery-evidence/eldraine-control-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c452.json).

The first War of the Spark readability batch updates 60 definitions (44 card
handlers and 16 helpers): 43 exact alpha-only substitutions and 17 reviewed
control-flow reconstructions. It removes 39 jumps (38 LAB jumps), 328 lines and
798 placeholder references. Review preserves trigger and aura context, native
death/detach differences and byte offsets, duplicate legacy detection, live
bounds, validation-driven mode reloads, loyalty/payment behavior and fresh
callback reads. Sorin retains grave selection after failed payment; two-target
damage, milling and amass paths preserve their native eligibility rules.

One target build and all 15,169 fixed veneers pass. No new native semantic
cases are claimed. Reccmp refresh remains deferred until the file's final
batch. Five larger definitions remain with 49 LAB jumps and 414 placeholder
references. The tracked census is 1,974 LAB jumps and 38,213 placeholder
references. The staged index is unchanged. See
[War coverage](recovery-evidence/war-control-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c456.json).

The final five War of the Spark routines are structured: the discounted mana
helper, Casualties of War, Liliana Dreadhorde General, Bolas's Citadel and
Planewide Celebration. This removes 51 jumps (49 LAB jumps), 404 lines and 414
placeholder references. Review preserves unsigned cost clamping, temporary
hybrid/phyrexian card setup and cleanup, fresh validation masks, death/activation
query order, sacrifice rollback and packed-mode resolution passes.

The cumulative War batch covers 65 definitions (48 card handlers and 17
helpers), removing 87 LAB jumps, 1,212 placeholder references and 732 lines.
The entire file now has zero LAB jumps and known placeholders. The final build
and all 15,169 fixed veneers pass. Reccmp is refreshed for the combined Aether
Revolt, Mirage, Eldraine and War cleanup: all 18,725 mapping identities remain,
with 12 mapping annotations updated. These four units have zero LAB jumps and
known placeholders. No new native semantic comparison cases are claimed.
The census is 1,925 LAB jumps and 37,799 placeholder references. The index is
unchanged. See [War coverage](recovery-evidence/war-control-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c460.json).

The first Strixhaven readability batch updates 75 definitions (62 card handlers
and 13 helpers): 52 exact alpha-only substitutions and 23 reviewed control-flow
reconstructions. It removes 31 LAB jumps, 526 lines and 1,177 placeholder
references. Review preserves selection order, trap and mana queries, native
power indexing, reverse hand/grave traversal, alternate/full/X cost outcomes,
validation/life order, death-helper differences, cleanup and context refreshes.
Journey to the Oracle boundary behavior remains separate native review work.

One target build and all 15,169 fixed veneers pass. No new native semantic
cases are claimed. Reccmp refresh is deferred until a larger checkpoint. The
file has 24 definitions remaining, with 74 LAB jumps and 1,134 placeholder
references. The tracked census is 1,894 LAB jumps and 36,622 placeholder
references. The staged index is unchanged. See
[Strixhaven coverage](recovery-evidence/strixhaven-control-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c464.json).

The final 24 Strixhaven definitions are now structured (21 card handlers and
three helpers), removing 78 gotos, including 74 LAB jumps, 642 lines and 1,134
placeholder references. Review preserves command choice/target ordering,
retarget context, payment/fizzle outcomes, library selection and array shifts,
mode rechecks, GFP detachment and cleanup distinctions. Quandrix's availability
calculation now uses separate counters with the same ordered queries and
0/1/99 outcomes. Native disassembly confirms Journey to the Oracle's inclusive
reverse scan, including its uninitialized count-index entry; that behavior is
preserved and documented in source.

The cumulative Strixhaven cleanup covers 99 definitions (83 card handlers and
16 helpers), removing 109 gotos (105 LAB jumps), 2,311 placeholder references
and 1,168 lines. The whole file has zero gotos and known Ghidra placeholders.
One final target build and all 15,169 fixed veneers pass. Reccmp refresh remains
deferred; no new native gameplay comparison cases are claimed. The census is
1,820 LAB jumps and 35,488 placeholder references. The staged index is unchanged.
See [Strixhaven coverage](recovery-evidence/strixhaven-control-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c468.json).

Commander Legends readability cleanup updates 86 definitions (69 public card
handlers and 17 helpers): 60 exact alpha-only local substitutions and 26
reviewed control-flow reconstructions. It removes 51 gotos (49 LAB jumps),
263 lines and 1,895 placeholder references. The whole file has zero gotos and
known Ghidra placeholders. Review preserves death eligibility and detach flags,
source-death early exits, pending-record cleanup, Colfenor's main/effect record
slot differences, meld-part exclusions, repeated queries, persistent filters
and retry budgets. Equipment mode rechecks, protection snapshots, Kamahl's
animation/damage order and War Room's native life/AI paths remain intact.

One target build and all 15,169 fixed veneers pass. No new native semantic
cases or historical replay are claimed. Reccmp refresh remains deferred. The
tracked census is 1,771 LAB jumps and 33,593 placeholder references. The staged
index is unchanged. See
[Commander Legends coverage](recovery-evidence/cmr-control-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c472.json).

Modern Horizons 2 readability cleanup updates 91 definitions (77 public card
handlers and 14 helpers): 59 exact alpha-only local substitutions and 32
reviewed control-flow reconstructions. It removes 77 gotos (71 LAB jumps),
532 lines and 1,532 placeholder references. The unit has zero LAB jumps and
known Ghidra placeholder locals; Rift Sower's descriptive-label jump remains
outside this batch. Review preserves ordinary/channel activation routing,
alternate costs, fizzle results, repeated grave queries, mode/validation
rechecks, legacy detach and cleanup differences, cached/live player/count
reads, reverse phasing/control/animation order, Garth's ordered availability
checks, split-card delegation and Saga routes.

Native disassembly confirms Verdant Command's human-path uninitialized dialog
priorities (0x022430df-0x0224311f); this existing behavior is retained and
commented. Its two-choice target indexing, including life mode's lack of
advancement, is preserved. One batch build and all 15,169 fixed veneers pass;
known uninitialized-priority warnings remain. No new native gameplay cases or
historical replay are claimed. Reccmp refresh remains deferred. The census is
1,700 LAB jumps and 32,061 placeholder references. The index is unchanged. See
[Modern Horizons 2 coverage](recovery-evidence/mh2-control-cleanup-coverage.json)
and [current inventory](recovery-evidence/readability-source-snapshot-c476.json).

Ice Age/New Phyrexia readability cleanup updates 71 definitions together:
28 exact alpha-only local substitutions and 43 reviewed control-flow changes.
It removes 112 gotos (108 LAB jumps), 560 lines and 1,565 placeholder references.
Payment rechecks, direct mana paths, cached/live attachment reads, aura event
routing, reverse creature scans and post-callback counter reads are preserved.
Karn's restart is unchanged for a focused follow-up. Ice Age's remaining legacy
break2 jump and historical/disabled code are outside this recovered-definition
batch. One combined build and all 15,169 fixed veneers pass.

Verification policy: exact token checks for local renames; focused source review
for structured edits; one incremental target build per substantial batch. Native
inspection is reserved for uncertain behavior. Reccmp and broader checks wait for
major comparison/commit checkpoints. No new tests or historical replay are added
for routine readability cleanup. The census is 1,592 LAB jumps and 30,496
placeholder references. The staged index is unchanged. See
[batch coverage](recovery-evidence/ice-nph-control-cleanup-coverage.json) and
[current inventory](recovery-evidence/readability-source-snapshot-c480.json).

Karn/Mercadian Masques readability cleanup updates 66 definitions: 54 exact
alpha-only local substitutions and 12 reviewed control-flow changes. It removes
32 LAB jumps and 1,533 placeholder references. Both files now have no LAB jumps
or known placeholder locals. Karn retains commander tracking and reentry order,
separate reverse scans, both stack sweeps, context snapshots, energy resets,
companion/plane handling and hand-size callback restoration. Native inspection
confirms its pre-sentinel card-data load and inclusive exile cleanup; both quirks
remain. Mercadian Masques retains alternative-cost cancellation, payment and
post-fizzle target-selection differences, aura/trigger routing and cleanup.

One combined build and all 15,169 fixed veneers pass. No new gameplay tests or
historical replay were run; reccmp refresh remains deferred under the lighter
batch verification policy. The census is 1,560 LAB jumps and 28,963 placeholder
references. The index is unchanged. See
[batch coverage](recovery-evidence/karn-masques-control-cleanup-coverage.json) and
[current inventory](recovery-evidence/readability-source-snapshot-c484.json).

Prophecy readability cleanup updates 57 definitions: 30 exact alpha-only local
substitutions and 27 reviewed control-flow changes. It removes 71 gotos (69 LAB
jumps), 453 lines and 761 placeholder references. The file now has no gotos or
known placeholder locals. Related any-player abilities preserve source checks,
repeated record queries, inclusive live legacy scans, opponent search exits,
source/activation slot differences and distinct card versus legacy cleanup.
Dual Nature retains departure query order, cached affected player and separate
reverse token scans. Copper-Leaf Angel retains marked selections, activation
before reverse sacrifices, live fizzle checks and post-kill slot increments.
Rhystic, aura, combat and regeneration routes retain their callback order.

One target build and all 15,169 fixed veneers pass. Reccmp refresh remains
deferred; no new gameplay tests or historical replay were run. The census is
1,491 LAB jumps and 28,202 placeholder references. The staged index is unchanged.
See [batch coverage](recovery-evidence/prophecy-control-cleanup-coverage.json) and
[current inventory](recovery-evidence/readability-source-snapshot-c488.json).

Modern Horizons/M19 readability cleanup updates 76 definitions: 47 exact
alpha-only local substitutions and 29 reviewed control-flow changes. It removes
55 gotos (48 LAB jumps), 251 lines and 1,316 placeholder references. Both files
now have no LAB jumps or known placeholder locals; existing descriptive-label
jumps in Hogaak/convoke implementations outside the census remain unchanged.
Kaya Guile retains ordered per-mode rechecks, Wrenn/Sisay keep player snapshots
and separate scans, and Endling retains cached keywords and priority/payment
routing. Pitch spells, shared Sliver abilities, Undying, Hogaak's grave masking
and convoke/delve sequence retain callbacks and native quirks. M19 death helpers
keep source-death differences, detach flags, low-byte counts and cleanup. Liliana
keeps grave-pointer refreshes and the selected list-index argument; Chaos Wand's
reveal/bottom counts and Contract's two-player scans are preserved.

One combined target build and all 15,169 fixed veneers pass. No new gameplay
tests or historical replay were run; reccmp refresh remains deferred. The census
is 1,443 LAB jumps and 26,886 placeholder references. The index is unchanged.
See [batch coverage](recovery-evidence/mh1-m19-control-cleanup-coverage.json) and
[current inventory](recovery-evidence/readability-source-snapshot-c492.json).

Invasion/Hour of Devastation readability cleanup updates 77 definitions:
49 exact alpha-only local substitutions and 28 control-flow changes. It removes
55 gotos (54 LAB jumps), 115 lines and 1,083 placeholder references. Both files
have no LAB jumps or known placeholder locals. Split-card selection/payment
keeps callback order, free/override paths, fused costs, X storage and fizzle
cleanup. Desert activations keep availability/payment differences, state rollback,
mode writes and mana life loss. Scorpion death tracking, Samut target cancellation,
Unesh triggers, Leave/Chance and Nimble stack/cycling behavior are preserved.

Native inspection confirms Torment's sacrifice choice also loses 3 life and Hour
Devastation's two reverse scans with a fresh player lookup between them; both are
retained. One combined build and all 15,169 fixed veneers pass. The batch includes
76 tracked definitions plus one unused private cycling worker outside the prior
census. The tracked census is 1,391 LAB jumps and 25,865 placeholder references;
the worker removed another 2 LAB jumps and 62 references outside those totals.
No new gameplay tests or historical replay were run. Reccmp refresh remains
deferred and the index is unchanged. See
[batch coverage](recovery-evidence/invasion-hou-control-cleanup-coverage.json) and
[current inventory](recovery-evidence/readability-source-snapshot-c496.json).

Amonkhet/Rivals of Ixalan readability cleanup updates 78 definitions (68 card
handlers and 10 helpers): 46 exact local-token substitutions and 32 reviewed
control-flow changes. It removes 70 gotos, including 65 LAB jumps, and 1,018
placeholder references. Both files now have no LAB jumps or known placeholder
locals. The combined build passes with all 15,169 fixed jump-table slots verified.
See [batch record](recovery-evidence/akh-rix-control-cleanup.json).

Further readability batches use `tools/recovery/apply_readability_batch.py` to
apply reviewed drafts and record exact source edits without adding a dedicated
verifier branch. Check affected files and local-token substitutions, review
structured flow, and build once per substantial combined batch. Reserve native
inspection for unresolved behavior; consolidate inventories and reccmp updates
at larger comparison/commit checkpoints. The c496 inventory predates this batch.

Dominaria/Zendikar Rising readability cleanup updates 94 definitions (77 card
handlers and 17 helpers): 37 exact local-token substitutions and 57 reviewed
structured edits. It removes 117 gotos, including 115 LAB jumps, 1,597 placeholder
references and 721 lines. Both files now have no LAB jumps or known placeholder
locals. The combined build passes with all 15,169 fixed jump-table slots verified.
Saga chapters, MDFC payment/dispatch, inscriptions, party counting, death effects
and activation cleanup retain their existing behavior and query/callback order.
See [batch record](recovery-evidence/dom-znr-control-cleanup.json). Current-file
checks pass; historical replay, reccmp and inventory regeneration remain deferred.

Guilds of Ravnica/Ravnica Allegiance/Unstable readability cleanup updates 104
existing definitions (83 card handlers and 21 helpers): 57 exact local-token
substitutions and 47 reviewed structured edits. It removes 110 gotos, including
104 LAB jumps, 1,801 placeholder references and 344 lines. All three files have
no LAB jumps. Guilds and Unstable also have no known placeholder locals; the
unused private `pay_spectacle` definition in Allegiance still has 18 placeholder
references outside the tracked census and remains a follow-up cleanup target.
The combined build and all 15,169 fixed jump-table slots pass. See
[batch record](recovery-evidence/grn-rna-unstable-control-cleanup.json). Historical
replay, reccmp and inventory regeneration remain deferred.

Theros Beyond Death/M20/private spectacle readability cleanup updates 78 existing
definitions (62 card handlers and 16 helpers): 40 exact local-token substitutions
and 38 reviewed structured edits. It removes 69 gotos, including 64 LAB jumps,
1,566 placeholder references and 140 lines. Theros, M20 and Allegiance now have
no LAB jumps or known placeholder locals; Allegiance's private `pay_spectacle`
follow-up above is complete. This batch has 77 tracked definitions plus that
private helper outside the old census. The combined build and all 15,169 fixed
jump-table slots pass. See
[batch record](recovery-evidence/thb-m20-spectacle-control-cleanup.json).
Historical replay, reccmp and full inventory regeneration remain deferred.

Tempest/Visions/Alliances readability cleanup updates 91 existing definitions
(79 card handlers and 12 helpers): 52 exact local-token substitutions and 39
structured edits. It removes 84 LAB jumps, 1,619 placeholder references and
269 lines. All three files now have no LAB jumps or known placeholder locals.
The combined incremental build passes with all 15,169 fixed jump-table slots.
Licid mode transitions, shared Sliver lifecycle checks, phased creature passes,
duplicate-target merging and legacy/exile bookkeeping retain recovered behavior.
Casting of Bones now clears its complete 200-int array with `memset` rather
than pointer stepping. See
[batch record](recovery-evidence/tempest-visions-alliances-control-cleanup.json).
Verification checks affected source pins, exact token substitution for local
renames, and changed control flow, followed by one combined build. Full audits,
inventory refreshes and reccmp remain deferred to a larger checkpoint; native
inspection is reserved for unresolved behavior.

Commander 2018/Dissension/Urza's Saga readability cleanup updates 73 existing
definitions (60 card handlers and 13 helpers): 39 exact local-token substitutions
and 34 structured edits. This includes the private `totem_armor_any` helper
outside the old inventory. It removes 86 gotos (84 LAB jumps), 1,861 placeholder
references and 436 lines. All three files now have no LAB jumps or known
placeholder locals. The combined incremental build and all 15,169 fixed
jump-table slots pass. Split-card routing, planeswalker battlefield scans,
Augury type choices, Arixmethes type caching and death-trigger callback order
retain recovered behavior. Saheeli/Windgrace buffer clearing uses bounded
`memset`, and Contamination uses indexed writes to the same override entries.
See [batch record](recovery-evidence/c18-dissension-usg-control-cleanup.json).
Only affected-file checks, exact local-token checks, focused structured review
and the combined build were run; full audits, inventory and reccmp refreshes
remain deferred.

Vanguard/Unglued-Unhinged/Ikoria readability cleanup updates 53 existing
definitions (32 card handlers and 21 helpers): 30 exact local-token substitutions
and 23 structured edits. It removes 32 gotos (30 LAB jumps), 1,798 placeholder
references and 192 lines. All three files now have no known placeholder locals;
Ikoria has no LAB jumps. The 48 LAB jumps in `card_edh` and 11 in `get_rules_text`
remain unchanged: those large definitions receive local renames only, verified
by exact token substitution. The batch application tool now permits unchanged
labels in alpha-only edits, allowing local cleanup independently of later
control-flow reconstruction. Other changes preserve commander movement and
special casting, word-count exceptions, die rolls after permanent removal,
companion legality quirks and Broodmoth target indexing. The combined incremental
build and all 15,169 fixed jump-table slots pass. See
[batch record](recovery-evidence/vanguard-un-ikoria-readability-cleanup.json).
Full audits, inventory regeneration and reccmp remain deferred.

Ixalan/Weatherlight/Exodus readability cleanup updates 69 existing definitions
(60 card handlers and 9 helpers): 42 exact local-token substitutions and 27
structured edits. It removes 51 gotos (48 LAB jumps), 956 placeholder references
and 231 lines. All three files now have no known placeholder locals. Star of
Extinction, Betrothed of Fire, Coils of the Medusa and both Exodus licids retain
their control flow while receiving local names. Other edits simplify activation
and aura routing, Huatli duplicate-target merging, death-trigger bookkeeping,
and Aether Tide resolution without changing recovered query/callback order.
Spike Cannibal uses indexed counter snapshots and a sizeof-based clear of the
same 302-int array. The combined incremental build and all 15,169 fixed
jump-table slots pass. See
[batch record](recovery-evidence/xln-wth-exo-readability-cleanup.json).
Full audits, inventory regeneration and reccmp remain deferred.

Time Spiral/Nemesis/Portal readability cleanup updates 62 existing definitions
(46 card handlers and 16 helpers): 42 exact local-token substitutions and 20
structured edits. It removes 33 gotos (32 LAB jumps), 734 placeholder references
and 284 lines. All three files now have no known placeholder locals. Nemesis
and Portal have no LAB jumps; the two in `generic_suspend_legacy` remain
unchanged under exact local-token substitution. Shared Sliver lifecycle checks,
Ib Halfheart sacrifice cleanup, Clockspinning AI predicates, Mana Cache source
handling and callback-sensitive target counts retain recovered behavior.
Pale Moon uses indexed writes to the same seven override entries per player.
The combined incremental build and all 15,169 fixed jump-table slots pass.
See [batch record](recovery-evidence/tsp-nemesis-portal-readability-cleanup.json).
Full audits, inventory regeneration and reccmp remain deferred.

Lorwyn/Fifth Dawn/Homelands/Commander 2017/Battlebond readability cleanup updates
58 existing definitions (45 card handlers and 13 helpers): 40 exact local-token
substitutions and 18 structured edits. This includes the private `assist` helper
outside the old inventory. It removes 31 LAB jumps, 1,273 placeholder references
and 81 lines. All five files now have no known placeholder locals; Lorwyn and
Commander 2017 also have no LAB jumps. Clockwork, Gemstone Array, Brightling and
random-permanent network handling retain their control flow under exact token
substitution. Other edits simplify Colfenor's Urn death/selection handling,
Station triggers, Homelands aura cleanup, Kindred event routing and Arcane
Artisan token removal while retaining recovered callback order and quirks.
Kindred Charge uses a sizeof-based clear of the same 151-byte marking buffer.
The combined incremental build and all 15,169 fixed jump-table slots pass.
See [batch record](recovery-evidence/lrw-5dn-hml-c17-bbd-readability-cleanup.json).
Full audits, inventory regeneration and reccmp remain deferred.

Core test/sacrifice readability cleanup updates 19 existing helpers in
`functions/global_checking.c` and `functions/sacrifices.c`: 18 exact local-token
substitutions and one structured payment/sacrifice branch. It removes 968
placeholder references, including the underscored `p_Var17` callback name,
and one LAB jump. Both files now have no known placeholder locals. Existing
optimized control flow, callback-sensitive cached flags, fizzle restoration,
target snapshots and packed sacrifice results remain unchanged apart from the
reviewed branch. The batch tooling and source-reader registry now accept
`functions/` inputs and detect underscored temporary spellings.

Direct inspection of the native DBG confirms useful original local names and
optimized location lists. The test helper records include owning DIEs, local
vocabulary and the DBG hash. Score accumulators use native `TEST_TYPE` and
`test_type` spellings; remaining names describe observed roles. A compiler
temporary can represent several original locals, and some DWARF locals have
no location, so a universal automatic name correspondence is not claimed.
The combined incremental build and all 15,169 fixed jump-table slots pass.
See [batch record](recovery-evidence/core-test-sacrifice-local-cleanup.json).
Full audits, inventory regeneration and reccmp remain deferred.

Ravnica/Innistrad/core-set readability cleanup updates 64 existing definitions
in Unlimited, Ravnica, Onslaught, Shadows over Innistrad, Return to Ravnica and
Dragon's Maze (38 card handlers and 26 helpers): 45 exact local-token
substitutions and 19 structured edits. It removes 47 gotos (46 LAB jumps),
1,741 placeholder references and 129 lines. All six files now have no known
placeholder locals, including the private `notion_thief_effect` missed by
the old census. Complex graveyard selection, Warp World, damage packing and
other retained flow receive exact token substitution only. Pointer-typed
counter representations remain candidates for a separate representation review.
Structured changes share aura/fuse event routing and clue/GFP lifecycle paths,
preserving target initialization order, callback-sensitive flag reloads and
GFP-specific early returns. Behind the Scenes retains generic cleanup handling
without pumping creatures; Entrails Feaster uses an initialized 100-byte,
zero-padded message array. The combined incremental build, affected-file and
definition pins, whitespace checks and all 15,169 fixed jump-table slots pass.
The user's staged index is preserved. See
[batch record](recovery-evidence/ravnica-innistrad-core-readability-cleanup.json).
Full audits, inventory regeneration and reccmp remain deferred.

Stronghold/Urza's Legacy/Commander 2019/Gargaroz/Jumpstart/Khans readability
cleanup updates 72 existing definitions (46 card handlers and 26 helpers):
49 exact local-token substitutions and 23 structured edits. It removes 39
gotos (33 LAB jumps), 894 placeholder references and 117 lines. All six files
now have no known placeholder locals; Stronghold and Urza's Legacy also have
no LAB jumps. Structured changes share Sliver source-lifecycle checks and GFP
effect routing, replace existing-legacy search jumps with found flags, and
simplify Elsha, Ricochet Trap, Retribution and Chainer event paths. Chainer
retains graveyard-pointer reloads after legality, mana and timing callbacks;
Elven Rite retains live target counts after counter callbacks. Aura destruction
clears the same 302-int snapshot using sizeof and caches the next card before
destruction callbacks. Larger Atla, Kels, Volrath, Consulate and sacrifice
selection paths retain their flow under exact token substitution. No new
native coverage or gameplay-equivalence claim is made. The combined incremental
build, affected-file and definition pins, evidence pins, whitespace checks and
all 15,169 fixed jump-table slots pass. The user's staged index is preserved.
See [batch record](recovery-evidence/stronghold-commander-khans-readability-cleanup.json).
Full audits, inventory regeneration and reccmp remain deferred.

Abilities/deck/older-set readability cleanup updates 68 existing definitions
in abilities, deck, Scars of Mirrodin, Shadowmoor, Zendikar, Antiquities and
Legends (35 card handlers and 33 helpers): 44 exact local-token substitutions
and 24 structured edits. It removes 45 gotos (44 LAB jumps), 1,056 placeholder
references and 82 lines. All seven files now have no known placeholder locals;
deck and Shadowmoor also have no LAB jumps. Structured changes cover Humility
cleanup, protection-source traversal, proliferate selection, GFP effect routing,
reverse permanent scans, Power Artifact and Time Elemental event paths, and
ordinary buffer initialization. The graveyard source-ID clear still affects
both players' 1,000 entries, and granted flanking clears the same 302-byte buffer.
Proliferate retains target-query and counter-choice order; Blowfly still queries
targets before checking its remaining count and retries canceled picks. Power
Artifact retains casting rechecks, its target-change return, temporary aura
states and cost-modifier calls. Larger duration, battery, aura and AI paths
retain their flow under exact token substitution. No new native coverage or
gameplay-equivalence claim is made. The combined incremental build, affected-file
and definition pins, evidence pins, whitespace checks and all 15,169 fixed
jump-table slots pass. The user's staged index is preserved. See
[batch record](recovery-evidence/ability-deck-older-set-readability-cleanup.json).
Full audits, inventory regeneration and reccmp remain deferred.

Subtype/plane/counter readability cleanup updates 62 existing definitions in
subtype helpers, Innistrad, Coldsnap, Champions of Kamigawa, Planar Chaos,
Planechase, The Dark and Darksteel (44 card handlers and 18 helpers): 36 exact
local-token substitutions and 26 structured edits. It removes 53 gotos
(52 LAB jumps), 1,114 placeholder references and 154 lines. All eight files
now have no known placeholder locals. Subtype helpers, Innistrad, Champions
and Darksteel also have no LAB jumps. Structured changes include Haakon casting
and graveyard dispatch, shared Sliver lifecycle, Reality Acid aura routing,
Grand Ossuary counter distribution, Aether Snap marking/removal, Frankenstein
counter choices, Spitting Slug combat eligibility and Worms of the Earth flags.
Subtype and shared-subtype buffers use sizeof-based clears; Deep Water writes
the same seven mana-override entries per player with indices. Recovered quirks
remain, including subtype-adjacent reads, Moonmist marked-value transforms,
Grand Ossuary target-record reuse, Bushido's enabled-resolution shortcut and
callback-sensitive snapshots. Larger land-conversion, plane-damage and modal
paths retain flow under exact token substitution. No new native coverage or
gameplay-equivalence claim is made. The combined incremental build, affected-file
and definition pins, evidence pins, whitespace checks and all 15,169 fixed
jump-table slots pass. The user's staged index is preserved. See
[batch record](recovery-evidence/subtype-plane-counters-readability-cleanup.json).
Full audits, inventory regeneration and reccmp remain deferred.

Tarkir/Strive/artifact readability cleanup updates 60 existing definitions across
Dragons of Tarkir, Future Sight, Gatecrash, Journey into Nyx, Mirrodin, Urza's
Destiny, Battle for Zendikar, Betrayers of Kamigawa and Fallen Empires (49 card
handlers and 11 helpers): 44 exact local-token substitutions and 16 structured
edits. It removes 31 gotos (30 LAB jumps), 1,299 placeholder references and 60
lines. All nine files now have no known placeholder locals; Dragons of Tarkir
and Gatecrash also have no LAB jumps. Structured edits cover Evolve, Thrull
Parasite, Strive selection/payment and target-state cleanup, counter marking
in Scale Blessing and Drana, Oblivion Stone scans, Catacomb Sifter lifecycle,
and straightforward spell/aura dispatch. Strive preserves cached cleanup-entry
counts, live target counts within cleanup and fizzle checks after callbacks.
Oblivion Stone retains the executable callback between its scans. Larger
Glissa, Kira, storage-land, aura and modal AI paths retain their flow under
token substitution; Merseine's pointer-typed cost representation and Zada's
documented native non-advancing copy loop remain unchanged. No new native
coverage or gameplay-equivalence claim is made. The combined incremental build,
affected-file and definition pins, evidence pins, whitespace checks and all
15,169 fixed jump-table slots pass. The user's staged index is preserved. See
[batch record](recovery-evidence/tarkir-strive-artifact-readability-cleanup.json).
Full audits, inventory regeneration and reccmp remain deferred.

The next combined readability batch covers 63 definitions in M15, Rise of the
Eldrazi, the graveyard-count helpers, Apocalypse, Avacyn Restored, Commander,
Commander 2016, Dark Ascension, Eldritch Moon, Eventide, Fate Reforged and
Origins: 45 card handlers and 18 helpers. Exact rename token checks cover 45
definitions; focused control-flow review covers the other 18. The batch removes
1,612 placeholder occurrences and 32 gotos, including 31 LAB jumps. Eternal
Thirst retains the instance refresh after GFP resolution; Aggressive Mining
retains cleanup and leaves-play callback ordering. Bolster preserves its initial
minimum and tie-count quirks, while Warden of the First Tree refreshes its
selected mode only on the original paths. Recruiting and Dark Impostor preserve
cursor advances around callbacks. Larger modal handlers retain their existing
control flow under exact local substitution. All twelve affected files now
have no placeholder locals matching the cleanup census. The single combined
incremental build passes, including all 15,169 fixed jump-table slots; affected
source/evidence pins, whitespace and staged-index preservation checks pass.
See [batch record](recovery-evidence/core-commander-level-readability-cleanup.json).
No new native coverage or gameplay-equivalence claim is made. Full audits,
inventory regeneration and reccmp refreshes remain deferred; the lightweight
batch verification policy continues at the user's request.

The next readability batch covers 71 definitions across twenty source files:
56 card handlers and 15 helpers. Exact rename token checks cover 55 definitions;
focused review covers 16 structured definitions. It removes 1,317 placeholder
references and 22 gotos, including 21 LAB jumps. Counter-distribution loops in
Shambling Swarm, Strength of the Tajuru and Reap What Is Sown preserve target
reads and cursor advances around callbacks. Shambling Swarm clears its full
302-int marking array with memset. Eye of Doom preserves its separate marking
and destruction passes and cached player values. Thornbite Staff and
Soulcatcher's Aerie retain their GFP/attachment/cleanup ordering. Veterans'
Armaments still queries its equipment twice during activation. Necropolis of
Azar retains its trigger loop, and Ruins of Oran-Rief retains its repeated
target updates. Larger Revoker, Marath, Sigil, planeswalker and modal AI paths
retain their control flow under local substitution. All twenty affected files
have no remaining placeholder locals matching the cleanup pattern. The batch
applicator and source-reader registry now accept the existing hyphenated
Conspiracy source filename. The single incremental build and all 15,169 fixed
jump-table slots pass, along with affected source/evidence pins, whitespace
and staged-index preservation checks. See
[batch record](recovery-evidence/counters-tribal-commander-readability-cleanup.json).
No new native coverage or gameplay-equivalence claim is made. Full audits,
inventory regeneration and reccmp refreshes remain deferred.

The engine/exile/text readability batch covers the final 45 previously
untouched entries from the C496 cleanup census: 13 card handlers and 32 helpers
across twenty-two files. Exact rename token checks cover 34 definitions;
focused control-flow review covers eleven. The batch removes 1,771 placeholder
references and 24 LAB jumps. It reconstructs the text-copy wrapper's empty-loop
gate, deadbox reanimation scan, attacker unlinking scan, exile CMC ownership
tests, Monstrosity activation gate, Song of the Dryads aura/type dispatch,
Titania GFP lifecycle, two shared Sliver ability lifecycles, Thief of Blood
scan and Savage Summonings event routing. Deadbox reanimation retains fresh
slot reads after classification calls and advances the slot before invoking
the reanimation callback. Exile CMC counting retains the paired-record pointer
comparison and its original twenty-bin counting bound within a twenty-one-bin
buffer. Text loading retains the existing clamp, trailing-row behavior and
file-cache fallback indexing. Larger parser, spell payment, cascade, mass-action,
control and modal paths retain their existing control flow under local
substitution. All twenty-two affected files have no remaining placeholder
locals matching the cleanup pattern. This closes the untouched-entry pass over
the older census; previously retained jumps still need reconstruction. The
single combined incremental build, all 15,169 fixed jump-table slots, affected
source/evidence pins, whitespace and staged-index preservation checks pass.
See [batch record](recovery-evidence/engine-exile-text-readability-cleanup.json).
No new native coverage or gameplay-equivalence claim is made. Full audits,
inventory regeneration and reccmp refreshes remain deferred.

The aura/selection/event-flow batch reconstructs control flow in 33 existing
definitions across twenty-six files: 21 card handlers and twelve helpers.
All 33 definitions now have no jumps or decompiler labels. The batch removes
53 gotos, including 51 LAB jumps, and 111 source lines. It covers aura upkeep
and casting dispatch, sacrifice scans, Clockwork and storage mana paths,
Coalition Relic, Scrying Glass, random permanent selection, Compost and
Grindclock AI paths, bestow, Tetravus, Sarkhan, Soulcatcher's Aerie, Descent
into Madness, Life/Death, Volrath and related event routes. Separate engine
queries, cached values and fresh reads after callbacks remain intact. Random
selection preserves host/client packets and its initial hand-count gate.
Life/Death retains the native empty-graveyard early return during resolution;
bestow retains the original distinction between characteristic-event routes
and routes gated by trigger context. Native quirks are preserved. The single
combined incremental build, all 15,169 fixed jump-table slots, affected
source/evidence pins, whitespace and staged-index preservation checks pass.
See [batch record](recovery-evidence/aura-selection-event-flow-cleanup.json).
This is readability reconstruction of already recovered functions, with no new
native-function coverage or gameplay-equivalence claim. Full historical audits,
inventory regeneration, reccmp refreshes and gameplay tests remain deferred.

The split/aura/counter-flow batch reconstructs 26 existing definitions across
twenty-one files: 23 card handlers and three helpers. All changed definitions
now have no jumps or decompiler labels. It removes 123 gotos, including 121
LAB jumps, and 580 source lines. Split/fuse dispatch retains half-selection
callbacks, target-slot offsets, priority snapshots, fresh half flags after
callbacks and distinct empty-graveyard behavior. Toil/Trouble retains its
fallback priority after failed arbitrary-target validation. Aura paths retain
attachment queries, regeneration checks, casting callbacks and upkeep behavior.
Kels and Atla retain source-presence gates, GFP/activation separation and
per-trigger work; Atla's legacy removal remains inside its per-Egg loop.
Gemstone Array retains repeated payment queries and direct mana production.
Sigil retains repeated equipment/instance queries. Set-PT legacy retains its
captured duration flags and source-leaves/upkeep removal ordering. Mimeoplasm
retains face-down selection handling and state cleanup. Star of Extinction
retains current-turn refreshes across damage callbacks, separate creature and
planeswalker damage tests, and mass damage after failed land-target validation.
Sacrifice and Bomb Squad scans retain cached next indices and live card-count
bounds. The single combined incremental build, all 15,169 fixed jump-table
slots, affected source/evidence pins, whitespace and staged-index preservation
checks pass. See
[batch record](recovery-evidence/split-aura-counter-flow-cleanup.json).
No new native-function coverage or gameplay-equivalence claim is made. Full
historical audits, inventory regeneration, reccmp refreshes and gameplay tests
remain deferred.

The modal/scan/suspend-flow batch reconstructs 22 existing definitions in
twenty-two files: fifteen card handlers and seven helpers. All changed
definitions now have no jumps or decompiler labels. It removes 98 gotos,
including 95 LAB jumps, and 329 source lines. Counter snapshots, contiguous
stored-power reads and clears, target-selection callbacks, mode packing and
AI priorities remain intact. The payment and test-score helpers retain their
original branches and bit masks. Exile bookkeeping and mass damage retain
cached next indices, live count bounds, fallback type queries and evolving
turn-order passes. Warp World retains its separate reveal and return passes
and pointer advances before put-into-play callbacks. Kira retains its distinct
trigger-target and top-stack scans, live/cached target counts and cleanup.
The aura, queued-source, draw-replacement, four-step animation and suspend
paths retain their original callback ordering and native quirks. A targeted
native inspection confirmed Relentless Dead reads its graveyard-position slot
before initialization on its no-black-mana/no-graveyard-shroud path (load at
0x022a91d7 within the function at 0x022a9130). That native quirk is preserved;
no initialization or stack-slot alias was invented. The batch record identifies
the native DLL and inspection output by hash. The single combined incremental
build, all 15,169 fixed jump-table slots, affected source/evidence pins,
whitespace and staged-index preservation checks pass. See
[batch record](recovery-evidence/modal-scan-suspend-flow-cleanup.json).
No new native-function coverage or gameplay-equivalence claim is made. Full
historical audits, inventory regeneration, reccmp refreshes and gameplay tests
remain deferred.

The next combined readability batch reconstructs 17 definitions (nine card
handlers and eight helpers) across 15 source files, removing 126 jumps and
487 source lines. It covers both card-test helpers, the Licids, Infinite
Reflection, Death's Presence's legacy, Brain in a Jar, Shared Discovery,
Tourach's Gate, Glissa, Phyrexian Revoker, Betrothed of Fire, text-stash and
rules-text parsing, exile permissions, multicascade and mass manipulation.
Native quirks in counter priorities, slash-token output, cascade indexing
and aura-bounce shifting are retained. Focused event/callback review and one
combined incremental build pass, including all 15,169 fixed entry slots;
affected source/evidence pins, whitespace and staged-index preservation
also pass. See [batch record](recovery-evidence/licid-parser-mass-action-flow-cleanup.json).
No new native-function coverage or gameplay-equivalence claim is made.
Full historical audits, inventory regeneration, reccmp refreshes and gameplay
tests remain deferred to milestones.

The final three definitions in the current recorded control-flow queue are now
structured: `card_edh`, `new_sacrifice_impl` and `generic_control_legacy`.
This batch removes 67 jumps and 317 source lines. EDH retains its partner and
Piper validation paths, temporary-card casting cleanup, all five commander
ability paths, two tax queries per eligibility probe, first-query zero-tax
fallback and live trigger context. Sacrifice selection retains optional/early
completion, AI-to-dialog fallback, target restoration and mark/kill ordering.
Control release retains cached mode flags, callback order and repeated state
writes. One combined incremental build and all 15,169 fixed entry slots pass;
focused source/evidence pins, whitespace and staged-index preservation checks
also pass. See [batch record](recovery-evidence/commander-sacrifice-control-flow-cleanup.json).
This completes that recorded cleanup queue, not whole-DLL recovery or gameplay
verification. No new native-function coverage or gameplay-equivalence claim is
made. Full historical audits, inventory regeneration, reccmp refresh and
gameplay tests remain deferred.

The numeric-storage follow-up changes 30 definitions (25 card handlers and five
helpers) across 19 original source files. Twenty-four definitions now use the
integer info_slot member for numeric values instead of function-pointer
arithmetic or integer-to-CardFunction casts; genuine legacy callbacks retain
the info_slot_fn view. Original DWARF identifies the three loop counters as
integer amount/amount/x, and four targeted native disassemblies support the
scalar operations. Their input hashes and DIE/local witnesses are recorded
in the [batch record](recovery-evidence/numeric-storage-odyssey-readability-cleanup.json).
The batch also removes 245 placeholder identifier occurrences, six jumps and
61 lines, including the missed Odyssey paths and the complete 151-byte inline
Breath of Dreams clear. Two alpha-only definitions receive exact token checks;
structured edits receive focused review. The combined incremental build,
all 15,169 fixed entry slots, affected source/evidence pins, whitespace and
staged-index preservation checks pass. A quick C/C++ source-pattern scan of
cards and functions finds no remaining matching placeholder names or union
keywords, but identifies 20 additional decompiler jumps outside the old
recorded queue; those are follow-up targets. This lexical scan does not count
union-typed locals declared through typedefs or prove whole-DLL completeness.
No new native-function coverage or gameplay-equivalence claim is made. Broad
historical audits, inventory regeneration, reccmp refresh and gameplay tests
remain deferred.

The expanded decompiler-label cleanup changes 20 definitions (14 card handlers
and six helpers) across ten original units, removing 45 jumps and 395 source
lines. It covers five kicker handlers, six MDFC fronts, Lifecraft Awakening,
Seize the Day, Jar of Eyeballs, the Vizkopa legacy, haste-effect creation,
per-card actions, the split-card lookup, Partner eligibility and counter
priority scoring. Named-card level/quest switches preserve the prior score
and threshold mappings; quest scoring still queries COUNTER_LEVEL. Existing
callback order, context refreshes, first-half/MDFC routing, flashback and
legacy -1 index/return quirks are retained. See the
[batch record](recovery-evidence/remaining-joined-counter-flow-cleanup.json).
One combined incremental build, all 15,169 fixed entry slots, focused
source/evidence pins, whitespace and staged-index preservation checks pass.
A fresh lexical scan of C/C++ files under cards and functions finds no matching
placeholder locals or LAB_/joined_r0x/code_r0x identifiers. This closes the
identified naming/label patterns, not all possible readability work or native
recovery. No new native-function coverage or gameplay-equivalence claim is
made. Broad historical audits, inventory regeneration, reccmp refresh and
gameplay tests remain deferred.

The native local-enum restoration closes the 290-declaration/730-name source
queue across 285 function owners in 82 units. A separate helper corrects
CHOICE_TOKEN_ONLY to native CHOICE_UNPRINTED, value 4, for a combined 286
edited definitions in 83 units. All restored enums are anonymous four-byte
native declarations, with exact names, ordering, values, source and function
ownership rechecked directly against the pinned original DBG. Removing the
inserted declarations and reversing that one alias rename reproduces the
previous implementation tokens. Existing source formatting and algorithms
are preserved. See [batch record](recovery-evidence/native-local-enum-restoration.json).
The combined build and all 15,169 fixed entry slots pass. The full 777-enum
census now has exact native names/values in current source (1,967 enumerators).
GCC emits 487 enums/1,237 names, independently checked in current DWARF; the
290 restored declarations are omitted while their implementations still use
numeric literals. Symbolic replacement of those numeric uses is separate work;
no artificial locals are introduced to force debug emission. The historical
alias receipt remains intact and its verifier recognizes the explicit later
name correction and directly validated source-only declarations. Focused
source/evidence pins, exact token projection, whitespace and staged-index
preservation checks pass. Broad historical replay, reccmp refresh and gameplay
tests remain deferred. No new native-function coverage or gameplay-equivalence
claim is made.

The first native choice-use pass replaces 731 numeric comparisons/case labels
with their exact native CHOICE_* constants across 223 definitions (216 card
handlers and seven helpers) in 61 original units. One primary dialog domain,
conservative reaching dialog-result locals and exclusively owned mode storage
select the uses. Secondary dialogs, mixed-purpose storage, escaped locals,
arithmetic, multiple domains and function/macro arguments are excluded.
Every replacement independently checks the exact local enum declaration and
an equal plain C-int literal; other implementation tokens and existing source
formatting remain unchanged. See
[batch record](recovery-evidence/native-dialog-choice-enum-uses.json).
The combined build, all 15,169 fixed entry slots, focused source/evidence pins,
whitespace and staged-index preservation checks pass. Current DWARF independently
checks 709 native enums/1,788 enumerators, 222 more enums than the preceding
build. The converted static fabricate helper has no other source reference in
its unit and remains unrepresented in debug. Exact native source declarations
remain checked for all 777 enums; 68 are omitted from current debug. The
remaining symbolic-use queue contains 62 owners with 67 as-yet-unused enums;
the already-symbolic fabricate declaration is tracked separately. No new
native-function coverage, runtime-byte identity or gameplay-equivalence claim
is made. Broad historical replay, reccmp refresh and gameplay tests remain
deferred.

The stored-mode enum pass now adds 300 equal C-int substitutions across 61
existing definitions (53 card handlers and eight helpers) in 42 original source
files. It covers repeated menus, byte-valued stored modes, packed choices,
zero-based legacy modes and separate graveyard trigger flag groups. Secondary
menus, numeric costs, target indices, AI priorities, cancellation values and
Thawing Glaciers' delayed-bounce sentinel 66 remain unchanged. Each replacement
is independently projected from its native name/value; all other implementation
tokens and source formatting are preserved. See
[batch record](recovery-evidence/native-stored-mode-enum-uses.json).
One combined incremental build passes, including all 15,169 fixed entry slots;
affected source/evidence pins, whitespace and unchanged staging checks pass.
All 777 native enum declarations/1,967 enumerators are present in source, and
current DWARF verifies 773 declarations/1,959 enumerators. The four debug
omissions are an unreferenced static fabricate helper, the graveyard life-gain
group already expressed as booleans, and two damage-effect enums whose decisions
are expressed as a boolean or have no numeric execution site. Their exact native
declarations remain in source without artificial uses. No outstanding owner
requires a symbolic-use pass from this queue. This is source readability and
native-name recovery, with no new body coverage or gameplay-equivalence claim.
Broad historical replay, reccmp refresh and gameplay tests remain deferred.

The post-enum comparison checkpoint refreshes all 18,725 configured function
entries with zero stubs. It retains 951 helper and 6,634 card ranges by local
instruction/storage evidence; 1,242 helper and 5,100 card ranges retain their
explicit reconstruction reviews, alongside 17 C++ and 34 compiler/header ranges.
Both recorded queues have zero pending ranges. Routine triage now uses the
current-source/name check instead of replaying deliberately retired snapshots.
Five false card differences were solely unprefixed short/near JE/JNE encodings
to the same destination ordinal. Triage now normalizes those internal branch
widths; conditions, exact destination ordinals, prefixes and other instructions
remain exact. Historical fingerprint behavior remains the default. Focused
checks reject changed conditions/targets, loops and prefixed encodings.

The following source batch restores three original helper interfaces across
eight definitions in functions/packs.c and functions/sacrifices.c, with 27 caller
updates. The pack workers now take the native eight-byte csvid_choices_t record,
including its card_id_t pointer member, instead of separate member addresses.
The pack insertion worker restores its rarity/expansion enum parameters. The
sacrifice prompt worker restores maxlen, supplied as the native witnessed 200
by all three callers. Independent implementation projections preserve other
tokens, native diagnostic strings, allocator reloads, random calls and duplicate
handling. One combined build and all 15,169 fixed entry slots pass; current
DWARF checks all three original signatures and the record member layout. Source
pins and staging preservation pass. See
[batch record](recovery-evidence/native-record-parameter-restoration.json).
The full comparison checkpoint predates these interface edits; it is not a
post-edit comparison. The original-signature screen has 43 further candidates
to classify, including wrapper/worker separation and compiler specializations;
these are not automatically runtime ABI bugs. No new body coverage, runtime-byte
identity or gameplay-equivalence claim is made.

The next original-interface batch restores 13 native contracts in 93 source/header
files. Twelve helper definitions and 55 helper call sites restore the embalm,
escape, replicate and spectacle color arguments, three Strixhaven token amounts,
the Havengul legacy callback and the target-source test parameter. Thirteen
concrete DWARF constant witnesses pin omitted color values to zero, token amounts
to one and the target-source mode to zero. Substituting those values reproduces
the prior implementation tokens. The unused replicate event formal comes from
the existing card event context; Havengul callers pass the original callback.

The public show_deck done-label parameter now has its native string-pointer type.
All 238 caller sites preserve their exact fixed addresses or existing string
pointers through EXE_STR, removing integer casts from literal and custom labels.
The raw EXE call still uses an explicit 32-bit pointer-to-word conversion. Public
expansion, card-chooser color/type and pack-flag parameters regain their native
enum types; the four pack-flag names and values remain exact. Both duplicate
show_deck header declarations and all affected forward declarations are updated.
Two legacy source files outside the current build receive matching argument
types and explicit current-source pins, with no compiled native coverage credit.

One combined build and all 15,169 fixed entry slots pass. Current owning-CU DWARF
checks all 13 restored function contracts and the anonymous four-byte pack enum.
Exact source replay, all 93 affected file/evidence pins, whitespace and unchanged
staging checks pass. See
[batch record](recovery-evidence/native-cost-token-public-interfaces.json).
The full comparison is deferred after this batch. The original-signature screen
has 30 further candidates, including deliberate public-wrapper/private-worker
separation and compiler specializations, rather than 30 newly discovered bodies
or established runtime ABI bugs. No new body coverage or gameplay-equivalence
claim is made; unobserved specialization domains remain outside these witnesses.

The GNU original-argument pass decodes call-site parameters linked through
DW_OP_GNU_parameter_ref, preserving values even after GCC removes their physical
argument slots. A reusable read-only extractor records 112 native call sites
and 89 original-parameter records for the 30 signature candidates. These are
source/debugger recipes, not a physical calling convention. See
[batch record](recovery-evidence/original-optimized-call-arguments.json).

The batch restores 26 source signatures and 78 calls across 18 original units.
This includes six level-up cards' literal base power/toughness pairs, forwarding
the existing level_up formals, omitted event/context parameters and mass
reanimation's zero counter/null-hook arguments. Surviving worker statement
tokens remain unchanged; av_free_spell reuses its original card formal after
unconditional assignment instead of declaring a local. Existing argument
expressions keep their order. Specialized domains and erased generic branches
are explicitly distinguished; no unseen callback behavior is invented. Three
return-type differences are explained by the already-correct buddy_creature,
alternate_legacy_text and type_text public wrappers, whose returns remain intact.

The corrected incremental build and all 15,169 fixed entry slots pass. Current
DWARF checks the 26 restored signatures and three public return contracts.
Canonical entry checking verifies 731 physical entries and five caller-owned
inline helpers. It now recognizes compound GCC suffixes such as .isra.0.part.0
while retaining exact canonical base names, recognized stages, uniqueness and
static code-linkage requirements. One existing target helper gains an explicit
compiler-clone allowance backed by its actual constprop entry. Source/evidence
pins, whitespace and staging preservation checks pass. Four differently
formatted forward declarations were repaired after the first build attempt.

One original-signature candidate remains: four Memorial call sites expose ai=1,
but Memorial to Folly omits that record. Its current behavior is preserved;
no arbitrary original argument is supplied. The full comparison remains deferred.
No new body coverage or gameplay-equivalence claim is made.

The shared-data batch restores seven native arithmetic objects to their original
source names and scopes. Four copied name-folding tables now use the original
external lc_unaccent_map; eight sentinel uses refer to txtfx_not_found. Two
private event states remain independent, the deck expansion override returns
to windows.c with its native expansion enum type, and the date buffer and pack
dialog template regain their original function-local identities. See
[batch record](recovery-evidence/native-shared-data-identities.json).

The combined GCC 9.3 build and all 15,169 fixed entry slots pass. A focused check
matches all seven objects' source identities, arithmetic shapes, complete
load-time bytes, qualifiers, linkage and section writability against the native
release. This caught and corrected the override's prior signed-int declaration.
The older arithmetic initializer report remains a historical snapshot; no full
comparison or historical audit replay was run. The master recovery ledger is
serialized compactly to remain below GitHub's per-file limit; its JSON content
and all evidence entries are preserved.

The data-declaration batch restores 32 native objects across 11 units. Twenty
AI byte banks regain char storage, including flat 14-byte card-count snapshots.
Four objects regain their original function-local scope or owning source unit.
Eight more declarations restore native constness, string-pointee types or
external linkage. Source consumers and initializers remain intact apart from
identifier substitution and the original static declarations. See
[batch record](recovery-evidence/native-data-declaration-restoration.json).

One combined GCC 9.3 build and all 15,169 fixed entry slots pass. Focused checks
match all 32 source identities, qualified types, array dimensions and linkage;
initializers match exact bytes or six string targets and 396 pack-table object
pointers. The original declaration screen covered 296 native static/global
objects. Nine apparent enum differences preserve every native name/value and
require no edit; the sleighted-color words tables already exist in both source
functions despite compiler-shared emitted storage. Aggregate field types and
runtime state remain outside this screen. Full comparison remains deferred.

The record-field follow-up removes three synthetic card-instance compatibility
unions. It restores the native four-byte anonymous extension union followed by
unused30, the direct mode byte, and the native unused0x124/unused0x128/unused0x12a
tail fields. The template initializer follows those declarations; both token
keyword uses now refer to the original uint32_t unused0x124 field. The genuine
native extension, info-slot and eot-toughness unions remain. count_and_card_t
also restores its card_id_t csvid member across eighteen collation tables. See
[batch record](recovery-evidence/native-card-collation-record-fields.json).

The combined GCC 9.3 build and all 15,169 fixed entry slots pass. A nested
record/pointer/array declaration screen compares 120 emitted objects with no
differences, allowing compatible additional enum names and limiting recursion
to eight levels. The one compiler-shared color table already has both original
source declarations. The complete 300-byte card template and all eighteen
collation arrays retain exact native initializer bytes; token statements change
only by field-name substitution. This is source/storage recovery, not a new
function-body or runtime-equivalence claim. Full comparison remains deferred.

The Memorial ai signature was revisited using concrete native parameter
locations, all five GNU call sites and the 2020 Dominaria card file. Folly's
original ai value remains absent; the earlier source only has card stubs.
No arbitrary source argument was inserted.

The private-record pass restores five original declarations: Karn's local
edh_data_t name, the engine's file-scope PotentialAttacker and ai_combat_data_t,
the complete windows.c inst_and_ext_t definition, and the original unused
dialog_args_t declaration. The two copied local combat-data definitions become
one shared record; the public displayed-instance pointer API keeps an opaque
typedef. All executable statement tokens remain unchanged, apart from Karn's
type-name substitution. See
[batch record](recovery-evidence/native-private-record-identities.json).

The discovery screen found 48 native project-owned named records. Forty-one
already matched; the five changes above explain the recoverable differences.
The other two are SafeBool specializations whose existing source templates and
enum operators are retained without forced uses. The combined GCC 9.3 build and
all 15,169 fixed entry slots pass. Focused checks match the four emitted records'
original owners, scopes and depth-eight nested field contracts; dialog_args_t
remains an unused source declaration, as no typed variable or formal references
it in the native debug data. No extra objects or debug-only probes were added.
Full comparison and historical replay remain deferred.

The renewed reccmp pass ranks full JSON instruction deltas instead of treating
prior body-review coverage as an absence of small source differences. The fresh
pre-correction comparison has 18,725 configured ranges: 12,280 exact, 261 effective
and 6,184 different. A reusable small_diff_queue.py tool identifies 494 candidates
with scores >=90% and at most eight changed instructions per side. Report/CSV
receipts are checked and stale binary snapshots are labeled explicitly. See
[batch record](recovery-evidence/reccmp-small-diff-corrections.json).

Thirteen source corrections restore native mana-color indices, GFP owner/grave
flags, regeneration and literal-prompt flags, forced token color, removal modes
and range bounds, Inquisitor Exarch's dialog event, and Foster/Genesis Wave text.
The combined GCC 9.3 build and all 15,169 fixed entry slots pass. Fourteen changed
instruction sites now match exact native numeric instructions or readonly strings
within fresh compiled function bounds. This is a focused delta check, not a new
full-match score or a statement that all remaining differences are harmless.

Equivalent masked shifts/byte-width tests and nonnegative byte-counter loop tests
were left alone. Oath of Nissa's apparent zero/99 mismatch was a moved-block
alignment artifact: both return blocks exist natively. No enum was globally
renumbered; GAA_NONSICK and the kill-code values already match native DWARF.

Routine chooser coverage now binds current source pins rather than obsolete
initial-recovery body hashes, retaining the original hashes and explicit
historical verification. Setup bounds caches bind input debug bytes, reader code
and inclusion mode, avoiding repeated parsing of unchanged inputs. The older
helper/card triage reports remain body-coverage snapshots; their reconstructed
classification does not close this finer instruction-difference queue.

The follow-up reccmp pass restores Dromar's native "Target creature gets -2/-2"
chooser label and resolves ambiguous C names by source compilation unit or an
exact public COFF entry on both sides. All 160 explicit address bindings are
checked against the actual fresh comparison, adding 103 formerly excluded
ranges and strengthening 57 existing pairings. Private split ranges and C++
overloads remain outside this automatic mapping rule. See the
[binding record](recovery-evidence/reccmp-source-bindings.json).

The combined build and all 15,169 fixed slots pass. The report now contains
18,828 ranges: 12,453 exact, 270 effective and 6,105 different. On the original
18,725 ranges alone, exact matches increase from 12,280 to 12,370, with none
losing exact status. Twelve of the fourteen corrected function owners now score
exactly; the other two improve but retain differences. The small-diff queue
falls from 494 to 399 through source corrections and better reference mapping.
Three lower nonexact scores were inspected: their instruction inventories differ
only in resolved call/callback labels, which changed grouped-diff alignment.
No gameplay equivalence is inferred from these scores. Each JSON comparison now
keeps its matching CSV snapshot so later metadata changes do not invalidate
historical queue inspection. Existing staged changes remain untouched.

The next JSON operand pass corrects ten functions across nine source files:
Azure/Jade Mage request native 3U costs; fe_chant passes its Swamp condition to
the native green upkeep argument; the permanent-subtype helper checks both
additional subtype cursors as unsigned values in 1..4095; and Golgari Germination
uses the extended GFP collector with its explicit skip-token flag. Null Brooch
sets tapped state directly and clears the mana marker without dispatching a tap
event. Deep Water sets the shared 0x40 mana-override flag for newly cast lands.
Lich's Mastery's chooser receives native EVENT_RESOLVE_ACTIVATION (0x72).
Rishadan Port and Sea Gate Wreckage regain DLG_AUTOCHOOSE_IF_1. Printed card rules
are not substituted for these release behaviors; in particular Jade Mage's
native cost is 3U. See the
[operand correction record](recovery-evidence/reccmp-operand-corrections.json).

The final GCC 9.3 build and all 15,169 fixed slots pass. Focused checks use fresh
DWARF function bounds to verify the actual callee identities, outgoing constant
arguments, upkeep slots, unsigned range branches, direct-tap marker write and
readonly chooser control strings for all ten functions. They caught and resolved
a duplicated source replacement and an incorrect event-name assumption before
this checkpoint. No global enum was renumbered. Source and definition hashes,
compiled receipts and existing staged changes are checked. A new full comparison
is deferred; the preceding 18,828-range report and its 399-item queue remain
historical snapshots after this build, not current scores or proof of remaining
behavioral equivalence.

The next caller pass restores eleven native call sites across seven functions.
check_battlefield_for_targettable_id and pick_targets_for_multidamage now use
is_protected_from with protected/source arguments in native order and IPF_TARGET
(5), replacing six calls to the deprecated card-specific restriction helper.
Thought Prison uses sp_ability_trigger with controller ANYBODY and type TYPE_ANY;
the old new_specific_spell_played call incorrectly supplied TYPE_ANY as a player
selector and used a different resolution path. Natural Order uses native
GS_SAC_PERMANENT_AS_COST (0x8000) with its existing green-creature test.
See the [caller record](recovery-evidence/reccmp-native-callers.json).

Three additional caller restorations improve source fidelity while preserving
the existing behavior for their operands: both Graven Dominator functions call
real_set_pt directly with mode 1, and Dispossess calls the native three-argument
choose_a_card interface. The native Graven marked-value target indexing is kept.
The two remaining deprecated protection calls belong to unbuilt recoded_cards.c,
which has no current emitted DWARF ranges; no unsupported rewrite of that file
or global removal of its helper is included.

The combined build and all 15,169 fixed slots pass. Fresh DWARF bounds, native
COFF callee identities, outgoing constants, chooser text and forbidden old
callee checks verify all eleven sites. The checks also distinguished Thought
Prison's native TYPE_ANY filter from TYPE_NONE before the final checkpoint.
Current source/definition pins and existing staging are checked. The full C622
comparison remains a historical discovery snapshot, and a new full comparison
is deferred. Repeated Aura event masks, compensated card-table offsets and
equivalent branch layouts were inspected without forced code-generation edits.

The subsequent full comparison covers 18,828 ranges: 12,466 exact, 270 effective
and 6,092 different. Thirteen of the preceding seventeen changed function owners
now compare exactly, and all 160 explicit source-address bindings are checked
against actual report addresses. No previous exact match is lost. The tiny-diff
queue contains 395 candidates. These are the scores before the two source
changes in the [filter/formatter record](recovery-evidence/reccmp-filter-text-fastpath.json).

Cruel Revival's graveyard selection now uses native TYPE_ANY (0xff) with its
Zombie subtype test, instead of restricting the selection to permanents. The
effect-card formatter regains its native strchr fast path for ordinary characters
before checking its nine format-token prefixes. All replacement rules, signed-zero
handling and buffer setup remain unchanged. Native operands and callees are
checked before editing; the final build and all 15,169 fixed slots pass. Focused
checks confirm the compiled filter, prompt, selector bytes and branch consuming
the strchr result, plus exact projections preserving all other source statements.

The repeated-call screen now stops at incoming branch targets. Shared call blocks
had produced false positives in Goblin Tutor, damage_effects and Timecrafting;
their native streams already contain the allegedly missing operands. The string
screen separately skips 75 diagnostic line-number differences. No source change
is made from a call-site constant count without checking the actual control flow.
Current source pins and existing staging pass. The full report is retained as a
historical snapshot after this latest build; no post-edit full score is claimed.

The [project-data comparison pass](recovery-evidence/reccmp-global-bindings.json)
adds 293 checked DLL-owned object bindings, including 51 function-local statics.
Source/scope/name identities, coarse storage contracts and nonoverlapping extents
select the pairs; the live comparator verifies their actual rebuilt addresses.
The local adapter uses the existing fork interface without modifying its checkout.
It preserves source-qualified labels for repeated data names and saves checked
global CSV/binding snapshots alongside each JSON report. Initializer equality,
complete record ABI, EXE-owned absolute globals and CRT data are outside this pass.

The fresh full comparison has 18,828 ranges: 12,467 exact, 271 effective and
6,090 different. All 160 explicit function bindings remain verified, the formatter
now compares exactly, and no previous exact match is lost. There are 393 tiny-diff
candidates. Relative to the prior report, 71 scores improve and 14 decrease; that
comparison combines new data normalization with the preceding two source edits.
Named operands can change diff alignment, so these scores order review work.

A whole-range global-reference screen of all 6,090 nonexact functions produces
one candidate, has_subtype_by_id. Native and rebuilt indexed operands both use
added_subtypes_not_in_play with a minus-five index bias. The raw displacement
lands inside different neighboring globals; interpreting it without its index
produces a false wrong-global report. Both full effective addresses are checked,
and no source edit is made. Historical JSON remains readable with its matching
snapshots and is correctly marked stale after a rebuild. No C/header input changes
in this pass; the successful build and 15,169 fixed slots are reused. Existing
staging remains unchanged.

The [next caller-mode batch](recovery-evidence/reccmp-caller-mode-corrections.json)
restores fourteen native call sites in seven cards and one helper. Abandoned
Sarcophagus, As Foretold, Lurrus and Fires of Invention now pass EVENT_ACTIVATE
while constructing activation choices, retaining EVENT_CAN_ACTIVATE in query
paths. Lost Legacy uses hand-only exile reporting instead of requiring every
copy. The Opposition Agent tutor destination grants mana-value payment instead
of free casting. Chaos Orb uses the native literal-prompt select_target call,
and Zedruu reuses the native ownership-count helper with the opponent and permanent
mask, preserving that helper's existing native behavior.

The widened constant screen examines 6,076 eligible nonexact functions and finds
eleven candidates. Five outlast findings are rejected: rebuilt GCC moves the
first three formals into registers and omits constant-zero color arguments, while
the native private helper uses stack arguments. Comparing matching stack offsets
without checking the private ABI gives false mana-cost differences. Hand checks,
protection dispatch and prowess differences are also retained where they reflect
inlining rather than a demonstrated source discrepancy.

One combined build and all 15,169 fixed slots pass. Fresh DWARF ranges and actual
callee identities verify 24 native/current sites, including all ten changed
activation-event operands and ten unchanged query-event operands. Eight full-file
source projections preserve every statement outside the recorded fragments.
Current source pins and existing staging pass. The C626 full report is now a
pre-batch snapshot; no post-batch whole-function score is claimed.

The [native buffer-initializer pass](recovery-evidence/native-buffer-initializer-restoration.json)
reconstructs five functions in three source units. The three council-vote cards
use ordinary initialized 100-byte text arrays; all 600 emitted text/padding bytes
match the native buffers before the council call, including the native Strenght
spelling. Breath of Dreams and Quagmire Lamprey replace expanded alignment peels,
word loops and tails with whole-array memset operations on their 151-byte and
2x151-byte marked arrays. Six private clearing-bookkeeping locals are removed.
Trigger, payment, vote and creature-iteration behavior remains outside the edits.

The build, all 15,169 fixed entry slots, fresh native/rebuilt array geometry and
three full-file source projections pass. The generic constant-frame analysis
cannot resolve alignment-dependent clears. A bounded instruction executor instead
runs each native/rebuilt clear region for all four low address-bit combinations:
sixteen runs cover the full typed objects and preserve neighboring sentinels.
Together with the text arrays, checks cover 1,053 buffer bytes. No code-generation
workaround is introduced to satisfy the analyzer; existing staging is preserved.

Discovery uses refreshed C627 bounds and binary hashes; C626 scores only order
review. A whole-function constant screen yields 625 candidates across 6,349
eligible ranges. A register-aware public-callee screen finds no additional complete
basic-block constant-argument discrepancies under its conservative model. Private
or unresolved layouts remain outside that result. A cross-unit public call-count
screen yields 1,129 signals; GCC's duplicated/shared branches mean static counts
alone cannot establish omitted runtime work. Leading predicate/mask/stride cases
are inspected without forced source edits. These screens do not close all remaining
instruction differences, and no new full reccmp score is claimed after this build.

The [shared-dispatch restoration](recovery-evidence/native-shared-dispatch-restoration.json)
restores native API reuse in eighty card owners: 55 basic_spell, 17 global_enchantment,
four generic_token, two flash, and one each for card_exploration and card_nantuko_shade.
The decompiled source had expanded those common implementations. Existing shared
helper definitions remain unchanged; native COFF names and actual call instructions
identify the intended delegation. These are eighty source-sharing restorations,
not eighty newly recovered handlers or independently established gameplay bugs.

The same batch corrects Takklemaggot: native is_this_dying precedes the death-removal
check. A positive removal check kills the Aura with KILL_STATE_BASED_ACTION (17)
and returns zero; the other path creates the legacy. The previous source reversed
the checks and omitted the kill/early-return path. Focused native/rebuilt conditional
next-call traces and the actual outgoing kill operand verify the restored behavior.

The second apparently absent mutating action, new_damage_all's damage_creature
call, is a compiler-worker false positive. Its fresh 55-byte public entry directly
jumps to a 557-byte same-source .part.0 range containing that call. No source edit
is made there. Absence checks must follow verified worker edges before treating
a missing entry-range call as missing behavior.

One combined build and all 15,169 fixed slots pass. Fresh DWARF public/worker
bounds and callee identities verify 81 emitted native-style API edges. Thirty-two
complete source-file projections preserve all unrecorded statements, six reused
helper definitions remain unchanged, and existing staging is preserved. No new
full reccmp comparison or transitive gameplay-equivalence claim is made.

The [mechanic-dispatch restoration](recovery-evidence/native-mechanic-dispatch-restoration.json)
recovers fifteen more shared native implementations across eleven source files,
including the shared-header prototype. Convoke, hybrid, Counterspell, Force Spike,
Fog, cycling, regeneration and mana-creature logic now reuse their existing APIs.
Iron Will and Rift Sower lose their compiler-shaped gotos. The Ikoria predicate
returns count_counters(player,card,-1), preserving a numeric total and activation
parent handling while removing raw aliased counter-byte reads. The token-haste
callback uses add_status with the native target player and invisible-effect bit.

Teach by Example preserves three raw native routes: basic_spell on the cast query,
card_doublecast on resolution, hybrid otherwise. The existing Doublecast function
receives its public three-argument declaration. Its reuse here is resolution-only;
no whole-function exactness is inferred from its historical comparison score.

A combined build and all 15,169 fixed slots pass. Fresh emitted bounds/callee
identities verify 21 edges. Five native/rebuilt selectors agree for all 149 distinct
native event enum values plus neighboring/sentinel inputs (322 event integers),
both players and four affected-card/controller combinations: 12,880 cases, including
outgoing cost operands. Six total-counter forwarding cases and four legacy-status
controller cases pass, with matching native/rebuilt token-record fields. Fifteen
shared helper definitions remain unchanged, eleven full-file projections and
current source/name pins pass, and existing staging is preserved.

Four suspicious native call-count cases are deliberately left behaviorally alone.
Shape of the Wiitigo's outer event gate excludes EVENT_DECLARE_BLOCKERS, so its
physical blocking call is not executed there. Three physical land-sound branches
are likewise unreachable on resolution; the actual native/current route calls
mana_producer. The bounded selector checks preserve these paths rather than
activating dormant native code or bypassing the real mana fallback. Selected
helpers/prefix stubs are not whole-gameplay simulations. The subsequent full comparison records the current snapshot below.

The post-batch full comparison covers 18,828 ranges: 12,527 exact, 273 effective
and 6,028 different. All 160 explicit function bindings and 293 actual data matches
are verified against the input-pinned report. Relative to the first C626 data-binding
snapshot, 105 scores improve, three decrease, sixty become exact and no previous
exact match is lost. The four source batches affect 108 distinct owners (Teach by
Example occurs twice); ten of the latest fifteen owners now compare exactly.
The current tiny-diff queue has 395 entries. Improved functions can enter that
threshold queue, so candidate count alone is not a remaining-bug measure.

Arguel's Blood Fast, Thran Temporal Gateway and Iron Will score lower. Iron Will's
2,576 event/affected-card combinations and outgoing operands agree with native code;
Arguel's retains the checked native enchantment delegation and all other source.
Game Plan and Thran's function bodies are unchanged from the pre-C629 snapshot,
although their scores move after their units are rebuilt. Those observations do
not close their residual full-body differences or justify code-layout steering.
The full snapshot describes the current build, and existing staging is preserved.

The next [tiny-diff pass](recovery-evidence/reccmp-tiny-return-correction.json)
adds review-pattern hints to the JSON queue without excluding any candidate.
Changed branch conditions remain in the operand/control-flow group. It restores
Rakdos Pit Dragon's native zero return after its +1/+0 pump action; the earlier
source returned the helper's effect value. All seven outgoing arguments remain
unchanged. One incremental build, all 15,169 slots and 24 bounded native/rebuilt
caller cases pass, and existing staging is preserved. The source projection
contains only that return removal. The refreshed full comparison now
covers 18,828 ranges: 12,528 exact, 273 effective and 6,027 different, with 394
tiny-diff candidates. Rakdos Pit Dragon becomes exact and every other function
retains its prior score. All 293 actual data bindings pass. The current pattern
counts are 19 assertion-line, 42 retarget-comparison-order, four instruction-order,
74 branch-layout/padding, seventeen callee/address-mapping and 238 operand/control-
flow signals. These hints do not establish equivalence or remove any candidate.

The [target/trigger dispatch pass](recovery-evidence/native-target-trigger-dispatch.json)
restores native caller interfaces in Betrayal of Flesh, Maze of Ith, Slave of Bolas
and Chrome Mox. The first three use the native explicit player/card validator and
target index zero. Chrome Mox selects optional/mandatory mode through
RESOLVE_TRIGGER_AI(player) before entering etb_ability, preserving the native
duh_mode observation after the cost-query checks. This restores the evaluation
location; the previous deferred mode was already understood by the shared helper.
No new gameplay fix is inferred from these call-count differences.

One combined build and all 15,169 fixed slots pass. Eight bounded native/rebuilt
validation routes preserve player, card and target index. Chrome Mox's four
outgoing trigger operands and prefix call order agree for 322 native event and
neighbor/sentinel integers, both players, speculation/trace states, eligibility
results and explicit duh_mode results: 15,456 cases. Four shared definitions remain
unchanged, three full-file projections and current source/name pins pass, and
existing staging is preserved. The focused caller stubs do not simulate gameplay.
The prior full comparison is now a pre-batch snapshot; no further full refresh is
needed to establish these recorded caller contracts.

The [split-card ID map](recovery-evidence/native-split-id-table.json) reconstructs
one existing helper as 75 named cases, with fifteen fusable combined-ID returns.
Its 225 card enumerators are checked against release DWARF. The 11,074-byte
decision tree becomes an 8,114-byte switch, removing compiler-shaped arithmetic
and branches without adding handlers or changing half-selection rules.

The release function, actual rebuilt DLL function and standalone named source
agree in 983,145 calls: every 16-bit card ID with fifteen zero/nonzero, low/high-bit
and signed flag values, plus seven additional signed/out-of-range IDs. The helper
has no external calls. The harness maps the DLLs without resolving imports or
running their entry points and calls only that integer helper. The complete
source-file projection, combined build, all 15,169 fixed slots and current source
pins pass; existing staging is preserved. This is a bounded functional check,
not a whole-program equivalence claim. The last full reccmp report is still a
pre-batch snapshot.

Broader discovery also checks 131 unique nonlocal scalar/array initializers with
no differences. Four Prismatic Vista activation-query routes reject its static
constant-call-count signal without a source edit. The preliminary counter-selector
signal was a stale-address mapping artifact; fresh DWARF/COFF locates the correct
helper. The refreshed partial-argument scan covers 6,276 functions. Unknown call
operands, private ABIs and record/pointer initializers remain separate review work.

The [DFC/counter lookup pass](recovery-evidence/native-dfc-counter-id-tables.json)
flattens both native face-ID helpers into named switches for 294 paired fronts
and backs. Outputs are captured independently from both release functions; the
inverse relationship is checked rather than assumed. The two 951-line decision
trees become 596-line switches. Eighty-four numeric card cases in the counter
map gain their native names. All 776 selected enum values match release DWARF;
the existing 721 nondefault counter mappings remain unchanged.

The release DLL, actual rebuilt DLL and standalone named source agree in 196,629
calls over every 16-bit card ID and seven additional signed/out-of-range IDs for
all three helpers. The call-free routines are invoked in DLLs mapped without
import resolution or entry-point execution. The build, all 15,169 fixed slots,
two full-file projections and current source/name pins pass. Existing staging
is preserved. This is source reconstruction and bounded lookup verification,
not new card implementation or a gameplay bug fix.

The [completed-edit archive](recovery-evidence/completed-source-edit-archive.json)
moves 18,964 finished projection records into a tracked, lossless gzip JSON.
Round-trip equality and byte-exact reconstruction of the prior entire manifest
pass before writing. The active manifest falls from 104,792,543 to 1,049,740 bytes;
the current 86 projections, the live Windows-hook receipt, 171 source pins and
all canonical identities remain active. Routine verification checks the archive's hash without decompressing
old records. This compacts evidence; it does not discard source or proof data or
re-enable the already retired historical compiler/runtime audit.

The full post-lookup comparison now describes the current build: 18,828 ranges,
12,531 exact, 273 effective and 6,024 different, with 394 tiny-diff candidates.
Seven scores improve relative to the tiny-return snapshot and none decreases.
Chrome Mox and both face-ID helpers become exact. All 160 explicit function
bindings and 293 actual data matches pass. Scores remain normalized assembly
classifications; the lookup execution checks separately establish the recorded
functional domain. The compacted manifest and its retained live hook receipt
pass routine setup and all fixed-slot checks.

The [test-score control-flow pass](recovery-evidence/native-test-score-control-flow.json)
reconstructs get_test_score and new_get_test_score as field/mode switches with
unsigned bit accumulation. Their combined source shrinks from 6,163 to 3,925
bytes. Cached mode variables and inverted-CMC arithmetic are removed. Native
differences remain explicit: legacy positive subtype modes versus new mode 1,
legacy nonzero versus new positive IDs, legacy type mode 5 versus new zero-color
mode 3, and the ownership bit shared with the upper toughness mode. The K'rrik
and Jasconian Isle cases also gain their release-verified card names.

Direct release DLL, actual rebuilt DLL and standalone source agree in 997,102
cases. These include 2,400 legacy and 787,500 record-based representative field
classes, 100,000 deterministic boundary/random cases per scorer and 7,202 display
bit/pointer fixtures. Native/current test records have matching member geometry
and size 216; card instances have size 300. The display predicate remains unchanged
after matching every flipped bit over three baselines and pointer-identity cases.
The source-name cases retain exact native numeric values. This does not exhaust
all possible records or whole-gameplay states. One build, all 15,169 fixed slots,
two full-file projections and current source/name/archive pins pass, with existing
staging preserved. The last full comparison is now a pre-batch snapshot.

The [card-ID naming pass](recovery-evidence/native-card-id-references.json)
names 628 existing numeric references in 422 functions across 90 original source
files. All 351 selected enumerators match release DWARF and compiler-evaluated
literal values. Independent projections verify that only the selected numeric
tokens change. All 90 allocated-object code/data/symbol/relocation snapshots and
the DLL's runtime sections remain identical, excluding debug/export metadata.
The build and all 15,169 fixed slots pass separately. Source/name/archive pins
pass and existing staging is preserved. This is identifier recovery, with no new
handlers or runtime changes; the last full reccmp report remains historical.

The [next full JSON refresh](recovery-evidence/reccmp-small-diff-refresh.json)
includes the score and card-ID naming passes. It retains 12,531 exact, 273
effective and 6,024 differing ranges, with all 160 explicit function entries and
293 data bindings verified. The two scorer scores decrease after their readable
switch reconstruction; the independent 997,102-case behavior evidence remains
the recorded functional check. The queue now groups repeated textual deltas,
retaining every candidate and its original order. The default 394 candidates
form 300 groups. Without the percentage-score cutoff, 617 candidates form 440
groups, still bounded to eight changed instructions per side. Grouping masks
only branch destinations and comparator-local OFFSET numbers and makes no
equivalence or completion claim.

The expanded queue led to a named reconstruction of
[get_partner_with_id](recovery-evidence/native-partner-id-table.json) in its
original source. All eleven native mappings remain, including Krav-to-Regna
without a Regna-to-Krav reverse case. The -1 default is preserved. One build,
all 15,169 fixed slots, complete source projection and 65,543 direct release/
actual-rebuilt/source cases pass. The rebuilt instruction sequence and complete
118-byte table also match after relocating the table/branch base. This is one
existing helper reconstructed, not eleven new handlers. Existing staging is
preserved. The full report above precedes this partner-table edit, whose compiled
verification is recorded separately.

The [counter/commander decision pass](recovery-evidence/native-counter-decision-control-flow.json)
flattens five counter-card trees and the 18 commander exceptions, expands 36
Boolean-mask priority expressions into explicit choices, and names counter/type
checks using 165 release-verified enum values. The counter scorer shrinks from
1,827 to 1,602 lines. Native quest-level queries, growth-counter rejection,
owner-sensitive priorities, hand-count read ordering and the commander card-ID
reload remain intact.

One combined build, all 15,169 fixed slots and full projections of both original
source files pass. Direct native/actual-rebuilt/old/new-source checks agree on
13,241,664 counter fixtures and 1,048,576 commander fixtures, including ordered
leaf-helper argument traces. A separate native-backed map extraction covers
458,752 fixtures. The DLLs run without entry points/import resolution, with
explicit leaf-call/data fixtures; these results establish the recorded caller
domain rather than engine behavior or all game states. Staging is preserved.
The last full JSON comparison precedes this batch; no full historical audit is
replayed.

The [typed Boolean-choice pass](recovery-evidence/native-boolean-choices.json)
reconstructs 173 explicit two-way values in 138 existing functions across 62
original files. It preserves each expression's int/unsigned-int type and both
Boolean outcomes, including unsigned wraparound; selected signed arithmetic
cannot overflow. GCC executes 346 typed outcome checks and verifies 173 type
contracts in their original scopes. Assertion/stringification/preprocessor
contexts are excluded, and all source line counts are preserved.

One combined build, all 15,169 fixed entries, 62 full-file projections and source/
name/archive pins pass. Twenty-eight allocated runtime objects are identical;
34 have different code generation. This pass establishes typed expression
equivalence to the predecessor source, not whole-function native equivalence
or new card handlers. It does not chase instruction-layout scores. Existing
staging is preserved and the latest full reccmp JSON remains historical.

The [counter-wrapper pass](recovery-evidence/native-counter-wrapper-control-flow.json)
reconstructs nine existing placement/cost/add/remove/clear wrappers, reducing
154 lines to 91. Four blocker-card IDs and counter/type constants receive their
release-verified names. The remove-all sentinel explicitly uses the low byte.
The shared counter-storage core is unchanged. Positive/negative/zero costs,
predoubled guard ordering, activation-parent redirection and native callback
amount-discard behavior remain intact.

One combined build, 15,169 fixed entries, full source projection and current
source/name/archive checks pass. Ten interfaces agree in 360,502 native/rebuilt/
old/new fixtures, comparing four complete 300-byte records per fixture, globals,
returns, aborts and ordered leaf-helper traces. Fixtures cover slot reuse and
overflow, saturation/wraparound boundaries, callbacks that alter globals/counter
bytes, all 16-bit placement IDs and low-byte sentinel aliases. Engine helpers are
stubbed; this does not establish all gameplay states. The predecessor cost worker
used a checked hoisted-guard adapter; both compiled callers had a dominating
nonzero guard. The new build emits the full cost body without that adapter.
Existing staging is preserved. The last full reccmp JSON remains the earlier
snapshot.

The [next comparison refresh](recovery-evidence/reccmp-counter-refresh.json)
contains 18,828 ranges: 12,538 exact, 274 effective and 6,016 different. Seven
ranges become exact and one effective, with no existing exact lost. Sixty-six
scores rise and ten fall. All 160 explicit entries and 293 data bindings pass.
The tiny queues now contain 393 default candidates in 299 groups, or 612
candidates in 435 groups without the percentage cutoff. Three fixed external
forwarders have identical decoded targets despite zero matching scores, so
external-target identity needs comparison-tool review before further decompilation.

The [residual-choice pass](recovery-evidence/native-residual-choices.json)
then recovers nine remaining pack/ward/mana expressions in six existing functions
across five files. Both Boolean outcomes, exact integer type and single evaluation
are preserved. One pack expression uses a checked preceding assignment that
restricts its input to 0/1. The artifact/colorless names match release DWARF.
Eighteen typed outcome checks, nine scope type contracts, five full-file
projections, unchanged line counts, one build and 15,169 fixed slots pass.
Existing staging is preserved. The full comparison above predates this final
expression batch; its pinned receipt remains the discovery snapshot.

The [fixed-engine identity pass](recovery-evidence/reccmp-external-identities.json)
validates 64 native COFF function identities at the same absolute addresses in
both debug files and executable storage in the pinned release Magic.exe. Ten
rebuilt assembler symbols are untyped; this does not validate their prototypes.
Lookup labels add no DLL entities or inferred ranges. All 18,828 native metadata
rows retain their names and bounds; 117 explicit rebuilt mappings refresh after
the preceding source build. The same-binary control compares all ranges, with
2,047 referencing the new labels. Of 266 changed scores, 259 rise and seven fall;
23 become exact and none lose exact status. The falling-score functions reference
the same external address sets on both sides; their score changes alone establish
no new source defect. Full baseline diffs remain in pinned local report sidecars.

The fresh report contains 12,561 exact, 274 effective and 5,993 differing ranges.
Tiny queues contain 374 default candidates in 299 groups, or 590 candidates in
435 groups without the percentage cutoff. Focused positive and negative parser
controls pass, including distinct callees, unknown/indirect addresses, data
exclusion and conflicting identities. No C/header source changes or new build
were needed. Current source checks and all existing staged changes are preserved.

The [target/byte walk pass](recovery-evidence/native-target-walks.json) reconstructs
five existing handlers selected from that JSON: Deepglow Skate, freeze-on-damage,
Venomous Fangs, Rakish Heir and Oona's Blackguard. Three loops use direct target
values in place of duplicate pointer aliases. Rakish Heir names its two-byte
damage-source encoding without casts from pointers to integers; Oona's counters
use explicit low-byte access while retaining modulo-256 wrapping and upper bytes.
Event gates, helper arguments, dynamic bounds and callback order are preserved.

The release and actual rebuilt DLL agree over 983,040 bounded fixtures comparing
ordered helper arguments, return values and complete card/extension records.
Leaf helpers are stubbed, including callbacks that mutate remaining loop bounds;
this does not establish unrestricted gameplay equivalence. Another 3,276,800
packing cases, five complete source projections, the build and all 15,169 fixed
slots pass. All five objects generate different code, so no byte-identity claim is
made. The full reccmp report above remains the pre-edit discovery snapshot; its
regenerated queues explicitly have `inputs_current: false`. Existing staging is
preserved and a further full comparison is deferred.

The [target-mask naming pass](recovery-evidence/native-target-masks.json) restores
1,152 release-valued enum expressions in 671 existing functions across 102 source
files: 529 zones, 348 special restrictions, 158 required/illegal states and 117
required/illegal type masks. All thirty selected enum names come from release
DWARF. Each replacement is compiled in its original scope to check the receiver
record, exact integer type and numeric value. Only literal tokens change, and
all source line counts are retained. Conditional/preprocessor functions and
unresolved contexts remain outside this batch; zeros retain their plain spelling.

The combined build and all 15,169 fixed slots pass. All 102 allocated objects
retain identical code/data bytes, relocation targets and runtime symbols. DLL
runtime sections, including relocation data, also remain identical to the prior
target-walk build; debug/export metadata are excluded from that section comparison.
This preserves the runtime covered by the preceding focused execution checks.
Full-file and independent literal-range projections both pass for all 102 files.
These are named operands in existing functions, not new handlers or newly
decompiled bodies. Existing staging is preserved; a full reccmp rerun is deferred.

The [event-guard review](recovery-evidence/reccmp-event-guard-review.json) checks
42 swapped `EVENT_CAN_CHANGE_TARGET`/`EVENT_CHANGE_TARGET` predicate sites against
the actual release and current DLL instructions. The two equality constants and
their complement partition the full 32-bit input domain. An instruction-level
guard interpreter checks 31,491 cases, including every incoming byte pattern,
matching continuation addresses and dead scratch/flag state. Default stack-ABI
public C calls in other source units are explicit liveness boundaries. This is a
local predicate review, not execution of the full handlers or a gameplay proof.

The queues retain all 374 default or 590 all-score candidates, with identical
scores, changes, ordering and groups. Forty-two sites now have current, pinned
annotations, leaving 332 or 548 candidates without that specific site review.
Positive and negative guard controls and both queue-integrity checks pass;
changed inputs, duplicate identities, wrong reports and changed sites are refused.
No C/header edits or rebuild were needed; staging is preserved. The older full
report remains marked as a discovery snapshot against the newer build.

The [card-data address review](recovery-evidence/reccmp-card-data-address-review.json)
checks 18 additional difference sets in existing functions. A table-base increase
of 32 or 48 bytes cancels an equal field-offset decrease. Release and rebuilt
DWARF agree on the 72-byte card-data record and its id/static/extra-ability fields.
The CFG review follows 3,899 register/stack relation states, 21 affected accesses
and 24 spill operations in 11 functions. Fixed frame objects delimit address-taken
locals; their declared bounds and the GCC i386 calling convention are assumptions.
Four named public C helper boundaries discharge caller-saved temporaries. Other
callee/global correspondences and whole-handler behavior remain outside this
local review. Shirei's mixed instruction reordering remains unresolved.

Eight positive controls and twenty deliberately invalid address/alias/control-flow
cases pass, as do six rejection controls for review annotations. Both queues keep
the same candidates, scores, ordering, changes and groups. Combining the event and
address reviews annotates 60 difference sets: 314 default or 530 all-score
candidates still lack an annotation. These counts concern this tiny-diff queue,
not all unresolved functions. No C/header edits or rebuild were needed; existing
staging remains intact.

The [diagnostic-line review](recovery-evidence/reccmp-diagnostic-line-review.json)
checks another 19 candidate difference sets at 28 pointer-store sites. Reading
both DLLs confirms identical paths and complete assertion expressions, with only
the decimal source line changed. The store encoding also agrees apart from its
string address. Native/current entries use exact COFF identities, including two
compiler clones; display suffixes are not treated as source identities.

These are diagnostic-only annotations, not byte or whole-handler equivalence:
the failure message's line number still differs on the assertion path. Four
positive controls, thirteen invalid diagnostic/instruction cases and six invalid
annotation cases pass. Both queues preserve all earlier annotations, candidates,
scores, ordering, original diffs and groups. The cumulative annotations cover 79
candidates, leaving 295 default or 511 all-score candidates unannotated. The
older full comparison remains a discovery snapshot; no C/header edits, rebuild,
full comparison rerun or staging changes were needed.

The [direct-CFG review](recovery-evidence/reccmp-branch-layout-review.json) checks
121 more candidates against the current successful build: 49 branch-layout,
34 callee/address-mapping and 38 other discovery patterns. A paired CFG walk
matches 9,833 relation states and 814 per-function operand bindings. It skips
architectural no-ops and direct internal jump trampolines, permits paired inverse
branch predicates, and follows both edges. Shared and duplicated return tails
can correlate without imposing the compiler's block order.

Ordinary operations, registers, widths, prefixes and edge predicates must agree.
Relocated operands require exact COFF names, pinned source/name function bindings,
PE import identities, or equal read-only literal contents. Short DIALOG tokens
use the pinned header's declared encodings. These are comparison identities;
callee implementations, data contents, literal pointer identity and aliasing
remain assumptions. Only entry-reachable direct edges and normal call returns
are covered; indirect jumps, exception/unwind edges and arbitrary return-address
inspection are outside this review. It does not certify whole-handler semantics.

Ten positive CFG controls, eighteen invalid/unsupported CFG controls, nine literal
controls and six annotation-rejection controls pass. Both queues retain every
candidate, raw score, original diff, textual group, ordering and earlier annotation.
The cumulative annotations cover 200 of 590 all-score candidates (390 unannotated)
or 150 of 374 default candidates (224 unannotated). The full instruction snapshot
remains historical; current native/rebuilt ranges are pinned separately. No game
source edits, rebuild, full comparison rerun or staging changes were needed.

The [broader direct-CFG pass](recovery-evidence/reccmp-broad-cfg-review.json)
applies the same model to all 5,993 differing ranges in the discovery report,
using a 50,000-instruction selection ceiling (the largest changed side is 4,173).
It finds 343 normalized CFG correspondences: the prior 121 results agree exactly,
plus 222 additional ranges, including 164 card-named ranges and 58 others. The
combined walk covers 29,684 relation states. Larger examples include Invasion's
apprentices, Liliana of the Veil, Aetherworks Marvel and Prince of Thralls' effect.
These are comparison results for existing source, not newly recovered handlers.

Combined with the earlier local reviews, 422 of the 5,993 differing ranges now
have annotations; 5,571 remain unannotated. The tiny queue remains separately at
200 of 590 annotated (390 unannotated), or 150 of 374 default candidates (224
unannotated). Every raw score, difference, candidate, ordering, textual group and
prior local annotation is retained. The report remains its historical snapshot;
matched current ranges and source/debug inputs are pinned in the new evidence.

The model and its earlier positive/negative controls are unchanged. Current queue
integrity and exact agreement of all 121 prior CFG results are checked separately.
The complete new review is stored as a pinned gzip archive to avoid duplicating
a large JSON receipt. Callee/data/literal-identity assumptions and normal-return,
direct-edge limits remain. Five narrow branch cases still need switch-table or
zero-byte/empty-string reasoning. No code edits, rebuild, full reccmp rerun or
staging changes were needed.

The [DIALOG token naming pass](recovery-evidence/native-dialog-token-names.json)
names 1,051 exact flag arguments in 432 existing source definitions across 86 C
files. It uses ten flag names, including 47 `DLG_MODE` uses. The public mode macro
and its documentation expose the existing native `DLGIMPL_MODE` value 27 and
parser case; they introduce no new dialog behavior. Native DWARF provides the
flag enum values. Argument-taking flag sequences and other contexts remain
outside this object-like flag pass.

Thirteen compile-time contracts check each public object-like token's exact
array type, size, three bytes including NUL, and native enum value. Full-file
edit replay and independent raw byte-range projections agree for all 87 changed
files. All 86 C files preserve their original line endings and line counts. The
combined build and all 15,169 fixed entries pass; every one of the 168 compiled
objects retains identical allocated code/data, relocation targets and runtime
symbols. DLL runtime sections, including relocation data, are also identical to
the prior successful build; debug/export metadata are excluded. This preserves
the compiled runtime covered by the earlier focused native/rebuilt checks.

The CFG reader now accepts the declared native mode token alongside the other
DIALOG flags; actual native/rebuilt read-only data both contain its exact byte
encoding. Its other CFG rules are unchanged. Earlier comparison/annotation
receipts retain their historical input hashes after this source/debug rebuild;
regenerate them before reusing the annotation commands. Runtime identity carries
forward their machine-code observations, without claiming a fresh comparison
score or whole-handler equivalence. No full reccmp rerun was needed, and existing
staging remains intact.

The [DIALOG argument-macro pass](recovery-evidence/native-dialog-argument-macros.json)
restores 101 flag-and-payload groups plus five local token selections in 51
existing definitions across 32 C files. Named forms now express mana costs,
targets, who chooses, headers, card displays, choice pairs and storage locations.
All original argument expressions and ordering remain; the header is unchanged.
Combined with the preceding object-token pass, no recognized unprefixed two-byte
DIALOG token string literals remain in the authored C source census. Concatenated
or other spellings, C++ and headers are outside that census.

Thirteen compile-time macro contracts check flag bytes/types/native enum values,
arity and representative scalar argument order/types. Full-file edit replay and
independent raw byte-range projections pass for all 32 files; original line counts
and endings are preserved. The single combined build and all 15,169 fixed slots
pass. All 168 runtime objects and DLL runtime sections remain identical to the
preceding successful build, including code/data bytes and relocation targets.
Debug/export metadata are excluded from that runtime section comparison.

The earlier focused runtime and machine-code observations therefore remain
applicable to the same compiled runtime. Input-pinned comparison receipts remain
historical after the debug/source rebuild and need regeneration before reuse.
No full reccmp rerun or gameplay change was needed; existing staging is preserved.

The [event/record cleanup](recovery-evidence/native-event-record-control-flow.json)
structures Dream Trawler's event dispatch, uses Urza's Saga's typed chapter flags
and names its activation priorities, and expresses Verdant Command's native
integer-array byte-fill. The release's unreachable Dream Trawler reset and
Verdant Command's `0x01010101` values and uninitialized human-path priorities
remain intact. These are three existing definitions across two source files.

The combined build, all 15,169 fixed slots and twelve ABI/constant contracts pass.
Actual native/rebuilt Dream Trawler and Urza's Saga machine code agrees in
458,752 post-change fixture comparisons (the same count also passed before the
edits). The fixtures execute the real record-access helpers, stub other leaves,
and compare call traces, return values, complete fixture card records/extensions
and AI/fizzle state. This is bounded caller evidence, not full gameplay testing
or a whole-handler verdict for Verdant Command. This batch can change generated
runtime code; the preceding byte-identity claims do not extend to this build.

The [renewed JSON triage](recovery-evidence/reccmp-tutor-return-guards.json) uses
the event/record build as a fresh discovery snapshot: 18,828 ranges, with 12,561
exact, 274 effective and 5,993 different. There are 369 candidates at the default
90% score cutoff and eight changed instructions per side, or 585 without the
score cutoff. Fresh event/address/diagnostic/CFG reviews annotate 200 of those
585 candidates; 385 remain unannotated. An annotation covers its stated changed
sites and assumptions, not whole-function or gameplay equivalence.

The same report exposed three tutor loops that treated only `-1` as failure,
whereas the native code stops for every negative return. Sarkhan Unbroken,
Drafna's Restoration and Denying Wind now use `>= 0` in their original source
files. Denying Wind came from a broader instruction-delta scan of the same JSON.
One combined build, all 15,169 fixed slots and inspection of all three rebuilt
tutor-call/sign-branch sequences pass. Other negative returns' practical gameplay
impact has not been established. Byte-bounded loop guards and layout-only
differences were left alone.

The discovery JSON and its annotation counts describe the build before these
three corrections. No second full comparison was run. Regenerate setup before
the next comparison because the DLL/debug input hashes changed. The latest
successful build receipt is the tutor-return pass; the preceding receipts retain
their historical input pins. Existing staging remains untouched.

The [native card-helper dispatch pass](recovery-evidence/native-card-helper-dispatch.json)
clarifies six existing definitions across five files. Petrified Field now accepts
only dialog choices 1 and 2 after handling cancellation, matching the native
default return instead of sending unexpected choices to mana production.
Implement of Improvement again calls `aer_implement` on its native fallback
events, and Spring // Mind uses `basic_spell` at both native call boundaries.
The other edits structure Inspiring Statuary's predicates, name Trench Gorger's
operations and Herald of Anafenza's cost/token fields, and remove temporary locals.
The casting-card capture, repeated aftermath queries, four-land AI limit and
zero-land effect creation are preserved.

One combined build and all 15,169 fixed slots pass. Source projections cover all
five files. Bounded interpretation of the native/rebuilt chooser instructions
agrees for 517 return-value cases on the next helper destination and cancellation
write; it does not execute callees or prove final handler return values/gameplay.
The two `basic_spell` boundaries and the shared `aer_implement` boundary are also
present in both actual binaries. The existing full JSON remains a pre-edit
discovery snapshot; no extra full comparison or historical replay was run.

The [subtype-record family pass](recovery-evidence/native-subtype-record-walks.json)
restructures nine existing helpers in `functions/has_creature_type.c` and removes
one unused compatibility wrapper. The report builder now traverses typed
eight-byte modifier records directly, replacing integer pointer casts and byte
offsets. Related add/force/query helpers use named flags and structured loops.
Native limits remain: only fifteen of sixteen modifier slots, first-invalid
termination, complete clears for forced subtypes, Grist's final Insect append,
and the ID query's second modifier pass and twenty-entry search. The live report
builder retains its native append bounds, including its possible overrun of the
26-entry report; fixture storage includes trailing canaries to observe those writes.

The combined build, all 15,169 fixed slots, twelve ABI/constant contracts and
source projections pass. Actual native/rebuilt code agrees in 81,920 post-change
fixture comparisons, with the same count passing before the edits. The fixtures
execute all nine bodies and selected real callees, substitute deterministic leaf
calls and four data objects, and compare complete output/card/extension/modifier
records, flags, ordered call arguments and integer results. Invalid live-card
abort paths and full gameplay are outside the fixture. No full comparison or
historical replay was needed, and existing staging is unchanged.

The [packed-byte access pass](recovery-evidence/native-packed-byte-access.json)
replaces 78 fixed-offset byte accesses in 66 existing definitions across 43 C
files. Existing `SET_BYTE1`/`SET_BYTE2` lvalues remove integer pointer round trips
for both reads and writes. Six signed reads retain explicit `char` conversion;
five signed writes are standalone assignments with discarded expression values.
All touched fields are native 32-bit info/target words. Dynamic offsets,
pointer-valued uses and other cast shapes remain outside this pass.

Nine target/type contracts, complete source and independent byte-range
projections pass, including unchanged line counts/endings. The build and all
15,169 fixed slots pass. Thirty-nine edited runtime objects are identical; four
have inspected generated-code differences involving register selection, loop
addressing, scheduling/padding and one commuted LEA sum. Isolated recompilation
of saved pre-edit sources reproduced their exact pre-edit runtime hashes for
the focused inspection. This is C byte-access reasoning plus assembly inspection,
not full-handler execution coverage. The linked runtime changes, and earlier
native fixture receipts remain historical. Initial value-extraction macros were
refined to character lvalues so packed accesses retain byte-sized source access
semantics. Existing staging remains unchanged.

The [packed-record consumer pass](recovery-evidence/native-packed-record-consumers.json)
restructures fourteen existing definitions across nine original source files.
Tracked attackers are explicit unsigned byte lists with reverse index loops;
Necropolis Regent uses three-byte hit records rather than an advancing
`target_t*`. Two remaining fixed pointer-valued byte accesses use existing byte
lvalues. The source retains fresh attacker/current-turn reads after helpers,
the live pending-count check during Regent resolution, its cap/upper-only damage
saturation/recomputed write offsets, and the Ur-Dragon's repeated draw/tutor
behavior. No new ABI records or compatibility wrappers are introduced.

The build, all 15,169 fixed slots, twelve ABI/constant contracts and complete
source projections pass. Actual native/rebuilt Master Symmetrist, Reyav and
Necropolis Regent code agrees in 36,864 post-change fixture comparisons, with
the same count passing before the edits. Deterministic leaf stubs include
mutations of attacker bytes, current turn and the pending count; snapshots
compare complete card records, ordered arguments, returns and fixture/global
state. This is bounded evidence for those three bodies; the other eleven have
source/control-flow/address review and build coverage, not full execution or
gameplay proof. The prior full JSON remains a discovery snapshot. Existing
staging was updated externally after the checks; the recovery tools made no index writes.

The [native local-name pass](recovery-evidence/native-local-identifier-restoration.json)
restores twenty original identifiers at 95 sites in twenty existing definitions
across fourteen C files. Names such as `selected`, `legacy_id`, `legacy_card` and
`t_player` come from the release DWARF. Each accepted mapping has a unique equal
type/location signature, equal function extent, and complete native/current
instruction alignment at the same relative offsets with identical widths and
registers. Changed addresses use explicit unique COFF identities or complete
readonly literal contents. Four candidates with different instructions/registers
or widths were rejected; shared storage and scores alone do not justify a rename.

The build, all 15,169 fixed slots and independent source projections pass. All
fourteen edited runtime objects and DLL runtime sections are unchanged, including
code/data and relocation targets; debug/export metadata are excluded. Earlier
bounded execution observations remain evidence for the unchanged runtime under
their original scope. No new gameplay claim or full comparison is made.

The remaining Righteous Indignation pointer-to-integer argument is intentional:
native DWARF gives both the generic iterator's `arg2` and the callback's `val`
signed four-byte integer types. The callback recovers its pointer from that
payload. Their original API is retained; this is not another packed byte walk.
No staging commands were issued.

The [signed-result and JSON refresh pass](recovery-evidence/reccmp-signed-result-guards.json)
restores two more native tutor guards in Praetor's Grasp and Nightmare Incursion:
`>= 0` accepts card IDs and rejects both `-1` and the tutor's explicit `-2`
cancellation result. Nightmare Incursion retains its counted-loop continuation.
The combined build, all 15,169 fixed slots, exact file projections and both
rebuilt tutor-call/sign-branch checks pass.

The fresh post-correction full JSON covers 18,828 ranges: 12,570 exact, 273
effective and 5,985 different. Its tiny queue has 581 candidates in 425 textual
groups without a score cutoff. Event/address/diagnostic/CFG reviews annotate
224 candidates; 357 remain unannotated. These are triage counts, not remaining
bugs or unrecovered bodies. The full report remains in ignored scratch storage;
the [compressed review archive](recovery-evidence/reccmp-tiny-equality-review.json.gz)
retains its receipt, decoder/input pins, reviews and focused checker.

The optional CFG `--equality-guards` check reviews swapped global/parameter
equality loads only when EAX and differing flags die before use on both paths.
It raises the CFG review count from 121 to 144 on the same queue: 23 additional
functions, with no loss of previous matches. Eight focused acceptance/rejection
cases pass. Other register, width, live-value and unsupported differences remain
unresolved. The review assumes ordinary disjoint stack/global memory and the
listed C calling conventions; it is structural evidence, not full execution or
callee-content proof. No source changes were made for these 23 matches. Staging
remains untouched.

The [small shared-caller pass](recovery-evidence/native-small-caller-restoration.json)
restores four existing definitions across three files, reducing 110 lines to 44.
Implement of Examination delegates its non-activation events to `aer_implement`;
Marisi's Twinclaws and Sewn-Eye Drake use `arb_new_hybrid` with the native colors,
with haste first for the Drake. Weather the Storm uses `basic_spell` for its
can-cast event and retains the native cast guard, cancellation/not-cast checks,
fresh storm-count read after every gain-life call, and resolve gain/kill order.
The shared helper definitions are unchanged.

The combined build, all 15,169 fixed slots, three full/raw source projections
and inspection of the relevant native/rebuilt call and tail-call counts pass.
These checks establish the restored shared boundaries, not whole-handler or
gameplay equivalence. The wider JSON scan found 53 named-callee and 18 argument
groups without score/instruction cutoffs; many reflect inlining or argument
placement. They are investigation hints, not confirmed defects. A strict scan
for immediate positive-return/zero-equality mismatches found no candidates among
4,262 source owners. The preceding full JSON and 581/224/357 tiny-queue counts
remain the pre-edit discovery snapshot; no second comparison was run. Staging
is unchanged.

The [wider return-epilogue review](recovery-evidence/reccmp-wide-return-review.json.gz)
checks actual native/current ranges from the four-caller build against candidates
in the preceding C659 JSON. With no score cutoff and at most 32 changed
instructions per side, the queue has 2,789 candidates in 2,098 textual groups.
Scoped reviews annotate 463; 2,326 remain unannotated. The eight-instruction tiny
queue still has 581 candidates, now with 239 annotated and 342 unannotated.

The optional CFG `--return-epilogues` check permits a single `xor eax,eax` to
move among identical ordered stack restores ending in a plain return. EAX uses,
flag consumers, different restores/adjustments, calls and other operations are
rejected. It raises the wider CFG count from 353 to 383 on the same inputs,
adding 30 functions without losing earlier matches. Ten focused positive/negative
cases and the eight equality-review regression cases pass. The address reader
also keeps unsupported display-name ordinals unresolved without aborting other
frame reviews. No game source changes or new build were needed; the successful
four-caller build remains current. The discovery report is older, while all four
reviewers' input pins and actual checked ranges are current. No candidate or
score was removed, and these are not remaining-bug counts. Staging is unchanged.

The [native and role-name pass](recovery-evidence/native-and-role-local-names.json)
clarifies 131 placeholder locals at 752 identifier sites in 119 definitions
across 58 C files. One spelling, Heroes' Bane's `pc`, comes from a unique native
DWARF type/storage lifetime matched through complete CFG instruction pairs.
This name proof preserves ordinary operations, registers and widths and does
not use the equality-load or return-epilogue normalizations. The strict scan
matches 48 of 409 eligible placeholder-owning functions; matching only variable
read sites establishes no further names confidently.

The other 130 names (`target_player`, `target_card`, `opponent`, `parent_card`,
`legacy_card` and `selected`) describe their sole direct assignment. They are
inferred cleanup names, not claims of original source spelling or new recovered
function bodies. Each has one local declaration and assignment, no other direct
writes/address-taking, no member-name confusion and no new identifier collision.
Only identifier tokens change; expressions, types, order, literals, comments,
line counts and endings are preserved.

One combined build, all 15,169 fixed slots and independent whole-file/raw-token
projections pass. All 58 edited runtime objects and the DLL's runtime sections
are identical, including code/data and relocation targets; debug/export metadata
are excluded. The latest successful build receipt is this pass. Earlier bounded
runtime observations retain their original scopes on the unchanged bytes, while
their source/debug input pins remain historical. No full comparison or gameplay
test was repeated. Regenerate setup before the next comparison. Staging is
unchanged.

The [common-caller restoration](recovery-evidence/native-common-caller-restoration.json)
organizes seven definitions in three files around their native shared workers.
`how_much_life_can_i_pay_impl` replaces the misleading `for_mana` identity: the
surviving flags=1 specialization serves both the ordinary life query and two
mana-availability callers. The public life query now delegates to it. The source
retains the native flags/event parameters even when the compiled worker drops
them; no behavior for other flags is invented. `txtfx` now delegates name events
to `txtfx_name`, which is moved before its caller. Shared bodies are available
where the earlier expanded public bodies were compiled; no forward declarations
or public API changes are needed. The corresponding CSV row uses the restored
life-helper name while retaining its native address and size.

Lightning Dart now has a structured five-event dispatch and common generic-spell
tail. It retains the affected-card check both before and after target setup,
captured target/color values across opaque calls, the native one/four damage
selection, and the generic-spell call after resolve/kill. One combined build,
all 15,169 fixed slots, three exact file projections, the one-row CSV projection
and six native/rebuilt shared call boundaries pass. The life worker has a strict
CFG match; the text worker matches after one explicitly checked independent
argument-store/card-byte-load swap under ordinary disjoint record/stack memory.
This is scoped structural evidence, not a full-game proof.

There are also 40,960 post-change native/rebuilt Lightning Dart comparisons:
2,048 environments, ten events and both players, with ten opaque cdecl leaves
and three globals rebound. Ordered arguments, returns, full card records, AI and
affected globals agree, including callbacks that change state between helper
calls. Leaf behavior and arbitrary memory aliases are outside the fixture.
The latest successful build receipt is this pass; the earlier full report and
239/342 tiny-queue counts remain historical discovery/review snapshots. No
second full comparison was run. Regenerate setup before comparing this build.
Staging is unchanged.

The [land and mode dispatch pass](recovery-evidence/native-land-and-mode-dispatch.json)
restores four existing cards: Cabal Stronghold, Ghost Town, Repudiate // Replicate
and Borrowed Grace. Borrowed Grace now returns the native constant can-cast result
and makes at most two mode selections, retaining the final ignored mana query
and exact cancellation reset. The other callers preserve separate live payment
queries, post-charge land recounts, stale counter markers and callback-sensitive
record reads. The source retains native behavior rather than repairing game rules.

One combined build, all 15,169 fixed slots and four canonical/raw file projections
pass. A bounded fixture executes the four actual native/rebuilt card bodies and
available private counter workers over 524,288 observations with no mismatches.
It compares ordered calls, arguments, complete records, returns and globals;
opaque leaves are stubbed, so this does not establish whole-game equivalence.
This is the latest successful build receipt. The earlier full comparison and
239/342 tiny-queue counts remain historical snapshots pending a refresh.
Staging is unchanged.

The [current tiny-diff refresh](recovery-evidence/reccmp-current-tiny-refresh.json.gz)
compares the four-card build across 18,828 ranges: 12,575 exact, 273 effective
and 5,980 different. The all-score eight-instruction queue contains 581 functions
in 425 textual groups. Current event/address/diagnostic/CFG reviews annotate
239 change sites, leaving 342 unannotated; these are not remaining-bug counts.
The full report includes instructions and pins its CSV, data and external maps.
Reuse this discovery baseline across several source batches, inspecting edited
functions directly; refresh the full report after enough changes accumulate.
This avoids treating register, layout and masked-width differences as required
source rewrites. No source edit or rebuild was needed for this refresh.

The [negative-index guard correction](recovery-evidence/reccmp-kill-card-index-guards.json)
is the first source fix from that refreshed tiny-diff queue. Native `kill_card`
and `kill_card_exe` return when either signed player/card index is negative;
the rebuilt entries previously reached the shared worker unconditionally.
The early return now lives in `kill_card_impl`, covering both wrappers and its
direct callers without inventing an upper-bound check. Both rebuilt public
wrapper CFGs match native. Another 1,024 bounded comparisons execute their
actual bodies with only the outlined worker replaced by an argument recorder;
negative inputs skip it, while nonnegative inputs preserve indices, mode and
the wrapper-specific flags. The worker body is outside this fixture.

The combined build, all 15,169 fixed slots and exact canonical/raw source
projection pass. This is the latest successful build receipt; the full JSON
above is the immediately preceding discovery baseline. Inspect edited functions
directly between full refreshes. Staging remains unchanged.

The [small-caller event pass](recovery-evidence/native-small-caller-event-paths.json)
reconstructs six existing card handlers in their original files, reducing their
definitions from 182 to 119 lines. Battletide Alchemist restores the release's
damage-target recheck after the planeswalker helper and captures its target
player before the automatic-choice query. Its native 500-byte formatting bound
and 100-byte local are retained. Leyline of Vitality, Court of Ambition, Blood
Pet, Razaketh's Rite and Strike It Rich gain clearer event dispatch and shared
tails. Their ignored probes, live/cached reads, raw helper return values and
mana query ordering remain unchanged; new role names are descriptive rather
than claims about original DWARF spellings.

One combined build, all 15,169 fixed slots and six exact canonical/raw source
projections pass. The same fixture runs 1,572,864 observations before and after
the changes against the actual native/current caller bodies, binding 30 opaque
leaves and seven globals. Its 128 Battletide post-helper-target differences fall
to zero; the five other callers agree in both runs. This is bounded caller
evidence, not helper-leaf or whole-game equivalence. This pass supplies the latest
build receipt; the C665 full JSON remains the discovery baseline. No second full
comparison was run, and staging is unchanged.

The [counterspell caller pass](recovery-evidence/native-counterspell-caller-flow.json)
flattens Intervene, Delay, Memory Lapse and Spell Swindle from 122 to 66 lines.
Intervene now uses one live-count target loop and preserves its positive-only
availability check and raw success value. The other three share their cast
dispatch while retaining different instance-getter timing, resolution-time
target rereads and zero returns for retarget events. Spell Swindle still caches
mana value before countering and truncates Treasure quantity to eight bits.
This is reconstruction of already-agreeing flow, not a new gameplay bug claim.

One combined build, all 15,169 fixed slots, four exact source projections and
655,360 bounded actual native/rebuilt caller observations pass before and after
the rewrite. Fourteen leaves and four globals are bound; leaf behavior and
whole-game equivalence are outside the fixture. This is the latest build receipt.
The C665 full JSON remains the discovery baseline; staging is unchanged.

The [pointer-return review](recovery-evidence/reccmp-pointer-return-review.json.gz)
adds a conservative optional `--pointer-return-tails` CFG model. It checks native
DWARF four-byte pointer returns against current C declarations, then compares
read-only symbolic return trees and preserved registers/stack. Calls, writes,
cycles, unsupported operations and predicates remain unresolved. Numeric masks
and comparison constants cannot borrow address identities. This adds 28 tiny
and 33 wider CFG matches, losing none of the same-input baseline matches.

The tiny queue now has 269 scoped annotations and 312 unannotated candidates;
the 2,787-entry wider queue has 502 and 2,285. These are review counts, not bugs.
Twenty-three focused pointer checks, ten diagnostic-drift checks and the earlier
ten return/eight equality regressions pass. The diagnostic reviewer also permits
current rebuilt line-number drift while keeping the path, expression, destination
and native discovery text exact. All review pins are current; C665 remains the
historical discovery JSON. No game source change or rebuild was needed. The
counterspell-caller build remains current, and staging is unchanged.

The [header enum pass](recovery-evidence/native-header-enum-completion.json)
uses the small-difference JSON to identify a real Chancellor of the Tangle
storage mismatch: `TRAP_COT_FLAG` was 0x408, sharing the noncreature-spell
slot; native code and DWARF use 0x40B (the high nibble at card offset 0xF1).
The header now matches. Thirty-four missing native enumerator names are also
restored, with 90 equal-valued state, card-count and card-flag references named.
All 26,968 header enumerators exposed by the native `functions/functions.c`
compilation unit now agree with the active compiler. This scope does not claim
that every enum in every compilation unit has been inventoried.

The combined build and 15,169 fixed slots pass. The actual native/rebuilt trap
helpers and private workers agree in 577,536 bounded comparisons, including
the Chancellor nibble, neighboring bits, missing caches and clamp boundaries.
Only the getter leaf and rules-engine cache are bound; this is not a whole-upkeep
or whole-game proof. Across 166 objects, 148 runtime snapshots are identical,
17 differ only in compiler-generated symbol suffixes, and rules_engine.c has
the expected trap-access code changes. All 38 raw/canonical source projections
pass. This is the latest build receipt; staging is preserved.

The C665 JSON remains a historical discovery baseline. The C669 counts of
269/581 tiny and 502/2,787 wider scoped annotations describe its review against
the preceding C668 build, not the new header build. Prioritize small literal,
argument, return and predicate differences from that queue, checking current
function ranges directly and batching source fixes before another full refresh.
Annotations are reviewed differences, not recovered-function or bug counts.

The [tiny caller-order pass](recovery-evidence/native-tiny-call-order.json)
closes two differences found in the C665 JSON and revalidated against C670.
Curse of Echoes now explicitly snapshots the enchanted player before calling
`duh_mode()`, preserving the release's argument observation order. The
`graveyard_from_play()` helper restores the second instance-getter call before
reading its extension's exile flag. This records native call sequences without
assuming the getter changes its return value in ordinary gameplay.

Both complete entry-reachable CFGs now pass the strict native/current comparison;
both failed the same comparison before the edits. Only padding, direct jump
trampolines and pinned symbol addresses are normalized. No optional symbolic
tail or equality models are enabled. Callee behavior and whole-game equivalence
remain outside this check. Two canonical/raw source projections, the combined
build and all 15,169 fixed slots pass. This is the latest build receipt.
The full JSON and C669 annotation counts remain historical; the two additional
matches are recorded here without regenerating those queues. Staging is intact.

The [small helper and aura pass](recovery-evidence/native-small-helper-readability.json)
reconstructs eight existing definitions from 191 to 127 lines. Aqueous Form and
Pursuit of Flight restore a second attachment getter present in the release;
both now pass the strict comparison of their complete entry-reachable CFGs.
Pursuit preserves the query on resolve/cleanup dispatch as well as activation.

Six readability-only changes simplify the card chooser, watermark callback,
mana-cost predicate, City's Blessing lookup, Capital Offense and sacrifice
availability wrapper. Named fields/constants and ordinary control flow replace
packed identity arithmetic, redundant temporaries and nested conditions.
Chooser sentinels and mode conversion, live card counts, callback argument
behavior, byte capitalization and the native zero/absent blessing ambiguity
are preserved. Three of these definitions retain identical runtime objects;
the other three were reviewed against native operations but still have compiler
differences and are not claimed as whole-function matches.

The combined build, all 15,169 fixed slots and seven exact canonical/raw file
projections pass. This is the latest build receipt; the C665 full JSON and C669
queue totals remain historical. This pass adds two focused strict CFG matches
without refreshing those counts. Staging is preserved.

The [spell-interaction and trigger pass](recovery-evidence/native-spell-creature-flow.json)
flattens six existing bodies from 320 to 202 lines: Mizzium Meddler, Frilled
Mystic, Muck Drubb, Naru Meha, Sweet-Gum Recluse and Quest for Ancient Secrets.
Four similar CAN_CAST paths now state their native 1/99 returns and stored
availability directly. The handlers retain their different ETB timing, live
target/index rereads, raw madness/flash/quest returns, prompt formatting and
post-dialog stack reads. Sweet-Gum keeps preexisting targets and the ten-target
selection cutoff; Quest keeps both graveyard-trigger paths. This is readable
reconstruction of already-agreeing flow, not a new handler or gameplay-bug count.

The combined build, 15,169 fixed slots and five exact raw/canonical file
projections pass. Six actual native/current bodies agree in 786,432 bounded
comparisons before and after the rewrite, with 35 opaque helper/callback
bindings and fourteen globals. Calls can mutate targets and stack globals;
complete records, ordered arguments, raw returns and globals are observed.
Helper behavior, UI, unrestricted memory aliases and whole-game equivalence
remain outside the fixture. This is the latest build receipt; the full C665
JSON and C669 queue annotation counts remain historical. Staging is preserved.

The [permanent-event flow pass](recovery-evidence/native-permanent-event-flow.json)
reconstructs eight existing bodies from 473 to 289 lines: Ashiok, Dream Render;
Arlinn, Voice of the Pack; Angrath, Captain of Chaos; Garruk Relentless;
Timebender; Simic Ascendancy; Kiora Bests the Sea God; and Unquenchable Fury's
legacy. It replaces nested event dispatch and reused scalar temporaries with
ordinary branches, loops and descriptive locals/constants. Getter calls on
cleanup, repeated loyalty checks, live target/parent/loop-bound reads and the
release's counter/AI quirks remain explicit. This is readable reconstruction
of already-agreeing bodies, not a new gameplay-bug or recovered-handler count.

The combined build, 15,169 fixed slots and six raw/canonical source projections
pass. All eight native/current callers agree in 1,179,648 bounded observations
before and after the rewrite, under 43 opaque helper/callback bindings and
fourteen globals. The fixture observes complete records, ordered arguments,
raw returns and globals, with callbacks changing target/parent pairs, query
results and battlefield bounds. Helper behavior, actual dialogs and whole-game
equivalence remain outside this check. This is the latest build receipt;
historical full JSON and queue annotation totals are unchanged. Staging is intact.

The [post-cleanup full refresh](recovery-evidence/reccmp-post-cleanup-refresh.json.gz)
is the current C674 discovery snapshot: 18,828 ranges, with 12,596 exact,
275 effective and 5,957 different. It gains 21 exact and two effective matches
against C665, with no previously exact/effective ranges moving backward between
categories. Individual scores remain instruction similarity rather than
behavioral proof. The refresh updates 146 metadata rows' rebuilt-entry mapping
addresses; the CSV's native identities and compared coverage stay intact.

The all-score tiny queue contains 577 functions in 422 textual groups.
Event/address/diagnostic/CFG reviews annotate 266 sites, leaving 311 unannotated.
The wider queue contains 2,767 functions in 2,077 groups, with 496 annotations
and 2,271 unannotated entries. Every prior annotation on a still-listed candidate
passes again. Prior annotated functions that left the queues became exact;
the refreshed wider queue also gains two annotations. Candidates and raw scores
are retained, with each annotation limited to its own proof scope.

Setup verifies 160 explicit function bindings, 293 globals, 64 engine targets
and 15,169 fixed slots. Zero source stubs are included. Its 21 excluded ranges
are recorded for mapping follow-up: ten lack a standalone current name, nine
have ambiguous native names, one is a verified merged helper and one has an
ambiguous rebuilt C++ name. This is not a missing-source inventory. No game
source edit or rebuild was needed. The permanent-event pass remains the latest
successful build; staging is preserved. Reuse this full JSON for subsequent
batches, checking changed functions directly between full refreshes.

The retained-helper mapping refresh (C677) restores two renamed landcycling
identities to reccmp through an explicit address/name/source alias manifest.
It uses the unchanged C674 build and compares 18,830 ranges: 12,599 exact,
275 effective and 5,956 different. The Commander helper and two existing
card callers are exact; no previous exact/effective category regresses.
The [mapping evidence](recovery-evidence/reccmp-retained-helper-aliases.json)
records rejection checks for conflicting/ambiguous aliases, one strict helper
CFG match and the manually explained Conflux compiler differences. Existing
scoped annotations transfer only for identical complete function report rows
on identical binary/debug/global/external inputs. The tiny queue now has 576
entries, 284 annotated and 292 unannotated; the wider queue has 2,766 entries,
514 annotated and 2,252 unannotated. Nineteen excluded ranges remain mapping
follow-ups. This pass changes comparison identities and tooling, with no game
source changes or rebuild. Staging is preserved.

The void-return and field-width pass (C678) adds 20 scoped helper CFG
correspondences, 19 within the tiny queue. Native DWARF and current source
both establish a void result before the checker permits residual EAX at an
ordinary return. Earlier scratch uses and live condition flags remain rejected.
All 20 fail the same checker with the option disabled; 20 regression checks
pass. Five further manual reviews explain discarded high bits of wider
record-field loads. The [evidence](recovery-evidence/reccmp-void-and-width-review.json)
records the native contracts, range hashes, control-flow correspondences and
manual assumptions. The current tiny queue has 576 entries, 308 annotated and
268 unannotated; the wider queue has 2,766 entries, 539 annotated and 2,227
unannotated. The C677 comparison scores and C674 game build remain unchanged.
No game source edit or rebuild was needed. Staging is preserved.

The equal-edge and compiler-worker pass (C679) adds 28 complete entry-reachable
CFG correspondences, 14 within the tiny queue. Reversed equality loads have
the same EAX and CMP flags on their equal successor; unequal successors retain
the scratch/flag liveness checks. Twenty existing and 13 path-sensitive
regression cases pass. The [evidence](recovery-evidence/reccmp-equal-edge-and-worker-review.json)
also confirms Miara's compiler split: the native positive-count body strictly
matches the rebuilt worker, and both direct callers guard its count to 1..255.
The original source guard remains present. Two manual argument-site reviews
confirm unchanged pump-result arguments in Paragon of the Amesha and Dragonsoul
Knight. The tiny queue now has 325 scoped annotations and 251 unannotated
entries; the wider queue has 570 annotations and 2,196 unannotated entries.
No game source edit, score change or rebuild was needed. Staging is preserved.

The Totem color-query order pass (C680) provided the preceding game build. Weatherseed,
Phyrexian and Foriysian Totem now call get_sleighted_color_test before modifying
the animation legacy's metadata. Their native subtype/color store order is
preserved, including Foriysian's color-first sequence and live parent query.
All three final complete entry-reachable CFGs correspond under unique symbol
identities and complete read-only C-string/DIALOG-token contents, with none of
the optional predicate/return-tail relaxations enabled. The [evidence](recovery-evidence/native-totem-color-order.json)
distinguishes the stricter baseline address binder from the final read-only
literal-content binder. The combined build, 15,169 slots and exact raw/canonical
file projection pass. Chronatog and Thunder Totem were already exact in the
C677 discovery baseline. That full report and the C679 queue counts remain
historical; no full comparison refresh or gameplay fixture was run for this
small source batch. Staging is preserved.

The token and marking-buffer pass (C681) is the latest game build. Crawling
Sensation's private make_insect helper explicitly resets no_sleight after the
conditional default initializer, as the release does. The complete caller CFG
now corresponds under the same strict checker that rejected it before.
Five other handlers reconstruct scoped zero-initialized char buffers at their
existing clear points: Kindred Charge, Symbol Status, Scale Blessing, Drana,
Liberator of Malakir and Saheeli, the Gifted. Native DWARF confirms their names
and all 802 declared bytes. Initializer spelling is inferred, and remaining
function differences stay unresolved. The [evidence](recovery-evidence/native-token-marking-initializers.json)
records these limits, six exact raw/canonical source projections and the
successful build with 15,169 fixed slots. The C677 full report and C679 queue
counts remain historical. No full comparison refresh or gameplay fixture was
run for this source batch. Staging is preserved.
