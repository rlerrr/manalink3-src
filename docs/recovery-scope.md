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

The first inline-name reconciliation leaves 103 inline-only names for caller
analysis, plus eight names with ordinary entries needing binding follow-up.
These may already be expanded into accepted parent bodies or represent C++
implicit members/operators. Name absence alone is not a missing-code finding.

Dynamic locations, EXE-owned storage, constructor effects, runtime state and
complete global declaration ABI remain separate evidence. Gameplay tests
remain deferred by the user.
