"""Recover the fixed-address jump veneers and their card-data registrations.

Run inspect_release.py --dwarf first. Static dependency edges are best-effort:
direct calls/jumps and immediate function-address operands, not indirect calls.
Requires the inspector's dependencies plus Capstone.
"""

import argparse
import collections
import csv
import json
import re
import struct
import zipfile
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_GRP_CALL, CS_GRP_JUMP
from capstone.x86_const import X86_OP_IMM

from inspect_release import Image, csv_write, sha


def symbol_addresses(image):
    result = collections.defaultdict(set)
    for name, value, section, kind, storage in image.functions():
        address = image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[section - 1].VirtualAddress + value
        result[address].add(name)
    return {a: sorted(names) for a, names in result.items()}


def veneers(image):
    base = image.pe.OPTIONAL_HEADER.ImageBase
    # Validated against the entire baseline source table, below.
    start = base + 0x1006
    data = image.pe.get_data(0x1006)
    symbols = symbol_addresses(image)
    result = []
    offset = 0
    while offset + 5 <= len(data) and data[offset] == 0xe9:
        address = start + offset
        target = address + 5 + struct.unpack_from("<i", data, offset + 1)[0]
        names = symbols.get(target, [])
        if len(names) != 1:
            raise ValueError(f"Non-unique or missing function symbol at {target:#x}: {names}")
        result.append(dict(slot=offset // 5, veneer=hex(address), target=hex(target), symbol=names[0]))
        offset += 5
    return result


def registrations(exe, table):
    # Packed Manalink card_data_t: size 72, ID at +0x24, code pointer +0x34.
    # .cards begins with 16 asterisks; capacity is MAXCARDS=30000. The section
    # also has a trailer beyond those records, which must not be read as cards.
    payload = exe.section_data(exe.sections[".cards"])
    assert payload[:16] == b"*" * 16
    assert len(payload) >= 16 + 30000 * 72
    entries = {int(v["veneer"], 16): v for v in table}
    result = []
    for iid in range(30000):
        row = payload[16 + iid * 72:16 + (iid + 1) * 72]
        if row == bytes(72):
            continue
        csvid = struct.unpack_from("<H", row, 0x24)[0]
        code = struct.unpack_from("<I", row, 0x34)[0]
        veneer = entries.get(code)
        result.append(dict(iid=iid, csvid=csvid,
                           name=row[1:19].split(b"\0", 1)[0].decode("cp1252"),
                           code_pointer=hex(code),
                           slot=veneer["slot"] if veneer else "",
                           target=veneer["target"] if veneer else "",
                           symbol=veneer["symbol"] if veneer else "",
                           dummy=csvid == 0xffff))
    return result


def card_database(data):
    count, text_size = struct.unpack_from("<II", data)
    text_start = 8 + count * 152
    flags_offset = text_start + text_size
    flags_size = struct.unpack_from("<I", data, flags_offset)[0]
    assert flags_size == (count + 7) // 8
    assert len(data) == flags_offset + 4 + flags_size + count * 20
    result = []
    for csvid in range(count):
        record = 8 + csvid * 152
        assert struct.unpack_from("<I", data, record)[0] == csvid
        name_offset = struct.unpack_from("<I", data, record + 4)[0]
        assert name_offset < text_size
        start = text_start + name_offset
        name = data[start:data.index(b"\0", start)].decode("cp1252")
        coded = bool(data[flags_offset + 4 + csvid // 8] & (1 << (csvid % 8)))
        result.append(dict(full_name=name, coded=coded))
    return result


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument("--output", type=Path, default=Path(".recovery"))
    parser.add_argument("--library", type=Path, default=Path("G:/Manalink"))
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    output = args.output
    dlls = {label: Image((output / (label + "-ManalinkEh.dll")).read_bytes())
            for label in ("2020", "2022")}
    exes = {"2020": Image((args.library / "test build 12-1-2020/Magic.exe").read_bytes()),
            "2022": Image((output / "2022-Magic.exe").read_bytes())}
    tables = {label: veneers(image) for label, image in dlls.items()}
    assembly = (root / "ManalinkEh.asm").read_text()
    original = re.findall(r"jmp near\s+(\S+)\s*;\s*([a-fA-F0-9]+)", assembly)
    assert [(v["symbol"], int(v["veneer"], 16)) for v in tables["2020"]] == [
        (name.rstrip(";"), int(address, 16)) for name, address in original]

    for label, table in tables.items():
        csv_write(output / (label + "-veneers.csv"), table, ["slot", "veneer", "target", "symbol"])
    baseline_slots = {v["slot"]: v for v in tables["2020"]}
    changed_slots = [dict(slot=v["slot"], veneer=v["veneer"],
                          old=baseline_slots[v["slot"]]["symbol"], new=v["symbol"])
                     for v in tables["2022"] if v["slot"] in baseline_slots
                     and baseline_slots[v["slot"]]["symbol"] != v["symbol"]]
    csv_write(output / "retargeted-veneers.csv", changed_slots, ["slot", "veneer", "old", "new"])
    lines = ["; Recovered from the verified October 2022 DLL. Analysis artifact.", "section .text", ""]
    lines += ["extern " + name for name in sorted({v["symbol"] for v in tables["2022"]})]
    lines += ["", "global _DllEntry", "_DllEntry:", "  mov eax, 1", "  ret", "",
              "global _CodeSectionEx", "_CodeSectionEx:"]
    assert dlls["2022"].pe.get_data(0x1000, 6) == bytes.fromhex("b801000000c3")
    lines += [f"  jmp near {v['symbol']} ; {int(v['veneer'], 16):x}" for v in tables["2022"]]
    lines += ["  align 32", ""]
    (output / "2022-ManalinkEh.recovered.asm").write_text("\n".join(lines))

    cards = {label: registrations(exes[label], tables[label]) for label in dlls}
    with zipfile.ZipFile(args.library / "Manalink-3_0-Oct-2022_Hotfix.zip") as archive:
        database = {"2020": card_database((args.library / "test build 12-1-2020/Cards.dat").read_bytes()),
                    "2022": card_database(archive.read("Cards.dat"))}
    old_names = {s[0] for s in dlls["2020"].functions()}
    old_records = collections.defaultdict(set)
    for card in cards["2020"]:
        old_records[card["csvid"]].add(card["symbol"] or card["code_pointer"])
    for label, records in cards.items():
        for c in records:
            c["id_absent_in_baseline"] = c["csvid"] >= len(database["2020"]) and not c["dummy"]
            c["handler_absent_in_baseline"] = bool(c["symbol"] and c["symbol"] not in old_names)
            c["baseline_handlers"] = ";".join(sorted(old_records[c["csvid"]]))
            if not c["dummy"]:
                c.update(database[label][c["csvid"]])
            else:
                c.update(full_name="", coded=False)
        csv_write(output / (label + "-registrations.csv"), records, list(records[0]))

    new_roots = {c["symbol"] for c in cards["2022"] if c["handler_absent_in_baseline"]}
    roots_by_name = {v["symbol"]: int(v["target"], 16) for v in tables["2022"]}
    ranges = {}
    with (output / "2022-functions.csv").open(newline="") as stream:
        for f in csv.DictReader(stream):
            if f["low_pc"] and f["high_pc"]:
                low, high = int(f["low_pc"], 16), int(f["high_pc"], 16)
                if low > 0 and high > low:
                    ranges[low] = (high, f["source"])
    symbols = symbol_addresses(dlls["2022"])
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.detail = True
    edges, missing = [], []
    for name in sorted(new_roots):
        address = roots_by_name[name]
        if address not in ranges:
            missing.append(name)
            continue
        end, source = ranges[address]
        code = dlls["2022"].pe.get_data(address - 0x2000000, end - address)
        for instruction in decoder.disasm(code, address):
            for operand in instruction.operands:
                if operand.type != X86_OP_IMM or operand.imm not in symbols:
                    continue
                kind = ("direct_call" if instruction.group(CS_GRP_CALL) else
                        "direct_jump" if instruction.group(CS_GRP_JUMP) else "function_address_candidate")
                for callee in symbols[operand.imm]:
                    edges.append(dict(caller=name, source=source, instruction=hex(instruction.address),
                                      callee=callee, target=hex(operand.imm), kind=kind,
                                      callee_absent_in_baseline=callee not in old_names))
    csv_write(output / "new-handler-dependencies.csv", edges,
              ["caller", "source", "instruction", "callee", "target", "kind", "callee_absent_in_baseline"])
    users = collections.defaultdict(set)
    for edge in edges:
        if (edge["callee_absent_in_baseline"] and not edge["callee"].startswith("_card_")
                and edge["kind"] == "direct_call"):
            users[edge["callee"]].add(edge["caller"])
    priorities = [dict(helper=name, new_handler_callers=len(callers), examples=";".join(sorted(callers)[:5]))
                  for name, callers in sorted(users.items(), key=lambda x: (-len(x[1]), x[0]))]
    csv_write(output / "new-helper-priorities.csv", priorities, ["helper", "new_handler_callers", "examples"])
    result = dict(analysis="Static registration inventory, not a playability or correctness claim",
                  input_hashes={label: dict(dll=sha(dlls[label].data), exe=sha(exes[label].data)) for label in dlls},
                  baseline_source_table_exact_match=True,
                  veneer_entries={label: len(table) for label, table in tables.items()},
                  retargeted_existing_slots=len(changed_slots),
                  appended_slots=len(tables["2022"]) - len(tables["2020"]),
                  populated_card_records={label: len(rows) for label, rows in cards.items()},
                  target_records_using_veneers=sum(bool(c["symbol"]) for c in cards["2022"]),
                  target_new_id_records_using_veneers=sum(c["id_absent_in_baseline"] and bool(c["symbol"]) for c in cards["2022"]),
                  target_new_id_coded_records_using_veneers=sum(c["id_absent_in_baseline"] and c["coded"] and bool(c["symbol"]) for c in cards["2022"]),
                  new_registered_handler_symbols=len(new_roots),
                  new_registered_handler_symbols_marked_coded=len({c["symbol"] for c in cards["2022"] if c["handler_absent_in_baseline"] and c["coded"]}),
                  card_database_entries={label: len(rows) for label, rows in database.items()},
                  new_registered_handlers_with_contiguous_dwarf_ranges=len(new_roots)-len(missing),
                  roots_without_contiguous_range=missing,
                  new_noncard_helpers_directly_called=len(priorities),
                  top_new_helper_priorities=priorities[:20],
                  examples=[c for c in cards["2022"] if c["name"] == "Professor Onyx"])
    (output / "registration-summary.json").write_text(json.dumps(result, indent=2))
    print(json.dumps(result, indent=2))


if __name__ == "__main__":
    main()
