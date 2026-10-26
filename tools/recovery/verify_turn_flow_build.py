"""Inspect rebuilt turn/combat/phasing helpers without running game code."""
from inspect_release import value, inherited_value


def verify(debug, pe, interfaces):
    if not interfaces.get('turn_flow_native_2022'):
        return {}
    import struct
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_AC_READ
    from capstone.x86_const import X86_OP_IMM, X86_OP_MEM, X86_REG_ESP
    from export_abi import type_of
    from pack_native_tables import unqual
    from recover_turn_flow import NAMES, ATTACK, DISPLAY, WINNER, DEAD, UNTAP, UPKEEP, COLOR, PHASING, EXE_TARGETS
    from verify_native_build import literal_register_call_targets
    decoder = Cs(CS_ARCH_X86, CS_MODE_32); decoder.detail = True
    wanted = set(NAMES.values()); symbols = {}; ranges = {}; bodies = {}; arrays = {}; retained_arrays = {}; table = None
    def coff(name):
        matches = [s for s in debug.symbols if s[0] == '_'+name]
        assert len(matches) == 1, (name, matches)
        _, off, sec, kind, storage = matches[0]
        at = off if sec == -1 else debug.pe.OPTIONAL_HEADER.ImageBase + debug.pe.sections[sec-1].VirtualAddress + off
        return at, sec, kind, storage
    for n in wanted:
        at, sec, kind, storage = coff(n)
        assert sec > 0 and kind == 32 and storage == 2
        symbols[n] = at
    native = interfaces['turn_flow_native_2022']['native_storage']
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') not in {'functions/engine.c', 'cards/mirage.c'}:
            continue
        for die in cu.iter_DIEs():
            n = inherited_value(die, 'DW_AT_name')
            if die.tag == 'DW_TAG_subprogram' and value(die, 'DW_AT_low_pc') is not None:
                lo = value(die, 'DW_AT_low_pc'); hi = die.attributes['DW_AT_high_pc']
                end = hi.value if hi.form == 'DW_FORM_addr' else lo + hi.value
                ranges[(n, lo)] = end
                if n in wanted:
                    assert lo == symbols[n]
                    bodies[n] = list(decoder.disasm(pe.get_data(lo-pe.OPTIONAL_HEADER.ImageBase, end-lo), lo))
            if die.tag != 'DW_TAG_variable':
                continue
            parent = die.get_parent()
            while parent is not None and parent.tag != 'DW_TAG_subprogram':
                parent = parent.get_parent()
            owner = inherited_value(parent, 'DW_AT_name') if parent is not None else None
            if (owner, n) == ('sub_495BE0', 'num_per_col'):
                t = unqual(type_of(die)); assert t.tag == 'DW_TAG_array_type'
                retained_arrays[n] = dict(element_bytes=value(unqual(type_of(t)), 'DW_AT_byte_size'), bounds=[value(c, 'DW_AT_upper_bound') for c in t.iter_children() if c.tag == 'DW_TAG_subrange_type'])
            if (owner, n) not in {(NAMES[UNTAP], 'untapped'), (NAMES[UNTAP], 'buf'), (NAMES[UPKEEP], 'str'), (NAMES[COLOR], 'num_per_col'), (NAMES[PHASING], 'shimmer_subtypes')}:
                continue
            t = unqual(type_of(die)); assert t.tag == 'DW_TAG_array_type'
            arrays[owner+'::'+n] = dict(element_bytes=value(unqual(type_of(t)), 'DW_AT_byte_size'), bounds=[value(c, 'DW_AT_upper_bound') for c in t.iter_children() if c.tag == 'DW_TAG_subrange_type'])
            if owner == NAMES[PHASING]:
                loc = value(die, 'DW_AT_location'); assert loc[0] == 3 and len(loc) == 5
                at = int.from_bytes(bytes(loc[1:]), 'little')
                assert list(struct.unpack('<14i', pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase, 56))) == native['shimmer_table']['values']
                table = dict(address=hex(at), elements=14, bytes=56)
    folded = {}
    for a in [ATTACK, DISPLAY, WINNER, DEAD, COLOR]:
        n = NAMES[a]
        if n in bodies:
            continue
        old = n.removesuffix('_2022'); target = coff(old)[0]; at = symbols[n]
        insns = list(decoder.disasm(pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase, 5), at))
        assert len(insns) == 1 and insns[0].mnemonic == 'jmp' and insns[0].operands[0].type == X86_OP_IMM and insns[0].operands[0].imm == target
        folded[n] = dict(retained_target=old, target=hex(target), bytes=insns[0].bytes.hex())
        end = ranges[(old, target)]
        bodies[n] = list(decoder.disasm(pe.get_data(target-pe.OPTIONAL_HEADER.ImageBase, end-target), target))
    assert set(bodies) == wanted, wanted-set(bodies)
    # A folded color helper uses the retained helper's identical six-word local.
    for key, expected in native['arrays'].items():
        owner, n = key.split('::'); new_key = owner+'_2022::'+n
        if new_key not in arrays:
            assert owner+'_2022' in folded and n == 'num_per_col'
            assert retained_arrays[n] == {k:expected[k] for k in ['element_bytes', 'bounds']}
            arrays[new_key] = retained_arrays[n]
        else:
            assert arrays[new_key] == {k:expected[k] for k in ['element_bytes', 'bounds']}
    assert table and len(arrays) == 5
    def targets(insns):
        return {i.operands[0].imm for i in insns if i.mnemonic in {'call', 'jmp'} and i.operands[0].type == X86_OP_IMM} | literal_register_call_targets(insns)
    def resolved(n):
        return int(folded[n]['target'], 16) if n in folded else coff(n)[0]
    all_targets = set().union(*(targets(v) for v in bodies.values()))
    assert set(EXE_TARGETS) <= all_targets, [hex(a) for a in set(EXE_TARGETS)-all_targets]
    assert 0x437620 not in all_targets
    deps = [NAMES[PHASING], 'count_colors_of_lands_in_play_2022', 'count_mana_2022', 'recalculate_all_cards_in_play_2022', NAMES[DEAD]]
    assert {resolved(n) for n in deps} <= targets(bodies[NAMES[UNTAP]])
    assert {resolved('phase_changed_2022'), resolved(NAMES[DEAD]), resolved('ai_decision_phase')} <= targets(bodies[NAMES[UPKEEP]])
    modes = {}
    insns = bodies[NAMES[PHASING]]
    for name in ['phase_in_impl_2022', 'phase_out_impl_2022']:
        modes[name] = []
        for j, ins in enumerate(insns):
            if ins.mnemonic != 'call' or ins.operands[0].type != X86_OP_IMM or ins.operands[0].imm != coff(name)[0]:
                continue
            found = next((p for p in reversed(insns[max(0, j-15):j]) if p.mnemonic == 'mov' and len(p.operands) == 2 and p.operands[0].type == X86_OP_MEM and p.operands[0].mem.base == X86_REG_ESP and p.operands[0].mem.disp == 12), None)
            assert found is not None and found.operands[1].type == X86_OP_IMM
            modes[name].append(dict(call=hex(ins.address), mode=found.operands[1].imm))
        assert sorted(m['mode'] for m in modes[name]) == [0, 1], (name, modes[name])
    poison = {}
    for a in [WINNER, DEAD]:
        sites = [dict(address=hex(i.address), bytes=i.bytes.hex(), target=hex(o.mem.disp), width=o.size) for i in bodies[NAMES[a]] for o in i.operands
                 if o.type == X86_OP_MEM and o.access & CS_AC_READ and o.mem.disp in {0x4ef530, 0x4ef534}]
        assert {s['target'] for s in sites} == {'0x4ef530', '0x4ef534'} and all(s['width'] == 1 for s in sites)
        poison[NAMES[a]] = sites
    display_masks = [o.imm & 0xffffffff for i in bodies[NAMES[DISPLAY]] for o in i.operands if o.type == X86_OP_IMM and o.imm & 0xffffffff in {0x10b21010, 0x20000}]
    assert set(display_masks) == {0x10b21010, 0x20000}
    return dict(public_symbols={n:hex(a) for n,a in symbols.items()}, folded_public_thunks=folded, array_shapes=arrays, shimmer_table=table,
                phasing_completion_modes=modes, poison_byte_reads=poison, display_masks=display_masks, untap_dependency_calls=deps,
                obsolete_upkeep_reset_absent=True, exe_boundaries=[hex(a) for a in sorted(EXE_TARGETS)])
