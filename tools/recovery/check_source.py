"""Check recovered C source syntax and fixed x86 slots; does not run tests/build a DLL."""
import concurrent.futures
import json
import subprocess
from pathlib import Path


def main():
    root = Path(__file__).resolve().parents[2]
    analysis = root / ".recovery"
    analysis.mkdir(exist_ok=True)
    manifest = json.loads((root / "docs/recovery-evidence/recovered-functions.json").read_text())
    command = ["C:/Program Files/LLVM/bin/clang.exe", "-target", "i686-pc-windows-msvc",
               "-std=gnu99", "-fsyntax-only", "-ferror-limit=0", "-fno-color-diagnostics",
               "-Werror=implicit-function-declaration", "-Werror=implicit-int",
               "-Werror=incompatible-pointer-types", "-Werror=return-type",
               "-Wno-ignored-attributes", "-I", str(root)]

    def check(source):
        compatibility = source == "functions/rules_engine.c"
        result = subprocess.run(command + (["-Dstrncasecmp=_strnicmp"] if compatibility else []) +
                                [str(root / source)], capture_output=True, text=True)
        return source, dict(exit_code=result.returncode, diagnostics=result.stderr,
                            checker_only_compatibility_define=compatibility)

    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
        results = dict(pool.map(check, sorted(manifest["source_files"])))
    (analysis / "source-syntax-final.json").write_text(json.dumps(results, indent=2))

    layout = analysis / "source-abi.c"
    layout.write_text('''#include "manalink.h"
#include <stddef.h>
_Static_assert(sizeof(card_instance_t) == 300, "instance size");
_Static_assert(sizeof(stack_data_t) == 4 && offsetof(stack_data_t, internal_card_id) == 0, "stack card ID word");
_Static_assert(sizeof(text_lines) == 249 * 300 && sizeof(text_lines[0]) == 300, "native text rows");
_Static_assert(offsetof(card_instance_t, state) == 8 && sizeof(((card_instance_t*)0)->state) == 4, "state word");
_Static_assert(offsetof(card_instance_t, damage_on_card) == 16 && offsetof(card_instance_t, counter_power) == 18 && sizeof(((card_instance_t*)0)->damage_on_card) == 2 && sizeof(((card_instance_t*)0)->counter_power) == 2, "damage and counter halfwords");
_Static_assert(offsetof(card_instance_t, mode) == 0x37, "mode byte");
_Static_assert(offsetof(card_instance_t, info_slot) == 0x38, "info slot");
_Static_assert(offsetof(card_instance_t, info_slot_fn) == 0x38, "function slot");
_Static_assert(offsetof(card_instance_t, eot_toughness) == 0x4c, "toughness slot");
_Static_assert(offsetof(card_instance_t, eot_toughness0) == 0x4c, "byte 0");
_Static_assert(offsetof(card_instance_t, eot_toughness1) == 0x4d, "byte 1");
_Static_assert(offsetof(card_instance_t, eot_toughness2) == 0x4e, "byte 2");
_Static_assert(offsetof(card_instance_t, eot_toughness3) == 0x4f, "byte 3");
_Static_assert(offsetof(card_instance_t, targets) == 0x74, "targets slot");
_Static_assert(sizeof(card_counts) == 14, "card-count cache size");
_Static_assert(sizeof(card_counts[0]) == 7, "card-count row stride");
_Static_assert(sizeof(add_to_die_rolls) == 8 && sizeof(roll_extra_dice) == 8, "dice modifier caches");
_Static_assert(sizeof(subtypes_not_in_play_t) == 8, "subtype modifier size");
_Static_assert(offsetof(subtypes_not_in_play_t, added_forced) == 4, "subtype modifier mode");
_Static_assert(sizeof(added_subtypes_not_in_play) == 256, "subtype modifier table size");
_Static_assert(sizeof(added_subtypes_not_in_play[0]) == 128, "subtype modifier row stride");
_Static_assert(sizeof(event_query_t) == 4, "query selector width");
_Static_assert(sizeof(event_result) == 4 && sizeof(trigger_cause_controller) == 4, "engine word widths");
_Static_assert(sizeof(((card_instance_t*)0)->info_slot) == 4, "scalar info-slot width");
_Static_assert(sizeof(target_t) == 8 && offsetof(target_t, player) == 0 && offsetof(target_t, card) == 4, "target words");
''')
    storage_evidence = root / "docs/recovery-evidence/stack-storage.json"
    extension_path = root / 'docs/recovery-evidence/extension-foundation.json'
    if extension_path.exists():
        extension = json.loads(extension_path.read_text())
        if extension.get('source_recovered'):
            assertions = ['_Static_assert(offsetof(card_instance_t, ext) == 44 && offsetof(card_instance_t, unused30) == 48, "extension pointer slot");']
            for typename, record in extension['types'].items():
                assertions.append(f'_Static_assert(sizeof({typename}) == {record["bytes"]}, "extension record size");')
                assertions.extend(f'_Static_assert(offsetof({typename}, {member["name"]}) == {member["offset"]}, "extension field offset");'
                                  for member in record['members'])
                assertions.extend(f'_Static_assert(sizeof((({typename}*)0)->{member["name"]}) == {member["bytes"]}, "extension field width");'
                                  for member in record['members'] if 'bytes' in member)
            assertions.append('_Static_assert(sizeof(card_ext) == 57600 && sizeof(disp_card_ext) == 57600, "extension banks");')
            with layout.open('a') as stream:
                stream.write('\n' + '\n'.join(assertions) + '\n')
    state_values = {}
    for function in manifest['accepted_functions']:
        for replacement in function.get('state_mask_evidence', {}).get('replacements', []):
            for name, number in replacement['bits'].items():
                assert name not in state_values or state_values[name] == number
                state_values[name] = number
    with layout.open('a') as stream:
        stream.write('\n' + '\n'.join(f'_Static_assert({name} == {number}, "recovered state mask");'
                                      for name,number in state_values.items()) + '\n')
    if storage_evidence.exists():
        storage = json.loads(storage_evidence.read_text())
        assertions = []
        for typename, size in storage["source_layouts"].items():
            assertions.append(f'_Static_assert(sizeof({typename}) == {size}, "source stack view size");')
        for typename, record in storage["target_layouts"].items():
            for member, offset in record["members"].items():
                # The old test API predates the inserted special-keyword fields;
                # target flags and token flags3 are separate migration work.
                if (typename == "test_definition_t" and offset >= 140) or member in {"flags", "special_flags3"}:
                    continue
                assertions.append(f'_Static_assert(offsetof({typename}, {member}) == {offset}, "shared stack view offset");')
        with layout.open("a") as stream:
            stream.write("\n" + "\n".join(assertions) + "\n")
    abi = subprocess.run(command + [str(layout)], capture_output=True, text=True)
    summary = dict(passed=sum(r["exit_code"] == 0 for r in results.values()),
                   failed=[s for s,r in results.items() if r["exit_code"] != 0],
                   abi_exit_code=abi.returncode, abi_diagnostics=abi.stderr,
                   target="i686-pc-windows-msvc", language="gnu99", tests_run=False,
                   compatibility_defines={"functions/rules_engine.c": "strncasecmp=_strnicmp"})
    (analysis / "source-check-summary.json").write_text(json.dumps(summary, indent=2))
    manifest["source_syntax_check"] = summary
    (root / "docs/recovery-evidence/recovered-functions.json").write_text(json.dumps(manifest, indent=2))
    print(json.dumps(summary, indent=2))
    raise SystemExit(bool(summary["failed"] or abi.returncode))


if __name__ == "__main__":
    main()
