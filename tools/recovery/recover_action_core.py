"""Recover native action, copy and random-permanent consumer helpers."""
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
from elftools.dwarf.locationlists import LocationParser, LocationExpr, BaseAddressEntry
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, strip_comments, UNRESOLVED
from recover_test_consumers import read, rename, format_c
from recover_literals import recover_strings
from audit_source import function_body

ADDRESSES = '2666bc0 2666f10 2666fe0 26670c0 26675a0 20277e0 2686bb0 267f4a0'.split()
PRIVATE = {'2666bc0','2666f10'}
ACTION_TYPE_DECLARATION = 'typedef uint32_t actions_2022_t;'
CONTROL_SITES = '2666bcd 2666bd0 2666c2a 2710008 2666fb8 2667220 2667277 2667388 20278d1 20278d7 202794a 2686c0b 2686dde 267f4fc 20278de 2027945'.split()


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    parser=argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply',action='store_true')
    args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    mp=root/'docs/recovery-evidence/recovered-functions.json'
    ip=root/'docs/recovery-evidence/interface-recovery.json'
    manifest=json.loads(mp.read_text());interfaces=json.loads(ip.read_text())
    assert 'action_core_2022' not in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in manifest['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    meta['2666bc0']=dict(name='action_on_card_impl',source='functions/manipulate_and_damage_all.c',line='3',high_pc='0x2666f0f')
    names={a:meta[a]['name']+'_2022' for a in ADDRESSES}
    mapping={meta[a]['name']:n for a,n in names.items()};mapping['get_updated_counters_number']='counter_number_2022';mapping.update({n:n+'_2022' for n in ['receive_network_packet','send_network_packet']})
    directories={a:cache/('decompiled-action-core' if a.startswith('266') else 'decompiled-global-consumers') for a in ADDRESSES}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug=Image(debug_data);dwarf=debug.dwarf();locations=LocationParser(dwarf.location_lists())
    parameters,locals_,enums={},{},{};external={};worker_ranges=[];actions_layout=None
    for cu in dwarf.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {meta[a]['source'] for a in ADDRESSES}:continue
        def location(die):
            attr=die.attributes.get('DW_AT_location')
            if attr is None:return None
            loc=locations.parse_from_attribute(attr,cu['version'],die=die)
            if isinstance(loc,LocationExpr):return describe_DWARF_expr(loc.loc_expr,dwarf.structs)
            return [dict(base=hex(x.base_address)) if isinstance(x,BaseAddressEntry) else
                    dict(start=hex(x.begin_offset),end=hex(x.end_offset),expression=describe_DWARF_expr(x.loc_expr,dwarf.structs)) for x in loc]
        def variables(die):
            out=[]
            for c in die.iter_children():
                if c.tag=='DW_TAG_variable':out.append(dict(name=inherited_value(c,'DW_AT_name'),type=type_name(type_of(c)),location=location(c)))
                elif c.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:out.extend(variables(c))
            return out
        for die in cu.get_top_DIE().iter_children():
            if inherited_value(die,'DW_AT_name') in {'receive_network_packet','send_network_packet'}:
                external[inherited_value(die,'DW_AT_name')]=dict(return_type=type_name(type_of(die)),parameter_count=sum(p.tag=='DW_TAG_formal_parameter' for p in die.iter_children()))
            if die.tag=='DW_TAG_typedef' and value(die,'DW_AT_name')=='actions_t':
                action_type=type_of(die);actions_layout=dict(bytes=value(action_type,'DW_AT_byte_size'),encoding=value(action_type,'DW_AT_encoding'))
            if die.tag=='DW_TAG_enumeration_type':enums.update({value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in die.iter_children() if c.tag=='DW_TAG_enumerator'})
            pc=value(die,'DW_AT_low_pc');address=f'{pc:x}' if pc else ''
            if inherited_value(die,'DW_AT_name')=='action_on_card_impl' and 'DW_AT_ranges' in die.attributes:
                address='2666bc0';worker_ranges=[dict(start=f'{r.begin_offset:x}',end=f'{r.end_offset:x}') for r in dwarf.range_lists().get_range_list_at_offset(die.attributes['DW_AT_ranges'].value,cu=cu)]
            if address not in ADDRESSES:continue
            parameters[address]=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p)) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter']
            locals_[address]=variables(die)
    assert set(parameters)==set(ADDRESSES) and [len(parameters[a]) for a in ADDRESSES]==[7,5,5,5,14,2,7,5]
    for address in PRIVATE:
        for parameter,expression in zip(parameters[address][:3],['DW_OP_reg0 (r0)','DW_OP_reg2 (r2)','DW_OP_reg1 (r1)']):
            assert expression in str(parameter['location']),(parameter,expression)
    assert actions_layout==dict(bytes=4,encoding=7)
    assert worker_ranges==[dict(start='2666bc0',end='2666f0f'),dict(start='2710008',end='271000d')]
    assert external=={n:dict(return_type='void',parameter_count=0) for n in ['receive_network_packet','send_network_packet']}
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    natives={a:runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)) for a in ADDRESSES}
    cold=runtime.pe.get_data(0x710008,5);assert cold.hex()=='e8ab1effff'
    instructions={a:list(decoder.disasm(natives[a],int(a,16))) for a in ADDRESSES}
    instructions['2666bc0']+=list(decoder.disasm(cold,0x2710008))
    by_site={f'{i.address:x}':i for items in instructions.values() for i in items}
    controls=[dict(address=a,bytes=by_site[a].bytes.hex(),instruction=by_site[a].mnemonic+' '+by_site[a].op_str) for a in CONTROL_SITES]
    proof=dict(debug_sha256=sha(debug_data),parameters=parameters,locals=locals_,enum_values={},control_sites=controls,functions={},
        external_interfaces=external,dwarf_worker_ranges=worker_ranges,actions_layout=actions_layout,
        network_bindings={'receive_network_packet_2022':'4a70c0','send_network_packet_2022':'4a6bc0'},
        action_worker_ranges=[dict(start='2666bc0',end='2666f0f',bytes=len(natives['2666bc0']),sha256=sha(natives['2666bc0'])),dict(start='2710008',end='271000d',bytes=5,sha256=sha(cold))],
        test_filter=dict(bytes=216,not_me_offset=170,not_me_bytes=2),target_filter=dict(bytes=84),
        abort_import=dict(dll='msvcrt.dll',name='abort',iat='27b5484',thunk='2701eb8',thunk_bytes='ff2584547b02'),
        network_alias=dict(name='dblword_628C10',address='628c10',bytes=4,packet_tag_name='byte_628C0C',packet_tag_address='628c0c',packet_tag_bytes=1),
        scope='Full native action decoder/worker, single-card and mass actions, random permanent choice, mass shapeshift and entry-copy target wrapper. Keep physical signatures, cold abort code, native fixed arrays, network packet fields and original source ownership.',
        integration_pending='Earlier manual/aggregate callers, lifecycle/dispatch integration, native linkage and gameplay verification remain pending.')
    targets={int(f['address'],16):f['name'] for f in manifest['accepted_functions']}
    targets.update({int(a,16):n for a,n in names.items()})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    bodies,entries={},[]
    for address,name in names.items():
        raw=(directories[address]/(address+'.c')).read_text();text=strip_comments(raw)
        proof['enum_values'].update({n:enums[n] for n in set(re.findall(r'\b(?:EVENT|AL|COUNTER|RESOLVE_TRIGGER|SF2|ACT)_\w+\b',text)) if n in enums})
        changes=[]
        repairs=[]
        if address=='2666bc0':repairs=[('func_0x02701eb8()','abort()')]
        if address=='20277e0':repairs=[('dword_628C10','dblword_628C10')]
        for original,replacement in repairs:
            count=text.count(original);assert count>0
            text=text.replace(original,replacement)
            changes.append(dict(original=original,replacement=replacement,occurrences=count,kind='verified native import or existing network word alias',machine_sites=[s for s in controls if s['address'] in ({'2710008'} if address=='2666bc0' else {'20278d1','202794a'})]))
        if address=='2666bc0':
            changes[0]['machine_ranges']=[proof['action_worker_ranges'][1]]
        text,_=rename(text,mapping);text,literals=recover_strings(text,runtime,{})
        body=clean(text,overlap,use_test_api_2022=True,use_target_api_2022=True,use_sacrifice_api_2022=True,
            use_extension_flags_2022=True,use_keyword_accessors_2022=True,use_ability_api_2022=True,
            use_pump_api_2022=True,use_activated_api_2022=True,use_attack_api_2022=True,use_action_api_2022=True)
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x',body),address
        if address in PRIVATE:body='static '+body
        body=format_c(body);bodies[address]=body
        symbols=[s for n,off,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off==int(address,16)]
        assert symbols==([3] if address in PRIVATE else [2]),(address,symbols)
        calls=[]
        for i in instructions[address]:
            if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm in targets:
                destination=targets[i.operands[0].imm]
                assert re.search(r'\b'+destination+r'\s*\(',body),(address,destination)
                calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{i.operands[0].imm:x}',name=destination))
        detail=dict(machine_bytes=len(natives[address]),machine_sha256=sha(natives[address]),changes=changes,canonical_calls=calls)
        proof['functions'][address]=detail
        entries.append(dict(name=name,address=address,source=meta[address]['source'],line=meta[address]['line'],static=address in PRIVATE,
            prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,
            action_core_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    import copy
    previous=copy.deepcopy(next(f for f in manifest['accepted_functions'] if f['address']=='26d4810'))
    assert previous['static'] and previous['name']=='counter_number_2022'
    counter_source=read(root/'functions/counters.c');signature='static int counter_number_2022('
    assert counter_source.count(signature)==1
    body=function_body(counter_source,previous['name']);counter_source=counter_source.replace(signature,'int counter_number_2022(',1)
    exposure=dict(previous_record=previous,body_sha256=sha(body.encode()),native_symbol=dict(name='_get_updated_counters_number',storage=2))
    proof['counter_number_exposure']=exposure
    counter_symbols=[n for n,off,sec,k,st in debug.functions() if st==2 and sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off==0x26d4810];assert counter_symbols==['_get_updated_counters_number']
    sources={s:read(root/s) for s in {e['source'] for e in entries}}
    for source in sources:
        sources[source]+='\n\n#ifndef SHANDALAR\n/* Recovered October 2022 action and copy helpers. */\n'
        for e in entries:
            if e['source']==source:sources[source]+=f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n"+bodies[e['address']]+'\n'
        sources[source]+='#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+'/* October 2022 action and copy APIs. */\n#ifndef SHANDALAR\nint counter_number_2022(int player, int card, counter_t type, int number);\nvoid receive_network_packet_2022(void);\nvoid send_network_packet_2022(void);\n'+'\n'.join(e['prototype'] for e in entries if not e['static'])+'\n#endif\n\n'+header[at:]
    definitions=read(root/'defs.h');assert ACTION_TYPE_DECLARATION not in definitions
    at=definitions.rfind('\n#endif');definitions=definitions[:at]+'\n\n#ifndef SHANDALAR\n/* October 2022 action values have unsigned 32-bit storage. */\n'+ACTION_TYPE_DECLARATION+'\n#endif\n'+definitions[at:]
    scratch=cache/'action-core-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'));(scratch/'defs.h').write_bytes(definitions.encode('latin1'))
    for source,text in {**sources,'functions/counters.c':counter_source}.items():
        target=scratch/Path(source).name;target.write_bytes(text.encode('latin1'))
        r=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only',
            '-ferror-limit=0','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type',
            '-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(target)],capture_output=True,text=True)
        (scratch/(Path(source).stem+'.log')).write_text(r.stderr)
        if r.returncode:print(r.stderr)
        r.check_returncode()
    export_path=cache/'decompiled-abi/export.tsv';rows=list(csv.DictReader(export_path.open(),delimiter='\t'));existing={r['address'] for r in rows}
    focused={r['address']:r for directory in set(directories.values()) for r in csv.DictReader((directory/'export.tsv').open(),delimiter='\t')}
    rows=[focused[r['address']] if r['address'] in names else r for r in rows];rows.extend(focused[a] for a in names if a not in existing)
    linker=read(root/'manalink.lds');linker+='\n/* October 2022 network packet interfaces. */\n'+''.join('_'+name+' = 0x'+address.upper()+';\n' for name,address in proof['network_bindings'].items())
    if args.apply:
        citations=manifest['source_fragments']+[s for f in manifest['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not any(c['address'] in names for c in citations)
        for source,text in {**sources,'functions/counters.c':counter_source}.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'defs.h').write_bytes(definitions.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.lds').write_bytes(linker.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        exposed=next(f for f in manifest['accepted_functions'] if f['address']=='26d4810');exposed['static']=False;exposed['prototype']=exposed['prototype'].removeprefix('static ');exposed['reconstruction_body_sha256']=exposure['body_sha256'];exposed['public_counter_number_2022']=exposure['native_symbol']
        manifest['accepted_functions']+=entries;manifest['accepted']+=8;manifest['helper_functions']+=8
        manifest['private_functions']+=1;manifest['private_helpers']+=1;manifest['shared_helper_interfaces']+=7
        for e in entries:manifest['source_files'][e['source']]=manifest['source_files'].get(e['source'],0)+1
        manifest['raw_export_entries']=len(rows);manifest['exported_functions']=len({r['name'] for r in rows})
        mp.write_text(json.dumps(manifest,indent=2));interfaces['action_core_2022']=proof;ip.write_text(json.dumps(interfaces,indent=2))
        with export_path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
        for a in names:shutil.copyfile(directories[a]/(a+'.c'),export_path.parent/(a+'.c'))
    (cache/'action-core-plan.json').write_text(json.dumps(proof,indent=2))
    print(json.dumps(dict(applied=args.apply,new_helpers=8,new_private_helpers=2,new_public_helpers=6,exposed_existing_helper=1)))


if __name__=='__main__':main()
