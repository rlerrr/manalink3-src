"""Inspect rebuilt stack/cast helpers and their native ABI boundaries."""
from inspect_release import value, inherited_value


def verify(debug, pe, interfaces):
    if not interfaces.get('stack_activation_native_2022'):
        return {}
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32
    from capstone.x86_const import X86_OP_IMM, X86_OP_MEM
    from export_abi import type_of
    from pack_native_tables import unqual
    from recover_stack_activation import NAMES, WORKER, RECOPY, RESPONSE, SPELL, FORCE, EXE_TARGETS
    from verify_native_build import literal_register_call_targets
    decoder = Cs(CS_ARCH_X86, CS_MODE_32); decoder.detail = True
    wanted = set(NAMES.values()); symbols = {}; bodies = {}; layouts = {}; enums = {}
    for symbol, offset, section, kind, storage in debug.symbols:
        name = symbol.lstrip('_')
        if name not in wanted:
            continue
        assert section > 0 and kind == 32 and storage == (3 if name == NAMES[WORKER] else 2)
        assert name not in symbols
        symbols[name] = dict(address=debug.pe.OPTIONAL_HEADER.ImageBase + debug.pe.sections[section-1].VirtualAddress + offset, storage=storage)
    assert set(symbols) == wanted
    constants = dict(STATE_BEING_CAST_2022=0x80000000, ENABLE_XTRIGGER_REPLACE_ETB_2022=0x80, XTRIGGER_REPLACE_ETB_2022=0x1000e00, WAV_SNOW_2022=84)
    # Inspect real compiled enum names after source consolidation. The retained
    # ledger spellings stay in historical evidence, not in compiler headers.
    from canonicalize_helpers import source_names
    spellings = source_names()
    constants = {spellings.get(name, name): expected for name, expected in constants.items()}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'functions/engine.c':
            continue
        for die in cu.iter_DIEs():
            name = inherited_value(die, 'DW_AT_name')
            if die.tag == 'DW_TAG_subprogram' and name in wanted and value(die, 'DW_AT_low_pc') is not None:
                low = value(die, 'DW_AT_low_pc'); high = die.attributes['DW_AT_high_pc']
                end = high.value if high.form == 'DW_FORM_addr' else low + high.value
                assert low == symbols[name]['address']
                bodies[name] = list(decoder.disasm(pe.get_data(low-pe.OPTIONAL_HEADER.ImageBase, end-low), low))
            if die.tag == 'DW_TAG_enumerator' and name in constants:
                enums[name] = value(die, 'DW_AT_const_value')
            if die.tag == 'DW_TAG_typedef' and name in {'stack_data_t', 'card_ext_t'}:
                t = unqual(type_of(die))
                layouts[name] = dict(bytes=value(t, 'DW_AT_byte_size'), members={value(c, 'DW_AT_name'):value(c, 'DW_AT_data_member_location') for c in t.iter_children() if c.tag == 'DW_TAG_member'})
    # GCC folds the identical response body into the retained implementation.
    # Prove its public entry is exactly a direct tail jump to that old body.
    folded = {}
    if NAMES[RESPONSE] not in bodies:
        records = [s for s in debug.symbols if s[0] == '_allow_response_to_activation']
        assert len(records) == 1
        _, off, sec, kind, storage = records[0]
        assert sec > 0 and kind == 32 and storage == 2
        target = debug.pe.OPTIONAL_HEADER.ImageBase + debug.pe.sections[sec-1].VirtualAddress + off
        entry = symbols[NAMES[RESPONSE]]['address']
        insns = list(decoder.disasm(pe.get_data(entry-pe.OPTIONAL_HEADER.ImageBase, 5), entry))
        assert len(insns) == 1 and insns[0].mnemonic == 'jmp' and insns[0].operands[0].type == X86_OP_IMM and insns[0].operands[0].imm == target
        bodies[NAMES[RESPONSE]] = insns
        folded[NAMES[RESPONSE]] = dict(retained_target='allow_response_to_activation', target=hex(target), bytes=insns[0].bytes.hex())
    assert set(bodies) == wanted and enums == constants, (wanted-set(bodies), enums, constants)
    expected = interfaces['stack_activation_native_2022']['native_storage']['types']
    assert layouts == expected
    def targets(insns):
        return {i.operands[0].imm for i in insns if i.mnemonic in {'call', 'jmp'} and i.operands[0].type == X86_OP_IMM} | literal_register_call_targets(insns)
    all_targets = set().union(*(targets(v) for v in bodies.values()))
    assert set(EXE_TARGETS) <= all_targets, [hex(x) for x in set(EXE_TARGETS)-all_targets]
    def address(name):
        matches = [s for s in debug.symbols if s[0] == '_'+name]
        assert len(matches) == 1
        _, offset, section, _, _ = matches[0]
        return offset if section == -1 else debug.pe.OPTIONAL_HEADER.ImageBase + debug.pe.sections[section-1].VirtualAddress + offset
    copy = address('memcpy_inst_and_ext')
    assert all(copy in targets(bodies[NAMES[a]]) for a in [WORKER, RECOPY])
    assert symbols[NAMES[WORKER]]['address'] in targets(bodies[NAMES[FORCE]])
    spell = bodies[NAMES[SPELL]]; spell_targets = targets(spell)
    dependencies = ['query', 'get_storm_count_2022', 'get_dead_count', 'copy_spell_from_stack_2022', 'dispatch_xtrigger2', 'load_text_2022']
    assert all(address(n) in spell_targets for n in dependencies)
    replicate = [dict(address=hex(i.address), bytes=i.bytes.hex()) for i in spell if any(o.type == X86_OP_MEM and o.size == 1 and o.mem.disp == 159 for o in i.operands)]
    assert len(replicate) == 1
    cast_sets = [hex(i.address) for i in spell if i.mnemonic == 'or' and any(o.type == X86_OP_IMM and o.imm & 0xffffffff == 0x80000000 for o in i.operands)]
    cast_clears = [hex(i.address) for i in spell if i.mnemonic == 'and' and any(o.type == X86_OP_IMM and o.imm & 0xffffffff == 0x7fffffff for o in i.operands)]
    assert cast_sets and len(cast_clears) >= 2
    return dict(folded_public_thunks=folded, symbols={n:dict(address=hex(v['address']), storage=v['storage']) for n,v in symbols.items()}, record_layouts=layouts,
                qualified_constants=enums, exe_boundaries=[hex(x) for x in sorted(EXE_TARGETS)], extension_copy_callers=[NAMES[WORKER], NAMES[RECOPY]],
                spell_dependency_calls=dependencies, replicate_byte_load=replicate, being_cast_sets=cast_sets, being_cast_clears=cast_clears)
