"""Verify inline card mechanics and the native Ghired AI token-filter correction."""
import json, re, struct
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86 import X86_OP_REG, X86_OP_MEM, X86_OP_IMM, X86_REG_EBP, X86_REG_ESP
from canonicalize_helpers import ROOT, read, sha, span
from inspect_release import Image, value
from global_initializers import inherited_type
from object_initializers import unqualify
from verify_inline_helpers import ranges
from verify_inline_wrapper_recovery import verify as verify_definitions
from verify_native_absolute_bindings import runtime_sections, stable_sections


def function_bounds(debug, name, source_leaf):
    dw = debug.dwarf()
    for cu in dw.iter_CUs():
        top = cu.get_top_DIE()
        if (value(top, 'DW_AT_name') or '').replace('\\', '/').split('/')[-1] != source_leaf:
            continue
        rows = [d for d in top.iter_children() if d.tag == 'DW_TAG_subprogram'
                and value(d, 'DW_AT_name') == name and ranges(dw, d)]
        assert len(rows) == 1, (name, len(rows))
        parts = ranges(dw, rows[0])
        assert len(parts) == 1
        return parts[0]
    raise AssertionError(name)


def symbol_address(debug, image, name):
    matches = {image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[s-1].VirtualAddress + v
               for n, v, s, k, c in debug.symbols if n == '_' + name and s > 0 and k == 32}
    assert len(matches) == 1, (name, matches)
    return next(iter(matches))


def ghired_flag_analysis(image, debug):
    """Follow stack addresses through the test-initialization/selection calls."""
    lo, hi = function_bounds(debug, 'card_ghired_conclave_exile', 'commander_2019.c')
    base = image.pe.OPTIONAL_HEADER.ImageBase
    data = image.pe.get_data(lo-base, hi-lo)
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    ins = list(md.disasm(data, lo))
    assert sum(i.size for i in ins) == len(data)
    default = symbol_address(debug, image, 'default_test_definition')
    select = symbol_address(debug, image, 'check_battlefield_for_special_card')
    registers, arguments = {}, {}
    pending = None
    stores, sites = [], []
    for instruction in ins:
        ops = instruction.operands
        if instruction.mnemonic == 'call':
            target = ops[0].imm if len(ops) == 1 and ops[0].type == X86_OP_IMM else None
            if target == default:
                assert arguments.get(0) is not None
                pending = (instruction.address, arguments[0])
                stores = []
            elif target == select and pending is not None:
                if arguments.get(16) == pending[1]:
                    frame = pending[1]
                    matching = [s for s in stores if s['frame_offset'] == frame + 108]
                    sites.append(dict(default_call=hex(pending[0]), select_call=hex(instruction.address),
                                      test_frame_offset=frame, flag_stores=matching))
                    pending = None
            for register in ['eax', 'ecx', 'edx']:
                registers.pop(register, None)
            continue
        if len(ops) >= 2 and ops[0].type == X86_OP_MEM and instruction.mnemonic == 'mov':
            memory = ops[0].mem
            if memory.base == X86_REG_ESP and memory.index == 0:
                if ops[1].type == X86_OP_REG:
                    arguments[memory.disp] = registers.get(instruction.reg_name(ops[1].reg))
                else:
                    arguments.pop(memory.disp, None)
            if (memory.base == X86_REG_EBP and memory.index == 0 and ops[0].size == 2
                    and ops[1].type == X86_OP_IMM and ops[1].imm == 6):
                stores.append(dict(address=hex(instruction.address), frame_offset=memory.disp,
                                   instruction=[instruction.mnemonic, instruction.op_str]))
        if ops and ops[0].type == X86_OP_REG:
            register = instruction.reg_name(ops[0].reg)
            if (instruction.mnemonic == 'lea' and len(ops) == 2 and ops[1].type == X86_OP_MEM
                    and ops[1].mem.base == X86_REG_EBP and ops[1].mem.index == 0):
                registers[register] = ops[1].mem.disp
            elif instruction.mnemonic == 'mov' and len(ops) == 2 and ops[1].type == X86_OP_REG:
                if instruction.reg_name(ops[1].reg) in registers:
                    registers[register] = registers[instruction.reg_name(ops[1].reg)]
                else:
                    registers.pop(register, None)
            elif instruction.mnemonic not in ['cmp', 'test', 'push']:
                registers.pop(register, None)
    assert len(sites) == 1, sites
    return dict(address=hex(lo), end=hex(hi), bytes_sha256=sha(data),
                instructions=[[hex(i.address), i.mnemonic, i.op_str] for i in ins], sites=sites)


def verify_runtime(record, before, after, root):
    control = record['runtime_control']
    artifact = Image((root / control['path']).read_bytes())
    assert sha(artifact.data) == control['dll_sha256']
    assert stable_sections(runtime_sections(before)) == stable_sections(runtime_sections(artifact))
    # The second incremental build changed exactly one linker input. Preserve
    # these observed manifests and both versions of that object as history;
    # future legitimate rebuilds need not retain the current object directory.
    left, right = record['control_link_inputs'], record['corrected_link_inputs']
    assert set(left) == set(right)
    changed = [p for p in left if left[p] != right[p]]
    assert changed == ['.recovery/build-gcc93/obj/cards/commander_2019.o'], changed
    for key, expected in [('control_ghired_object', left[changed[0]]), ('corrected_ghired_object', right[changed[0]])]:
        obj = record[key]
        assert sha((root / obj['path']).read_bytes()) == obj['sha256'] == expected
    previous, current = record['compiled_ghired_control'], record['compiled_ghired_corrected']
    assert len(previous['sites']) == len(current['sites']) == 1
    assert previous['sites'][0]['flag_stores'] == []
    assert len(current['sites'][0]['flag_stores']) == 1
    for row, image in [(previous, artifact), (current, after)]:
        lo, hi = int(row['address'], 16), int(row['end'], 16)
        data = image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase, hi-lo)
        assert sha(data) == row['bytes_sha256']
        md = Cs(CS_ARCH_X86, CS_MODE_32)
        assert [[hex(i.address), i.mnemonic, i.op_str] for i in md.disasm(data, lo)] == row['instructions']
    # Replay the call/stack/field analysis against the sealed debug sidecars.
    for key, artifact_key, expected in [('control_debug', 'runtime_control', previous),
                                         ('corrected_debug', 'runtime_after', current)]:
        debug = record[key]
        raw = (root / debug['path']).read_bytes()
        assert sha(raw) == debug['sha256']
        target = Image((root / record[artifact_key]['path']).read_bytes())
        assert ghired_flag_analysis(target, Image(raw)) == expected


def verify(proof, image, root=ROOT):
    path = proof.get('native_inline_card_mechanics_recovery')
    if not path:
        return {}
    result = verify_definitions(proof, image, root, record_key='native_inline_card_mechanics_recovery',
        expected_count=17, edit_key='native_inline_card_mechanics_extension', runtime_check=verify_runtime)
    record = json.loads((root / path).read_bytes())
    for row in record['native_support']:
        data = image.pe.get_data(int(row['address'],16)-0x2000000, row['size'])
        assert data.hex() == row['bytes'] and sha(data) == row['sha256']
    table = next(r for r in record['native_support'] if r['address']=='0x275e13c')
    entries = struct.unpack('<53I', bytes.fromhex(table['bytes']))
    truth = []
    for c in range(256):
        index = (c-0x41) & 255
        target = entries[index] if index <= 52 else 0x2662bfd
        assert target in [0x2662bfd, 0x2662cc0]
        is_vowel = target == 0x2662cc0
        assert is_vowel == (chr(c) in 'AEIOUaeiou')
        truth.append(is_vowel)
    assert sum(truth) == 10
    fold = next(r for r in record['native_support'] if r['address']=='0x273f300')
    helper = next(r for r in record['helpers'] if r['name']=='three_nonland_permanents_start_with_same_letter')
    literal = re.search(r'native_name_fold\[256\] = \{(.*?)\};', helper['definition'], re.S).group(1)
    assert bytes(int(n,16) for n in re.findall(r'0x([0-9a-fA-F]+)',literal)).hex() == fold['bytes']
    correction = record['source_correction']
    text = read(root/correction['source'])
    a,b = span(text, correction['name'])
    assert sha(correction['after_definition'].encode()) == correction['after_sha256']
    if text[a:b] != correction['after_definition']:
        from verify_token_aggregate_recovery import verify_superseded_definition
        verify_superseded_definition(proof,image,root,correction['source'],correction['name'],correction['after_sha256'])
    assert sha(correction['before_definition'].encode()) == correction['before_sha256']
    assert correction['before_definition'].count(correction['before']) == 1
    assert correction['before_definition'].replace(correction['before'],correction['after']) == correction['after_definition']
    assert any(e.get('native_inline_card_mechanics_correction')==path and e['before']==correction['before']
               and e['after']==correction['after'] for e in proof['source_edits'])
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    dw = debug.dwarf()
    variable = dw.get_DIE_from_refaddr(int(correction['native_test_variable_die'],16))
    t = unqualify(inherited_type(variable))
    fields = [f for f in t.iter_children() if f.tag=='DW_TAG_member' and value(f,'DW_AT_name')=='type_flag']
    assert len(fields)==1 and value(fields[0],'DW_AT_data_member_location')==108
    assert value(unqualify(inherited_type(fields[0])),'DW_AT_byte_size')==2
    native_analysis = ghired_flag_analysis(image, debug)
    assert native_analysis == record['native_ghired']
    assert len(native_analysis['sites'][0]['flag_stores'])==1
    result.update(native_vowel_byte_inputs=256, native_name_fold_entries=256,
                  corrected_native_callers=1, control_runtime_sections_unchanged=26,
                  only_changed_link_object='cards/commander_2019.o', remaining_inline_review_names=22)
    return result


if __name__ == '__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
