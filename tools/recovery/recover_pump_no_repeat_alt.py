"""Recover the alternate-text keyword pump using its neutral-field specialization."""
import argparse
import csv
import hashlib
import json
import shutil
import subprocess
from pathlib import Path

from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean
from audit_source import function_body


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    evidence = {}; enums = {}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'functions/functions.c':
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_typedef' and value(die, 'DW_AT_name') == 'pump_ability_t':
                struct = type_of(die)
                assert value(struct, 'DW_AT_byte_size') == 48
                evidence['target_pump_layout'] = {value(m, 'DW_AT_name'): value(m, 'DW_AT_data_member_location')
                    for m in struct.iter_children() if m.tag == 'DW_TAG_member'}
            if value(die, 'DW_AT_low_pc') == 0x267b1b0:
                assert inherited_value(die, 'DW_AT_name') == 'pump_ability_until_eot_no_repeat_alt'
                evidence['line'] = str(value(die, 'DW_AT_decl_line'))
                assert value(die, 'DW_AT_high_pc') == 336
            if value(die, 'DW_AT_low_pc') == 0x267ad70:
                assert inherited_value(die, 'DW_AT_name') == 'pump_ability_until_eot_with_precomputed_sleight_hack_suppress'
                assert value(die, 'DW_AT_high_pc') == 287
        for die in cu.iter_DIEs():
            if die.tag == 'DW_TAG_enumerator' and value(die, 'DW_AT_name') in (
                'SUBTYPE_INVALID', 'COLOR_TEST_0', 'KEYWORD_RECALC_ABILITIES', 'SP_KEYWORD_DOES_NOT_END_AT_EOT', 'PAUE_END_AT_EOT'):
                enums[value(die, 'DW_AT_name')] = value(die, 'DW_AT_const_value')
        break
    assert enums == dict(SUBTYPE_INVALID=0, COLOR_TEST_0=0, KEYWORD_RECALC_ABILITIES=1 << 27,
                         SP_KEYWORD_DOES_NOT_END_AT_EOT=-(1 << 31), PAUE_END_AT_EOT=512)
    layout = evidence['target_pump_layout']
    assert list(layout.values()) == list(range(0, 48, 4))
    for name, address, storage in [('pump_ability_until_eot_no_repeat_alt', 0x267b1b0, 2),
                                  ('pump_ability_until_eot_with_precomputed_sleight_hack_suppress', 0x267ad70, 3)]:
        assert any(n == '_' + name and st == storage and debug.pe.OPTIONAL_HEADER.ImageBase +
                   debug.pe.sections[s - 1].VirtualAddress + off == address
                   for n, off, s, kind, st in debug.functions())
    def read(at, length):
        rva = at - runtime.pe.OPTIONAL_HEADER.ImageBase
        section = next(s for s in runtime.pe.sections if s.VirtualAddress <= rva < s.VirtualAddress + s.Misc_VirtualSize)
        offset = section.PointerToRawData + rva - section.VirtualAddress
        return runtime.data[offset:offset + length]
    checks = {
        '267b24a': 'c745e800000000c745ec00000000c745f000000000', # subtype/color/disabling all zero
        '267b2ed': 'e8aefdffff83faff',      # find_repeat pair; test EDX card member
        '267b295': '89784c',                # alternate text in eot_toughness
        '267ae15': '8b72248b5228',          # read added subtype and color fields
        '267ae25': '89b0b40000008990b8000000', # targets[8].player/card
        '267ae38': 'e893840300',            # in_play returns instance pointer
        '267ae41': '8b502881ca00000008895028', # set recalc bit in regen_status
        '267ae50': 'c744240cffffffffc744240834000000',
        '267ae67': 'e8a4190500',            # get_abilities(...EVENT_ABILITIES,-1)
    }
    for at, code in checks.items():
        assert read(int(at, 16), len(bytes.fromhex(code))) == bytes.fromhex(code)
    source_path = root / 'functions/functions.c'; original = source_path.read_bytes()
    source = original.decode('latin1')
    old_worker = function_body(source, 'pump_ability_until_eot_with_precomputed_sleight_hack_suppress')
    # Every old member is still assigned by the existing initializer; only the
    # target's appended fields and final recalculation need specialization.
    mappings = {
        'source_p': 'legacy_instance->targets[6].player = pump->source_p;',
        'source_c': 'legacy_instance->targets[6].card = pump->source_c;',
        'pow_mod': 'legacy_instance->counter_power = pump->pow_mod;',
        'tou_mod': 'legacy_instance->counter_toughness = pump->tou_mod;',
        'key_mod': 'legacy_instance->targets[1].player = pump->key_mod;',
        'skey_mod': 'legacy_instance->targets[1].card = pump->skey_mod;',
        'paue_flags': 'legacy_instance->targets[3].player = pump->paue_flags;',
        'can_block_additional_creatures': 'legacy_instance->targets[5].player = pump->can_block_additional_creatures;',
        'eot_removal_method': 'legacy_instance->targets[3].card = pump->eot_removal_method;',
    }
    assert all(statement in old_worker for statement in mappings.values())
    assert 'legacy_instance->number_of_targets = 1;' in old_worker
    assert 'legacy_instance->targets[4].player = 1;' in old_worker
    callback = function_body(source, 'legacy_effect_pump_ability_until_eot')
    assert 'targets[8]' not in callback
    assert 'instance->targets[3].player & PAUE_END_AT_EOT' in callback
    raw = (cache / 'decompiled-abi/267b1b0.c').read_text()
    worker_raw = (cache / 'decompiled-focused11/267ad70.c').read_text()
    assert 'pump->disabling_mode' not in worker_raw
    body = clean(raw)
    neutral = '  pump.subtype = SUBTYPE_INVALID;\n  pump.color = COLOR_TEST_0;\n  pump.disabling_mode = 0;\n'
    assert neutral in body
    body = body.replace(neutral, '')
    call = '''card_00 = pump_ability_until_eot_with_precomputed_sleight_hack_suppress
                      (player,card,t_player,t_card,&pump);'''
    assert call in body
    body = body.replace(call, call + '''
  /* The target wrapper supplies neutral values for the three appended pump fields. */
  instance = get_card_instance(player, card_00);
  instance->targets[8].player = SUBTYPE_INVALID;
  instance->targets[8].card = COLOR_TEST_0;
  instance = in_play(t_player, t_card);
  if (instance != NULL)
  {
    instance->regen_status |= KEYWORD_RECALC_ABILITIES;
    get_abilities(t_player, t_card, EVENT_ABILITIES, -1);
  }''')
    body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
        '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
        input=body.encode(), capture_output=True, check=True).stdout.decode()
    addition = f"\n\n/* ManalinkEh 2022: 0x267b1b0; original functions/functions.c:{evidence['line']}. */\n" + body
    assert b'int pump_ability_until_eot_no_repeat_alt(' not in original
    candidate = cache / 'pump-no-repeat-alt-candidate.c'
    assertions = '\n_Static_assert(sizeof(pump_ability_t) == 36, "source pump size");\n' + ''.join(
        f'_Static_assert(__builtin_offsetof(pump_ability_t, {member}) == {layout[member]}, "{member}");\n'
        for member in mappings) + '\n_Static_assert(SUBTYPE_INVALID == 0 && COLOR_TEST_0 == 0 && PAUE_END_AT_EOT == 512 && KEYWORD_RECALC_ABILITIES == 0x8000000, "pump flags");\n'
    candidate.write_bytes(original + addition.encode() + assertions.encode())
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
        '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-Werror=return-type',
        '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-Werror=format', '-I', str(root), str(candidate)],
        capture_output=True, text=True)
    (cache / 'pump-no-repeat-alt-check.log').write_text(result.stderr); result.check_returncode()
    evidence.update(machine_checks=checks, source_worker_sha256=hashlib.sha256(old_worker.encode()).hexdigest(),
        wrapper_sha256=hashlib.sha256(read(0x267b1b0, 336)).hexdigest(),
        worker_sha256=hashlib.sha256(read(0x267ad70, 287)).hexdigest(), enum_values=enums,
        scope='Only this keyword-only wrapper: all appended fields are zero. Preserve nine common source fields, write neutral legacy subtype/color, then force the target ability recalculation. No general pump-struct or legacy-callback upgrade is claimed.')
    entry = dict(name='pump_ability_until_eot_no_repeat_alt', address='267b1b0', source='functions/functions.c',
        line=evidence['line'], static=False, prototype=body[:body.index('{')].strip() + ';',
        raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=[], interface_evidence=evidence,
        supporting_exports=[dict(address='267ad70', raw_sha256=hashlib.sha256(worker_raw.encode()).hexdigest())],
        reconstruction=evidence['scope'])
    if args.apply:
        path = root / 'docs/recovery-evidence/recovered-functions.json'; manifest = json.loads(path.read_text())
        assert not any(f['address'] == entry['address'] for f in manifest['accepted_functions'])
        source_path.write_bytes(original.rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        path_header = root / 'manalink.h'; header = path_header.read_bytes(); at = header.rfind(b'#ifdef __cplusplus')
        path_header.write_bytes(header[:at] + ('/* Recovered alternate-text keyword pump. */\n' + entry['prototype'] + '\n\n').replace('\n', '\r\n').encode() + header[at:])
        manifest['accepted_functions'].append(entry); manifest['accepted'] = len(manifest['accepted_functions'])
        path.write_text(json.dumps(manifest, indent=2))
        exports_path = cache / 'decompiled-abi/export.tsv'; rows = list(csv.DictReader(exports_path.open(), delimiter='\t'))
        assert not any(r['address'] == '267ad70' for r in rows)
        supporting = next(r for r in csv.DictReader((cache / 'decompiled-focused11/export.tsv').open(), delimiter='\t') if r['address'] == '267ad70')
        rows.append(supporting)
        shutil.copyfile(cache / 'decompiled-focused11/267ad70.c', cache / 'decompiled-abi/267ad70.c')
        with exports_path.open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=rows[0], delimiter='\t'); writer.writeheader(); writer.writerows(rows)
        path = root / 'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(path.read_text())
        interfaces['pump_no_repeat_alt_neutral_specialization'] = evidence; path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, helper=entry['name'], scope=evidence['scope']), indent=2))


if __name__ == '__main__':
    main()
