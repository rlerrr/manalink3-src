"""Recover the separate game-mode flag word and its three exact pregame writes."""
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
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    source = 'functions/rules_engine.c'
    debug_data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug = Image(debug_data)
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    members = {'OGM_NONE':0, 'OGM_EDH':1, 'OGM_PLANECHASE':2, 'OGM_RANDOM_SINGLETON':4, 'OGM_SPAT':8}
    variable, enum = None, None
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != source:
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_variable' and inherited_value(die, 'DW_AT_name') == 'other_game_mode' and value(die, 'DW_AT_location'):
                assert type_name(type_of(die)) == 'other_game_mode_t' and value(die, 'DW_AT_location') == [3, 40, 55, 123, 2]
                variable = dict(die=hex(die.offset), bytes=4, address='27b3728', source=source, zero_initialized=True)
            if die.tag == 'DW_TAG_enumeration_type':
                found = {value(c, 'DW_AT_name'): value(c, 'DW_AT_const_value') for c in die.iter_children()}
                if 'OGM_NONE' in found:
                    assert found == members and value(die, 'DW_AT_byte_size') == 4
                    enum = dict(die=hex(die.offset), bytes=4, members=members)
        break
    assert variable and enum
    symbol = next(s for s in debug.symbols if s[0] == '_other_game_mode')
    section = debug.pe.sections[symbol[2] - 1]
    assert symbol[4] == 2 and section.Name.rstrip(b'\0') == b'.bss' and section.SizeOfRawData == 0
    assert debug.pe.OPTIONAL_HEADER.ImageBase + section.VirtualAddress + symbol[1] == 0x27b3728
    row = next(r for r in csv.DictReader((cache / '2022-functions.csv').open()) if r['name'] == 'pregame' and r['low_pc'])
    code = runtime.pe.get_data(0x26c7a80 - 0x2000000, int(row['high_pc'], 16) - 0x26c7a80)
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.detail = True
    instructions = list(decoder.disasm(code, 0x26c7a80))
    assert sum(i.size for i in instructions) == len(code)
    sites = {f'{i.address:x}': i.bytes.hex() for i in instructions if any((o.type == X86_OP_MEM and o.mem.disp == 0x27b3728) or (o.type == X86_OP_IMM and o.imm == 0x3953) for o in i.operands)}
    expected_sites = {'26c7aba':'c70528377b0200000000', '26c7d0a':'81f953390000',
                      '26c7d12':'8b1528377b02', '26c7d1b':'891528377b02',
                      '26c7d84':'81fa53390000', '26c7d8c':'a128377b02', '26c7d94':'a328377b02'}
    assert sites == expected_sites
    checks = {**sites, '26c7a93':'833dd4ba730001', '26c7aae':'7414', '26c7d10':'750f',
              '26c7d18':'83ca01', '26c7d8a':'750d', '26c7d91':'83c801'}
    for at, expected_hex in checks.items():
        expected_bytes = bytes.fromhex(expected_hex)
        assert runtime.pe.get_data(int(at, 16) - 0x2000000, len(expected_bytes)) == expected_bytes
    raw_path = cache / 'decompiled-game-mode/26c7a80.c'
    raw = raw_path.read_text()
    assert raw.count('other_game_mode = OGM_NONE;') == 1 and raw.count('other_game_mode = other_game_mode | OGM_EDH;') == 2
    assert 'CARD_ID_ELDER_DRAGON_HIGHLANDER       =14675,' in (root / 'card_id.h').read_text()
    assert re.search(r'GAMETYPE_SOLO_DUEL\s*=\s*1\b', (root / 'defs.h').read_text())
    original = (root / source).read_bytes().decode('latin1').replace('\r\n', '\n')
    candidate = original.replace('#include "manalink.h"\n', '#include "manalink.h"\n\n/* October 2022 independent game-mode flags. */\nother_game_mode_2022_t other_game_mode;\n', 1)
    fragments = []
    def fragment(label, body, indent):
        start = f'/* Recovered 2022 game-mode fragment: {label} @ 0x26c7a80. */'
        end = f'/* End recovered 2022 game-mode fragment: {label}. */'
        formatted = '\n'.join(indent + line if line else '' for line in body.splitlines())
        fragments.append(dict(source=source, original_function='pregame', address='26c7a80', raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), start_marker=start, end_marker=end, body_sha256=hashlib.sha256(formatted.strip().encode()).hexdigest(), recovery='Only the corresponding verified game-mode write in existing pregame control flow; original pregame behavior retained.'))
        return indent + start + '\n' + formatted + '\n' + indent + end + '\n'
    at = candidate.index('\t\tsingleplayer_mode = 0;', candidate.index('void pregame(void)'))
    at = candidate.index('\n', at) + 1
    candidate = candidate[:at] + fragment('reset outside solo duel', 'other_game_mode = OGM_NONE;', '\t\t') + candidate[at:]
    pattern = re.compile(r'(?m)^(\t+)if \(csvid == CARD_ID_AVATAR_MOMIR_VIG\)\{\n\1\tsingleplayer_mode = SPM_MOMIR;\n\1\}\n')
    matches = list(pattern.finditer(candidate))
    assert len(matches) == 2
    for match, label in reversed(list(zip(matches, ['EDH avatar in deck', 'EDH avatar already drawn']))):
        addition = fragment(label, 'if (csvid == CARD_ID_ELDER_DRAGON_HIGHLANDER)\n{\n  other_game_mode |= OGM_EDH;\n}', match[1])
        candidate = candidate[:match.end()] + addition + candidate[match.end():]
    defs = (root / 'defs.h').read_bytes().decode('latin1').replace('\r\n', '\n')
    assert not re.search(r'\bOGM_NONE\s*=', defs)
    at = defs.rfind('#endif')
    defs = defs[:at] + '\n#ifndef SHANDALAR\n/* October 2022 independent game modes; original singleplayer modes retained. */\ntypedef enum\n{\n' + ',\n'.join(f'  {name} = {number}' for name, number in members.items()) + '\n} other_game_mode_2022_t;\n#endif\n\n' + defs[at:]
    header = (root / 'manalink.h').read_bytes().decode('latin1').replace('\r\n', '\n')
    at = header.rfind('#ifdef __cplusplus')
    header = header[:at] + '/* October 2022 independent game-mode state. */\n#ifndef SHANDALAR\nextern other_game_mode_2022_t other_game_mode;\n#endif\n\n' + header[at:]
    scratch = cache / 'game-mode-check'
    scratch.mkdir(exist_ok=True)
    for name, text in [('rules_engine.c', candidate), ('defs.h', defs), ('manalink.h', header)]:
        (scratch / name).write_bytes(text.encode('latin1'))
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99', '-fsyntax-only', '-Dstrncasecmp=_strnicmp', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-I', str(scratch), '-I', str(root), str(scratch / 'rules_engine.c')], capture_output=True, text=True)
    (scratch / 'check.log').write_text(result.stderr)
    result.check_returncode()
    proof = dict(debug_sha256=hashlib.sha256(debug_data).hexdigest(), global_state=variable, enum=enum, pregame_machine_bytes=len(code), pregame_machine_sha256=hashlib.sha256(code).hexdigest(), machine_checks=checks, scope='Four-byte independent flag enum and the exact three writes in pregame: conditional reset, EDH avatar in deck, EDH avatar already drawn. No additional flag producers invented.', integration_pending='Existing source/native callers and save/load behavior still need downstream integration; no game execution performed.')
    if args.apply:
        p = root / 'docs/recovery-evidence/recovered-functions.json'
        manifest = json.loads(p.read_text())
        assert 'other_game_mode' not in {g['name'] for g in manifest['source_globals']}
        (root / source).write_bytes(candidate.replace('\n', '\r\n').encode('latin1'))
        (root / 'defs.h').write_bytes(defs.replace('\n', '\r\n').encode('latin1'))
        (root / 'manalink.h').write_bytes(header.replace('\n', '\r\n').encode('latin1'))
        manifest['source_globals'].append(dict(source=source, name='other_game_mode', declaration='other_game_mode_2022_t other_game_mode;', target_address='27b3728', zero_initialized=True, linkage='external'))
        manifest['source_fragments'].extend(fragments)
        p.write_text(json.dumps(manifest, indent=2))
        p = root / 'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(p.read_text())
        interfaces['game_mode_2022'] = proof
        p.write_text(json.dumps(interfaces, indent=2))
        canonical_path = cache / 'decompiled-abi/export.tsv'
        rows = list(csv.DictReader(canonical_path.open(), delimiter='\t'))
        extra = list(csv.DictReader((raw_path.parent / 'export.tsv').open(), delimiter='\t'))
        assert len(extra) == 1 and extra[0]['address'] not in {r['address'] for r in rows}
        rows.extend(extra)
        shutil.copyfile(raw_path, canonical_path.parent / raw_path.name)
        with canonical_path.open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=rows[0], delimiter='\t')
            writer.writeheader()
            writer.writerows(rows)
    print(json.dumps(dict(applied=args.apply, globals=1, lifecycle_fragments=3, enum_members=5)))


if __name__ == '__main__':
    main()
