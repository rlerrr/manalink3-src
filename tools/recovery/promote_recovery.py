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


UNRESOLVED = re.compile(r"\b(?:undefined\w*|in_(?!play(?:_and_attacking)?\b|hand\b)\w+|extraout_\w+|unaff_\w+|_?DAT_[0-9a-fA-F]{8}\w*|UNK_\w+|PTR_\w+|func_0x[0-9a-fA-F]+|field_\w+|CONCAT\w*|SUB\d+|ZEXT\w*|SEXT\w*)\b")
LITERAL = r'"(?:\\.|[^"\\\r\n])*"|\'(?:\\.|[^\'\\\r\n])*\''
FUNCTION_DEFINITION = re.compile(r"\b([A-Za-z_]\w*)\s*\([^;{}()]*\)\s*\{")
STACK_AGGREGATES = ('test_definition_t', 'target_definition_t', 'token_generation_t')


def aggregate_stack_aliases(body):
    """Cross-type addresses of small locals need verified storage reconstruction."""
    declarations = {m[2]: m[1] for m in re.finditer(
        r'(?m)^\s+([A-Za-z_]\w*)\s+(\w+)\s*(?:\[[^]\n]*\])?\s*;', body)}
    return [(m[1], m[2], declarations[m[2]]) for m in re.finditer(
        r'\((' + '|'.join(STACK_AGGREGATES) + r')\s*\*\)\s*&\s*(\w+)\b', body)
        if m[2] in declarations and declarations[m[2]] != m[1]]


def strip_comments(text):
    return re.sub(LITERAL + r"|/\*.*?\*/|//[^\r\n]*", lambda m: "" if m[0].startswith(("/*", "//")) else m[0], text, flags=re.S)


def code_only(text):
    return re.sub(LITERAL, " ", text)


def bound_variables(text):
    """Ghidra emits simple named parameters and one local per declaration."""
    signature, body = code_only(text).split("{", 1)
    parameters = signature[signature.index("(") + 1:signature.rfind(")")]
    names = {m[1] for part in parameters.split(",")
             if (m := re.search(r"\b([A-Za-z_]\w*)\s*(?:\[[^]]*\])?\s*$", part))}
    declarations = re.finditer(
        r"(?m)^\s+(?!return\b|goto\b|else\b|case\b)"
        r"(?:(?:const|volatile|unsigned|signed|long|short|struct|enum)\s+)*"
        r"[A-Za-z_]\w*(?:[ \t]+\*?\s*|\s*\*\s*)"
        r"([A-Za-z_]\w*)\s*(?:\[[^]\n]*\]\s*)*(?:;|=)", body)
    names.update(m[1] for m in declarations)
    return names


def rewrite_arity_call(text, name, arity, replacement):
    edits = []
    for match in re.finditer(r"\b" + re.escape(name) + r"\s*\(", text):
        level, count = 0, 1
        for char in text[match.end():]:
            if char in "([":
                level += 1
            elif char in ")]":
                if char == ")" and level == 0:
                    if count == arity:
                        edits.append((match.start(), match.start() + len(name)))
                    break
                level -= 1
            elif char == "," and level == 0:
                count += 1
    for start, end in reversed(edits):
        text = text[:start] + replacement + text[end:]
    return text


def rewrite_argument(text, name, arity, argument, replacements):
    """Rewrite one exact argument token, after literals have been protected."""
    edits = []
    for match in re.finditer(r"\b" + re.escape(name) + r"\s*\(", text):
        start, level, arguments = match.end(), 0, []
        for at in range(start, len(text)):
            char = text[at]
            if char in "([":
                level += 1
            elif char in ")]":
                if char != ")" or level != 0:
                    level -= 1
                    continue
            if (char == "," or char == ")") and level == 0:
                arguments.append((start, at))
                start = at + 1
                if char == ")":
                    if arity is None or len(arguments) == arity:
                        for index in (range(len(arguments)) if argument is None else [argument]):
                            begin, end = arguments[index]
                            token = text[begin:end].strip()
                            if token in replacements:
                                edits.append((begin, end, replacements[token]))
                    break
    for begin, end, replacement in reversed(edits):
        text = text[:begin] + replacement + text[end:]
    return text


def clean(text, overlapping_globals=(), local_type_declarations=(), array_boundaries=(), state_mask_replacements=()):
    text = strip_comments(text).strip()
    for replacement in state_mask_replacements:
        text = text.replace(replacement['original'], replacement['replacement'])
    literals = []
    def save_literal(match):
        literals.append(match[0])
        return f"RECOVERY_LITERAL_{len(literals)-1}"
    text = re.sub(LITERAL, save_literal, text)
    if re.search(r'\bload_text_2022\s*\(', text):
        # Both spellings point at the first byte of the verified 249x300
        # native buffer. A string formatter needs the first char row.
        for formatter in ('scnprintf', 'snprintf', '_snprintf', 'sprintf'):
            text = rewrite_argument(text, formatter, None, None, {'text_lines':'text_lines[0]'})
    for boundary in array_boundaries:
        text = text.replace(boundary["original"], boundary["replacement"])
    # DWARF callback int (*)(int, int, event_t) is the existing CardFunction.
    text = re.sub(r"\b_func_int_int_int_event_t\s*\*", "CardFunction ", text)
    # DWARF callback int (*)(int, int), used by quantity/predicate helpers.
    text = re.sub(r"\b_func_int_int_int\s*\*", "CardPairFunction ", text)
    # Preserve the old four-argument ABI. New six-argument calls retain their
    # source/recipient pair through the separately recovered target core.
    text = rewrite_arity_call(text, "add_counters", 6, "add_counters_from_source")
    # show_deck deliberately accepts an integer address for its final label
    # argument in this source tree. Keep the localized engine strings live.
    text = rewrite_argument(text, "show_deck", 6, 5,
                            {"str_Done": "ENGINE_DONE_LABEL", "str_OK": "ENGINE_OK_LABEL"})
    # Identical kill-code value and meaning in the two interfaces.
    text = re.sub(r"\bACT_KILL_EXILE\b", "ACT_KILL_REMOVE", text)
    # Existing auras.c documents this packed player/card engine global at
    # exactly 0x78fa30; give the recovered access a descriptive source name.
    text = re.sub(r"\b_DAT_0078fa30\b", "engine_selected_card", text)
    # These are flag arguments, not executable addresses. Keep the match
    # restricted to simple three-argument calls to this typed accessor.
    text = re.sub(r"(\bcheck_special_flags\([^(),]*,[^(),]*,)\s*0x800000(\s*\))",
                  r"\1SF_MOVE_AURA_LEGAL_TARGET\2", text)
    # Both DWARF units and the existing header identify this state mask.
    text = re.sub(r"(\bcheck_state\([^(),]*,[^(),]*,)\s*0x800000(\s*\))",
                  r"\1STATE_OUBLIETTED\2", text)
    text = re.sub(r"\b__(?:cdecl|regparm[123])\b", "", text)
    # Ghidra's fixed-length string-copy intrinsic has the C strncpy contract.
    text = re.sub(r"\bbuiltin_strncpy\b", "strncpy", text)
    for old, new in (("uint", "unsigned int"), ("ushort", "unsigned short"),
                     ("byte", "unsigned char"), ("uchar", "unsigned char"), ("ulonglong", "unsigned long long"),
                     ("longlong", "long long")):
        text = re.sub(r"\b" + old + r"\b", new, text)
    # The corrected EDX:EAX return can still expose the packed low word.
    # Its two uint16_t fields are fixed at offsets zero and two.
    for local in set(re.findall(r'\binst_info_t\s+(\w+)\s*;', text)):
        for suffix, member in [('_0_2_', 'id'), ('_2_2_', 'data2'), ('_4_4_', 'data')]:
            text = re.sub(r'\b' + re.escape(local) + r'\.' + suffix + r'\b', local + '.' + member, text)
        text = re.sub(r'\b' + re.escape(local) + r'\._0_4_\b(?!\s*(?:=(?!=)|[+*/%&|^-]=))',
            '((uint32_t)' + local + '.id | ((uint32_t)' + local + '.data2 << 16))', text)
    def damage_word(match):
        name, instance = match[1], match[2]
        if not re.search(r'\b(?:unsigned int|uint32_t)\s+' + re.escape(name) + r'\s*;', text):
            return match[0]
        return '\n  ' + name + ' = (uint32_t)' + instance + '->damage_on_card | ((uint32_t)(uint16_t)' + instance + '->counter_power << 16);'
    text = re.sub(r'(?m)^\s*(\w+)\._0_2_\s*=\s*(\w+)->damage_on_card\s*;\s*'
                  r'\1\._2_2_\s*=\s*\2->counter_power\s*;', damage_word, text)
    # Verified anonymous card_instance_t unions: offset 0x38 contains
    # info_slot/info_slot_fn; offset 0x4c contains eot_toughness and its bytes.
    # Retain the real member names rather than Ghidra's generated containers.
    text = re.sub(r"\((\w+)->field_26\)\.(info_slot(?:_fn)?)\b", r"\1->\2", text)
    # The four-byte slot union copied into the first word of a target entry
    # is a bit-preserving scalar copy on the verified 32-bit instance layout.
    text = re.sub(r"\*\(anon_union_4_2_[0-9a-f]+_for_card_instance_struct_26\s*\*\)"
                  r"\((\w+)->targets\s*\+\s*(\w+)\)\s*=\s*(\w+)->field_26\s*;",
                  r"\1->targets[\2].player = \3->info_slot;", text)
    text = re.sub(r"\(int32_t\)\s*(\w+)->field_26\b", r"\1->info_slot", text)
    text = re.sub(r"\((\w+)->field_31\)\.field_1\.(eot_toughness[0-3])\b", r"\1->\2", text)
    text = re.sub(r"\((\w+)->field_31\)\.(eot_toughness)\b", r"\1->\2", text)
    text = re.sub(r"\(code\s*\*\)\s*(\w+)->info_slot\b", r"\1->info_slot_fn", text)
    # DWARF identifies this events.c global as a four-byte int. Its second
    # byte is a Ghidra partial-value expression, not a real C struct member.
    text = text.replace("raw_characteristic_pt._1_1_", "((unsigned int)raw_characteristic_pt >> 8 & 0xff)")
    # DWARF + matching executable bindings prove these are four-byte ints.
    # Only scalar reads are representable here; writes to an extracted value
    # remain invalid C and are rejected by the compiler.
    for word in ("event_flags", "land_can_be_played", "trigger_cause_controller", "event_result"):
        def partial_word(match):
            offset, size = int(match[1]), int(match[2])
            assert offset + size <= 4
            return "((unsigned int)" + word + " >> " + str(offset * 8) + " & " + hex((1 << (size * 8)) - 1) + ")"
        text = re.sub(r"\b" + word + r"\._([0-3])_([12])_", partial_word, text)
    # stack_data records are four bytes; generating_event is at offset two.
    text = text.replace("*(char *)(stack_size * 4 + 0x50962e)",
                        "(char)stack_data[stack_size - 1].generating_event")
    text = text.replace("*(short *)(stack_size * 4 + 0x50962c)",
                        "stack_data[stack_size - 1].internal_card_id")
    text = rewrite_arity_call(text, "is_humiliated", 3, "is_humiliated_for_ability")
    # Verified EXE interface player_bits[2] at 0x4ef1a4, four bytes per
    # element; this load selects byte 3 of player_bits[player].
    text = re.sub(r"\*\(unsigned char \*\)\(player \* 4 \+ 0x4ef1a7\)",
                  "((unsigned int)player_bits[player] >> 24 & 0xff)", text)
    # Ghidra loses const on local string pointers. Keep parameters/return types
    # unchanged; compilation still rejects uses that require writable buffers.
    if "{" in text:
        signature, body = text.split("{", 1)
        if local_type_declarations:
            body = "\n  " + "\n  ".join(local_type_declarations) + "\n" + body
        # Opaque scalar loads/stores have known machine widths. Do not infer
        # missing parameter/return types, pointers, aggregate layouts or state.
        for old, new in (("undefined1", "uint8_t"), ("undefined2", "uint16_t"),
                         ("undefined4", "uint32_t"), ("undefined8", "uint64_t")):
            body = re.sub(r"\b" + old + r"\b", new, body)
        if not re.match(r"\s*char\s*\*", signature):
            def character_pointer(match):
                name = re.escape(match[2])
                # Byte views used by lowered copies/clears must stay writable.
                # Header signatures still reject writes through borrowed const
                # pointers and uses that require a mutable string argument.
                direct_write = re.search(r"(?:\*\s*" + name + r"\b|\b" + name +
                    r"\s*\[[^]\n]*\])\s*(?:=(?!=)|[+*/%&|^-]=)", body)
                buffer_writer = re.search(r"\b(?:scnprintf|snprintf|sprintf|vsnprintf|strcpy|strncpy|"
                    r"strcat|strncat|memcpy|memmove|memset)\s*\(\s*" + name + r"\b", body)
                return match[0] if direct_write or buffer_writer else match[1] + "const char *" + match[2] + ";"
            body = re.sub(r"(?m)^(\s+)char\s*\*\s*(\w+)\s*;", character_pointer, body)
        # These source APIs return borrowed, read-only target lists. Restore
        # const only on the locals assigned their results; writes still fail.
        target_readers = ("any_becomes_target", "becomes_target_of_spell_or_effect",
                          "any_creature_becomes_target_of_spell_or_effect", "becomes_target_of_spell")
        for local in set(re.findall(r"\b(\w+)\s*=\s*(?:" + "|".join(target_readers) + r")\s*\(", body)):
            body = re.sub(r"(?m)^(\s+)target_t\s*\*\s*" + re.escape(local) + r"\s*;",
                          r"\1const target_t *" + local + ";", body)
            # Taking the address of a member of the borrowed const list also
            # yields a pointer to const. Writes remain compiler errors.
            aliases = set(re.findall(r"\b(\w+)\s*=\s*&" + re.escape(local) + r"->(?:card|player)\b", body))
            for alias in aliases:
                body = re.sub(r"(?m)^(\s+)(int32_t|int)\s*\*\s*" + re.escape(alias) + r"\s*;",
                              r"\1const \2 *" + alias + ";", body)
        text = signature + "{" + body
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
    local_types_path = root / "docs/recovery-evidence/local-enums.json"
    local_types = json.loads(local_types_path.read_text())["functions"] if local_types_path.exists() else {}
    card_ids_path = root / "docs/recovery-evidence/local-card-ids.json"
    if card_ids_path.exists():
        for address, item in json.loads(card_ids_path.read_text())["functions"].items():
            if address in local_types:
                old = local_types[address]
                assert old["name"] == item["name"] and old["source"] == item["source"]
                old["types"] += item["types"]
                old["declarations"] += item["declarations"]
            else:
                local_types[address] = item
    clang = Path("C:/Program Files/LLVM/bin/clang.exe")
    source_text = {p.relative_to(root).as_posix(): p.read_bytes().decode("utf-8", errors="replace")
                   for folder in ("cards", "functions") for p in (root / folder).glob("*.c")}
    all_sources = "\n".join(source_text.values())
    definitions = set(FUNCTION_DEFINITION.findall(code_only(strip_comments(all_sources))))
    file_definitions = {path:set(FUNCTION_DEFINITION.findall(code_only(strip_comments(text))))
                        for path,text in source_text.items()}
    # Existing set-local helpers and some older shared functions have no
    # declaration in manalink.h. Use their actual source signatures, not the
    # newer binary's potentially changed interfaces, to check new callers.
    source_prototypes = collections.defaultdict(list)
    for source, text in source_text.items():
        for match in re.finditer(r"(?m)^[ \t]*([A-Za-z_][\w\s*]*?\b([A-Za-z_]\w*)\s*\([^;{}()]*\))\s*\{", strip_comments(text)):
            signature, name = match.groups()
            if name in {"if", "for", "while", "switch", "return", "sizeof"}:
                continue
            if name in file_definitions[source]:
                source_prototypes[name].append(dict(source=source, prototype=signature.strip() + ";",
                                                    static=bool(re.search(r"\bstatic\b", signature))))
    functions = {}
    for row in csv.DictReader((analysis / "2022-functions.csv").open()):
        if row["low_pc"] and row["high_pc"]:
            functions[int(row["low_pc"], 16)] = row
    from inspect_release import Image
    from recover_literals import recover_strings
    image = Image((analysis / "2022-ManalinkEh.dll").read_bytes())
    string_aliases = collections.defaultdict(dict)
    aliases_file = analysis / "string-aliases.tsv"
    if aliases_file.exists():
        for row in csv.DictReader(aliases_file.open(), delimiter="\t"):
            string_aliases[int(row["function"], 16)][row["alias"]] = int(row["address"], 16)
    address_symbols = collections.defaultdict(set)
    for symbol, offset, section, kind, storage in image.functions():
        address_symbols[image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[section-1].VirtualAddress + offset].add(symbol)
    static_addresses = {image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[s-1].VirtualAddress + v
                        for n, v, s, t, storage in image.functions() if storage == 3}
    static_names = {n[1:] for n,v,s,t,storage in image.functions() if storage == 3 and "." not in n}
    overlapping_globals = [f[1] for line in (analysis / "absolute-interfaces.tsv").read_text().splitlines()
                           if (f := line.split("\t"))[2] == "global"]
    candidates, rejected = {}, {}
    storage_path = root / 'docs/recovery-evidence/stack-storage.json'
    storage_evidence = json.loads(storage_path.read_text()) if storage_path.exists() else {}
    masks_path = root / 'docs/recovery-evidence/state-masks.json'
    state_masks = json.loads(masks_path.read_text())['functions'] if masks_path.exists() else {}
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
        resolved, recovered_literals = recover_strings(strip_comments(raw), image, string_aliases[address])
        local = local_types.get(row["address"], {})
        if local:
            assert local["source"] == row["source"] and local["name"] == name
        state_proof = state_masks.get(row['address'])
        if state_proof:
            assert state_proof['name'] == name and state_proof['source'] == row['source']
            assert state_proof['raw_sha256'] == hashlib.sha256(raw.encode()).hexdigest()
        body = clean(resolved, overlapping_globals, local.get("declarations", []), local.get("array_boundaries", []),
                     state_proof['replacements'] if state_proof else [])
        storage_proof = None
        prepared_storage = storage_evidence.get('candidates', {}).get(row['address'])
        if prepared_storage:
            assert prepared_storage['source'] == row['source'] and prepared_storage['name'] == name
            assert prepared_storage['raw_sha256'] == hashlib.sha256(raw.encode()).hexdigest()
            assert aggregate_stack_aliases(body) == [tuple(a) for a in prepared_storage['expected_aliases']]
            from recover_stack_storage import rewrite
            previous_body = body
            body, views = rewrite(body, aggregate_stack_aliases(body), prepared_storage['groups'], prepared_storage['target_sizes'])
            from resolve_stack_partials import resolve_partials
            body, partials, unresolved_partials = resolve_partials(body, views, storage_evidence['target_layouts'])
            body = subprocess.run(["C:/Program Files/LLVM/bin/clang-format.exe",
                '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
                input=body.encode(), capture_output=True, check=True).stdout.decode()
            from audit_source import function_body
            storage_proof = dict(storage=views, frame_base=prepared_storage['frame_base'],
                previous_body_sha256=hashlib.sha256(function_body(previous_body, name).encode()).hexdigest(),
                body_sha256=hashlib.sha256(function_body(body, name).encode()).hexdigest(), scope=prepared_storage['scope'],
                resolved_partial_views=partials, unresolved_partial_views=unresolved_partials)
        code = code_only(strip_comments(body))
        if aggregate_stack_aliases(code):
            rejected[name] = "overlapping stack objects awaiting verified storage"
            continue
        if UNRESOLVED.search(code) or re.search(r"::|\bthis\b|\._\d|\.field_", code):
            rejected[name] = "unresolved machine state or layout"
            continue
        # Literal executable addresses are still unresolved interfaces.
        literals = re.findall(r"\b0x[0-9a-fA-F]+\b", code)
        if any(0x400000 <= int(x, 16) < 0xa00000 or 0x2000000 <= int(x, 16) < 0x2800000 for x in literals):
            rejected[name] = "absolute address or ambiguous large constant"
            continue
        if len(body) > args.max_characters:
            rejected[name] = "larger function awaiting focused reconstruction"
            continue
        if state_proof:
            body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
                '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
                input=body.encode(), capture_output=True, check=True).stdout.decode().replace('\r\n', '\n')
        if static:
            body = "static " + body
        signature = body[:body.index("{")].strip()
        metadata = functions.get(address, {})
        candidates[name] = dict(name=name, address=row["address"], source=row["source"],
                                line=metadata.get("line", ""), static=static,
                                body=body, prototype=signature + ";", raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),
                                recovered_literals=recovered_literals)
        if state_proof:
            from audit_source import function_body
            candidates[name].update(state_mask_evidence=state_proof,
                reconstruction='State masks named from verified per-unit DWARF enumerators and machine operands; numeric value and operators preserved.',
                reconstruction_body_sha256=hashlib.sha256(function_body(body,name).encode()).hexdigest())
        if storage_proof:
            candidates[name]['stack_storage_reconstruction'] = storage_proof
            candidates[name]['reconstruction'] = storage_proof['scope']
        if local:
            candidates[name]["local_type_declarations"] = local["declarations"]
            candidates[name]["local_enum_evidence"] = local["types"]
            candidates[name]["array_boundaries"] = local.get("array_boundaries", [])
        if "is_humiliated_for_ability(" in body:
            candidates[name]["interface_adaptations"] = ["Three-argument is_humiliated calls use is_humiliated_for_ability."]
        adaptations = candidates[name].setdefault("interface_adaptations", [])
        if "add_counters_from_source(" in body:
            adaptations.append("Six-argument add_counters calls retain source/recipient through add_counters_from_source.")
        if "ENGINE_DONE_LABEL" in body or "ENGINE_OK_LABEL" in body:
            adaptations.append("Localized show_deck label pointers use the legacy integer-address argument.")
        if "builtin_strncpy(" in raw:
            adaptations.append("Ghidra fixed-length builtin_strncpy uses the standard C strncpy interface.")
        if re.search(r"anon_union_4_2_[0-9a-f]+_for_card_instance_struct_26|\(int32_t\)\s*\w+->field_26", raw):
            adaptations.append("Four-byte info-slot union payloads copy through the scalar member into the corresponding target word.")
        if re.search(r"\bconst target_t\s*\*", body):
            adaptations.append("Borrowed target-list locals preserve the existing APIs' const qualifier.")
        if not adaptations:
            candidates[name].pop("interface_adaptations", None)

    initial = set(candidates)
    # References to any exported function require its implementation to be
    # present already or accepted in this same batch, including callbacks.
    exported_names = {r["name"] for r in csv.DictReader((exports / "export.tsv").open(), delimiter="\t")}
    dependency_names = exported_names | set(source_prototypes)
    known = definitions - static_names
    known -= {n for n, entries in source_prototypes.items() if all(p["static"] for p in entries)}
    dependencies = {name: (set(re.findall(r"\b[A-Za-z_]\w*\b", code_only(c["body"]))) & dependency_names) - {name} - bound_variables(c["body"])
                    for name, c in candidates.items()}
    scratch = analysis / "source-candidates.c"
    compiler_diagnostics = []
    iterations = 0
    existing_interfaces = {}
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
        # A verified private global belongs only to its original source unit.
        # Make it visible in the scratch check when all candidate users share
        # that owner, as they will in the complete recovered source file.
        manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
        globals_evidence = json.loads(manifest_path.read_text()).get('source_globals', [])
        for declaration in globals_evidence:
            if declaration.get('linkage') != 'static':
                continue
            users = [c for c in candidates.values() if re.search(
                r'\b' + re.escape(declaration['name']) + r'\b', code_only(c['body']))]
            if (users and all(c['source'] == declaration['source'] for c in users) and
                source_text[declaration['source']].count(declaration['declaration']) == 1):
                parts.append(declaration['declaration'] + '\n')
        watermark_users = [c for c in candidates.values() if re.search(r"\bwatermarks\b", code_only(c["body"]))]
        if watermark_users and all(c["source"] == "cards/unstable.c" for c in watermark_users):
            # This private cache was recovered with COFF/.bss and DWARF layout
            # evidence. Check consumers in the declaration context they have
            # in their complete source unit, without changing its linkage.
            owner = (root / "cards/unstable.c").read_text()
            declaration = "static const Watermark* watermarks = NULL;"
            if declaration in owner:
                parts.append(declaration + "\n")
        existing_interfaces = {}
        private_interfaces = {}
        for c in candidates.values():
            for name in set(re.findall(r"\b[A-Za-z_]\w*\b", code_only(c["body"]))) - set(candidates) - bound_variables(c["body"]):
                matches = [p for p in source_prototypes.get(name, [])
                           if not p["static"] or p["source"] == c["source"]]
                # Historical source units can contain duplicate public
                # definitions. Their identical declarations still establish
                # one interface; keep conflicting signatures ambiguous.
                if matches and all(not p['static'] for p in matches):
                    unique = {re.sub(r'\s+', '', p['prototype']):p for p in matches}
                    if len(unique) == 1:
                        matches = [next(iter(unique.values()))]
                if len(matches) == 1:
                    p = matches[0]
                    (private_interfaces if p["static"] else existing_interfaces)[name] = p["prototype"]
        for name, prototype in {**existing_interfaces, **private_interfaces}.items():
            parts.extend([f'#line 1 "existing/{name}"\n', prototype + "\n"])
        for name, c in candidates.items():
            parts.extend([f'#line 1 "prototype/{name}"\n', c["prototype"] + "\n"])
        for name, c in candidates.items():
            parts.extend([f'#line 1 "body/{name}"\n', c["body"] + "\n"])
        scratch.write_text("".join(parts), encoding="utf-8")
        command = [str(clang), "-target", "i686-pc-windows-msvc", "-std=gnu99", "-fsyntax-only",
                   "-ferror-limit=0", "-fno-color-diagnostics", "-Werror=implicit-function-declaration",
                   "-Werror=implicit-int", "-Werror=incompatible-pointer-types", "-Werror=return-type",
                   "-Werror=uninitialized", "-Werror=sometimes-uninitialized", "-Wno-ignored-attributes",
                   "-Werror=format",
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
    if not candidates:
        existing_interfaces = {}
    report = dict(exported_functions=len(exported_names), initially_reconstructable=len(initial),
                  accepted=len(candidates), compiler_passes=iterations,
                  maximum_function_characters=args.max_characters,
                  source_files=dict(collections.Counter(c["source"] for c in candidates.values())),
                  accepted_functions=[{k:v for k,v in c.items() if k not in ("body",)} for c in candidates.values()],
                  deferred=rejected)
    header_text = (root / "manalink.h").read_text()
    report["existing_source_interfaces"] = {n:p for n,p in existing_interfaces.items()
                                             if not re.search(r"\b" + re.escape(n) + r"\s*\(", header_text)}
    for name in candidates:
        report["deferred"].pop(name, None)
    (analysis / "promotion-report.json").write_text(json.dumps(report, indent=2))
    if args.apply:
        by_file = collections.defaultdict(list)
        for candidate in candidates.values():
            by_file[candidate["source"]].append(candidate)
        public = list(report["existing_source_interfaces"].values())
        referenced_from = collections.defaultdict(set)
        for name, c in candidates.items():
            for dependency in dependencies[name]:
                referenced_from[dependency].add(c["source"])
        for relative, entries in by_file.items():
            destination = root / relative
            content = destination.read_bytes() if destination.exists() else b'#include "manalink.h"\n'
            addition = "\n\n/* Recovered October 2022 implementations; see docs/recovery-progress.md. */\n"
            needed = set().union(*(dependencies[c["name"]] for c in candidates.values()))
            addition += "\n".join(c["prototype"] for c in entries if c["name"] in needed) + "\n\n"
            for c in sorted(entries, key=lambda c: int(c["line"] or 0)):
                addition += f"/* ManalinkEh 2022: 0x{c['address']}; original {relative}:{c['line']}. */\n"
                addition += c["body"] + "\n"
                if not c["static"] and (not c["name"].startswith("card_") or
                                        referenced_from[c["name"]] - {relative}):
                    public.append(c["prototype"])
            destination.write_bytes((content.rstrip() + addition.encode()).replace(b"\r\n", b"\n").replace(b"\n", b"\r\n").rstrip(b"\r\n") + b"\r\n")
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
        saved = dict(previous)
        saved.update(report)
        saved["accepted_functions"] = list(merged.values())
        saved["accepted"] = len(merged)
        saved["latest_batch_accepted"] = len(candidates)
        saved["source_files"] = dict(collections.Counter(f["source"] for f in merged.values()))
        for fragment in saved.get("source_fragments", []):
            saved["source_files"].setdefault(fragment["source"], 0)
        saved["card_handler_functions"] = sum(f["name"].startswith("card_") for f in merged.values())
        saved["helper_functions"] = len(merged) - saved["card_handler_functions"]
        saved["private_functions"] = sum(f["static"] for f in merged.values())
        saved["private_helpers"] = sum(f["static"] and not f["name"].startswith("card_") for f in merged.values())
        saved["shared_helper_interfaces"] = sum(not f["static"] and not f["name"].startswith("card_")
                                                 for f in merged.values())
        saved["existing_source_interfaces"] = {**previous.get("existing_source_interfaces", {}),
                                                **report["existing_source_interfaces"]}
        for f in merged.values():
            saved["deferred"].pop(f["name"], None)
        saved["deferred_reason_counts"] = dict(collections.Counter(reason.split(":")[0]
                                                                   for reason in saved["deferred"].values()))
        registrations = list(csv.DictReader((analysis / "2022-registrations.csv").open()))
        recovered_names = {f["name"] for f in merged.values()}
        saved["registered_handler_symbols"] = len({r["symbol"].lstrip("_") for r in registrations
                                                   if r["symbol"].lstrip("_") in recovered_names})
        saved["registered_handler_symbols_marked_coded"] = len({r["symbol"].lstrip("_") for r in registrations
                                                                if r["coded"] == "True" and
                                                                r["symbol"].lstrip("_") in recovered_names})
        saved["source_syntax_check"] = dict(status="Pending complete source-unit check after promotion.", tests_run=False)
        saved["tests"] = "Deferred by user request; no tests added or run."
        saved["native_build"] = "Not run; downstream integration and behavioral validation remain."
        manifest.write_text(json.dumps(saved, indent=2))
        if storage_evidence:
            for c in candidates.values():
                if c.get('stack_storage_reconstruction'):
                    storage_evidence.setdefault('functions', {})[c['address']] = dict(
                        name=c['name'], source=c['source'], **c['stack_storage_reconstruction'])
            storage_path.write_text(json.dumps(storage_evidence, indent=2))
    print(json.dumps({k:v for k,v in report.items() if k not in ("accepted_functions", "deferred")}, indent=2))


if __name__ == "__main__":
    main()
