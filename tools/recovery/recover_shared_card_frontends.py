"""Recover public 2022 Vow and Lace helpers in their original source units."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from elftools.dwarf.descriptions import describe_DWARF_expr
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, strip_comments, UNRESOLVED
from recover_test_consumers import read, rename, format_c
from recover_literals import recover_strings
from audit_source import function_body

ADDRESSES = ['23e1320', '24bd8f0']
HEADER_LABEL = '/* October 2022 Vow and Lace APIs. */'


def sha(data):
    return hashlib.sha256(data).hexdigest()


def audit_component(component, manifest, sources, machine_bytes):
    def require(ok, message):
        if not ok:
            raise ValueError(message)
    entries = {f['address']: f for f in manifest['accepted_functions'] if f.get('shared_card_frontend_evidence')}
    require(set(entries) == set(ADDRESSES) == set(component['functions']), 'Shared card frontend membership mismatch')
    for address, entry in entries.items():
        require(entry['shared_card_frontend_evidence'] == component['functions'][address] and not entry['static'],
                'Shared card frontend evidence/linkage mismatch')
    require([len(component['parameters'][a]) for a in ADDRESSES] == [7, 4], 'Shared frontend argument count mismatch')
    require(component['target_locals'] == [dict(name='td', type='target_definition_t', location='(DW_OP_fbreg: -188)'),
                                         dict(name='td1', type='target_definition_t', location='(DW_OP_fbreg: -104)')],
            'Lace target storage mismatch')
    require(component['enum_values']['CHANGE_COLOR_SET'] == 2 and component['enum_values']['AL_STATIC'] == 4,
            'Shared frontend native enum mismatch')
    changes = component['functions']['24bd8f0']['changes']
    require(len(changes) == 4 and [c['replacement'] for c in changes[2:]] == ['pcVar1->info_slot = iVar2 != 0;']*2,
            'Lace boolean slot reconstruction mismatch')
    expected = [('24bdac4', '31c9'), ('24bdacc', '85c0'), ('24bdace', '0f95c1'), ('24bdad1', '894a38'),
                ('24bdaf6', '31c9'), ('24bdafe', '85c0'), ('24bdb00', '0f95c1'), ('24bdb03', '894a38')]
    require([(s['address'], s['bytes']) for c in changes[2:] for s in c['machine_sites']] == expected,
            'Lace full-word boolean store evidence mismatch')
    for address, data in expected:
        require(machine_bytes(int(address,16),len(bytes.fromhex(data))).hex() == data, 'Lace native boolean store mismatch')
    body = function_body(sources[entries['23e1320']['source']], 'vow_2022')
    require('event == EVENT_ABILITIES' in body and 'affected_card_controller' in body and 'affected_card' in body,
            'Vow affected-card/ability-event guard missing')
    lace = function_body(sources[entries['24bd8f0']['source']], 'lace_2022')
    require(lace.count('instance->info_slot = result != 0;') == 2 and lace.count('target_definition_2022_t') == 2,
            'Lace original assignment behavior/native target storage changed')


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root/'.recovery'
    mp = root/'docs/recovery-evidence/recovered-functions.json'
    ip = root/'docs/recovery-evidence/interface-recovery.json'
    manifest = json.loads(mp.read_text()); interfaces = json.loads(ip.read_text())
    assert 'shared_card_frontends_2022' not in interfaces
    assert not set(ADDRESSES) & {f['address'] for f in manifest['accepted_functions']}
    meta = {r['low_pc'][2:]: r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    names = {a: meta[a]['name']+'_2022' for a in ADDRESSES}
    debug_data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug = Image(debug_data); dwarf = debug.dwarf()
    proof = dict(debug_sha256=sha(debug_data), parameters={}, target_locals=[], enum_values={}, functions={},
                 scope='Native Vow guards and Lace event/target flow, retaining the boolean info_slot assignments in Lace. Two native 84-byte target locals use the recovered target API.',
                 integration_pending='Earlier source callers, native linking and gameplay verification remain pending.')
    wanted_enums = {'CHANGE_COLOR_SET', 'AL_STATIC', 'EVENT_ABILITIES', 'EVENT_CAN_CAST', 'EVENT_CAST_SPELL', 'EVENT_RESOLVE_SPELL', 'KILL_DESTROY'}
    for cu in dwarf.iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') not in {meta[a]['source'] for a in ADDRESSES}:
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_enumeration_type':
                for child in die.iter_children():
                    name = value(child, 'DW_AT_name')
                    if name in wanted_enums:
                        number = value(child, 'DW_AT_const_value')
                        assert name not in proof['enum_values'] or proof['enum_values'][name] == number
                        proof['enum_values'][name] = number
            pc = value(die, 'DW_AT_low_pc'); address = f'{pc:x}' if pc else ''
            if address not in ADDRESSES:
                continue
            proof['parameters'][address] = [dict(name=inherited_value(p,'DW_AT_name'), type=type_name(type_of(p)),
                location=describe_DWARF_expr(p.attributes['DW_AT_location'].value, dwarf.structs))
                for p in die.iter_children() if p.tag == 'DW_TAG_formal_parameter']
            if address == '24bd8f0':
                proof['target_locals'] = [dict(name=inherited_value(v,'DW_AT_name'), type=type_name(type_of(v)),
                    location=describe_DWARF_expr(v.attributes['DW_AT_location'].value,dwarf.structs))
                    for v in die.iter_children() if v.tag == 'DW_TAG_variable' and inherited_value(v,'DW_AT_name') in {'td','td1'}]
    runtime = Image((cache/'2022-ManalinkEh.dll').read_bytes())
    decoder = Cs(CS_ARCH_X86, CS_MODE_32); decoder.detail = True
    overlap = [line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2] == 'global']
    targets = {int(f['address'],16): f['name'] for f in manifest['accepted_functions']}
    entries, bodies, sources = [], {}, {}
    for address in ADDRESSES:
        row = meta[address]; name = names[address]
        raw = (cache/'decompiled-shared-card-frontends'/f'{address}.c').read_text()
        text = strip_comments(raw); changes = []
        native = runtime.pe.get_data(int(address,16)-0x2000000,int(row['high_pc'],16)-int(address,16))
        if address == '24bd8f0':
            for variable in ['aVar3', 'aVar4']:
                original = f'  anon_union_4_2_7165ea48_for_card_instance_struct_26 {variable};'
                assert text.count(original) == 1
                text = text.replace(original, '')
                changes.append(dict(original=original,replacement='',occurrences=1,kind='remove scalarized local union',machine_sites=[]))
            for variable, addresses in [('aVar3',['24bdac4','24bdacc','24bdace','24bdad1']), ('aVar4',['24bdaf6','24bdafe','24bdb00','24bdb03'])]:
                pattern = rf'{variable}\.info_slot\._1_3_ = 0;\s+{variable}\.info_slot\._0_1_ = iVar2 != 0;\s+pcVar1->field_26 = {variable};'
                matches = list(re.finditer(pattern,text)); assert len(matches) == 1
                original = matches[0][0]; replacement = 'pcVar1->info_slot = iVar2 != 0;'
                sites = []
                instructions = {f'{i.address:x}':i for i in decoder.disasm(native,int(address,16))}
                for site in addresses:
                    ins = instructions[site]; sites.append(dict(address=site,bytes=ins.bytes.hex()))
                changes.append(dict(original=original,replacement=replacement,occurrences=1,kind='full-word boolean info-slot store',machine_sites=sites))
                text = text.replace(original,replacement)
        text, _ = rename(text,{row['name']:name})
        text, literals = recover_strings(text,runtime,{})
        body = format_c(clean(text,overlap,use_target_api_2022=True,use_test_api_2022=True,use_spell_api_2022=True,
                              use_ability_api_2022=True,use_pump_api_2022=True,use_keyword_accessors_2022=True,use_extension_flags_2022=True))
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x',body),address
        calls = []
        for ins in decoder.disasm(native,int(address,16)):
            if ins.mnemonic in {'call','jmp'} and ins.operands[0].type == X86_OP_IMM and ins.operands[0].imm in targets:
                target = targets[ins.operands[0].imm]
                assert re.search(r'\b'+target+r'\s*\(',body),(address,target)
                calls.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{ins.operands[0].imm:x}',name=target))
        symbols = [s for n,off,sec,k,s in debug.functions() if debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off==int(address,16)]
        assert symbols == [2]
        detail = dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,canonical_calls=calls)
        proof['functions'][address] = detail
        entries.append(dict(name=name,address=address,source=row['source'],line=row['line'],static=False,
            prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,
            shared_card_frontend_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
        bodies[address] = body
        sources.setdefault(row['source'],read(root/row['source']))
        sources[row['source']] += f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{address}; original {row["source"]}:{row["line"]}. */\n'+body+'\n#endif\n'
    header = read(root/'manalink.h'); at = header.rfind('#ifdef __cplusplus')
    header = header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries)+'\n#endif\n\n'+header[at:]
    scratch = cache/'shared-card-frontends-check'; scratch.mkdir(exist_ok=True)
    (scratch/'manalink.h').write_bytes(header.encode('latin1'))
    for source,text in sources.items():
        path = scratch/Path(source).name; path.write_bytes(text.encode('latin1'))
        result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only',
            '-ferror-limit=0','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type',
            '-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(path)],capture_output=True,text=True)
        path.with_suffix('.log').write_text(result.stderr)
        if result.returncode:
            print(result.stderr[-7000:])
        result.check_returncode()
    audit_component(proof,{**manifest,'accepted_functions':manifest['accepted_functions']+entries},sources,
                    lambda a,n: runtime.pe.get_data(a-0x2000000,n))
    if args.apply:
        citations = manifest['source_fragments']+manifest['accepted_functions']+[s for f in manifest['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not any(c['address'] in ADDRESSES for c in citations)
        for source,text in sources.items():
            (root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        manifest['accepted_functions'] += entries
        manifest['accepted'] += 2; manifest['helper_functions'] += 2; manifest['shared_helper_interfaces'] += 2
        for entry in entries:
            manifest['source_files'][entry['source']] = manifest['source_files'].get(entry['source'],0)+1
        mp.write_text(json.dumps(manifest,indent=2))
        interfaces['shared_card_frontends_2022'] = proof; ip.write_text(json.dumps(interfaces,indent=2))
        for address in ADDRESSES:
            shutil.copyfile(cache/'decompiled-shared-card-frontends'/f'{address}.c',cache/'decompiled-abi'/f'{address}.c')
    (cache/'shared-card-frontends-plan.json').write_text(json.dumps(proof,indent=2))
    print(json.dumps(dict(applied=args.apply,new_helpers=2,new_public_helpers=2)))


if __name__ == '__main__':
    main()
