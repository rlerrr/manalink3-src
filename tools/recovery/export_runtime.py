"""Resolve standard CRT thunk signatures from COFF plus the PE import table."""
import json
import struct
from pathlib import Path

import pefile
from inspect_release import Image


def main():
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    image = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    image.pe.parse_data_directories(directories=[pefile.DIRECTORY_ENTRY['IMAGE_DIRECTORY_ENTRY_IMPORT']])
    imports = {item.address: (entry.dll.decode(), item.name.decode())
               for entry in image.pe.DIRECTORY_ENTRY_IMPORT for item in entry.imports if item.name}
    signatures = {
        'memcpy': ['void *', 'void *', 'void *', 'unsigned int'],
        'memmove': ['void *', 'void *', 'void *', 'unsigned int'],
        'memset': ['void *', 'void *', 'int', 'unsigned int'],
        'memcmp': ['int', 'void *', 'void *', 'unsigned int'],
        'strlen': ['unsigned int', 'char *'],
        'strcmp': ['int', 'char *', 'char *'],
        'strncmp': ['int', 'char *', 'char *', 'unsigned int'],
        'strcpy': ['char *', 'char *', 'char *'],
        'strncpy': ['char *', 'char *', 'char *', 'unsigned int'],
        'strcat': ['char *', 'char *', 'char *'],
        'strchr': ['char *', 'char *', 'int'],
        'strstr': ['char *', 'char *', 'char *'],
        'malloc': ['void *', 'unsigned int'],
        'calloc': ['void *', 'unsigned int', 'unsigned int'],
        'realloc': ['void *', 'void *', 'unsigned int'],
        'free': ['void', 'void *'],
    }
    rows, evidence = [], []
    for symbol, offset, section, kind, storage in sorted(image.functions()):
        name = symbol.removeprefix('_')
        if name not in signatures:
            continue
        address = image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[section - 1].VirtualAddress + offset
        raw = image.pe.get_data(address - image.pe.OPTIONAL_HEADER.ImageBase, 6)
        if raw[:2] != b'\xff\x25':
            continue
        slot = struct.unpack('<I', raw[2:])[0]
        library, imported_name = imports[slot]
        if library.lower() != 'msvcrt.dll' or name != imported_name:
            raise ValueError((symbol, library, imported_name))
        rows.append([hex(address), name, *signatures[name]])
        evidence.append(dict(name=name, address=hex(address), import_slot=hex(slot), library=library,
                             signature=signatures[name], thunk_bytes=raw.hex()))
    (cache / 'runtime-interfaces.tsv').write_text(''.join('\t'.join(row) + '\n' for row in rows))
    (cache / 'runtime-interfaces.json').write_text(json.dumps(evidence, indent=2))
    print(f'Verified {len(rows)} CRT import thunks.')


if __name__ == '__main__':
    main()
