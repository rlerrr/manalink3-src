"""Promote reconstructable functions into their DWARF source units.

This deliberately rejects unresolved machine state, absolute-address expressions,
unsupported types, missing dependencies and signatures incompatible with the
current headers. Clang is used only for C syntax/interface checking, not tests.
Raw exports stay in .recovery. Source mutation requires --apply.
"""
import argparse
import collections
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path


UNRESOLVED = re.compile(r"\b(?:undefined\w*|in_\w+|extraout_\w+|unaff_\w+|_?DAT_\w+|UNK_\w+|PTR_\w+|func_\w+|field_\w+|CONCAT\w*|SUB\d+|ZEXT\w*|SEXT\w*)\b")
LITERAL = r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\''


def strip_comments(text):
    return re.sub(LITERAL + r"|/\*.*?\*/", lambda m: "" if m[0].startswith("/*") else m[0], text, flags=re.S)


def clean(text, overlapping_globals=()):
    text = strip_comments(text).strip()
    literals = []
    def save_literal(match):
        literals.append(match[0])
        return f"RECOVERY_LITERAL_{len(literals)-1}"
    text = re.sub(LITERAL, save_literal, text)
    text = re.sub(r"\b__(?:cdecl|regparm[123])\b", "", text)
    for old, new in (("uint", "unsigned int"), ("ushort", "unsigned short"),
                     ("byte", "unsigned char"), ("ulonglong", "unsigned long long"),
                     ("longlong", "long long")):
        text = re.sub(r"\b" + old + r"\b", new, text)
    text = re.sub(r"\([\w\s]+\*\)0x0\b", "NULL", text)
    text = re.sub(r"\n[ \t]+\n", "\n", text)
    text = re.sub(r"\n{3,}", "\n\n", text)
    pointers = re.findall(r"card_instance_t\s*\*\s*(\w+)\s*;", text)
    if len(pointers) == 1:
        text = re.sub(r"\b" + re.escape(pointers[0]) + r"\b", "instance", text)
    temporaries = set(re.findall(r"\biVar\d+\b", text))
    if len(temporaries) == 1:
        text = re.sub(r"\b" + temporaries.pop() + r"\b", "result", text)
    for name in overlapping_globals:
        text = re.sub(r"\b_" + re.escape(name) + r"\b", name, text)
    text = re.sub(r"\bRECOVERY_LITERAL_(\d+)\b", lambda m: literals[int(m[1])], text)
    text = re.sub(r"\)\s*\n\s*\n\{", ")\n{", text)
    return text + "\n"


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument("--apply", action="store_true")
    parser.add_argument("--max-characters", type=int, default=3000)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    analysis = root / ".recovery"
    exports = analysis / "decompiled-abi"
    clang = Path("C:/Program Files/LLVM/bin/clang.exe")
    source_text = {p.relative_to(root).as_posix(): p.read_bytes().decode("utf-8", errors="replace")
                   for folder in ("cards", "functions") for p in (root / folder).glob("*.c")}
    all_sources = "\n".join(source_text.values())
    definitions = set(re.findall(r"\b([A-Za-z_]\w*)\s*\([^;{}]*\)\s*\{", strip_comments(all_sources)))
    file_definitions = {path:set(re.findall(r"\b([A-Za-z_]\w*)\s*\([^;{}]*\)\s*\{", strip_comments(text)))
                        for path,text in source_text.items()}
    functions = {}
    for row in csv.DictReader((analysis / "2022-functions.csv").open()):
        if row["low_pc"] and row["high_pc"]:
            functions[int(row["low_pc"], 16)] = row
    from inspect_release import Image
    image = Image((analysis / "2022-ManalinkEh.dll").read_bytes())
    address_symbols = collections.defaultdict(set)
    for symbol, offset, section, kind, storage in image.functions():
        address_symbols[image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[section-1].VirtualAddress + offset].add(symbol)
    static_addresses = {image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[s-1].VirtualAddress + v
                        for n, v, s, t, storage in image.functions() if storage == 3}
    static_names = {n[1:] for n,v,s,t,storage in image.functions() if storage == 3 and "." not in n}
    overlapping_globals = [f[1] for line in (analysis / "absolute-interfaces.tsv").read_text().splitlines()
                           if (f := line.split("\t"))[2] == "global"]
    candidates, rejected = {}, {}
    for row in csv.DictReader((exports / "export.tsv").open(), delimiter="\t"):
        name = row["name"]
        if row["status"] != "ok" or not row["source"].endswith(".c"):
            rejected[name] = "unsupported export or C++ source"
            continue
        address = int(row["address"], 16)
        static = address in static_addresses
        if name in (file_definitions.get(row["source"], set()) if static else definitions):
            rejected[name] = "implementation already present in source"
            continue
        if "_" + name not in address_symbols[int(row["address"], 16)]:
            rejected[name] = "compiler clone or noncanonical entry"
            continue
        raw = (exports / (row["address"] + ".c")).read_text(encoding="utf-8")
        if re.search(r"WARNING:.*(?:bad instruction|Could not recover|uninitialized)", raw, re.I):
            rejected[name] = "decompiler control-flow warning"
            continue
        body = clean(raw, overlapping_globals)
        if UNRESOLVED.search(body) or re.search(r"::|\bthis\b|\._\d|\.field_", body):
            rejected[name] = "unresolved machine state or layout"
            continue
        # Literal executable addresses are still unresolved interfaces.
        literals = re.findall(r"\b0x[0-9a-fA-F]+\b", body)
        if any(0x400000 <= int(x, 16) < 0xa00000 or 0x2000000 <= int(x, 16) < 0x2800000 for x in literals):
            rejected[name] = "absolute address or ambiguous large constant"
            continue
        if len(body) > args.max_characters:
            rejected[name] = "larger function awaiting focused reconstruction"
            continue
        if static:
            body = "static " + body
        signature = body[:body.index("{")].strip()
        metadata = functions.get(address, {})
        candidates[name] = dict(name=name, address=row["address"], source=row["source"],
                                line=metadata.get("line", ""), static=static,
                                body=body, prototype=signature + ";", raw_sha256=hashlib.sha256(raw.encode()).hexdigest())

    initial = set(candidates)
    # References to any exported function require its implementation to be
    # present already or accepted in this same batch, including callbacks.
    exported_names = {r["name"] for r in csv.DictReader((exports / "export.tsv").open(), delimiter="\t")}
    known = definitions - static_names
    dependencies = {name: (set(re.findall(r"\b[A-Za-z_]\w*\b", c["body"])) & exported_names) - {name}
                    for name, c in candidates.items()}
    scratch = analysis / "source-candidates.c"
    compiler_diagnostics = []
    iterations = 0
    while candidates:
        iterations += 1
        dropped = True
        while dropped:
            dropped = False
            for name in list(candidates):
                available = {other for other,c in candidates.items()
                             if not c["static"] or c["source"] == candidates[name]["source"]}
                missing = dependencies[name] - available - known - file_definitions.get(candidates[name]["source"], set())
                if missing:
                    rejected[name] = "unrecovered dependencies: " + ", ".join(sorted(missing))
                    del candidates[name]
                    dropped = True
        if not candidates:
            break
        parts = ['#include "manalink.h"\n']
        for name, c in candidates.items():
            parts.extend([f'#line 1 "prototype/{name}"\n', c["prototype"] + "\n"])
        for name, c in candidates.items():
            parts.extend([f'#line 1 "body/{name}"\n', c["body"] + "\n"])
        scratch.write_text("".join(parts), encoding="utf-8")
        command = [str(clang), "-target", "i686-pc-windows-msvc", "-std=gnu99", "-fsyntax-only",
                   "-ferror-limit=0", "-fno-color-diagnostics", "-Werror=implicit-function-declaration",
                   "-Werror=implicit-int", "-Werror=incompatible-pointer-types", "-Werror=return-type",
                   "-Werror=uninitialized", "-Werror=sometimes-uninitialized", "-Wno-ignored-attributes",
                   "-I", str(root), str(scratch)]
        result = subprocess.run(command, capture_output=True, text=True)
        compiler_diagnostics.append(result.stderr)
        if result.returncode == 0:
            break
        invalid = set()
        for match in re.finditer(r"^(?:prototype|body)/([^:]+):\d+:\d+: (?:fatal )?error: (.+)$", result.stderr, re.M):
            invalid.add(match[1])
            rejected[match[1]] = "interface/syntax: " + match[2]
        if not invalid:
            (analysis / "promotion-compiler.log").write_text(result.stderr)
            raise RuntimeError("Compiler errors outside candidate functions; see promotion-compiler.log")
        for name in invalid:
            candidates.pop(name, None)
    (analysis / "promotion-compiler.log").write_text("\n".join(compiler_diagnostics))
    report = dict(exported_functions=len(exported_names), initially_reconstructable=len(initial),
                  accepted=len(candidates), compiler_passes=iterations,
                  source_files=dict(collections.Counter(c["source"] for c in candidates.values())),
                  accepted_functions=[{k:v for k,v in c.items() if k not in ("body",)} for c in candidates.values()],
                  deferred=rejected)
    for name in candidates:
        report["deferred"].pop(name, None)
    (analysis / "promotion-report.json").write_text(json.dumps(report, indent=2))
    if args.apply:
        by_file = collections.defaultdict(list)
        for candidate in candidates.values():
            by_file[candidate["source"]].append(candidate)
        public = []
        for relative, entries in by_file.items():
            destination = root / relative
            content = destination.read_bytes() if destination.exists() else b'#include "manalink.h"\n'
            addition = "\n\n/* Recovered October 2022 implementations; see docs/recovery-progress.md. */\n"
            needed = set().union(*(dependencies[c["name"]] for c in candidates.values()))
            addition += "\n".join(c["prototype"] for c in entries if c["name"] in needed) + "\n\n"
            for c in sorted(entries, key=lambda c: int(c["line"] or 0)):
                addition += f"/* ManalinkEh 2022: 0x{c['address']}; original {relative}:{c['line']}. */\n"
                addition += c["body"] + "\n"
                if not c["static"] and not c["name"].startswith("card_"):
                    public.append(c["prototype"])
            destination.write_bytes((content.rstrip() + addition.encode()).replace(b"\r\n", b"\n").replace(b"\n", b"\r\n"))
        if public:
            header = root / "manalink.h"
            content = header.read_bytes()
            marker = content.rfind(b"#ifdef __cplusplus")
            addition = "/* Interfaces recovered from October 2022 DWARF. */\n" + "\n".join(sorted(public)) + "\n\n"
            header.write_bytes((content[:marker] + addition.encode() + content[marker:]).replace(b"\r\n", b"\n").replace(b"\n", b"\r\n"))
        report["applied"] = True
        manifest = root / "docs/recovery-evidence/recovered-functions.json"
        previous = json.loads(manifest.read_text()) if manifest.exists() else {}
        merged = {(f["source"], f["address"]):f for f in previous.get("accepted_functions", [])}
        merged.update({(f["source"], f["address"]):f for f in report["accepted_functions"]})
        saved = dict(report)
        saved["accepted_functions"] = list(merged.values())
        saved["accepted"] = len(merged)
        saved["latest_batch_accepted"] = len(candidates)
        saved["source_files"] = dict(collections.Counter(f["source"] for f in merged.values()))
        saved["card_handler_functions"] = sum(f["name"].startswith("card_") for f in merged.values())
        saved["helper_functions"] = len(merged) - saved["card_handler_functions"]
        saved["tests"] = "Deferred by user request; no tests added or run."
        saved["native_build"] = "Not run; downstream integration and behavioral validation remain."
        manifest.write_text(json.dumps(saved, indent=2))
    print(json.dumps({k:v for k,v in report.items() if k not in ("accepted_functions", "deferred")}, indent=2))


if __name__ == "__main__":
    main()
