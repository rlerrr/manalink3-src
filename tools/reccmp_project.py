"""Set up and run the local DWARF-enabled reccmp fork without installing it.

Only reads the release/build binaries. Local paths and reports are ignored by Git.
"""

import argparse
from collections import Counter, defaultdict
import csv
import hashlib
import json
import os
from pathlib import Path
import struct
import sys
import zipfile

ROOT = Path(__file__).resolve().parents[1]
TARGET = "MANALINK2022"
ORIGINAL_SHA256 = "52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5"
ORIGINAL_DEBUG_SHA256 = "7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85"


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def write_json(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")


def read_functions(path, all_functions=False):
    """Read functions one CU at a time, avoiding retained DIE graphs.

    The rebuilt name inventory includes CRT and nested C++ methods too: those
    can collide with file-scope C helpers in reccmp's global name matcher.
    """
    sys.path.insert(0, str(ROOT / "tools" / "recovery"))
    from inspect_release import Image, inherited_value, value

    image = Image(path.read_bytes())
    dwarf = image.dwarf()
    debug_info = image.section_data(image.sections[".debug_info"])
    offset = 0
    rows = []
    while offset + 4 <= len(debug_info):
        length = struct.unpack_from("<I", debug_info, offset)[0]
        if length == 0:
            break
        if length == 0xFFFFFFFF:
            raise ValueError("DWARF64 units are not supported by this setup reader")
        cu = dwarf.get_CU_at(offset)
        offset += 4 + length
        source = value(cu.get_top_DIE(), "DW_AT_name", "").replace("\\", "/")
        if all_functions or source.startswith(("cards/", "functions/")):
            dies = cu.iter_DIEs() if all_functions else cu.get_top_DIE().iter_children()
            for die in dies:
                if die.tag != "DW_TAG_subprogram":
                    continue
                low = value(die, "DW_AT_low_pc")
                high = die.attributes.get("DW_AT_high_pc")
                name = inherited_value(die, "DW_AT_name")
                if not low or not high or not name or value(die, "DW_AT_declaration", False):
                    continue
                end = high.value if high.form == "DW_FORM_addr" else low + high.value
                if end > low:
                    rows.append(dict(address=low, size=end-low, name=name, source=source))
        cu._dielist.clear()
        cu._diemap.clear()
    # GCC can describe one body through several DIEs. It remains one function.
    return list({(r["address"], r["name"]): r for r in rows}.values())


def setup(args):
    from reccmp.project.config import ProjectFile, UserFile, BuildFile

    local = ROOT / ".recovery" / "reccmp"
    local.mkdir(parents=True, exist_ok=True)
    original = args.original.resolve()
    if not original.is_file():
        archive = args.library / "Manalink-3_0-Oct-2022_Hotfix.zip"
        # Deliberately use the archive root; ManaLink/ contains an obsolete DLL.
        with zipfile.ZipFile(archive) as release:
            binary = release.read("ManalinkEh.dll")
        if hashlib.sha256(binary).hexdigest() != ORIGINAL_SHA256:
            raise ValueError("Unexpected October 2022 Hotfix DLL hash")
        original.parent.mkdir(parents=True, exist_ok=True)
        original.write_bytes(binary)
    if digest(original) != ORIGINAL_SHA256:
        raise ValueError("Original DLL differs from the pinned October 2022 Hotfix")
    original_debug = args.original_debug.resolve()
    rebuilt = args.rebuilt.resolve()
    rebuilt_debug = args.rebuilt_debug.resolve()
    for path in (original_debug, rebuilt, rebuilt_debug):
        if not path.is_file():
            raise FileNotFoundError(path)
    if digest(original_debug) != ORIGINAL_DEBUG_SHA256:
        raise ValueError("Release debug sidecar differs from the pinned October 2022 Hotfix")
    # Check objcopy's CRC when available, rather than trusting only the filenames.
    import zlib
    # Long section names use the COFF string table, so resolve with our reader.
    from inspect_release import Image
    rebuilt_image = Image(rebuilt.read_bytes())
    debuglink = rebuilt_image.sections.get(".gnu_debuglink")
    if debuglink is not None:
        data = rebuilt_image.section_data(debuglink)
        crc_offset = (data.index(b"\0") + 4) & ~3
        crc = struct.unpack_from("<I", data, crc_offset)[0]
        if zlib.crc32(rebuilt_debug.read_bytes()) != crc:
            raise ValueError("Rebuilt DLL/debug sidecar CRC mismatch; rebuild both artifacts")

    print("Reading release DWARF function bounds...", flush=True)
    native = read_functions(original_debug)
    print("Reading rebuilt DWARF names...", flush=True)
    current = read_functions(rebuilt_debug, all_functions=True)
    manifest = json.loads((ROOT / "docs/recovery-evidence/recovered-functions.json").read_text())
    accepted = {int(row["address"], 16): row for row in manifest["accepted_functions"]}
    stubs = {int(row["native_address"], 16): row for row in manifest["source_stubs"]}
    current_names = defaultdict(set)
    for row in current:
        current_names[row["name"]].add(row["address"])

    # Use explicit recovery mappings first; retained 2020 helpers keep their names.
    for row in native:
        mapped = accepted.get(row["address"]) or stubs.get(row["address"])
        row["match_name"] = mapped["name"] if mapped else row["name"]
        row["mapping"] = "recovery ledger" if mapped else "same native name"
    native_names = defaultdict(set)
    for row in native:
        native_names[row["match_name"]].add(row["address"])

    included, excluded = [], []
    for row in native:
        name = row["match_name"]
        reason = None
        if len(native_names[name]) != 1:
            reason = "ambiguous original name"
        elif len(current_names[name]) > 1:
            reason = "ambiguous rebuilt name"
        elif not current_names[name]:
            reason = "no rebuilt function with this name"
        if reason:
            excluded.append(dict(row, reason=reason))
        else:
            included.append(row)

    # These are absolute VAs, not RVAs or veneer jump-table entries.
    metadata = ROOT / "config" / "reccmp" / "functions.csv"
    metadata.parent.mkdir(parents=True, exist_ok=True)
    with metadata.open("w", newline="", encoding="utf-8") as stream:
        writer = csv.writer(stream, lineterminator="\n")
        writer.writerow(["address", "type", "name", "size", "native_name", "source", "mapping"])
        for row in sorted(included, key=lambda r: r["address"]):
            writer.writerow([hex(row["address"]), "stub" if row["address"] in stubs else "function",
                             row["match_name"], row["size"], row["name"], row["source"], row["mapping"]])
    write_json(local / "excluded-functions.json", excluded)

    # JSON is a YAML subset; the public project uses the hand-maintained YAML file.
    write_json(ROOT / "reccmp-user.yml", {"targets": {TARGET: {"path": original.as_posix()}}})
    write_json(ROOT / "reccmp-build.yml", {"project": ".", "targets": {
        TARGET: {"path": rebuilt.as_posix(), "pdb": rebuilt_debug.as_posix()}}})
    # Validate using this fork's actual configuration schema.
    for model, filename in ((ProjectFile, "reccmp-project.yml"), (UserFile, "reccmp-user.yml"),
                            (BuildFile, "reccmp-build.yml")):
        model.from_file(ROOT / filename)
    summary = dict(target=TARGET, original_functions=len(native), csv_functions=len(included),
                   ledger_mappings=sum(r["mapping"] == "recovery ledger" for r in included),
                   stubs=sum(r["address"] in stubs for r in included),
                   excluded_reasons=dict(Counter(r["reason"] for r in excluded)),
                   inputs={str(p): digest(p) for p in (original, original_debug, rebuilt, rebuilt_debug)},
                   metadata_sha256=digest(metadata), reccmp_root=str(args.reccmp_root.resolve()),
                   scope="File-scope card/helper functions with contiguous DWARF bounds and unique names; no globals or GUI recovery.")
    write_json(local / "setup.json", summary)
    print(json.dumps({k: v for k, v in summary.items() if k != "inputs"}, indent=2))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--reccmp-root", type=Path, default=Path("C:/dev/reccmp"))
    subparsers = parser.add_subparsers(dest="command", required=True)
    init = subparsers.add_parser("setup", help="Generate function CSV and local path configuration")
    init.add_argument("--library", type=Path, default=Path("G:/Manalink"))
    init.add_argument("--original", type=Path, default=ROOT / ".recovery/2022-ManalinkEh.dll")
    init.add_argument("--original-debug", type=Path, default=Path("G:/Manalink/ManalinkEh_10_22_Hotfix.dbg"))
    init.add_argument("--rebuilt", type=Path, default=ROOT / "ManalinkEh.dll")
    init.add_argument("--rebuilt-debug", type=Path, default=ROOT / "ManalinkEh.dbg")
    subparsers.add_parser("compare", help="Forward remaining options to reccmp-reccmp")
    args, remaining = parser.parse_known_args()
    if args.command == "setup" and remaining:
        parser.error("Unrecognized setup options: " + " ".join(remaining))
    fork = args.reccmp_root.resolve()
    if not (fork / "reccmp/dwarf/analysis.py").is_file():
        parser.error(f"DWARF-enabled reccmp checkout not found: {fork}")
    sys.path.insert(0, str(fork))
    os.chdir(ROOT)
    if args.command == "setup":
        sys.path.insert(0, str(ROOT / "tools" / "recovery"))
        setup(args)
        return 0
    summary_path = ROOT / ".recovery/reccmp/setup.json"
    if not summary_path.is_file():
        parser.error("Run tools/reccmp_project.py setup first")
    summary = json.loads(summary_path.read_text())
    for filename, expected in summary["inputs"].items():
        if digest(Path(filename)) != expected:
            parser.error(f"Input changed since setup: {filename}; rerun setup")
    if digest(ROOT / "config/reccmp/functions.csv") != summary["metadata_sha256"]:
        parser.error("Function metadata changed since setup; rerun setup")
    from reccmp.tools.asmcmp import main as compare_main
    # Upstream prints Unicode status markers even with --no-color. Windows
    # redirected stdout otherwise defaults to cp1252 and fails on exact matches.
    for stream in (sys.stdout, sys.stderr):
        if hasattr(stream, "reconfigure"):
            stream.reconfigure(encoding="utf-8")
    sys.argv = ["reccmp-reccmp", "--target", TARGET, "--no-color", *remaining]
    return compare_main()


if __name__ == "__main__":
    raise SystemExit(main())
