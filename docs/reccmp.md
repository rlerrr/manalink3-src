# Comparing the recovery build with reccmp

This project uses the DWARF-enabled fork at `C:\dev\reccmp` (currently
`feat/dwarf-ingest`), rather than the MSVC-only upstream installation. No
changes to that checkout are needed. The wrapper imports that checkout
explicitly, so an installed upstream `reccmp-reccmp` cannot silently take its
place. The current local Python environment already has its dependencies.

From this repository root:

```powershell
# Build first when necessary; setup itself does not compile anything.
.\tools\build-recovery.ps1
python tools/reccmp_project.py setup

# Inspect one function, using its October 2022 absolute virtual address.
python tools/reccmp_project.py compare --verbose 0x2377a70 --print-rec-addr

# Generate the searchable assembly-diff report and machine-readable summary.
python tools/reccmp_project.py compare --html .recovery/reccmp/report.html --json .recovery/reccmp/report.json --json-diet --silent
```

The target ID is `MANALINK2022`. To use a different fork location, put
`--reccmp-root PATH` before `setup` or `compare`. Setup also accepts
`--library`, `--original`, `--original-debug`, `--rebuilt`, and `--rebuilt-debug`.
Run `python tools/reccmp_project.py setup --help` for the defaults. Reccmp's
DWARF parser takes roughly a minute on the current build and uses several GB
of memory. Generating all the assembly diffs takes additional time.

## Inputs and metadata

`reccmp-project.yml` pins the October 2022 Hotfix DLL SHA-256. Setup uses
`.recovery/2022-ManalinkEh.dll`, or extracts the archive-root `ManalinkEh.dll`
from `G:\Manalink\Manalink-3_0-Oct-2022_Hotfix.zip` if the copy is absent. The
archive also contains an obsolete nested DLL; setup deliberately avoids it.
The matching original sidecar is
`G:\Manalink\ManalinkEh_10_22_Hotfix.dbg`; its hash is also pinned in the setup
tool. The recompiled inputs are the root `ManalinkEh.dll` and `ManalinkEh.dbg`.
Setup verifies their GNU debuglink CRC when present.

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
DWARF. For an address in the recovery ledger, it uses the explicitly recorded
source name, including `_2022` compatibility names. Other functions use their
original names. A pair is emitted only when the name identifies one original
address and one rebuilt address. The rebuilt inventory includes nested C++
and runtime functions to catch name collisions across compilation units.
Missing functions and ambiguous names are listed, with reasons, in
`.recovery/reccmp/excluded-functions.json`. Functions without contiguous
low/high DWARF bounds are outside this initial CSV's scope.

Rerun setup after a rebuild or recovery-ledger change. The wrapper rejects
binary/debug inputs or metadata whose hashes differ from the last setup.
`.recovery/reccmp/setup.json` records the input hashes and coverage statistics.
Build outputs, local configs, reports, and exclusions remain ignored; the
project configuration, generated function CSV, and tools belong in Git.

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
Wish chooser placeholders are marked `stub`, rather than recovered code; a
chooser optimized into an alias may have no separate DWARF body. The chooser
dialogs remain [out of scope](recovery-scope.md). Nothing here executes or
deploys either DLL.

## Initial validation

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
