# Building the recovery branch

The 2026-10-22 recovery build successfully compiles, assembles and links a
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

`Makefile-common` now allows warnings by default. `WERROR=-Werror` restores
fatal warnings for make users. The stricter recovery checks remain available:

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
