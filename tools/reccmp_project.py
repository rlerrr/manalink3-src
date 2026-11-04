"""Set up and run the local DWARF-enabled reccmp fork without installing it.

Only reads the release/build binaries. Local paths and reports are ignored by Git.
"""

import argparse
from collections import Counter, defaultdict
import csv
import hashlib
import inspect
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


def comparison_function_aliases():
    """Read explicit identities for retained C helpers renamed in source."""
    path = ROOT / 'config/reccmp/function-aliases.json'
    if not path.exists():
        return []
    record = json.loads(path.read_bytes())
    if record.get('format') != 1 or not isinstance(record.get('functions'), list):
        raise ValueError('Invalid comparison function-alias manifest')
    seen = set()
    for row in record['functions']:
        address = int(row['native_address'], 16)
        source = row['source']
        if (address in seen or not 0x2000000 <= address < 0x3000000
                or not source.startswith(('cards/', 'functions/'))
                or not source.endswith('.c') or '..' in source.split('/')
                or not row['native_name'].isidentifier()
                or not row['rebuilt_name'].isidentifier()):
            raise ValueError(f'Invalid or duplicate comparison alias: {row}')
        seen.add(address)
    return record['functions']


def naming_state():
    """Bind generated names to the source mapping, independently of the build.

    The recovery ledger keeps historical identities after C functions are
    renamed. A matching DLL/debug pair alone cannot detect an outdated CSV.
    Ignore verification summaries that do not affect these name mappings.
    """
    manifest = json.loads((ROOT / "docs/recovery-evidence/recovered-functions.json").read_text())
    mappings = {
        key: sorted((row.get("address", row.get("native_address")), row["name"])
                    for row in manifest[key])
        for key in ("accepted_functions", "source_stubs")
    }
    canonical = ROOT / "docs/recovery-evidence/helper-canonicalization.json"
    canonical_bindings = None
    if canonical.exists():
        proof = json.loads(canonical.read_bytes())
        entries = proof['functions'] + proof.get('auxiliary_functions', [])
        canonical_bindings = dict(
            names=sorted((row['recovery_name'], row['canonical_name']) for row in entries),
            auxiliary=[{key: row.get(key) for key in
                        ('native_address', 'source', 'recovery_name', 'canonical_name')}
                       for row in proof.get('auxiliary_functions', [])],
            equivalents=proof.get('native_equivalences', []),
            additional_stubs=proof.get('additional_source_stubs', []))
        chooser = proof.get('native_cpp_chooser_recovery')
        if chooser:
            record = json.loads((ROOT / chooser).read_bytes())
            canonical_bindings['chooser_stubs'] = record['resolved_stubs']
            canonical_bindings['chooser_functions'] = [
                (row['address'], row['name'], row['kind']) for row in record['functions']]
    return dict(format=3, comparison_function_aliases=comparison_function_aliases(),
                canonical_bindings_sha256=hashlib.sha256(
                    json.dumps(canonical_bindings, sort_keys=True).encode()).hexdigest(),
                recovery_mappings_sha256=hashlib.sha256(
                    json.dumps(mappings, sort_keys=True).encode()).hexdigest())


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


def cached_functions(path, cache, all_functions=False):
    """Reuse bounds only for the same debug bytes and reader implementation."""
    state = dict(sha256=digest(path), all_functions=all_functions,
                 reader_sha256=hashlib.sha256(inspect.getsource(read_functions).encode()).hexdigest())
    if cache.is_file():
        saved = json.loads(cache.read_bytes())
        if all(saved.get(key) == value for key, value in state.items()):
            return saved['functions']
    rows = read_functions(path, all_functions=all_functions)
    write_json(cache, dict(state, functions=rows))
    return rows


def bind_native_function_names(native, accepted, stubs, canonical_names, auxiliary,
                               comparison_aliases=()):
    """Honor address-bound retained helpers as well as recovered entries.

    A retained helper can have a new C name because another source unit still
    owns its old spelling. A global spelling match would select that other
    function even though its source and native address are unrelated.
    """
    bindings = {}
    for entry in auxiliary:
        if not entry.get('native_address'):
            continue
        address = int(str(entry['native_address']), 16)
        if address in bindings:
            raise ValueError(f'Duplicate auxiliary native binding: {hex(address)}')
        bindings[address] = entry
    aliases = {}
    for entry in comparison_aliases:
        address = int(entry['native_address'], 16)
        if address in aliases:
            raise ValueError(f'Duplicate comparison alias at {hex(address)}')
        originals = [row for row in native if row['address'] == address]
        if (len(originals) != 1 or originals[0]['name'] != entry['native_name']
                or originals[0]['source'] != entry['source']):
            raise ValueError(f'Comparison alias disagrees with native DWARF at {hex(address)}')
        aliases[address] = entry
    for row in native:
        mapped = accepted.get(row['address']) or stubs.get(row['address'])
        logical_name = mapped['name'] if mapped else row['name']
        match_name = canonical_names.get(logical_name, logical_name)
        auxiliary = bindings.get(row['address'])
        if auxiliary:
            if auxiliary['source'] != row['source']:
                raise ValueError(f'Auxiliary source disagrees at {hex(row["address"])}')
            if mapped and match_name != auxiliary['canonical_name']:
                raise ValueError(f'Recovery/auxiliary names disagree at {hex(row["address"])}')
            row['match_name'] = auxiliary['canonical_name']
            row['mapping'] = 'verified auxiliary native binding'
        else:
            row['match_name'] = match_name
            row['mapping'] = 'recovery ledger' if mapped else 'same native name'
        alias = aliases.get(row['address'])
        if alias:
            if row['match_name'] not in (alias['native_name'], alias['rebuilt_name']):
                raise ValueError(f'Comparison/recovery names disagree at {hex(row["address"])}')
            row['match_name'] = alias['rebuilt_name']
            row['mapping'] = 'comparison source-name alias'


def source_bound_entries(native, current, native_symbols, rebuilt_symbols):
    """Resolve C name collisions by CU identity or an exact COFF entry.

    A plain C function has no overloads. A unique source/name range on each
    side is an identity, even when another CU or C++ class uses the same name.
    For split ranges, only a unique exact undecorated COFF entry is selected;
    the other native ranges still need their own mapping.
    """
    normalize = lambda source: source.replace('cards/odyssey.c', 'cards/odissey.c')
    old_groups, new_groups = defaultdict(list), defaultdict(list)
    for row in native:
        old_groups[(normalize(row['source']), row['match_name'])].append(row)
    for row in current:
        new_groups[(normalize(row['source']), row['name'])].append(row)
    result = {}
    for key, originals in old_groups.items():
        source, name = key
        if not source.endswith('.c') or not name.isidentifier():
            continue
        rebuilt = new_groups.get(key, [])
        if not rebuilt:
            continue
        if len(originals) == len(rebuilt) == 1:
            result[originals[0]['address']] = dict(
                rebuilt_address=rebuilt[0]['address'], rebuilt_size=rebuilt[0]['size'],
                source=source, name=name, resolution='unique C source/name range on each side')
            continue
        # Select the public entry only when both sides' exact COFF name and
        # owning-CU DWARF range agree. Do not pair private parts by size/order.
        original_entries = [row for row in originals
                            if row['address'] in native_symbols.get(row['name'], set())]
        rebuilt_entries = [row for row in rebuilt
                           if row['address'] in rebuilt_symbols.get(name, set())]
        if len(original_entries) == len(rebuilt_entries) == 1:
            result[original_entries[0]['address']] = dict(
                rebuilt_address=rebuilt_entries[0]['address'], rebuilt_size=rebuilt_entries[0]['size'],
                source=source, name=name, resolution='owning C CU and unique exact COFF entry on each side')
    # Reccmp's entity database is one-to-one. Never resolve two original
    # identities to one emitted body through this source-name rule.
    by_rebuilt = Counter(row['rebuilt_address'] for row in result.values())
    return {address: row for address, row in result.items()
            if by_rebuilt[row['rebuilt_address']] == 1}


def setup(args):
    if args.audit_recovery and (ROOT/'docs/recovery-evidence/historical-artifact-retirement.json').exists():
        raise ValueError('Historical recovery replay was retired with its obsolete compiler/runtime snapshots. Run setup without --audit-recovery; see docs/recovery-evidence/historical-artifact-retirement.json.')
    from reccmp.project.config import ProjectFile, UserFile, BuildFile

    local = ROOT / ".recovery" / "reccmp"
    local.mkdir(parents=True, exist_ok=True)
    names_before = naming_state()
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
    engine = args.engine_exe.resolve()
    from reccmp_externals import ENGINE_SHA256, read_bindings as read_external_bindings
    if not engine.is_file():
        archive = args.library / 'Manalink-3_0-Oct-2022_Hotfix.zip'
        with zipfile.ZipFile(archive) as release:
            binary = release.read('Magic.exe')
        if hashlib.sha256(binary).hexdigest() != ENGINE_SHA256:
            raise ValueError('Unexpected October 2022 Hotfix Magic.exe hash')
        engine.parent.mkdir(parents=True, exist_ok=True)
        engine.write_bytes(binary)
    if digest(engine) != ENGINE_SHA256:
        raise ValueError('Engine executable differs from the pinned October 2022 Hotfix')
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
    native = cached_functions(original_debug, local / 'native-function-bounds.json')
    print("Reading rebuilt DWARF names...", flush=True)
    current = cached_functions(rebuilt_debug, local / 'triage-rebuilt-bounds.json', all_functions=True)
    # Triage needs these same complete bounds. Persist this parse once per
    # setup; the consumer rejects it if the actual debug-file hash changes.
    manifest = json.loads((ROOT / "docs/recovery-evidence/recovered-functions.json").read_text())
    accepted = {int(row["address"], 16): row for row in manifest["accepted_functions"]}
    from canonicalize_helpers import names as canonical_helper_names,verify as verify_canonical_helpers,verify_current
    canonical_helpers=(verify_canonical_helpers if args.audit_recovery else verify_current)(ROOT)
    canonical_names=canonical_helper_names(ROOT)
    from canonicalize_helpers import load as load_canonical,verify_native_equivalences
    canonical_data=load_canonical(ROOT)
    equivalent_helpers=(canonical_helpers['equivalent_native_helpers'] if args.audit_recovery else
                        verify_native_equivalences(canonical_data,ROOT)) if canonical_data else []
    from verify_native_jump_table import verify as verify_fixed_slots
    fixed_slots=(canonical_helpers['native_interfaces']['native_jump_table'] if args.audit_recovery else
                 verify_fixed_slots(canonical_data or {},root=ROOT,binary_only=True,rebuilt_path=rebuilt,rebuilt_debug_path=rebuilt_debug))
    equivalent_addresses={int(e['duplicate_address'],16) for e in equivalent_helpers}
    stubs = {int(row["native_address"], 16): row for row in
             manifest["source_stubs"] + (canonical_data or {}).get("additional_source_stubs", [])}
    from verify_cpp_chooser_recovery import resolved_stub_addresses
    for address in resolved_stub_addresses(canonical_data or {},ROOT):
        stubs.pop(address,None)
    current_names = defaultdict(set)
    current_ranges = defaultdict(list)
    for row in current:
        current_names[row["name"]].add(row["address"])
        current_ranges[(row["source"], row["name"])].append(row)
    # GCC may keep a public COFF entry as a JMP to an identical retained
    # body, omitting a separate DWARF range. Record that evidence explicitly;
    # do not call the five-byte thunk a full-body assembly match.
    debug_image = Image(rebuilt_debug.read_bytes())
    coff_functions = defaultdict(set)
    for name, offset, section, kind, storage in debug_image.functions():
        if section > 0 and kind == 32:
            coff_functions[name.lstrip('_')].add(debug_image.pe.OPTIONAL_HEADER.ImageBase +
                debug_image.pe.sections[section-1].VirtualAddress + offset)
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32
    from capstone.x86_const import X86_OP_IMM
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.detail = True
    folded_recovered_helpers = []
    def compiler_fold(row):
        choices = coff_functions[row['match_name']]
        if row['address'] not in accepted or len(choices) != 1:
            return None
        address = next(iter(choices))
        same_address = [r for r in current if r['address'] == address and
                        r['name'] == row['name'] and r['source'] == row['source']]
        if len(same_address) == 1:
            target = address
            representation, branch_bytes = 'coff_alias', ''
        else:
            code = rebuilt_image.pe.get_data(address-rebuilt_image.pe.OPTIONAL_HEADER.ImageBase, 5)
            first = next(decoder.disasm(code, address, count=1), None)
            if first is None or first.mnemonic != 'jmp' or first.operands[0].type != X86_OP_IMM:
                return None
            target = first.operands[0].imm
            representation, branch_bytes = 'thunk', first.bytes.hex()
        ranges = [r for r in current if r['address'] == target and
                  r['name'] == row['name'] and r['source'] == row['source']]
        if len(ranges) != 1:
            return None
        length = ranges[0]['size']
        return dict(native_address=hex(row['address']), name=row['match_name'], source=row['source'],
                    retained_name=row['name'], address=hex(address), representation=representation, branch_bytes=branch_bytes,
                    target=hex(target), target_bytes=length,
                    target_sha256=hashlib.sha256(rebuilt_image.pe.get_data(
                        target-rebuilt_image.pe.OPTIONAL_HEADER.ImageBase,length)).hexdigest())

    # Explicit address bindings also cover retained helpers whose C spelling
    # changed to avoid a collision with a helper in another source unit.
    comparison_aliases = comparison_function_aliases()
    bind_native_function_names(native, accepted, stubs, canonical_names,
                               (canonical_data or {}).get('auxiliary_functions', []),
                               comparison_aliases)
    native_image = Image(original_debug.read_bytes())
    native_coff_functions = defaultdict(set)
    for symbol, offset, section, kind, storage in native_image.functions():
        if section > 0 and kind == 32:
            native_coff_functions[symbol.lstrip('_')].add(
                native_image.pe.OPTIONAL_HEADER.ImageBase +
                native_image.pe.sections[section-1].VirtualAddress + offset)
    source_entries = source_bound_entries(native, current, native_coff_functions, coff_functions)
    alias_addresses = {int(row['native_address'], 16) for row in comparison_aliases}
    if not alias_addresses.issubset(source_entries):
        missing = sorted(hex(address) for address in alias_addresses - source_entries.keys())
        raise ValueError(f'Comparison aliases lack unique source-bound rebuilt ranges: {missing}')
    native_names = defaultdict(set)
    for row in native:
        if row['address'] in equivalent_addresses:continue
        native_names[row["match_name"]].add(row["address"])

    included, excluded = [], []
    resolved_duplicate_names = []
    resolved_public_entries = []
    resolved_source_entries = []
    for row in native:
        name = row["match_name"]
        reason = None
        if row['address'] in equivalent_addresses:
            reason='verified equivalent native helper shares a canonical implementation'
        elif (row['address'] in source_entries
              and (row['address'] in alias_addresses
                   or len(native_names[name]) != 1 or len(current_names[name]) > 1)):
            entry = source_entries[row['address']]
            row['mapping'] = f"rebuilt entry {hex(entry['rebuilt_address'])}"
            resolved_source_entries.append(dict(native_address=hex(row['address']),
                                                native_size=row['size'], **entry))
        elif len(native_names[name]) != 1:
            # Release DWARF can contain multiple non-contiguous ranges for a
            # single C function name (for example, a retained body plus a
            # compiler-generated argument-checking wrapper).  Keep the whole
            # range set when it is isolated to one source/name and the rebuilt
            # image has the same number of ranges.  The reccmp fork resolves
            # these entries by exact emitted size, without joining the bytes
            # between them, which may belong to other functions.
            native_group = [candidate for candidate in native
                            if candidate["match_name"] == name
                            and candidate["source"] == row["source"]]
            current_group = current_ranges.get((row["source"], name), [])
            if (len(native_group) > 1
                    and len(native_group) == len(current_group)
                    and len(native_names[name]) == len(native_group)
                    and len(current_names[name]) == len(current_group)):
                resolved_duplicate_names.append({
                    "name": name,
                    "source": row["source"],
                    "address": hex(row["address"]),
                    "size": row["size"],
                    "resolution": "isolated duplicate range set matched by emitted size/order",
                    "native_candidates": sorted(hex(address)
                                                 for address in native_names[name]),
                })
            else:
                reason = "ambiguous original name"
        elif len(current_names[name]) > 1:
            # GCC may emit a public entry plus a private .part.0 body under
            # the same DWARF name.  If the rebuilt image has one unique COFF
            # entry for the name, retain the native row; the fork matcher will
            # select that public symbol while leaving the private body as a
            # separate unmatched range.
            current_group = current_ranges.get((row["source"], name), [])
            public_entries = coff_functions.get(name, set())
            if (len(native_names[name]) == 1 and len(current_group) > 1
                    and len(public_entries) == 1
                    and any(candidate["address"] in public_entries
                            for candidate in current_group)):
                public_address = next(iter(public_entries))
                row["mapping"] = f"public COFF entry {hex(public_address)}"
                resolved_public_entries.append({
                    "name": name,
                    "source": row["source"],
                    "address": hex(row["address"]),
                    "size": row["size"],
                    "resolution": "unique rebuilt public COFF entry",
                    "rebuilt_address": hex(public_address),
                    "rebuilt_candidates": sorted(
                        hex(candidate["address"]) for candidate in current_group),
                })
            else:
                reason = "ambiguous rebuilt name"
        elif not current_names[name]:
            folded = compiler_fold(row)
            if folded is not None:
                folded_recovered_helpers.append(folded)
                reason = "verified compiler-folded helper without standalone DWARF range"
            else:
                reason = "no rebuilt function with this name"
        if reason:
            excluded.append(dict(row, reason=reason))
        else:
            included.append(row)

    # These are absolute VAs, not RVAs or veneer jump-table entries.
    obsolete_names = set(canonical_names) - set(canonical_names.values())
    assert not any(row["match_name"] in obsolete_names for row in included)
    if naming_state() != names_before:
        raise ValueError("Source name mappings changed during setup; rerun setup")
    metadata = ROOT / "config" / "reccmp" / "functions.csv"
    metadata.parent.mkdir(parents=True, exist_ok=True)
    temporary_metadata = metadata.with_suffix(".pending.csv")
    with temporary_metadata.open("w", newline="", encoding="utf-8") as stream:
        writer = csv.writer(stream, lineterminator="\n")
        writer.writerow(["address", "type", "name", "size", "native_name", "source", "mapping"])
        for row in sorted(included, key=lambda r: r["address"]):
            writer.writerow([hex(row["address"]), "stub" if row["address"] in stubs else "function",
                             row["match_name"], row["size"], row["name"], row["source"], row["mapping"]])
    temporary_metadata.replace(metadata)
    from reccmp_globals import cached_objects, pair_objects
    print('Reading project data identities...', flush=True)
    native_objects = cached_objects(original_debug, original, local / 'native-project-objects.json')
    rebuilt_objects = cached_objects(rebuilt_debug, rebuilt, local / 'rebuilt-project-objects.json')
    global_bindings, excluded_globals = pair_objects(native_objects, rebuilt_objects)
    global_metadata = metadata.with_name('globals.csv')
    temporary_globals = global_metadata.with_suffix('.pending.csv')
    with temporary_globals.open('w', newline='', encoding='utf-8') as stream:
        writer = csv.writer(stream, lineterminator='\n')
        writer.writerow(['address', 'type', 'name', 'size', 'symbol', 'source', 'scope', 'native_name'])
        for row in sorted(global_bindings, key=lambda row: row['native_address']):
            writer.writerow([hex(row['native_address']), 'global', row['display_name'], row['size'],
                             row['symbol'], row['source'], '::'.join(row['scopes']), row['name']])
    temporary_globals.replace(global_metadata)
    externals = read_external_bindings(original_debug, rebuilt_debug, engine)
    external_metadata = metadata.with_name('externals.csv')
    temporary_externals = external_metadata.with_suffix('.pending.csv')
    with temporary_externals.open('w', newline='', encoding='utf-8') as stream:
        writer = csv.writer(stream, lineterminator='\n')
        writer.writerow(['address', 'type', 'label', 'aliases'])
        for row in externals['bindings']:
            writer.writerow([hex(row['address']), 'external_function', row['label'],
                             '|'.join(alias['symbol'] for alias in row['aliases'])])
    temporary_externals.replace(external_metadata)
    write_json(local / "excluded-functions.json", excluded)

    # JSON is a YAML subset; the public project uses the hand-maintained YAML file.
    write_json(ROOT / "reccmp-user.yml", {"targets": {TARGET: {"path": original.as_posix()}}})
    write_json(ROOT / "reccmp-build.yml", {"project": ".", "targets": {
        TARGET: {"path": rebuilt.as_posix(), "pdb": rebuilt_debug.as_posix()}}})
    # Validate using this fork's actual configuration schema.
    for model, filename in ((ProjectFile, "reccmp-project.yml"), (UserFile, "reccmp-user.yml"),
                            (BuildFile, "reccmp-build.yml")):
        model.from_file(ROOT / filename)
    summary = dict(target=TARGET, canonical_helpers=canonical_helpers,native_equivalent_helpers=equivalent_helpers,
                   recovery_audit_mode='full_history' if args.audit_recovery else 'current_inputs',fixed_jump_table=fixed_slots,
                   original_functions=len(native), csv_functions=len(included),
                   ledger_mappings=sum(r["mapping"] == "recovery ledger" for r in included),
                   stubs=sum(r["address"] in stubs for r in included),
                   folded_recovered_helpers=folded_recovered_helpers,
                   resolved_duplicate_names=resolved_duplicate_names,
                   resolved_public_entries=resolved_public_entries,
                   resolved_source_entries=resolved_source_entries,
                   comparison_function_aliases=comparison_aliases,
                   excluded_reasons=dict(Counter(r["reason"] for r in excluded)),
                   inputs={str(p): digest(p) for p in (original, original_debug, rebuilt, rebuilt_debug, engine)},
                   naming_inputs=names_before,
                   externals=externals, external_metadata_sha256=digest(external_metadata),
                   metadata_sha256=digest(metadata), reccmp_root=str(args.reccmp_root.resolve()),
                   global_metadata_sha256=digest(global_metadata),
                   globals=dict(bindings=global_bindings, count=len(global_bindings), excluded=excluded_globals,
                                inventory_inputs=dict(native=native_objects['inputs'], rebuilt=rebuilt_objects['inputs']),
                                scope='Direct-address DLL data with unique source/scope/name identities, matching coarse storage contracts and no overlapping candidate extents; not initializer or full record-ABI equivalence.'),
                   scope="Contiguous card/helper functions plus checked project data identities; GUI recovery is tracked separately.")
    write_json(local / "setup.json", summary)
    print(json.dumps({k: v for k, v in summary.items() if k != "inputs"}, indent=2))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--reccmp-root", type=Path, default=Path("C:/dev/reccmp"))
    subparsers = parser.add_subparsers(dest="command", required=True)
    init = subparsers.add_parser("setup", help="Generate function CSV and local path configuration")
    init.add_argument("--audit-recovery", action="store_true",
                      help="Also replay all historical recovery/native audits (slow; not needed to regenerate metadata)")
    init.add_argument("--library", type=Path, default=Path("G:/Manalink"))
    init.add_argument("--original", type=Path, default=ROOT / ".recovery/2022-ManalinkEh.dll")
    init.add_argument("--original-debug", type=Path, default=Path("G:/Manalink/ManalinkEh_10_22_Hotfix.dbg"))
    init.add_argument("--rebuilt", type=Path, default=ROOT / "ManalinkEh.dll")
    init.add_argument("--rebuilt-debug", type=Path, default=ROOT / "ManalinkEh.dbg")
    init.add_argument('--engine-exe', type=Path, default=ROOT / '.recovery/2022-Magic.exe',
                      help='Original engine used only to validate fixed external function targets')
    compare = subparsers.add_parser("compare", help="Forward remaining options to reccmp-reccmp")
    compare.add_argument('--audit-externals', action='store_true',
                         help='Also compare affected ranges without external labels, using the same bytes')
    compare.add_argument('--no-external-symbols', action='store_true',
                         help='Disable external labels for a comparison control run')
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
    summary_bytes = summary_path.read_bytes()
    summary = json.loads(summary_bytes)
    comparison_setup_sha256 = hashlib.sha256(summary_bytes).hexdigest()
    for filename, expected in summary["inputs"].items():
        if digest(Path(filename)) != expected:
            parser.error(f"Input changed since setup: {filename}; rerun setup")
    if digest(ROOT / "config/reccmp/functions.csv") != summary["metadata_sha256"]:
        parser.error("Function metadata changed since setup; rerun setup")
    if 'globals' not in summary or 'global_metadata_sha256' not in summary:
        parser.error('Setup predates project-data support; rerun setup')
    global_metadata = ROOT / 'config/reccmp/globals.csv'
    if digest(global_metadata) != summary['global_metadata_sha256']:
        parser.error('Global metadata changed since setup; rerun setup')
    if summary.get("naming_inputs") != naming_state():
        parser.error("Source name mappings changed since setup; rerun setup")
    if 'externals' not in summary or 'external_metadata_sha256' not in summary:
        parser.error('Setup predates external-symbol support; rerun setup')
    external_metadata = ROOT / 'config/reccmp/externals.csv'
    if digest(external_metadata) != summary['external_metadata_sha256']:
        parser.error('External metadata changed since setup; rerun setup')
    if args.audit_externals and args.no_external_symbols:
        parser.error('--audit-externals requires external-symbol matching')
    sys.path.insert(0, str(ROOT / 'tools/recovery'))
    from reccmp_externals import read_bindings as read_external_bindings
    external_paths = summary['externals']['paths']
    if read_external_bindings(external_paths['original_debug'], external_paths['rebuilt_debug'],
                              external_paths['engine']) != summary['externals']:
        parser.error('External identity inputs or reader changed; rerun setup')
    comparator_sources = {str(fork / name): digest(fork / name) for name in (
        'reccmp/compare/functions.py', 'reccmp/compare/asm/parse.py',
        'reccmp/compare/asm/fixes.py', 'reccmp/compare/report.py',
        'reccmp/compare/csv.py', 'reccmp/compare/match_msvc.py',
        'reccmp/dwarf/analysis.py', 'reccmp/compare/asm/replacement.py',
        'reccmp/compare/core.py', 'reccmp/compare/db.py', 'reccmp/types.py')}
    comparator_sources[str(ROOT / 'tools/recovery/reccmp_globals.py')] = digest(ROOT / 'tools/recovery/reccmp_globals.py')
    comparator_sources[str(ROOT / 'tools/recovery/reccmp_externals.py')] = digest(ROOT / 'tools/recovery/reccmp_externals.py')
    comparator_sources[str(Path(__file__))] = digest(Path(__file__))
    project_config = ROOT / 'reccmp-project.yml'
    project_config_sha256 = digest(project_config)
    ledger_path = ROOT / 'docs/recovery-evidence/recovered-functions.json'
    comparison_ledger_sha256 = digest(ledger_path)
    from reccmp.tools import asmcmp
    sys.path.insert(0, str(ROOT / 'tools/recovery'))
    from reccmp_globals import install_adapter
    global_audit = {}
    install_adapter(asmcmp, summary['globals']['bindings'], global_audit)
    from reccmp_externals import install_adapter as install_external_adapter
    external_audit = dict(verified=0, enabled=not args.no_external_symbols)
    if not args.no_external_symbols:
        install_external_adapter(asmcmp, summary['externals']['bindings'], external_audit,
                                 compare_baseline=args.audit_externals)
    # Upstream prints Unicode status markers even with --no-color. Windows
    # redirected stdout otherwise defaults to cp1252 and fails on exact matches.
    for stream in (sys.stdout, sys.stderr):
        if hasattr(stream, "reconfigure"):
            stream.reconfigure(encoding="utf-8")
    sys.argv = ["reccmp-reccmp", "--target", TARGET, "--no-color", *remaining]
    exit_code = asmcmp.main()
    report_path = None
    for index, option in enumerate(remaining):
        if option == '--json' and index + 1 < len(remaining):
            report_path = Path(remaining[index + 1])
        elif option.startswith('--json='):
            report_path = Path(option.partition('=')[2])
    if exit_code == 0 and report_path is not None:
        # Bind scores to the precise pair used by this invocation. JSON's
        # timestamp/name fields alone cannot detect a stale rebuild/report.
        for filename, expected in summary['inputs'].items():
            if digest(Path(filename)) != expected:
                raise ValueError(f'Input changed during comparison: {filename}')
        if digest(ROOT / 'config/reccmp/functions.csv') != summary['metadata_sha256']:
            raise ValueError('Function metadata changed during comparison')
        if digest(global_metadata) != summary['global_metadata_sha256']:
            raise ValueError('Global metadata changed during comparison')
        if digest(external_metadata) != summary['external_metadata_sha256']:
            raise ValueError('External metadata changed during comparison')
        if digest(project_config) != project_config_sha256:
            raise ValueError('Project configuration changed during comparison')
        if global_audit.get('verified') != summary['globals']['count']:
            raise ValueError('Project-data address bindings were not verified')
        if external_audit.get('verified') != (0 if args.no_external_symbols else summary['externals']['count']):
            raise ValueError('External function lookups were not verified')
        if digest(summary_path) != comparison_setup_sha256 or digest(ledger_path) != comparison_ledger_sha256:
            raise ValueError('Setup or recovery ledger changed during comparison')
        if naming_state() != summary['naming_inputs']:
            raise ValueError('Source name mappings changed during comparison')
        if any(digest(Path(name)) != expected for name, expected in comparator_sources.items()):
            raise ValueError('Comparator implementation changed during comparison')
        metadata_snapshot = report_path.with_suffix('.metadata.csv')
        metadata_bytes = (ROOT / 'config/reccmp/functions.csv').read_bytes()
        if hashlib.sha256(metadata_bytes).hexdigest() != summary['metadata_sha256']:
            raise ValueError('Function metadata changed before report snapshot')
        metadata_snapshot.write_bytes(metadata_bytes)
        global_snapshot = report_path.with_suffix('.globals.csv')
        global_bytes = global_metadata.read_bytes()
        if hashlib.sha256(global_bytes).hexdigest() != summary['global_metadata_sha256']:
            raise ValueError('Global metadata changed before report snapshot')
        global_snapshot.write_bytes(global_bytes)
        bindings_snapshot = report_path.with_suffix('.globals.json')
        write_json(bindings_snapshot, summary['globals'])
        external_snapshot = report_path.with_suffix('.externals.csv')
        external_bytes = external_metadata.read_bytes()
        if hashlib.sha256(external_bytes).hexdigest() != summary['external_metadata_sha256']:
            raise ValueError('External metadata changed before report snapshot')
        external_snapshot.write_bytes(external_bytes)
        external_bindings_snapshot = report_path.with_suffix('.externals.json')
        write_json(external_bindings_snapshot, summary['externals'])
        external_audit_snapshot = report_path.with_suffix('.external-audit.json')
        write_json(external_audit_snapshot, external_audit)
        write_json(report_path.with_suffix('.inputs.json'), dict(
            setup_sha256=comparison_setup_sha256, report_sha256=digest(report_path),
            inputs=summary['inputs'], metadata_sha256=summary['metadata_sha256'],
            metadata_snapshot=str(metadata_snapshot),
            global_metadata_snapshot=str(global_snapshot), global_metadata_sha256=summary['global_metadata_sha256'],
            global_bindings_snapshot=str(bindings_snapshot), global_bindings_sha256=digest(bindings_snapshot),
            external_metadata_snapshot=str(external_snapshot), external_metadata_sha256=summary['external_metadata_sha256'],
            external_bindings_snapshot=str(external_bindings_snapshot), external_bindings_sha256=digest(external_bindings_snapshot),
            external_audit_snapshot=str(external_audit_snapshot), external_audit_sha256=digest(external_audit_snapshot),
            verified_external_bindings=external_audit['verified'], external_matching_enabled=not args.no_external_symbols,
            verified_global_bindings=global_audit, project_config_sha256=project_config_sha256,
            recovery_ledger_sha256=comparison_ledger_sha256,
            comparator_source_sha256=comparator_sources,
            command=sys.argv, exit_code=exit_code))
    return exit_code


if __name__ == "__main__":
    raise SystemExit(main())
