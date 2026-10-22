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
