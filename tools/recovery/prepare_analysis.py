"""Rebuild an analysis-only PE with original runtime bytes and matching DWARF."""
import argparse
import json
import struct
from pathlib import Path

from inspect_release import Image, sha


def align(number, boundary=512):
    return (number + boundary - 1) // boundary * boundary


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--year', choices=['2020', '2022'], default='2022')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    output = root / ".recovery"
    dll = Image((output / f"{args.year}-ManalinkEh.dll").read_bytes())
    debug_path = (root / 'ManalinkEh.dbg' if args.year == '2020' else
                  Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg'))
    dbg = Image(debug_path.read_bytes())
    for name, section in dll.sections.items():
        other = dbg.sections[name]
        assert (section.VirtualAddress, section.Misc_VirtualSize) == (other.VirtualAddress, other.Misc_VirtualSize)
    section_end = dbg.pe.sections[-1].get_file_offset() + 40
    result = bytearray(dbg.data[:section_end])
    result.extend(bytes(align(len(result)) - len(result)))
    optional_offset = dbg.pe.OPTIONAL_HEADER.get_file_offset()
    struct.pack_into("<I", result, optional_offset + 60, len(result))
    # Use original DLL data-directory RVAs with the expanded debug image size.
    for index, directory in enumerate(dll.pe.OPTIONAL_HEADER.DATA_DIRECTORY):
        struct.pack_into("<II", result, optional_offset + 96 + index * 8,
                         directory.VirtualAddress, directory.Size)
    for name, section in dbg.sections.items():
        source = dll if name in dll.sections else dbg
        original = source.sections[name]
        payload = source.section_data(original)
        raw_size = align(len(payload)) if payload else 0
        offset = len(result) if payload else 0
        struct.pack_into("<II", result, section.get_file_offset() + 16, raw_size, offset)
        struct.pack_into("<I", result, section.get_file_offset() + 36, original.Characteristics)
        if payload:
            result.extend(payload)
            result.extend(bytes(raw_size - len(payload)))
    struct.pack_into("<I", result, dbg.pe.FILE_HEADER.get_file_offset() + 8, len(result))
    result.extend(dbg.data[dbg.pe.FILE_HEADER.PointerToSymbolTable:])
    built = Image(bytes(result))
    for name, section in built.sections.items():
        source = dll if name in dll.sections else dbg
        assert built.section_data(section) == source.section_data(source.sections[name]), name
    assert built.functions() == dbg.functions()
    path = output / f"{args.year}-ManalinkEh.analysis.dll"
    path.write_bytes(result)
    manifest_name = 'analysis-image.json' if args.year == '2022' else '2020-analysis-image.json'
    (output / manifest_name).write_text(json.dumps(dict(
        artifact=str(path), sha256=sha(result), runtime_sha256=sha(dll.data),
        debug_sha256=sha(dbg.data), all_section_payloads_verified=True,
        note="Analysis only. Never deploy this composite file."), indent=2))
    print(path)


if __name__ == "__main__":
    main()
