# Random sample of shared 2020/2022 functions

A second comparison uses a frozen random sample rather than the first pilot's
deliberately chosen controls and likely candidates. It finds **12 local code/API
changes in 30 sampled function identities**. Many other differences still look
like compiler effects. This is an exploratory estimate, not a gameplay-equivalence
test or an estimate of the number of functions left to recover.

## Selection and scope

- Draw 15 names beginning with `card_` and 15 other C function identities
  ("helpers", including private functions defined in card source units).
- Eligible frame: **11,729 card identities and 2,653 helper identities** paired
  unambiguously between the release inventories.
- Exclude the prior pilot's 30 names, 18 shared target entries already in the
  recovered-function manifest, and six non-C-source pairs. This excludes
  catalogued known work, not every change someone might already know about.
- Keep the prior identity pairing's ambiguous/split/unpaired cases outside this
  frame. Results do not describe new functions or those unresolved identities.
- Seed: `manalink-shared-functions-2026-10-18-v1`. Rank eligible identities by
  SHA-256 of the seed, stratum and identity fields, then take the lowest 15 ranks
  per stratum. The frame, ranks and selection were saved before decoding the
  sampled bytes. No size/similarity filter or replacement of difficult functions
  was used. This is a reproducible pseudorandom selection.

The tracked [evidence](recovery-evidence/random-modification-sample.json) includes
the complete selected names, seed, frame counts, exclusion counts and hashes of
the full local sampling frame and frozen selection.

## Results

| Group | Sample | Confirmed code/API changes | Compiler/layout candidates | Same symbolic instructions |
| --- | ---: | ---: | ---: | ---: |
| Card handlers | 15 | 5 | 8 | 2 |
| Helpers | 15 | 7 | 6 | 2 |
| Total sampled | 30 | 12 | 14 | 4 |

The observed change rate is **33% for cards** and **47% for helpers**. With only
15 observations per group, the respective 95% Wilson intervals are approximately
**15–58%** and **25–70%**. These intervals describe sampling uncertainty under the
review labels; they do not account for classification errors or indirect changes
through callees and data.

Helpers are deliberately oversampled: they make up about 18% of this eligible
frame, not 50%. Weighting each group's observed rate by its population gives an
exploratory **36% local code/API-change rate** for this frame. The unweighted
12/30 = 40% is the sample composition, not the population estimate. The sample is
too small to establish a difference from the earlier purposive pilot or support
a precise project-wide recovery workload estimate.

"Confirmed change" includes four interface migrations, two display-related
changes, an aura event filter and a token-color change, as well as four broader
logic changes. It does not mean twelve new card effects or twelve bug fixes.

## Changed functions

| Function | Observed change |
| --- | --- |
| `card_azorius_ploy` | Two target definitions/slots and separate prevention of damage dealt by one target and dealt to the other replace a single Maze-of-Ith-style effect |
| `card_reap_intellect` | Explicit X-payment/special-flag handling and changed target/`lobotomy_effect` interface |
| `card_anvil_of_bagardan` | Separate `AL_TRIG` and `AL_STATIC` suppression checks for drawing/discarding and hand size |
| `card_cage_of_hands` | An explicit aura-event/casting filter gates the `disabling_aura` fallback; the activated bounce path remains |
| `card_brindle_shoat` | Literal green token color replaces a sleighted-color lookup |
| `when_attacks_pump_self` | New alternate pump-helper interface and `AL_TRIG` checks |
| `copy_spell_from_stack` | New copy-worker interface, including `forced_target = NULL`; the apparent changed flags literal is a compiler-specialization trap, described below |
| `show_deck` | Resets `hack_show_deck_force_expansion` before the native deck display |
| `is_legal_block_impl` | Calls a new base-legality worker and adds other-blocker scans/counting for multi-blocker requirements |
| `genju` | Death-trigger suppression check and Spirit subtype setup through the animation legacy |
| `gain_control_until_source_is_in_play_and_tapped` | Selects alternate legacy display text after successful creation |
| `dash_cost_cless` | The cost reducer's ability-loss check now passes `AL_STATIC` |

These twelve entries are comparison findings, not newly integrated recovered
implementations. Full recovery still needs the relevant interfaces, storage and
callee behavior checked against the retained source.

## Remaining sampled functions

The exact local instruction controls are Harbor Bandit, Verdeloth the Ancient,
`lose_2_life_thunk` and `boost_creature_type`.

The compiler/layout candidates are Arena Athlete, Rampant Growth, Phytotitan,
Runehorn Hellkite, Serene Offering, Chatter of the Squirrel, Caverns of Despair,
Thrull Wizard, `player_sacrifices_a_creature`,
`glyph_of_destruction_boost_legacy`, `manifest_iid_impl`,
`in_play_and_attacking`, `wild_growth_aura_all_one_color` and
`get_hacked_land_text`. The reviews retain constants and call arguments;
corresponding local behavior is not a proof that their callees or referenced data
are unchanged.

## Compiler and analysis traps checked

The old compiled `real_copy_spell` receives `special_flags3` in ECX because GCC
has specialized `special_flags` to constant `0xc0000`. The old concrete DWARF DIE
explicitly records that constant. In the new worker, ECX supplies the restored
`special_flags` argument, and `special_flags3` moves to the stack. Comparing the
call-site register literals alone would incorrectly report a new flags behavior.
The new optional target parameter is an actual interface change. Both raw C
and assembly are retained, and the optimized-parameter evidence qualifies the
misleading old decompiler argument list.

`get_hacked_land_text` shrinks from 700 to 556 bytes and its decompilation changes
substantially as GCC lowers switches and loops differently. Review found matching
mapping/article/case logic; an independent DWARF-guided read verifies all 24
pointed-to strings in its land-word table match between releases.

The dedicated project lacks a complete native executable call model. In
`show_deck`, raw C ends in `halt_baddata` at the unmapped native tail jump. The
new global zero store is independently visible in assembly. Reap Intellect's
variable-length stack allocation also confuses raw decompilation; its explicit
new casting/payment path establishes the finding without claiming complete
resolution-body recovery.

## Reproduction and validation

The existing `.recovery/ghidra/ComparisonPilot.gpr` now contains session
**2020-to-2022-random-v1** alongside the original pilot sessions. All 30 expected
symbol pairs were verified. Exact Bytes produced zero matches and Exact
Instructions four; these scores are not semantic similarity percentages. No
Version Tracking markup was transferred between releases.

Local `.recovery/comparison-random/` contains the full sampling frame, frozen
selection, paired assembly/C, metadata and Version Tracking export. It is kept
separate from the first pilot's exports so that comparison evidence is preserved.
The raw artifact hashes and per-function notes are in the tracked evidence JSON.

```powershell
python tools/recovery/random_comparison_sample.py
```

This refuses to change an existing frozen selection. Process the already
symbolized 2022 program in the comparison project with `-noanalysis` and:

```text
-postScript RunRecoveryComparison.java <absolute-path-to-.recovery/comparison-random> <new-session-name>
```

Then inspect the selected pairs and run:

```powershell
python tools/recovery/inspect_random_comparison_context.py
python tools/recovery/record_random_comparison.py
```

The recorder is specific to the reviewed release hashes and this fixed sample.
For an additional session, pass its actual name to the recorder with `--session`.
All **60 function bodies decompiled**, all **60 byte extents decoded fully**,
and all **30 expected Version Tracking identities matched**. Original game
binaries and retained C implementations were not edited by this analysis.
Gameplay testing and downstream integration remain separate work.
