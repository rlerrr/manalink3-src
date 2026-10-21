"""Recover the 2022 selector, auto-target loader and target-picking frontends."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM, X86_OP_MEM
from elftools.dwarf.dwarf_expr import DWARFExprParser
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, strip_comments, code_only, UNRESOLVED
from recover_literals import readonly_string
from audit_source import function_body
from target_selection_bodies import reconstruct, STORAGE


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    directory = cache / 'decompiled-target-selection'
    source = 'functions/targets.c'
    source_text = (root / source).read_bytes().decode('latin1').replace('\r\n', '\n')
    manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
    manifest = json.loads(manifest_path.read_text())
    interface_path = root / 'docs/recovery-evidence/interface-recovery.json'
    interfaces = json.loads(interface_path.read_text())
    assert 'target_validation_2022' in interfaces and 'target_selection_2022' not in interfaces
    focused = {r['address']: r for r in csv.DictReader((directory / 'export.tsv').open(), delimiter='\t')}
    assert len(focused) == 18 and all(r['status'] == 'ok' and r['source'] == source for r in focused.values())
    order = sorted(focused, key=lambda a: (a != '265f3d0', int(a, 16)))
    old_entries = {f['address']: f for f in manifest['accepted_functions']}
    assert set(focused) & set(old_entries) == {'2661490'}
    debug_data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug = Image(debug_data)
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    metadata = {r['low_pc'][2:]: r for r in csv.DictReader((cache / '2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    decoder = Cs(CS_ARCH_X86, CS_MODE_32); decoder.detail = True
    dwarf = debug.dwarf(); expr_parser = DWARFExprParser(dwarf.structs)
    parameters, locals_found, native_globals = {}, {}, {}
    for cu in dwarf.iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != source:
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_variable' and value(die, 'DW_AT_name') in {'auto_targets', 'auto_targets_target', 'read_auto_target_file'}:
                location = expr_parser.parse_expr(die.attributes['DW_AT_location'].value)
                assert len(location) == 1 and location[0].op_name == 'DW_OP_addr'
                native_globals[value(die, 'DW_AT_name')] = dict(address=f'{location[0].args[0]:x}', type=type_name(type_of(die)))
            at = value(die, 'DW_AT_low_pc'); address = f'{at:x}' if at else ''
            if die.tag != 'DW_TAG_subprogram' or address not in focused:
                continue
            ps = []
            for p in die.iter_children():
                if p.tag != 'DW_TAG_formal_parameter':
                    continue
                name = inherited_value(p, 'DW_AT_name'); location = p.attributes.get('DW_AT_location')
                assert location and value(p, 'DW_AT_const_value') is None
                pointer = type_of(p)
                readonly_prompt = name == 'prompt' and pointer.tag == 'DW_TAG_pointer_type'
                if readonly_prompt:
                    assert type_of(pointer).tag == 'DW_TAG_const_type'
                ps.append(dict(name=name, type=type_name(pointer), location=location.value, location_form=location.form,
                               readonly_prompt=readonly_prompt))
            parameters[address] = ps
            def walk(parent):
                for child in parent.iter_children():
                    location = child.attributes.get('DW_AT_location')
                    if child.tag == 'DW_TAG_variable' and location and isinstance(location.value, list):
                        ops = expr_parser.parse_expr(location.value)
                        if len(ops) == 1 and ops[0].op_name == 'DW_OP_fbreg':
                            locals_found.setdefault(address, []).append(dict(name=inherited_value(child, 'DW_AT_name'),
                                type=type_name(type_of(child)), frame_offset=ops[0].args[0], die=hex(child.offset)))
                    if child.has_children:
                        walk(child)
            walk(die)
        break
    assert set(parameters) == set(focused)
    assert len(parameters['265f3d0']) == len(parameters['265fb20']) == 23
    selector_locals = {(v['name'], v['type'], v['frame_offset']) for v in locals_found['265f3d0']}
    assert {('err_reason', 'char[208]', -500), ('illegal_tgt', 'char[200]', -500),
            ('candidates', 'target_t[60]', -500), ('illegal_tgt', 'char[200]', -700),
            ('tmp_tgt', 'target_t', -708), ('unknown_v44', 'int', -712)} <= selector_locals
    assert native_globals == {'auto_targets': dict(address='2796840', type='int[500]'),
                              'auto_targets_target': dict(address='2796060', type='int[500]'),
                              'read_auto_target_file': dict(address='2774800', type='int')}
    assert all(declaration in source_text for declaration in ['int auto_targets[500];', 'int auto_targets_target[500];', 'int read_auto_target_file = 0;'])
    runtime.pe.parse_data_directories()
    imports = [(lib.dll.decode(), item.name.decode(), item.address) for lib in runtime.pe.DIRECTORY_ENTRY_IMPORT
               for item in lib.imports if item.name == b'fscanf']
    assert imports == [('msvcrt.dll', 'fscanf', 0x27b54b4)]

    def machine(at, length):
        return runtime.pe.get_data(at - 0x2000000, length)

    def site(at):
        instruction = next(decoder.disasm(machine(int(at, 16), 15), int(at, 16), count=1))
        return dict(address=at, bytes=instruction.bytes.hex(), instruction=instruction.mnemonic + ' ' + instruction.op_str)

    assert machine(0x2701e58, 6).hex() == 'ff25b4547b02'
    proof = dict(debug_sha256=hashlib.sha256(debug_data).hexdigest(), functions={}, parameters=parameters,
                 locals=locals_found, shared_storage_declaration=STORAGE, shared_storage_bytes=480,
                 native_globals=native_globals, frame_base_bias_from_ebp=8,
                 fscanf=dict(thunk='2701e58', bytes='ff25b4547b02', iat='27b54b4', dll='msvcrt.dll', name='fscanf', fixed_arguments=2, variadic=True),
                 scope='Versioned 2022 target selection and picking family. Preserve flags, AI preference fallback, '
                       'candidate limit, player candidates, engine selection encoding, count/cancellation and marker cleanup. '
                       'Human errors and sixty AI targets share a verified 480-byte union. Localized engine strings remain live.',
                 integration_pending='Aura/generic-spell and card callers still need migration to the complete 84-byte API. '
                                     'Original targeting APIs and callers remain available; no game execution performed.')
    proof['candidate_bound_instructions'] = [site(at) for at in ['265f737', '265f738', '265f73b']]
    assert [s['bytes'] for s in proof['candidate_bound_instructions'][:2]] == ['47', '83ff39']
    assert proof['candidate_bound_instructions'][2]['instruction'] == 'jg 0x265f755'
    encoding = machine(0x265fa1f, 0x265fa47 - 0x265fa1f)
    assert all(bytes.fromhex(part) in encoding for part in ['0f9ec2', '81e200010000', '8a8cc518feffff', '80cc40'])
    mapping = {r['name']: r['name'] + '_2022' for a, r in focused.items() if a != '2661490'}
    mapping.update({f['name'][:-5]: f['name'] for f in manifest['accepted_functions']
                    if f['source'] == source and f['name'].endswith('_2022')})
    mapping.update(target_definition_t='target_definition_2022_t', load_text='load_text_2022')
    targets = {int(a, 16): mapping.get(r['name'], r['name']) for a, r in focused.items()}
    targets.update({int(f['address'], 16): f['name'] for f in manifest['accepted_functions']
                    if f['source'] == source or f['name'] in {'check_special_flags_2022', 'load_text_2022'}})
    # The corrected chooser keeps its public name; every new function gets its versioned name.
    targets[0x2661490] = 'choose_player'; targets[0x2701e58] = 'fscanf'
    entries, bodies = [], {}
    legacy_vla = 'int ' + function_body(source_text, 'pick_up_to_n_targets_noload')
    vla_literals = [('275deb6', 'TARGET_COUNT', '2660fb0'), ('275dec3', 'FIREBALL', '2661107')]
    vla_ranges = [('2660ee4', '2660f1e'), ('2660f32', '2660f44'), ('2661056', '266108a'),
                  ('266108a', '26610b6'), ('2660f5c', '2660fa7'), ('2660ff8', '2661026'), ('26610be', '2661127')]
    for address in order:
        row = metadata[address]; original_name = row['name']; name = mapping.get(original_name, original_name)
        raw = (directory / (address + '.c')).read_text(); text = strip_comments(raw)
        coff = [dict(name=n, storage=s) for n, off, sec, kind, s in debug.functions()
                if debug.pe.OPTIONAL_HEADER.ImageBase + debug.pe.sections[sec - 1].VirtualAddress + off == int(address, 16)]
        private = address in {'265f3d0', '265fb20'}
        assert len(coff) == 1 and coff[0]['storage'] == (3 if private else 2)
        changes = []
        if address == '265f3d0':
            text, changes = reconstruct(text)
        elif address == '2660eb0':
            change = dict(original=text, replacement=legacy_vla, occurrences=1, kind='VLA call-frame reconstruction',
                          source_template_name='pick_up_to_n_targets_noload',
                          source_template_sha256=hashlib.sha256(function_body(source_text, 'pick_up_to_n_targets_noload').encode()).hexdigest(),
                          logical_size_addend=50, native_alignment=4, literals=[], ranges=[])
            for pointer, literal, at in vla_literals:
                data = readonly_string(runtime, int(pointer, 16)); assert data == literal.encode()
                instruction = site(at); assert instruction['bytes'].endswith(int(pointer, 16).to_bytes(4, 'little').hex())
                change['literals'].append(dict(address=pointer, bytes=data.hex(), instruction=instruction))
            for start, end in vla_ranges:
                data = machine(int(start, 16), int(end, 16) - int(start, 16))
                instructions = list(decoder.disasm(data, int(start, 16)))
                assert sum(i.size for i in instructions) == len(data)
                change['ranges'].append(dict(start=start, end=end, bytes=len(data), sha256=hashlib.sha256(data).hexdigest()))
            assert machine(0x2660f32, 18).hex() == 'c7459025732025c7459464202564c6459800'
            assert machine(0x2660f01, 5).hex() == 'e8cab20900'
            change['format_word_stores'] = [site(at) for at in ['2660f32', '2660f39', '2660f40']]
            text = legacy_vla; changes = [change]
        for change in changes:
            change['machine_sites'] = [site(at) for at in change.get('sites', [])]
            if 'machine_range' in change:
                start, end = (int(a, 16) for a in change['machine_range'])
                data = machine(start, end - start)
                change['machine_bytes'] = len(data); change['machine_sha256'] = hashlib.sha256(data).hexdigest()
            pointer_addresses = change.get('addresses', [change['address']] if 'address' in change else [])
            if pointer_addresses:
                code = machine(int(address, 16), int(row['high_pc'], 16) - int(address, 16))
                for pointer in pointer_addresses:
                    assert re.search(r'0x0*' + re.escape(pointer) + r'\b', source_text, re.I), pointer
                    sites = [dict(address=f'{i.address:x}', bytes=i.bytes.hex(), instruction=i.mnemonic + ' ' + i.op_str)
                             for i in decoder.disasm(code, int(address, 16)) if any(
                                 (op.type == X86_OP_IMM and op.imm == int(pointer, 16)) or
                                 (op.type == X86_OP_MEM and op.mem.disp == int(pointer, 16)) for op in i.operands)]
                    assert sites, pointer
                    change.setdefault('pointer_operands', []).append(dict(address=pointer, sites=sites))
        text = re.sub(r'\b(?:' + '|'.join(mapping) + r')\b', lambda m: mapping[m[0]], text)
        if any(p['readonly_prompt'] for p in parameters[address]):
            text, count = re.subn(r'(?<!const )\bchar\s*\*\s*prompt\b', 'const char *prompt', text)
            assert count == (0 if address == '2660eb0' else 1)
        if address == '2660eb0':
            text = re.sub(r'\bcancel\b', 'spell_fizzled', text)
        body = clean(text, use_keyword_accessors_2022=True, use_extension_flags_2022=True)
        if private:
            body = 'static ' + body
        assert not UNRESOLVED.search(code_only(body)), (original_name, UNRESOLVED.search(code_only(body)))
        body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
                               '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
                              input=body.encode(), capture_output=True, check=True).stdout.decode().replace('\r\n', '\n')
        code = machine(int(address, 16), int(row['high_pc'], 16) - int(address, 16)); calls = []
        for i in decoder.disasm(code, int(address, 16)):
            if i.mnemonic in {'call', 'jmp'} and i.operands[0].type == X86_OP_IMM and i.operands[0].imm in targets:
                destination = targets[i.operands[0].imm]
                assert re.search(r'\b' + re.escape(destination) + r'\s*\(', body), (name, destination)
                calls.append(dict(address=f'{i.address:x}', bytes=i.bytes.hex(), target=f'{i.operands[0].imm:x}', name=destination))
        detail = dict(machine_bytes=len(code), machine_sha256=hashlib.sha256(code).hexdigest(), changes=changes, canonical_calls=calls, coff=coff[0])
        if address == '2660930':
            assert sum(c['name'] == 'fscanf' for c in calls) == 8
        proof['functions'][address] = detail
        entry = dict(name=name, address=address, source=source, line=row['line'], static=private,
                     prototype=body[:body.index('{')].strip() + ';', raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=[],
                     target_selection_evidence=detail, reconstruction=proof['scope'],
                     reconstruction_body_sha256=hashlib.sha256(function_body(body, name).encode()).hexdigest())
        entries.append(entry); bodies[address] = body
    # The compatible chooser API is migrated in place; its target object now has all 84 bytes.
    before = function_body(source_text, 'choose_player'); updated = function_body(bodies['2661490'], 'choose_player')
    proof['chooser_migration'] = dict(address='2661490', previous_body_sha256=hashlib.sha256(before.encode()).hexdigest(),
                                     body_sha256=hashlib.sha256(updated.encode()).hexdigest())
    candidate = source_text.replace(before, updated) + '\n\n#ifndef SHANDALAR\n' + STORAGE
    for e in entries:
        if e['address'] != '2661490':
            candidate += f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n" + bodies[e['address']] + '\n'
    candidate += '#endif\n'
    header = (root / 'manalink.h').read_bytes().decode('latin1').replace('\r\n', '\n'); at = header.rfind('#ifdef __cplusplus')
    block = '/* October 2022 selection/picking APIs; require the 84-byte definition. */\n#ifndef SHANDALAR\n'
    block += '\n'.join(e['prototype'] for e in entries if not e['static'] and e['address'] != '2661490') + '\n#endif\n\n'
    header = header[:at] + block + header[at:]
    scratch = cache / 'target-selection-check'; scratch.mkdir(exist_ok=True)
    (scratch / 'targets.c').write_bytes(candidate.encode('latin1')); (scratch / 'manalink.h').write_bytes(header.encode('latin1'))
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99', '-fsyntax-only',
                             '-ferror-limit=0', '-Werror=implicit-function-declaration', '-Werror=implicit-int', '-Werror=incompatible-pointer-types',
                             '-Werror=return-type', '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-Werror=format',
                             '-I', str(scratch), '-I', str(root), str(scratch / 'targets.c')], capture_output=True, text=True)
    (scratch / 'check.log').write_text(result.stderr); result.check_returncode()
    path = cache / 'decompiled-abi/export.tsv'; rows = list(csv.DictReader(path.open(), delimiter='\t')); existing = {r['address'] for r in rows}
    rows = [focused[r['address']] if r['address'] in focused else r for r in rows]
    rows.extend(focused[a] for a in focused if a not in existing)
    if args.apply:
        for address in focused:
            if address != '2661490':
                assert all(address not in json.dumps(f) for f in manifest['accepted_functions'] + manifest['source_fragments'])
        assert not any('2661490' in json.dumps(f) for f in manifest['accepted_functions'] if f['address'] != '2661490')
        (root / source).write_bytes(candidate.replace('\n', '\r\n').encode('latin1')); (root / 'manalink.h').write_bytes(header.replace('\n', '\r\n').encode('latin1'))
        merged = {f['address']: f for f in manifest['accepted_functions']}; merged.update({e['address']: e for e in entries})
        manifest['accepted_functions'] = list(merged.values()); manifest['accepted'] = len(merged)
        manifest['helper_functions'] = manifest['accepted'] - manifest['card_handler_functions']; manifest['source_files'][source] += 17
        manifest['private_functions'] += 2; manifest['private_helpers'] += 2; manifest['shared_helper_interfaces'] += 15
        manifest['raw_export_entries'] = len(rows); manifest['exported_functions'] = len({r['name'] for r in rows})
        manifest_path.write_text(json.dumps(manifest, indent=2)); interfaces['target_selection_2022'] = proof; interface_path.write_text(json.dumps(interfaces, indent=2))
        with path.open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=rows[0], delimiter='\t'); writer.writeheader(); writer.writerows(rows)
        for address in focused:
            shutil.copyfile(directory / (address + '.c'), path.parent / (address + '.c'))
    print(json.dumps(dict(applied=args.apply, new_helpers=17, corrected_existing_choosers=1, private_workers=2, public_frontends=15,
                          shared_candidate_storage_bytes=480, candidate_capacity=60, reconstructed_vla_helpers=1)))


if __name__ == '__main__':
    main()
