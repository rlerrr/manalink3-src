"""Recover Aether Revolt handlers with verified packed costs and energy-byte reads."""
import argparse
import collections
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM, X86_OP_MEM
from inspect_release import Image, value
from promote_recovery import clean, strip_comments, UNRESOLVED
from recover_literals import recover_strings
from recover_test_consumers import read, format_c
from audit_source import function_body

ADDRESSES = '20bd2c0 20bf370 20c1230 20c14d0 20c15d0 20c4c70 20c67a0 20c9770 20c9880 20cae10'.split()
ENUMS = dict(PCOUNTER_POISON=0, PCOUNTER_EXPERIENCE=1, PCOUNTER_ENERGY=2,
             GAA_UNTAPPED=1, GAA_CAN_ONLY_TARGET_OPPONENT=0x400000)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    mp = root / 'docs/recovery-evidence/recovered-functions.json'
    ip = root / 'docs/recovery-evidence/interface-recovery.json'
    manifest = json.loads(mp.read_text())
    interfaces = json.loads(ip.read_text())
    assert 'energy_card_operands_2022' not in interfaces
    assert not set(ADDRESSES) & {f['address'] for f in manifest['accepted_functions']}
    meta = {r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    debug_data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug = Image(debug_data)
    enums = {}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'cards/aether_revolt.c':
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_enumeration_type':
                for child in die.iter_children():
                    name = value(child, 'DW_AT_name')
                    if name in ENUMS:
                        enums[name] = value(child, 'DW_AT_const_value')
    assert enums == ENUMS
    runtime = Image((cache/'2022-ManalinkEh.dll').read_bytes())
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.detail = True
    proof = dict(debug_sha256=sha(debug_data), enum_values=enums, functions={},
        player_counters=interfaces['dependency_repairs_2022']['player_counters'],
        cost_word=dict(bytes=4,kind_shift=24,counter_shift=16,quantity_shift=8,life_shift=0,
                       player_kind=2,energy_counter=2,physical_argument=10,stack_offset=40),
        mutable_prompt_local=dict(address='20bf370',name='prompt',type='char *',reason='Match the native activated API prompt parameter; do not add a const qualifier to this local.'),
        deferred=dict(address='20c1380',name='card_gontis_aether_heart',reason='Native new_specific_cip aggregate interface still requires recovery.'),
        scope='Ten native Aether Revolt handlers. Decode packed energy costs only with native outgoing argument stores; read unsigned energy bytes through existing player-counter fields. Preserve all native events, targets, AI branches and return values.',
        integration_pending='Native linking, earlier caller migrations and gameplay verification remain pending.')
    targets = {int(f['address'],16):f['name'] for f in manifest['accepted_functions']}
    overlap = [x.split('\t')[1] for x in (cache/'absolute-interfaces.tsv').read_text().splitlines() if x.split('\t')[2]=='global']
    entries = []
    source = 'cards/aether_revolt.c'
    content = read(root/source) + '\n\n#ifndef SHANDALAR\n/* Recovered October 2022 energy-card operands. */\n'
    for address in ADDRESSES:
        row = meta[address]
        name = row['name']
        assert row['source'] == source
        native = runtime.pe.get_data(int(address,16)-0x2000000,int(row['high_pc'],16)-int(address,16))
        instructions = list(decoder.disasm(native,int(address,16)))
        raw = (cache/'decompiled-abi'/(address+'.c')).read_text()
        text = strip_comments(raw)
        changes = []
        sites = []
        def change(original, replacement, evidence):
            nonlocal text
            count = text.count(original)
            assert count > 0 and evidence, (address, original)
            changes.append(dict(original=original,replacement=replacement,occurrences=count,
                                kind='verified packed activation cost or unsigned energy byte',machine_sites=evidence))
            text = text.replace(original,replacement)
        def site(ins):
            return dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),instruction=ins.mnemonic+' '+ins.op_str)
        for number in [1,2,3]:
            word = 0x2020000 | (number << 8)
            operand_sites = []
            for i in instructions:
                if (i.mnemonic=='mov' and len(i.operands)==2 and i.operands[0].type==X86_OP_MEM and
                    i.reg_name(i.operands[0].mem.base)=='esp' and i.operands[0].mem.disp==40 and
                    i.operands[0].size==4 and i.operands[1].type==X86_OP_IMM and i.operands[1].imm==word):
                    operand_sites.append(site(i))
            token = f'0x{word:x}'
            if token in text:
                change(token,f'GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, {number})',operand_sites)
                sites.extend(dict(**s,kind='packed_cost',value=word) for s in operand_sites)
            if address=='20bf370' and number==3:
                change('undefined *variable_costs;','uint32_t variable_costs;',operand_sites)
                change('(undefined *)0x0','0',operand_sites)
                change('&UNK_02020300','GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 3)',operand_sites)
        energy_sites = [site(i) for i in instructions if any(o.type==X86_OP_MEM and o.size==1 and o.mem.disp==0x4ef532 and o.mem.scale==4 for o in i.operands)]
        if energy_sites:
            token = '*(char *)(player * 4 + 0x4ef532)' if address=='20c9880' else '*(byte *)(player * 4 + 0x4ef532)'
            change(token,'ENERGY_COUNTERS(player)',energy_sites)
            sites.extend(dict(**s,kind='energy_byte') for s in energy_sites)
        if address=='20cae10':
            mode_sites = [site(i) for i in instructions if i.mnemonic=='mov' and len(i.operands)==2 and
                i.operands[0].type==X86_OP_MEM and i.reg_name(i.operands[0].mem.base)=='esp' and
                i.operands[0].mem.disp==12 and i.operands[1].type==X86_OP_IMM and i.operands[1].imm==0x400001]
            change('0x400001','(GAA_UNTAPPED | GAA_CAN_ONLY_TARGET_OPPONENT)',mode_sites)
            sites.extend(dict(**s,kind='activation_mode',value=0x400001) for s in mode_sites)
        text,literals = recover_strings(text,runtime,{})
        body = clean(text,overlap,use_test_api_2022=True,use_target_api_2022=True,
            use_extension_flags_2022=True,use_keyword_accessors_2022=True,use_ability_api_2022=True,
            use_pump_api_2022=True,use_activated_api_2022=True,use_attack_api_2022=True,use_entry_api_2022=True)
        if address=='20bf370':
            assert body.count('const char *prompt;')==1
            body=body.replace('const char *prompt;','char *prompt;')
        body=format_c(body)
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x|\b0x(?:4ef532|2020[1238]00)\b',body),address
        calls = []
        for i in instructions:
            if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm in targets:
                target = targets[i.operands[0].imm]
                assert re.search(r'\b'+target+r'\s*\(',body),(address,target)
                calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{i.operands[0].imm:x}',name=target))
        symbols = [s for n,off,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off==int(address,16)]
        assert symbols==[2],(address,symbols)
        detail = dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,canonical_calls=calls,operand_sites=sites)
        proof['functions'][address] = detail
        entries.append(dict(name=name,address=address,source=source,line=row['line'],static=False,
            prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,
            energy_card_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
        content += f'\n/* ManalinkEh 2022: 0x{address}; original {source}:{row["line"]}. */\n'+body+'\n'
    content += '#endif\n'
    scratch = cache/'energy-cards-check'
    scratch.mkdir(exist_ok=True)
    path = scratch/'aether_revolt.c'
    path.write_bytes(content.encode('latin1'))
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0',
        '-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized',
        '-Werror=format','-I',str(root),str(path)],capture_output=True,text=True)
    (scratch/'compile.log').write_text(result.stderr)
    if result.returncode:print(result.stderr[-9000:])
    result.check_returncode()
    if args.apply:
        (root/source).write_bytes(content.replace('\n','\r\n').encode('latin1'))
        manifest['accepted_functions'] += entries
        manifest['accepted'] += len(entries)
        manifest['card_handler_functions'] += len(entries)
        manifest['source_files'][source] = manifest['source_files'].get(source,0)+len(entries)
        for entry in entries:
            manifest.get('deferred',{}).pop(entry['name'],None)
        manifest['deferred_reason_counts']=dict(collections.Counter(reason.split(':')[0] for reason in manifest.get('deferred',{}).values()))
        registrations = list(csv.DictReader((cache/'2022-registrations.csv').open()))
        recovered = {f['name'] for f in manifest['accepted_functions']}
        manifest['registered_handler_symbols'] = len({r['symbol'].lstrip('_') for r in registrations if r['symbol'].lstrip('_') in recovered})
        manifest['registered_handler_symbols_marked_coded'] = len({r['symbol'].lstrip('_') for r in registrations if r['coded']=='True' and r['symbol'].lstrip('_') in recovered})
        mp.write_text(json.dumps(manifest,indent=2))
        interfaces['energy_card_operands_2022'] = proof
        ip.write_text(json.dumps(interfaces,indent=2))
    (cache/'energy-cards-plan.json').write_text(json.dumps(proof,indent=2))
    print(json.dumps(dict(applied=args.apply,new_card_functions=len(entries),operand_sites=sum(len(x['operand_sites']) for x in proof['functions'].values()))))


if __name__ == '__main__':
    main()
