# Helper recovery triage

Current review frame: **2,195 native helper ranges**, plus **49 explicitly tagged bodies already reconstructed during consolidation**. The classifier does not edit C source or recovery records. Narrow argument/interface edits do not close whole-body review.

| Review queue | Ranges |
| --- | ---: |
| `same_local_code` | 1,071 |
| `callee_binding_only` | 0 |
| `reference_review` | 145 |
| `small_diff_review` | 223 |
| `full_review` | 535 |
| `mapping_first` | 191 |
| `deferred_chooser` | 18 |
| `cpp_review` | 12 |
| `reconstructed_source` | 49 |

**735 accepted canonical function names plus 3 auxiliary helper identities** are checked against physical source and compiler output. Initial consolidation removed 425 compatibility aliases/adapters. Historical recovery names remain metadata identities.

5 private canonical helpers have verified concrete DWARF inline instances instead of callable COFF entries. Their caller-owned ranges do not receive standalone helper scores or fabricated callee addresses.

**0 duplicate implementation candidates** were found among retained/recovered pairs. These require ABI and caller review before canonicalizing names.

Of the 1071 local matches, 202 have no pending callees in the recorded dependency list; 869 still depend on functions without an accepted recovery or explicitly tagged full reconstruction. Keep this distinction when tracking completion.

Reccmp scores are used to order review. A 100% score alone is insufficient because unknown addresses can be replaced with matching placeholders. The strict comparison keeps numeric constants and structure/stack offsets, checks complete DWARF ranges, and only translates known function identities and exact source-string bytes. Unresolved data and indirect-call dependencies remain review work. Local equality is not transitive semantic or gameplay proof.

The report compares the current recovery build to October 2022. Recovered bodies, canonical caller contracts and modern headers can create differences from retained source; it is not an isolated 2020 build. A small assembly delta can still have a large gameplay effect; scores never authorize ignoring changed constants or flags.

## Next workflow

1. Keep `same_local_code` bodies; review their recorded dependency closure rather than decompiling them again.
2. Review `callee_binding_only` and `small_diff_review` against the existing C source and captured instruction deltas. Make targeted edits where the evidence supports them.
3. Resolve unknown references and ambiguous mappings before interpreting their scores.
4. Reserve full Ghidra decompilation for substantial changes, missing source, and complex workers.
5. Retain native bodies under their intended canonical names, unify callers/prototypes/callbacks/registration bindings with the native contracts, and record predecessor hashes. Compiler sharing is a candidate signal, not permission to delete an arbitrary body. Shared native APIs can resolve many retained-caller bindings together.

## Shared binding priorities

| Recovered API | Distinct pending callers | Call/reference sites |
| --- | ---: | ---: |

## Refresh

Run `python tools/recovery/triage_helpers.py --refresh` with a current build/setup to refresh JSON, its input receipt and the DWARF bounds cache, then classify the helpers. Without `--refresh`, it reuses those inputs and rejects stale hashes. Source files newer than the DLL are rejected. Detailed categories, instruction deltas, dependency addresses, dedupe proofs and input/source hashes are in [helper-triage.json](recovery-evidence/helper-triage.json).
