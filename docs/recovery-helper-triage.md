# Helper recovery triage

Current pending helper frame: **2,244 native ranges**. The classifier does not edit C source or recovery records.

| Review queue | Ranges |
| --- | ---: |
| `same_local_code` | 979 |
| `callee_binding_only` | 5 |
| `reference_review` | 128 |
| `small_diff_review` | 274 |
| `full_review` | 625 |
| `mapping_first` | 203 |
| `deferred_chooser` | 18 |
| `cpp_review` | 12 |

**587 recovered helpers now use their original C names**, with 425 compatibility aliases/adapters removed. Historical recovery names remain metadata identities. Current source and physical symbols are checked independently.

**0 duplicate implementation candidates** were found among retained/recovered pairs. These require ABI and caller review before canonicalizing names.

Of the 979 local matches, 180 have no pending callees in the recorded dependency list; 799 still depend on functions without recovery records. Keep this distinction when tracking completion.

Reccmp scores are used to order review. A 100% score alone is insufficient because unknown addresses can be replaced with matching placeholders. The strict comparison keeps numeric constants and structure/stack offsets, checks complete DWARF ranges, and only translates known function identities and exact source-string bytes. Unresolved data and indirect-call dependencies remain review work. Local equality is not transitive semantic or gameplay proof.

The report compares the current recovery build to October 2022. Existing `_2022` bindings and modern headers can create differences from retained source; it is not an isolated 2020 build. A small assembly delta can still have a large gameplay effect; scores never authorize ignoring changed constants or flags.

## Next workflow

1. Keep `same_local_code` bodies; review their recorded dependency closure rather than decompiling them again.
2. Review `callee_binding_only` and `small_diff_review` against the existing C source and captured instruction deltas. Make targeted edits where the evidence supports them.
3. Resolve unknown references and ambiguous mappings before interpreting their scores.
4. Reserve full Ghidra decompilation for substantial changes, missing source, and complex workers.
5. Canonicalize one recovered family at a time: retain the 2022 body under its intended canonical name, update callers/prototypes/callbacks/registration bindings, keep explicit ABI adapters where necessary, and migrate the provenance ledger with predecessor hashes. Compiler sharing is a candidate signal, not permission to delete an arbitrary body. Compatible canonical core APIs can resolve many retained-caller bindings together; adding another suffixed copy for every such caller would preserve the duplication problem.

## Shared binding priorities

| Recovered API | Distinct pending callers | Call/reference sites |
| --- | ---: | ---: |
| `new_sacrifice_2022` | 2 | 3 |
| `action_on_card_2022` | 1 | 1 |
| `new_manipulate_all_2022` | 1 | 1 |
| `manipulate_all_2022` | 1 | 1 |

## Refresh

Run `python tools/recovery/triage_helpers.py --refresh` with a current build/setup to refresh JSON, its input receipt and the DWARF bounds cache, then classify the helpers. Without `--refresh`, it reuses those inputs and rejects stale hashes. Source files newer than the DLL are rejected. Detailed categories, instruction deltas, dependency addresses, dedupe proofs and input/source hashes are in [helper-triage.json](recovery-evidence/helper-triage.json).
