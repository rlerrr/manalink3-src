"""Audit source provenance and literal bytes against cached recovery inputs."""
import hashlib
import json
import re
import struct
from pathlib import Path

from inspect_release import Image
from promote_recovery import LITERAL, strip_comments
from recover_literals import recover_strings, readonly_string, byte_literal


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
    def machine_bytes(address, length):
        rva = address - image.pe.OPTIONAL_HEADER.ImageBase
        for section in image.pe.sections:
            if section.VirtualAddress <= rva < section.VirtualAddress + section.Misc_VirtualSize:
                offset = section.PointerToRawData + rva - section.VirtualAddress
                return image.data[offset:offset + length]
        raise ValueError("Machine literal load outside runtime sections")
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
            literal_input = strip_comments(raw)
            for entry in function.get("reconstructed_literal_loads", []):
                # VLA stack state can obscure a direct constant argument. Accept
                # its explicit reconstruction only with the x86 ESP-relative
                # argument store and relative CALL bytes verified independently.
                address = int(entry["address"], 16)
                argument = entry["argument"]
                if not 0 <= argument < 32:
                    raise ValueError("Unsupported reconstructed literal argument")
                load = b'\xc7\x44\x24' + bytes([argument * 4]) + struct.pack('<I', address)
                load_at = int(entry["load_address"], 16)
                call_at = int(entry["call_address"], 16)
                target = int(entry["call_target"], 16)
                call = b'\xe8' + struct.pack('<i', target - call_at - 5)
                if (load.hex() != entry["load_bytes"] or machine_bytes(load_at, len(load)) != load or
                    call.hex() != entry["call_bytes"] or machine_bytes(call_at, len(call)) != call or
                    not load_at + len(load) <= call_at < load_at + 64):
                    raise ValueError("Reconstructed literal machine argument mismatch")
                literal = readonly_string(image, address)
                if literal != bytes.fromhex(entry["bytes"]):
                    raise ValueError("Reconstructed literal byte mismatch")
                token = '&' + entry["symbol"]
                if literal_input.count(token) != 1:
                    raise ValueError("Reconstructed literal raw load is ambiguous")
                literal_input = literal_input.replace(token, byte_literal(literal))
                recovered_strings += 1
            expected, resolved = recover_strings(literal_input, image, aliases)
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
            compact = re.sub(r"\s+", "", body)
            for enum in function.get("local_enum_evidence", []):
                for name, number in enum["enumerators"].items():
                    if not re.search(r"\b" + name + "=" + str(number) + r"\b", compact):
                        raise ValueError("Function-local enum value mismatch")
            boundaries = function.get("array_boundaries", [])
            for boundary in boundaries:
                if re.sub(r"\s+", "", boundary["original"]) in compact:
                    raise ValueError("Adjacent-object array endpoint remains")
                count = sum(b["replacement"] == boundary["replacement"] for b in boundaries)
                if compact.count(re.sub(r"\s+", "", boundary["replacement"])) != count:
                    raise ValueError("Recovered array endpoint mismatch")
        except (ValueError, OSError) as error:
            issues.append(dict(source=function["source"], name=function["name"], error=str(error)))
    for declaration in manifest.get("source_globals", []):
        source = strip_comments(sources[declaration["source"]])
        if source.count(declaration["declaration"]) != 1:
            issues.append(dict(source=declaration["source"], name=declaration["name"],
                               error="Missing or repeated recovered global declaration"))
    for fragment in manifest.get("source_fragments", []):
        try:
            raw = (analysis / "decompiled-abi" / (fragment["address"] + ".c")).read_text(encoding="utf-8")
            if hashlib.sha256(raw.encode()).hexdigest() != fragment["raw_sha256"]:
                raise ValueError("Fragment raw export hash mismatch")
            source = sources[fragment["source"]]
            start, end = fragment["start_marker"], fragment["end_marker"]
            if source.count(start) != 1 or source.count(end) != 1:
                raise ValueError("Missing or repeated fragment marker")
            body = source.split(start)[1].split(end)[0].strip().replace("\r\n", "\n")
            if hashlib.sha256(body.encode()).hexdigest() != fragment["body_sha256"]:
                raise ValueError("Source fragment hash mismatch")
        except (ValueError, OSError) as error:
            issues.append(dict(source=fragment["source"], name=fragment["original_function"], error=str(error)))
    report = dict(functions=len(manifest["accepted_functions"]), source_fragments=len(manifest.get("source_fragments", [])), recovered_literal_arguments=recovered_strings,
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
