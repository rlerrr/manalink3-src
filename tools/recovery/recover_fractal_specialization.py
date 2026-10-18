"""Recover the quantity-one Fractal clone and its token counter-action fragment."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from elftools.dwarf.locationlists import LocationParser, LocationExpr, BaseAddressEntry
from inspect_release import Image, inherited_value, value
from promote_recovery import clean


def digest(text):
    return hashlib.sha256(text.encode()).hexdigest()


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--prepare', action='store_true')
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    image = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    dwarf = image.dwarf()
    locations = LocationParser(dwarf.location_lists())
    evidence = {}
    for cu in dwarf.iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'cards/strixhaven.c':
            continue
        for die in cu.iter_DIEs():
            name = value(die, 'DW_AT_name')
            if die.tag == 'DW_TAG_enumerator' and name in {'TOKEN_ACTION_ADD_COUNTERS', 'COUNTER_P1_P1'}:
                evidence[name] = value(die, 'DW_AT_const_value')
            if value(die, 'DW_AT_low_pc') != 0x23c8b60:
                continue
            evidence['line'] = str(inherited_value(die, 'DW_AT_decl_line'))
            parameters = []
            for parameter in die.iter_children():
                if parameter.tag != 'DW_TAG_formal_parameter':
                    continue
                expression = None
                attribute = parameter.attributes.get('DW_AT_location')
                if attribute:
                    loc = locations.parse_from_attribute(attribute, cu['version'], parameter)
                    if isinstance(loc, LocationExpr):
                        expression = loc.loc_expr
                    else:
                        base = value(cu.get_top_DIE(), 'DW_AT_low_pc', 0)
                        for entry in loc:
                            if isinstance(entry, BaseAddressEntry):
                                base = entry.base_address
                            else:
                                begin = entry.begin_offset if entry.is_absolute else base + entry.begin_offset
                                end = entry.end_offset if entry.is_absolute else base + entry.end_offset
                                if begin <= 0x23c8b60 < end:
                                    expression = entry.loc_expr
                                    break
                parameters.append(dict(name=inherited_value(parameter, 'DW_AT_name'),
                                       constant=value(parameter, 'DW_AT_const_value'), entry_location=expression))
            evidence['parameters'] = parameters
        break
    assert evidence['parameters'] == [
        dict(name='player', constant=None, entry_location=[0x50]),
        dict(name='card', constant=None, entry_location=[0x52]),
        dict(name='number_of_p1_p1_counters', constant=None, entry_location=[0x51]),
        dict(name='amount', constant=1, entry_location=None)]
    assert evidence['TOKEN_ACTION_ADD_COUNTERS'] == 1 << 20
    assert evidence['COUNTER_P1_P1'] == 0
    symbols = [n for n, v, s, k, st in image.functions()
               if image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[s - 1].VirtualAddress + v == 0x23c8b60]
    assert any(n.startswith('_make_fractal.constprop.') for n in symbols), symbols
    exports = list(csv.DictReader((cache / 'decompiled-abi/export.tsv').open(), delimiter='\t'))
    if args.prepare:
        functions = {row['low_pc'].removeprefix('0x'): row for row in csv.DictReader(
            (cache / '2022-functions.csv').open()) if row['low_pc']}
        requests = []
        for row in exports:
            raw = (cache / 'decompiled-abi' / (row['address'] + '.c')).read_text()
            if re.search(r'\b(?:make_fractal|make_one_fractal)\s*\(', raw):
                assert row['source'] == 'cards/strixhaven.c'
                high = int(functions[row['address']]['high_pc'], 16)
                requests.append(f"{row['address']}\t{row['source']}\t{row['name']}\t{high:x}\n")
        (cache / 'export-request-fractal.tsv').write_text(''.join(requests))
        print(json.dumps(dict(evidence=evidence, symbols=symbols, exports=len(requests))))
        return

    raw = (cache / 'decompiled-fractal/23c8b60.c').read_text()
    body = 'static ' + clean(raw)
    assert re.search(r'void\s+make_one_fractal\(int player,int card,int number_of_p1_p1_counters\)', body)
    assert 'number_of_p1_p1_counters & 0xffff' in body
    command = ['C:/Program Files/LLVM/bin/clang-format.exe',
               '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}']
    body = subprocess.run(command, input=body.encode(), capture_output=True, check=True).stdout.decode()
    addition = (f"\n\n/* ManalinkEh 2022: 0x23c8b60; original cards/strixhaven.c:{evidence['line']}. */\n"
                '/* Compiler specialization: quantity is always one in this binary. */\n' + body)
    token_raw = (cache / 'decompiled-fractal-token/26659d0.c').read_text()
    # Verify the actual consumer and ordering; a pre-ETB callback would differ.
    consumer = '''put_into_play(iVar14,iVar13);
          tVar10 = token->action;
          if ((tVar10 & TOKEN_ACTION_ADD_COUNTERS) != 0) {
            hack_silent_counters = hack_silent_counters + 1;
            add_counters(token->s_player,token->s_card,token->t_player,iVar13,
                         (uint)token->action_argument >> 0x10,token->action_argument & 0xffff);
            hack_silent_counters = hack_silent_counters + -1;'''
    assert consumer in token_raw
    start = '/* Recovered 2022 token counter action (0x26659d0); after put_into_play. */'
    end = '/* End recovered 2022 token counter action. */'
    fragment = '''\t\tif (token->action & TOKEN_ACTION_ADD_COUNTERS){
\t\t\t++hack_silent_counters;
\t\t\tadd_counters_from_source(token->s_player, token->s_card, token->t_player, card_added,
\t\t\t\t\t\t\t\t (unsigned int)token->action_argument >> 16, token->action_argument & 0xffff);
\t\t\t--hack_silent_counters;
\t\t}'''
    source = root / 'cards/strixhaven.c'
    original = source.read_bytes()
    assert b'void make_one_fractal(' not in original
    definitions = root / 'defs.h'
    old_definitions = definitions.read_bytes()
    assert b'TOKEN_ACTION_ADD_COUNTERS' not in old_definitions
    needle = b'\tTOKEN_ACTION_ADD_SUBTYPE= 1<<19,'
    at = old_definitions.index(b'\r\n', old_definitions.index(needle))
    new_definitions = (old_definitions[:at] +
                       b'\r\n\tTOKEN_ACTION_ADD_COUNTERS = 1<<20, // 2022: counter type in high 16 bits, quantity in low 16 bits; added after put_into_play.' +
                       old_definitions[at:])
    generator = root / 'functions/token_generation.c'
    old_generator = generator.read_bytes()
    anchor = b'\t\tput_into_play(token->t_player, card_added);\r\n'
    assert old_generator.count(anchor) == 1
    block = '\n\t\t' + start + '\n' + fragment + '\n\t\t' + end + '\n'
    new_generator = old_generator.replace(anchor, anchor + block.replace('\n', '\r\n').encode())
    candidate = cache / 'fractal-specialization-candidate.c'
    candidate.write_bytes(original + addition.encode())
    definitions.write_bytes(new_definitions)
    try:
        generator_candidate = cache / 'token-counter-action-candidate.c'
        generator_candidate.write_bytes(new_generator)
        for file in (candidate, generator_candidate):
            command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
                       '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types',
                       '-Werror=return-type', '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-I', str(root), str(file)]
            result = subprocess.run(command, capture_output=True, text=True)
            (cache / (file.stem + '-check.log')).write_text(result.stderr)
            result.check_returncode()
    finally:
        definitions.write_bytes(old_definitions)
    entry = dict(name='make_one_fractal', original_name='make_fractal', address='23c8b60',
                 source='cards/strixhaven.c', line=evidence['line'], static=True,
                 prototype=body[:body.index('{')].strip() + ';', raw_sha256=digest(raw), recovered_literals=[],
                 supporting_exports=[dict(address='26659d0', raw_sha256=digest(token_raw))],
                 reconstruction='Concrete DWARF amount=1 removed; recover only the quantity-one clone with counters in ECX.',
                 specialization=dict(symbols=symbols, constant_parameters={'amount': 1},
                                     machine_parameters=['player', 'card', 'number_of_p1_p1_counters'], convention='__regparm3'))
    fragment_entry = dict(source='functions/token_generation.c', original_function='generate_token',
                          address='26659d0', raw_sha256=digest(token_raw),
                          start_marker=start, end_marker=end, body_sha256=digest(fragment.strip()),
                          recovery='Only the post-put_into_play counter action; existing token generation retained.')
    if args.apply:
        definitions.write_bytes(new_definitions)
        generator.write_bytes(new_generator)
        source.write_bytes(original.rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        path = root / 'docs/recovery-evidence/recovered-functions.json'
        manifest = json.loads(path.read_text())
        manifest['accepted_functions'].append(entry)
        manifest['accepted'] = len(manifest['accepted_functions'])
        manifest.setdefault('source_fragments', []).append(fragment_entry)
        path.write_text(json.dumps(manifest, indent=2))
        new = list(csv.DictReader((cache / 'decompiled-fractal/export.tsv').open(), delimiter='\t'))
        rows = {row['address']: row for row in exports}
        accepted = {f['address'] for f in manifest['accepted_functions']} - {'23c8b60'}
        for row in new:
            assert row['address'] not in accepted
            shutil.copyfile(cache / 'decompiled-fractal' / (row['address'] + '.c'),
                            cache / 'decompiled-abi' / (row['address'] + '.c'))
            rows[row['address']] = row
        shutil.copyfile(cache / 'decompiled-fractal-token/26659d0.c', cache / 'decompiled-abi/26659d0.c')
        with (cache / 'decompiled-abi/export.tsv').open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=exports[0], delimiter='\t')
            writer.writeheader()
            writer.writerows(rows.values())
        path = root / 'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(path.read_text())
        interfaces.pop('fractal_constant_specialization_blocker', None)
        interfaces['fractal_constant_specialization'] = dict(
            **evidence, symbols=symbols, recovered_name='make_one_fractal',
            token_counter_action=fragment_entry,
            ordering='Counters added after put_into_play, before blocking processing, with silent-counter nesting restored.',
            counter_api='Uses recovered six-argument add_counters_from_source; source is token s_player/s_card.',
            scope='No full generate_token replacement; other 2022 token actions/replacements remain downstream integration work.')
        path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, helper=entry['name'], fragment=fragment_entry), indent=2))


if __name__ == '__main__':
    main()
