"""Conservative project-data identities for the local DWARF reccmp adapter.

The fork currently emits function nodes only. Supply checked data nodes through
its existing CvdumpNode interface; leave its matching algorithms unchanged.
"""
import argparse
from collections import Counter, defaultdict
import hashlib
import inspect
import json
from pathlib import Path
import struct

from inspect_release import Image, inherited_value, value
from global_initializers import inherited_type
from helper_contracts import supported, type_contract
from object_initializers import sizeof

ROOT = Path(__file__).resolve().parents[2]


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def scopes(die):
    result = []
    parent = die.get_parent()
    while parent is not None and parent.tag != 'DW_TAG_compile_unit':
        name = inherited_value(parent, 'DW_AT_name')
        if name:
            result.append(name)
        parent = parent.get_parent()
    return list(reversed(result))


def read_objects(debug_path, dll_path):
    debug, image = Image(debug_path.read_bytes()), Image(dll_path.read_bytes())
    dwarf = debug.dwarf()
    info = debug.section_data(debug.sections['.debug_info'])
    offset, rows, excluded = 0, [], Counter()
    while offset + 4 <= len(info):
        length = struct.unpack_from('<I', info, offset)[0]
        if length == 0:
            break
        if length == 0xffffffff:
            raise ValueError('DWARF64 data inventory is unsupported')
        cu = dwarf.get_CU_at(offset)
        offset += 4 + length
        source = value(cu.get_top_DIE(), 'DW_AT_name', '').replace('\\', '/').removeprefix('./')
        if source.startswith(('cards/', 'functions/')):
            for die in cu.iter_DIEs():
                if die.tag != 'DW_TAG_variable' or value(die, 'DW_AT_declaration', False):
                    continue
                location = die.attributes.get('DW_AT_location')
                name = inherited_value(die, 'DW_AT_name')
                if (not name or not location or not isinstance(location.value, list)
                        or len(location.value) != 5 or location.value[0] != 3):
                    excluded['no named direct-address storage'] += 1
                    continue
                address = int.from_bytes(bytes(location.value[1:]), 'little')
                section = image.pe.get_section_by_rva(address - image.pe.OPTIONAL_HEADER.ImageBase)
                if section is None or section.Characteristics & 0x20000000:
                    excluded['outside DLL data sections'] += 1
                    continue
                declared = inherited_type(die)
                try:
                    size = sizeof(declared)
                    contract = type_contract(declared)
                except (ValueError, KeyError, TypeError, RecursionError):
                    excluded['unsupported fixed storage type'] += 1
                    continue
                if not 0 < size <= 0x1000000 or not supported(contract):
                    excluded['unsupported fixed storage type'] += 1
                    continue
                if address + size > image.pe.OPTIONAL_HEADER.ImageBase + section.VirtualAddress + section.Misc_VirtualSize:
                    excluded['object exceeds section'] += 1
                    continue
                rows.append(dict(source=source, name=name, scopes=scopes(die),
                                 address=address, size=size, storage_contract=contract,
                                 writable=bool(section.Characteristics & 0x80000000),
                                 die_offset=hex(die.offset)))
        cu._dielist.clear()
        cu._diemap.clear()
    # Several DIEs can describe one emitted static. Preserve distinct owners,
    # sizes and types, even if their addresses happen to coincide.
    unique = {}
    for row in rows:
        key = json.dumps({k: v for k, v in row.items() if k != 'die_offset'}, sort_keys=True)
        unique.setdefault(key, row)
    return dict(objects=list(unique.values()), excluded=dict(excluded))


def cached_objects(debug_path, dll_path, cache):
    state = dict(debug_sha256=digest(debug_path), dll_sha256=digest(dll_path),
                 reader_sha256=hashlib.sha256((inspect.getsource(read_objects) + inspect.getsource(scopes)).encode()).hexdigest(),
                 dependencies={name: digest(Path(__file__).with_name(name)) for name in
                               ('inspect_release.py', 'helper_contracts.py', 'global_initializers.py', 'object_initializers.py')})
    if cache.is_file():
        saved = json.loads(cache.read_bytes())
        if saved.get('inputs') == state:
            return saved
    result = dict(inputs=state, **read_objects(debug_path, dll_path))
    cache.parent.mkdir(parents=True, exist_ok=True)
    cache.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    return result


def pair_objects(native, rebuilt):
    normalize = lambda source: source.replace('cards/odyssey.c', 'cards/odissey.c')
    key = lambda row: (normalize(row['source']), tuple(row['scopes']), row['name'])
    originals, currents = defaultdict(list), defaultdict(list)
    for row in native['objects']:
        originals[key(row)].append(row)
    for row in rebuilt['objects']:
        currents[key(row)].append(row)
    bindings, excluded = [], Counter()
    for identity, old in originals.items():
        new = currents.get(identity, [])
        if len(old) != 1 or len(new) != 1:
            excluded['missing or ambiguous source/scope/name identity'] += 1
            continue
        a, b = old[0], new[0]
        if any(a[field] != b[field] for field in ('size', 'storage_contract', 'writable')):
            excluded['storage contract differs'] += 1
            continue
        bindings.append(dict(name=a['name'], source=identity[0], scopes=list(identity[1]),
                             symbol='manalink-data:' + json.dumps(identity, separators=(',', ':')),
                             native_address=a['address'], rebuilt_address=b['address'], size=a['size'],
                             storage_contract=a['storage_contract'], writable=a['writable'],
                             native_die=a['die_offset'], rebuilt_die=b['die_offset']))
    # Never give overlapping or compiler-shared storage a guessed identity.
    conflicts = set()
    for field in ('native_address', 'rebuilt_address'):
        ordered = sorted(enumerate(bindings), key=lambda pair: pair[1][field])
        active = []
        for index, row in ordered:
            active = [(i, end) for i, end in active if end > row[field]]
            for other, _ in active:
                conflicts.update((index, other))
            active.append((index, row[field] + row['size']))
    excluded['overlapping candidate storage'] = len(conflicts)
    bindings = [row for i, row in enumerate(bindings) if i not in conflicts]
    counts = Counter(row['name'] for row in bindings)
    for row in bindings:
        owner = '::'.join([row['source'], *row['scopes']])
        row['display_name'] = row['name'] if counts[row['name']] == 1 else f"{row['name']} [{owner}]"
    if len({row['display_name'] for row in bindings}) != len(bindings):
        raise ValueError('Project-data display names are not unique')
    return bindings, dict(excluded)


def install_adapter(asmcmp, bindings, audit):
    """Seed missing data nodes, then verify the fork's actual data matches."""
    from reccmp.cvdump.analysis import CvdumpNode
    from reccmp.types import EntityType

    parent = asmcmp.Compare

    class ManalinkCompare(parent):
        def run(self):
            analysis = self.cvdump_analysis
            for row in bindings:
                address = row['rebuilt_address']
                section, offset = self.recomp_bin.get_relative_addr(address)
                # A future fork may parse globals itself. Reuse exactly the
                # expected node, and refuse conflicting or ambiguous nodes.
                present = [node for node in analysis.nodes if node.node_type == EntityType.DATA
                           and (node.section, node.offset) == (section, offset)]
                if present:
                    if (len(present) != 1 or present[0].friendly_name not in (row['name'], row['display_name'])
                            or (present[0].section, present[0].offset) != (section, offset)
                            or present[0].confirmed_size != row['size']):
                        raise ValueError('Conflicting rebuilt data node: ' + row['name'])
                    present[0].decorated_name = row['symbol']
                    present[0].friendly_name = row['display_name']
                    continue
                analysis.nodes.append(CvdumpNode(section=section, offset=offset,
                                                friendly_name=row['display_name'], decorated_name=row['symbol'], node_type=EntityType.DATA,
                                                confirmed_size=row['size']))
            analysis.nodes.sort(key=lambda node: (node.section, node.offset))
            super().run()
            actual = {row.orig_addr: row.recomp_addr for row in self.get_variables()}
            for row in bindings:
                if actual.get(row['native_address']) != row['rebuilt_address']:
                    raise ValueError('Global was not matched to its checked address: ' + row['name'])
            audit.update(verified=len(bindings), bindings_sha256=hashlib.sha256(
                json.dumps(bindings, sort_keys=True).encode()).hexdigest())

    asmcmp.Compare = ManalinkCompare


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--debug', type=Path, required=True)
    parser.add_argument('--dll', type=Path, required=True)
    parser.add_argument('--cache', type=Path, required=True)
    args = parser.parse_args()
    result = cached_objects(args.debug, args.dll, args.cache)
    print(json.dumps(dict(objects=len(result['objects']), excluded=result['excluded'], cache=str(args.cache))))
