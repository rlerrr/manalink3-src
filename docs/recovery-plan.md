# Recovering the post-source-release Manalink changes

Inspection date: 2026-10-17. Repository baseline: `427968c`, including the
January 12, 2020 source snapshot applied by `cad36b4`. The archive name
`test build 12-1-2020` uses day-month ordering; it is not December 2020.

Recovery has started: see [the first source-recovery batch](recovery-progress.md)
for the implementations now placed in the original source tree and the remaining work.

## Recommendation

Proceed. This is unusually favorable for reconstructing a native-code update:
we have working older source, its matching binary/debug pair, several intermediate
binary/debug pairs, and a richly symbolized final target.

Use **DWARF and COFF identities first, then Ghidra comparison and decompilation**.
Version Tracking is useful, but discovering most function identities should not
require fuzzy binary matching. The difficult work is deciding which behaviors
changed and translating those changes into the existing source and build system.

The target is the **root `ManalinkEh.dll` in the October 2022 Hotfix ZIP**, paired
with `ManalinkEh_10_22_Hotfix.dbg`. The same ZIP also contains an obsolete
`ManaLink/ManalinkEh.dll` dated September 2020. Do not accidentally analyze that copy.

### Priority refinement: recover new cards and their helpers first

The registration path has now been decoded and verified against the entire
baseline assembly table. See [registration trace](registration-trace.md).
This gives a concrete first recovery backlog without requiring a whole-program
behavioral diff: start with new registered handlers marked CODED in `Cards.dat`,
then recover their helper dependencies and the ABI/engine changes those helpers
actually require. Use Version Tracking for ambiguous or changed dependencies.

The target has 3,310 registered handler symbols absent from the baseline,
3,217 of which serve at least one record marked CODED. These include newly
implemented older cards, renames, tokens and effects as well as new printings.
There are 1,269 target records that both use a veneer and have a new data ID and
a CODED flag. Neither count is a tested-playability claim.

The fixed-address table can be regenerated from the DLL. Its 15,169 entries
all resolve to named implementation symbols; 3,355 slots were appended and 44
old slots were retargeted. Preserve the target mapping and its matching card
data rather than appending only entries to the baseline table.

## What was verified locally

| Item | Finding |
| --- | --- |
| Latest local release pair | `G:\Manalink\Manalink-3_0-Oct-2022_Hotfix.zip` and `G:\Manalink\ManalinkEh_10_22_Hotfix.dbg` |
| Target architecture | 32-bit x86 PE DLL, preferred image base `0x02000000`, entry RVA `0x1000` |
| Release DLL | 8,922,311 bytes; ZIP entry dated October 14, 2022 |
| Debug file | 129,795,835 bytes; **runtime sections contain no code/data payload** |
| Pair validation | All 26 runtime section names/RVAs/virtual sizes agree; all 19,774 defined COFF function records agree, including name, section, offset, type and storage class |
| Baseline pair | Extracted January 2020 DLL and repo `ManalinkEh.dbg`: all 17 runtime layouts and all function records agree |
| Debug format | DWARF 4 inside PE/COFF, not a Microsoft PDB |
| Target DWARF payload | `.debug_info` 120,059,124 bytes; `.debug_line` 3,535,305 bytes; also abbreviation, type-string, frame, location and range sections |
| Compilation units | Baseline: 166 total, 145 project units. Target: 200 total, 170 project units |
| Project function metadata | Target has 18,849 file-scope subprogram DIEs with `low_pc`, plus 5 with range-list attributes. This is not a count of changed functions or a deduplicated function inventory |
| Compiler | Baseline project: GCC/G++ 4.8.1. Target project: GCC/G++ 9.3-win32 20200320, `-O2`, `-march=i586`, frame pointers, `-ggdb2` |
| Other tools available | Ghidra 12.0, JDK 21, LLVM tools, Python with pefile, pyelftools and Capstone |

Both target PE timestamps are zero; filename/date alone would not establish a
match. Section layouts and the complete agreement of function records provide
strong pairing evidence. There is no independent code hash embedded in the
code-empty DBG that proves the original code bytes.

SHA-256 identities:

```text
2022 ZIP  6ba126c2795db8a472239ce37b93cc4a388082fe4a471f3aa51f7ffc8263c178
2022 DLL  52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5
2022 DBG  7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85
2020 DLL  927d076ca9e6a46b8a2bde0aeeed5e3b847ec8f55d22cebc9f92744bea460759
2020 DBG  b49af15964635442a12416b993764d70c3b31866418b12c8ee4af0ef027cf4f3
```

The existing `G:\Manalink\source_inventory.md` describes the DBG as containing
code. Its PE section headers describe the code's addresses and sizes, but its
`.text` has zero raw bytes. The local build rule confirms the intended process:
`objcopy --only-keep-debug` followed by `objcopy --strip-debug`.

### How much useful metadata survives?

Actual examples extracted from the target:

- `card_professor_onyx`: `cards/strixhaven.c:5351`, address `0x023d7d50`,
  948 machine-code bytes, parameters `player`, `card`, `event`, and 70 line-table
  rows within its range.
- `dispatch_trigger`: `functions/events.c:507`, named parameters including
  `TENTATIVE_allow_response`, and the named local `old_xtrigger` with a location-list
  reference.
- `target_t`: fields `player` at offset 0 and `card` at offset 4, total size 8.
- `card_instance_t`: size 300 in both builds, with named fields and offsets.
  The target changes the member/anonymous-union structure: equal total size does
  not make the old header a safe description of the new ABI.
- `event_t`: 132 enumerators in the baseline versus 154 in the target.
  `EVENT_CANT_CREW` changes from `0x4f00` to `0x1d00`. Added names include
  `EVENT_QUERY_CUMULATIVE_UPKEEP`, `EVENT_SPELL_COPIED`, and saga chapter events.
- `card_id_t`: the full target enumeration exposes 24,862 named constants,
  versus 19,739 in the baseline. These values can be exported directly rather
  than inferred from individual card implementations. Release data still needs
  checking for consistency with the recovered dispatch tables.

Extract types, signatures and constants **before interpreting decompiled code**.
DWARF type references, abstract origins, specification references, anonymous
unions and range lists must be resolved, rather than reading only each DIE's
immediate name/address attributes.

### What the symbol counts do and do not mean

COFF defined function-name sets:

| | Baseline | Target |
| --- | ---: | ---: |
| Unique function names, including runtime/compiler code | 15,392 | 19,704 |
| Names beginning `_card_` | 11,809 | 15,095 |

There are 14,936 shared names, 4,768 target-only names and 456 baseline-only
names. These are **triage numbers**, not 4,768 new implemented game functions:
they include C++ runtime code, compiler-generated helpers, renames, and stubs.
For example, the new `cards/stubs.c` contains `card_crab`, a three-byte
`xor eax,eax; ret` function.

The target adds 28 project CU paths and loses 3. Some are moves or renames:
`cards/odissey.c` becomes `cards/odyssey.c`, and draft code moves from `cards/`
to `functions/`. Newly represented sets include Modern Horizons, Throne of
Eldraine, Ikoria, Zendikar Rising, Kaldheim, Strixhaven, Forgotten Realms,
Midnight Hunt and Crimson Vow. New supporting units include
`functions/{dice.c,load_text.c,packs.c,choose_a_card.cpp}`.

### Intermediate releases are usable

All five of the following local ZIP/DBG pairs pass the same section-layout and
function-record checks, with zero function-record differences:

| Release | Project compiler |
| --- | --- |
| October 2020 v1.1 | GCC 4.8.1 |
| December 2020 patch | GCC 4.8.1 |
| March 2021 v1.1 | GCC 4.8.1 |
| July 2021 v1.1 | GCC 9.3, C17 |
| December 2021 | GCC 9.3, C99 |
| October 2022 Hotfix | GCC 9.3, C99 |

This locates the major compiler transition between March and July 2021. Use
adjacent releases to narrow a difficult change, especially across that boundary.
Keep a direct baseline-to-target inventory as the authoritative scope; do not
require reconstructing every historical release first. The separate July patch
DBG has not been paired with its own program package.

## Decompilation feasibility check already completed

Ghidra 12.0 headless successfully imported both actual DLLs and decompiled
selected functions using their DWARF addresses/ranges and manually supplied
card signatures. This pilot did **not** import the complete DWARF database or
run whole-program analysis. Raw outputs retain unresolved callee names; the
names below were independently resolved through the COFF symbol tables.

1. **Control: Black Lotus.** Both builds have a 49-byte function implementing
   `return artifact_mana_all_one_color(player, card, event, 3, 1);`, which agrees
   with the existing source. Its call target address changes, illustrating why
   raw byte inequality is not itself a source change.
2. **Actual changed wrapper: Winter Orb.** The baseline has a 51-byte wrapper
   calling `untap_only_one_permanent_type`. The target has an 84-byte wrapper
   which first returns if `is_tapped(player, card)` is nonzero. The target's
   `is_tapped` address is `0x004e81fe`, outside the DLL: it is an existing
   executable integration boundary that analysis must model.
3. **New implementation: Professor Onyx.** Ghidra recovered readable control
   flow with calls that COFF identifies as `magecraft_trigger`, `dialog_impl`,
   `reveal_top_cards_of_library_and_choose`, `discard`, and `planeswalker`.
   Complete types, strings and callee signatures should improve this considerably.

The Winter Orb observation supports this approximate target wrapper:

```c
int card_winter_orb(int player, int card, event_t event)
{
    if (!is_tapped(player, card))
        untap_only_one_permanent_type(player, card, event, ANYBODY, TYPE_LAND);
    return 0;
}
```

This is an illustrative reconstruction, not an applied patch or a claim that
Winter Orb is fully recovered. Its helper and engine behavior also need comparison.
There is an actual source patch in
`G:\Manalink\forum_downloads\WinterOrbPatch.zip`, changing the helper and
`functions/engine.c`. Use it as source evidence, and verify how it relates to
the target instead of assuming the 2020 patch is the final implementation.

## Execution plan

### 1. Preserve inputs and generate a recovery database

- Preserve originals on `G:`. Work in ignored analysis storage and an isolated
  recovery branch/check-out when source editing starts.
- Record archive entry paths, hashes, machine/image base, section layouts,
  all symbols, source units, compiler flags and pair-check results.
- Export DWARF types, globals, enums, parameters, local names/locations,
  inline instances and source-line mappings into a queryable database.
- Give each entity a stable key: release, CU path, linkage name, DIE offset
  and address ranges. Static names are only unique within a unit; compiler
  clones and aliases need explicit records. Preserve raw and normalized names.
- Inventory the release's card data, strings and dispatch tables alongside code.

Deliverable: verified inputs plus a searchable function/type inventory with
every project entity either mapped to baseline source or explicitly unmatched.

### 2. Establish reliable symbolized Ghidra imports

Import the **DLL code plus its matching DWARF**, preserving `0x02000000` as
the image base. Use the x86 32-bit GCC compiler model and validate stack layout
against the debug metadata and the baseline control.

The installed Ghidra supports DWARF in PE, but its automatic external-DWARF
provider loads an **ELF** debug file. Merely putting this PE `.dbg` beside the
DLL or treating it as a PDB is not a verified integration method.

Recommended implementation spike:

1. Create an analysis-only PE combining the verified DLL's runtime payloads
   with the DBG's original debug sections and COFF metadata, or use a Ghidra
   script to populate the DBG program's empty runtime blocks from the DLL.
   Retain every runtime RVA, section permission and preferred base. A rebuilt
   PE must update raw offsets/alignment/header sizes consistently.
2. Verify every runtime section's bytes against the DLL and every debug
   section's bytes against the DBG. Save the derivation manifest. This file
   is an analysis artifact, not a replacement game DLL.
3. Run DWARF import, inspect diagnostics and memory use, and verify known
   functions, `event_t`, packed structures, anonymous unions and source mapping.
4. If full import has problems, export/import types and function records with
   a script using the Python extractor; bring up selected units first. Retain
   omitted/failed records in the recovery inventory.

Also import relevant `Magic.exe` code/labels and model absolute symbols from
`manalink.lds`. Recover target absolute symbols from COFF rather than assuming
all old addresses remain valid. Set callee prototypes and return behavior
before assessing caller decompilation. Keep separate programs for releases.

Deliverable: symbolized baseline/target programs and a repeated pilot with
correct types, named calls, strings and credible decompiler signatures.

### 3. Compare identities and behavior

- Match unique linkage names first; use CU-qualified names for statics.
  Account for moved files, `.isra`/`.constprop` clones, inlining and split ranges.
- Use Version Tracking with the known-source baseline as source and 2022 as
  destination. Start with exact symbol names, then exact bytes/instructions,
  references and manually reviewed matches for remaining functions.
- Preserve target DWARF names/types. Accepting an identity match must not
  blindly overwrite a changed target signature or structure with old markup.
- Compare decoded instructions, CFGs, symbolic call targets, global references,
  strings and constants. Normalize address movement and relocation fields,
  while retaining semantic constants such as event/card IDs and masks.
  Mnemonics alone are too coarse to establish equal behavior.
- Use three views together: original baseline C, baseline decompilation and
  target decompilation. The baseline pair calibrates compiler/decompiler artifacts.
- Classify functions as unchanged with evidence, compiler/layout-only candidate,
  behavior changed, added, removed/renamed/inlined, or unresolved. A failed
  match is unresolved, not proof of a new function.
- For ambiguous large differences, use the intermediate release pairs to
  isolate when they appeared. Do not call every GCC 4.8-to-9.3 change a fix.

Deliverable: ranked recovery backlog with evidence, dependencies and confidence;
separate totals for game code, stubs and runtime/library code.

### 4. Recover into the existing source, in dependency order

First reconstruct changed ABI definitions and enums, global declarations,
function signatures, absolute bindings and dispatch/trampoline changes. DWARF
types can recover much of the declaration material directly; original macro
definitions generally need to be retained or inferred from the old source.

Next recover shared helpers and small coherent feature slices. Follow their
call dependencies rather than rewriting all engine code before testing a card.
Use a small unchanged control, a known changed wrapper/helper pair, one new
simple card, and one complex new card as the initial milestone. Include the
matching engine support where required.

Keep unchanged original C. Edit changed implementations using its style and
existing abstractions; translate raw offsets/constants back to verified fields
and enums. Use the new set CUs as the natural file layout for added cards.
Recover function tables, `.def` exports, jump-table assembly and fixed-address
bindings together with implementations. Regenerate the matching card IDs/data
from release data where possible; a stale `card_id.h` can silently misdispatch.

Record provenance per recovered function: binary hash/address/ranges, matching
source unit/line, relevant intermediate releases or original source patch,
assumptions, confidence and validation. Keep raw decompiler output separate from
reviewed compilable source.

Deliverable: small, reviewable commits that each build and implement a coherent
set of target behaviors, with an explicit unresolved list.

### 5. Validate against the shipped target

- Reproduce the baseline build first if feasible, using GCC 4.8.1 and existing
  objects as evidence. The repo's `-O2`/stack/frame settings matter. Do not block
  all recovery on producing an exact historical toolchain.
- Build target reconstructions with a pinned 32-bit MinGW GCC 9.3-compatible
  environment and the observed language/ABI flags. Verify packed layouts,
  calling conventions, link addresses, imports/exports and trampolines.
- Compare reconstructed function disassembly with the target. Exact equality
  is especially valuable on small leaf/wrapper functions; instruction/CFG
  differences on optimized code require explanation, not automatic rejection.
- Add focused tests for reconstructed logic and regression scenarios for
  engine/card interactions. Compare state transitions against the shipped game
  in a disposable installation: trigger ordering, targets, mana, zones and AI
  decisions. Control randomness where possible for reproducible comparisons.
- Run integration tests across feature slices. A correct isolated card can
  still depend on changed shared engine semantics or data IDs.

Deliverable: buildable recovered source with measured coverage and reproducible
behavioral evidence. Track coverage by functions, bytes and important behavior;
symbol counts alone are not a quality measure.

### 6. Account for the rest of the release

`Magic.exe`, `Drawcardlib.dll`, `CardArtLib.dll` and `DeckDll.dll` all have
different `.text` hashes from their baseline counterparts. Their target files
have no embedded DWARF. The three DLLs retain COFF symbols, while `Magic.exe`
has none. The main ManalinkEh DBG does not describe their implementations.

Prioritize ManalinkEh, but keep these modules as explicit remaining work.
Compare executable patch regions and existing `patches/*.pl` scripts; recover
changed patches and integration interfaces as needed. Use existing library
source and COFF names for the companion DLLs. Preserve release data and assets
needed to exercise the recovered code.

## Expected payoff and remaining uncertainties

The strongest recovery targets are declarations/types/enums, named card
functions, small helpers, dispatch tables and direct wrapper changes. Complex
engine behavior is feasible to investigate but needs more review and differential
testing. Optimization removes some source distinctions and variable lifetimes;
macros, comments and original formatting are not supplied by these debug sections.

No reliable percentage of recoverable source follows from the current counts.
Full DWARF-to-Ghidra import and 3,496 source implementations are now
complete; see [recovery progress](recovery-progress.md). This demonstrates batch
recovery into the original source units. Changed interfaces/layouts, remaining
implementations, downstream integration and game-behavior validation remain.
Version Tracking is optional support for later ambiguous dependencies.

## Saved evidence and reproduction

`tools/recovery/inspect_release.py` reads the inputs without executing them and
generates PE summaries, pair checks, CU/function CSVs and metadata examples.
It was run with pefile 2024.8.26 and pyelftools 0.33. With these existing Python
dependencies installed, run from the repository root:

```powershell
python tools/recovery/inspect_release.py --dwarf
```

Large generated evidence and copied binaries live in ignored `.recovery/`.
Compact inspection evidence is preserved under `docs/recovery-evidence/`.
The intermediate-release checks were also saved as `release-chain.json`.

To reproduce the selective Ghidra pilot, use a writable Ghidra settings/cache
directory and the installed JDK, then import the actual DLL with no global
analysis and run `tools/recovery/RecoveryPilot.java`:

```powershell
$env:XDG_CONFIG_HOME = "$PWD\.recovery\config"
$env:XDG_CACHE_HOME = "$PWD\.recovery\cache"
$env:JAVA_HOME = 'C:\Program Files\Eclipse Adoptium\jdk-21.0.9.10-hotspot'
& 'C:\bin\ghidra_12.0_PUBLIC\support\analyzeHeadless.bat' `
  "$PWD\.recovery\ghidra" RecoveryPilot `
  -import "$PWD\.recovery\2022-ManalinkEh.dll" -noanalysis `
  -scriptPath "$PWD\tools\recovery" `
  -postScript RecoveryPilot.java "$PWD\.recovery\2022-pilot.c" `
  card_black_lotus:24bf4e0:49 card_winter_orb:24cff90:84 card_professor_onyx:23d7d50:948
```

Use a fresh project name when rerunning an import into an existing project.
The baseline pilot uses `2020-ManalinkEh.dll`, Black Lotus `2356440:49` and
Winter Orb `23654f0:51`. The script deliberately supplies only card signatures;
it is not the proposed full debug importer.

References: local `Makefile-common`, `Makefile-targets`, source and actual PE/DWARF
records are the primary evidence. Ghidra implementation details were checked
against the installed 12.0 `Base-src.zip`. Upstream references:
[DWARF analyzer](https://github.com/NationalSecurityAgency/ghidra/blob/Ghidra_12.0_build/Ghidra/Features/Base/src/main/java/ghidra/app/plugin/core/analysis/DWARFAnalyzer.java),
[external debug provider](https://github.com/NationalSecurityAgency/ghidra/blob/Ghidra_12.0_build/Ghidra/Features/Base/src/main/java/ghidra/app/util/bin/format/dwarf/sectionprovider/ExternalDebugFileSectionProvider.java),
[Version Tracking correlators](https://github.com/NationalSecurityAgency/ghidra/blob/Ghidra_12.0_build/Ghidra/Features/VersionTracking/src/main/help/help/topics/VersionTrackingPlugin/VT_Correlators.html),
[GNU objcopy split-debug behavior](https://www.sourceware.org/binutils/docs/binutils/objcopy.html).
