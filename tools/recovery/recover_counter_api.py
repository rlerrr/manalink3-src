"""Reconstruct the source-aware counter API beside the preserved legacy API.

Uses focused raw exports and explicit, documented interface adaptations. This
does not wire the target's trigger numbering into the old engine.
"""
import argparse
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from promote_recovery import clean


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    # The two same-named get_updated_counters_number entries are a canonical
    # positivity-check wrapper and its register-call worker, not recursion.
    entries = [
        ('26d4540', 'get_updated_counters_number_as_cost', 'counter_number_as_cost_2022', 919, True, 'decompiled-counter-cost'),
        ('26d45f0', 'get_updated_counters_number', 'counter_number_2022_impl', 933, True, 'decompiled-counters'),
        ('26d4810', 'get_updated_counters_number', 'counter_number_2022', 933, True, 'decompiled-counters'),
        ('26d0e90', 'add_or_remove_counters_impl', 'add_or_remove_counters_from_source_impl', 1012, True, 'decompiled-counters'),
        ('26d4840', 'add_counters', 'add_counters_from_source', 1266, False, 'decompiled-counters'),
    ]
    evidence = json.loads((root / 'docs/recovery-evidence/interface-recovery.json').read_text())
    enum_values = evidence['source_aware_counters']['enum_values']
    flags = {
        'ENABLE_XTRIGGER_0': 'COUNTERS_2022_FLAGS_NONE',
        'ENABLE_XTRIGGER_COUNTERS': 'COUNTERS_2022_ENABLE_REPLACEMENT',
        'ENABLE_XTRIGGER_M1_M1_COUNTERS': 'COUNTERS_2022_ENABLE_M1_M1',
        'ENABLE_XTRIGGER_1_1_COUNTERS': 'COUNTERS_2022_ENABLE_P1_P1',
        'XTRIGGER_REPLACE_COUNTERS': 'COUNTERS_2022_REPLACE',
        'XTRIGGER_M1_M1_COUNTERS': 'COUNTERS_2022_M1_M1',
        'XTRIGGER_1_1_COUNTERS': 'COUNTERS_2022_P1_P1',
    }
    addition = '\n\n/* October 2022 source-aware counters. Legacy four-argument APIs above remain.\n'
    addition += ' * Target trigger/mask numbers differ from the old engine; integration is pending. */\n'
    addition += 'int counters_added_type = -1; /* Target .data 0x02721d68, counters.c:7. */\n'
    addition += 'enum {\n' + ''.join(f'  {new} = 0x{enum_values[old]:x},\n' for old, new in flags.items()) + '};\n'
    functions = []
    for address, original, name, line, private, directory in entries:
        raw = (cache / directory / (address + '.c')).read_text()
        body = clean(raw, ['trigger_condition'])
        body = re.sub(r'\b' + original + r'\b', name, body, count=1)
        body = body.replace('get_updated_counters_number_as_cost(', 'counter_number_as_cost_2022(')
        if address == '26d4810':
            body = body.replace('get_updated_counters_number(', 'counter_number_2022_impl(')
        if address == '26d4840':
            body = body.replace('get_updated_counters_number(', 'counter_number_2022(')
            body = body.replace('add_or_remove_counters_impl(', 'add_or_remove_counters_from_source_impl(')
        if address == '26d0e90':
            # Every access through these decompiler pointer views is byte-wide.
            # special_counters is at offset zero; no aggregate is being copied.
            body = body.replace('card_instance_t *pcStack_18;', 'uint8_t *position;')
            body = body.replace('target_t *ptVar9;', 'uint8_t *label;')
            body = body.replace('pcStack_18->special_counters', '*position')
            body = body.replace('ptVar9->player', '*label')
            body = re.sub(r'\bpcStack_18\b', 'position', body)
            body = re.sub(r'\bptVar9\b', 'label', body)
            body = body.replace('(card_instance_t *)', '(uint8_t *)').replace('(target_t *)', '(uint8_t *)')
            body = body.replace('position = instance;', 'position = (uint8_t *)instance;')
            body = body.replace('label = instance->targets + 0x12;', 'label = (uint8_t *)(instance->targets + 0x12);')
            for old, new in flags.items():
                body = re.sub(r'\b' + old + r'\b', new, body)
        if private:
            body = 'static ' + body
        command = ['C:/Program Files/LLVM/bin/clang-format.exe',
                   '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}']
        body = subprocess.run(command, input=body.encode(), capture_output=True, check=True).stdout.decode()
        prototype = body[:body.index('{')].strip() + ';'
        addition += f'\n/* ManalinkEh 2022: 0x{address}; original functions/counters.c:{line}. */\n' + body
        functions.append(dict(name=name, original_name=original, address=address, source='functions/counters.c',
                              line=str(line), static=private, prototype=prototype,
                              raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=[],
                              reconstruction='Focused counter API with distinct source names; byte-wide pointer views normalized.',
                              interface_adaptations=['Preserve legacy counter API; target trigger constants have distinct private names.']))
    destination = root / 'functions/counters.c'
    original_source = destination.read_bytes()
    if b'void add_counters_from_source(' in original_source:
        raise ValueError('Counter API already recovered')
    # Check the actual whole source unit before writing it.
    scratch = cache / 'counter-source-candidate.c'
    scratch.write_bytes(original_source + addition.encode())
    command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
               '-fsyntax-only', '-ferror-limit=0', '-Werror=implicit-function-declaration',
               '-Werror=incompatible-pointer-types', '-Werror=return-type', '-Werror=uninitialized',
               '-Werror=sometimes-uninitialized', '-I', str(root), str(scratch)]
    result = subprocess.run(command, capture_output=True, text=True)
    (cache / 'counter-source-check.log').write_text(result.stderr)
    if result.returncode:
        raise RuntimeError('Counter API syntax/interface check failed; see counter-source-check.log')
    if args.apply:
        destination.write_bytes(original_source.rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        header = root / 'manalink.h'
        content = header.read_bytes()
        marker = content.rfind(b'#ifdef __cplusplus')
        declarations = ('/* October 2022 source-aware counter interface; engine trigger migration is pending. */\n'
                        'extern int counters_added_type;\n'
                        'void add_counters_from_source(int player, int card, int t_player, int t_card, counter_t type, int num);\n\n')
        header.write_bytes(content[:marker] + declarations.replace('\n', '\r\n').encode() + content[marker:])
        manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
        manifest = json.loads(manifest_path.read_text())
        manifest['accepted_functions'].extend(functions)
        manifest['accepted'] = len(manifest['accepted_functions'])
        for address, original, name, line, private, directory in entries:
            shutil.copyfile(cache / directory / (address + '.c'), cache / 'decompiled-abi' / (address + '.c'))
        manifest_path.write_text(json.dumps(manifest, indent=2))
    print(json.dumps(dict(functions=[f['name'] for f in functions], applied=args.apply), indent=2))


if __name__ == '__main__':
    main()
