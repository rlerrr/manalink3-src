# Registration-led recovery of new cards

Follow-up to [the recovery plan](recovery-plan.md), prioritizing new card
implementations and their shared helpers. These findings come from the actual
January 2020 and October 2022 binaries, the current source, `Cards.dat`, and
the original card-data layout in `C:\dev\shandalar-decomp\src\defs.h` and its
`tools/carddata_dump` utility. The latter repository was inspected read-only;
its `Ghidra` directory was excluded from the investigation.

## The complete registration chain is available

```text
Cards.dat entry: full name, CSV/data ID, CODED flag
          |
          | matched by ID (not by internal index)
          v
Magic.exe .cards: card_data_t[internal_id]
          |  record size 72; id +0x24; code_pointer +0x34
          v
ManalinkEh.dll fixed-address jump slot
          |  E9 <signed 32-bit displacement>, 5 bytes per slot
          v
Named function body, resolved through COFF
          |
          v
DWARF source unit, line, signature and function ranges
          |
          v
Calls to helpers and function-pointer/callback references
```

The source-level dispatch confirms this route:
`functions/events.c::call_card_function()` reads
`cards_data[instance->internal_card_id].code_pointer`, then calls the function
through `call_card_fn()`. The packed `card_data_t` definition in `defs.h`
agrees with the binary records. The modified executable stores the records at
`0x007e7010`, 16 bytes into `.cards`. There are 30,000 reserved record slots;
data after those slots is not another card record.

Internal IDs and CSV/data IDs are different. Internal indices can shift between
releases, several records can share a handler, and effect/dummy records are
present. Keep every internal record while joining releases by data ID and
checking names and handler identities.

## The assembly table is straightforward to regenerate

Both DLLs begin with `mov eax,1; ret` at `0x02001000`; the veneer table begins
at `0x02001006`. For a slot at address `A`, its target is:

```text
target = A + 5 + signed_le32(bytes[A+1 : A+5])
```

This is an unconditional tail jump. It retains the caller's arguments and
return address. There is no need to infer a C switch or guess a hidden hash map.
This address-to-function mapping survives the compiler change.

The extractor reproduces **all 11,814 baseline entries**, including their
addresses and symbol names, against `ManalinkEh.asm`. Applying the same procedure
to the target recovers **15,169 entries, all with uniquely resolved destinations**.

The source `manalink.lds` explicitly places `ManalinkEh.ao(.text)` first, keeping
the table at the expected addresses when function bodies are relinked. A generated
replacement can use the existing entry-point/label convention and an `extern`
plus `jmp near` for every recovered symbol. This does not require decompiling
the card bodies first.

The important compatibility issue is **slot reuse**:

- 11,770 existing slots keep the same target symbol name.
- 44 existing slots change target symbol name. Some are spelling corrections;
  some are different handlers, such as `_card_flourishing_defenses` becoming
  `_card_zap` at `0x0200f6ab`.
- 3,355 slots are appended.

Do not assume a slot has the same meaning in both releases. Use the target
executable's card records and target table together. The recovered assembly
is saved as `.recovery/2022-ManalinkEh.recovered.asm`; the complete mapping
is `.recovery/2022-veneers.csv`.

## A verified example

```text
Professor Onyx
  CSV/data ID:            22761
  target internal ID:    22698
  Cards.dat CODED flag:  true
  code_pointer/veneer:   0x02013328
  veneer index:          14906 (zero-based)
  implementation:        0x023d7d50
  symbol:                _card_professor_onyx
  DWARF source:          cards/strixhaven.c:5351
```

The function was already decompiled in the initial pilot. Its direct helper
calls include `_magecraft_trigger`, a good example of recovering a shared
mechanic before recovering the cards that use it.

## The first recovery backlog

The target contains 24,920 populated internal card records, including dummy and
effect records, versus 19,796 in the baseline. Its `Cards.dat` has 24,941 entries
versus 19,818; the file structure, record IDs, string offsets, bit-table length
and total size were validated against the existing `csv2dat.c` serialization.

Of the target records, 17,120 point through decoded veneers. There are 3,310
distinct registered handler symbols absent from the baseline function table.
Of these, 3,217 serve at least one record marked CODED in the shipped data.

Separately, 1,269 target records have an ID beyond the baseline data range, a
CODED flag, and a veneer. This smaller number measures records for newer data
IDs; the larger handler count also includes newly implemented old cards,
renames, tokens, transformed faces and effects. Registration and CODED flags
establish a recovery target, not correct or complete gameplay behavior.

3,306 of the 3,310 new registered handlers already have contiguous ranges in
the extracted DWARF census. Four require additional range/alias handling:
`card_cat_dragon_token`, `card_pirate_token`, `card_professor_of_symbology`, and
`card_treasure`. They remain in the backlog; absence from this initial range
export does not mean their implementations are unavailable.

A Capstone pass over those ranges found **243 non-card helper symbols absent
from the baseline** that are called directly by new registered handlers.
Some useful priorities, ranked by distinct direct callers:

| New helper symbol | New registered handlers calling it |
| --- | ---: |
| `dat_ability` | 125 |
| `sp_ability_trigger` | 79 |
| `aura` | 61 |
| `attached_gets_pt` | 33 |
| `inst_info` | 31 |
| `set_inst_info` | 28 |
| `saga` | 25 |
| `magecraft_trigger` | 23 |
| `mdfc_front_impl` | 21 |
| `learn` | 17 |
| `encore` | 15 |

These are static counts across all 3,310 roots, including handlers whose only
records are not marked CODED. A renamed helper may appear new. The pass records
direct calls/jumps and immediate operands matching function-entry addresses;
the latter are marked as callback candidates, not proven calls. It does not
yet resolve indirect calls, recover helpers fully inlined into callers, or
walk every transitive dependency. Old-name helpers may also have changed.

## Revised order of work

1. Preserve the recovered target veneer mapping and registration manifest.
   Separate newly added data IDs, newly implemented older cards, aliases,
   tokens/faces/effects, and unmarked/placeholder records.
2. Select a small mechanic group using the registration and helper graph.
   A useful first candidate is `magecraft_trigger` plus Clever Lumimancer and
   Archmage Emeritus. Their target function sizes are 55, 126 and 97 bytes,
   respectively, and all have named DWARF ranges.
3. Recover that group's types, constants, globals and helper dependencies.
   Recursively inspect callees until reaching verified compatible baseline
   implementations or understood executable interfaces. Include callbacks,
   legacy effects, tokens and dispatch events in the dependency graph.
4. Reconstruct the shared mechanic and a few representative cards, build, then
   compare behavior with the target. Check copied spells and trigger ordering
   for magecraft; a small card function does not guarantee a small engine change.
5. Expand the card group, then move to another mechanic. Rank helpers by the
   cards they serve and the amount of changed infrastructure required. Broad
   helpers such as `dat_ability` may have higher payoff but more dependencies.
6. Use Version Tracking and adjacent releases for ambiguous identities and
   changed baseline dependencies. A complete diff of every old function is
   not a prerequisite for recovering the first new card group.

## Reproduce and inspect

Run from this repository after the DWARF inspection:

```powershell
python tools/recovery/inspect_release.py --dwarf
python tools/recovery/trace_registration.py
```

The second command also requires Capstone (tested with 5.0.9). It verifies the
baseline table, reconstructs target assembly, joins executable records to
`Cards.dat` and named veneers, and emits direct dependency/prioritization CSVs.
It reads the game files without running them and does not modify game source.

Large outputs remain under ignored `.recovery/`:

- `2022-ManalinkEh.recovered.asm`: generated target jump-table assembly.
- `2022-veneers.csv`: every slot, address, implementation and symbol.
- `2022-registrations.csv`: internal/data IDs, full names, CODED flags, handlers
  and baseline comparison fields.
- `new-handler-dependencies.csv`: instruction-address evidence for static edges.
- `new-helper-priorities.csv`: helpers ranked by distinct direct new-handler callers.

Compact evidence under `docs/recovery-evidence/` includes
`registration-summary.json`, `retargeted-veneers.csv`, and
`new-helper-priorities.csv`. Recovered implementations are now being placed in
the original source files; see [recovery progress](recovery-progress.md).
