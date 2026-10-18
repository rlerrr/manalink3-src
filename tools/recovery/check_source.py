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
''')
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
