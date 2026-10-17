"""Export absolute EXE interfaces and entry-register ABI evidence from DWARF."""
import csv
import functools
import json
import re
import struct
from pathlib import Path

from elftools.dwarf.locationlists import LocationParser, LocationExpr, BaseAddressEntry
from inspect_release import Image, value, inherited_value


def type_of(die):
    if "DW_AT_type" in die.attributes:
        return die.get_DIE_from_attribute("DW_AT_type")
    for key in ("DW_AT_abstract_origin", "DW_AT_specification"):
        if key in die.attributes:
            return type_of(die.get_DIE_from_attribute(key))
    return None


@functools.lru_cache(maxsize=None)
def type_name(die):
    if die is None:
        return "void"
    name = value(die, "DW_AT_name")
    if name:
        return name
    if die.tag == "DW_TAG_pointer_type":
        return type_name(type_of(die)) + " *"
    if die.tag in ("DW_TAG_const_type", "DW_TAG_volatile_type", "DW_TAG_restrict_type"):
        return type_name(type_of(die))
    if die.tag == "DW_TAG_array_type":
        dims = []
        for child in die.iter_children():
            if child.tag == "DW_TAG_subrange_type":
                count = value(child, "DW_AT_count")
                if count is None:
                    upper = value(child, "DW_AT_upper_bound")
                    count = upper + 1 - value(child, "DW_AT_lower_bound", 0) if isinstance(upper, int) else 1
                dims.append(f"[{count}]")
        return type_name(type_of(die)) + "".join(dims)
    if die.tag == "DW_TAG_enumeration_type":
        return "int"
    return "UNRESOLVED"


def main():
    root = Path(__file__).resolve().parents[2]
    output = root / ".recovery"
    image = Image(Path("G:/Manalink/ManalinkEh_10_22_Hotfix.dbg").read_bytes())
    absolute = {name: address for name, address, section, kind, storage in image.symbols if section == -1 and address >= 0x400000}
    dwarf = image.dwarf()
    parser = LocationParser(dwarf.location_lists())
    interfaces, conventions, unprototyped = {}, {}, {}
    for cu in dwarf.iter_CUs():
        top = cu.get_top_DIE()
        if not value(top, "DW_AT_name", "").startswith(("cards/", "functions/")):
            continue
        cu_base = value(top, "DW_AT_low_pc", 0)
        for die in top.iter_children():
            name = inherited_value(die, "DW_AT_name", "")
            if die.tag in ("DW_TAG_variable", "DW_TAG_subprogram") and "_" + name in absolute and name not in interfaces:
                if die.tag == "DW_TAG_variable":
                    typ = type_name(type_of(die))
                    if name == "cards_data":
                        typ = "card_data_t[30000]"
                    row = [hex(absolute["_" + name]), name, "global", typ]
                else:
                    params = [type_name(type_of(c)) for c in die.iter_children() if c.tag == "DW_TAG_formal_parameter"]
                    row = [hex(absolute["_" + name]), name, "function", type_name(type_of(die)), *params]
                    if not params and type_of(die) is None and not value(die, "DW_AT_prototyped", False):
                        # Name-only absolute DIEs provide neither a return type
                        # nor an argument list. Do not manufacture void(void).
                        unprototyped[name] = row
                        continue
                if not any("UNRESOLVED" in field or "::" in field for field in row):
                    interfaces[name] = row
            low = value(die, "DW_AT_low_pc")
            if die.tag != "DW_TAG_subprogram" or not low:
                continue
            expressions = []
            for child in die.iter_children():
                if child.tag != "DW_TAG_formal_parameter":
                    continue
                attr = child.attributes.get("DW_AT_location")
                expression = None
                if attr:
                    loc = parser.parse_from_attribute(attr, cu["version"], child)
                    if isinstance(loc, LocationExpr):
                        expression = loc.loc_expr
                    else:
                        base = cu_base
                        for entry in loc:
                            if isinstance(entry, BaseAddressEntry):
                                base = entry.base_address
                            else:
                                begin = entry.begin_offset if entry.is_absolute else base + entry.begin_offset
                                end = entry.end_offset if entry.is_absolute else base + entry.end_offset
                                if begin <= low < end:
                                    expression = entry.loc_expr
                                    break
                expressions.append(expression)
            count = 0
            for actual, expected in zip(expressions, ([0x50], [0x52], [0x51])):
                if actual != expected:
                    break
                count += 1
            if count:
                conventions[low] = [hex(low), name, "__regparm" + str(count)]
        type_name.cache_clear()
        cu._dielist.clear()
        cu._diemap.clear()
    (output / "absolute-interfaces.tsv").write_text("".join("\t".join(row) + "\n" for row in interfaces.values()))
    (output / "unprototyped-absolute-interfaces.tsv").write_text(
        "".join("\t".join(row) + "\n" for name, row in unprototyped.items() if name not in interfaces))
    (output / "register-conventions.tsv").write_text("".join("\t".join(row) + "\n" for row in conventions.values()))
    magic = Image((output / "2022-Magic.exe").read_bytes())
    segments = []
    for name, section in magic.sections.items():
        payload = magic.section_data(section)
        path = output / ("magic-" + name.lstrip(".") + ".bin")
        path.write_bytes(payload + bytes(max(0, section.Misc_VirtualSize - len(payload))))
        segments.append([name, hex(magic.pe.OPTIONAL_HEADER.ImageBase + section.VirtualAddress),
                         str(section.Characteristics), str(path)])
    (output / "magic-sections.tsv").write_text("".join("\t".join(row) + "\n" for row in segments))
    # Keep batch selection reproducible. This is symbol membership selection,
    # not a binary or behavioral diff. Canonical entry checks happen at promotion.
    old_names = {row["name"] for row in csv.DictReader((output / "coff-function-names.csv").open())
                 if row["status"] != "added"}
    requests = []
    for row in csv.DictReader((output / "2022-functions.csv").open()):
        if not row["low_pc"] or not row["high_pc"] or not row["name"]:
            continue
        low, high = int(row["low_pc"], 16), int(row["high_pc"], 16)
        if low > 0 and high > low and "_" + row["name"] not in old_names and not row["name"].startswith(("_GLOBAL", "__", "std::")):
            requests.append((low, row["source"], row["name"], high))
    # A repeated private name may already exist in an older, different unit.
    # Keep focused dependency requests reproducible without selecting all old
    # functions or requiring a behavioral diff.
    evidence = root / "docs/recovery-evidence/interface-recovery.json"
    if evidence.exists():
        for row in json.loads(evidence.read_text()).get("supplemental_exports", []):
            requests.append((int(row["address"], 16), row["source"], row["name"], int(row["end"], 16)))
    first = {"magecraft_trigger", "sp_ability_trigger", "sp_ability_trigger_impl", "dat_ability"}
    requests.sort(key=lambda f: (0 if f[2] in first else 1 if f[1] == "cards/strixhaven.c" else 2 if f[3]-f[0] <= 100 else 3, f[1], f[0]))
    seen = set()
    lines = []
    for low, source, name, high in requests:
        if low not in seen:
            seen.add(low)
            lines.append(f"{low:x}\t{source}\t{name}\t{high:x}\n")
    (output / "export-request.tsv").write_text("".join(lines))
    print("Absolute interfaces", len(interfaces), "register conventions", len(conventions))


if __name__ == "__main__":
    main()
