"""Recover the 2022 target validator, availability workers and compatible versioned frontends."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM, X86_OP_MEM, X86_REG_ESI, X86_REG_ECX
from elftools.dwarf.dwarf_expr import DWARFExprParser
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, strip_comments, code_only, UNRESOLVED
from audit_source import function_body
from target_validation_bodies import reconstruct


ENTRIES = {
    '265d1a0': ('target_match_type', True),
    '265d720': ('validate_target_impl', True),
    '265ef60': ('target_available_impl', True),
    '265f1f0': ('target_available_maybe_force', True),
    '265fcf0': ('real_validate_target', False),
    '265fe80': ('real_target_available', False),
    '26604c0': ('can_target', False),
    '2660570': ('target_available', False),
    '2660750': ('would_validate_arbitrary_target', False),
    '2660810': ('validate_arbitrary_target', False),
    '2660850': ('would_validate_target', False),
    '2661360': ('would_valid_target', False),
    '2661710': ('validate_target', False),
    '2661750': ('valid_target', False),
}


def merged_exports(cache, directory):
    rows = list(csv.DictReader((cache / 'decompiled-abi/export.tsv').open(), delimiter='\t'))
    focused = {r['address']: r for r in csv.DictReader((directory / 'export.tsv').open(), delimiter='\t')}
    assert set(ENTRIES) <= set(focused)
    existing = {r['address'] for r in rows}
    rows = [focused[r['address']] if r['address'] in ENTRIES else r for r in rows]
    rows.extend(focused[address] for address in ENTRIES if address not in existing)
    assert len(rows) == len({r['address'] for r in rows})
    return rows


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    directory = cache / 'decompiled-target-core'
    source = 'functions/targets.c'
    debug_data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug = Image(debug_data)
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    metadata = {r['low_pc'][2:]: r for r in csv.DictReader((cache / '2022-functions.csv').open())
                if r['low_pc'] and r['high_pc']}
    manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
    manifest = json.loads(manifest_path.read_text())
    assert not set(ENTRIES) & {f['address'] for f in manifest['accepted_functions']}
    interfaces_path = root / 'docs/recovery-evidence/interface-recovery.json'
    interfaces = json.loads(interfaces_path.read_text())
    assert interfaces['target_initializers_2022']['layout']['bytes'] == 84
    dwarf = debug.dwarf()
    parameters, locals_found, card_members, predicate = {}, {}, {}, None
    for cu in dwarf.iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != source:
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_typedef' and value(die, 'DW_AT_name') == 'card_data_t':
                record = type_of(die)
                assert value(record, 'DW_AT_byte_size') == 72
                card_members = {value(m, 'DW_AT_name'): dict(offset=value(m, 'DW_AT_data_member_location'), type=type_name(type_of(m)))
                                for m in record.iter_children() if m.tag == 'DW_TAG_member' and value(m, 'DW_AT_name') in {'id', 'reserved2', 'type'}}
            at = value(die, 'DW_AT_low_pc')
            address = f'{at:x}' if at else ''
            if die.tag != 'DW_TAG_subprogram' or address not in ENTRIES:
                continue
            ps, constants = [], []
            for p in die.iter_children():
                if p.tag != 'DW_TAG_formal_parameter':
                    continue
                name = inherited_value(p, 'DW_AT_name')
                const = value(p, 'DW_AT_const_value')
                location = p.attributes.get('DW_AT_location')
                if location is None:
                    assert address == '265f1f0' and name == 'test_damage_target_or_source' and const == 0
                    constants.append(dict(name=name, value=const, physical_argument=False))
                    continue
                assert const is None
                ps.append(dict(name=name, type=type_name(type_of(p)), location=location.value, location_form=location.form))
            parameters[address] = dict(physical=ps, abstract_constants=constants)
            if address == '265d720':
                def walk(parent):
                    nonlocal predicate
                    for child in parent.iter_children():
                        name = inherited_value(child, 'DW_AT_name')
                        if child.tag == 'DW_TAG_typedef' and name == 'Predicate':
                            pointer = type_of(child)
                            callback = type_of(pointer)
                            assert pointer.tag == 'DW_TAG_pointer_type' and callback.tag == 'DW_TAG_subroutine_type'
                            return_type = type_of(callback)
                            assert return_type.tag == 'DW_TAG_pointer_type' and type_of(return_type).tag == 'DW_TAG_const_type'
                            predicate = dict(die=hex(child.offset), return_type='const char *',
                                             parameters=[type_name(type_of(p)) for p in callback.iter_children()])
                        if child.tag == 'DW_TAG_variable' and name in {'error_str', 'buffer'}:
                            location = child.attributes['DW_AT_location']
                            ops = DWARFExprParser(dwarf.structs).parse_expr(location.value)
                            assert len(ops) == 1 and ops[0].op_name == 'DW_OP_fbreg'
                            locals_found[name] = dict(die=hex(child.offset), type=type_name(type_of(child)), frame_offset=ops[0].args[0])
                        if child.has_children:
                            walk(child)
                walk(die)
        break
    assert set(parameters) == set(ENTRIES)
    assert len(parameters['265d720']['physical']) == 23
    assert len(parameters['265ef60']['physical']) == len(parameters['265f1f0']['physical']) == 22
    assert locals_found['error_str']['type'] == 'char[200]' and locals_found['error_str']['frame_offset'] == -220
    assert locals_found['buffer']['type'] == 'char[100]' and locals_found['buffer']['frame_offset'] == -320
    assert predicate and predicate['parameters'] == ['int'] * 5
    assert card_members == {'id': dict(offset=36, type='uint16_t'), 'reserved2': dict(offset=38, type='uint16_t'), 'type': dict(offset=40, type='uint8_t')}
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.detail = True
    proof = dict(debug_sha256=hashlib.sha256(debug_data).hexdigest(), functions={}, parameters=parameters,
                 validator_locals=locals_found, predicate=predicate, frame_base_bias_from_ebp=8, card_data_members=card_members,
                 scope='Versioned 2022 target validation/availability component. Preserve the native gameplay branches, '
                       'flags forwarding, count and AI fallback behavior. Recover inlined string appends using the '
                       'verified adjacent 100-byte/200-byte local buffers, native scan/copy lengths and literal bytes. '
                       'Localized executable error strings remain live pointers. Old targeting APIs stay intact.',
                 integration_pending='Target selection/UI workers and downstream Aura/generic-spell callers remain pending; '
                                     'no legacy caller is silently migrated and no game execution is performed.')

    def machine(at, length):
        return runtime.pe.get_data(at - 0x2000000, length)

    def record_site(address):
        at = int(address, 16)
        instruction = next(decoder.disasm(machine(at, 15), at, count=1))
        return dict(address=address, bytes=instruction.bytes.hex(), instruction=instruction.mnemonic + ' ' + instruction.op_str)

    bodies, entries = {}, []
    source_text = (root / source).read_bytes().decode('latin1').replace('\r\n', '\n')
    target_addresses = {int(a, 16): name + '_2022' for a, (name, private) in ENTRIES.items()}
    target_addresses.update({int(f['address'], 16): f['name'] for f in manifest['accepted_functions']
                             if f['name'] in {'is_protected_from_2022', 'check_for_special_ability_2022',
                                              'check_special_flags_2022', 'is_humiliated_for_ability'}})
    for address, (original_name, private) in ENTRIES.items():
        row = metadata[address]
        assert row['source'] == source and row['name'] == original_name
        coff = [dict(name=n, storage=s) for n, offset, sec, kind, s in debug.functions()
                if debug.pe.OPTIONAL_HEADER.ImageBase + debug.pe.sections[sec - 1].VirtualAddress + offset == int(address, 16)]
        assert len(coff) == 1 and coff[0]['storage'] == (3 if private else 2)
        raw = (directory / (address + '.c')).read_text()
        text = strip_comments(raw)
        changes = []
        if address == '265d720':
            text, changes = reconstruct(text)
        elif address == '265d1a0':
            old = '*(int *)&cards_data[iVar1].id'
            new = '((uint32_t)cards_data[iVar1].id | ((uint32_t)cards_data[iVar1].reserved2 << 16))'
            assert text.count(old) == 1
            text = text.replace(old, new)
            changes.append(dict(original=old, replacement=new, occurrences=1, kind='native adjacent ID word', sites=['265d213', '265d21a', '265d21f']))
        elif address == '265ef60':
            old, new = 'DAT_0060e9fc', 'EXE_DWORD(0x60e9fc)'
            assert text.count(old) == 1 and 'EXE_DWORD(0x60E9FC)' in source_text
            text = text.replace(old, new)
            code = machine(int(address, 16), int(row['high_pc'], 16) - int(address, 16))
            sites = [f'{i.address:x}' for i in decoder.disasm(code, int(address, 16))
                     if any(op.type == X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0 and op.mem.disp == 0x60e9fc for op in i.operands)]
            assert len(sites) == 1
            changes.append(dict(original=old, replacement=new, occurrences=1, kind='native engine chooser flag', sites=sites))
        for change in changes:
            change['machine_sites'] = [record_site(at) for at in change.get('sites', [])]
            if change['kind'] == 'byte initialization':
                assert all(s['bytes'] == 'c6852cffffff00' for s in change['machine_sites'])
            elif change['kind'] == 'five-argument predicate':
                assert change['machine_sites'][0]['bytes'] == 'ff5538'
            elif change['kind'] == 'low-bit boolean':
                assert [s['bytes'] for s in change['machine_sites']] == ['8b4514', '83e001', 'f6470910', '85c0', '0f94c0', '84c0']
            if 'machine_range' in change:
                start, end = (int(a, 16) for a in change['machine_range'])
                code = machine(start, end - start)
                instructions = list(decoder.disasm(code, start))
                assert sum(i.size for i in instructions) == len(code)
                change['machine_bytes'] = len(code)
                change['machine_sha256'] = hashlib.sha256(code).hexdigest()
                if change['kind'] in {'inline literal append', 'inline five-word append'}:
                    assert sum('scasb' in i.mnemonic for i in instructions) == 1
                    copy = next(i for i in instructions if i.mnemonic in {'rep movsb', 'rep movsd'})
                    pointer = next(i.operands[1].imm for i in instructions if i.mnemonic == 'mov' and
                                   i.operands[0].type != X86_OP_MEM and i.operands[0].reg == X86_REG_ESI and i.operands[1].type == X86_OP_IMM)
                    count = next(i.operands[1].imm for i in instructions if i.mnemonic == 'mov' and
                                 i.operands[0].type != X86_OP_MEM and i.operands[0].reg == X86_REG_ECX and i.operands[1].type == X86_OP_IMM)
                    size = count * (4 if copy.mnemonic == 'rep movsd' else 1)
                    literal = machine(pointer, size)
                    expected = (change['literal'] if change['kind'] == 'inline literal append' else ',cannot target self').encode() + b'\0'
                    assert literal == expected and len(literal) == size
                    if change['kind'] == 'inline five-word append':
                        assert pointer == int(change['literal_address'], 16) and size == change['literal_bytes'] == 20
                    if change.get('prefix_comma'):
                        assert any(i.mnemonic == 'mov' and i.operands[0].type == X86_OP_MEM and i.operands[0].size == 1 and
                                   i.operands[1].type == X86_OP_IMM and i.operands[1].imm == 44 for i in instructions)
                    change['native_literal'] = dict(address=f'{pointer:x}', bytes=literal.hex(), copy_bytes=size)
                elif change['kind'] in {'inline predicate comma', 'inline formatted comma'}:
                    assert any(i.mnemonic == 'mov' and i.operands[0].type == X86_OP_MEM and i.operands[0].size == 2 and
                               i.operands[1].type == X86_OP_IMM and i.operands[1].imm == 44 for i in instructions)
                elif change['kind'] == 'inline effect append':
                    assert all(bytes.fromhex(b) in code for b in ['c6040a2c', 'c70065666665', '66c740046374', 'c6400600'])
            if change['kind'] == 'live engine error string':
                literal = '0x' + change['address']
                assert re.search(re.escape(literal) + r'\b', source_text, re.I)
                code = machine(int(address, 16), int(row['high_pc'], 16) - int(address, 16))
                sites = [f'{i.address:x}' for i in decoder.disasm(code, int(address, 16)) if i.mnemonic == 'mov' and
                         any(op.type == X86_OP_IMM and op.imm == int(change['address'], 16) for op in i.operands)]
                assert sites
                change['machine_sites'] = [record_site(at) for at in sites]
        mapping = {name: name + '_2022' for name, private in ENTRIES.values()}
        mapping['target_definition_t'] = 'target_definition_2022_t'
        text = re.sub(r'\b(?:' + '|'.join(mapping) + r')\b', lambda m: mapping[m[0]], text)
        body = clean(text, use_keyword_accessors_2022=True, use_extension_flags_2022=True)
        if private:
            body = 'static ' + body
        assert not UNRESOLVED.search(code_only(body)), (original_name, UNRESOLVED.search(code_only(body)))
        body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
                               '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
                              input=body.encode(), capture_output=True, check=True).stdout.decode().replace('\r\n', '\n')
        code = machine(int(address, 16), int(row['high_pc'], 16) - int(address, 16))
        calls = []
        for i in decoder.disasm(code, int(address, 16)):
            if i.mnemonic in {'call', 'jmp'} and i.operands[0].type == X86_OP_IMM and i.operands[0].imm in target_addresses:
                destination = target_addresses[i.operands[0].imm]
                assert re.search(r'\b' + destination + r'\s*\(', body), (original_name, destination)
                calls.append(dict(address=f'{i.address:x}', bytes=i.bytes.hex(), target=f'{i.operands[0].imm:x}', name=destination))
        detail = dict(machine_bytes=len(code), machine_sha256=hashlib.sha256(code).hexdigest(), coff=coff[0],
                      changes=changes, canonical_calls=calls)
        proof['functions'][address] = detail
        name = original_name + '_2022'
        entries.append(dict(name=name, address=address, source=source, line=row['line'], static=private,
                            prototype=body[:body.index('{')].strip() + ';', raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),
                            recovered_literals=[], target_validation_evidence=detail, reconstruction=proof['scope'],
                            reconstruction_body_sha256=hashlib.sha256(function_body(body, name).encode()).hexdigest()))
        bodies[address] = body
    # The already recovered protection dispatcher is another native caller of
    # the private getter. Complete that binding now that its body is available.
    old_worker = function_body(source_text, 'has_protection_from_impl')
    assert old_worker.count('get_protections_from(') == 1
    new_worker = old_worker.replace('get_protections_from(', 'get_protections_from_part_2022(')
    getter_call = record_site('265d5ac')
    assert getter_call['bytes'] == 'e82ffdffff'
    binding = dict(address='265d580', name='has_protection_from_impl',
                   original='get_protections_from', replacement='get_protections_from_part_2022',
                   previous_body_sha256=hashlib.sha256(old_worker.encode()).hexdigest(),
                   body_sha256=hashlib.sha256(new_worker.encode()).hexdigest(),
                   call=dict(**getter_call, target='265d2e0'))
    proof['protection_getter_binding'] = binding
    candidate = source_text.replace(old_worker, new_worker)
    marker = '/* ManalinkEh 2022: 0x265d580; original functions/targets.c:107. */'
    assert candidate.count(marker) == 1
    candidate = candidate.replace(marker, '#ifndef SHANDALAR\nstatic int get_protections_from_part_2022(int player, int card);\n#endif\n\n' + marker)
    candidate += '\n\n#ifndef SHANDALAR\n'
    for e in entries:
        candidate += f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n" + bodies[e['address']] + '\n'
    candidate += '#endif\n'
    header = (root / 'manalink.h').read_bytes().decode('latin1').replace('\r\n', '\n')
    at = header.rfind('#ifdef __cplusplus')
    block = '/* October 2022 target validation/availability; require the 84-byte definition. */\n#ifndef SHANDALAR\n'
    block += '\n'.join(e['prototype'] for e in entries if not e['static']) + '\n#endif\n\n'
    header = header[:at] + block + header[at:]
    scratch = cache / 'target-validation-check'
    scratch.mkdir(exist_ok=True)
    (scratch / 'targets.c').write_bytes(candidate.encode('latin1'))
    (scratch / 'manalink.h').write_bytes(header.encode('latin1'))
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
                             '-fsyntax-only', '-ferror-limit=0', '-Werror=implicit-function-declaration', '-Werror=implicit-int',
                             '-Werror=incompatible-pointer-types', '-Werror=return-type', '-Werror=uninitialized',
                             '-Werror=sometimes-uninitialized', '-Werror=format', '-I', str(scratch), '-I', str(root),
                             str(scratch / 'targets.c')], capture_output=True, text=True)
    (scratch / 'check.log').write_text(result.stderr)
    result.check_returncode()
    export_rows = merged_exports(cache, directory)
    if args.apply:
        for address in ENTRIES:
            assert all(address not in json.dumps(f) for f in manifest['accepted_functions'] + manifest['source_fragments'])
        (root / source).write_bytes(candidate.replace('\n', '\r\n').encode('latin1'))
        (root / 'manalink.h').write_bytes(header.replace('\n', '\r\n').encode('latin1'))
        manifest['accepted_functions'].extend(entries)
        for entry in manifest['accepted_functions']:
            if entry['address'] == binding['address']:
                assert entry['reconstruction_body_sha256'] == binding['previous_body_sha256']
                entry['target_getter_binding_2022'] = binding
                entry['reconstruction_body_sha256'] = binding['body_sha256']
        manifest['accepted'] = len(manifest['accepted_functions'])
        manifest['helper_functions'] = manifest['accepted'] - manifest['card_handler_functions']
        manifest['source_files'][source] += len(entries)
        manifest['private_functions'] += sum(e['static'] for e in entries)
        manifest['private_helpers'] += sum(e['static'] for e in entries)
        manifest['shared_helper_interfaces'] += sum(not e['static'] for e in entries)
        manifest['raw_export_entries'] = len(export_rows)
        manifest['exported_functions'] = len({r['name'] for r in export_rows})
        manifest_path.write_text(json.dumps(manifest, indent=2))
        interfaces['target_validation_2022'] = proof
        interfaces_path.write_text(json.dumps(interfaces, indent=2))
        path = cache / 'decompiled-abi/export.tsv'
        with path.open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=export_rows[0], delimiter='\t')
            writer.writeheader(); writer.writerows(export_rows)
        for address in ENTRIES:
            shutil.copyfile(directory / (address + '.c'), path.parent / (address + '.c'))
    print(json.dumps(dict(applied=args.apply, implementations=len(entries), private_workers=4, public_frontends=10,
                          corrected_existing_getter_bindings=1,
                          validator_buffer_bytes=200, formatted_buffer_bytes=100,
                          reconstructed_appends=sum(c['kind'].startswith('inline') for c in proof['functions']['265d720']['changes']))))


if __name__ == '__main__':
    main()
