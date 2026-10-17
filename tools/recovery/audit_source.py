"""Audit source provenance and literal bytes against cached recovery inputs."""
import hashlib
import json
import re
from pathlib import Path

from inspect_release import Image
from promote_recovery import LITERAL, strip_comments
from recover_literals import recover_strings, readonly_string


def literal_tokens(text):
    text = strip_comments(text)
    values, end = [], -1
    for match in re.finditer(LITERAL, text):
        token = match[0]
        if values and token[0] == '"' and values[-1][0] == '"' and not text[end:match.start()].strip():
            values[-1] = ('"', values[-1][1] + token[1:-1])
        else:
            values.append((token[0], token[1:-1]))
        end = match.end()
    return values


def function_body(source, name):
    source = strip_comments(source)
    masked = re.sub(LITERAL, lambda m: " " * len(m[0]), source)
    match = re.search(r"\b" + re.escape(name) + r"\s*\([^;{}]*\)\s*\{", masked)
    if not match:
        raise ValueError("Missing function definition")
    level = 1
    for at in range(match.end(), len(masked)):
        if masked[at] == "{":
            level += 1
        elif masked[at] == "}":
            level -= 1
            if level == 0:
                return source[match.start():at + 1]
    raise ValueError("Unclosed function body")


def main():
    root = Path(__file__).resolve().parents[2]
    analysis = root / ".recovery"
    path = root / "docs/recovery-evidence/recovered-functions.json"
    manifest = json.loads(path.read_text())
    image = Image((analysis / "2022-ManalinkEh.dll").read_bytes())
    sources = {p:(root / p).read_bytes().decode("utf-8", errors="replace") for p in manifest["source_files"]}
    issues = []
    recovered_strings = 0
    for function in manifest["accepted_functions"]:
        try:
            raw = (analysis / "decompiled-abi" / (function["address"] + ".c")).read_text(encoding="utf-8")
            if hashlib.sha256(raw.encode()).hexdigest() != function["raw_sha256"]:
                raise ValueError("Raw export hash mismatch")
            for support in function.get("supporting_exports", []):
                support_raw = (analysis / "decompiled-abi" / (support["address"] + ".c")).read_text(encoding="utf-8")
                if hashlib.sha256(support_raw.encode()).hexdigest() != support["raw_sha256"]:
                    raise ValueError("Supporting export hash mismatch")
            evidence = function.get("recovered_literals", [])
            aliases = {e["symbol"]:int(e["address"], 16) for e in evidence if e["symbol"] == "_rdata"}
            for entry in evidence:
                if readonly_string(image, int(entry["address"], 16)) != bytes.fromhex(entry["bytes"]):
                    raise ValueError("Read-only string byte mismatch")
            expected, resolved = recover_strings(strip_comments(raw), image, aliases)
            if resolved != evidence:
                raise ValueError("Literal argument provenance mismatch")
            recovered_strings += len(evidence)
            source = sources[function["source"]]
            marker = "/* ManalinkEh 2022: 0x" + function["address"] + ";"
            if source.count(marker) != 1:
                raise ValueError("Missing or repeated source provenance marker")
            body = function_body(source[source.index(marker):], function["name"])
            if literal_tokens(expected) != literal_tokens(body):
                raise ValueError("Source literal mismatch")
        except (ValueError, OSError) as error:
            issues.append(dict(source=function["source"], name=function["name"], error=str(error)))
    report = dict(functions=len(manifest["accepted_functions"]), recovered_literal_arguments=recovered_strings,
                  source_markers="All unique and present." if not issues else "See issues.",
                  raw_export_hashes="All match." if not issues else "See issues.",
                  literal_values="Checked, including byte-exact read-only argument recovery and adjacent C strings.",
                  issues=issues)
    (analysis / "recovery-audit.json").write_text(json.dumps(report, indent=2))
    if not issues:
        manifest["provenance_audit"] = {**report, "issues": 0}
        path.write_text(json.dumps(manifest, indent=2))
    print(json.dumps(report, indent=2))
    raise SystemExit(bool(issues))


if __name__ == "__main__":
    main()
