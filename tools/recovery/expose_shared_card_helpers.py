"""Expose three retained source helpers whose October 2022 COFF symbols are public.

No recovered implementations are added. Native hashes, direct-call instructions,
retained source bodies and the two existing sunburst callees remain auditable.
"""
import argparse
import csv
import hashlib
import json
import re
import shutil
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image, value
from audit_source import function_body
from recover_test_consumers import read
from recover_test_consumers import rename

SPECS = {
    '244d530': ('cards/battle_for_zendikar.c', 'converge', 'static inline void', 'void', 'int player, int card, event_t event'),
    '2672840': ('functions/has_creature_type.c', 'is_creature_type', 'static int', 'int', 'int new_type'),
    '2073940': ('cards/innistrad.c', 'exile_at_end_of_combat', 'static int', 'int', 'int player, int card, event_t event'),
}
SUPPORT = {'2028b30': ('cards/fifth_dawn.c', 'num_colors_paid'),
           '20292f0': ('cards/fifth_dawn.c', 'store_num_colors_paid_in_info_slot')}
HEADER_LABEL = '/* Existing shared card helpers: public in October 2022. */'
PUBLIC_NAMES = {'exile_at_end_of_combat': 'exile_at_end_of_combat_2022'}
ENUMS = dict(STATE_OUBLIETTED=0x800000, RESOLVE_TRIGGER_MANDATORY=2, KILL_EXILE=4,
             EVENT_CAST_SPELL=108, EVENT_MODIFY_COST=164, COLOR_BLACK=1, COLOR_WHITE=5)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def audit_component(component, sources, machine_bytes, root):
    def require(ok, message):
        if not ok:
            raise ValueError(message)
    require(set(component['functions']) == set(SPECS) | set(SUPPORT), 'Shared helper exposure membership mismatch')
    require(component['enum_values'] == ENUMS, 'Shared helper native enum mismatch')
    header = read(root/'manalink.h')
    for address, detail in component['functions'].items():
        require(sha(machine_bytes(int(address, 16), detail['machine_bytes'])) == detail['machine_sha256'],
                'Shared helper exposure native hash mismatch')
        require(sha((root/'.recovery/decompiled-abi'/f'{address}.c').read_bytes()) == detail['raw_sha256'],
                'Shared helper exposure raw export mismatch')
        body = function_body(sources[detail['source']].replace('\r\n', '\n'), detail.get('public_name', detail['name']))
        body, _ = rename(body, {v:k for k,v in PUBLIC_NAMES.items()})
        require(sha(body.encode()) == detail['body_sha256'],
                'Retained shared helper source body mismatch')
        for call in detail['calls']:
            data = bytes.fromhex(call['bytes'])
            require(machine_bytes(int(call['address'], 16), len(data)) == data, 'Shared helper native call mismatch')
        if address in SPECS:
            source, name, old, new, arguments = SPECS[address]
            public_name = PUBLIC_NAMES.get(name, name)
            require(detail['storage'] == 2 and detail['old_signature'] == f'{old} {name}({arguments})' and
                    detail['new_signature'] == f'{new} {public_name}({arguments})', 'Shared helper public linkage mismatch')
            require(detail['new_signature'] in sources[source] and detail['old_signature'] not in sources[source] and
                    detail['new_signature']+';' in header, 'Shared helper exposure declaration mismatch')
            if public_name != name:
                require(detail['source_renames'] == 2, 'End-combat helper definition/callback rename mismatch')
    # The converge body is a native tail call, without extra state or argument changes.
    require(component['functions']['244d530']['calls'] == [dict(address='244d530', bytes='e9bbbdbdff', target='20292f0')],
            'Converge tail-call mismatch')
    require(component['creature_subtype_range'] == [1, 4095], 'Creature subtype predicate bounds mismatch')
    subtypes = read(root/'subtypes.h')
    require(re.search(r'SUBTYPE_MIN_CREATURE_SUBTYPE\s*=\s*0x0001', subtypes) and
            re.search(r'SUBTYPE_MAX_CREATURE_SUBTYPE\s*=\s*0x0FFF', subtypes), 'Existing creature subtype enum mismatch')
    require(machine_bytes(0x2672847, 14).hex() == '483dfe0f00000f96c025ff000000', 'Creature subtype unsigned range predicate mismatch')
    require(machine_bytes(0x2029338, 3).hex() == '895838', 'Sunburst info-slot store mismatch')
    require(machine_bytes(0x2073984, 8).hex() == 'c744240800008000', 'End-combat oubliette flag mismatch')


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root/'.recovery'
    ip = root/'docs/recovery-evidence/interface-recovery.json'
    interfaces = json.loads(ip.read_text())
    assert 'shared_card_exposures_2022' not in interfaces
    meta = {r['low_pc'][2:]: r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    debug_data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug = Image(debug_data)
    native = Image((cache/'2022-ManalinkEh.dll').read_bytes())
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.detail = True
    sources = {s: read(root/s) for s in {v[0] for v in list(SPECS.values())+list(SUPPORT.values())}}
    proof = dict(debug_sha256=sha(debug_data), functions={}, creature_subtype_range=[1, 4095], enum_values={},
                 scope='Public exposure of three retained bodies; rename the Innistrad callback and its local reference to avoid the distinct Commander 2015 private helper. Retain existing sunburst worker and wrapper. No new implementations counted.',
                 integration_pending='Native linkage and gameplay integration remain pending.')
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {v[0] for v in list(SPECS.values())+list(SUPPORT.values())}:
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_enumeration_type':
                for child in die.iter_children():
                    name = value(child,'DW_AT_name')
                    if name in ENUMS:
                        number = value(child,'DW_AT_const_value'); assert number == ENUMS[name],(name,number)
                        proof['enum_values'][name] = number
    assert proof['enum_values'] == ENUMS
    for address in list(SPECS)+list(SUPPORT):
        source, name = (SPECS.get(address) or SUPPORT[address])[:2]
        assert meta[address]['source'] == source and meta[address]['name'] == name
        data = native.pe.get_data(int(address, 16)-0x2000000, int(meta[address]['high_pc'], 16)-int(address, 16))
        symbols = [storage for n, off, section, kind, storage in debug.functions()
                   if debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[section-1].VirtualAddress+off == int(address, 16)]
        assert symbols == ([3] if address == '2028b30' else [2]), (address, symbols)
        raw = (cache/'decompiled-shared-card-frontends'/f'{address}.c').read_bytes()
        detail = dict(source=source, name=name, storage=symbols[0], body_sha256=sha(function_body(sources[source], name).encode()),
                      machine_bytes=len(data), machine_sha256=sha(data), raw_sha256=sha(raw), calls=[])
        for i in decoder.disasm(data, int(address, 16)):
            if i.mnemonic in {'call', 'jmp'} and i.operands[0].type == X86_OP_IMM and not int(address,16) <= i.operands[0].imm < int(meta[address]['high_pc'],16):
                detail['calls'].append(dict(address=f'{i.address:x}', bytes=i.bytes.hex(), target=f'{i.operands[0].imm:x}'))
        if address in SPECS:
            source, name, old, new, arguments = SPECS[address]
            public_name = PUBLIC_NAMES.get(name, name)
            detail.update(old_signature=f'{old} {name}({arguments})', new_signature=f'{new} {public_name}({arguments})', public_name=public_name)
            assert sources[source].count(detail['old_signature']) == 1
            sources[source] = sources[source].replace(detail['old_signature'], f'{new} {name}({arguments})', 1)
            if public_name != name:
                sources[source], changes = rename(sources[source], {name:public_name})
                detail['source_renames'] = changes
        proof['functions'][address] = detail
    header = read(root/'manalink.h')
    at = header.rfind('#ifdef __cplusplus')
    header = header[:at]+HEADER_LABEL+'\n'+'\n'.join(proof['functions'][a]['new_signature']+';' for a in SPECS)+'\n\n'+header[at:]
    exports = cache/'decompiled-abi'
    rows = list(csv.DictReader((exports/'export.tsv').open(), delimiter='\t'))
    focused = {r['address']: r for r in csv.DictReader((cache/'decompiled-shared-card-frontends/export.tsv').open(), delimiter='\t')}
    manifest = json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text())
    citations = manifest['source_fragments'] + manifest['accepted_functions'] + [s for f in manifest['accepted_functions'] for s in f.get('supporting_exports', [])]
    for address in list(SPECS)+list(SUPPORT):
        if (exports/f'{address}.c').exists():
            assert not any(c['address'] == address for c in citations), address
        else:
            rows.append(focused[address])
    if args.apply:
        for source, text in sources.items():
            if source != 'cards/fifth_dawn.c':
                (root/source).write_bytes(text.replace('\n', '\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n', '\r\n').encode('latin1'))
        for address in list(SPECS)+list(SUPPORT):
            shutil.copyfile(cache/'decompiled-shared-card-frontends'/f'{address}.c', exports/f'{address}.c')
        with (exports/'export.tsv').open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=rows[0], delimiter='\t')
            writer.writeheader(); writer.writerows(rows)
        interfaces['shared_card_exposures_2022'] = proof
        ip.write_text(json.dumps(interfaces, indent=2))
        mp = root/'docs/recovery-evidence/recovered-functions.json'
        manifest = json.loads(mp.read_text())
        manifest['raw_export_entries'] = len(rows)
        manifest['exported_functions'] = len({r['name'] for r in rows})
        mp.write_text(json.dumps(manifest, indent=2))
    (cache/'shared-card-exposure-plan.json').write_text(json.dumps(proof, indent=2))
    print(json.dumps(dict(applied=args.apply, exposed_existing_helpers=3, verified_existing_support_helpers=2, new_implementations=0)))


if __name__ == '__main__':
    main()
