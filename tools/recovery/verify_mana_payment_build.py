"""Inspect rebuilt mana-payment storage and call boundaries without execution."""
from inspect_release import value, inherited_value


def verify(debug, pe, interfaces):
    if not interfaces.get('mana_payment_native_2022'):
        return {}
    from export_abi import type_of
    from pack_native_tables import unqual
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_AC_READ, CS_AC_WRITE
    from capstone.x86_const import X86_OP_IMM, X86_OP_MEM
    from recover_mana_payment import NAMES, CORE, MULTI, ARTIFACT, RESOLVE, CSV, MULTI_RESOLVE, MULTI_CSV, HYBRID, EXE_TARGETS
    from verify_native_build import literal_register_call_targets
    decoder = Cs(CS_ARCH_X86, CS_MODE_32); decoder.detail = True
    wanted = set(NAMES.values()); arrays = {}; pointers = {}; bodies = {}; symbols = {}
    for symbol, offset, section, kind, storage in debug.symbols:
        if symbol.lstrip('_') not in wanted:
            continue
        assert section > 0 and kind == 32 and storage == 2
        symbols[symbol.lstrip('_')] = debug.pe.OPTIONAL_HEADER.ImageBase + debug.pe.sections[section-1].VirtualAddress + offset
    assert set(symbols) == wanted
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') not in {'functions/engine.c', 'functions/functions.c'}:
            continue
        for die in cu.iter_DIEs():
            name = inherited_value(die, 'DW_AT_name')
            if die.tag == 'DW_TAG_subprogram' and name in wanted and value(die, 'DW_AT_low_pc') is not None:
                low = value(die, 'DW_AT_low_pc'); high = die.attributes['DW_AT_high_pc']
                end = high.value if high.form == 'DW_FORM_addr' else low + high.value
                assert low == symbols[name]
                bodies[name] = list(decoder.disasm(pe.get_data(low-pe.OPTIONAL_HEADER.ImageBase, end-low), low))
            if die.tag != 'DW_TAG_variable':
                continue
            parent = die.get_parent()
            while parent is not None and parent.tag != 'DW_TAG_subprogram':
                parent = parent.get_parent()
            if parent is None or inherited_value(parent, 'DW_AT_name') != NAMES[CORE]:
                continue
            t = unqual(type_of(die))
            if name in {'amtspaid', 'buffer', 'old_mana_pool', 'old_pay_mana'}:
                assert t.tag == 'DW_TAG_array_type'
                bounds = [value(c, 'DW_AT_upper_bound') for c in t.iter_children() if c.tag == 'DW_TAG_subrange_type']
                size = value(unqual(type_of(t)), 'DW_AT_byte_size')
                expected = (149, 1) if name == 'buffer' else (6, 4) if name == 'amtspaid' else (7, 4)
                assert (bounds, size) == ([expected[0]], expected[1]), (name, bounds, size)
                arrays[name] = dict(elements=bounds[0]+1, element_bytes=size)
            if name in interfaces['mana_payment_native_2022']['native_storage']['pointer_slots']:
                assert t.tag == 'DW_TAG_pointer_type' and value(t, 'DW_AT_byte_size') == 4
                pointers[name] = 4
    assert len(arrays) == len(pointers) == 4 and set(bodies) == wanted
    def direct(insns):
        return {i.operands[0].imm for i in insns if i.mnemonic in {'call', 'jmp'} and i.operands[0].type == X86_OP_IMM}
    core = bodies[NAMES[CORE]]
    targets = direct(core) | literal_register_call_targets(core)
    assert set(EXE_TARGETS) <= targets, [hex(x) for x in set(EXE_TARGETS)-targets]
    stores = {}; timing = {}
    for a in [MULTI, ARTIFACT, MULTI_RESOLVE, MULTI_CSV]:
        count = 7 if a == ARTIFACT else 6; covered = set()
        for ins in bodies[NAMES[a]]:
            for op in ins.operands:
                if op.type == X86_OP_MEM and op.access & CS_AC_WRITE and op.mem.base == op.mem.index == 0 and 0x4ef400 <= op.mem.disp < 0x4ef400+count*4:
                    assert op.mem.disp % 4 == 0 and op.size % 4 == 0
                    covered.update(range(op.mem.disp, op.mem.disp+op.size))
        assert covered == set(range(0x4ef400, 0x4ef400+count*4)), (a, covered)
        stores[NAMES[a]] = dict(words=count, bytes=len(covered))
    for a in [RESOLVE, CSV, MULTI_RESOLVE, MULTI_CSV, HYBRID]:
        insns = bodies[NAMES[a]]
        pay = next(i.address for i in insns if i.mnemonic == 'call' and i.operands[0].type == X86_OP_IMM and i.operands[0].imm == symbols[NAMES[CORE]]) if a != HYBRID else None
        pops = [i.address for i in insns if i.mnemonic == 'call' and i.operands[0].type == X86_OP_IMM and i.operands[0].imm == 0x436930]
        assert len(pops) == 1
        reads = [i.address for i in insns if (pay is None or i.address > pay) and any(o.type == X86_OP_MEM and o.access & CS_AC_READ and o.mem.base == o.mem.index == 0 and o.mem.disp == 0x4ef194 for o in i.operands)]
        if a in {RESOLVE, CSV}:
            assert len(reads) == 1 and reads[0] > pops[0], (a, reads, pops)
        elif a != HYBRID:
            assert len(reads) == 1 and reads[0] < pops[0], (a, reads, pops)
        else:
            assert not reads
        timing[NAMES[a]] = dict(pop=hex(pops[0]), fizzle_reads=[hex(x) for x in reads], result='after cleanup' if a in {RESOLVE, CSV} else 'captured before cleanup')
    return dict(public_symbols={n:hex(a) for n,a in symbols.items()}, array_shapes=arrays, saved_pointer_bytes=pointers,
                exe_boundaries=[hex(x) for x in sorted(EXE_TARGETS)], payment_request_stores=stores, resolving_return_timing=timing)
