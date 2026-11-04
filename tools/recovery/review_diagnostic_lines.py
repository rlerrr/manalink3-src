"""Identify assertion-location text changes in full reccmp JSON queues.

Only a decimal source line may differ. Diagnostic output still differs on the
assertion path; this annotation does not assert byte or whole-body equivalence.
"""
import argparse
from collections import defaultdict
import csv
import hashlib
import json
from pathlib import Path
import re

import capstone
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM, X86_OP_MEM
from inspect_release import Image


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def diagnostic_identity(raw):
    match = re.fullmatch(rb'((?:cards|functions)/[^:\n\r\x00]+\.(?:c|cpp)):([1-9][0-9]*)\n([^\x00]+)', raw)
    assert match, 'Not a source assertion diagnostic'
    return match[1], int(match[2]), match[3]


def diagnostic_pair(original, rebuilt):
    a, b = diagnostic_identity(original), diagnostic_identity(rebuilt)
    assert (a[0], a[2]) == (b[0], b[2]), 'Diagnostic path or expression changed'
    assert a[1] != b[1], 'Diagnostic line did not change'
    return dict(source=a[0].decode('ascii'), native_line=a[1], rebuilt_line=b[1],
                expression=a[2].decode('ascii'))


def string_at(image, address):
    rva = address - image.pe.OPTIONAL_HEADER.ImageBase
    section = image.pe.get_section_by_rva(rva)
    assert section is not None and not section.Characteristics & 0x20000000
    available = section.VirtualAddress + min(section.Misc_VirtualSize, section.SizeOfRawData) - rva
    assert available > 0
    data = image.pe.get_data(rva, min(available, 8192))
    end = data.find(b'\0')
    assert end >= 0, 'Unterminated diagnostic'
    return data[:end]


def check_rendering(discovery, current, allow_line_drift=False):
    if discovery == current:
        return None
    assert allow_line_drift, 'Discovery diagnostic changed'
    pattern = r'(mov dword ptr \[(?:esp|ebp \+ 8)\], "(?:cards|functions)/[^:"]+\.(?:c|cpp)):([1-9][0-9]*)(\\n.*" \(STRING\))'
    before, after = re.fullmatch(pattern, discovery), re.fullmatch(pattern, current)
    assert before and after and (before[1], before[3]) == (after[1], after[3]), 'Discovery diagnostic changed beyond its decimal line'
    return dict(discovery_line=int(before[2]), current_line=int(after[2]))


def check_site(image, code, address, discovery_text, *, allow_line_drift=False):
    inst = code[address]
    assert inst.mnemonic == 'mov' and len(inst.operands) == 2
    dest, source = inst.operands
    assert dest.type == X86_OP_MEM and dest.size == 4
    destination = inst.reg_name(dest.mem.base), dest.mem.disp
    assert not dest.mem.index and destination in (('esp', 0), ('ebp', 8))
    assert source.type == X86_OP_IMM and inst.imm_size == 4
    raw = string_at(image, source.imm)
    # Use reccmp's literal rendering, including escaped assertion quotes.
    quoted = raw.decode('ascii').encode('unicode_escape').decode('ascii').replace('"', '\\"')
    memory = '[esp]' if destination == ('esp', 0) else '[ebp + 8]'
    drift = check_rendering(discovery_text, 'mov dword ptr ' + memory + ', "' + quoted + '" (STRING)', allow_line_drift)
    encoding = bytes(inst.bytes[:inst.imm_offset] + inst.bytes[inst.imm_offset + inst.imm_size:])
    return raw, encoding, dict(instruction=hex(address), instruction_bytes=bytes(inst.bytes).hex(),
                               string_address=hex(source.imm), string_sha256=hashlib.sha256(raw).hexdigest(),
                               discovery_line_drift=drift)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--queue', required=True, type=Path)
    parser.add_argument('--build-evidence', required=True, type=Path)
    parser.add_argument('--output', required=True, type=Path)
    args = parser.parse_args()
    queue = json.loads(args.queue.read_bytes())
    report_path = Path(queue['report'])
    receipt_path = report_path.with_suffix('.inputs.json')
    receipt = json.loads(receipt_path.read_bytes())
    metadata_path = Path(receipt['metadata_snapshot'])
    assert digest(report_path) == receipt['report_sha256'] == queue['report_sha256']
    assert digest(receipt_path) == queue['receipt_sha256']
    assert digest(metadata_path) == receipt['metadata_sha256']
    metadata = {r['address']: r for r in csv.DictReader(metadata_path.open(newline='', encoding='utf-8'))}
    report = {r['address']: r for r in json.loads(report_path.read_bytes())['data']}
    rows = [r for r in queue['functions'] if r['review_pattern'] == 'assertion_line_numbers']
    for row in rows:
        original = report[row['address']]
        changes = [dict(original=b.get('orig', []), rebuilt=b.get('recomp', []))
                   for _, blocks in original['diff'] for b in blocks if 'both' not in b]
        assert original['name'] == row['name'] and changes == row['changes']
        assert metadata[row['address']]['source'] == row['source']
        assert int(metadata[row['address']]['size']) == row['native_size']

    paths = ['.recovery/2022-ManalinkEh.dll', 'ManalinkEh.dll',
             'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg', 'ManalinkEh.dbg']
    inputs = [Path(p) for p in paths]
    inputs += [args.queue, report_path, receipt_path, metadata_path, args.build_evidence, Path(__file__),
               Path(__file__).with_name('inspect_release.py'), Path(capstone.__file__), Path(capstone._cs._name)]
    inputs += [Path(r['source']) for r in rows]
    state = {str(p): digest(p) for p in inputs}
    assert state[str(Path(paths[0]))] == '52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert state[str(Path(paths[2]))] == '7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    build = json.loads(args.build_evidence.read_bytes())['compiled_verification']
    assert build['build'] == 'passed' and build['fixed_entry_slots'] == 15169
    assert state[paths[1]] == build['dll_sha256']
    assert state[paths[3]] == build.get('debug_sha256', build.get('dbg_sha256'))

    images = [Image(Path(p).read_bytes()) for p in paths[:2]]
    tables = []
    for path in paths[2:]:
        debug = Image(Path(path).read_bytes())
        table = defaultdict(set)
        for name, offset, section, kind, storage in debug.symbols:
            if section > 0 and kind == 32:
                table[name.removeprefix('_')].add(0x2000000 + debug.pe.sections[section - 1].VirtualAddress + offset)
        tables.append(table)
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    reviewed, unresolved = [], []
    for row in rows:
        try:
            sites = [[], []]
            native_base = int(row['address'], 16)
            identities = [name for name, entries in tables[0].items() if native_base in entries]
            assert len(identities) == 1, 'Missing or ambiguous native COFF entry'
            identity = identities[0]
            assert re.sub(r'\.(?:part|constprop|isra)\.[0-9]+$', '', identity) == row['native_name']
            for side in (0, 1):
                entries = tables[side][identity]
                assert len(entries) == 1, 'Missing or ambiguous function identity'
                base = next(iter(entries))
                if side == 0:
                    assert base == int(row['address'], 16)
                next_entry = min(address for entries in tables[side].values() for address in entries if address > base)
                size = min(row['native_size'], next_entry - base)
                raw = images[side].pe.get_data(base - 0x2000000, size)
                code = {i.address: i for i in md.disasm(raw, base)}
                discovery_base = int(row['address' if side == 0 else 'recomp'], 16)
                for block in row['changes']:
                    assert len(block['original']) == len(block['rebuilt']) > 0
                    for address, text in block['original' if side == 0 else 'rebuilt']:
                        sites[side].append(check_site(images[side], code, base + int(address, 16) - discovery_base, text,
                                                      allow_line_drift=side == 1))
            checked = []
            for a, b in zip(*sites):
                assert a[1] == b[1], 'Instruction changed beyond its string pointer'
                pair = diagnostic_pair(a[0], b[0])
                assert pair['source'] == row['source']
                checked.append(dict(**pair, native=a[2], rebuilt=b[2]))
            assert checked
            reviewed.append(dict(name=row['name'], address=row['address'], source=row['source'],
                                 coff_identity=identity, discovery_changes=row['changes'], diagnostic_sites=len(checked), sites=checked))
        except (AssertionError, KeyError, ValueError) as error:
            unresolved.append(dict(name=row['name'], reason=str(error)))
    assert all(digest(p) == expected for p, expected in state.items()), 'Input changed during review'
    result = dict(format=1, reviewed=reviewed, unresolved=unresolved, inputs=state,
                  discovery_report_sha256=digest(report_path), script_sha256=digest(__file__),
                  decoder_version=capstone.__version__,
                  scope='Only the listed assertion-location strings and pointer-store instructions. '
                        'Paths and complete assertion expressions agree; decimal source lines differ. '
                        'Diagnostic output on the assertion path therefore differs intentionally. '
                        'No claim about other instructions, whole handlers, gameplay, or byte identity. '
                        'Source, binary, debug and successful-build inputs are pinned. Current rebuilt discovery strings '
                        'may drift only in their decimal source line, with the old/new lines recorded; native discovery '
                        'text remains exact. Other stale sites remain unresolved.')
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(dict(reviewed=len(reviewed), sites=sum(r['diagnostic_sites'] for r in reviewed), unresolved=unresolved), indent=2))


if __name__ == '__main__':
    main()
