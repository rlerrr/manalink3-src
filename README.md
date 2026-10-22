# Manalink3 source

Initially forked from https://github.com/ShandalarMagic/Shandalar

Updated source applied from [test build 12-1-2020.rar](https://slightlymagic.net/forum/viewtopic.php?f=85&t=29228) seemingly the last publicly released source?

Post-release card and helper implementations are being reconstructed from the
October 2022 Hotfix binaries and matching DWARF debug data. Recovered functions
live in their original source units and include binary-address provenance.
See [recovery progress](docs/recovery-progress.md) for coverage, tooling and the
remaining downstream integration work. Behavioral tests are deferred.

Build the 32-bit recovery DLL with the local GCC 9.3 toolchain:

```powershell
.\tools\build-recovery.ps1
```

This creates `ManalinkEh.dll` and `ManalinkEh.dbg` in the repository root, with
objects and logs under `.recovery/build-gcc93/`. Warnings are allowed by default;
set `WERROR=-Werror` when invoking make to restore fatal warnings. See
[build details and validation scope](docs/recovery-build.md). The build is not
gameplay validated, and the Wish dialog entry points remain explicit stubs.

The local DWARF-enabled reccmp fork can compare this build with the October
2022 Hotfix. Run `python tools/reccmp_project.py setup`, then use the commands
in [the reccmp project guide](docs/reccmp.md) to inspect functions or generate
a searchable assembly-diff report.
