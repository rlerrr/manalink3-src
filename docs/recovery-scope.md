# Recovery scope

The goal remains best-effort recovery of October 2022 card handlers and their
helpers into their original source units. Native linking and downstream gameplay
verification are separate work.

## Built-in Wish/card-picker dialogs: temporarily out of scope

Per the user's 2026-10-22 instruction, decompilation of the built-in chooser UI
and its dialog-specific internals in `functions/choose_a_card.cpp` is deferred.
The two entry points are explicit C-linkage stubs in that source file:

| Entry point | Native address | Current stub result |
| --- | --- | --- |
| `choose_a_card_test_ct` | `0x26ef820` | `-1`, no selection |
| `choose_a_card_test_ct_allow_cancel` | `0x26ef860` | `-1`, no selection |

Both stubs ignore their arguments and return no selection, including the
mandatory-selection entry point. Interactive Wish/card-list selection therefore
remains unavailable. These bodies are placeholders, not native decompilations,
and are excluded from the recovered-function totals. Their source and boundary
metadata are recorded separately under `wish_dialog_stubs_2022` in
`recovery-evidence/interface-recovery.json`, and in the manifest's `source_stubs`
and `out_of_scope_source_units` fields.

The surrounding filter-aware card-list/Wish logic, AI selection and fallback,
and card handlers remain in scope. Their source preserves the native flow and
records the stubbed dialog dependency. UI integration or replacement can later
fill these boundaries without recovering the original dialog internals first.
