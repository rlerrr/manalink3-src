# Building the recovery branch

The 2026-10-23 recovery build successfully compiles, assembles and links a
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
successful build uses 164 C files, two C++ files, 20 linked assembly objects
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
