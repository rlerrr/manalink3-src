"""Recover card flag masks and the already verified native attacker-query boundary."""
import argparse
import collections
import csv
import inspect
import json
import re
import shutil
import struct
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM, X86_OP_MEM, X86_OP_REG
from inspect_release import Image, value
from promote_recovery import clean, strip_comments, UNRESOLVED
from recover_test_consumers import read
from recover_literals import recover_strings
from recover_spell_copy import compile_sources, format_c
from recover_temporary_exile import save_json, sha
from audit_source import function_body


def mask(literal, *names):
    return (literal, names[0] if len(names) == 1 else '(' + ' | '.join(names) + ')', names)


KEYWORDS = ('KEYWORD_DOUBLE_STRIKE', 'KEYWORD_REACH', 'KEYWORD_FIRST_STRIKE', 'KEYWORD_TRAMPLE', 'KEYWORD_FLYING')
SPECIAL = ('SP_KEYWORD_HEXPROOF', 'SP_KEYWORD_MENACE', 'SP_KEYWORD_INDESTRUCTIBLE',
           'SP_KEYWORD_DEATHTOUCH', 'SP_KEYWORD_VIGILANCE', 'SP_KEYWORD_HASTE', 'SP_KEYWORD_LIFELINK')
PROTECTION = ('KEYWORD_PROT_LANDS', 'KEYWORD_PROT_CREATURES', 'KEYWORD_PROT_SORCERIES',
              'KEYWORD_PROT_INSTANTS', 'KEYWORD_PROT_ARTIFACTS', 'KEYWORD_PROT_COLORED')
CONFIG = {
    '2014020': [mask('0x400000', 'SF2_CANNOT_BE_SACRIFICED')],
    '2022ea0': [mask('0x800000', 'GAA_REGENERATION')],
    '25bb910': [mask('0x800000', 'KEYWORD_DEFENDER')],
    '2218990': [mask('0x800000', 'KEYWORD_DEFENDER')],
    '2375b00': [mask('0x800000', 'KEYWORD_DEFENDER')],
    '2207550': [mask('0x800010', 'SP_KEYWORD_HEXPROOF', 'SP_KEYWORD_UNBLOCKABLE')],
    '25ab390': [mask('0x400000', 'SF3_IN_PLAY_VIA_KODAMA_EAST_TREE'), mask('0x40000', 'SF_NOT_CAST')],
    '24aff70': [mask('0x800000', 'SF_MOVE_AURA_LEGAL_TARGET')],
    '23600c0': [mask('0x800800', 'SP_KEYWORD_HEXPROOF', 'SP_KEYWORD_INDESTRUCTIBLE')],
    '25cdd60': [mask('0x40000080', 'KEYWORD_DOUBLE_STRIKE', 'KEYWORD_TRAMPLE'), mask('0x800000', 'KEYWORD_DEFENDER')],
    '257d680': [('*(uint *)(&player_bits + player * 4)', 'player_bits[player]', ()),
                ('*(uint *)(&player_bits + (1 - player) * 4)', 'player_bits[1 - player]', ()),
                mask('0x2000000', 'PB_GY_ABILITIES_DISABLED')],
    '2488450': [mask('0x800000', 'GAA_REGENERATION')],
    '2027e50': [mask('0x1000000', 'EA_FORCE_ATTACK'), mask('0x6000000', 'KEYWORD_RECALC_POWER', 'KEYWORD_RECALC_TOUGHNESS')],
    '2028690': [mask('0x1000000', 'EA_FORCE_ATTACK')],
    '2206800': [mask('0x1000000', 'EA_FORCE_ATTACK')],
    '20d5cb0': [mask('0x1000000', 'EA_FORCE_ATTACK')],
    '20d1930': [mask('0x1000000', 'EA_FORCE_ATTACK'), mask('0x400005a0', *KEYWORDS), mask('0x810a0e', *SPECIAL)],
    '2374670': [mask('0x1000000', 'EA_FORCE_ATTACK')],
    '25a2c20': [mask('0x400005a0', *KEYWORDS), mask('0x810a0e', *SPECIAL), mask('0x75f800', *PROTECTION)],
    '22c63c0': [],
    '22c6410': [],
}
PARTNERS = {'22c63c0', '22c6410'}
FOLLOWUPS = {'24975d0': '2488450', '25c3d60': '25bb910'}
FOLLOWUP_TARGETS = {'2488450': 'effect_defiling_tears', '25bb910': 'tin_street_dodger_legacy'}
LOOKUP_RAW = 'iVar1 = (int)*(short *)(&UNK_0273831a + (csvid - 0x48a6U) + csvid);'
ENGINE_RAW = '(*(code *)0x434be0)'
ENGINE_NAME = 'engine_any_legal_attacker_2022'
HEADER_LABEL = '/* October 2022 native legal-attacker query, verified against Magic.exe. */'
HEADER = HEADER_LABEL + '''
#ifndef SHANDALAR
static inline int engine_any_legal_attacker_2022(int player)
{
  return EXE_FN(int, 0x434be0, int)(player);
}
int general_has_partner_2022(int csvid);
int get_partner_with_id(int csvid);
#endif
'''
DEFS_LABEL = '/* October 2022 graveyard-ability and Kodama entry flags. */'
DEFINITIONS = DEFS_LABEL + '''
#ifndef SHANDALAR
enum
{
  PB_GY_ABILITIES_DISABLED = 0x2000000,
  SF3_IN_PLAY_VIA_KODAMA_EAST_TREE = 0x400000
};
#endif
'''


def cleaned(text, overlap):
    options = {name: True for name in inspect.signature(clean).parameters if name.startswith('use_')}
    text = re.sub(r'\bgeneral_has_partner\b', 'general_has_partner_2022', text)
    return format_c(clean(text, overlap, **options))


def bind_followups(root, apply):
    from bind_delayed_callback_callers import audit_component as audit_callers
    cache = root / '.recovery'
    manifest = json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text())
    ip = root/'docs/recovery-evidence/interface-recovery.json'
    interfaces = json.loads(ip.read_text())
    assert 'combat_mask_followups_2022' not in interfaces
    entries = {f['address']: f for f in manifest['accepted_functions']}
    meta = {r['low_pc'][2:]: r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    runtime = Image((cache/'2022-ManalinkEh.dll').read_bytes())
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.detail = True
    sources = {entries[a]['source']: read(root/entries[a]['source']) for a in FOLLOWUPS}
    proof = dict(functions={}, scope='One native private callback operand each in Defiling Tears and Tin Street Dodger; both callbacks stay in their owning source units.')
    for address, target in FOLLOWUPS.items():
        assert entries[target]['static'] and entries[target]['source'] == entries[address]['source']
        code = runtime.pe.get_data(int(address, 16)-0x2000000, int(meta[address]['high_pc'], 16)-int(address, 16))
        sites = [dict(address=f'{ins.address:x}', bytes=ins.bytes.hex(), target=target, name=FOLLOWUP_TARGETS[target])
                 for ins in decoder.disasm(code, int(address, 16)) if any(op.type == X86_OP_IMM and op.imm == int(target, 16) for op in ins.operands)]
        proof['functions'][address] = dict(machine_bytes=len(code), machine_sha256=sha(code), sites=sites,
            body_sha256=sha(function_body(sources[entries[address]['source']], entries[address]['name']).encode()))
    audit_callers(proof, manifest, sources, lambda a, n: runtime.pe.get_data(a-0x2000000, n), expected=FOLLOWUPS, targets=FOLLOWUP_TARGETS)
    if apply:
        interfaces['combat_mask_followups_2022'] = proof
        save_json(ip, interfaces)
    print(json.dumps(dict(applied=apply, bound_functions=len(FOLLOWUPS), native_callback_operands=2)))


def audit_component(component, manifest, sources, machine_bytes, root):
    entries = {f['address']: f for f in manifest['accepted_functions'] if f.get('combat_mask_evidence')}
    assert set(entries) == set(CONFIG) == set(component['functions'])
    assert component['engine_interface']['address'] == '434be0'
    magic = Image((root / '.recovery/2022-Magic.exe').read_bytes())
    native_interface = component['engine_interface']
    assert sha(magic.data) == native_interface['executable_sha256']
    assert sha(magic.pe.get_data(0x434be0-magic.pe.OPTIONAL_HEADER.ImageBase, 75)) == native_interface['body_sha256']
    for at, code in native_interface['machine_checks'].items():
        assert magic.pe.get_data(int(at, 16)-magic.pe.OPTIONAL_HEADER.ImageBase, len(bytes.fromhex(code))).hex() == code
    if not component.get('planning'):
        assert HEADER in read(root / 'manalink.h') and DEFINITIONS in read(root / 'defs.h')
    assert sha(function_body(sources['cards/vanguard.c'].replace('\r\n', '\n'), 'general_has_partner').encode()) == component['retained_partner_body_sha256']
    table = component['partner_lookup']
    assert table['address'] == '273cbc0' and table['values'] == list(struct.unpack('<59h', machine_bytes(0x273cbc0, 118)))
    assert table['bytes'] == machine_bytes(0x273cbc0, 118).hex()
    for table_info in component['partner_switch_tables']:
        values = list(struct.unpack('<'+'I'*table_info['count'], machine_bytes(int(table_info['address'], 16), table_info['count']*4)))
        assert values == table_info['targets'] and set(values) == {0x22c63e0, 0x22c63ef}
        expected_ids = {table_info['first_id']+i for i, target in enumerate(values) if target == 0x22c63e0}
        partner_body = function_body(sources['cards/vanguard.c'], 'general_has_partner_2022')
        case_ids = {int(a, 16) for a in re.findall(r'case (0x[0-9a-f]+):', partner_body)}
        assert {i for i in case_ids if table_info['first_id'] <= i < table_info['first_id']+table_info['count']} == expected_ids
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.detail = True
    for address, entry in entries.items():
        detail = component['functions'][address]
        body = function_body(sources[entry['source']].replace('\r\n', '\n'), entry['name'])
        assert entry['combat_mask_evidence'] == detail and sha(body.encode()) == entry['reconstruction_body_sha256']
        assert entry['static'] == (detail['coff_storage_class'] == 3)
        code = machine_bytes(int(address, 16), detail['machine_bytes'])
        assert sha(code) == detail['machine_sha256']
        instructions = {f'{i.address:x}': i for i in decoder.disasm(code, int(address, 16))}
        assert len(detail['changes']) == len(CONFIG[address]) + bool(detail['engine_calls']) + (address == '22c6410')
        for change, (original, replacement, names) in zip(detail['changes'], CONFIG[address]):
            assert (change['original'], change['replacement']) == (original, replacement)
            if names:
                bits = component['unit_enums'][entry['source']]
                expected = 0
                for name in names:
                    expected |= bits[name]
                assert expected == int(original, 16)
            for site in change['machine_sites']:
                ins = instructions[site['address']]
                assert ins.bytes.hex() == site['bytes']
                if names:
                    assert any(op.type == X86_OP_IMM and op.imm & 0xffffffff == expected for op in ins.operands)
        for call in detail['canonical_calls']:
            ins = instructions[call['address']]
            assert ins.bytes.hex() == call['bytes'] and ins.mnemonic in {'call', 'jmp'}
            assert ins.operands[0].type == X86_OP_IMM and ins.operands[0].imm == int(call['target'], 16)
            assert re.search(r'\b' + call['name'] + r'\s*\(', body)
        for call in detail['engine_calls']:
            ins, load = instructions[call['address']], instructions[call['load_address']]
            assert ins.bytes.hex() == call['bytes'] and ins.mnemonic == 'call'
            assert load.bytes.hex() == call['load_bytes'] and load.mnemonic == 'mov'
            assert load.operands[0].type == X86_OP_REG and load.operands[1].type == X86_OP_IMM
            assert load.operands[1].imm == 0x434be0 and load.address + load.size == ins.address
            assert ins.operands[0].type == X86_OP_REG and ins.operands[0].reg == load.operands[0].reg
            assert call['name'] == ENGINE_NAME and re.search(r'\b'+ENGINE_NAME+r'\s*\(', body)
    jailer = entries['257d680']
    body = function_body(sources[jailer['source']], jailer['name'])
    assert body.count('player_bits[player]') == 2 and body.count('player_bits[1 - player]') == 2


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    parser.add_argument('--bind-followups', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    if args.bind_followups:
        bind_followups(root, args.apply)
        return
    cache = root / '.recovery'
    mp = root / 'docs/recovery-evidence/recovered-functions.json'
    ip = root / 'docs/recovery-evidence/interface-recovery.json'
    manifest = json.loads(mp.read_text())
    interfaces = json.loads(ip.read_text())
    assert 'combat_mask_cards_2022' not in interfaces
    assert not set(CONFIG) & {f['address'] for f in manifest['accepted_functions']}
    meta = {r['low_pc'][2:]: r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    wanted = collections.defaultdict(set)
    for address, changes in CONFIG.items():
        wanted[meta[address]['source']].update(n for o, r, names in changes for n in names)
    debug_data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug = Image(debug_data)
    unit_enums = {}
    for cu in debug.dwarf().iter_CUs():
        source = value(cu.get_top_DIE(), 'DW_AT_name')
        if source not in wanted:
            continue
        enums = {}
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_enumeration_type':
                for child in die.iter_children():
                    name = value(child, 'DW_AT_name')
                    if name in wanted[source]:
                        enums[name] = value(child, 'DW_AT_const_value')
        assert set(enums) == wanted[source], (source, wanted[source] - set(enums))
        unit_enums[source] = enums
    assert set(unit_enums) == set(wanted)
    runtime = Image((cache/'2022-ManalinkEh.dll').read_bytes())
    machine_bytes = lambda a, n: runtime.pe.get_data(a-0x2000000, n)
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.detail = True
    native_interface = interfaces['batch14_compatibility_interfaces']['2697910']['native_interface']
    proof = dict(debug_sha256=sha(debug_data), unit_enums=unit_enums, engine_interface=native_interface,
                 functions={}, scope='Caller-specific native flag masks, indexed Yixlid Jailer player bits, and the preverified EXE legal-attacker query; preserve native control flow and source compatibility APIs.')
    lookup_bytes = machine_bytes(0x273cbc0, 118)
    lookup_values = list(struct.unpack('<59h', lookup_bytes))
    proof['partner_lookup'] = dict(address='273cbc0', bytes=lookup_bytes.hex(), values=lookup_values)
    proof['partner_switch_tables'] = [dict(address=f'{at:x}', count=count, first_id=first,
        targets=list(struct.unpack('<'+'I'*count, machine_bytes(at, count*4))))
        for at, count, first in [(0x273c638, 155, 0x562f), (0x273c8a4, 55, 0x40c8)]]
    proof['retained_partner_body_sha256'] = sha(function_body(read(root/'cards/vanguard.c'), 'general_has_partner').encode())
    targets = {int(f['address'], 16): f['name'] for f in manifest['accepted_functions']}
    targets.update({int(a, 16): meta[a]['name'] for a in CONFIG})
    overlap = [line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2] == 'global']
    entries, bodies = [], {}
    for address, changes in CONFIG.items():
        row = meta[address]
        raw = read(cache/('decompiled-partner' if address in PARTNERS else 'decompiled-abi')/f'{address}.c')
        text = strip_comments(raw)
        code = machine_bytes(int(address, 16), int(row['high_pc'], 16)-int(address, 16))
        instructions = list(decoder.disasm(code, int(address, 16)))
        detail_changes = []
        for original, replacement, names in changes:
            count = text.count(original)
            assert count > 0, (address, original)
            if names:
                mask_value = 0
                for name in names:
                    mask_value |= unit_enums[row['source']][name]
                assert mask_value == int(original, 16)
                sites = [dict(address=f'{ins.address:x}', bytes=ins.bytes.hex()) for ins in instructions
                         if ins.mnemonic in {'mov', 'push', 'or', 'and', 'test', 'cmp'} and
                         any(op.type == X86_OP_IMM and op.imm & 0xffffffff == mask_value for op in ins.operands)]
            else:
                sites = [dict(address=f'{ins.address:x}', bytes=ins.bytes.hex()) for ins in instructions
                         if any(op.type == X86_OP_MEM and op.mem.disp == 0x4ef1a4 for op in ins.operands)]
            assert sites, (address, original)
            detail_changes.append(dict(original=original, replacement=replacement, occurrences=count,
                                       kind='scoped native flag/query reconstruction', machine_sites=sites))
            text = text.replace(original, replacement)
        if address == '22c6410':
            replacement = 'static const int16_t partner_with_ids[59] = {\n'+','.join(map(str, lookup_values))+'\n};\niVar1 = partner_with_ids[(uint32_t)csvid - 0x48a6U];'
            assert text.count(LOOKUP_RAW) == 1
            sites = [dict(address=f'{ins.address:x}', bytes=ins.bytes.hex()) for ins in instructions]
            detail_changes.append(dict(original=LOOKUP_RAW, replacement=replacement, occurrences=1,
                                       kind='scoped native flag/query reconstruction', machine_sites=sites))
            text = text.replace(LOOKUP_RAW, replacement)
        engine_calls = []
        for load, ins in zip(instructions, instructions[1:]):
            if (load.mnemonic == 'mov' and load.operands[0].type == X86_OP_REG and
                load.operands[1].type == X86_OP_IMM and load.operands[1].imm == 0x434be0 and
                ins.mnemonic == 'call' and ins.operands[0].type == X86_OP_REG and
                ins.operands[0].reg == load.operands[0].reg):
                engine_calls.append(dict(address=f'{ins.address:x}', bytes=ins.bytes.hex(),
                                         load_address=f'{load.address:x}', load_bytes=load.bytes.hex(),
                                         target='434be0', name=ENGINE_NAME))
        if engine_calls:
            count = text.count(ENGINE_RAW)
            assert count == len(engine_calls), address
            detail_changes.append(dict(original=ENGINE_RAW, replacement=ENGINE_NAME, occurrences=count,
                                       kind='scoped native flag/query reconstruction', machine_sites=
                                       [dict(address=c['load_address'], bytes=c['load_bytes']) for c in engine_calls]+engine_calls))
            text = text.replace(ENGINE_RAW, ENGINE_NAME)
        text, literals = recover_strings(text, runtime, {})
        body = cleaned(text, overlap)
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x|_conflict', body), address
        storage = [s for n, v, section, kind, s in debug.functions() if section > 0 and
                   debug.pe.OPTIONAL_HEADER.ImageBase + debug.pe.sections[section-1].VirtualAddress + v == int(address, 16)]
        assert len(storage) == 1 and storage[0] in {2, 3}, (address, storage)
        private = storage == [3]
        if private:
            body = 'static ' + body
        calls = []
        for ins in instructions:
            if ins.mnemonic not in {'call', 'jmp'} or ins.operands[0].type != X86_OP_IMM or ins.operands[0].imm not in targets:
                continue
            name = targets[ins.operands[0].imm]
            if not re.search(r'\b' + name + r'\s*\(', body):
                probe = cleaned('int probe(void) { ' + name + '(); return 0; }', overlap)
                name = re.search(r'\{\s*(\w+)\(', probe)[1]
            assert re.search(r'\b' + name + r'\s*\(', body), (address, name)
            calls.append(dict(address=f'{ins.address:x}', bytes=ins.bytes.hex(), target=f'{ins.operands[0].imm:x}', name=name))
        detail = dict(machine_bytes=len(code), machine_sha256=sha(code), changes=detail_changes,
                      canonical_calls=calls, engine_calls=engine_calls, coff_storage_class=storage[0])
        proof['functions'][address] = detail
        bodies[address] = body
        name = 'general_has_partner_2022' if address == '22c63c0' else row['name']
        entries.append(dict(address=address, name=name, source=row['source'], line=row['line'], static=private,
                            prototype=body[:body.index('{')].strip()+';', raw_sha256=sha(raw.encode()), recovered_literals=literals,
                            combat_mask_evidence=detail, reconstruction=proof['scope'],
                            reconstruction_body_sha256=sha(function_body(body, name).encode())))
    sources = {}
    for source in {f['source'] for f in entries}:
        original = read(root/source)
        selected = [f for f in entries if f['source'] == source]
        for entry in selected:
            assert not re.search(r'\b'+entry['name']+r'\s*\([^;{}]*\)\s*\{', strip_comments(original))
        addition = '\n\n#ifndef SHANDALAR\n/* Recovered October 2022 flag and combat-query consumers. */\n'
        addition += '\n'.join(f['prototype'] for f in selected)+'\n'
        for entry in selected:
            addition += f'\n/* ManalinkEh 2022: 0x{entry["address"]}; original {source}:{entry["line"]}. */\n'+bodies[entry['address']]+'\n'
        sources[source] = original + addition + '#endif\n'
    header = read(root/'manalink.h')
    assert HEADER_LABEL not in header
    at = header.rfind('#ifdef __cplusplus')
    header = header[:at] + HEADER + '\n' + header[at:]
    definitions = read(root/'defs.h')
    assert DEFS_LABEL not in definitions
    at = definitions.rfind('\n#endif')
    definitions = definitions[:at]+'\n\n'+DEFINITIONS+definitions[at:]
    scratch = cache/'combat-mask-check'
    scratch.mkdir(exist_ok=True)
    (scratch/'defs.h').write_bytes(definitions.encode('latin1'))
    strict_sources = {}
    for source in sources:
        selected = [f for f in entries if f['source'] == source]
        text = '\n'.join(bodies[f['address']] for f in selected)
        retained = [f['prototype'] for f in manifest['accepted_functions'] if f['source'] == source and f['static'] and re.search(r'\b'+f['name']+r'\b', text)]
        strict_sources[source] = '#include "manalink.h"\n'+'\n'.join(retained+[f['prototype'] for f in selected])+'\n'+text
    compile_sources(root, scratch, strict_sources, header)
    proof['planning'] = True
    audit_component(proof, {**manifest, 'accepted_functions': manifest['accepted_functions']+entries}, sources, machine_bytes, root)
    del proof['planning']
    if args.apply:
        citations = manifest['accepted_functions']+manifest['source_fragments']+[s for f in manifest['accepted_functions'] for s in f.get('supporting_exports', [])]
        assert not PARTNERS & {f['address'] for f in citations}
        rows = {r['address']: r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(), delimiter='\t')}
        focused = {r['address']: r for r in csv.DictReader((cache/'decompiled-partner/export.tsv').open(), delimiter='\t')}
        for address in PARTNERS:
            rows[address] = focused[address]
            shutil.copyfile(cache/'decompiled-partner'/f'{address}.c', cache/'decompiled-abi'/f'{address}.c')
        with (cache/'decompiled-abi/export.tsv').open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=['address', 'source', 'name', 'status', 'prototype'], delimiter='\t')
            writer.writeheader()
            writer.writerows(rows.values())
        manifest['raw_export_entries'] = len(rows)
        manifest['exported_functions'] = len({r['name'] for r in rows.values()})
        for source, text in sources.items():
            (root/source).write_bytes(text.replace('\n', '\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n', '\r\n').encode('latin1'))
        (root/'defs.h').write_bytes(definitions.replace('\n', '\r\n').encode('latin1'))
        manifest['accepted_functions'] += entries
        manifest['accepted'] = len(manifest['accepted_functions'])
        for entry in entries:
            manifest['source_files'][entry['source']] = manifest['source_files'].get(entry['source'], 0)+1
        for key, predicate in [('card_handler_functions', lambda f: f['name'].startswith('card_')),
                               ('helper_functions', lambda f: not f['name'].startswith('card_')),
                               ('private_functions', lambda f: f['static']),
                               ('private_helpers', lambda f: f['static'] and not f['name'].startswith('card_')),
                               ('shared_helper_interfaces', lambda f: not f['static'] and not f['name'].startswith('card_'))]:
            manifest[key] = sum(predicate(f) for f in manifest['accepted_functions'])
        save_json(mp, manifest)
        interfaces['combat_mask_cards_2022'] = proof
        save_json(ip, interfaces)
    save_json(cache/'combat-mask-plan.json', proof)
    print(json.dumps(dict(applied=args.apply, new_implementations=len(entries),
                         new_cards=sum(f['name'].startswith('card_') for f in entries),
                         engine_query_calls=sum(len(d['engine_calls']) for d in proof['functions'].values()))))


if __name__ == '__main__':
    main()
