"""Conservative symbolic comparison of read-only i386 pointer-return suffixes.

No solver, sampling, callee execution, writes or loop unrolling. Expressions and
equality decision trees must correspond structurally. The caller must establish
that both entries return a single pointer in EAX under the ordinary C ABI.
"""
from capstone.x86_const import X86_OP_IMM, X86_OP_MEM, X86_OP_REG
from review_event_guards import roots


def constant(value, bits=32):
    return ('constant', bits, value & ((1 << bits) - 1))


def truncate(value, bits):
    if value[0] == 'constant':
        return constant(value[2], bits)
    if value[0] == 'extend' and bits == value[2]:
        return value[3]
    if value[0] == 'low' and value[1] >= bits:
        return ('low', bits, value[2])
    return ('low', bits, value)


def add(left, right):
    if left == constant(0):
        return right
    if right == constant(0):
        return left
    if left[0] == right[0] == 'constant':
        return constant(left[2] + right[2])
    return ('add', left, right)


def equal(left, right):
    if left == right:
        return True
    if left[0] == right[0] == 'constant':
        return left[1:] == right[1:]
    return ('equal', left, right)


def describe(code, start, base, size):
    visited = set()
    budget = [0]
    initial = {name: ('incoming', name) for name in
               ('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp', 'esp')}

    def walk(pc, registers, zf, path):
        registers = dict(registers)
        path = set(path)
        while True:
            assert pc not in path, 'Pointer suffix cycle'
            assert base <= pc < base + size and pc in code, 'Pointer suffix leaves bounds'
            path.add(pc)
            visited.add(pc)
            budget[0] += 1
            assert budget[0] <= 256, 'Pointer suffix state limit'
            inst = code[pc]
            assert not any(inst.prefix), 'Pointer suffix instruction prefix'
            ops = inst.operands

            def address(op):
                assert not op.mem.segment, 'Pointer suffix segmented address'
                # Only a no-base memory displacement carries a relocation
                # identity. An integer used as a mask/compare operand does not.
                result = (('address', 32, op.mem.disp & 0xffffffff)
                          if not op.mem.base and op.mem.disp >= 0x400000 else constant(op.mem.disp))
                if op.mem.base:
                    name = inst.reg_name(op.mem.base)
                    assert name in registers, 'Pointer suffix address register'
                    result = add(registers[name], result)
                if op.mem.index:
                    name = inst.reg_name(op.mem.index)
                    assert name in registers, 'Pointer suffix index register'
                    term = registers[name] if op.mem.scale == 1 else ('multiply', registers[name], constant(op.mem.scale))
                    result = add(result, term)
                return result

            def read(op):
                assert op.size in (1, 2, 4), 'Pointer suffix operand width'
                bits = op.size * 8
                if op.type == X86_OP_IMM:
                    return constant(op.imm, bits)
                if op.type == X86_OP_MEM:
                    return ('load', bits, address(op))
                assert op.type == X86_OP_REG, 'Pointer suffix operand type'
                name = inst.reg_name(op.reg)
                assert name in roots and roots[name][1] == 0, 'Pointer suffix high-byte register'
                root = roots[name][0]
                return registers[root] if bits == 32 else truncate(registers[root], bits)

            def write(op, value):
                assert op.type == X86_OP_REG and op.size == 4, 'Pointer suffix write is not full register'
                name = inst.reg_name(op.reg)
                assert name in registers and name not in ('ebp', 'esp'), 'Pointer suffix frame write'
                registers[name] = value

            name = inst.mnemonic
            following = pc + inst.size
            if name == 'nop':
                pass
            elif name == 'mov' and len(ops) == 2:
                write(ops[0], read(ops[1]))
            elif name in ('movsx', 'movzx') and len(ops) == 2:
                assert ops[1].size in (1, 2), 'Pointer suffix extension width'
                write(ops[0], ('extend', name, ops[1].size * 8, read(ops[1])))
            elif name == 'lea' and len(ops) == 2 and ops[1].type == X86_OP_MEM:
                write(ops[0], address(ops[1]))
            elif (name == 'xor' and len(ops) == 2 and all(op.type == X86_OP_REG for op in ops)
                  and ops[0].reg == ops[1].reg and ops[0].size == 4):
                write(ops[0], constant(0))
                zf = True
            elif name == 'and' and len(ops) == 2 and ops[0].type == X86_OP_REG:
                value = ('and', read(ops[0]), read(ops[1]))
                write(ops[0], value)
                zf = equal(value, constant(0))
            elif name in ('cmp', 'test') and len(ops) == 2:
                left, right = read(ops[0]), read(ops[1])
                assert ops[0].size == ops[1].size, 'Pointer suffix compare widths'
                if name == 'cmp':
                    zf = equal(left, right)
                elif left == right:
                    zf = equal(left, constant(0, ops[0].size * 8))
                else:
                    zf = equal(('and', left, right), constant(0, ops[0].size * 8))
            elif name in ('je', 'jne') and len(ops) == 1 and ops[0].type == X86_OP_IMM:
                assert zf is not None, 'Pointer suffix reads incoming flags'
                on_equal = ops[0].imm if name == 'je' else following
                on_other = following if name == 'je' else ops[0].imm
                if isinstance(zf, bool):
                    pc = on_equal if zf else on_other
                    continue
                left = walk(on_equal, registers, zf, path)
                right = walk(on_other, registers, zf, path)
                return left if left == right else ('choice', zf, left, right)
            elif name == 'jmp' and len(ops) == 1 and ops[0].type == X86_OP_IMM:
                pc = ops[0].imm
                continue
            elif name == 'pop' and len(ops) == 1 and ops[0].type == X86_OP_REG and ops[0].size == 4:
                target = inst.reg_name(ops[0].reg)
                assert target in ('ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp'), 'Pointer suffix pop register'
                registers[target] = ('load', 32, registers['esp'])
                registers['esp'] = add(registers['esp'], constant(4))
            elif name == 'leave' and not ops:
                registers['esp'] = add(registers['ebp'], constant(4))
                registers['ebp'] = ('load', 32, registers['ebp'])
            elif (name == 'add' and len(ops) == 2 and ops[0].type == X86_OP_REG
                  and inst.reg_name(ops[0].reg) == 'esp' and ops[1].type == X86_OP_IMM
                  and ops[0].size == 4 and ops[1].imm > 0 and ops[1].imm % 4 == 0):
                registers['esp'] = add(registers['esp'], constant(ops[1].imm))
                zf = equal(registers['esp'], constant(0))
            elif name == 'ret' and not ops:
                return ('return', *(registers[n] for n in ('eax', 'ebx', 'esi', 'edi', 'ebp')),
                        add(registers['esp'], constant(4)))
            else:
                raise AssertionError(('Unsupported pointer suffix operation', name))
            pc = following

    tree = walk(start, initial, None, set())
    return dict(tree=tree, visited=sorted(visited), interpreted_states=budget[0])


def compare_pointer_tails(codes, starts, bases, sizes, correspondence):
    models = [describe(code, start, base, size) for code, start, base, size in zip(codes, starts, bases, sizes)]
    references = set()

    def same(left, right, pointer_result=False):
        if left == right:
            return True
        if not isinstance(left, tuple) or not isinstance(right, tuple) or left[0] != right[0]:
            return False
        if left[0] in ('constant', 'address'):
            if left[0] == 'constant' and not pointer_result:
                return False
            if left[1] != right[1] or left[1] != 32:
                return False
            # Direct pointer results and tagged memory addresses may map.
            # Masks, comparison literals, offsets and all other integers may not.
            if min(left[2], right[2]) < 0x400000 or max(left[2], right[2]) >= 0x80000000:
                return False
            token = correspondence(left[2], right[2], 'value')
            if token is None:
                return False
            references.add((left[2], right[2], token))
            return True
        if len(left) != len(right):
            return False
        if left[0] == 'return':
            return same(left[1], right[1], pointer_result=True) and all(
                same(a, b) for a, b in zip(left[2:], right[2:]))
        if left[0] in ('equal', 'and'):
            return (same(left[1], right[1]) and same(left[2], right[2]) or
                    same(left[1], right[2]) and same(left[2], right[1]))
        return all(same(a, b) if isinstance(a, tuple) or isinstance(b, tuple) else a == b
                   for a, b in zip(left[1:], right[1:]))

    assert same(models[0]['tree'], models[1]['tree']), 'Pointer return decision trees differ'
    return dict(models=models, address_correspondences=[dict(native=hex(a), rebuilt=hex(b), identity=t)
                for a, b, t in sorted(references)])


def native_pointer_contracts(debug, candidates):
    """Read only candidate CUs; do not infer a native return ABI from its name."""
    import struct
    from inspect_release import inherited_value, value
    wanted = {int(r['address'], 16): r['source'] for r in candidates}
    sources = set(wanted.values())
    dwarf = debug.dwarf()
    data = debug.section_data(debug.sections['.debug_info'])
    offset, found = 0, {}

    def return_type(die, seen):
        if die.offset in seen:
            return None
        seen.add(die.offset)
        if 'DW_AT_type' in die.attributes:
            return die.get_DIE_from_attribute('DW_AT_type')
        for name in ('DW_AT_abstract_origin', 'DW_AT_specification'):
            if name in die.attributes:
                result = return_type(die.get_DIE_from_attribute(name), seen)
                if result is not None:
                    return result
        return None

    while offset + 4 <= len(data):
        length = struct.unpack_from('<I', data, offset)[0]
        if length == 0:
            break
        assert length != 0xffffffff, 'DWARF64 is unsupported'
        cu = dwarf.get_CU_at(offset)
        offset += length + 4
        source = value(cu.get_top_DIE(), 'DW_AT_name', '').replace('\\', '/').replace('cards/odyssey.c', 'cards/odissey.c')
        if source not in sources:
            continue
        for die in cu.iter_DIEs():
            if die.tag != 'DW_TAG_subprogram':
                continue
            address = value(die, 'DW_AT_low_pc')
            if address not in wanted or wanted[address] != source:
                continue
            typ = return_type(die, set())
            seen = set()
            while typ is not None and typ.tag in ('DW_TAG_typedef', 'DW_TAG_const_type', 'DW_TAG_volatile_type'):
                if typ.offset in seen:
                    typ = None
                    break
                seen.add(typ.offset)
                typ = typ.get_DIE_from_attribute('DW_AT_type') if 'DW_AT_type' in typ.attributes else None
            if typ is not None and typ.tag == 'DW_TAG_pointer_type' and value(typ, 'DW_AT_byte_size', cu['address_size']) == 4:
                assert address not in found, 'Ambiguous native pointer contract'
                found[address] = dict(source=source, native_name=inherited_value(die, 'DW_AT_name'),
                                      function_die=hex(die.offset), pointer_type_die=hex(typ.offset), bytes=4)
    return found
