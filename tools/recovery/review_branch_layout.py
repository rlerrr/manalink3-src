"""Compare decoded CFGs while ignoring padding and internal jump trampolines.

Address identities come from pinned function metadata, exact COFF names and PE
imports. Matched callees/data remain correspondence assumptions, not proofs of
their behavior or contents. Unsupported instruction differences stay unresolved.
"""
import argparse
from collections import defaultdict
import csv
import hashlib
import json
from pathlib import Path
import re
import sys

import capstone
from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_GRP_JUMP
from capstone.x86_const import X86_OP_IMM, X86_OP_MEM, X86_OP_REG
from inspect_release import Image
from review_event_guards import flags_dead, public_cdecl, roots
from pointer_return_tail import compare_pointer_tails, native_pointer_contracts

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from reccmp_project import cached_functions


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


INVERSE = dict(zip(
    ('je', 'jne', 'ja', 'jbe', 'jae', 'jb', 'jg', 'jle', 'jge', 'jl', 'js', 'jns', 'jo', 'jno', 'jp', 'jnp'),
    ('jne', 'je', 'jbe', 'ja', 'jb', 'jae', 'jle', 'jg', 'jl', 'jge', 'jns', 'js', 'jno', 'jo', 'jnp', 'jp')))


def padding(inst):
    return (inst.mnemonic == 'nop' or (inst.mnemonic == 'lea' and inst.op_str in
            ('esi, [esi]', 'edi, [edi]', 'eax, [eax]', 'ebx, [ebx]', 'ecx, [ecx]', 'edx, [edx]')))


def literal_value(image, address, dialog_codes):
    rva = address - image.pe.OPTIONAL_HEADER.ImageBase
    section = image.pe.get_section_by_rva(rva) if rva >= 0 else None
    if section is None or section.Characteristics & 0xa0000000:
        return None
    available = section.VirtualAddress + min(section.Misc_VirtualSize, section.SizeOfRawData) - rva
    if available <= 0:
        return None
    data = image.pe.get_data(rva, min(available, 8192))
    end = data.find(b'\0')
    if end <= 0:
        return None
    value = data[:end]
    if len(value) == 2 and value[0] == 1 and value[1] in dialog_codes:
        return 'DIALOG-token', value
    if all(c in (9, 10, 13) or 32 <= c < 127 for c in value):
        return 'C-string', value
    return None


def eax_dead(code, start, symbols, *, void_return=False):
    """Require every path to overwrite/clobber EAX before reading any of it."""
    visiting, proven = set(), set()

    def walk(pc):
        if pc in proven:
            return True
        if pc in visiting or len(visiting) > 256:
            return False
        inst = code.get(pc)
        if inst is None:
            return False
        if inst.mnemonic == 'ret':
            return void_return and not inst.operands and not any(inst.prefix)
        if inst.mnemonic == 'call':
            return public_cdecl(inst, symbols)
        visiting.add(pc)
        reads, writes = inst.regs_access()
        zero = (inst.mnemonic in ('xor', 'sub') and len(inst.operands) == 2
                and all(op.type == X86_OP_REG for op in inst.operands)
                and inst.operands[0].reg == inst.operands[1].reg)
        if not zero and any(roots.get(inst.reg_name(r), ('',))[0] == 'eax' for r in reads):
            visiting.remove(pc)
            return False
        if any(inst.reg_name(r) == 'eax' for r in writes):
            ok = True
        elif inst.group(CS_GRP_JUMP):
            ok = inst.operands[0].type == X86_OP_IMM and walk(inst.operands[0].imm)
            if inst.mnemonic != 'jmp':
                ok = ok and walk(pc + inst.size)
        else:
            ok = walk(pc + inst.size)
        visiting.remove(pc)
        if ok:
            proven.add(pc)
        return ok

    return walk(start)


def native_void_declaration(die, seen=None):
    """C DWARF omits the return type for void; follow every inherited declaration."""
    seen = set() if seen is None else set(seen)
    if die.offset in seen or die.tag != 'DW_TAG_subprogram' or 'DW_AT_type' in die.attributes:
        return False
    seen.add(die.offset)
    inherited = [die.get_DIE_from_attribute(name) for name in
                 ('DW_AT_abstract_origin', 'DW_AT_specification') if name in die.attributes]
    if inherited:
        return all(native_void_declaration(parent, seen) for parent in inherited)
    return 'DW_AT_name' in die.attributes


def native_void_contracts(debug, candidates):
    """Require a named native C definition with no direct or inherited result type."""
    import struct
    from inspect_release import inherited_value, value
    wanted = {int(row['address'], 16): row for row in candidates}
    sources = {row['source'] for row in candidates}
    dwarf = debug.dwarf()
    data = debug.section_data(debug.sections['.debug_info'])
    offset, found = 0, {}
    while offset + 4 <= len(data):
        length = struct.unpack_from('<I', data, offset)[0]
        if length == 0:
            break
        assert length != 0xffffffff, 'DWARF64 is unsupported'
        cu = dwarf.get_CU_at(offset)
        offset += length + 4
        top = cu.get_top_DIE()
        source = value(top, 'DW_AT_name', '').replace('\\', '/').replace('cards/odyssey.c', 'cards/odissey.c')
        if (source not in sources or not source.endswith('.c') or cu['address_size'] != 4
                or value(top, 'DW_AT_language') not in (1, 2, 12, 29)):
            continue
        for die in cu.iter_DIEs():
            if die.tag != 'DW_TAG_subprogram':
                continue
            address = value(die, 'DW_AT_low_pc')
            row = wanted.get(address)
            if row is None or row['source'] != source:
                continue
            name = inherited_value(die, 'DW_AT_name')
            if (name != row['native_name'] or value(die, 'DW_AT_declaration', False)
                    or inherited_value(die, 'DW_AT_calling_convention', 1) != 1
                    or not native_void_declaration(die)):
                continue
            assert address not in found, 'Ambiguous native void contract'
            found[address] = dict(source=source, native_name=name,
                                  function_die=hex(die.offset), result='void',
                                  language=value(top, 'DW_AT_language'), calling_convention=1)
    return found


def equality_load_guard(code, inst):
    """Recognize only EAX loads comparing an ordinary global with a C parameter."""
    if inst.mnemonic != 'mov' or inst.op_str.split(',', 1)[0] != 'eax':
        return None
    cmp = code.get(inst.address + inst.size)
    if cmp is None or cmp.mnemonic != 'cmp' or len(cmp.operands) != 2:
        return None
    if not (len(inst.operands) == 2 and inst.operands[1].type == X86_OP_MEM
            and cmp.operands[0].type == X86_OP_MEM and cmp.operands[1].type == X86_OP_REG
            and cmp.reg_name(cmp.operands[1].reg) == 'eax'):
        return None
    branch = code.get(cmp.address + cmp.size)
    if branch is None or branch.mnemonic not in ('je', 'jne') or branch.operands[0].type != X86_OP_IMM:
        return None
    if any(any(i.prefix) for i in (inst, cmp, branch)):
        return None
    operands = (inst.operands[1], cmp.operands[0])
    if any(op.size != 4 or op.mem.segment or op.mem.index for op in operands):
        return None
    kinds = []
    for op in operands:
        base = inst.reg_name(op.mem.base)
        if not base and op.mem.disp >= 0x400000:
            kinds.append('global')
        elif base == 'ebp' and op.mem.disp in (8, 12):
            kinds.append('parameter')
        else:
            return None
    if set(kinds) != {'global', 'parameter'}:
        return None
    return operands, kinds, branch


def zero_return_epilogue(code, start, base, size):
    """Recognize a zero return among otherwise identical stack restores."""
    pc, visited, signature, clear = start, [], [], None
    for _ in range(32):
        if pc in visited or not base <= pc < base + size:
            return None
        inst = code.get(pc)
        if inst is None:
            return None
        visited.append(pc)
        if padding(inst):
            pc += inst.size
            continue
        if inst.mnemonic == 'jmp' and inst.operands[0].type == X86_OP_IMM:
            pc = inst.operands[0].imm
            continue
        if any(inst.prefix):
            return None
        if inst.mnemonic == 'xor' and inst.op_str == 'eax, eax':
            if clear is not None:
                return None
            clear = (pc, len(signature))
        elif (inst.mnemonic == 'pop' and len(inst.operands) == 1
              and inst.operands[0].type == X86_OP_REG and inst.operands[0].size == 4
              and inst.op_str in ('ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp')):
            signature.append(('pop', inst.op_str))
        elif (inst.mnemonic == 'add' and len(inst.operands) == 2
              and inst.operands[0].type == X86_OP_REG and inst.op_str.startswith('esp, ')
              and inst.operands[0].size == 4 and inst.operands[1].type == X86_OP_IMM
              and inst.operands[1].imm > 0 and inst.operands[1].imm % 4 == 0):
            signature.append(('add', inst.op_str))
        elif inst.mnemonic == 'leave' and not inst.operands:
            signature.append(('leave', ''))
        elif inst.mnemonic == 'ret' and not inst.operands:
            if clear is None:
                return None
            signature.append(('ret', ''))
            return dict(signature=signature, visited=visited,
                        clear_address=clear[0], clear_position=clear[1])
        else:
            return None
        pc += inst.size
    return None


def compare_cfg(codes, bases, sizes, correspondence, *, equality_guards=False, public_symbols=None,
                return_epilogues=False, instruction_pairs=None, pointer_return_tails=False,
                void_return=False):
    assert not equality_guards or public_symbols is not None, 'Equality review needs call ABI identities'
    pending = [(bases[0], bases[1])]
    seen, reached = set(), [set(), set()]
    references = set()
    terminals = 0
    equality_reviews = []
    return_reviews = []
    pointer_reviews = []

    def local(side, address):
        return bases[side] <= address < bases[side] + sizes[side]

    def advance(side, address):
        skipped = set()
        while True:
            assert address not in skipped, 'Pure trampoline cycle'
            skipped.add(address)
            inst = codes[side].get(address)
            assert inst is not None and local(side, address), ('CFG outside function', side, hex(address))
            reached[side].add(address)
            if padding(inst):
                address += inst.size
            elif (inst.mnemonic == 'jmp' and inst.operands[0].type == X86_OP_IMM
                  and local(side, inst.operands[0].imm)):
                address = inst.operands[0].imm
            else:
                return inst

    def equivalent_value(a, b, usage='value'):
        if a == b and usage == 'value':
            return
        token = correspondence(a & 0xffffffff, b & 0xffffffff, usage)
        assert token is not None, ('Unbound operand', hex(a & 0xffffffff), hex(b & 0xffffffff))
        references.add((a & 0xffffffff, b & 0xffffffff, token))

    while pending:
        a, b = pending.pop()
        if (a, b) in seen:
            continue
        seen.add((a, b))
        assert len(seen) < 100000, 'Unbounded CFG product'
        i, j = advance(0, a), advance(1, b)
        assert tuple(i.prefix) == tuple(j.prefix), 'Instruction prefix changed'
        if pointer_return_tails:
            try:
                proof = compare_pointer_tails(codes, [i.address, j.address], bases, sizes, correspondence)
            except AssertionError:
                proof = None
            if proof is not None:
                for side, model in enumerate(proof['models']):
                    reached[side].update(model['visited'])
                references.update((int(r['native'], 16), int(r['rebuilt'], 16), r['identity'])
                                  for r in proof['address_correspondences'])
                pointer_reviews.append(dict(native=hex(i.address), rebuilt=hex(j.address), **proof))
                terminals += 1
                continue
        if return_epilogues:
            tails = [zero_return_epilogue(code, inst.address, base, size)
                     for code, inst, base, size in zip(codes, (i, j), bases, sizes)]
            if (all(tails) and tails[0]['signature'] == tails[1]['signature']
                    and tails[0]['clear_position'] != tails[1]['clear_position']):
                for side, tail in enumerate(tails):
                    reached[side].update(tail['visited'])
                return_reviews.append(dict(native=hex(i.address), rebuilt=hex(j.address),
                    stack_restores=tails[0]['signature'], clear_positions=[t['clear_position'] for t in tails],
                    clear_addresses=[hex(t['clear_address']) for t in tails],
                    instructions=[[hex(pc) for pc in t['visited']] for t in tails]))
                terminals += 1
                continue
        guards = [equality_load_guard(code, inst) for code, inst in zip(codes, (i, j))] if equality_guards else []
        if guards and all(guards) and guards[0][1] == guards[1][1][::-1]:
            for x, y in zip(guards[0][0], guards[1][0][::-1]):
                assert i.reg_name(x.mem.base) == j.reg_name(y.mem.base)
                equivalent_value(x.mem.disp, y.mem.disp)
            branches = [g[2] for g in guards]
            successors = [(k.operands[0].imm, k.address + k.size) for k in branches]
            for side, edges in enumerate(successors):
                reached[side].update((guards[side][2].address, i.address + i.size if side == 0 else j.address + j.size))
                # On equality both loads yield the same EAX value, and both
                # CMP orders subtract equal 32-bit values: all CMP flags agree.
                # Only the unequal edge needs scratch/flag liveness checks.
                unequal_index = 1 if branches[side].mnemonic == 'je' else 0
                for edge in edges:
                    assert local(side, edge), 'Equality edge exits function'
                unequal_edge = edges[unequal_index]
                assert eax_dead(codes[side], unequal_edge, public_symbols[side], void_return=void_return), 'Equality scratch EAX is live'
                assert flags_dead(codes[side], unequal_edge, public_symbols[side])[0], 'Equality flags are live'
            if branches[0].mnemonic != branches[1].mnemonic:
                successors[1] = successors[1][::-1]
            pending.extend(zip(successors[0], successors[1]))
            equality_reviews.append(dict(native=hex(i.address), rebuilt=hex(j.address),
                branches=[hex(k.address) for k in branches], scratch='eax',
                equal_edges=[hex(k.operands[0].imm if k.mnemonic == 'je' else k.address + k.size) for k in branches],
                unequal_edges=[hex(k.address + k.size if k.mnemonic == 'je' else k.operands[0].imm) for k in branches],
                successors=[[hex(e) for e in edges] for edges in successors]))
            continue
        if i.mnemonic == 'jmp' or j.mnemonic == 'jmp':
            assert i.mnemonic == j.mnemonic == 'jmp', 'Terminal operation changed'
            assert i.operands[0].type == j.operands[0].type == X86_OP_IMM, 'Indirect jump successors are unresolved'
        if i.group(CS_GRP_JUMP) and i.mnemonic != 'jmp':
            assert j.group(CS_GRP_JUMP) and j.mnemonic != 'jmp', 'Conditional instruction changed'
            assert i.operands[0].type == j.operands[0].type == X86_OP_IMM
            t0, t1 = i.operands[0].imm, j.operands[0].imm
            assert local(0, t0) and local(1, t1), 'Conditional edge exits function'
            if i.mnemonic == j.mnemonic:
                pending.extend([(t0, t1), (i.address + i.size, j.address + j.size)])
            else:
                assert INVERSE.get(i.mnemonic) == j.mnemonic, 'Branch condition changed'
                pending.extend([(t0, j.address + j.size), (i.address + i.size, t1)])
            if instruction_pairs is not None:
                instruction_pairs.append((i.address, j.address))
            continue
        assert i.mnemonic == j.mnemonic and len(i.operands) == len(j.operands), ('Operation changed', i.mnemonic, j.mnemonic)
        for x, y in zip(i.operands, j.operands):
            assert x.type == y.type and x.size == y.size, 'Operand type or width changed'
            if x.type == X86_OP_REG:
                assert i.reg_name(x.reg) == j.reg_name(y.reg), 'Register allocation changed'
            elif x.type == X86_OP_MEM:
                assert (i.reg_name(x.mem.segment), i.reg_name(x.mem.base), i.reg_name(x.mem.index), x.mem.scale) == (
                    j.reg_name(y.mem.segment), j.reg_name(y.mem.base), j.reg_name(y.mem.index), y.mem.scale), 'Address expression changed'
                equivalent_value(x.mem.disp, y.mem.disp, 'indirect' if i.mnemonic in ('call', 'jmp') else 'value')
            elif x.type == X86_OP_IMM:
                equivalent_value(x.imm, y.imm, 'code' if i.mnemonic in ('call', 'jmp') else 'value')
            else:
                raise AssertionError('Unsupported operand')
        if instruction_pairs is not None:
            instruction_pairs.append((i.address, j.address))
        if i.mnemonic in ('ret', 'jmp'):
            terminals += 1
        else:
            pending.append((i.address + i.size, j.address + j.size))
    assert terminals, 'No matched terminal'
    return dict(cfg_states=len(seen), reached_instruction_counts=[len(r) for r in reached], terminals=terminals,
                equality_load_guards=equality_reviews,
                zero_return_epilogues=return_reviews,
                pointer_return_tails=pointer_reviews,
                address_correspondences=[dict(native=hex(a), rebuilt=hex(b), identity=token) for a, b, token in sorted(references)])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--queue', required=True, type=Path)
    parser.add_argument('--build-evidence', required=True, type=Path)
    parser.add_argument('--output', required=True, type=Path)
    parser.add_argument('--bounds-cache', type=Path, default=Path('.recovery/reccmp/triage-rebuilt-bounds.json'))
    parser.add_argument('--equality-guards', action='store_true',
                        help='Also check swapped global/parameter equality loads with dead EAX and flags.')
    parser.add_argument('--return-epilogues', action='store_true',
                        help='Also check moved zero-return assignments among identical stack restores.')
    parser.add_argument('--pointer-return-tails', action='store_true',
                        help='Also compare read-only decision tails for source/native-verified 32-bit pointer returns.')
    parser.add_argument('--void-equality-guards', action='store_true',
                        help='Permit unobserved EAX at plain void returns, only for native/source-verified void functions.')
    args = parser.parse_args()
    if args.void_equality_guards and not args.equality_guards:
        parser.error('--void-equality-guards requires --equality-guards')
    queue = json.loads(args.queue.read_bytes())
    report_path = Path(queue['report'])
    receipt_path = report_path.with_suffix('.inputs.json')
    receipt = json.loads(receipt_path.read_bytes())
    metadata_path = Path(receipt['metadata_snapshot'])
    assert digest(report_path) == receipt['report_sha256'] == queue['report_sha256']
    assert digest(receipt_path) == queue['receipt_sha256']
    assert digest(metadata_path) == receipt['metadata_sha256']
    metadata = list(csv.DictReader(metadata_path.open(newline='', encoding='utf-8')))
    metadata_by_address = {r['address']: r for r in metadata}
    report = {r['address']: r for r in json.loads(report_path.read_bytes())['data']}
    rows = [r for r in queue['functions'] if 'change_site_review' not in r]
    for row in rows:
        original, entry = report[row['address']], metadata_by_address[row['address']]
        changes = [dict(original=b.get('orig', []), rebuilt=b.get('recomp', []))
                   for _, blocks in original['diff'] for b in blocks if 'both' not in b]
        assert original['name'] == row['name'] and changes == row['changes']
        assert entry['source'].replace('cards/odyssey.c', 'cards/odissey.c') == row['source']
        assert int(entry['size']) == row['native_size']
    paths = ['.recovery/2022-ManalinkEh.dll', 'ManalinkEh.dll',
             'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg', 'ManalinkEh.dbg']
    inputs = [Path(p) for p in paths] + [args.queue, report_path, receipt_path, metadata_path, args.build_evidence,
        Path(__file__), Path(__file__).with_name('inspect_release.py'), Path(__file__).resolve().parents[1] / 'reccmp_project.py',
        Path(__file__).with_name('review_event_guards.py'),
        Path(__file__).with_name('pointer_return_tail.py'),
        Path(capstone.__file__), Path(capstone._cs._name), Path('manalink.h')]
    inputs += [Path(r['source']) for r in rows]
    state = {str(p): digest(p) for p in inputs}
    assert state[str(Path(paths[0]))] == '52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert state[str(Path(paths[2]))] == '7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    build = json.loads(args.build_evidence.read_bytes())['compiled_verification']
    assert build['build'] == 'passed' and build['fixed_entry_slots'] == 15169
    assert state[paths[1]] == build['dll_sha256'] and state[paths[3]] == build.get('debug_sha256', build.get('dbg_sha256'))
    bounds = cached_functions(Path(paths[3]), args.bounds_cache, all_functions=True)
    state[str(args.bounds_cache)] = digest(args.bounds_cache)
    bounds_by_address, bounds_by_name = defaultdict(list), defaultdict(set)
    for row in bounds:
        source = row['source'].replace('cards/odyssey.c', 'cards/odissey.c')
        bounds_by_address[row['address']].append(row)
        bounds_by_name[source, row['name']].add(row['address'])
    images = [Image(Path(p).read_bytes()) for p in paths[:2]]
    debugs = [Image(Path(p).read_bytes()) for p in paths[2:]]
    pointer_sources = {}
    if args.pointer_return_tails:
        from canonicalize_helpers import read, span
        sources = {}
        for row in rows:
            name = re.sub(r'\([0-9]+\)$', '', row['name'])
            if not row['source'].endswith('.c') or not re.fullmatch(r'[A-Za-z_]\w*', name):
                continue
            text = sources.setdefault(row['source'], read(row['source']))
            bounds = span(text, name)
            if bounds is None:
                continue
            head = text[slice(*bounds)].split('(', 1)[0]
            if re.fullmatch(r'\s*(?:static\s+)?(?:inline\s+)?(?:const\s+)?char\s*\*\s*' + re.escape(name) + r'\s*', head):
                pointer_sources[int(row['address'], 16)] = head.strip()
    pointer_contracts = native_pointer_contracts(debugs[0], [r for r in rows if int(r['address'], 16) in pointer_sources]) if pointer_sources else {}
    void_sources = {}
    if args.void_equality_guards:
        from canonicalize_helpers import read, span
        sources = {}
        for row in rows:
            name = re.sub(r'\([0-9]+\)$', '', row['name'])
            if not row['source'].endswith('.c') or not re.fullmatch(r'[A-Za-z_]\w*', name):
                continue
            text = sources.setdefault(row['source'], read(row['source']))
            bounds = span(text, name)
            if bounds is None:
                continue
            head = text[slice(*bounds)].split('(', 1)[0]
            if re.fullmatch(r'\s*(?:static\s+)?(?:inline\s+)?void\s+' + re.escape(name) + r'\s*', head):
                void_sources[int(row['address'], 16)] = head.strip()
    void_contracts = native_void_contracts(debugs[0], [r for r in rows if int(r['address'], 16) in void_sources]) if void_sources else {}
    symbols, tokens = [], [defaultdict(set), defaultdict(set)]
    public_symbols = [defaultdict(set), defaultdict(set)]
    for side, debug in enumerate(debugs):
        names = defaultdict(set)
        for name, offset, section, kind, storage in debug.symbols:
            if section > 0 and storage in (2, 3) and kind in (0, 32) and not name.startswith('.'):
                names[name].add(0x2000000 + debug.pe.sections[section - 1].VirtualAddress + offset)
                if storage == 2 and kind == 32:
                    public_symbols[side][0x2000000 + debug.pe.sections[section - 1].VirtualAddress + offset].add(name.lstrip('_'))
        symbols.append(names)
    for name in symbols[0].keys() & symbols[1].keys():
        if len(symbols[0][name]) == len(symbols[1][name]) == 1:
            for side in (0, 1):
                tokens[side][next(iter(symbols[side][name]))].add('COFF:' + name)
    for row in metadata:
        source = row['source'].replace('cards/odyssey.c', 'cards/odissey.c')
        current = bounds_by_name[source, row['name']]
        if len(current) == 1:
            token = 'function:' + json.dumps([source, row['name']])
            tokens[0][int(row['address'], 16)].add(token)
            tokens[1][next(iter(current))].add(token)
    for side, image in enumerate(images):
        image.pe.parse_data_directories(directories=[1])
        for entry in getattr(image.pe, 'DIRECTORY_ENTRY_IMPORT', []):
            for item in entry.imports:
                tokens[side][item.address].add('import:' + entry.dll.decode().lower() + ':' +
                    (item.name.decode() if item.name else '#' + str(item.ordinal)))

    header = Path('manalink.h').read_text(encoding='latin1')
    assert re.search(r'^#define DLGIMPL\s+"\\x01"$', header, re.M)
    dialog_codes = {int(code, 16) for code in re.findall(
        r'^#define DLG_\w+(?:\([^)]*\))?\s+DLGIMPL\s+"\\x([0-9a-fA-F]{2})"', header, re.M)}
    assert dialog_codes == set(range(2, 28)), 'DIALOG token declaration changed'
    assert re.search(r'\bDLGIMPL_MODE\s*=\s*0x1B\b', header), 'Native MODE token declaration changed'

    def correspondence(a, b, usage='value'):
        if a == b and not 0x2000000 <= a < 0x3000000:
            return 'fixed-address:' + hex(a)
        if usage == 'code':
            for image, address in zip(images, (a, b)):
                section = image.pe.get_section_by_rva(address - 0x2000000)
                if section is None or not section.Characteristics & 0x20000000:
                    return None
        common = tokens[0][a] & tokens[1][b]
        if common:
            return min(common)
        if usage == 'value':
            x, y = literal_value(images[0], a, dialog_codes), literal_value(images[1], b, dialog_codes)
            if x is not None and x == y:
                return 'readonly-' + x[0] + '-contents:' + hashlib.sha256(x[1]).hexdigest()
        return None

    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    reviewed, unresolved = [], []
    for row in rows:
        try:
            base0 = int(row['address'], 16)
            native_names = [name for name, addresses in symbols[0].items() if base0 in addresses]
            candidates = set()
            for name in native_names:
                if len(symbols[0][name]) == len(symbols[1].get(name, set())) == 1:
                    candidates.update(symbols[1][name])
            if not candidates:
                candidates.update(bounds_by_name[row['source'], re.sub(r'\([0-9]+\)$', '', row['name'])])
            assert len(candidates) == 1, 'Ambiguous current entry'
            base1 = next(iter(candidates))
            declarations = bounds_by_address[base1]
            assert declarations and {r['source'].replace('cards/odyssey.c', 'cards/odissey.c') for r in declarations} == {row['source']}, 'Current source owner differs'
            widths = {r['size'] for r in declarations}
            assert len(widths) == 1, 'Ambiguous current bounds'
            sizes = [row['native_size'], next(iter(widths))]
            bases = [base0, base1]
            codes, ranges = [], []
            for side in (0, 1):
                raw = images[side].pe.get_data(bases[side] - 0x2000000, sizes[side])
                code = {i.address: i for i in md.disasm(raw, bases[side])}
                assert sum(i.size for i in code.values()) == sizes[side], 'Incomplete decoding'
                codes.append(code)
                ranges.append(dict(address=hex(bases[side]), size=sizes[side], sha256=hashlib.sha256(raw).hexdigest()))
            result = compare_cfg(codes, bases, sizes, correspondence,
                                 equality_guards=args.equality_guards, public_symbols=public_symbols,
                                 return_epilogues=args.return_epilogues,
                                 pointer_return_tails=base0 in pointer_contracts,
                                 void_return=base0 in void_contracts)
            reviewed.append(dict(name=row['name'], address=row['address'], source=row['source'],
                discovery_changes=row['changes'], ranges=ranges,
                void_return_contract=dict(native=void_contracts[base0], current_source_head=void_sources[base0]) if base0 in void_contracts else None,
                pointer_return_contract=dict(native=pointer_contracts[base0], current_source_head=pointer_sources[base0]) if base0 in pointer_contracts else None, **result))
        except (AssertionError, KeyError, ValueError) as error:
            unresolved.append(dict(name=row['name'], reason=str(error)))
    assert all(digest(p) == expected for p, expected in state.items()), 'Input changed during review'
    result = dict(format=1, reviewed=reviewed, unresolved=unresolved, inputs=state,
        equality_guard_option=args.equality_guards,
        return_epilogue_option=args.return_epilogues,
        pointer_return_tail_option=args.pointer_return_tails,
        void_equality_guard_option=args.void_equality_guards,
        discovery_report_sha256=digest(report_path), script_sha256=digest(__file__), decoder_version=capstone.__version__,
        scope='Entry-reachable direct CFG correspondence under the listed exact-name/import/function-address identities. '
              'Ignore architectural no-ops and direct internal jump trampolines; preserve ordinary operations, '
              'registers, operand widths, prefixes and edge predicates, including paired inverse predicates. '
              'Shared/duplicated tails may correlate to the same instructions. This is structural evidence, '
              'not execution, callee/data-content equivalence, literal pointer-identity proof or a whole-game proof. '
              'Read-only C-string and declared DIALOG-token mappings compare complete contents through NUL, not storage identity, '
              'alias relationships or bytes beyond the terminator. Arbitrary PC/return-address inspection is outside this review. '
              'Calls are followed through normal returns only; exception/unwind/longjmp edges are not covered. Indirect jumps are refused. '
              'Unsupported operation, register, address and width differences remain unresolved. '
              'Raw scores and discovery differences are unchanged; current native/rebuilt ranges are pinned.')
    if args.equality_guards:
        result['scope'] += (' Swapped global/stack-parameter equality loads may differ in EAX and non-ZF flags: '
            'the unequal successor must overwrite/clobber EAX before any read and discard comparison flags before any use. '
            'On the equal successor both loads yield identical EAX, and subtracting equal 32-bit operands gives identical CMP flags. '
            'Requires an ordinary disjoint C stack and global memory without concurrent mutation, MMIO, '
            'read-order-sensitive faults or asynchronous observation. Only named cross-CU cdecl calls are accepted '
            'as EAX/flag clobbers; unknown/inline register-argument calls remain unresolved.')
    if args.return_epilogues:
        result['scope'] += (' A single XOR EAX,EAX may move among an identical ordered sequence of '
            '32-bit stack restores ending in a plain RET. No EAX reads/restores, flag consumers, memory writes, '
            'calls or conditional branches are accepted in those suffixes. Requires the ordinary C return ABI '
            'with EFLAGS unobserved after return and valid stack accesses; exceptions, asynchronous/intermediate '
            'state inspection and fault-order equivalence are outside this check.')
    if args.void_equality_guards:
        result['scope'] += (' For functions with a current plain C void declaration and a named native C DWARF '
            'definition lacking any direct or inherited return type, equality scratch EAX may remain live only '
            'until an unprefixed operand-free RET. It still must not affect a memory address, store, argument, '
            'branch or any other instruction before return. Flags and ordinary operation/side-effect matching '
            'retain their existing checks. Return-register contents and EFLAGS are unobserved by the ordinary '
            'C void-return ABI; integer/pointer results and unknown declarations receive no relaxation.')
    if args.pointer_return_tails:
        result['scope'] += (' Optional pointer suffixes use structural symbolic expressions and equality decision trees, '
            'under native DWARF 32-bit pointer returns and matching current C char-pointer declarations. Only read-only '
            'MOV/extension/LEA/register AND/zero XOR/CMP/TEST, direct JE/JNE/internal JMP and ordinary stack restores/plain RET '
            'are supported. EAX, callee-saved registers and final ESP must agree on every modeled path. ECX, EDX and EFLAGS '
            'are caller-saved and unobserved after the ordinary C pointer return. Calls, stores, prefixes, cycles, other '
            'predicates/operations and state limits remain unresolved. Requires valid ordinary memory reads without '
            'concurrent mutation, volatile/MMIO effects, asynchronous intermediate observation or read/fault-order inspection; '
            'address/literal correspondences retain the assumptions above. No solver or sampled-input inference is used.')
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(dict(reviewed=len(reviewed), unresolved=len(unresolved), reasons=dict(__import__('collections').Counter(r['reason'] for r in unresolved).most_common(12))), indent=2))


if __name__ == '__main__':
    main()
