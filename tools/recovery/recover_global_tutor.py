"""Recover native global tutoring, destination values and control transfer."""
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
from global_tutor_body import BODY,GAIN_CONTROL_BODY

ADDRESSES = '2663990 2651990 26524d0'.split()
PRIVATE = set()
CONTROL_SITES = '26519a1 26519b8 26519bf 26519c2 26519c7 26519cc 26519d0 2651c04 2651e5e 26520f9 265214f 265249b 2663a51 2663ab1 2663abc 2663aca'.split()


def definitions(destinations,origins):
    return ('#ifndef SHANDALAR\n/* October 2022 tutor domains; retain legacy destination values above. */\n'
        'typedef uint32_t tutor_destination_2022_t;\nenum\n{\n'+',\n'.join('  '+n.replace('TUTOR_','TUTOR2022_',1)+' = '+str(v) for n,v in destinations.items())+'\n};\n'
        'typedef uint32_t tutor_from_2022_t;\nenum\n{\n'+',\n'.join('  '+n.replace('TUTOR_FROM_','TUTOR_FROM2022_',1)+' = '+str(v) for n,v in origins.items())+'\n};\n#endif')


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
    assert 'global_tutor_core_2022' not in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in manifest['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    names={a:meta[a]['name']+'_2022' for a in ADDRESSES}
    mapping={meta[a]['name']:n for a,n in names.items()}
    directories={'2663990':cache/'decompiled-abi','2651990':cache/'decompiled-global-consumers','26524d0':cache/'decompiled-global-tutor-wrapper'}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug=Image(debug_data);dwarf=debug.dwarf();locations=LocationParser(dwarf.location_lists())
    parameters,locals_,enums={},{},{};destinations={};origins={};vla_bound=None
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
                if c.tag=='DW_TAG_variable':
                    out.append(dict(name=inherited_value(c,'DW_AT_name'),type=type_name(type_of(c)),location=location(c)))
                    if inherited_value(c,'DW_AT_name')=='tutored':
                        nonlocal vla_bound
                        bound=next(type_of(c).iter_children()).attributes['DW_AT_upper_bound']
                        vla_bound=dict(expression_bytes=bound.value,expression=describe_DWARF_expr(bound.value,dwarf.structs))
                elif c.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:out.extend(variables(c))
            return out
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':
                values={value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in die.iter_children() if c.tag=='DW_TAG_enumerator'};enums.update(values)
                if 'TUTOR_HAND' in values:destinations=values;assert value(die,'DW_AT_byte_size')==4 and value(die,'DW_AT_encoding')==7
                if 'TUTOR_FROM_DECK' in values:origins=values
            pc=value(die,'DW_AT_low_pc');address=f'{pc:x}' if pc else ''
            if address not in set(ADDRESSES)|{'2650b80'}:continue
            parameters[address]=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p)) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter']
            locals_[address]=variables(die)
    assert set(parameters)==set(ADDRESSES)|{'2650b80'} and [len(parameters[a]) for a in ADDRESSES]==[2,7,16]
    assert destinations['TUTOR_HAND']==1 and destinations['TUTOR_OPPOSITION_AGENT']==14 and origins['TUTOR_FROM_DECK_AUTO']==6
    assert vla_bound['expression_bytes']==[119,174,1,148,2,10,255,255,26,49,28]
    assert all('DW_OP_GNU_parameter_ref' in x['expression'] for x in parameters['2650b80'][1]['location'])
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    natives={a:runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)) for a in ADDRESSES}
    instructions={a:list(decoder.disasm(natives[a],int(a,16))) for a in ADDRESSES}
    by_site={f'{i.address:x}':i for items in instructions.values() for i in items}
    controls=[dict(address=a,bytes=by_site[a].bytes.hex(),instruction=by_site[a].mnemonic+' '+by_site[a].op_str) for a in CONTROL_SITES]
    proof=dict(debug_sha256=sha(debug_data),parameters=parameters,locals=locals_,enum_values={},control_sites=controls,functions={},
        destinations=destinations,origins=origins,vla_bound=vla_bound,
        test_layout=dict(bytes=216,qty_offset=174,qty_bytes=2,no_shuffle_offset=192,create_minideck_offset=194,storage_offset=212),
        native_edge_cases=dict(single_cancel_returns_minus_two=True,multi_cancel_keeps_selected=True,cage_failure_does_not_advance_count=True,hand_id_used_for_instance_marking=True,failed_control_allocation_still_runs_tail=True),
        side_effect_helper=dict(address='2650b80',physical_arguments=['player'],retained_unused_source_argument='targ_player'),
        timestamp_arrays=dict(card_address='5076f0',player_address='507ec0',elements=500,element_bytes=4),
        scope='Full native VLA-backed global tutoring and scalar frontend, separate native destination/origin values, Opposition Agent routing and native control transfer. Preserve shuffle stages, return values, ownership, original fixed arrays and source bodies.',
        integration_pending='Earlier manual/aggregate consumers, lifecycle/dispatch integration, native linkage and gameplay verification remain pending.')
    targets={int(f['address'],16):f['name'] for f in manifest['accepted_functions']}
    targets.update({int(a,16):n for a,n in names.items()})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    bodies,entries={},[]
    for address,name in names.items():
        raw=(directories[address]/(address+'.c')).read_text();text=strip_comments(raw)
        proof['enum_values'].update({n:enums[n] for n in set(re.findall(r'\b(?:EVENT|AL|COUNTER|RESOLVE_TRIGGER|SF2)_\w+\b',text)) if n in enums})
        changes=[]
        if address in {'2651990','2663990'}:
            replacement=BODY if address=='2651990' else GAIN_CONTROL_BODY
            changes.append(dict(original=text,replacement=replacement,occurrences=1,kind='native VLA call-frame/control-transfer reconstruction',machine_sites=controls))
            text=replacement
        text,_=rename(text,mapping);text,literals=recover_strings(text,runtime,{})
        body=clean(text,overlap,use_test_api_2022=True,use_target_api_2022=True,use_sacrifice_api_2022=True,
            use_extension_flags_2022=True,use_keyword_accessors_2022=True,use_ability_api_2022=True,
            use_pump_api_2022=True,use_activated_api_2022=True,use_attack_api_2022=True)
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
            global_tutor_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    sources={s:read(root/s) for s in {e['source'] for e in entries}}
    for source in sources:
        sources[source]+='\n\n#ifndef SHANDALAR\n/* Recovered October 2022 global tutoring and control transfer. */\n'
        for e in entries:
            if e['source']==source:sources[source]+=f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n"+bodies[e['address']]+'\n'
        sources[source]+='#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+'/* October 2022 global tutor APIs. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if not e['static'])+'\n#endif\n\n'+header[at:]
    added_definitions=definitions(destinations,origins);source_definitions=read(root/'defs.h');assert added_definitions not in source_definitions
    at=source_definitions.rfind('\n#endif');source_definitions=source_definitions[:at]+'\n\n'+added_definitions+'\n'+source_definitions[at:]
    scratch=cache/'global-tutor-core-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'));(scratch/'defs.h').write_bytes(source_definitions.encode('latin1'))
    for source,text in sources.items():
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
    if args.apply:
        citations=manifest['source_fragments']+[s for f in manifest['accepted_functions'] for s in f.get('supporting_exports',[])]
        for c in citations:
            if c['address'] in names:assert hashlib.sha256((directories[c['address']]/(c['address']+'.c')).read_text().encode()).hexdigest()==c['raw_sha256']
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'defs.h').write_bytes(source_definitions.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        manifest['accepted_functions']+=entries;manifest['accepted']+=3;manifest['helper_functions']+=3
        manifest['shared_helper_interfaces']+=3
        for e in entries:manifest['source_files'][e['source']]=manifest['source_files'].get(e['source'],0)+1
        manifest['raw_export_entries']=len(rows);manifest['exported_functions']=len({r['name'] for r in rows})
        mp.write_text(json.dumps(manifest,indent=2));interfaces['global_tutor_core_2022']=proof;ip.write_text(json.dumps(interfaces,indent=2))
        with export_path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
        for a in names:
            src=directories[a]/(a+'.c');dest=export_path.parent/(a+'.c')
            if src.resolve()!=dest.resolve():shutil.copyfile(src,dest)
    (cache/'global-tutor-core-plan.json').write_text(json.dumps(proof,indent=2))
    print(json.dumps(dict(applied=args.apply,new_helpers=3,private_helpers=0,public_helpers=3)))


if __name__=='__main__':main()
