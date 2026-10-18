"""Place the three changed public APIs whose original names already exist in source."""
import argparse
import collections
import hashlib
import json
import re
import subprocess
from pathlib import Path

from promote_recovery import clean, code_only, UNRESOLVED
from recover_batch17_interfaces import ENTRIES


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    proof = json.loads((root / 'docs/recovery-evidence/interface-recovery.json').read_text())['batch17_interfaces']
    manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
    manifest = json.loads(manifest_path.read_text())
    addresses = {'26d5970', '26d5a00', '269b2e0'}
    assert not addresses & {f['address'] for f in manifest['accepted_functions']}
    additions = collections.defaultdict(str); entries = []
    for address, original, name, source, count, linkage in ENTRIES:
        if address not in addresses:
            continue
        raw = (cache / 'decompiled-abi' / (address + '.c')).read_text()
        body = clean(raw)
        assert not UNRESOLVED.search(code_only(body)) and re.search(r'\b' + name + r'\s*\(', body)
        body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
            '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
            input=body.encode(), capture_output=True, check=True).stdout.decode()
        line = proof['functions'][address]['line']
        additions[source] += f'\n\n/* ManalinkEh 2022: 0x{address}; original {source}:{line}. */\n' + body
        entries.append(dict(name=name, original_name=original, address=address, source=source,
            line=line, static=False, prototype=body[:body.index('{')].strip() + ';',
            raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=[],
            reconstruction='Full target body under a distinct public interface; the old source API and body are retained.',
            interface_evidence=proof['functions'][address]))
    originals = {s: (root / s).read_bytes() for s in additions}
    for source, addition in additions.items():
        prototypes = '\n'.join(e['prototype'] for e in entries if e['source'] == source) + '\n'
        candidate = cache / ('batch17-wrappers-' + Path(source).name)
        candidate.write_bytes(originals[source] + ('\n' + prototypes + addition).encode())
        result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc',
            '-std=gnu99', '-fsyntax-only', '-Werror=implicit-function-declaration',
            '-Werror=incompatible-pointer-types', '-Werror=return-type', '-Werror=uninitialized',
            '-Werror=sometimes-uninitialized', '-Werror=format', '-I', str(root), str(candidate)],
            capture_output=True, text=True)
        (cache / ('batch17-wrappers-' + Path(source).stem + '-check.log')).write_text(result.stderr)
        result.check_returncode()
    if args.apply:
        for source, addition in additions.items():
            (root / source).write_bytes(originals[source].rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        p = root / 'manalink.h'; header = p.read_bytes(); at = header.rfind(b'#ifdef __cplusplus')
        block = '/* October 2022 counter movement and bounded name-exile APIs. */\n' + '\n'.join(e['prototype'] for e in entries) + '\n\n'
        p.write_bytes(header[:at] + block.replace('\n', '\r\n').encode() + header[at:])
        manifest['accepted_functions'].extend(entries); manifest['accepted'] = len(manifest['accepted_functions'])
        manifest_path.write_text(json.dumps(manifest, indent=2))
    print(json.dumps(dict(applied=args.apply, helpers=[e['name'] for e in entries]), indent=2))


if __name__ == '__main__':
    main()
