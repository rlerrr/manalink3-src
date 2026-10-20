"""Expose retained life-upkeep callbacks after verifying their 2022 public interfaces."""
import argparse
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path
from inspect_release import Image
from audit_source import function_body


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    source = 'cards/weatherlight.c'
    text = (root / source).read_bytes().decode('latin1').replace('\r\n', '\n')
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    metadata = {r['name']: r for r in csv.DictReader((cache / '2022-functions.csv').open()) if r['low_pc'] and r['high_pc'] and r['source'] == source}
    entries = {}
    for name in ('can_pay_1_life_thunk', 'lose_1_life_thunk'):
        row = metadata[name]
        address = int(row['low_pc'], 16)
        assert any(n == '_' + name and storage == 2 and runtime.pe.OPTIONAL_HEADER.ImageBase + runtime.pe.sections[s - 1].VirtualAddress + off == address for n, off, s, kind, storage in runtime.functions())
        signature = 'int ' + name + '(int player, int card, int number_of_age_counters)'
        assert text.count('static ' + signature) == 1
        body = function_body(text, name)
        expected = (name + '(int player, int card, int number_of_age_counters)\n{\n  return can_pay_life(player, number_of_age_counters);\n}' if name.startswith('can_pay') else name + '(int player, int card, int number_of_age_counters)\n{\n  lose_life(player, number_of_age_counters);\n  return 1;\n}')
        assert re.sub(r'\s+', '', body) == re.sub(r'\s+', '', expected)
        raw = (cache / 'decompiled-abi' / (f'{address:x}' + '.c')).read_text()
        assert name + '(int player,int card,int number_of_age_counters)' in raw
        assert ('can_pay_life(player,number_of_age_counters)' if name.startswith('can_pay') else 'lose_life(player,number_of_age_counters)') in raw
        code = runtime.pe.get_data(address - runtime.pe.OPTIONAL_HEADER.ImageBase, int(row['high_pc'], 16) - address)
        entries[name] = dict(address=f'{address:x}', source=source, prototype=signature + ';', body_sha256=hashlib.sha256(body.encode()).hexdigest(), raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), machine_bytes=len(code), machine_sha256=hashlib.sha256(code).hexdigest())
        text = text.replace('static ' + signature, signature, 1)
    header = (root / 'manalink.h').read_bytes().decode('latin1').replace('\r\n', '\n')
    at = header.rfind('#ifdef __cplusplus')
    header = header[:at] + '/* Public 2022 life-upkeep callback interfaces; retained source bodies. */\n#ifndef SHANDALAR\n' + '\n'.join(e['prototype'] for e in entries.values()) + '\n#endif\n\n' + header[at:]
    scratch = cache / 'life-upkeep-check'
    scratch.mkdir(exist_ok=True)
    (scratch / 'manalink.h').write_bytes(header.encode('latin1'))
    (scratch / 'weatherlight.c').write_bytes(text.encode('latin1'))
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99', '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-I', str(scratch), '-I', str(root), str(scratch / 'weatherlight.c')], capture_output=True, text=True)
    (scratch / 'check.log').write_text(result.stderr)
    result.check_returncode()
    if args.apply:
        (root / source).write_bytes(text.replace('\n', '\r\n').encode('latin1'))
        (root / 'manalink.h').write_bytes(header.replace('\n', '\r\n').encode('latin1'))
        p = root / 'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(p.read_text())
        assert 'life_upkeep_thunks' not in interfaces
        interfaces['life_upkeep_thunks'] = dict(functions=entries, new_implementations=0, scope='Remove static from two existing source callbacks to match verified external target linkage and allow cross-set cumulative-upkeep callers. Function bodies retained exactly.')
        p.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, exposed_callbacks=2, new_implementations=0)))


if __name__ == '__main__':
    main()
