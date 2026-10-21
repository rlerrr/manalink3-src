"""Restore spacing in unstaged recovery additions without changing code tokens.

Uses raw exports for original blank-line hints. Binary clang-format pipes avoid
Windows text-mode conversion of CRLF into CRCRLF. The Git index is read only.
"""
import argparse
import collections
import hashlib
import json
import re
import subprocess
from pathlib import Path

from inspect_release import Image
from promote_recovery import LITERAL, clean, strip_comments, MANA_API_NAMES
from recover_literals import recover_strings


SECTION = "/* Recovered October 2022 implementations; see docs/recovery-progress.md. */"
INTERFACES = "/* Interfaces recovered from October 2022 DWARF. */"
STYLE = "{BasedOnStyle: LLVM, IndentWidth: 2, ColumnLimit: 100, BreakBeforeBraces: Allman, SortIncludes: Never}"
COMMENT = r"/\*.*?\*/|//[^\r\n]*"
TOKEN = re.compile(LITERAL + r"|[A-Za-z_]\w*|\d[\w.]*|>>=|<<=|\.\.\.|->|\+\+|--|&&|\|\||[+*/%&|^!=<>-]=|<<|>>|\S")


def tokens(text):
    values = []
    for match in TOKEN.finditer(strip_comments(text)):
        token = match[0]
        if token.startswith('"') and values and values[-1].startswith('"'):
            values[-1] = values[-1][:-1] + token[1:]
        else:
            values.append(token)
    return values


def comments(text):
    return [m[0] for m in re.finditer(LITERAL + "|" + COMMENT, text, re.S) if m[0].startswith(("/*", "//"))]


def definition_span(text, name):
    start = text.index("*/") + 2
    marker_end = start
    while True:
        start += len(text[start:]) - len(text[start:].lstrip())
        if text.startswith("/*", start):
            start = text.index("*/", start) + 2
        else:
            break
    masked = re.sub(LITERAL + "|" + COMMENT, lambda m: " " * len(m[0]), text, flags=re.S)
    match = re.search(r"\b" + re.escape(name) + r"\s*\(", masked[start:])
    if not match:
        raise ValueError("Missing function " + name)
    opening = masked.index("{", start + match.end())
    depth = 1
    for at in range(opening + 1, len(masked)):
        depth += (masked[at] == "{") - (masked[at] == "}")
        if depth == 0:
            return marker_end, start, at + 1
    raise ValueError("Unclosed function " + name)


def format_bytes(data):
    result = subprocess.run(["C:/Program Files/LLVM/bin/clang-format.exe", "-style=" + STYLE],
                            input=data.replace(b"\r\n", b"\n"), capture_output=True, check=True)
    return result.stdout.replace(b"\r\n", b"\n").decode("latin1").rstrip()


def compact_declarations(text):
    lines = []
    for line in text.splitlines():
        if not line.strip():
            continue
        if line.lstrip().startswith("/*") and lines:
            lines.append("")
        lines.append(line)
    return format_bytes("\n".join(lines).encode("latin1"))


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument("--apply", action="store_true")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    analysis = root / ".recovery"
    manifest = json.loads((root / "docs/recovery-evidence/recovered-functions.json").read_text())
    image = Image((analysis / "2022-ManalinkEh.dll").read_bytes())
    overlapping = [f[1] for line in (analysis / "absolute-interfaces.tsv").read_text().splitlines()
                   if (f := line.split("\t"))[2] == "global"]
    by_file = collections.defaultdict(list)
    for function in manifest["accepted_functions"]:
        by_file[function["source"]].append(function)
    by_file["manalink.h"] = []
    changes, skipped = {}, []
    index_before = subprocess.check_output(["git", "ls-files", "--stage", "-z"], cwd=root)
    for relative, functions in by_file.items():
        path = root / relative
        original = path.read_bytes()
        text = original.decode("latin1").replace("\r\n", "\n")
        staged = subprocess.run(["git", "show", ":" + relative], cwd=root, capture_output=True).stdout.decode("latin1")
        edits = []
        formatted_eof = False
        for function in functions:
            marker = "/* ManalinkEh 2022: 0x" + function["address"] + ";"
            if marker not in staged and function.get("reconstruction"):
                at = text.index(marker)
                _, _, end = definition_span(text[at:], function["name"])
                if not text[at + end:].strip():
                    formatted_eof = True
            if marker in staged or function.get("reconstruction"):
                continue
            at = text.index(marker)
            marker_end, start, end = definition_span(text[at:], function["name"])
            current = text[at + start:at + end]
            raw = (analysis / "decompiled-abi" / (function["address"] + ".c")).read_text(encoding="utf-8")
            if hashlib.sha256(raw.encode()).hexdigest() != function["raw_sha256"]:
                raise ValueError("Raw export changed: " + function["name"])
            aliases = {e["symbol"]:int(e["address"], 16) for e in function.get("recovered_literals", []) if e["symbol"] == "_rdata"}
            resolved, _ = recover_strings(strip_comments(raw), image, aliases)
            # Keep the export's intentional paragraph breaks: clean() otherwise
            # collapses lines containing only indentation along with their break.
            resolved = re.sub(r"(?m)^[ \t]+$", "", resolved)
            candidate = ("static " if function["static"] else "") + clean(
                resolved, overlapping, function.get("local_type_declarations", []), function.get("array_boundaries", []),
                use_keyword_accessors_2022=("get_special_abilities_2022(" in current or "check_for_special_ability_2022(" in current),
                use_extension_flags_2022=bool(re.search(r'\b(?:get|set|remove|check)_special_flags[23]?(?:_by_instance)?_2022\s*\(',current)),
                use_mana_api_2022=bool(re.search(r'\b(?:' + '|'.join(MANA_API_NAMES) + r')_2022\s*\(',current)),
                use_token_api_2022=bool(re.search(r'\btoken_generation_2022_t\b',current)),
                use_test_api_2022=bool(function.get('test_interface_2022')),
                use_target_api_2022=bool(function.get('target_interface_2022')),
                use_sacrifice_api_2022=bool(function.get('sacrifice_interface_2022')),
                use_spell_api_2022=bool(function.get('spell_interface_2022')),
                use_ability_api_2022=bool(function.get('ability_interface_2022')),
                use_pump_api_2022=bool(function.get('pump_interface_2022')),
                use_activated_api_2022=bool(function.get('activated_interface_2022')),
                use_attack_api_2022=bool(function.get('attack_interface_2022')),
                use_entry_api_2022=bool(function.get('entry_interface_2022')),
                use_level_buddy_api_2022=bool(function.get('level_buddy_interface_2022')),
                use_phyrexian_api_2022=bool(function.get('phyrexian_interface_2022')),
                use_spell_trigger_api_2022=bool(function.get('spell_trigger_interface_2022')),
                use_death_trigger_api_2022=bool(function.get('death_trigger_interface_2022')),
                use_action_api_2022=bool(function.get('action_interface_2022')),
                use_global_tutor_api_2022=bool(function.get('global_tutor_interface_2022')),
                use_specific_cip_api_2022=bool(function.get('specific_cip_interface_2022')),
                use_life_api_2022=bool(function.get('life_interface_2022')),
                use_cannot_lose_api_2022=bool(function.get('cannot_lose_interface_2022')),
                use_life_loss_api_2022=bool(function.get('life_loss_interface_2022')))
            formatted = format_bytes(candidate.encode("utf-8"))
            if tokens(current) != tokens(formatted) or comments(current) != comments(formatted):
                skipped.append(relative + "::" + function["name"])
                continue
            if not text[at + end:].strip():
                formatted_eof = True
            if text[at + marker_end:at + start].strip():
                edits.append((at + start, at + end, formatted))
            else:
                edits.append((at + marker_end, at + end, "\n" + formatted))
        section = INTERFACES if relative == "manalink.h" else SECTION
        positions = [m.start() for m in re.finditer(re.escape(section), text)]
        for pos in positions[staged.count(section):]:
            start = pos + len(section)
            if relative == "manalink.h":
                next_section = text.find(section, start)
                end = next_section if next_section >= 0 else text.rfind("#ifdef __cplusplus")
            else:
                end = text.index("/* ManalinkEh 2022:", start)
            declarations = compact_declarations(text[start:end])
            edits.append((start, end, "\n" + declarations + "\n\n" if declarations else "\n\n"))
        for start, end, replacement in sorted(edits, reverse=True):
            text = text[:start] + replacement + text[end:]
        if formatted_eof:
            text = text.rstrip("\n") + "\n"
        if tokens(original.decode("latin1")) != tokens(text):
            raise ValueError("Token change refused: " + relative)
        result = text.replace("\n", "\r\n").encode("latin1")
        if result != original:
            changes[relative] = result
    if args.apply:
        for relative, data in changes.items():
            (root / relative).write_bytes(data)
    if subprocess.check_output(["git", "ls-files", "--stage", "-z"], cwd=root) != index_before:
        raise RuntimeError("Git index changed during formatting")
    report = dict(files=len(changes), applied=args.apply, skipped=skipped,
                  code_tokens="Unchanged in every affected file.", staging="Unchanged.")
    (analysis / "format-report.json").write_text(json.dumps(report, indent=2))
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
