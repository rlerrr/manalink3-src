"""Inspect rebuilt mana-drain and AI phase/choice bodies and inline instances."""
from inspect_release import value, inherited_value


def verify(debug, pe, interfaces):
    if not interfaces.get('mana_ai_phase_native_2022'):
        return {}
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_AC_WRITE
    from capstone.x86_const import X86_OP_IMM, X86_OP_MEM, X86_OP_REG, X86_REG_ESI
    from export_abi import type_of
    from pack_native_tables import unqual
    from recover_mana_ai_phase import NAMES, PUBLIC_HELPERS, WORKER, BURN, NONDRAIN, MINIMIZE, REMEMBER, RAND, TIMER, DECISION, EXE_TARGETS
    from verify_native_build import literal_register_call_targets
    decoder = Cs(CS_ARCH_X86, CS_MODE_32); decoder.detail = True
    wanted = {NAMES[a] for a in PUBLIC_HELPERS}; symbols = {}; bodies = {}; ranges = {}; arrays = {}; inline = []
    def coff(n):
        matches = [s for s in debug.symbols if s[0] == '_'+n]
        assert len(matches) == 1, (n, matches)
        _, off, sec, kind, storage = matches[0]
        at = off if sec == -1 else debug.pe.OPTIONAL_HEADER.ImageBase + debug.pe.sections[sec-1].VirtualAddress + off
        return at, sec, kind, storage
    for n in wanted:
        at, sec, kind, storage = coff(n)
        assert sec > 0 and kind == 32 and storage == 2
        symbols[n] = at
    assert not any(s[0] == '_'+NAMES[WORKER] for s in debug.symbols)
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') not in {'functions/ai.c', 'functions/produce_mana.c'}:
            continue
        for d in cu.iter_DIEs():
            n = inherited_value(d, 'DW_AT_name')
            if d.tag == 'DW_TAG_subprogram' and value(d, 'DW_AT_low_pc') is not None:
                lo = value(d, 'DW_AT_low_pc'); hi = d.attributes['DW_AT_high_pc']; end = hi.value if hi.form == 'DW_FORM_addr' else lo+hi.value
                ranges[(n, lo)] = end
                if n in wanted:
                    assert lo == symbols[n]
                    bodies[n] = list(decoder.disasm(pe.get_data(lo-pe.OPTIONAL_HEADER.ImageBase, end-lo), lo))
            if d.tag == 'DW_TAG_inlined_subroutine' and n == NAMES[WORKER]:
                parent = d.get_parent()
                while parent is not None and parent.tag != 'DW_TAG_subprogram':
                    parent = parent.get_parent()
                owner = inherited_value(parent, 'DW_AT_name'); assert owner in {NAMES[REMEMBER], NAMES[RAND]}
                origin = d.get_DIE_from_attribute('DW_AT_abstract_origin')
                params = [inherited_value(p, 'DW_AT_name') for p in origin.iter_children() if p.tag == 'DW_TAG_formal_parameter']
                assert params == ['value'] and value(origin, 'DW_AT_low_pc') is None
                base = value(cu.get_top_DIE(), 'DW_AT_low_pc'); spans = []
                for entry in debug.dwarf().range_lists().get_range_list_at_offset(value(d, 'DW_AT_ranges'), cu=cu):
                    if hasattr(entry, 'base_address'):
                        base = entry.base_address
                    else:
                        lo = entry.begin_offset + (0 if entry.is_absolute else base)
                        end = entry.end_offset + (0 if entry.is_absolute else base)
                        if end > lo:
                            spans.append((lo, end))
                assert spans
                inline.append(dict(parent=owner, ranges=spans))
            if d.tag != 'DW_TAG_variable' or n not in {'str', 'store_raw_mana_available'}:
                continue
            parent = d.get_parent()
            while parent is not None and parent.tag != 'DW_TAG_subprogram':
                parent = parent.get_parent()
            if parent is None or inherited_value(parent, 'DW_AT_name') != NAMES[DECISION]:
                continue
            t = unqual(type_of(d)); bounds = []
            while t.tag == 'DW_TAG_array_type':
                bounds.extend(value(c, 'DW_AT_upper_bound') for c in t.iter_children() if c.tag == 'DW_TAG_subrange_type'); t = unqual(type_of(t))
            arrays[n] = dict(bounds=bounds, element_bytes=value(t, 'DW_AT_byte_size'))
    native = interfaces['mana_ai_phase_native_2022']['native_storage']
    assert arrays == {n:{k:v[k] for k in ['bounds', 'element_bytes']} for n,v in native['arrays'].items()}
    folded = {}
    for a in [NONDRAIN, MINIMIZE]:
        n = NAMES[a]
        if n in bodies:
            continue
        old = n.removesuffix('_2022'); target = coff(old)[0]; at = symbols[n]
        insns = list(decoder.disasm(pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase, 5), at))
        assert len(insns) == 1 and insns[0].mnemonic == 'jmp' and insns[0].operands[0].type == X86_OP_IMM and insns[0].operands[0].imm == target
        folded[n] = dict(retained_target=old, target=hex(target), bytes=insns[0].bytes.hex())
        end = ranges[(old, target)]
        bodies[n] = list(decoder.disasm(pe.get_data(target-pe.OPTIONAL_HEADER.ImageBase, end-target), target))
    assert set(bodies) == wanted
    def targets(insns):
        return {i.operands[0].imm for i in insns if i.mnemonic in {'call', 'jmp'} and i.operands[0].type == X86_OP_IMM} | literal_register_call_targets(insns)
    assert len(inline) == 2 and {x['parent'] for x in inline} == {NAMES[REMEMBER], NAMES[RAND]}
    for instance in inline:
        insns = bodies[instance['parent']]; lo = symbols[instance['parent']]; end = ranges[(instance['parent'], lo)]
        assert all(lo <= a < b <= end for a,b in instance['ranges'])
        calls = set()
        for j,i in enumerate(insns):
            if i.mnemonic != 'call' or not any(a <= i.address < b for a,b in instance['ranges']):
                continue
            if i.operands[0].type == X86_OP_IMM:
                calls.add(i.operands[0].imm)
            else:
                calls.update(literal_register_call_targets(insns[max(0,j-8):j+1]))
        assert calls == {0x498f20, 0x499050}, (instance, calls)
        instance['record_replay_targets'] = [hex(x) for x in sorted(calls)]
        instance['ranges'] = [[hex(a), hex(b)] for a,b in instance['ranges']]
    # The activate pointer stays in callee-saved ESI across a loop and is
    # reused in a distant block. Track constants along actual CFG edges;
    # lexical backward scanning would cross a different path's ESI overwrite.
    insns = bodies[NAMES[BURN]]; by_address = {i.address:j for j,i in enumerate(insns)}
    states = {0:{None}}; work = [0]
    while work:
        j = work.pop(); ins = insns[j]; incoming = states[j]; outgoing = incoming
        if ins.mnemonic == 'mov' and len(ins.operands) == 2 and ins.operands[0].type == X86_OP_REG and ins.operands[0].reg == X86_REG_ESI and ins.operands[1].type == X86_OP_IMM:
            outgoing = {ins.operands[1].imm}
        elif X86_REG_ESI in ins.regs_access()[1]:
            identity = (ins.mnemonic == 'lea' and len(ins.operands) == 2 and ins.operands[0].type == X86_OP_REG and ins.operands[0].reg == X86_REG_ESI and ins.operands[1].type == X86_OP_MEM and ins.operands[1].mem.base == X86_REG_ESI and ins.operands[1].mem.index == 0 and ins.operands[1].mem.disp == 0)
            if not identity:
                outgoing = {None}
        successors = []
        if ins.mnemonic.startswith('j'):
            if ins.operands[0].type == X86_OP_IMM and ins.operands[0].imm in by_address:
                successors.append(by_address[ins.operands[0].imm])
            if ins.mnemonic != 'jmp' and j+1 < len(insns):
                successors.append(j+1)
        elif ins.mnemonic not in {'ret', 'retf'} and j+1 < len(insns):
            successors.append(j+1)
        for k in successors:
            joined = states.get(k,set()) | outgoing
            if joined != states.get(k,set()):
                states[k] = joined; work.append(k)
    cached_activate = []
    for j,ins in enumerate(insns):
        if ins.mnemonic == 'call' and ins.operands[0].type == X86_OP_REG and ins.operands[0].reg == X86_REG_ESI:
            assert states.get(j) == {0x434040}, states.get(j)
            cached_activate.append(dict(address=hex(ins.address), target='0x434040', callee_saved_register='esi'))
    assert len(cached_activate) == 1
    all_targets = set().union(*(targets(v) for v in bodies.values())) | {0x434040}
    assert set(EXE_TARGETS) <= all_targets, [hex(a) for a in set(EXE_TARGETS)-all_targets]
    assert {coff(NAMES[BURN])[0], coff(NAMES[TIMER])[0], coff('recalculate_all_cards_in_play_2022')[0], coff('is_anyone_dead_2022')[0]} <= targets(bodies[NAMES[DECISION]])
    assert coff('finalize_activation_2022')[0] in targets(bodies[NAMES[BURN]])
    stores = [dict(address=hex(i.address), bytes=i.bytes.hex(), target=hex(o.mem.disp), width=o.size) for i in bodies[NAMES[DECISION]] for o in i.operands
              if o.type == X86_OP_MEM and o.access & CS_AC_WRITE and o.mem.disp in {0x4ef3c0, 0x4ef3e0}]
    assert {x['target'] for x in stores} == {'0x4ef3c0', '0x4ef3e0'} and all(x['width'] == 4 for x in stores)
    timer = bodies[NAMES[TIMER]]
    assert any(i.mnemonic == 'idiv' for i in timer) and any(o.type == X86_OP_IMM and o.imm == 5405 for i in timer for o in i.operands)
    return dict(cached_activate_calls=cached_activate, public_symbols={n:hex(a) for n,a in symbols.items()}, folded_public_thunks=folded, private_worker_inline_instances=inline, array_shapes=arrays,
                partial_snapshot_restores=stores, signed_timer_division=True, timer_default_ms=5405, exe_boundaries=[hex(a) for a in sorted(EXE_TARGETS)],
                partial_snapshot_limit='One player row remains uninitialized in the recovered source, as in the native body. Stack-byte equivalence is not established.')
