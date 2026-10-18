"""Audit source provenance and literal bytes against cached recovery inputs."""
import hashlib
import json
import re
import struct
from pathlib import Path

from inspect_release import Image
from promote_recovery import LITERAL, strip_comments, aggregate_stack_aliases
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
    literal_pointer_stores = 0
    literal_constants = 0
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
            for entry in function.get("reconstructed_literal_constants", []):
                # Manually reconstructed VLA call frames can lose typed string
                # arguments. The raw pointer spelling, read-only bytes and all
                # recorded immediate materializations must agree. These are
                # explicit per-function reconstructions, never a global cast.
                from capstone import Cs, CS_ARCH_X86, CS_MODE_32
                from capstone.x86_const import X86_OP_IMM
                address = int(entry['address'], 16)
                token = '&' + entry['symbol']
                literal = readonly_string(image, address)
                if (entry['symbol'] not in {f'UNK_{address:08x}', '_rdata'} or
                    (entry['symbol'] == '_rdata' and entry['raw_occurrences'] != 1) or
                    literal != bytes.fromhex(entry['bytes']) or
                    literal_input.count(token) != entry['raw_occurrences'] or
                    not entry['materializations']):
                    raise ValueError('VLA literal pointer provenance mismatch')
                for site in entry['materializations']:
                    at, code = int(site['address'],16), bytes.fromhex(site['bytes'])
                    decoder = Cs(CS_ARCH_X86, CS_MODE_32);decoder.detail = True
                    instructions = list(decoder.disasm(code, at))
                    if (machine_bytes(at,len(code)) != code or len(instructions) != 1 or
                        not int(function['address'],16) <= at < int(entry['function_end'],16)):
                        raise ValueError('VLA literal instruction mismatch')
                    ins = instructions[0]
                    if (ins.mnemonic != 'mov' or ins.operands[1].type != X86_OP_IMM or
                        ins.operands[1].imm != address or ins.operands[0].size != 4):
                        raise ValueError('VLA literal immediate mismatch')
                literal_input = literal_input.replace(token,byte_literal(literal))
                literal_constants += entry['raw_occurrences']
            for entry in function.get("reconstructed_literal_pointers", []):
                # Typed pointer fields may still print as &UNK. Require each
                # materialization to be a four-byte immediate MOV to memory,
                # inside this function, pointing at the exact read-only bytes.
                from capstone import Cs, CS_ARCH_X86, CS_MODE_32
                from capstone.x86_const import X86_OP_MEM, X86_OP_IMM
                address = int(entry["address"], 16)
                literal = readonly_string(image, address)
                token = '&' + entry["symbol"]
                if (entry["symbol"] != f'UNK_{address:08x}' or
                    literal != bytes.fromhex(entry["bytes"]) or
                    literal_input.count(token) != len(entry["materializations"])):
                    raise ValueError("Reconstructed literal pointer provenance mismatch")
                for site in entry["materializations"]:
                    at, code = int(site["address"], 16), bytes.fromhex(site["bytes"])
                    decoded = Cs(CS_ARCH_X86, CS_MODE_32); decoded.detail = True
                    instructions = list(decoded.disasm(code, at))
                    if (machine_bytes(at, len(code)) != code or len(instructions) != 1 or
                        not int(function["address"], 16) <= at < int(entry["function_end"], 16)):
                        raise ValueError("Reconstructed literal pointer machine range mismatch")
                    ins = instructions[0]
                    if (ins.mnemonic != 'mov' or ins.operands[0].type != X86_OP_MEM or
                        ins.operands[0].size != 4 or ins.operands[1].type != X86_OP_IMM or
                        ins.operands[1].imm != address):
                        raise ValueError("Reconstructed literal pointer MOV mismatch")
                literal_input = literal_input.replace(token, byte_literal(literal))
                literal_pointer_stores += len(entry["materializations"])
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
            native_string = function.get('native_string_view')
            if native_string:
                address = int(native_string['address'],16)
                argument = native_string['argument']
                expected_load = b'\xc7\x44\x24' + bytes([argument * 4]) + struct.pack('<I',address)
                if (native_string['type'] != 'const char *' or
                    expected_load.hex() != native_string['load_bytes'] or
                    machine_bytes(int(native_string['load_address'],16),len(expected_load)) != expected_load):
                    raise ValueError('Native runtime-string argument mismatch')
            masks = function.get('state_mask_evidence')
            if masks:
                if (masks['raw_sha256'] != function['raw_sha256'] or masks['source'] != function['source'] or
                    masks['unit_evidence']['state_member']['offset'] != 8 or
                    masks['unit_evidence']['state_member']['bytes'] != 4):
                    raise ValueError('State-mask type provenance mismatch')
                for replacement in masks['replacements']:
                    match = re.fullmatch(r'\b\w+->state\s*[&|]\s*(0x[0-9a-fA-F]+)', replacement['original'])
                    if (not match or int(match[1],16) != replacement['mask'] or
                        sum(replacement['bits'].values()) != replacement['mask']):
                        raise ValueError('State-mask value mismatch')
                    for name,number in replacement['bits'].items():
                        if masks['unit_evidence']['enumerators'][name]['value'] != number:
                            raise ValueError('State-mask enumerator mismatch')
                    for site in replacement['machine_sites']:
                        code=bytes.fromhex(site['bytes'])
                        if machine_bytes(int(site['address'],16),len(code)) != code:
                            raise ValueError('State-mask instruction mismatch')
            if function.get("reconstruction_body_sha256") and hashlib.sha256(
                    body.replace("\r\n", "\n").encode()).hexdigest() != function["reconstruction_body_sha256"]:
                raise ValueError("Manual reconstruction body hash mismatch")
            if aggregate_stack_aliases(body):
                raise ValueError("Cross-type stack cast remains without verified storage")
            storage = function.get("stack_storage_reconstruction")
            if storage and hashlib.sha256(body.replace("\r\n", "\n").encode()).hexdigest() != storage["body_sha256"]:
                raise ValueError("Reconstructed stack-storage body hash mismatch")
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
        source = strip_comments(sources[declaration["source"]]).replace('\r\n', '\n')
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
    report = dict(functions=len(manifest["accepted_functions"]), source_fragments=len(manifest.get("source_fragments", [])),
                  stack_storage_functions=sum(bool(f.get("stack_storage_reconstruction")) for f in manifest["accepted_functions"]),
                  recovered_literal_arguments=recovered_strings,
                  reconstructed_literal_pointer_stores=literal_pointer_stores,
                  reconstructed_literal_constants=literal_constants,
                  source_markers="All unique and present." if not issues else "See issues.",
                  raw_export_hashes="All match." if not issues else "See issues.",
                  literal_values="Checked, including byte-exact read-only arguments, verified pointer constants/stores and adjacent C strings.",
                  issues=issues)
    (analysis / "recovery-audit.json").write_text(json.dumps(report, indent=2))
    if not issues:
        manifest["provenance_audit"] = {**report, "issues": 0}
        path.write_text(json.dumps(manifest, indent=2))
    print(json.dumps(report, indent=2))
    raise SystemExit(bool(issues))


if __name__ == "__main__":
    main()
