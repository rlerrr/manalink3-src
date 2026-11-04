# Comparing the recovery build with reccmp

This project uses the DWARF-enabled fork at `C:\dev\reccmp` (currently
`feat/dwarf-ingest`), rather than the MSVC-only upstream installation. No
changes to that checkout are needed. The wrapper imports that checkout
explicitly, so an installed upstream `reccmp-reccmp` cannot silently take its
place. The current local Python environment already has its dependencies.

The [C750 source-layout receipt](recovery-evidence/source-layout-restoration.json.gz)
covers function placement, card-description restoration and excess blank-line
cleanup. Function bodies, literals and conditional branches match `1864623`;
file-scope tokens match except eight forward declarations. The GCC 9.3 build
and all 15,169 fixed veneer slots pass. The one comment-bearing tail forwarder
retains its historical definition hash plus a separately pinned comment
projection; both definitions must match the same strict forwarding syntax.

The C751 follow-up repositions loose section and skipped-card comments only.
Whole-file code tokens, definition order and conditional guards match C750;
no rebuild or comparison rerun was needed. Debug line numbers in the existing
C750 artifacts still describe the source layout at that build.

Moving definitions changes generated addresses and debug line mappings. Reports
from C736/C735 are historical snapshots, not scores for this layout build; no
full comparison rerun was needed for this cleanup. Use the normal comparison
wrapper when the next behavioral recovery batch needs fresh scores. C750 is the
latest build receipt; C751 is the latest source-layout receipt. Older review
sections retain their batch-local
build and comparison references.

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

The later [C726 source batch](recovery-evidence/native-spell-instance-query-placement.json),
[C727](recovery-evidence/native-attachment-query-and-argument-snapshots.json),
[C728](recovery-evidence/native-spell-event-and-convoke-flow.json) and
[C729](recovery-evidence/native-cycling-and-aura-query-flow.json) refine native
query/snapshot/event behavior and structured source flow. C729 reviews 74
functions, corrects six handlers and builds once. Selected native/rebuilt sites
confirm the changes; six strict entry checks retain their remaining differences.
C725 queues are historical; regenerate report/queue and choose a matching build
receipt before running the example on newer code.

The compared source build ends with [C724](recovery-evidence/native-type-worker-and-color-query-order.json).
Queues are `.recovery/reccmp/current-tiny-reviewed-c725.json`,
`.recovery/reccmp/current-wide-reviewed-c725.json`, and
`.recovery/reccmp/current-expanded-reviewed-c725.json`.

`tools/recovery/review_spell_continuations.py` processes a whole current queue
without rewriting game source or changing reccmp scores. For example:

```powershell
python tools/recovery/review_spell_continuations.py --queue .recovery/reccmp/current-expanded-reviewed-c725.json --build-evidence docs/recovery-evidence/native-type-worker-and-color-query-order.json --output .recovery/reccmp/next-spell-continuations.json --limit 2000 --public-call-flags
```

The optional flag verifies both callee entry sequences and unique public/source
identities; it does not trust an arbitrary call as a flag clobber. Native/current
helper-prefix packets, complete event partitions and continuation results are
archived. Model controls reject flag reads, insufficient/undefined flag writes,
unknown calls and a changed actual outgoing argument. Unsupported cases remain
pending. Regenerate the report/queue and choose the matching build receipt after
later source builds; a stale input fails the hash guards.

The current cached named-call lead packet is
`.recovery/reccmp/semantic-call-leads-c725.json`, bound to C724 source hashes and
the C725 report/queue. Its scanner caches each source mask once and ranks new
leads before unchanged recent source observations. Inlining/outlining and branch
duplication can create call deltas; this is a priority packet, not a missing-body
count or a replacement for the complete comparison queues.

`.recovery/reccmp/call-lead-progress-c731.json` supplies the historical call-packet cursor:
352 C726-C731 corrections or scoped observations are deprioritized with their
source/native hashes; 640 packet indices remain. Use
`.recovery/reccmp/priority-progress-c731.json` for the next small-operand batch;
C730 instruction scores remain historical after the C731 source build. Read the live source for each
next candidate. This cursor does not change scores, prove whole functions or
close comparison ranges. Source changes invalidate the associated observation.

Review 100-200 small candidates together using cached JSON and native decompilation,
grouping repeated differences and parking hard outliers early. Explained compiler
layout differences need no source rewrite or repeat review unless their source or
relevant context changes. Make all related edits before one incremental build;
use focused comparisons for changed code and refresh the full comparison at
substantial source checkpoints. Keep one compact batch manifest, with extra detail
only for uncertain sites and real mismatches. Gameplay tests remain downstream.

The preceding [private-helper route restoration](recovery-evidence/native-private-helper-routes.json)
changes three callers to use their native private helper directly, with two
existing declarations moved ahead of the callers. The C697 build and all
15,169 fixed entry slots pass; the three caller ranges remain byte-identical
to C695. Their full native instruction differences remain.

The separate [entry-dispatch analysis](recovery-evidence/reccmp-spell-entry-dispatch.json.gz)
covers 240 handlers from five large repeated difference groups. Each proof
covers the five allowed spell events and their full 32-bit complement, plus
both affected-card/controller equalities: 24 paired partitions per handler,
5,760 total. It stops at the first named target-definition call or zero return.
Call identity, four arguments, private stack and callee-saved state agree.
Corrupted target-type, inverted event-branch and caller-stack store controls
are rejected. Complete
source/COFF/DWARF entry identities and raw byte packets support replay without
old full-build copies.

These are partial dispatch proofs, not 240 completed function reviews. Their
separate `entry_dispatch_review` annotations are in
`.recovery/reccmp/current-wide-entry-reviewed-c697.json` and carried into the
C699 queue; these partial proofs alone close no full site review. The comparison
scores stay C696/C695; the fragment proofs use the historical C697 build.

The preceding [numeric/snapshot batch](recovery-evidence/native-numeric-slot-restoration.json)
restores six definitions, with Ashiok and Jace gaining full CFG correspondence;
its 110 site reviews and the [C693 batch](recovery-evidence/reccmp-tiny-site-batch-c693.json.gz)
remain supporting evidence. C693-C695 together add 194 scoped reviews and 14 source edits. This C696
refresh includes those edits and adds ten focused explanations. The older
discovery ranking remains useful; consult the current annotated queue before
revisiting an entry.

The latest [callback/helper-route recovery](recovery-evidence/native-callback-and-debuff-routes.json)
restores Harnessed Lightning's AI-only callback registration at its native
resolution-time location and makes its helper query order explicit. Three
Phyrexian cards again call the shared `debuff_for_T_sac` helper instead of
duplicating its body. The AI helper and all three thin wrappers gain strict
complete CFG correspondence; the general debuff helper and Harnessed Lightning
still have other instruction differences. The build and all 15,169 fixed entry
slots pass.

A scan of 1,609 unsuffixed source-owned native private helper entries accounts
for 1,607 through emitted or concrete inline DWARF, one through an exact
17-byte private COFF body without matching DWARF bounds, and one through the
existing verified shared attachment-helper equivalence. Source ownership
includes 54 ARCANE macro-generated bodies and nine Odyssey filename aliases.
This is representation accounting, not full semantic coverage or a claim that
recovery is complete.

Setup binds 163 functions explicitly by owning source and current entry, plus
293 DLL-owned objects and 64 fixed engine targets. There are zero source stubs.
Twelve native ranges remain comparison-mapping follow-ups, including code
already accounted for through inlining, compiler parts or native equivalence.
They are not a count of twelve missing source bodies.

`config/reccmp/function-aliases.json` binds the Conflux and Commander 2016
landcycling helpers and Stronghold's `create_stronghold_end_combat_bounce` by
native address/name, owning source and rebuilt name. The Stronghold helper is
a void/four-argument function, distinct from Prophecy's int/three-argument legacy
callback with the same original spelling. Setup requires a unique source-bound
range and rejects conflicting identities; changing aliases requires setup again.

Historical source edits and their narrower proofs remain in the recovery
evidence ledger. The preceding full checkpoint is
[C692](recovery-evidence/reccmp-callback-route-refresh.json.gz). Its scores and
pending-refresh notes are superseded here. The old ignored C692 DLL/DBG copies
can be retired; selected historical byte ranges remain in their evidence
archives.

From this repository root:

```powershell
# Build first when necessary; setup itself does not compile anything.
.\tools\build-recovery.ps1
python tools/reccmp_project.py setup

# Inspect one function, using its October 2022 absolute virtual address.
python tools/reccmp_project.py compare --verbose 0x2377a70 --print-rec-addr

# Generate the searchable assembly-diff report and machine-readable summary.
python tools/reccmp_project.py compare --html .recovery/reccmp/report.html --json .recovery/reccmp/report.json --json-diet --silent

# Include instruction diffs when looking for small source corrections.
python tools/reccmp_project.py compare --json .recovery/reccmp/small-diffs.json --silent
python tools/recovery/small_diff_queue.py --report .recovery/reccmp/small-diffs.json --output .recovery/reccmp/small-diff-queue.json

# Short functions can have tiny deltas despite a low percentage score.
python tools/recovery/small_diff_queue.py --report .recovery/reccmp/small-diffs.json --output .recovery/reccmp/small-diffs-all-scores.json --min-score 0

# Optional targeted field/return interval leads, using live rebuilt COFF addresses.
python tools/recovery/rank_interval_leads.py --self-test
python tools/recovery/rank_interval_leads.py --report .recovery/reccmp/current-refresh-c736.json --metadata .recovery/reccmp/current-refresh-c736.metadata.csv --output .recovery/reccmp/interval-leads.json --include-returns

# Optional comparison of the same bytes before/after external-target naming.
python tools/reccmp_project.py compare --audit-externals --json .recovery/reccmp/external-audit.json --silent
```

Setup also validates 64 fixed Magic.exe function targets against the original
Hotfix executable and both debug files. The native symbol must have a COFF
function type; ten rebuilt assembler declarations are untyped, so this establishes
name/address identity without claiming prototype equivalence. These exact lookup
labels live in `config/reccmp/externals.csv`; they are not additional DLL function
ranges or fork CSV data sources. Unknown addresses and indirect-call resolution
retain the fork's behavior. The original executable is extracted into ignored
local storage when needed and checked against its pinned release hash.

Report receipts snapshot the external registry and lookup audit alongside function
and data metadata. `--audit-externals` additionally compares referencing functions
with fresh unmodified parsers over the same bytes and bindings. Its sidecar records
score changes and the earlier diffs. Normal comparisons skip that extra pass.
`--no-external-symbols` disables the adapter for a control run. Correct naming can
lower scores when generic placeholders previously hid different callees; score
increases from naming alone are not new source recoveries.

The small-diff queue defaults to scores of at least 90% and at most eight changed
instructions per side, ordered by total changed instructions and score. Omit
`--json-diet`: that option removes the instructions needed for this queue. The
tool verifies the report receipt and matching CSV, and labels whether the input
binaries are still current. An older report remains a snapshot after rebuilding.
Unknown addresses, moved blocks, assertion line numbers and equivalent width or
branch optimizations need inspection before changing source. The first pass
found 494 candidates and produced 13 native-backed source corrections; see
[batch evidence](recovery-evidence/reccmp-small-diff-corrections.json).

Each candidate also has a `review_pattern`. The `review_patterns` totals group
assertion line numbers, retarget-event comparison order, instruction reordering,
branch layout/padding and callee/address mapping. Changed branch conditions remain
in `inspect_operands_and_control_flow`. These labels help batch inspection; they
do not prove equivalence, exclude candidates or change the ordering. Source paths
translate the native `cards/odyssey.c` spelling to the actual `cards/odissey.c`.

`change_groups` batches repeated textual deltas while retaining the original
candidate order and every address. Signatures mask branch destinations and
comparator-local OFFSET numbers; numeric operands and branch conditions remain.
Each member still needs its own address and control-flow check. `--group-preview`
controls the printed group summary. An earlier report had 374 default
candidates in 299 groups; removing the score cutoff found 590 candidates in 435
groups, still limited to eight changed instructions per side. These are review
batches, not automatic equivalence decisions.

The [tiny-return correction](recovery-evidence/reccmp-tiny-return-correction.json)
comes from a two-instruction difference in Rakdos Pit Dragon. Native resolution
calls the pump helper and returns zero; the earlier source returned the helper's
effect value. Twenty-four bounded caller cases preserve every outgoing argument
and now return zero for zero, positive and negative helper results. The combined
build and all 15,169 fixed slots pass. Byte-count loops, caller-hoisted guards and
shared instruction tails were also inspected without speculative source edits.

The following [target/trigger dispatch pass](recovery-evidence/native-target-trigger-dispatch.json)
restores three explicit native `validate_target(player, card, &td, 0)` calls and
Chrome Mox's caller-side `RESOLVE_TRIGGER_AI(player)` selection. A combined build,
all 15,169 slots, eight target-validation routes and 15,456 Chrome Mox mode/prefix
routes pass. Shared helpers remain unchanged. These four source edits are included in the latest full comparison above;
its current candidates describe the refreshed build.

The [split-card ID map reconstruction](recovery-evidence/native-split-id-table.json)
then replaces an 11 KB decision tree with 75 named card cases, preserving fifteen
fusable pairs and the native handling of half-selection flags. All 225 named
enumerator values match release DWARF. A small 32-bit harness calls only this pure
integer helper in DLLs mapped without import resolution or entry-point execution;
983,145 cases agree across the release DLL, actual rebuilt DLL and standalone
named source. These cover all 65,536 16-bit card IDs, fifteen flag values and seven
additional signed/out-of-range IDs. The latest full report now includes this edit.

A fresh partial-constant screen covers 6,276 functions. Its apparent Prismatic
Vista difference is a folded query/activation-tail signal: four native/rebuilt
query routes retain the same event and operands. A preliminary time-counter
signal used a stale explicit rebuilt address after relinking; fresh DWARF/COFF
finds the real helper and its correct time-counter selector. Discovery scripts
must rebind explicit addresses before using newer binaries. No speculative card
edit is made for either signal. Separately, 131 unique nonlocal scalar/array data
initializers compare byte-for-byte with the release; pointer/record objects remain
outside that data comparison.

The [DFC/counter lookup pass](recovery-evidence/native-dfc-counter-id-tables.json)
reconstructs both double-faced-card directions as 294 named cases each, replacing
1,902 lines of comparison trees with 1,192 lines of flat switches. It also names
84 numeric card labels in the existing counter map. The release, actual rebuilt
DLL and standalone source agree in 196,629 cases across the three helpers: all
16-bit card IDs plus seven additional signed/out-of-range IDs for each function.
All 776 relevant native enum values, both complete source-file projections, the
combined build and 15,169 fixed slots pass. These restore readable identities and
control flow; they do not add card handlers or change the native lookup results.

Completed edit records through the split-card pass are now stored losslessly in
[a compressed JSON archive](recovery-evidence/completed-source-edits-through-c633.json.gz).
The [archive receipt](recovery-evidence/completed-source-edit-archive.json) records
18,964 archived edits and a byte-exact reconstruction of the preceding manifest.
The live manifest shrinks from 104,792,543 to 1,049,740 bytes while retaining the
current batch's 86 projections, the live Windows-hook receipt, all 171 file pins
and all canonical identities.
`--verify-current` hashes the compressed archive without decompressing it. The
previously retired historical compiler/runtime replay remains unsupported.
`tools/recovery/archive_completed_source_edits.py` can repeat this compaction with
a new archive/receipt and one complete trailing batch; it never changes staging.

Successful JSON comparisons also save a matching `.metadata.csv` snapshot next
to the report. The queue selects that snapshot automatically and checks its
receipt hash, so later setup runs do not prevent inspecting an older report.
Use `--metadata PATH` to supply an older report's exact CSV explicitly.

Setup also generates `config/reccmp/globals.csv`. The local fork's DWARF reader
currently emits functions only, so `tools/recovery/reccmp_globals.py` supplies
data nodes through its existing comparison interface. The fork checkout remains
unchanged. Each binding requires a unique original/rebuilt source, lexical scope
and variable name, matching storage size, coarse type and writability, and
nonoverlapping candidate extents. Repeated names receive source/scope-qualified
display labels. The adapter verifies every actual matched rebuilt address.

The current pass binds **293 DLL-owned project objects**, including **51
function-local statics**. It covers direct-address DWARF storage in project
compilation units; EXE-owned absolute globals, CRT data, stack variables and
unsupported or unemitted objects remain outside this pass. These identities do
not establish initializer equality or complete aggregate ABI equivalence.
Successful reports retain `.globals.csv` and `.globals.json` snapshots and their
hashes alongside the function metadata. The queue checks these when present,
while continuing to accept historical reports that predate data bindings.

At the first data-binding checkpoint, after the filter/formatter corrections, the
report had **18,828 ranges: 12,467 exact, 271 effective and 6,090 different**;
**393** meet the tiny-diff threshold. The formatter now compares exactly. No
previous exact match is lost. Names can change diff alignment and scores in
either direction, so score changes alone do not establish source correctness.
See the [global-binding checkpoint](recovery-evidence/reccmp-global-bindings.json).

That full report precedes the next
[eight-function correction batch](recovery-evidence/reccmp-caller-mode-corrections.json).
A broader constant-argument screen found activation-event, hand-count reporting
and exile-payment mode errors outside the tiny-diff threshold. Focused checks
verify 24 native/current call sites after one build. Private helper calls need
ABI inspection: GCC can move formals into registers and remove constant-zero
arguments, so identical stack offsets need not refer to identical parameters.
The five apparent outlast cost differences were rejected for this reason.

The next [buffer-initializer pass](recovery-evidence/native-buffer-initializer-restoration.json)
uses fresh C627 bounds for broader constant and call-count screens. Its public
basic-block argument analysis finds no further complete constant discrepancies;
unresolved/private layouts remain outside that result. The 625 whole-body
constant candidates and 1,129 call-count candidates require expression/path review.
GCC can regroup predicates and duplicate or share call blocks, so these counts
do not identify missing gameplay actions on their own. That pass also reconstructs
five native buffer initializers and validates their 1,053 bytes after one build.

The subsequent [shared-dispatch pass](recovery-evidence/native-shared-dispatch-restoration.json)
restores original helper calls in **80 card functions** where decompiled source
had expanded their implementations: 55 `basic_spell`, 17 `global_enchantment`,
four `generic_token`, two `flash`, and one each for Exploration and Nantuko Shade.
It also restores Takklemaggot's missing state-based removal and death-check order.
All 81 emitted callee edges, conditional removal/legacy paths and 15,169 fixed
slots pass after one build; the six shared helpers themselves remain unchanged.
These are restored delegations plus one behavior correction, rather than newly
discovered handlers. Full comparison scores still describe the earlier snapshot.

Before treating an absent call as omitted behavior, follow verified compiler
workers. `new_damage_all` has a 55-byte public entry that jumps to a 557-byte
`.part.0` worker containing the supposedly missing damage call. Match that edge
using its actual COFF symbol, owning source and fresh DWARF range; keep static
occurrence counts distinct from execution counts along a path.

The next [mechanic-dispatch batch](recovery-evidence/native-mechanic-dispatch-restoration.json)
restores fifteen more shared implementations, including convoke, hybrid, cycling,
counterspell/Fog/regeneration, the mana-creature helper and total-counter counting.
It removes two gotos and raw counter-byte aliasing. Five native/rebuilt selectors
agree on **12,880 event-routing cases**, including full outgoing costs/operands.
Teach by Example retains its distinct cast-query, resolution and hybrid routes;
the existing Doublecast helper receives a public three-argument declaration.

Some native calls are physically present in unreachable blocks. Shape of the
Wiitigo's outer gate excludes the event needed by its blocking call. Three
apparent land-sound omissions instead reach `mana_producer` on resolution; adding
a direct sound-only return would bypass that real fallback. Bounded native/current
path checks preserve those gates. These checks stop at selected gameplay helpers
and explicitly stub limited prefix calls; they do not simulate a whole card.

Indexed address displacements need special care: GCC can subtract an index bias
from an array's base, leaving a displacement inside an unrelated neighboring
object. In `has_subtype_by_id`, both binaries use the same subtype table at
`base + player * 128 + (index - 5) * 8`; a displacement-only scan incorrectly
called these accesses different globals. Check the complete effective address
before editing source from a named-data difference.

Setup resolves C name collisions using the owning source compilation unit and
unique function ranges. When a function has split ranges, it selects only the
entry identified by an exact COFF symbol on both sides. The generated CSV carries
explicit rebuilt addresses, using the fork's existing address-hint support.
This rule does not pair private parts by order or automatically resolve C++
overloads. Name-state receipts track mapping identities independently of source
verification summaries; source pins and binary/debug hashes remain checked.

The refreshed comparison verifies all 160 explicit bindings, including 103
previously excluded ranges. Of the original 18,725 ranges, 90 more now score
exactly; the combined metadata and source corrections reduce the small-diff
queue from 494 to 399. Twelve of the fourteen corrected function owners score
exactly. Inquisitor Exarch and Dromar's Charm retain other differences. Scores
can also change when resolved names alter diff alignment; see the checked
[binding and comparison record](recovery-evidence/reccmp-source-bindings.json).

The target ID is `MANALINK2022`. To use a different fork location, put
`--reccmp-root PATH` before `setup` or `compare`. Setup also accepts
`--library`, `--original`, `--original-debug`, `--rebuilt`, and `--rebuilt-debug`.
Run `python tools/reccmp_project.py setup --help` for the defaults. Reccmp's
DWARF parser takes roughly a minute on the current build and uses several GB
of memory. Generating all the assembly diffs takes additional time.

Setup caches native and rebuilt function bounds by debug-file hash, bounds-reader
implementation and inclusion mode. Reusing unchanged inputs does not parse their
DWARF again. Routine C++ chooser coverage uses current canonical source pins and
body hashes while keeping original review hashes for historical verification.

## Inputs and metadata

`reccmp-project.yml` pins the October 2022 Hotfix DLL SHA-256. Setup uses
`.recovery/2022-ManalinkEh.dll`, or extracts the archive-root `ManalinkEh.dll`
from `G:\Manalink\Manalink-3_0-Oct-2022_Hotfix.zip` if the copy is absent. The
archive also contains an obsolete nested DLL; setup deliberately avoids it.
The matching original sidecar is
`G:\Manalink\ManalinkEh_10_22_Hotfix.dbg`; its hash is also pinned in the setup
tool. The recompiled inputs are the root `ManalinkEh.dll` and `ManalinkEh.dbg`.
Setup verifies their GNU debuglink CRC when present.

Routine setup checks the current source-file hashes and canonical ledger/name
references, original binary hashes, the rebuilt debuglink CRC, and all 15,169
built jump-table slots against their COFF targets. It does not replay archived
source edits, old compiler outputs, or the historical per-batch semantic audits.
Archived compiler/runtime snapshots have now been deliberately retired to free
disk space. Full historical replay (`setup --audit-recovery` or
`canonicalize_helpers.py --verify`) reports that retirement immediately. The
[retirement record](recovery-evidence/historical-artifact-retirement.json)
lists the retained inputs and deleted artifacts. The setup report's
`recovery_audit_mode` reports `current_inputs`; historical receipts may contain
`full_history`. Routine setup must not be described as a full recovery audit.

After changing recovery code, build and run the focused checker for the
affected behavior or representation. Regenerating the comparison CSV does not
require rerunning every earlier reconstruction experiment. Historical evidence
and its standalone audit tools remain records of the completed work; old
standalone verifiers may reference retired snapshots. Moving forward, use current
input checks, the current batch checker and a current build.

For a source-only checkpoint, use
`python tools/recovery/canonicalize_helpers.py --verify-current`; it checks the
live source hashes and canonical ledger/name references without replaying old
experiments. `audit_compiler_parts.py` also uses this current-input check by
default. Its historical `--audit-recovery` option is retired with the archived
compiler/runtime inputs.

The old alias/layout reconstruction audits, archived compiler probes, and
predecessor-source replay are historical evidence. Do not run them after every
readability batch. `audit_source.py`, `check_source.py`, and
`inventory_helper_pairs.py` retain that historical scope and are investigation
tools, rather than routine recovery gates. For normal work, check the changed
functions, build once per batch, and regenerate comparison metadata when needed.

Setup writes two ignored, machine-specific files at the repository root:

- `reccmp-user.yml`: original DLL path.
- `reccmp-build.yml`: rebuilt DLL path and `.dbg` sidecar. The schema still calls
  this field `pdb`; the fork selects its DWARF reader from the `.dbg` extension.

`config/reccmp/functions.csv` contains original-side **absolute virtual
addresses**, names, and exact contiguous DWARF sizes. These are implementation
addresses, rather than the `Manalink.asm` jump-table veneers. No Ghidra export
or bulk source-comment annotations are required. The CSV's extra provenance
columns are ignored by reccmp.

The generator reads native file-scope card/helper functions from the original
DWARF. For an address in the recovery ledger, it translates the historical
recovery name through the current canonical function-name map. The CSV `name`
column therefore matches the real rebuilt function name; historical recovery
identities remain in the evidence ledger. Other functions use their original
names. A pair is emitted only when the name identifies one original
address and one rebuilt address. The rebuilt inventory includes nested C++
and runtime functions to catch name collisions across compilation units.
Missing functions and ambiguous names are listed, with reasons, in
`.recovery/reccmp/excluded-functions.json`. Functions without contiguous
low/high DWARF bounds are outside this initial CSV's scope.

Rerun setup after a rebuild, recovery-ledger mapping change or canonicalization
change. The wrapper rejects binary/debug inputs, metadata or source name mappings
whose hashes differ from the last setup.
`.recovery/reccmp/setup.json` records the input hashes and coverage statistics.
Build outputs, local configs, reports, and exclusions remain ignored; the
project configuration, generated function/data CSVs, and tools belong in Git.

## Reading the results

This compares the **current recovery branch** with the October 2022 DLL. It
does not recreate a pristine 2020 source build or classify historical changes
by itself. The report includes retained original-source functions alongside
recovered functions, so its function count exceeds the recovery ledger count.

Use scores and assembly diffs to prioritize review. Compiler decisions,
retained compatibility APIs, missing interfaces, and incomplete pointer/global
normalization can reduce scores without establishing a behavioral difference.
A high score is also not a gameplay correctness test. Reccmp's progress and
accuracy percentages are matching statistics, not source-recovery coverage.

The fork currently imports function and line information, but no DWARF global
variables or complete type/stack layouts. Global comparison and type-aware
`datacmp`/`stackcmp` are therefore outside this initial setup. Uniquely mapped
Unresolved source placeholders are marked `stub`, rather than recovered code;
a chooser optimized into an alias may have no separate DWARF body. The current
C++ picker recovery retires its resolved Wish chooser stubs. Current recovery
coverage and remaining runtime limits are recorded in
[recovery-scope.md](recovery-scope.md). Nothing here executes or
deploys either DLL.

## Initial validation

The 2026-10-26 canonical-name refresh pairs all 18,544 CSV entries under their
expected names, with unique rebuilt addresses: 18,543 functions and one stub,
including 4,770 recovery-ledger mappings. No CSV matching name or report name
contains `2022`. This replaces 129 stale recovery spellings from the previous
CSV. The HTML and JSON reports and their binary/metadata input receipt are
refreshed. The full GCC 9.3 build, source/ABI checks, provenance audit and native
build audit pass. Five private helpers have 23 verified concrete inline instances;
their caller-owned ranges are not treated as standalone helper entries.

Native verification exposed an accidental recursive fallback in the consolidated
`mana_producer_fixed`. Its fallback now uses the distinct four-argument
`engine_mana_producer_fixed` import at `0x47B6C0`, matching the release tail jump.
The reports and default triage caches have been refreshed after this repair.

The 2026-10-22 run used fork commit
`e9fbb784cd8f88120c198f7e7285e3bb5262da3f` and successfully generated both
reports. All 18,142 CSV entries paired under the expected names, with no reused
rebuilt addresses: 18,141 functions and one stub. This includes 4,048 explicit
recovery-ledger mappings; 707 other native functions were excluded with recorded
reasons. Of the compared functions, 3,262 received a 100% reccmp score, including
`phase_out_2022` and `card_black_lotus`. These are normalized assembly scores.
The full HTML report is about 90 MiB; the JSON summary is about 2 MiB.

## Helper triage and duplicate implementations

Use the current build and setup to refresh the prioritized helper inventory:

```powershell
python tools/recovery/triage_helpers.py --refresh
```

This runs reccmp JSON comparison, writes a report/input receipt, caches rebuilt
DWARF bounds by debug-file hash and classifies the helpers without recovery
records. Without `--refresh`, the tool reuses the report and rejects stale
inputs. JSON comparisons through `tools/reccmp_project.py` now produce an
adjacent `.inputs.json` receipt identifying the exact binary/debug pair,
metadata, recovery ledger and comparator implementation files.

The [helper triage summary](recovery-helper-triage.md) and
[detailed evidence](recovery-evidence/helper-triage.json) separate matching
local code, callee-binding changes, small-diff candidates, deeper review,
unresolved references and ambiguous mappings. A normalized score of 100% is
not enough: reccmp can assign matching placeholders to unknown addresses.
The stricter pass preserves constants and field offsets, verifies function
identities and exact source-string contents, and refuses to certify unknown
DLL data, indirect control flow, incomplete ranges or stub dependencies.
Dependency addresses remain recorded even when the caller's local body matches.

The initial 2026-10-25 pass classified 2,245 helper ranges: 442 local matches, 402
callee-binding-only cases, 259 small-diff candidates, 826 deeper-review
candidates, 75 reference reviews, 211 mapping reviews, 18 deferred chooser
ranges and twelve other C++ ranges. These counts prioritize work; they do not
increase the manual recovery ledger or establish historical changes/gameplay.
The strict comparison was checked against synthetic counterexamples involving
changed callees, numeric constants, layout offsets, unknown data, source
strings, incomplete code, indirect calls and code read as data.

It initially found 64 retained/recovered duplicate candidates: 41 verified compiler
thunks and 23 pairs with identical local code in the same rebuilt image.
Canonicalization should keep one intended 2022 implementation under its
canonical name, migrate callers, prototypes, callbacks and registration
bindings, and keep adapters only where an ABI requires them. Review pointer
identity as well as ABI. Existing provenance audits pin old names and retained
bodies, so their predecessor records and hashes must migrate explicitly;
removing bodies without updating that history would invalidate the evidence.
Compatible core APIs can fix many caller bindings together. For example,
`is_what_2022` needs rebinding at 131 sites in 105 pending helpers; 42 of those
helpers also need other callee bindings. This is a higher-priority integration
family than making 105 more copies of those callers.

The first unification pass now removes fifteen retained bodies: `is_what`,
`is_token`, and all thirteen special-flag APIs. Their public compatibility
names are GCC aliases to the recovered implementations, with identical COFF
addresses and no forwarding functions. The recovered spellings remain the
provenance anchors during the name migration. Old source bodies and recovery
records are preserved in [helper-unification.json](recovery-evidence/helper-unification.json).
The source audit checks the live recovered bodies and aliases independently
from the archived retained-body history. All 4,871 implementation records stay
unchanged; the provenance audit records the new sharing.

This makes seven additional callers identical enough for GCC to fold into
retained bodies. Setup now records 48 verified compiler folds, including the
41 it previously classified as missing rebuilt DWARF functions. The comparison
has 18,470 full-body pairs and 4,701 ledger mappings; those numbers omit the
48 recorded thunks. The previous build had 18,477 pairs and 4,708 mappings:
the seven-range reduction is compiler sharing, not lost recovery. Setup
verifies each actual JMP, target name/source, complete retained DWARF range
and target-byte hash. The triage pass recognizes both verified entry points
as valid callee bindings, without scoring a short thunk as a full body.

The [current summary](recovery-helper-triage.md) supersedes the initial queue
counts above: 577 local matches, 271 callee-binding-only cases, 267 small-diff,
809 deeper-review, 80 reference, 211 mapping and thirty deferred/C++ ranges.
The current 76 duplicate candidates exclude the fifteen shared pairs.
Run `python tools/recovery/unify_scalar_helpers.py --verify`
to check the sharing/provenance evidence without changing source. The GCC
build verifier additionally checks all fifteen actual address pairs.

The following core pass now brings the shared total to **107 pairs**, removing
92 more retained bodies across three source units while keeping all recovered
implementation records unchanged. This resolves 35 of the prior duplicate
candidates; two private compiler aliases become visible, leaving **43**.
The current queue is 687 local matches, 162 callee-binding-only cases, 273
small-diff, 797 deeper-review, 85 reference, 211 mapping and thirty deferred/C++
ranges. None of the 107 shared names remain as binding-repair target groups.

Current setup has **18,493 full-body pairs / 4,724 ledger mappings**. It also
records 27 verified JMP thunks and two same-address private compiler aliases.
For the latter, COFF supplies the qualified symbol and the retained-name
DWARF DIE at the same address supplies the complete body range. Setup and the
strict pass verify that actual symbol, exact source/name, complete target
bytes and hash. These cases count as valid bindings without inventing a DWARF
range or comparing a short thunk against a full native implementation.

Subsequent duplicate closure and record migration bring the shared total to
**222 helper pairs**: 219 same-address aliases and three explicitly typed
const-return tail adapters. The original target/test/token/pump type names now
describe the recovered 84/216/76/48-byte records; the versioned typedefs name
those same types. Original token action constants also use the native 2022
bits. Seven card-handler scratch areas are restored as typed unions using
matching frame-base locations in the original DWARF. This changes the mixed
build substantially, so the 107-pair queue above is a historical checkpoint.
Use the [current triage](recovery-helper-triage.md) and its pinned input receipt.

[helper-pairs.json](recovery-evidence/helper-pairs.json) inventories accepted
retained/recovered pairs even when their bodies differ. A shared classification
requires the exact native address as well as source/name identity; compiler
workers inheriting a parent's name do not count as another resolved pair.
Matching C declarations remain semantic review opportunities. Canonical record
types remove a layout obstacle without proving that every remaining caller,
flag domain or helper body already matches the native DLL.

The 222-pair checkpoint maps **18,521 full bodies / 4,753 recovery-ledger entries**
plus one verified compiler thunk. Strict triage finds 752 local matches, 209
binding-only, 316 small-diff, 637 deeper-review, 89 reference, 212 mapping and
thirty deferred/C++ ranges. Nine duplicate candidates remain visible; no
already-shared API remains in the binding target groups. Scores and matching
local instructions remain review evidence, not a whole-program correctness
or recovery-completion percentage.

The next mana/activation/token pass consolidates another 65 helper pairs,
bringing sharing to **287 pairs: 284 same-address aliases and three typed tail
adapters**. The nine duplicate candidates at the previous checkpoint are
resolved, along with 56 compatible runtime APIs. GCC outlines one public
mana helper into its canonical entry and a separately verified 442-byte
`.part.0` worker. Native validation follows that edge explicitly; comparison
setup continues to exclude ambiguous ranges rather than treating an outlined
worker as an independently recovered complete function. Use the latest
[triage summary](recovery-helper-triage.md) and its pinned receipts for current
queue counts. The accepted implementation count remains 4,871.

The 287-pair refresh maps **18,519 full bodies / 4,751 ledger entries**, with
no remaining standalone compiler-thunk mappings. Strict local matches rise
from 752 to **830** and binding-only cases fall from 209 to **124**. The other
queues contain 317 small-diff, 631 deeper-review, 101 reference and 212 mapping
cases, plus thirty deferred/C++ ranges. Three identical-code candidates are
newly exposed. No already-shared helper remains a binding target; generic
activation, reanimation and life loss now lead that queue.

The activation/effect pass consolidates **44 further pairs**, reaching **331:
328 same-address aliases and three typed const-return tail adapters**. Its three
activation prompt interfaces use checked read-only source annotations; native
recovery signatures and executable expressions remain unchanged. The current
setup maps **18,526 full bodies / 4,750 ledger entries**, with no separately
scored compiler thunks. The three previously exposed identical-code candidates
are resolved. The candidate list is empty, while differing-body and interface
pairs remain review work in [helper-pairs.json](recovery-evidence/helper-pairs.json).

Strict local matches increase **830 → 880** and binding-only cases decrease
**124 → 70**. Other queues contain 320 small-diff, 636 deeper-review, 105 reference,
204 mapping and thirty deferred/C++ ranges. Of the 880 local matches, 714 still
have pending callees in their recorded dependency lists. No already-shared API
remains a binding-repair target; reanimation is now the largest group. All binary,
metadata, source, ledger and comparator receipts are current. These counts refer
to local review categories, not completed gameplay recovery.

The reanimation/spell/Aura/tutor pass adds **40 shared pairs**, reaching **371:
368 same-address aliases and three typed const-return tail adapters**. It also
retires one unused private eight-argument predecessor separately from alias
counts; the native five-argument mass worker remains. Four canonical action/value
domains now use the native definitions throughout the source: signed reanimation,
generic-spell flags, tutor destinations and tutor origins. Native enums/callbacks
were re-read from release DWARF, and the source/ABI checks pin the full maps and
actual typedef identities. Remaining literal caller operands still need review.

The refreshed setup retains **18,526 full bodies / 4,750 ledger mappings**, with
no separately scored compiler thunks. Strict local matches increase **880 → 919**;
binding-only cases fall **70 → 40**. Other queues contain 310 small-diff, 627
deeper-review, 115 reference, 204 mapping and thirty deferred/C++ ranges. Of the
919 local matches, 749 retain pending callees. One identical-code candidate is
newly visible: `tutor_card_with_the_same_name`. Already-shared APIs are absent
from the remaining binding target groups. Source provenance, compiler-part,
full GCC/native checks and every input/source/comparator receipt pass.

GCC now gives the shared name getter its own complete recovered-name DWARF body
and outlines a 91-byte color-text worker. Native verification checks the getter's
same-address alias, full worker range and canonical entry edge. The worker's
merged constant table has no DWARF location; its actual private COFF objects,
indexed load, preserved DWARF type and complete table values prove the emitted
view. Ambiguous ranges continue to be excluded from full-body comparisons.

The post-lookup refresh covers 18,828 ranges: 12,531 exact, 273 effective and
6,024 different, with 394 tiny-diff candidates. Relative to the preceding tiny-return
snapshot, seven scores improve and none decreases. Chrome Mox and both face-ID
helpers become exact; the split-card ID helper rises from 22.8% to 72.8%, while
its direct native/rebuilt lookup checks remain the functional evidence. All 160
explicit function bindings and 293 actual data matches pass.

The following [test-score control-flow pass](recovery-evidence/native-test-score-control-flow.json)
reconstructs the legacy and record-based filter scorers as explicit field/mode
switches. It removes 2,238 bytes of temporaries and inverted arithmetic while
preserving the native differences in subtype modes, ID sign checks, color/type
mode bits and ownership. K'rrik and Jasconian Isle also replace their existing
numeric card-ID cases with release-verified names. One build, all 15,169 fixed
slots and full source-file projections pass.

Direct release/actual-rebuilt/standalone checks agree in 997,102 cases: 102,400
legacy-score cases, 887,500 record-score cases and 7,202 unchanged display-predicate
fixtures. Scorer inputs cover representative field/mode combinations plus 100,000
deterministic boundary/random inputs per interface. Display fixtures flip every
bit in the 300-byte instance over three baselines and check pointer identity.
Native/current test-record size and member geometry agree at 216 bytes. These
checks establish the recorded domain, not every possible record or game state.
The full report above now precedes this batch; its 394 candidates remain a pinned
discovery snapshot until the next comparison refresh.

The [card-ID naming pass](recovery-evidence/native-card-id-references.json)
replaces 628 numeric references with 351 release-verified enum names across 422
existing functions in 90 files. GCC checks every selected value against the
original literal. All 90 allocated-object code/data/symbol/relocation snapshots
and the DLL's runtime sections remain identical; debug and export metadata are
excluded from that comparison and fixed-entry checks pass separately. The build,
15,169 fixed slots and independent literal-only projections pass. This recovers
identifiers, not new handlers or changed behavior. The full report above predates
this pass and the scorer reconstruction.

The following [JSON refresh](recovery-evidence/reccmp-small-diff-refresh.json)
includes both passes. Its counts remain 12,531 exact, 273 effective and 6,024
different. All 160 explicit function entries and 293 data bindings pass. Only
the two scorer scores change: their readable switches score lower despite the
separate 997,102-case behavior checks. Scores guide review rather than source
shaping. The default 394 candidates form 300 textual groups; including short
functions below the score cutoff yields 617 candidates in 440 groups.

That expanded queue exposes the 30-byte native
[partner-ID lookup](recovery-evidence/native-partner-id-table.json). Its eleven
mappings now use named cases in `cards/vanguard.c`, preserving the native missing
Regna-to-Krav direction and -1 default. One build, 15,169 fixed slots and 65,543
release/actual-rebuilt/source cases pass. The rebuilt instruction sequence matches
the release after normalizing only the relocated table and branch base; all 118
table bytes also match. The full JSON refresh predates this final source edit;
its receipt keeps that snapshot distinct from the focused compiled verification.

The [counter/commander decision pass](recovery-evidence/native-counter-decision-control-flow.json)
then reconstructs five counter-card trees, 36 Boolean priority expressions and
18 commander exceptions as explicit named decisions. It checks 165 release enum
values and names the natural-counter/type guards. The counter helper loses 225
lines; native quirks and helper-call ordering remain intact. One combined build,
15,169 fixed entries and 14,290,240 native/actual-rebuilt/old/new-source fixtures
pass, comparing returns and ordered leaf-helper arguments. The harness maps the
DLLs without entry/import execution and supplies explicit engine-call/data
fixtures; this checks caller decisions rather than engine behavior. The full
JSON report above remains the earlier pinned snapshot.

The [typed Boolean-choice pass](recovery-evidence/native-boolean-choices.json)
replaces 173 mask/arithmetic encodings with explicit choices in 138 existing
functions across 62 files. Both Boolean outcomes and the exact original integer
type are checked by GCC, including type contracts in each original lexical
scope. All 346 outcome checks, 173 scope contracts, full source projections,
unchanged line counts, one build and 15,169 fixed slots pass. Signed outcomes
fit int32; unsigned expressions retain their type. Assertions, stringifying
macros and preprocessor directives are excluded. Twenty-eight objects retain
identical runtime code/data; 34 differ. This is expression-equivalent source
reconstruction, not new handlers, native whole-function validation or a score
improvement claim. The last full JSON report still predates this batch.

The [counter-wrapper pass](recovery-evidence/native-counter-wrapper-control-flow.json)
reconstructs nine placement/cost/add/remove/clear wrappers as 91 lines instead of
154, names four blocker cards and exposes the low-byte remove-all sentinel.
The storage core remains unchanged. One build and all 15,169 fixed entries pass.
Across ten interfaces, 360,502 release/rebuilt/old/new fixtures agree on complete
300-byte records, globals, returns, abort paths and ordered leaf-helper traces.
Engine calls/data are explicit fixtures. The predecessor cost worker required a
hoisted-guard adapter, with both compiled callers checked for a dominating guard;
the new build emits the full cost body and needs no such adapter. Native callback
amount-discard behavior, parent redirection and extreme-amount wraparound remain.
These are focused caller/storage checks; the full JSON report remains historical.

The next [full refresh](recovery-evidence/reccmp-counter-refresh.json) adds seven
exact ranges and one effective range, with no prior exact lost. Sixty-six scores
improve and ten decrease; separate behavior/type checks remain the evidence for
the readability rewrites. All 160 explicit entries and 293 data bindings pass.
The default queue has 393 candidates in 299 textual groups; including lower-score
short functions yields 612 candidates in 435 groups. Three five-byte forwarders
(`card_detonate`, `card_whimsy`, `draw_cards`) score zero despite identical decoded
external jump targets. External-target matching is the next comparison inquiry.

The subsequent [residual-choice pass](recovery-evidence/native-residual-choices.json)
recovers nine pack-weight, ward-priority and named artifact/colorless mana choices
in six existing functions. Eighteen typed outcome checks, nine original-scope
type contracts, five full-file projections, one build and all 15,169 fixed entries
pass. A pack weight also uses a checked preceding Boolean assignment. Types,
evaluation count and line counts remain intact. The full report above predates
this final expression batch; its receipt identifies the precise snapshot.

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

To annotate a queue after generating its full JSON, pass the source batch receipt
that pins the current successful DLL/debug build. Update `--build-evidence` when
using a newer build. The reviewer rebinds current entries from debug symbols and
leaves unsupported shapes unresolved:

```powershell
python tools/recovery/review_event_guards.py --queue .recovery/reccmp/small-diffs-all-scores.json --build-evidence docs/recovery-evidence/native-token-marking-initializers.json --output .recovery/reccmp/event-review.json
python tools/recovery/small_diff_queue.py --report .recovery/reccmp/small-diffs.json --min-score 0 --event-review .recovery/reccmp/event-review.json --output .recovery/reccmp/reviewed-small-diffs.json
```

`change_site_review` marks only the listed predicate differences. Scores and
whole-function review status remain unchanged. Omit `--event-review` to generate
the ordinary unannotated queue.

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

Generate address annotations against the same full queue, then combine them:

```powershell
python tools/recovery/review_card_data_bias.py --queue .recovery/reccmp/small-diffs-all-scores.json --build-evidence docs/recovery-evidence/native-token-marking-initializers.json --output .recovery/reccmp/address-review.json
python tools/recovery/small_diff_queue.py --report .recovery/reccmp/small-diffs.json --min-score 0 --event-review .recovery/reccmp/event-review.json --address-review .recovery/reccmp/address-review.json --output .recovery/reccmp/reviewed-small-diffs.json
```

`--address-review` adds `verified_card_data_address_bias` site annotations.
`site_review_summary` counts the union of annotations. Overlapping annotations
require a combined review and are refused. Both review options are optional;
ordinary scores and whole-function status remain unchanged.

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

Assertion-location differences can be annotated separately:

```powershell
python tools/recovery/review_diagnostic_lines.py --queue .recovery/reccmp/small-diffs-all-scores.json --build-evidence docs/recovery-evidence/native-token-marking-initializers.json --output .recovery/reccmp/diagnostic-review.json
python tools/recovery/small_diff_queue.py --report .recovery/reccmp/small-diffs.json --min-score 0 --event-review .recovery/reccmp/event-review.json --address-review .recovery/reccmp/address-review.json --diagnostic-review .recovery/reccmp/diagnostic-review.json --output .recovery/reccmp/reviewed-small-diffs.json
```

`--diagnostic-review` adds `diagnostic_source_lines_only` annotations. A different
path, assertion expression, instruction width or pointer-store destination is
left unresolved. Source-line output differences are retained, and scores are
never promoted to exact/effective by these annotations.

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

To correlate layout and shared-tail cases, start with the queue containing prior
annotations, so their candidates remain separate:

```powershell
python tools/recovery/review_branch_layout.py --queue .recovery/reccmp/reviewed-small-diffs.json --build-evidence docs/recovery-evidence/native-token-marking-initializers.json --equality-guards --void-equality-guards --return-epilogues --output .recovery/reccmp/cfg-review.json
python tools/recovery/small_diff_queue.py --report .recovery/reccmp/small-diffs.json --min-score 0 --event-review .recovery/reccmp/event-review.json --address-review .recovery/reccmp/address-review.json --diagnostic-review .recovery/reccmp/diagnostic-review.json --cfg-review .recovery/reccmp/cfg-review.json --output .recovery/reccmp/cfg-reviewed-small-diffs.json
```

`--cfg-review` adds `structural_cfg_correspondence` annotations. The reader caches
current DWARF bounds by debug-file and reader hashes, then decodes the current
native/rebuilt ranges. Matching structure narrows manual work; it does not turn
an address/name correspondence into a callee, data-content or gameplay proof.

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

To expand the workflow from tiny deltas to the complete differing-range backlog,
add `--min-score 0 --max-changed-per-side 50000` to both queue-generation commands
and feed the full queue to the CFG reviewer. Retain the event/address/diagnostic
options when generating that input queue. The [broad-pass receipt](recovery-evidence/reccmp-broad-cfg-review.json)
records the exact three commands used for the 5,993-range pass.

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
