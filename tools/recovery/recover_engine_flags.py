"""Recover per-player card/counterability flag storage and its complete lifecycle fragments."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_REG, X86_OP_MEM, X86_OP_IMM
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name


LIVE = {'card_flags': (0x2797218, 'uint32_t[2]', 8),
        'your_spells_cant_be_countered_by_color': (0x27971a0, 'uint8_t[2]', 2)}
BACKUP = {'backup_card_flags': 0x2784e74, 'backup0_card_flags': 0x2775bd4,
          'backup_your_spells_cant_be_countered_by_color': 0x2784e70,
          'backup0_your_spells_cant_be_countered_by_color': 0x2775bd0}


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    debug_data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug = Image(debug_data)
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    proof = dict(debug_sha256=hashlib.sha256(debug_data).hexdigest(), globals={}, copies={}, resets={}, serialization='Both live arrays appear in the native supplemental record; full save-format migration remains deferred.')
    for cu in debug.dwarf().iter_CUs():
        source = value(cu.get_top_DIE(), 'DW_AT_name')
        if source not in {'functions/engine.c', 'functions/ai.c'}:
            continue
        for die in cu.get_top_DIE().iter_children():
            name = inherited_value(die, 'DW_AT_name', '')
            if die.tag != 'DW_TAG_variable' or not value(die, 'DW_AT_location') or name not in set(LIVE) | set(BACKUP):
                continue
            private = name in BACKUP
            base = name.removeprefix('backup0_').removeprefix('backup_')
            at = BACKUP[name] if private else LIVE[name][0]
            size = LIVE[base][2]
            typ = type_name(type_of(die))
            assert typ == (f'char[{size}]' if private else LIVE[name][1])
            assert value(die, 'DW_AT_location') == [3, *at.to_bytes(4, 'little')]
            symbols = [s for s in debug.symbols if s[0] == '_' + name]
            assert len(symbols) == 1
            symbol = symbols[0]
            section = debug.pe.sections[symbol[2] - 1]
            assert symbol[4] == (3 if private else 2) and section.Name.rstrip(b'\0') == b'.bss' and section.SizeOfRawData == 0
            assert debug.pe.OPTIONAL_HEADER.ImageBase + section.VirtualAddress + symbol[1] == at
            declaration = f'static uint8_t {name}[{size}];' if private else f'{typ.split("[")[0]} {name}[2];'
            proof['globals'][name] = dict(address=f'{at:x}', type=typ, bytes=size, private=private, source=source, declaration=declaration, die=hex(die.offset))
        cu._dielist.clear()
        cu._diemap.clear()
    assert len(proof['globals']) == 6
    rows = {r['name']: r for r in csv.DictReader((cache / '2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.detail = True
    def machine(name):
        row = rows[name]
        start = int(row['low_pc'], 16)
        code = runtime.pe.get_data(start - runtime.pe.OPTIONAL_HEADER.ImageBase, int(row['high_pc'], 16) - start)
        return dict(function_address=f'{start:x}', machine_bytes=len(code), machine_sha256=hashlib.sha256(code).hexdigest()), list(decoder.disasm(code, start))
    def absolute(op):
        return op.type == X86_OP_MEM and not op.mem.base and not op.mem.index
    for function, name in [('recalculate_all_cards_in_play', 'card_flags'), ('untap_phase', 'your_spells_cant_be_countered_by_color')]:
        info, instructions = machine(function)
        at, typ, size = LIVE[name]
        width = min(size, 4)
        sites = []
        for ins in instructions:
            ops = ins.operands
            if ins.mnemonic == 'mov' and absolute(ops[0]) and at <= ops[0].mem.disp < at + size:
                assert ops[0].size == width and ops[1].type == X86_OP_IMM and ops[1].imm == 0
                sites.append(dict(address=f'{ins.address:x}', bytes=ins.bytes.hex(), destination=f'{ops[0].mem.disp:x}', width=width))
        assert {int(s['destination'], 16) for s in sites} == set(range(at, at + size, width))
        proof['resets'][function] = dict(**info, global_name=name, instructions=sites)
    snapshots = [('backup_data_for_ai', 'backup_', False), ('restore_data_for_ai', 'backup_', True),
                 ('backup_data_for_ai_0', 'backup0_', False), ('restore_data_for_ai_0', 'backup0_', True)]
    for function, prefix, restoring in snapshots:
        pairs = {}
        for name, (live_at, typ, size) in LIVE.items():
            backup_at = BACKUP[prefix + name]
            src, dst = (backup_at, live_at) if restoring else (live_at, backup_at)
            width = min(size, 4)
            for offset in range(0, size, width):
                pairs[dst + offset] = (src + offset, width)
        info, instructions = machine(function)
        tracked, stores, sites = {}, {}, []
        def family(reg):
            name = decoder.reg_name(reg)
            return {'ax':'eax', 'al':'eax', 'ah':'eax', 'bx':'ebx', 'bl':'ebx', 'bh':'ebx', 'cx':'ecx', 'cl':'ecx', 'ch':'ecx', 'dx':'edx', 'dl':'edx', 'dh':'edx'}.get(name, name)
        for ins in instructions:
            ops = ins.operands
            new = None
            if ins.mnemonic == 'mov' and len(ops) == 2:
                if ops[0].type == X86_OP_REG and absolute(ops[1]) and (ops[1].mem.disp, ops[1].size) in pairs.values():
                    new = (family(ops[0].reg), (ops[1].mem.disp, ops[1].size))
                    sites.append(dict(address=f'{ins.address:x}', bytes=ins.bytes.hex(), role='source load'))
                if absolute(ops[0]) and ops[0].mem.disp in pairs:
                    dst = ops[0].mem.disp
                    assert ops[1].type == X86_OP_REG and tracked.get(family(ops[1].reg)) == pairs[dst] and ops[0].size == pairs[dst][1]
                    stores[dst] = pairs[dst]
                    sites.append(dict(address=f'{ins.address:x}', bytes=ins.bytes.hex(), role='destination store'))
            for reg in ins.regs_access()[1]:
                tracked.pop(family(reg), None)
            if ins.mnemonic == 'call':
                for reg in ('eax', 'ecx', 'edx'):
                    tracked.pop(reg, None)
            if new:
                tracked[new[0]] = new[1]
        assert stores == pairs and sum(p[1] for p in pairs.values()) == 10
        proof['copies'][function] = dict(**info, bytes=10, pairs=[dict(destination=f'{dst:x}', source=f'{src:x}', width=width) for dst, (src, width) in sorted(pairs.items())], instructions=sites)
    save_raw = (cache / 'decompiled-abi/26b1f10.c').read_text()
    assert 'save_or_load_data(card_flags,8)' in save_raw and 'save_or_load_data(your_spells_cant_be_countered_by_color,2)' in save_raw
    texts = {source: (root / source).read_bytes().decode('latin1').replace('\r\n', '\n') for source in ('functions/engine.c', 'functions/ai.c')}
    for source in texts:
        addition = '\n/* October 2022 per-player card and counterability flag storage. */\n' + '\n'.join(g['declaration'] for g in proof['globals'].values() if g['source'] == source) + '\n'
        texts[source] = texts[source].replace('#include "manalink.h"\n', '#include "manalink.h"\n' + addition, 1)
    fragments = []
    raw_dir = cache / 'decompiled-engine-flags'
    for function in [*proof['resets'], *proof['copies']]:
        source = rows[function]['source']
        address = rows[function]['low_pc'][2:]
        if function in proof['resets']:
            name = proof['resets'][function]['global_name']
            body = f'memset({name}, 0, sizeof({name}));'
        else:
            _, prefix, restoring = next(s for s in snapshots if s[0] == function)
            body = '\n  '.join(f'memcpy({name if restoring else prefix + name}, {prefix + name if restoring else name}, sizeof({name}));' for name in LIVE)
        start = f'/* Recovered 2022 player-flag fragment: {function} @ 0x{address}. */'
        end = f'/* End recovered 2022 player-flag fragment: {function}. */'
        raw_path = (raw_dir if function == 'untap_phase' else cache / 'decompiled-abi') / (address + '.c')
        raw = raw_path.read_text()
        fragments.append(dict(source=source, original_function=function, address=address, raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), start_marker=start, end_marker=end, body_sha256=hashlib.sha256(body.encode()).hexdigest(), recovery='Only verified complete card/counterability flag reset or AI snapshot; original body retained.'))
        signature = ('int untap_phase(int player)' if function == 'untap_phase' else f'void {function}(void)') + '\n{\n'
        assert texts[source].count(signature) == 1
        texts[source] = texts[source].replace(signature, signature + f'  {start}\n  {body}\n  {end}\n', 1)
    header = (root / 'manalink.h').read_bytes().decode('latin1').replace('\r\n', '\n')
    at = header.rfind('#ifdef __cplusplus')
    header = header[:at] + '/* October 2022 per-player card/counterability flags. */\n#ifndef SHANDALAR\n' + '\n'.join('extern ' + g['declaration'] for g in proof['globals'].values() if not g['private']) + '\n#endif\n\n' + header[at:]
    scratch = cache / 'engine-flags-check'
    scratch.mkdir(exist_ok=True)
    (scratch / 'manalink.h').write_bytes(header.encode('latin1'))
    for source, text in texts.items():
        path = scratch / Path(source).name
        path.write_bytes(text.encode('latin1'))
        result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99', '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-I', str(scratch), '-I', str(root), str(path)], capture_output=True, text=True)
        (scratch / (path.name + '.log')).write_text(result.stderr)
        result.check_returncode()
    if args.apply:
        p = root / 'docs/recovery-evidence/recovered-functions.json'
        manifest = json.loads(p.read_text())
        assert not set(proof['globals']) & {g['name'] for g in manifest['source_globals']}
        for source, text in texts.items():
            (root / source).write_bytes(text.replace('\n', '\r\n').encode('latin1'))
        (root / 'manalink.h').write_bytes(header.replace('\n', '\r\n').encode('latin1'))
        manifest['source_globals'].extend(dict(source=g['source'], name=name, declaration=g['declaration'], target_address=g['address'], zero_initialized=True, linkage='static' if g['private'] else 'external') for name, g in proof['globals'].items())
        manifest['source_fragments'].extend(fragments)
        p.write_text(json.dumps(manifest, indent=2))
        p = root / 'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(p.read_text())
        interfaces['player_flag_caches_2022'] = proof
        p.write_text(json.dumps(interfaces, indent=2))
        canonical_path = cache / 'decompiled-abi/export.tsv'
        rows = list(csv.DictReader(canonical_path.open(), delimiter='\t'))
        extra = list(csv.DictReader((raw_dir / 'export.tsv').open(), delimiter='\t'))
        assert len(extra) == 1 and extra[0]['address'] not in {r['address'] for r in rows}
        rows.extend(extra)
        shutil.copyfile(raw_dir / '26b08d0.c', canonical_path.parent / '26b08d0.c')
        with canonical_path.open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=rows[0], delimiter='\t')
            writer.writeheader()
            writer.writerows(rows)
    print(json.dumps(dict(applied=args.apply, globals=6, fragments=6, live_bytes=10, total_snapshot_bytes=40)))


if __name__ == '__main__':
    main()
