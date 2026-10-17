"""Supplement untyped DWARF EXE names with address-matched source declarations.

Some absolute DWARF subprogram DIEs contain only a name, with no return type or
prototype. They are not declarations of void functions taking zero arguments.
Only use a source declaration when manalink.lds agrees with the target address.
"""
import json
import re
import subprocess
from pathlib import Path

from promote_recovery import strip_comments


def main():
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    rows = [line.split('\t') for line in (cache / 'absolute-interfaces.tsv').read_text().splitlines()]
    unknown_file = cache / 'unprototyped-absolute-interfaces.tsv'
    if unknown_file.exists():
        rows.extend(line.split('\t') for line in unknown_file.read_text().splitlines())
    untyped = {row[1]: row for row in rows if row[2] == 'function' and len(row) == 4 and row[3] == 'void'}
    linker = {name: int(address, 16) for name, address in re.findall(
        r'(?m)^_(\w+)\s*=\s*(0x[0-9a-fA-F]+)\s*;', (root / 'manalink.lds').read_text())}
    source = '#include "manalink.h"\n'
    declarations = {}
    for folder in ['functions', 'cards']:
        for path in (root / folder).glob('*.c'):
            text = strip_comments(path.read_bytes().decode('latin1'))
            for match in re.finditer(r'(?m)^[ \t]*((?:int|void|unsigned int)\s+(\w+)\s*\([^;{}()]*\))\s*;', text):
                prototype, name = match.groups()
                if name in untyped and name not in declarations:
                    declarations[name] = dict(source=path.relative_to(root).as_posix(), prototype=prototype)
                    source += prototype + ';\n'
    scratch = cache / 'engine-interface-types.c'
    scratch.write_text(source)
    command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc',
               '-std=gnu99', '-fsyntax-only', '-Xclang', '-ast-dump=json', '-I', str(root), str(scratch)]
    result = subprocess.run(command, capture_output=True, text=True, check=True)
    ast = json.loads(result.stdout)
    enum_types = set()
    def has_enum(node):
        return node.get('kind') == 'EnumType' or any(has_enum(c) for c in node.get('inner', []))
    for node in ast.get('inner', []):
        if node.get('kind') == 'TypedefDecl' and has_enum(node):
            enum_types.add(node['name'])
    types = {}
    def walk(node):
        if node.get('kind') == 'FunctionDecl' and node.get('name') in untyped:
            name = node['name']
            signature = node['type']['qualType']
            if signature.endswith('()') or '(*' in signature or node.get('variadic'):
                return
            return_type = signature.split('(')[0].strip()
            parameters = [child['type']['qualType'] for child in node.get('inner', [])
                          if child.get('kind') == 'ParmVarDecl']
            types[name] = [return_type, *parameters]
        for child in node.get('inner', []):
            walk(child)
    walk(ast)
    output, evidence = [], []
    used_enums = set()
    for name, signature in types.items():
        row = untyped[name]
        if linker.get(name) != int(row[0], 16):
            raise ValueError('Source/target absolute address mismatch: ' + name)
        # Some source enum typedef names were changed or removed in the target.
        # On this x86 ABI their scalar arguments occupy the same int slots.
        abi_signature = ['int' if t in enum_types else re.sub(r'\b(?:const|volatile|restrict)\s+', '', t)
                         for t in signature]
        abi_signature = ['unsigned int' if t == 'size_t' else t for t in abi_signature]
        used_enums.update(t for t in signature if t in enum_types)
        output.append([row[0], name, *abi_signature])
        evidence.append(dict(name=name, address=row[0], signature=signature,
                             applied_abi_signature=abi_signature,
                             declaration=declarations.get(name, dict(source='manalink.h')),
                             binding_source='manalink.lds'))
    (cache / 'engine-interfaces.tsv').write_text(''.join('\t'.join(row) + '\n' for row in output))
    abi_source = '#include "manalink.h"\n' + ''.join(
        f'_Static_assert(sizeof({name}) == 4, "{name} ABI width");\n'
        for name in sorted(used_enums | {'size_t', 'void *'}))
    abi_file = cache / 'engine-interface-abi.c'
    abi_file.write_text(abi_source)
    subprocess.run(command[:command.index('-Xclang')] + ['-I', str(root), str(abi_file)],
                   capture_output=True, text=True, check=True)
    (cache / 'engine-interface-abi.json').write_text(json.dumps(
        dict(target='i686-pc-windows-msvc', width=4, types=sorted(used_enums | {'size_t', 'void *'})), indent=2))
    (cache / 'engine-interfaces.json').write_text(json.dumps(evidence, indent=2))
    print(json.dumps(dict(untyped=len(untyped), source_declarations=len(output),
                         still_unresolved=sorted(set(untyped) - types.keys())), indent=2))


if __name__ == '__main__':
    main()
