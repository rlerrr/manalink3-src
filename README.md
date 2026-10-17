# Manalink3 source

Initially forked from https://github.com/ShandalarMagic/Shandalar

Updated source applied from [test build 12-1-2020.rar](https://slightlymagic.net/forum/viewtopic.php?f=85&t=29228) seemingly the last publicly released source?

Post-release card and helper implementations are being reconstructed from the
October 2022 Hotfix binaries and matching DWARF debug data. Recovered functions
live in their original source units and include binary-address provenance.
See [recovery progress](docs/recovery-progress.md) for coverage, tooling and the
remaining downstream integration work. Behavioral tests are deferred.
