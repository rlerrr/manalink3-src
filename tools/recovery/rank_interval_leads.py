"""Find differing local field/return interval checks; emit leads, never proofs.

Tracks affine field loads and optional named-call results in one basic block. Register allocation,
branch orientation and full-width versus low-byte comparisons are normalized.
Field offsets alone do not identify records or matching execution paths, so every
reported difference still requires source and native instruction inspection.
"""
import argparse
import collections
import csv
import json
import re
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM, X86_OP_MEM, X86_OP_REG

from inspect_release import Image
from unify_scalar_helpers import read, sha, mask


REGISTERS = {
    n: (full, width) for full, names in [
        ('eax', ['eax', 'ax', 'al']), ('ebx', ['ebx', 'bx', 'bl']),
        ('ecx', ['ecx', 'cx', 'cl']), ('edx', ['edx', 'dx', 'dl']),
        ('esi', ['esi', 'si']), ('edi', ['edi', 'di'])]
    for n, width in zip(names, [32, 16, 8])
}
CONDITIONS = {'ja', 'jae', 'jb', 'jbe', 'je', 'jne', 'jg', 'jge', 'jl', 'jle', 'js', 'jns'}
HIGH_REGISTERS = {'ah': 'eax', 'bh': 'ebx', 'ch': 'ecx', 'dh': 'edx'}


def register(ins, operand):
    return REGISTERS.get(ins.reg_name(operand.reg)) if operand.type == X86_OP_REG else None


def field(ins, operand, include_dwords=False):
    if operand.type != X86_OP_MEM or operand.size not in ((1, 2, 4) if include_dwords else (1, 2)):
        return None
    m = operand.mem
    if m.index or not m.base or ins.reg_name(m.base) in ('esp', 'ebp') or not 0 <= m.disp <= 0x400:
        return None
    return m.disp, operand.size * 8


def signed(value, bits):
    return value - (1 << bits) if value & (1 << (bits - 1)) else value


def outcomes(expression, width, immediate, condition):
    offset, bits, extension, delta, known = expression
    if offset is None or width > known or (bits == 32 and width < 32):
        return None
    modulus = 1 << width
    limit = modulus - 1
    domain = 1 << bits
    rhs = immediate & limit
    def test(value):
        lhs = signed(value, bits) if extension == 'signed' else value
        lhs = (lhs + delta) & limit
        a, b = (signed(lhs, width), signed(rhs, width)) if condition in {'jg', 'jge', 'jl', 'jle'} else (lhs, rhs)
        sign = signed((lhs - rhs) & limit, width)
        return {'ja': a > b, 'jae': a >= b, 'jb': a < b, 'jbe': a <= b,
                'je': a == b, 'jne': a != b, 'jg': a > b, 'jge': a >= b,
                'jl': a < b, 'jle': a <= b, 'js': sign < 0, 'jns': sign >= 0}[condition]
    # Between wrap, sign and comparison boundaries the predicate is constant.
    # Enumerate boundaries, rather than all 65,536 possible word values per site.
    boundaries = {0, domain}
    regions = [(0, domain, 0)] if extension != 'signed' else [(0, domain // 2, 0), (domain // 2, domain, -domain)]
    for lo, hi, extension_delta in regions:
        boundaries.update([lo, hi])
        shift = delta + extension_delta
        for target in [0, rhs, rhs + 1, modulus // 2, modulus, (rhs + modulus // 2) & limit]:
            first = (lo + shift - target) // modulus - 1
            last = (hi + shift - target) // modulus + 1
            for multiple in range(first, last + 1):
                value = multiple * modulus + target - shift
                if lo <= value <= hi:
                    boundaries.add(value)
    true_ranges, false_ranges = [], []
    ordered = sorted(boundaries)
    for lo, end in zip(ordered, ordered[1:]):
        ranges = true_ranges if test(lo) else false_ranges
        if ranges and ranges[-1][1] + 1 == lo:
            ranges[-1][1] = end - 1
        else:
            ranges.append([lo, end - 1])
    # A condition and its inverse describe the same partition, not the same edge.
    partition = min([true_ranges, false_ranges], key=lambda ranges: (sum(hi - lo + 1 for lo, hi in ranges), ranges))
    return offset, bits, partition


def intervals(raw, base=0x1000, callees=None, include_dwords=False):
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    instructions = list(md.disasm(raw, base))
    if sum(i.size for i in instructions) != len(raw):
        return None
    labels = {i.operands[0].imm for i in instructions if i.mnemonic.startswith('j') and
              i.operands and i.operands[0].type == X86_OP_IMM}
    state = {}
    comparison = None
    found = []
    for ins in instructions:
        if ins.address in labels:
            state.clear()
            comparison = None
        ops = ins.operands
        mnemonic = ins.mnemonic
        condition = mnemonic if mnemonic in CONDITIONS else (
            'j' + mnemonic[3:] if mnemonic.startswith('set') else
            'j' + mnemonic[4:] if mnemonic.startswith('cmov') else
            'jb' if mnemonic in {'sbb', 'adc'} else None)
        if condition in CONDITIONS and comparison:
            expression, width, immediate, at, allowed = comparison
            result = outcomes(expression, width, immediate, condition) if condition in allowed else None
            if result and result[2]:
                origin = hex(result[0]) if isinstance(result[0], int) else result[0]
                found.append(dict(origin=origin, field_offset=origin, field_bits=result[1], partition=result[2],
                                  compare_address=hex(at), branch_address=hex(ins.address),
                                  branch=mnemonic))
        if mnemonic.startswith('j') or mnemonic.startswith('ret') or mnemonic == 'call':
            state.clear()
            comparison = None
            if mnemonic == 'call' and callees and ops and ops[0].type == X86_OP_IMM:
                callee = callees.get(ops[0].imm)
                if callee:
                    state['eax'] = ('return:' + callee, 32, 'unsigned', 0, 32)
            continue
        if mnemonic == 'cmp' and len(ops) == 2 and ops[1].type == X86_OP_IMM:
            reg = register(ins, ops[0])
            offset = field(ins, ops[0], include_dwords)
            expression = state.get(reg[0]) if reg else None
            if offset is not None:
                expression = (*offset, 'unsigned', 0, offset[1])
            comparison = (expression, ops[0].size * 8, ops[1].imm, ins.address, CONDITIONS) if expression else None
            continue
        if mnemonic == 'test' and len(ops) == 2 and register(ins, ops[0]) == register(ins, ops[1]):
            reg = register(ins, ops[0])
            expression = state.get(reg[0]) if reg else None
            comparison = (expression, ops[0].size * 8, 0, ins.address, CONDITIONS) if expression else None
            continue
        if 'eflags' in [ins.reg_name(r) for r in ins.regs_access()[1]]:
            comparison = None
        written = {REGISTERS.get(ins.reg_name(r), (HIGH_REGISTERS.get(ins.reg_name(r), ''), 0))[0]
                   for r in ins.regs_access()[1]}
        dest = register(ins, ops[0]) if ops else None
        update = None
        if dest:
            full, width = dest
            previous = state.get(full)
            if mnemonic in {'xor', 'sub'} and len(ops) == 2 and register(ins, ops[1]) == dest:
                update = (None, 0, 'unsigned', 0, width)
            elif mnemonic in {'mov', 'movzx', 'movsx'} and len(ops) == 2:
                offset = field(ins, ops[1], include_dwords)
                src = register(ins, ops[1])
                if offset is not None:
                    extension = 'signed' if mnemonic == 'movsx' else 'unsigned'
                    known = width if mnemonic != 'mov' else offset[1]
                    if mnemonic == 'mov' and previous == (None, 0, 'unsigned', 0, 32):
                        known = 32
                    update = (*offset, extension, 0, known)
                elif src and src[0] in state:
                    expression = state[src[0]]
                    update = (*expression[:4], min(width, expression[4], src[1]))
                elif ops[1].type == X86_OP_IMM and ops[1].imm == 0:
                    update = (None, 0, 'unsigned', 0, width)
            elif previous and mnemonic in {'dec', 'inc', 'add', 'sub'}:
                amount = -1 if mnemonic == 'dec' else 1 if mnemonic == 'inc' else None
                if len(ops) == 2 and ops[1].type == X86_OP_IMM:
                    amount = ops[1].imm * (-1 if mnemonic == 'sub' else 1)
                if amount is not None:
                    update = (*previous[:3], previous[3] + amount, min(width, previous[4]))
            elif mnemonic == 'lea' and len(ops) == 2 and ops[1].type == X86_OP_MEM:
                m = ops[1].mem
                src = REGISTERS.get(ins.reg_name(m.base))
                if src and not m.index and src[0] in state:
                    expression = state[src[0]]
                    update = (*expression[:3], expression[3] + m.disp, min(width, expression[4]))
            # Instructions not modeled above cannot preserve a tracked destination.
        for r in written:
            state.pop(r, None)
        if update:
            state[dest[0]] = update
            if mnemonic in {'inc', 'dec'}:
                comparison = (update, dest[1], 0, ins.address, {'je', 'jne', 'js', 'jns'})
    return found


def site_signature(site, code_targets):
    partition = tuple(map(tuple, site['partition']))
    # Callback addresses move between builds. Only normalize equality uses of
    # a uniquely mapped function entry; retain the raw address in the packet.
    if (site['field_bits'] == 32 and site['branch'] in
            {'je', 'jne', 'sete', 'setne', 'cmove', 'cmovne'} and
            len(partition) == 1 and partition[0][0] == partition[0][1]):
        identity = code_targets.get(partition[0][0])
        if identity:
            partition = ('function', identity)
    return site['field_offset'], site['field_bits'], partition


def self_test():
    # Real instruction forms from the C737 native/before/final death-count guard.
    native = bytes.fromhex('31d28a506889d04883f8027710')
    before = bytes.fromhex('807868037710')
    final = bytes.fromhex('8a4068483c027710')
    # Include both real native zeroing and an equivalent movzx load.
    modeled_native = bytes.fromhex('0fb640684883f8027710')
    assert intervals(modeled_native)[0]['partition'] == [[1, 3]]
    assert intervals(before)[0]['partition'] == [[0, 3]]
    assert intervals(final)[0]['partition'] == [[1, 3]]
    assert intervals(native)[0]['partition'] == [[1, 3]]
    assert intervals(bytes.fromhex('0fb640684883f8027610'))[0]['partition'] == [[1, 3]]
    assert intervals(bytes.fromhex('0fb64068e80000000083f8027710')) == []
    assert intervals(bytes.fromhex('0fb6406883f80283c0017710')) == []
    assert intervals(bytes.fromhex('807d68037710')) == []  # frame slots are not record fields
    assert intervals(bytes.fromhex('8a40684883f8027710')) == []  # unknown high bits
    assert intervals(bytes.fromhex('0fb64068b40183f8027710')) == []  # AH invalidates EAX
    assert intervals(bytes.fromhex('0fb740684883f8027710'))[0]['partition'] == [[1, 3]]
    assert intervals(bytes.fromhex('66837868037710'))[0]['partition'] == [[0, 3]]
    # Check computed boundaries against exhaustive, independent word predicates.
    for delta in [-65537, -17, -1, 0, 1, 65535, 65537]:
        result = outcomes((0x68, 16, 'unsigned', delta, 32), 32, 2, 'ja')[2]
        actual = {value for lo, hi in result for value in range(lo, hi + 1)}
        expected = {value for value in range(65536) if ((value + delta) & 0xffffffff) <= 2}
        if len(expected) > 32768:
            expected = set(range(65536)) - expected
        assert actual == expected
    # Real forms used for nonnegative tutor success versus the old != -1 check.
    callee = {0x1005: 'tutor'}
    native_result = intervals(bytes.fromhex('e8000000003d0000008019d2'), callees=callee)
    wrong_result = intervals(bytes.fromhex('e800000000400f95c0'), callees=callee)
    assert native_result[0]['partition'] == [[0, 0x7fffffff]]
    assert wrong_result[0]['partition'] == [[0xffffffff, 0xffffffff]]
    assert intervals(bytes.fromhex('e80000000085c07810'), callees=callee)[0]['partition'] == [[0, 0x7fffffff]]
    assert intervals(bytes.fromhex('e80000000085c00f95c0'), callees=callee)[0]['partition'] == [[0, 0]]
    # Dword fields are opt-in. Full-width signed comparisons must retain the
    # high-bit partition, and subtraction must wrap before an unsigned check.
    dword = bytes.fromhex('8b40684883f8027710')
    assert intervals(dword) == []
    assert intervals(dword, include_dwords=True)[0]['partition'] == [[1, 3]]
    assert intervals(bytes.fromhex('837868007c10'), include_dwords=True)[0]['partition'] == [[0, 0x7fffffff]]
    assert intervals(bytes.fromhex('837868007710'), include_dwords=True)[0]['partition'] == [[0, 0]]
    assert intervals(bytes.fromhex('8b406885c07810'), include_dwords=True)[0]['partition'] == [[0, 0x7fffffff]]
    pointer = dict(field_offset='0x38', field_bits=32, partition=[[0x1234, 0x1234]], branch='jne')
    assert site_signature(pointer, {0x1234: 'callback'})[2] == ('function', 'callback')
    assert site_signature(pointer, {})[2] == ((0x1234, 0x1234),)
    assert site_signature(dict(pointer, branch='js'), {0x1234: 'callback'})[2] == ((0x1234, 0x1234),)
    assert site_signature(dict(pointer, partition=[[0x1234, 0x1235]]), {0x1234: 'callback'})[2] == ((0x1234, 0x1235),)
    return 32


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--report', type=Path)
    parser.add_argument('--metadata', type=Path)
    parser.add_argument('--output', type=Path)
    parser.add_argument('--self-test', action='store_true')
    parser.add_argument('--include-returns', action='store_true')
    parser.add_argument('--include-dwords', action='store_true')
    args = parser.parse_args()
    if args.self_test:
        print(json.dumps(dict(focused_checks=self_test(), scope='Pattern extraction and rejection checks, not gameplay tests.')))
        return
    if not all([args.report, args.metadata, args.output]):
        parser.error('--report, --metadata and --output are required')
    receipt = json.loads(args.report.with_suffix('.inputs.json').read_bytes())
    assert receipt['exit_code'] == 0 and sha(args.report.read_bytes()) == receipt['report_sha256']
    assert sha(args.metadata.read_bytes()) == receipt['metadata_sha256']
    paths = ['.recovery/2022-ManalinkEh.dll', 'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg', 'ManalinkEh.dll', 'ManalinkEh.dbg']
    binaries = [Path(p).read_bytes() for p in paths]
    input_hashes = {p: sha(raw) for p, raw in zip(paths, binaries)}
    receipt_inputs = {Path(p).resolve(): digest for p, digest in receipt['inputs'].items()}
    for p in paths[:2]:
        assert input_hashes[p] == receipt_inputs[Path(p).resolve()], p
    report_inputs_current = all(sha(Path(p).read_bytes()) == digest for p, digest in receipt['inputs'].items())
    native, _, rebuilt, debug = [Image(raw) for raw in binaries]
    symbols = collections.defaultdict(set)
    for name, offset, section, kind, storage in debug.symbols:
        if section > 0 and kind == 32:
            symbols[name].add(0x2000000 + debug.pe.sections[section - 1].VirtualAddress + offset)
    starts = sorted({a for addresses in symbols.values() for a in addresses})
    ends = dict(zip(starts, starts[1:]))
    meta = {r['address']: r for r in csv.DictReader(args.metadata.open(encoding='utf-8'))}
    code_targets = [collections.defaultdict(set), collections.defaultdict(set)]
    for m in meta.values():
        if len(symbols['_' + m['name']]) == 1:
            code_targets[0][int(m['address'], 0)].add(m['name'])
            code_targets[1][next(iter(symbols['_' + m['name']]))].add(m['name'])
    code_targets = [{address: next(iter(names)) for address, names in side.items() if len(names) == 1}
                    for side in code_targets]
    return_callees = code_targets if args.include_returns else [{}, {}]
    report = json.loads(args.report.read_bytes())
    sources = {}
    counts = collections.Counter()
    leads = []
    pointer_only_leads = []
    for row in report['data']:
        if row.get('type') != 1 or row['matching'] == 1 or row.get('effective'):
            continue
        m = meta.get(row['address'], {})
        name, source = m.get('name'), m.get('source')
        if source == 'cards/odyssey.c':
            source = 'cards/odissey.c'
        if not name or not source or not Path(source).is_file() or len(symbols['_' + name]) != 1:
            counts['missing_unique_source_or_symbol'] += 1
            continue
        if source not in sources:
            text = read(source)
            sources[source] = text, mask(text), sha(Path(source).read_bytes())
        text, code, source_hash = sources[source]
        definition = re.search(r'(?m)^[ \t]*(?:[A-Za-z_]\w*[ \t\r\n*]+)+' + re.escape(name) + r'\s*\([^;{}]*\)\s*\{', code)
        if definition is None:
            counts['no_source_definition'] += 1
            continue
        depth = 1
        end = definition.end()
        while end < len(code) and depth:
            depth += (code[end] == '{') - (code[end] == '}')
            end += 1
        assert depth == 0, name
        original_base = int(row['address'], 0)
        rebuilt_base = next(iter(symbols['_' + name]))
        if rebuilt_base not in ends:
            counts['no_rebuilt_end'] += 1
            continue
        raws = [native.pe.get_data(original_base - 0x2000000, int(m['size'])),
                rebuilt.pe.get_data(rebuilt_base - 0x2000000, ends[rebuilt_base] - rebuilt_base)]
        found = [intervals(raw, base, callees, args.include_dwords) for raw, base, callees in zip(raws, [original_base, rebuilt_base], return_callees)]
        if None in found:
            counts['incomplete_disassembly'] += 1
            continue
        counts['scanned_functions'] += 1
        counts['native_interval_sites'] += len(found[0])
        counts['rebuilt_interval_sites'] += len(found[1])
        counts['functions_with_recognized_sites'] += bool(found[0] or found[1])
        signatures = [{(r['field_offset'], r['field_bits'], tuple(map(tuple, r['partition']))) for r in side} for side in found]
        differing = [signatures[0] - signatures[1], signatures[1] - signatures[0]]
        offsets = [{(p[0], p[1]) for p in side} for side in differing]
        if not offsets[0] & offsets[1]:
            continue
        normalized = [{site_signature(site, targets) for site in side}
                      for side, targets in zip(found, code_targets)]
        normalized_differences = [normalized[0] - normalized[1], normalized[1] - normalized[0]]
        normalized_offsets = [{signature[:2] for signature in side} for side in normalized_differences]
        remaining_offsets = normalized_offsets[0] & normalized_offsets[1]
        destination = leads if remaining_offsets else pointer_only_leads
        destination.append(dict(address=row['address'], name=name, source=source,
                          source_definition_sha256=sha(text[definition.start():end].encode()),
                          source_file_sha256=source_hash,
                          native_bytes_sha256=sha(raws[0]), rebuilt_bytes_sha256=sha(raws[1]),
                          native_intervals=found[0], rebuilt_intervals=found[1],
                          raw_differing_field_offsets=sorted(offsets[0] & offsets[1]),
                          differing_field_offsets=sorted(remaining_offsets)))
    for p, digest in input_hashes.items():
        assert sha(Path(p).read_bytes()) == digest, f'Binary/debug input changed during scan: {p}'
    for source, (_, _, digest) in sources.items():
        assert sha(Path(source).read_bytes()) == digest, f'Source changed during scan: {source}'
    output = dict(format=1, inputs=input_hashes,
                  report=str(args.report), report_sha256=sha(args.report.read_bytes()),
                  report_receipt_sha256=sha(args.report.with_suffix('.inputs.json').read_bytes()),
                  report_build_inputs_current=report_inputs_current,
                  source_snapshot_hashes={p: value[2] for p, value in sources.items()},
                  metadata_sha256=sha(args.metadata.read_bytes()), generator_sha256=sha(Path(__file__).read_bytes()),
                  summary=dict(counts), candidates=len(leads), functions=leads,
                  normalized_pointer_only_candidates=len(pointer_only_leads),
                  normalized_pointer_only_functions=pointer_only_leads,
                  includes_named_return_predicates=args.include_returns,
                  includes_dword_fields=args.include_dwords,
                  scope='Heuristic differing local byte/word-field, optional dword-field and optional full-width named-call-result partitions on live binaries, using historical report rows only to select non-exact functions. Offset identity, call arguments, path alignment, record type and condition-use meaning require manual inspection. Function-local partition sets ignore counts/orientation, so they do not prove matching paths. No equivalence or coverage claim for skipped patterns/functions; no game-source or comparator edits.')
    args.output.write_text(json.dumps(output, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(dict(summary=dict(counts), candidates=len(leads))))
    for r in leads[:30]:
        print(r['name'], r['source'], r['differing_field_offsets'])


if __name__ == '__main__':
    main()
