"""Exact Magic.exe function-address identities for DLL comparison only.

These are lookup labels, never extra DLL ranges. A release COFF function must
have the same absolute name/address in the rebuild and point into executable
storage in the pinned release Magic.exe. A rebuilt untyped assembler symbol is
allowed; this does not assert that the two declarations have identical ABIs.
"""
from collections import Counter, defaultdict
from dataclasses import replace
import hashlib
import json
from pathlib import Path

from inspect_release import Image

ENGINE_SHA256 = '0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def pair_symbols(original_symbols, rebuilt_symbols, engine):
    """Match exact global ABS identities; reject conflicting known types."""
    original, rebuilt = defaultdict(set), defaultdict(set)
    for rows, output in ((original_symbols, original), (rebuilt_symbols, rebuilt)):
        for row in rows:
            name, address, section, kind, storage = row
            if section == -1 and storage == 2:
                output[name].add(tuple(row))
    excluded = Counter()
    candidates = []
    for name, old in original.items():
        if not any(row[3] == 32 for row in old):
            continue
        new = rebuilt.get(name, set())
        if len(old) != 1 or len(new) != 1:
            excluded['missing or ambiguous absolute name'] += 1
            continue
        a, b = next(iter(old)), next(iter(new))
        if a[1] != b[1]:
            excluded['absolute address differs'] += 1
            continue
        if a[3] != 32 or b[3] not in (0, 32):
            excluded['conflicting COFF type'] += 1
            continue
        address = a[1]
        rva = address - engine.pe.OPTIONAL_HEADER.ImageBase
        section = engine.pe.get_section_by_rva(rva) if rva >= 0 else None
        if section is None or not section.Characteristics & 0x20000000:
            excluded['target outside executable engine storage'] += 1
            continue
        data = engine.pe.get_data(rva, 16)
        if len(data) != 16:
            excluded['engine entry bytes unavailable'] += 1
            continue
        candidates.append(dict(address=address, symbol=name, native_coff=list(a),
                               rebuilt_coff=list(b), engine_section=section.Name.rstrip(b'\0').decode(),
                               engine_entry_sha256=hashlib.sha256(data).hexdigest()))
    by_address = defaultdict(list)
    for row in candidates:
        by_address[row['address']].append(row)
    bindings = []
    for address, aliases in sorted(by_address.items()):
        aliases.sort(key=lambda row: row['symbol'])
        bindings.append(dict(address=address, aliases=aliases,
                             label='Magic.exe!' + aliases[0]['symbol'] + ' (FUNCTION)'))
    if len({row['label'] for row in bindings}) != len(bindings):
        raise ValueError('External function labels are not unique')
    return bindings, dict(excluded)


def read_bindings(original_debug, rebuilt_debug, engine_path):
    original_debug, rebuilt_debug, engine_path = (Path(path).resolve() for path in
                                                (original_debug, rebuilt_debug, engine_path))
    if digest(engine_path) != ENGINE_SHA256:
        raise ValueError('Expected the original October 2022 Hotfix Magic.exe')
    original, rebuilt, engine = (Image(path.read_bytes()) for path in
                                (original_debug, rebuilt_debug, engine_path))
    if any(image.pe.FILE_HEADER.Machine != 0x14c for image in (original, rebuilt, engine)):
        raise ValueError('External identities require the i386 release/build/engine')
    bindings, excluded = pair_symbols(original.symbols, rebuilt.symbols, engine)
    return dict(bindings=bindings, count=len(bindings), excluded=excluded,
                paths=dict(original_debug=str(original_debug), rebuilt_debug=str(rebuilt_debug), engine=str(engine_path)),
                inputs={str(path): digest(path) for path in (original_debug, rebuilt_debug, engine_path)},
                reader_sha256=digest(Path(__file__)),
                image_reader_sha256=digest(Path(__file__).with_name('inspect_release.py')),
                scope='Exact global ABS name/address identities with a native COFF function type '
                      'and executable entry in pinned Magic.exe. Rebuilt type 0 is untyped, not '
                      'a validated prototype. Exact addresses only; no inferred extents, EXE-body '
                      'comparison, DLL function nodes or ABI-equivalence claim.')


def wrap_parser(parser, bindings, side, record):
    """Add exact non-indirect lookups; keep every other parser rule intact."""
    known = {row['address']: row['label'] for row in bindings}
    parent_lookup, parent_addr_test = parser.name_lookup, parser.addr_test
    for address, label in known.items():
        existing = parent_lookup(address, exact=True, indirect=False) if callable(parent_lookup) else None
        if existing is not None and existing != label:
            raise ValueError(f'External address already has a different identity: {hex(address)}: {existing}')

    def lookup(address, exact=False, indirect=False):
        if not indirect and address in known:
            record(side, address)
            return known[address]
        return parent_lookup(address, exact=exact, indirect=indirect) if callable(parent_lookup) else None

    def is_addr(address):
        return address in known or (parent_addr_test(address) if callable(parent_addr_test) else False)

    parser.name_lookup = lookup
    parser.addr_test = is_addr


def install_adapter(asmcmp, bindings, audit, *, compare_baseline=False):
    """Attach lookup labels after entity matching; optionally audit same-byte scores."""
    parent = asmcmp.Compare

    class ExternalCompare(parent):
        def run(self):
            super().run()
            comparator = self.function_comparator
            baseline = replace(comparator) if compare_baseline else None
            totals = Counter()
            active = None

            def record(side, address):
                totals[side, address] += 1
                if active is not None:
                    active[side, address] += 1

            for side, parser in (('native', comparator.orig_sanitize), ('rebuilt', comparator.recomp_sanitize)):
                wrap_parser(parser, bindings, side, record)
                for row in bindings:
                    if parser.lookup(row['address'], exact=True) != row['label'] or not parser.is_addr(row['address']):
                        raise ValueError('External lookup failed: ' + row['label'])
            totals.clear()
            audit.update(verified=len(bindings), bindings_sha256=hashlib.sha256(
                json.dumps(bindings, sort_keys=True).encode()).hexdigest(),
                same_binary_baseline=compare_baseline, compared_functions=0,
                referencing_functions=[], normalizations=[])
            after_compare = comparator.compare_function

            def compare_function(match):
                nonlocal active
                active = Counter()
                result = after_compare(match)
                used = active
                active = None
                audit['compared_functions'] += 1
                if used:
                    row = dict(address=hex(match.orig_addr), rebuilt_address=hex(match.recomp_addr),
                               name=match.best_name(), after=result.match_ratio,
                               after_effective=result.is_effective_match,
                               used=[dict(side=side, address=hex(address), count=count)
                                     for (side, address), count in sorted(used.items())])
                    if baseline is not None:
                        before = baseline.compare_function(match)
                        row.update(before=before.match_ratio, before_effective=before.is_effective_match)
                        if (before.match_ratio, before.is_effective_match) != (result.match_ratio, result.is_effective_match):
                            from reccmp.compare.diff import raw_diff_to_udiff
                            row['before_diff'] = (raw_diff_to_udiff(before.diff, grouped=True)
                                                  if before.match_ratio != 1 or before.is_effective_match else None)
                    audit['referencing_functions'].append(row)
                audit['normalizations'] = [dict(side=side, address=hex(address), count=count)
                                           for (side, address), count in sorted(totals.items())]
                return result

            comparator.compare_function = compare_function

    asmcmp.Compare = ExternalCompare
