"""Inspect the recovered engine-state helpers without executing the DLL."""
from inspect_release import value, inherited_value


def verify(debug, pe, interfaces):
    if not interfaces.get('engine_state_native_2022'):
        return {}
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_AC_READ
    from capstone.x86_const import X86_OP_IMM, X86_OP_MEM
    from export_abi import type_of
    from pack_native_tables import unqual
    from recover_engine_state import NAMES, LANDS, PHASE, ASTERISK, LEGACY, FROM_IID, FORMAT, AUTOTAP, RECALC, RESET, EXE_TARGETS
    from verify_native_build import literal_register_call_targets
    decoder = Cs(CS_ARCH_X86, CS_MODE_32); decoder.detail = True
    wanted = set(NAMES.values()); symbols = {}; bodies = {}; arrays = {}; pointers = {}; override_type = None
    def coff(name):
        matches = [s for s in debug.symbols if s[0] == '_'+name]
        assert len(matches) == 1, (name, matches)
        _, offset, section, kind, storage = matches[0]
        address = offset if section == -1 else debug.pe.OPTIONAL_HEADER.ImageBase + debug.pe.sections[section-1].VirtualAddress + offset
        return address, section, kind, storage
    for name in wanted:
        address, section, kind, storage = coff(name)
        assert section > 0 and kind == 32 and storage == 2
        symbols[name] = address
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') not in {'functions/engine.c', 'functions/has_creature_type.c'}:
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
            t = unqual(type_of(die))
            if name == 'override_xx' and value(die, 'DW_AT_location') is not None:
                assert value(t, 'DW_AT_byte_size') == 4 and inherited_value(die, 'DW_AT_external')
                loc = value(die, 'DW_AT_location'); assert loc[0] == 3 and len(loc) == 5
                assert int.from_bytes(bytes(loc[1:]), 'little') == coff('override_xx')[0]
                override_type = dict(bytes=4, external=True)
            parent = die.get_parent()
            while parent is not None and parent.tag != 'DW_TAG_subprogram':
                parent = parent.get_parent()
            owner = inherited_value(parent, 'DW_AT_name') if parent is not None else None
            if owner not in {NAMES[FORMAT], NAMES[AUTOTAP]}:
                continue
            if t.tag == 'DW_TAG_array_type':
                bounds = [value(c, 'DW_AT_upper_bound') for c in t.iter_children() if c.tag == 'DW_TAG_subrange_type']
                arrays[owner+'::'+name] = dict(element_bytes=value(unqual(type_of(t)), 'DW_AT_byte_size'), bounds=bounds)
            if str(name).startswith('old_charge_mana_addr_of_'):
                assert t.tag == 'DW_TAG_pointer_type' and value(t, 'DW_AT_byte_size') == 4
                pointers[name] = 4
    assert len(arrays) == 4 and len(pointers) == 3 and override_type
    native = interfaces['engine_state_native_2022']['native_storage']
    assert {k.removesuffix('_2022')+'::'+n:v for key,v in arrays.items() for k,n in [key.split('::')]} == native['arrays']
    assert pointers == native['saved_pointer_bytes']
    address, section, _, storage = coff('override_xx')
    assert storage == 2 and section > 0
    assert debug.pe.sections[section-1].Name.rstrip(b'\0') == b'.bss' and debug.pe.sections[section-1].SizeOfRawData == 0
    folded = {}
    for a in [PHASE, ASTERISK, LEGACY, FROM_IID]:
        name = NAMES[a]
        if name in bodies:
            continue
        retained = name.removesuffix('_2022'); target = coff(retained)[0]; entry = symbols[name]
        insns = list(decoder.disasm(pe.get_data(entry-pe.OPTIONAL_HEADER.ImageBase, 5), entry))
        assert len(insns) == 1 and insns[0].mnemonic == 'jmp' and insns[0].operands[0].type == X86_OP_IMM and insns[0].operands[0].imm == target
        bodies[name] = insns
        folded[name] = dict(retained_target=retained, target=hex(target), bytes=insns[0].bytes.hex())
    assert set(bodies) == wanted, wanted-set(bodies)
    def targets(insns):
        return {i.operands[0].imm for i in insns if i.mnemonic in {'call', 'jmp'} and i.operands[0].type == X86_OP_IMM} | literal_register_call_targets(insns)
    # Folded bodies retain the original code; include its concrete DWARF ranges
    # in the boundary inspection instead of guessing a thunk's body length.
    for name, detail in folded.items():
        old = detail['retained_target']; entry = coff(old)[0]
        for cu in debug.dwarf().iter_CUs():
            if value(cu.get_top_DIE(), 'DW_AT_name') != 'functions/engine.c':
                continue
            matches = [d for d in cu.iter_DIEs() if d.tag == 'DW_TAG_subprogram' and inherited_value(d, 'DW_AT_name') == old and value(d, 'DW_AT_low_pc') == entry]
            assert len(matches) == 1
            high = matches[0].attributes['DW_AT_high_pc']; end = high.value if high.form == 'DW_FORM_addr' else entry + high.value
            bodies[name] = list(decoder.disasm(pe.get_data(entry-pe.OPTIONAL_HEADER.ImageBase, end-entry), entry))
    all_targets = set().union(*(targets(v) for v in bodies.values()))
    assert set(EXE_TARGETS) <= all_targets, [hex(a) for a in set(EXE_TARGETS)-all_targets]
    recalc_targets = targets(bodies[NAMES[RECALC]])
    assert {symbols[NAMES[LANDS]], symbols[NAMES[RESET]], coff('count_mana_2022')[0]} <= recalc_targets
    assert coff('put_card_or_activation_onto_stack_impl_2022')[0] in targets(bodies[NAMES[AUTOTAP]])
    def reads(name, at):
        return [dict(address=hex(i.address), bytes=i.bytes.hex(), width=o.size) for i in bodies[name] for o in i.operands
                if o.type == X86_OP_MEM and o.access & CS_AC_READ and o.mem.disp == at]
    override_reads = reads(NAMES[FORMAT], address)
    added_reads = reads(NAMES[LANDS], coff('mana_colors_added')[0])
    assert override_reads and added_reads and all(x['width'] == 4 for x in override_reads+added_reads)
    mask_sites = [dict(address=hex(i.address), bytes=i.bytes.hex()) for i in bodies[NAMES[RECALC]] if any(o.type == X86_OP_IMM and o.imm & 0xffffffff == 0xf0c0000f for o in i.operands)]
    assert len(mask_sites) == 2
    reset_filter = [dict(address=hex(i.address), bytes=i.bytes.hex()) for i in bodies[NAMES[RESET]] if i.mnemonic in {'and', 'mov'} and any(o.type == X86_OP_IMM and o.imm == 0x35f for o in i.operands)]
    assert len(reset_filter) == 1
    instructions = bodies[NAMES[RESET]]
    at = next(j for j,i in enumerate(instructions) if hex(i.address) == reset_filter[0]['address'])
    if instructions[at].mnemonic == 'mov':
        assert instructions[at+1].mnemonic == 'shr' and instructions[at+1].op_str.endswith(', cl')
        assert instructions[at+2].mnemonic == 'and' and instructions[at+2].operands[1].type == X86_OP_IMM and instructions[at+2].operands[1].imm == 1
    return dict(public_symbols={n:hex(a) for n,a in symbols.items()}, folded_public_thunks=folded, array_shapes=arrays, saved_pointer_bytes=pointers,
                override_global=dict(address=hex(address), **override_type), override_reads=override_reads, added_mana_color_reads=added_reads,
                recalc_player_masks=mask_sites, subtype_filter=reset_filter, exe_boundaries=[hex(a) for a in sorted(EXE_TARGETS)])
