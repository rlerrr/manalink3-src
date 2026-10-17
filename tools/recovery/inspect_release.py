"""Read local Manalink PE/DWARF artifacts; never execute or change the inputs.

Requires pefile and pyelftools. Output is evidence for recovery planning, not
reconstructed source. Run from the repository root; see docs/recovery-plan.md.
"""

import argparse
import collections
import csv
import hashlib
import inspect
import io
import json
import struct
import zipfile
from pathlib import Path

import pefile
from elftools.dwarf.dwarfinfo import DWARFInfo, DwarfConfig, DebugSectionDescriptor


def sha(data):
    return hashlib.sha256(data).hexdigest()


class Image:
    def __init__(self, data):
        self.data = data
        self.pe = pefile.PE(data=data, fast_load=True)
        header = self.pe.FILE_HEADER
        offset = header.PointerToSymbolTable
        strings = data[offset + 18 * header.NumberOfSymbols:]

        def long_name(index):
            return strings[index:strings.index(b"\0", index)].decode(errors="replace")

        self.sections = {}
        for section in self.pe.sections:
            name = section.Name.rstrip(b"\0").decode()
            if name.startswith("/"):
                name = long_name(int(name[1:]))
            self.sections[name] = section
        self.symbols = []
        i = 0
        while i < header.NumberOfSymbols:
            record = data[offset + i * 18:offset + (i + 1) * 18]
            name = (long_name(struct.unpack("<I", record[4:8])[0])
                    if record[:4] == b"\0" * 4
                    else record[:8].rstrip(b"\0").decode(errors="replace"))
            value, section, kind, storage, auxiliary = struct.unpack("<IhHBB", record[8:])
            self.symbols.append((name, value, section, kind, storage))
            i += 1 + auxiliary

    def section_data(self, section):
        # pefile get_data() uses PE alignment rules. These separate-debug PE
        # files have deliberately unaligned raw offsets; read their offsets
        # literally, otherwise the DWARF header is silently misread.
        offset = section.PointerToRawData
        return self.data[offset:offset + min(section.SizeOfRawData, section.Misc_VirtualSize)]

    def functions(self):
        return {s for s in self.symbols if s[2] > 0 and s[3] == 0x20}

    def summary(self):
        return dict(sha256=sha(self.data), bytes=len(self.data),
                    machine=hex(self.pe.FILE_HEADER.Machine),
                    timestamp=self.pe.FILE_HEADER.TimeDateStamp,
                    image_base=hex(self.pe.OPTIONAL_HEADER.ImageBase),
                    entry_rva=hex(self.pe.OPTIONAL_HEADER.AddressOfEntryPoint),
                    coff_records_including_aux=self.pe.FILE_HEADER.NumberOfSymbols,
                    coff_function_records=len(self.functions()),
                    sections={n: dict(rva=hex(s.VirtualAddress),
                                      virtual_size=s.Misc_VirtualSize,
                                      raw_size=s.SizeOfRawData)
                              for n, s in self.sections.items()})

    def dwarf(self):
        args = {key: None for key in inspect.signature(DWARFInfo).parameters}
        args["config"] = DwarfConfig(True, "x86", 4)
        for name, section in self.sections.items():
            key = name.lstrip(".") + "_sec"
            if name.startswith(".debug_") and key in args:
                data = self.section_data(section)
                args[key] = DebugSectionDescriptor(io.BytesIO(data), name,
                                                   section.PointerToRawData,
                                                   len(data), section.VirtualAddress)
        return DWARFInfo(**args)


def value(die, name, default=None):
    attr = die.attributes.get(name)
    if not attr:
        return default
    return attr.value.decode(errors="replace") if isinstance(attr.value, bytes) else attr.value


def inherited_value(die, name, default=None):
    """GCC may put a concrete function's name on its abstract origin."""
    if name in die.attributes:
        return value(die, name, default)
    for reference in ("DW_AT_abstract_origin", "DW_AT_specification"):
        if reference in die.attributes:
            return inherited_value(die.get_DIE_from_attribute(reference), name, default)
    return default


def csv_write(path, rows, fields):
    with path.open("w", newline="", encoding="utf-8") as stream:
        writer = csv.DictWriter(stream, fieldnames=fields)
        writer.writeheader()
        writer.writerows(rows)


def census(image, output, label):
    dwarf = image.dwarf()
    units, functions, samples, examples = [], [], [], {}
    for cu in dwarf.iter_CUs():
        top = cu.get_top_DIE()
        source = value(top, "DW_AT_name", "")
        unit = dict(source=source, directory=value(top, "DW_AT_comp_dir"),
                    producer=value(top, "DW_AT_producer"), version=cu["version"])
        units.append(unit)
        if not source.startswith(("cards/", "functions/")):
            continue
        # Direct children include file-scope functions and types. Do not call
        # these numbers a census of inline instances or nested DIEs.
        for die in top.iter_children():
            if die.tag == "DW_TAG_subprogram":
                low = value(die, "DW_AT_low_pc")
                high_attr = die.attributes.get("DW_AT_high_pc")
                high = high_attr.value if high_attr else None
                if high is not None and high_attr.form != "DW_FORM_addr" and low is not None:
                    high += low
                functions.append(dict(source=source, name=inherited_value(die, "DW_AT_name"),
                                      low_pc=hex(low) if low is not None else None,
                                      high_pc=hex(high) if high is not None else None,
                                      line=inherited_value(die, "DW_AT_decl_line"),
                                      declaration=value(die, "DW_AT_declaration", False),
                                      ranges=value(die, "DW_AT_ranges"),
                                      die_offset=hex(die.offset)))
                if low and value(die, "DW_AT_name") in ("card_winter_orb", "dispatch_trigger", "card_black_lotus", "card_professor_onyx"):
                    samples.append(dict(source=source, tag=die.tag, name=value(die, "DW_AT_name"),
                                        attributes={k: str(v.value) for k, v in die.attributes.items()},
                                        children=[dict(tag=c.tag, name=value(c, "DW_AT_name"),
                                                       attributes={k: str(v.value) for k, v in c.attributes.items()})
                                                  for c in die.iter_children()]))
                    line_program = dwarf.line_program_for_CU(cu)
                    lines = [dict(address=hex(e.state.address), line=e.state.line)
                             for e in line_program.get_entries()
                             if e.state and low <= e.state.address < high]
                    examples[value(die, "DW_AT_name")] = dict(source=source, address=hex(low),
                        size=high-low, line=value(die, "DW_AT_decl_line"),
                        parameters=[value(c, "DW_AT_name") for c in die.iter_children()
                                    if c.tag == "DW_TAG_formal_parameter"],
                        locals=[value(c, "DW_AT_name") for c in die.iter_children()
                                if c.tag == "DW_TAG_variable"],
                        line_table_rows=len(lines), line_examples=lines[:5])
            elif (die.tag == "DW_TAG_typedef" and value(die, "DW_AT_name") in
                  ("card_instance_t", "event_t", "target_t", "card_id_t") and
                  value(die, "DW_AT_name") not in examples):
                target = die.get_DIE_from_attribute("DW_AT_type")
                examples[value(die, "DW_AT_name")] = dict(source=source, tag=target.tag,
                    name=value(target, "DW_AT_name"), size=value(target, "DW_AT_byte_size"),
                    members=[dict(name=value(c, "DW_AT_name"),
                                  offset=value(c, "DW_AT_data_member_location"),
                                  value=value(c, "DW_AT_const_value")) for c in target.iter_children()])
        print(f"{label}: {source}", flush=True)
        # Avoid retaining parsed DIE graphs across large, independent CUs.
        cu._dielist.clear()
        cu._diemap.clear()
    csv_write(output / f"{label}-units.csv", units, ["source", "directory", "producer", "version"])
    csv_write(output / f"{label}-functions.csv", functions,
              ["source", "name", "low_pc", "high_pc", "line", "declaration", "ranges", "die_offset"])
    (output / f"{label}-samples.json").write_text(json.dumps(samples, indent=2), encoding="utf-8")
    (output / f"{label}-metadata-examples.json").write_text(json.dumps(examples, indent=2), encoding="utf-8")
    project_units = [u for u in units if u["source"].startswith(("cards/", "functions/"))]
    return dict(total_units=len(units), project_units=len(project_units),
                sources=[u["source"] for u in project_units],
                producers=dict(collections.Counter(u["producer"] for u in project_units)),
                file_scope_subprogram_dies=len(functions),
                file_scope_subprograms_with_low_pc=sum(f["low_pc"] is not None for f in functions),
                file_scope_subprograms_with_ranges=sum(f["ranges"] is not None for f in functions))


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument("--library", type=Path, default=Path("G:/Manalink"))
    parser.add_argument("--output", type=Path, default=Path(".recovery"))
    parser.add_argument("--dwarf", action="store_true", help="Also inspect project CU functions/types")
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    (args.output / ".gitignore").write_text("*\n", encoding="utf-8")
    repo = Path(__file__).resolve().parents[2]
    archive_path = args.library / "Manalink-3_0-Oct-2022_Hotfix.zip"
    with zipfile.ZipFile(archive_path) as archive:
        # The archive also has an obsolete ManaLink/ManalinkEh.dll. Select root.
        latest = Image(archive.read("ManalinkEh.dll"))
        for name in ("ManalinkEh.dll", "Magic.exe", "Drawcardlib.dll", "CardArtLib.dll", "DeckDll.dll"):
            (args.output / ("2022-" + name)).write_bytes(archive.read(name))
    baseline_path = args.library / "test build 12-1-2020" / "ManalinkEh.dll"
    old = Image(baseline_path.read_bytes())
    debug_path = args.library / "ManalinkEh_10_22_Hotfix.dbg"
    debug = Image(debug_path.read_bytes())
    old_debug = Image((repo / "ManalinkEh.dbg").read_bytes())
    (args.output / "2020-ManalinkEh.dll").write_bytes(old.data)
    images = {"2020-dll": old, "2020-dbg": old_debug, "2022-dll": latest, "2022-dbg": debug}
    result = {"inputs": dict(library=str(args.library), archive=str(archive_path),
                             archive_sha256=sha(archive_path.read_bytes()),
                             baseline=str(baseline_path), debug=str(debug_path)),
              "images": {key: image.summary() for key, image in images.items()}}
    pairs = {}
    for label, dll, dbg in (("2020", old, old_debug), ("2022", latest, debug)):
        runtime = {n: s for n, s in dbg.sections.items() if not n.startswith(".debug_")}
        pairs[label] = dict(runtime_sections=len(runtime),
                            all_layouts_equal=all(n in dll.sections and
                                (s.VirtualAddress, s.Misc_VirtualSize) ==
                                (dll.sections[n].VirtualAddress, dll.sections[n].Misc_VirtualSize)
                                for n, s in runtime.items()),
                            function_records_equal=dll.functions() == dbg.functions(),
                            symmetric_function_record_difference=len(dll.functions() ^ dbg.functions()))
    result["pair_checks"] = pairs
    old_names = {s[0] for s in old.functions()}
    new_names = {s[0] for s in latest.functions()}
    result["coff_name_comparison"] = dict(old=len(old_names), new=len(new_names),
        shared=len(old_names & new_names), added=len(new_names - old_names), removed=len(old_names - new_names),
        old_card_names=sum(n.startswith("_card_") for n in old_names),
        new_card_names=sum(n.startswith("_card_") for n in new_names))
    csv_write(args.output / "coff-function-names.csv",
              [dict(name=n, status=("shared" if n in old_names & new_names else
                                    "added" if n in new_names else "removed"))
               for n in sorted(old_names | new_names)], ["name", "status"])
    if args.dwarf:
        result["dwarf"] = {label: census(images[label + "-dbg"], args.output, label)
                           for label in ("2020", "2022")}
        result["dwarf"]["added_sources"] = sorted(set(result["dwarf"]["2022"]["sources"]) -
                                                  set(result["dwarf"]["2020"]["sources"]))
    (args.output / "inspection.json").write_text(json.dumps(result, indent=2), encoding="utf-8")
    print(json.dumps({"output": str(args.output), "pair_checks": pairs,
                      "coff_name_comparison": result["coff_name_comparison"]}, indent=2))


if __name__ == "__main__":
    main()
