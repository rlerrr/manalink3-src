"""Verify scalar choice/count facets and distinct callback comparisons, then merge exports."""
import argparse
import csv
import hashlib
import json
import shutil
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86 import X86_OP_MEM, X86_OP_REG, X86_OP_IMM, X86_REG_ESP, X86_REG_EBP
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name


LOCALS = {'20ab320': 'choice2', '25a49c0': 'numdam', '24044c0': 'choice', '2496770': 'choice',
          '2208d20': 'numdam', '217fba0': 'choice', '2180010': 'choice', '2180480': 'choice',
          '233b0f0': 'subt', '235d7b0': None, '25d4b90': 'amt', '20585d0': 'pos'}
CALLBACKS = {'217fba0': 'flailing_manticore_legacy', '2180010': 'flailing_ogre_legacy',
             '2180480': 'flailing_soldier_legacy'}


def descendants(die):
    for child in die.iter_children():
        yield child
        yield from descendants(child)


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--prepare', action='store_true', help='Regenerate precise hints from recorded evidence for a fresh analysis project')
    parser.add_argument('--prepare-callbacks', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    functions = {r['low_pc'].removeprefix('0x'): r for r in csv.DictReader((cache / '2022-functions.csv').open()) if r['low_pc']}
    disassembler = Cs(CS_ARCH_X86, CS_MODE_32); disassembler.detail = True
    checks = {}; callback_requests = []
    for address, name in CALLBACKS.items():
        row = functions[address]; start = int(address, 16)
        legacy = int(next(r['low_pc'] for r in functions.values() if r['name'] == name), 16)
        code = runtime.pe.get_data(start - runtime.pe.OPTIONAL_HEADER.ImageBase, int(row['high_pc'], 16) - start)
        instructions = list(disassembler.disasm(code, start))
        comparisons = [i for i in instructions if i.mnemonic == 'cmp' and len(i.operands) == 2 and
            i.operands[0].type == X86_OP_MEM and i.operands[0].size == 4 and i.operands[0].mem.disp == 56 and
            i.operands[1].type == X86_OP_IMM and i.operands[1].imm == legacy]
        assert len(comparisons) == 1
        instruction = comparisons[0]
        checks[address] = dict(address=f'{instruction.address:x}', bytes=instruction.bytes.hex(),
            legacy=name, legacy_address=f'{legacy:x}', instruction=instruction.mnemonic + ' ' + instruction.op_str)
        callback_requests.append('\t'.join([address, row['source'], row['name'], row['high_pc'][2:],
            'info_slot', 'info_slot_fn', f'{instruction.address:x}']))
    if args.prepare_callbacks or args.prepare:
        (cache / 'export-request-flailing-callbacks.tsv').write_text('\n'.join(callback_requests) + '\n')
        if args.prepare:
            interfaces = json.loads((root / 'docs/recovery-evidence/interface-recovery.json').read_text())
            facets = interfaces['scalar_choices_and_damage_counts']['scalar_facets']
            hints = []; exports = {}
            for facet in facets:
                address = f'{int(facet["function"],16):x}'; row = functions[address]; at = int(facet['access'],16)
                assert runtime.pe.get_data(at - runtime.pe.OPTIONAL_HEADER.ImageBase, len(bytes.fromhex(facet['bytes']))).hex() == facet['bytes']
                fields = [address, row['source'], row['name'], row['high_pc'][2:]]
                hints.append('\t'.join(fields + ['info_slot_fn', 'info_slot', f'{at:x}']))
                exports[address] = '\t'.join(fields)
            (cache / 'scalar-choice-hints.tsv').write_text('\n'.join(hints) + '\n')
            (cache / 'export-request-scalar-choices.tsv').write_text('\n'.join(exports.values()) + '\n')
        print(json.dumps(checks, indent=2))
        return
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    evidence = {}
    for cu in debug.dwarf().iter_CUs():
        source = value(cu.get_top_DIE(), 'DW_AT_name')
        if source not in {functions[a]['source'] for a in LOCALS}:
            continue
        for die in cu.get_top_DIE().iter_children():
            at = value(die, 'DW_AT_low_pc'); address = f'{at:x}' if at else ''
            if address not in LOCALS:
                continue
            assert inherited_value(die, 'DW_AT_name') == functions[address]['name']
            variables = [(inherited_value(v, 'DW_AT_name'), type_name(type_of(v))) for v in descendants(die) if v.tag == 'DW_TAG_variable']
            assert not any(t == 'CardFunction' for n, t in variables)
            if LOCALS[address]:
                assert (LOCALS[address], 'int') in variables
            else:
                assert ('buffer', 'char[100]') in variables  # Haktos stores the arithmetic result directly.
            evidence[address] = dict(name=functions[address]['name'], source=source,
                scalar_local=LOCALS[address], local_types=list(dict.fromkeys(variables)),
                machine_sha256=hashlib.sha256(runtime.pe.get_data(at - runtime.pe.OPTIONAL_HEADER.ImageBase,
                                                               value(die, 'DW_AT_high_pc'))).hexdigest())
    assert set(evidence) == set(LOCALS)
    facets = list(csv.DictReader((cache / 'scalar-choice-facets.tsv').open(), delimiter='\t'))
    assert len(facets) == 14 and {f'{int(r["function"],16):x}' for r in facets} == set(LOCALS)
    precise_requests = []
    for facet in facets:
        address = f'{int(facet["function"],16):x}'; at = int(facet['access'], 16)
        instruction = next(disassembler.disasm(runtime.pe.get_data(at - runtime.pe.OPTIONAL_HEADER.ImageBase, 16), at))
        assert instruction.mnemonic == 'mov' and any(o.type == X86_OP_MEM and o.size == 4 and
            o.mem.disp == 56 and o.mem.base not in {X86_REG_ESP, X86_REG_EBP} for o in instruction.operands)
        facet.update(bytes=instruction.bytes.hex(), instruction=instruction.mnemonic + ' ' + instruction.op_str)
        row = functions[address]
        precise_requests.append('\t'.join([address, row['source'], row['name'], row['high_pc'][2:], 'info_slot_fn', 'info_slot', f'{at:x}']))
    (cache / 'scalar-choice-hints.tsv').write_text('\n'.join(precise_requests) + '\n')
    for filename, count, exports in [('scalar-choices-console.log', 14, 12), ('flailing-callbacks-console.log', 3, 3)]:
        log = (cache / filename).read_text()
        assert f'RECOVERY_SCALAR_INFO_SLOTS_COMPLETE count={count}' in log
        assert f'RECOVERY_EXPORT_COMPLETE success={exports} failed=0' in log and 'ERROR' not in log
    manifest = json.loads((root / 'docs/recovery-evidence/recovered-functions.json').read_text())
    accepted = {f['address'] for f in manifest['accepted_functions']}; assert not accepted & set(LOCALS)
    path = cache / 'decompiled-abi/export.tsv'; rows = list(csv.DictReader(path.open(), delimiter='\t'))
    replacements = {}
    for directory in ['decompiled-scalar-choices', 'decompiled-flailing-callbacks']:
        for row in csv.DictReader((cache / directory / 'export.tsv').open(), delimiter='\t'):
            replacements[row['address']] = row
            shutil.copyfile(cache / directory / (row['address'] + '.c'), cache / 'decompiled-abi' / (row['address'] + '.c'))
    assert set(replacements) == set(LOCALS)
    with path.open('w', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=rows[0], delimiter='\t'); writer.writeheader()
        writer.writerows(replacements.get(r['address'], r) for r in rows)
    path = root / 'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(path.read_text())
    interfaces['scalar_choices_and_damage_counts'] = dict(functions=evidence, scalar_facets=facets,
        callback_comparisons=checks, callback_facets=list(csv.DictReader((cache / 'flailing-callback-facets.tsv').open(), delimiter='\t')),
        scope='Separate fourteen scalar reads/stores from three genuine callback comparisons at verified instance offset 56. Member selection is local to each instruction; source APIs and genuine callback identity remain intact.')
    path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(merged=12, scalar_facets=14, callback_facets=3), indent=2))


if __name__ == '__main__':
    main()
