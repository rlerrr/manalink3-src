# 2020 to October 2022 modification pilot

The release binaries can identify changes in functions already present in the
2020 source. A complete GCC rebuild is not a prerequisite. This pilot compares
30 shared function identities: 14 card handlers and 16 helpers. Selection is
deliberate, including simple controls and shared engine functions; these results
must not be extrapolated into a project-wide percentage of modified functions.

A subsequent [random sample](recovery-random-sample.md) uses a frozen selection
to investigate change rates without this pilot's purposive choice of functions.

## Results

| Reviewed category | Functions | Meaning |
| --- | ---: | --- |
| Same symbolic instructions | 5 | Same local instructions, operands and widths after resolving moved symbols and internal branch targets |
| Compiler/layout candidate | 11 | Reviewed paths correspond; scheduling, registers, padding, block ordering or wrapper prologues differ |
| Confirmed code/interface change | 13 | Concrete changed calls, arguments, conditions or storage accesses |
| Refactor needing follow-up | 1 | Old body moved behind a new worker; equal-name comparison is insufficient |

All **30 expected function pairs** were independently produced by Ghidra's
Exact Symbol Name correlator. Exact Bytes produced zero matches; Exact
Instructions produced two (Giant Growth and `create_legacy_effect`). Those
instruction scores mask operands and are not semantic accuracy percentages.
No Version Tracking markup was applied from 2020 to 2022.

Across the project inventories, identity pairing found **14,436 pairs**:
14,214 by source unit plus name, and 222 by a unique name moved between source
units. There are 75 shared source/name keys with multiple compiled definitions.
They were left ambiguous rather than arbitrarily selecting a clone. Unpaired
records (253 old, 4,413 new) include unresolved identity cases and must not be
reported as exact removed/added-function totals.

## Reviewed sample

| Function(s) | Assessment | Evidence |
| --- | --- | --- |
| Black Lotus, Giant Growth, Counterspell, `get_power`, `get_toughness` | Same instructions | Moved calls resolve to the same qualified identities; argument constants are retained |
| Ancestral Recall, Lightning Bolt | Compiler/layout candidate | Reviewed targeting, resolution and AI paths correspond; target-definition layout also differs |
| Sol Ring, Dark Ritual | Compiler/layout candidate | Corresponding local decompilations; block ordering differs |
| Doubling Season, `draw_cards` | Compiler/layout candidate | Same tail call; the old build emits an otherwise redundant frame prologue/epilogue |
| `global_enchantment` | Compiler/layout candidate | Padding NOP changes branch instruction indices |
| `generic_activated_ability` | Compiler/layout candidate | Same forwarded arguments; setup instructions are scheduled differently |
| `create_legacy_effect` | Compiler/layout candidate | EBX/ESI allocation differs; same constructor and info-slot assignment |
| `count_graveyard`, `check_for_ability` | Compiler/layout candidate | Corresponding loop/query behavior with different register and block layouts |
| Winter Orb | Confirmed; known positive control | New `is_tapped(player, card)` guard before untap-limiting helper |
| Howling Mine, Sylvan Library | Confirmed interface change | `is_humiliated` gains third argument `AL_TRIG` (2) |
| Static Orb | Confirmed interface change | `is_humiliated` gains `AL_STATIC` (4); its same-card duplicate scan already existed |
| Mystic Remora | Confirmed | Uses `sp_ability_trigger`, sets `test.type_flag = 2`, resolves through `EVENT_SPA` |
| Rhystic Study | Confirmed | Trigger registration and `EVENT_SPA` resolution replace the old helper's return-value gating |
| `untap_only_one_permanent_type` | Confirmed | Scans other in-play copies with the same card ID and sets their `info_slot` to 2; also passes `AL_TRIG` |
| `draw_a_card` | Confirmed | Uses DLL `add_card_to_hand` in place of native insertion and the old preincrement/decrement sequence |
| `lose_life` | Confirmed | Adds `life_total_can_change`, Lich's Mastery and another draw reaction; uses newer ability categories |
| `damage_creature` | Confirmed | New attack/planeswalker metadata, target type and source-being-cast tagging; extension-backed keyword copying |
| `is_what` | Confirmed | New out-of-play creature exception for CSV ID `0x5a13`; special flags now read through the extension |
| `add_counters` | Confirmed API change; adapter already recovered | Four parameters become six, separating source from target; target entry already reconstructed as `add_counters_from_source` |
| `can_regenerate` | Confirmed | Replaces generic activated-ability eligibility with a direct flag check using mask `0x28` |
| `gain_life` | Refactor; follow-up required | Old 2,544-byte body becomes a 17-byte wrapper around `gain_life_impl(player, amount, 0)` |

These are changes between release binaries, not 13 newly completed recoveries.
Winter Orb was already known from the initial inspection. Counter source-aware
recovery already supplies an adapter. Existing source bodies and their baseline
hashes are recorded so changes still needing integration can be distinguished
from work already completed. The pilot itself changes no game source.

## Implications for recovery

1. Finish the ability-loss representation and its writers before converting
   existing callers. `is_humiliated` is now category-aware, and regeneration's
   `0x28` mask belongs to that newer representation. Copying that condition into
   the old nibble-counter scheme independently would be incorrect.
2. Complete extension-backed special-flag and keyword access, then recover
   `is_what` and damage changes. This directly supports newer cards including
   the already recovered Grist handler. Confirm the `0x5a13` name against the
   target card database when replacing the literal.
3. Migrate existing spell-trigger callers to the recovered registration/resolution
   API, starting with Remora and Rhystic Study.
4. Compare old `gain_life` with new `gain_life_impl`, then recover changed life
   loss/draw behavior and reconcile source-aware counter callers.
5. Expand the paired-function screening. Promote candidates after reviewing
   constants, call arguments and storage accesses; do not use call-set equality
   or a Version Tracking identity score as evidence of unchanged behavior.

A GCC 9.3 rebuild of clean 2020 source remains useful for difficult compiler-noise
cases. This sample found actionable changes without that prerequisite.

## Artifacts and reproducibility

Tracked evidence: [modified-functions-pilot.json](recovery-evidence/modified-functions-pilot.json).
It contains raw DLL/inventory hashes, per-function addresses/RVAs/sizes, code
hashes, direct call changes, reviewed categories, raw artifact hashes and
baseline/current source-body hashes.

Local Ghidra project: `.recovery/ghidra/ComparisonPilot.gpr`.
Open session **2020-to-2022-pilot-abi**, with 2020 as source and 2022 as
destination. The earlier exploratory session is retained; use the `-abi` session
for the final 30-entry comparison.

Raw paired assembly, C, diffs, identity census and Version Tracking exports are
in `.recovery/comparison-pilot/`. Original DLLs and DBG files are never modified.
The analysis-only PE files retain every runtime/debug section payload and COFF
function record; their derivation manifests are in `.recovery/`.

Preparation:

```powershell
python tools/recovery/prepare_analysis.py --year 2020
python tools/recovery/compare_releases.py
python tools/recovery/export_abi.py --year 2020 --metadata-only --output .recovery/comparison-pilot/2020-metadata
python tools/recovery/export_abi.py --year 2022 --metadata-only --output .recovery/comparison-pilot/2022-metadata
```

Import both analysis PEs into a fresh Ghidra project using `-noanalysis`,
`-processor x86:LE:32:default`, `-cspec gcc`, and post-script
`ImportRecoveryDwarf.java`. The existing 2022 analysis PE comes from
`prepare_analysis.py` with its default year. Use a 6 GB Java heap and the
repository's `tools/recovery` script path. Process the 2022 program with:

```text
-noanalysis -postScript RunRecoveryComparison.java <absolute-path-to-.recovery/comparison-pilot> <new-session-name>
```

That script imports the independently extracted entry register conventions
(423 baseline, 516 target), disables misleading automatic inlining, disassembles
the requested extents, models direct sibling calls, decompiles all 60 bodies and
runs the three correlators restricted to the 30 function entry points. It refuses
to overwrite an existing session. It applies no match markup. To record the final
evidence after review, run `python tools/recovery/record_comparison_pilot.py`.

## Limits

Same local instructions do not imply equal behavior of called functions, data
or the native executable. A compiler/layout candidate remains a reviewed
candidate, not a proof of unchanged source or behavior. No exact-source matching
is attempted.

Ghidra's DWARF import reports unsupported optimized local-location expressions.
Native executable functions/globals are not fully modeled in this dedicated
project. Register conventions materially affect the helper output: before those
corrections, the decompiler invented a third stack argument for `gain_life_impl`
and misidentified counter-helper arguments. The recorded exports are from the
corrected run, but remain analysis output requiring review before source edits.

Validation covers exact expected Version Tracking pairs, 60 successful
decompilations, complete Capstone decoding for all 60 extents and unchanged
runtime/debug payloads. No gameplay tests, DLL linking or native deployment
were performed.
