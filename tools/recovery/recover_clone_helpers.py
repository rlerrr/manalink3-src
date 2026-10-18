"""Recover three concrete compiler clones without manufacturing removed arguments."""
import argparse
import collections
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


CLONES = [
    dict(address='23c89e0', source='cards/strixhaven.c', original='make_elemental',
         name='make_one_elemental_strixhaven', symbol='_make_elemental.constprop.0', returns='void',
         parameters=['int:player', 'int:card'], removed='amount', constant=1),
    dict(address='26e5c70', source='functions/count_graveyard.c', original='count_zone_by_type',
         name='count_zone_by_type_2022', symbol='_count_zone_by_type.isra.0', returns='int',
         parameters=['int:type', 'int_ptr:zone'], removed='player', constant=None),
    dict(address='25bbcd0', source='cards/ravnica_allegiance.c', original='can_adapt',
         name='can_adapt_without_fx', symbol='_can_adapt.part.0', returns='int',
         parameters=['int:player', 'int:card'], removed='fx', constant=None)]


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
    by_source = collections.defaultdict(list)
    for clone in CLONES:
        by_source[clone['source']].append(clone)
    evidence = {}
    for cu in dwarf.iter_CUs():
        source = value(cu.get_top_DIE(), 'DW_AT_name')
        if source not in by_source:
            continue
        for die in cu.get_top_DIE().iter_children():
            clone = next((c for c in by_source[source] if value(die, 'DW_AT_low_pc') == int(c['address'], 16)), None)
            if not clone:
                continue
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
                                if begin <= int(clone['address'], 16) < end:
                                    expression = entry.loc_expr
                                    break
                parameters.append(dict(name=inherited_value(parameter, 'DW_AT_name'),
                                       constant=value(parameter, 'DW_AT_const_value'), entry_location=expression))
            machine = [p['name'] for p in parameters if p['entry_location'] in ([0x50], [0x52])]
            assert machine == [p.split(':')[1] for p in clone['parameters']]
            assert [p['entry_location'] for p in parameters if p['name'] in machine] == [[0x50], [0x52]]
            removed = [p for p in parameters if p['name'] not in machine]
            assert len(removed) == 1 and removed[0]['name'] == clone['removed']
            assert removed[0]['constant'] == clone['constant']
            expression = removed[0]['entry_location']
            assert expression is None if clone['constant'] is not None else expression[0] == 0xfa and expression[-1] == 0x9f
            symbols = [n for n, v, s, k, st in image.functions()
                       if image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[s - 1].VirtualAddress + v == int(clone['address'], 16)]
            assert clone['symbol'] in symbols
            evidence[clone['address']] = dict(parameters=parameters, symbol=clone['symbol'],
                                              line=str(inherited_value(die, 'DW_AT_decl_line')))
    assert len(evidence) == len(CLONES)
    exports = list(csv.DictReader((cache / 'decompiled-abi/export.tsv').open(), delimiter='\t'))
    manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
    manifest = json.loads(manifest_path.read_text())
    accepted = {f['address'] for f in manifest['accepted_functions']}
    if args.prepare:
        functions = {row['low_pc'].removeprefix('0x'): row for row in csv.DictReader(
            (cache / '2022-functions.csv').open()) if row['low_pc']}
        pattern = r'\b(?:' + '|'.join(c[k] for c in CLONES for k in ['original', 'name']) + r')\s*\('
        requests = []
        for row in exports:
            raw = (cache / 'decompiled-abi' / (row['address'] + '.c')).read_text()
            if re.search(pattern, raw):
                assert row['address'] not in accepted
                high = int(functions[row['address']]['high_pc'], 16)
                requests.append(f"{row['address']}\t{row['source']}\t{row['name']}\t{high:x}\n")
        (cache / 'export-request-clones.tsv').write_text(''.join(requests))
        (cache / 'clone-interfaces.tsv').write_text(''.join('\t'.join(
            [c['address'], c['name'], c['returns'], '__regparm2', *c['parameters']]) + '\n' for c in CLONES))
        print(json.dumps(dict(exports=len(requests), evidence=evidence), indent=2))
        return
    additions = collections.defaultdict(str)
    metadata = []
    for clone in CLONES:
        raw = (cache / 'decompiled-clones' / (clone['address'] + '.c')).read_text()
        body = 'static ' + clean(raw)
        prototype = body[:body.index('{')].strip() + ';'
        assert clone['name'] in prototype
        assert len(prototype[prototype.index('(') + 1:prototype.index(')')].split(',')) == 2
        assert not re.search(r'\b' + clone['removed'] + r'\b', body)
        command = ['C:/Program Files/LLVM/bin/clang-format.exe',
                   '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}']
        body = subprocess.run(command, input=body.encode(), capture_output=True, check=True).stdout.decode()
        line = evidence[clone['address']]['line']
        additions[clone['source']] += (f"\n\n/* ManalinkEh 2022: 0x{clone['address']}; original {clone['source']}:{line}. */\n"
                                        '/* Concrete compiler clone: only EAX/EDX are ABI arguments. */\n' + body)
        metadata.append(dict(name=clone['name'], original_name=clone['original'], address=clone['address'],
                             source=clone['source'], line=line, static=True, prototype=prototype,
                             raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=[],
                             reconstruction='Recover the concrete two-register clone; removed parameter has no ABI storage.',
                             specialization=evidence[clone['address']]))
    for source, addition in additions.items():
        candidate = cache / ('clone-candidate-' + Path(source).name)
        original = (root / source).read_bytes()
        assert all((' ' + f['name'] + '(').encode() not in original for f in metadata if f['source'] == source)
        candidate.write_bytes(original + addition.encode())
        command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
                   '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types',
                   '-Werror=return-type', '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-I', str(root), str(candidate)]
        result = subprocess.run(command, capture_output=True, text=True)
        (cache / (candidate.stem + '-check.log')).write_text(result.stderr)
        result.check_returncode()
    if args.apply:
        for source, addition in additions.items():
            path = root / source
            path.write_bytes(path.read_bytes().rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        manifest['accepted_functions'].extend(metadata)
        manifest['accepted'] = len(manifest['accepted_functions'])
        manifest_path.write_text(json.dumps(manifest, indent=2))
        new = list(csv.DictReader((cache / 'decompiled-clones/export.tsv').open(), delimiter='\t'))
        rows = {row['address']: row for row in exports}
        for row in new:
            assert row['address'] not in accepted
            shutil.copyfile(cache / 'decompiled-clones' / (row['address'] + '.c'), cache / 'decompiled-abi' / (row['address'] + '.c'))
            rows[row['address']] = row
        with (cache / 'decompiled-abi/export.tsv').open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=exports[0], delimiter='\t')
            writer.writeheader()
            writer.writerows(rows.values())
        path = root / 'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(path.read_text())
        interfaces['additional_clone_specializations'] = metadata
        path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, helpers=[f['name'] for f in metadata]), indent=2))


if __name__ == '__main__':
    main()
