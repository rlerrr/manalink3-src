"""Recover attacker trigger storage and native Support/Battle Cry/Renown/Retrace."""
import argparse
import copy
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from elftools.dwarf.locationlists import LocationParser, LocationExpr, BaseAddressEntry
from elftools.dwarf.descriptions import describe_DWARF_expr
from promote_recovery import clean, strip_comments, UNRESOLVED
from recover_test_consumers import read, format_c, rename
from recover_literals import recover_strings, readonly_string
from audit_source import function_body
from attacker_core_bodies import reconstruct

ADDRESSES = '268d340 268d6a0 268d7e0 268d940 268d840 225a8c0 232b5e0 20d9ef0 23636c0'.split()
DAT_ADDRESS = '268d9a0'
CONTROL_SITES = '268d403 268d43e 268d497 268d571 268d57d 268d582 268d588 268d5c5 268d609 268d614 268d619 268d61a 268d651 268d74a 268d75b 268d797 225a948 225a966 225aa7a 225aa98 225a9d3 225aa63 225a99b'.split()


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
    assert 'attacker_core_2022' not in interfaces
    accepted = {f['address']: f for f in manifest['accepted_functions']}
    assert not set(ADDRESSES) & set(accepted)
    meta = {r['low_pc'][2:]: r for r in csv.DictReader((cache/'2022-functions.csv').open())
            if r['low_pc'] and r['high_pc']}
    debug_data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug = Image(debug_data)
    dwarf = debug.dwarf()
    locations = LocationParser(dwarf.location_lists())
    parameters, locals_, enums = {}, {}, {}
    wanted = set(ADDRESSES) | {DAT_ADDRESS}
    sources_wanted = {meta[a]['source'] for a in wanted}
    for cu in dwarf.iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') not in sources_wanted:
            continue

        def location(die):
            attr = die.attributes.get('DW_AT_location')
            if attr is None:
                return None
            loc = locations.parse_from_attribute(attr, cu['version'], die=die)
            if isinstance(loc, LocationExpr):
                return describe_DWARF_expr(loc.loc_expr, dwarf.structs)
            return [dict(base=hex(x.base_address)) if isinstance(x, BaseAddressEntry) else
                    dict(start=hex(x.begin_offset), end=hex(x.end_offset),
                         expression=describe_DWARF_expr(x.loc_expr, dwarf.structs)) for x in loc]

        def variables(die):
            result = []
            for child in die.iter_children():
                if child.tag == 'DW_TAG_variable':
                    result.append(dict(name=inherited_value(child, 'DW_AT_name'),
                                       type=type_name(type_of(child)), location=location(child)))
                elif child.tag in {'DW_TAG_lexical_block', 'DW_TAG_inlined_subroutine'}:
                    result.extend(variables(child))
            return result

        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_enumeration_type':
                enums.update({value(c, 'DW_AT_name'): value(c, 'DW_AT_const_value')
                              for c in die.iter_children() if c.tag == 'DW_TAG_enumerator'})
            pc = value(die, 'DW_AT_low_pc')
            address = f'{pc:x}' if pc else ''
            if address not in wanted:
                continue
            parameters[address] = [dict(name=inherited_value(p, 'DW_AT_name'),
                                        type=type_name(type_of(p)), location=location(p))
                                   for p in die.iter_children() if p.tag == 'DW_TAG_formal_parameter']
            locals_[address] = variables(die)
    assert set(parameters) == wanted
    assert any(v['name']=='targs' and v['type']=='target_t[152]' and
               v['location']=='(DW_OP_fbreg: -1236)' for v in locals_['225a8c0'])
    assert all(any(v['name']=='storage' and v['type']=='int32_t *' for v in locals_[a])
               for a in ['268d340', '268d6a0'])
    names = {a: meta[a]['name']+'_2022' for a in ADDRESSES}
    mapping = {meta[a]['name']: name for a, name in names.items()}
    runtime = Image((cache/'2022-ManalinkEh.dll').read_bytes())
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.detail = True
    native, instructions = {}, {}
    for address in wanted:
        native[address] = runtime.pe.get_data(int(address,16)-0x2000000,
                                             int(meta[address]['high_pc'],16)-int(address,16))
        instructions[address] = list(decoder.disasm(native[address], int(address,16)))
    by_site = {f'{i.address:x}': i for items in instructions.values() for i in items}
    controls = [dict(address=a, bytes=by_site[a].bytes.hex(),
                     instruction=by_site[a].mnemonic+' '+by_site[a].op_str) for a in CONTROL_SITES]
    support_prompt=readonly_string(runtime,0x27389c4)
    assert support_prompt==b'Select another target creature. (%d of %d)'
    proof = dict(debug_sha256=sha(debug_data), parameters=parameters, locals=locals_, enum_values={},
                 flags={n:v for n,v in enums.items() if n.startswith('DAT_')}, control_sites=controls,
                 storage=dict(word_bytes=4, count_offsets=[56,124,140], attacker_bytes_offset=132,
                              tracked_capacity=64, separate_trigger_uses_count_before_decrement=True),
                 support_copy=dict(declared_pairs=152, array_bytes=1216, copied_bytes=152,
                                   target_bytes=8, saved_count_bytes=1), functions={}, repairs={},
                 support_prompt=dict(address='27389c4',bytes=support_prompt.hex()),
                 scope='Native attacker tracking/resolution and test wrapper, attack self-pump frontend, Support, Battle Cry, Renown and Retrace. Preserve integer counter slots, packed attacker bytes and original selection/cleanup behavior.',
                 integration_pending='Earlier aggregate/manual callers, native linkage, lifecycle/dispatch integration and gameplay verification remain pending.')
    targets = {int(f['address'],16): f['name'] for f in manifest['accepted_functions']}
    targets.update({int(a,16): n for a,n in names.items()})
    overlap = [line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines()
               if line.split('\t')[2]=='global']
    entries, bodies = [], {}
    for address, name in names.items():
        raw = (cache/'decompiled-attacker-mechanics'/(address+'.c')).read_text()
        original = strip_comments(raw)
        text = reconstruct(address, original)
        changes = [] if text == original else [dict(original=original, replacement=text, occurrences=1,
            kind='native attacker integer storage/Support copy reconstruction', machine_sites=[],
            machine_range=[address,meta[address]['high_pc'][2:]], machine_bytes=len(native[address]),
            machine_sha256=sha(native[address]))]
        proof['enum_values'].update({n:enums[n] for n in set(re.findall(r'\b(?:EVENT|DAT|AL|SF|COUNTER|SFG)_\w+\b',text)) if n in enums})
        assert enums['SF_0']==0
        text, _ = rename(text, {**mapping, 'SF_0':'0'})
        text, literals = recover_strings(text, runtime, {})
        body = clean(text,overlap,use_test_api_2022=True,use_target_api_2022=True,
            use_sacrifice_api_2022=True,use_spell_api_2022=True,use_extension_flags_2022=True,
            use_keyword_accessors_2022=True,use_ability_api_2022=True,use_pump_api_2022=True,
            use_activated_api_2022=True)
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x',body), address
        body = format_c(body)
        calls = []
        for i in instructions[address]:
            if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm in targets:
                destination = targets[i.operands[0].imm]
                assert re.search(r'\b'+destination+r'\s*\(',body), (address,destination)
                calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),
                                  target=f'{i.operands[0].imm:x}',name=destination))
        public = any(s==2 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off==int(address,16)
                     for n,off,sec,k,s in debug.functions())
        assert public, address
        bodies[address] = body
        detail = dict(machine_bytes=len(native[address]),machine_sha256=sha(native[address]),
                      changes=changes,canonical_calls=calls)
        proof['functions'][address] = detail
        entries.append(dict(name=name,address=address,source=meta[address]['source'],line=meta[address]['line'],
            static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),
            recovered_literals=literals,attacker_core_evidence=detail,reconstruction=proof['scope'],
            reconstruction_body_sha256=sha(function_body(body,name).encode())))
    sources = {s: read(root/s) for s in sources_wanted}
    entry = accepted[DAT_ADDRESS]
    assert not entry.get('reconstruction_body_sha256')
    previous = copy.deepcopy(entry)
    before = function_body(sources[entry['source']],entry['name'])
    calls = {n:mapping[n] for n in ['store_attackers','resolve_declare_attackers_trigger']}
    after, count = rename(before,calls)
    assert count==2 and sources[entry['source']].count(before)==1
    sites = [dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{i.operands[0].imm:x}',
                  name=targets[i.operands[0].imm]) for i in instructions[DAT_ADDRESS]
             if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and
             i.operands[0].imm in {int(a,16) for a in ADDRESSES[:2]}]
    assert len(sites)==2
    detail = dict(machine_bytes=len(native[DAT_ADDRESS]),machine_sha256=sha(native[DAT_ADDRESS]),
                  changes=[],canonical_calls=sites)
    proof['repairs'][DAT_ADDRESS] = dict(previous_record=previous,previous_body_sha256=sha(before.encode()),
        body_sha256=sha(after.encode()),calls=calls,evidence=detail)
    entry['attacker_core_evidence']=detail
    entry['reconstruction']='Native attacker storage/resolution binding; earlier source body otherwise retained.'
    entry['reconstruction_body_sha256']=sha(after.encode())
    sources[entry['source']]=sources[entry['source']].replace(before,after)
    for source in sources:
        sources[source]+='\n\n#ifndef SHANDALAR\n/* Recovered October 2022 attacker/keyword helpers. */\n'
        for entry in entries:
            if entry['source']==source:
                sources[source]+=f"\n/* ManalinkEh 2022: 0x{entry['address']}; original {source}:{entry['line']}. */\n"+bodies[entry['address']]+'\n'
        sources[source]+='#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+'/* October 2022 attacker and keyword APIs. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries)+'\n#endif\n\n'+header[at:]
    scratch=cache/'attacker-core-check';scratch.mkdir(exist_ok=True)
    (scratch/'manalink.h').write_bytes(header.encode('latin1'))
    for source,content in sources.items():
        target=scratch/Path(source).name;target.write_bytes(content.encode('latin1'))
        result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc',
            '-std=gnu99','-fsyntax-only','-ferror-limit=0','-Werror=implicit-function-declaration',
            '-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized',
            '-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(target)],capture_output=True,text=True)
        (scratch/(Path(source).stem+'.log')).write_text(result.stderr)
        if result.returncode:print(result.stderr)
        result.check_returncode()
    export_path=cache/'decompiled-abi/export.tsv'
    rows=list(csv.DictReader(export_path.open(),delimiter='\t'));existing={r['address'] for r in rows}
    focused={r['address']:r for r in csv.DictReader((cache/'decompiled-attacker-mechanics/export.tsv').open(),delimiter='\t')}
    rows=[focused[r['address']] if r['address'] in names else r for r in rows]
    rows.extend(focused[a] for a in names if a not in existing)
    if args.apply:
        citations=manifest['source_fragments']+[s for f in manifest['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not any(c['address'] in names for c in citations)
        for source,content in sources.items():(root/source).write_bytes(content.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        manifest['accepted_functions']+=entries;manifest['accepted']+=len(entries);manifest['helper_functions']+=len(entries);manifest['shared_helper_interfaces']+=len(entries)
        for entry in entries:manifest['source_files'][entry['source']]=manifest['source_files'].get(entry['source'],0)+1
        manifest['raw_export_entries']=len(rows);manifest['exported_functions']=len({r['name'] for r in rows})
        mp.write_text(json.dumps(manifest,indent=2));interfaces['attacker_core_2022']=proof;ip.write_text(json.dumps(interfaces,indent=2))
        with export_path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
        for a in names:shutil.copyfile(cache/'decompiled-attacker-mechanics'/(a+'.c'),export_path.parent/(a+'.c'))
    (cache/'attacker-core-plan.json').write_text(json.dumps(proof,indent=2))
    print(json.dumps(dict(applied=args.apply,new_helpers=len(entries),repaired_native_dispatchers=1,
                         support_copy_bytes=152,tracked_attackers=64)))


if __name__=='__main__':
    main()
