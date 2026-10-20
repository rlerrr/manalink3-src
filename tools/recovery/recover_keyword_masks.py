"""Name ambiguous numeric keyword masks in verified argument/field contexts."""
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image, value
from promote_recovery import strip_comments, rewrite_argument, project_extension_keywords


NAMES = {1:'SP_KEYWORD_FEAR', 2:'SP_KEYWORD_LIFELINK', 4:'SP_KEYWORD_HASTE',
    8:'SP_KEYWORD_VIGILANCE', 16:'SP_KEYWORD_UNBLOCKABLE', 32:'SP_KEYWORD_CANNOT_BLOCK',
    256:'SP_KEYWORD_SHADOW', 512:'SP_KEYWORD_DEATHTOUCH', 1024:'SP_KEYWORD_MUST_ATTACK',
    2048:'SP_KEYWORD_INDESTRUCTIBLE', 4096:'SP_KEYWORD_MUST_BE_BLOCKED',
    8192:'SP_KEYWORD_FLANKING', 16384:'SP_KEYWORD_WITHER', 32768:'SP_KEYWORD_BUSHIDO',
    65536:'SP_KEYWORD_MENACE', 131072:'SP_KEYWORD_SKULK', 0x100000:'SP_KEYWORD_RFG_WHEN_DAMAGE',
    0x400000:'SP_KEYWORD_LURE', 0x800000:'SP_KEYWORD_HEXPROOF'}
ARGUMENTS = {'special_abilities': (6,3), 'pump_ability_until_eot': (8,7),
    'pump_ability_until_eot_no_repeat': (6,5), 'pump_ability_until_eot_no_repeat_alt': (7,5),
    'check_for_special_ability': (3,2), 'remove_special_ability': (3,2)}


def main():
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    manifest = json.loads((root / 'docs/recovery-evidence/recovered-functions.json').read_text())
    accepted = {f['address'] for f in manifest['accepted_functions']}
    requests = {}; wanted = {}
    for row in csv.DictReader((cache / 'decompiled-abi/export.tsv').open(), delimiter='\t'):
        if row['address'] in accepted or row['status'] != 'ok' or not row['source'].endswith('.c'):
            continue
        raw = (cache / 'decompiled-abi' / (row['address'] + '.c')).read_text()
        body = strip_comments(raw); replacements = []
        for literal in sorted(set(re.findall(r'\b0x[0-9a-f]+\b', body))):
            mask = int(literal,16)
            if not 0x400000 <= mask < 0xa00000:
                continue
            bits = {name: bit for bit,name in NAMES.items() if bit & mask}
            if sum(bits.values()) != mask:
                continue
            expression = '(' + ' | '.join(bits) + ')'
            for function, (arity,argument) in ARGUMENTS.items():
                if rewrite_argument(body,function,arity,argument,{literal:expression}) != body:
                    replacements.append(dict(context='argument', function=function, arity=arity,
                        argument=argument, literal=literal, expression=expression, mask=mask, bits=bits))
            # Only the verified first extension keyword word, on typed pointers.
            _, projections = project_extension_keywords(body)
            for original in sorted({p['original'] for p in projections if p['extension_offset'] == 0}):
                for match in re.finditer(re.escape(original) + r'\s*([&|])\s*' + re.escape(literal) + r'\b', body):
                    replacements.append(dict(context='extension_word', original=match[0],
                        replacement=match[0][:match.start(1)-match.start()] + match[1] + ' ' + expression,
                        literal=literal, expression=expression, mask=mask, bits=bits))
        if replacements:
            requests[row['address']] = dict(name=row['name'], source=row['source'], raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), replacements=replacements)
            wanted.setdefault(row['source'],set()).update(n for r in replacements for n in r['bits'])
    data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug = Image(data); units = {}
    for cu in debug.dwarf().iter_CUs():
        source = value(cu.get_top_DIE(),'DW_AT_name')
        if source not in wanted:
            continue
        enums = {}
        for die in cu.get_top_DIE().iter_children():
            if die.tag != 'DW_TAG_enumeration_type': continue
            for child in die.iter_children():
                name = value(child,'DW_AT_name')
                if name in wanted[source]:
                    record = dict(value=value(child,'DW_AT_const_value'), die=hex(child.offset), enum_die=hex(die.offset))
                    assert name not in enums or enums[name]['value'] == record['value']
                    enums[name] = record
        assert wanted[source] <= set(enums)
        for name in wanted[source]: assert enums[name]['value'] == next(b for b,n in NAMES.items() if n==name)
        units[source] = enums
        cu._dielist.clear(); cu._diemap.clear()
    assert set(units) == set(wanted)
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    functions = {r['low_pc'][2:]: r for r in csv.DictReader((cache / '2022-functions.csv').open()) if r['low_pc']}
    decoder = Cs(CS_ARCH_X86,CS_MODE_32); decoder.detail = True
    rejected = {}
    for address, request in list(requests.items()):
        start = int(address,16); end = int(functions[address]['high_pc'],16)
        code = runtime.pe.get_data(start-runtime.pe.OPTIONAL_HEADER.ImageBase,end-start)
        instructions = list(decoder.disasm(code,start))
        for replacement in request['replacements']:
            sites = [dict(address=f'{i.address:x}', bytes=i.bytes.hex(), instruction=i.mnemonic+' '+i.op_str)
                for i in instructions if i.mnemonic in {'mov','push','or','and','test'} and
                any(o.type==X86_OP_IMM and o.imm & 0xffffffff == replacement['mask'] for o in i.operands)]
            if not sites:
                rejected[address] = request['name']; break
            replacement['machine_sites'] = sites
        else:
            request['machine_sha256'] = hashlib.sha256(code).hexdigest()
            request['enumerators'] = units[request['source']]
            continue
        del requests[address]
    used = {n:b for r in requests.values() for e in r['replacements'] for n,b in e['bits'].items()}
    probe = cache / 'keyword-mask-check.c'
    probe.write_text('#include "manalink.h"\n' + '\n'.join(f'_Static_assert({n} == {b}, "keyword mask");' for n,b in used.items()))
    subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99',
        '-fsyntax-only','-I',str(root),str(probe)],check=True)
    path = root / 'docs/recovery-evidence/keyword-masks.json'
    saved = json.loads(path.read_text()) if path.exists() else dict(debug_sha256=hashlib.sha256(data).hexdigest(), functions={})
    assert saved['debug_sha256'] == hashlib.sha256(data).hexdigest()
    saved['functions'] = {a:r for a,r in saved['functions'].items() if a in accepted}
    saved['functions'].update(requests)
    saved['scope'] = 'Known special-keyword arguments and typed first extension keyword word only. Per-unit DWARF enum values, unchanged numeric bits, and release instruction operands verified.'
    path.write_text(json.dumps(saved,indent=2))
    print(json.dumps(dict(prepared=len(requests), replacements=sum(len(r['replacements']) for r in requests.values()), rejected=rejected),indent=2))


if __name__ == '__main__':
    main()
