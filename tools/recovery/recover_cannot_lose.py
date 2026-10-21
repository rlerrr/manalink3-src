"""Recover the constant-specialized cannot-lose worker and two public frontends."""
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

ADDRESSES = '26a3060 26a3390 26a33c0'.split()
PRIVATE = {'26a3060'}
CONTROL_SITES = '26a3177 26a3214 26a31f3 26a310b 26a316a 26a33a4 26a33ab 26a33d4 26a33db 26a33b3 26a33e3'.split()


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
    assert 'cannot_lose_core_2022' not in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in manifest['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    names={a:meta[a]['name']+'_2022' for a in ADDRESSES}
    mapping={meta[a]['name']:n for a,n in names.items()}
    directories={a:cache/'decompiled-cannot-lose' for a in ADDRESSES}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug=Image(debug_data);dwarf=debug.dwarf();locations=LocationParser(dwarf.location_lists())
    parameters,locals_,enums={},{},{}
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
            if die.tag=='DW_TAG_enumeration_type':enums.update({value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in die.iter_children() if c.tag=='DW_TAG_enumerator'})
            pc=value(die,'DW_AT_low_pc');address=f'{pc:x}' if pc else ''
            if address not in ADDRESSES:continue
            parameters[address]=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p),constant=value(p,'DW_AT_const_value')) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter']
            locals_[address]=variables(die)
    assert set(parameters)==set(ADDRESSES) and [len(parameters[a]) for a in ADDRESSES]==[7,4,4]
    private_parameters=parameters['26a3060']
    assert [p['name'] for p in private_parameters]==['player','card','event','who_cant_lose','for_poison','for_draw_on_empty_deck','for_life']
    expected=['DW_OP_reg0 (r0)','DW_OP_reg2 (r2)','DW_OP_reg1 (r1)','DW_OP_fbreg: 0','DW_OP_fbreg: 4','DW_OP_fbreg: 8']
    for parameter,expression in zip(private_parameters,expected):
        assert expression in str(parameter['location']),(parameter,expression)
    assert private_parameters[6]['constant']==1 and private_parameters[6]['location'] is None
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    natives={a:runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)) for a in ADDRESSES}
    instructions={a:list(decoder.disasm(natives[a],int(a,16))) for a in ADDRESSES}
    by_site={f'{i.address:x}':i for items in instructions.values() for i in items}
    controls=[dict(address=a,bytes=by_site[a].bytes.hex(),instruction=by_site[a].mnemonic+' '+by_site[a].op_str) for a in CONTROL_SITES]
    proof=dict(debug_sha256=sha(debug_data),parameters=parameters,locals=locals_,enum_values={},control_sites=controls,functions={},
        specialized_constant=dict(name='for_life',value=1,has_storage=False,physical_arguments=6),
        counter_layout=interfaces['dependency_repairs_2022']['player_counters'],
        storage_offsets=dict(life_debt=156,poison_debt=160,protected_player=164,leaving_marker=168,leaving_value=17),
        public_modes={'26a3390':dict(for_poison=0,for_draw_on_empty_deck=0),'26a33c0':dict(for_poison=1,for_draw_on_empty_deck=1)},
        scope='Full native constant-specialized cannot-lose worker and general/life-only public wrappers. Recover six physical parameters, implicit life protection, poison byte reads, debt restoration on changing protected player/leaving play, draw suppression and target-slot state. Retain the original source APIs.',
        integration_pending='Earlier manually reconstructed consumers, native lose_life, broader lifecycle integration, native linking and gameplay verification remain pending.')
    targets={int(f['address'],16):f['name'] for f in manifest['accepted_functions']}
    targets.update({int(a,16):n for a,n in names.items()})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    bodies,entries={},[]
    for address,name in names.items():
        raw=(directories[address]/(address+'.c')).read_text();text=strip_comments(raw)
        proof['enum_values'].update({n:enums[n] for n in set(re.findall(r'\b(?:EVENT|AL|COUNTER|RESOLVE_TRIGGER)_\w+\b',text)) if n in enums})
        changes=[]
        if address=='26a3060':
            for original,replacement,site in [('(&player_counters)[player_00 * 4]','POISON_COUNTERS(player_00)','26a3214'),('(&player_counters)[who_cant_lose * 4]','POISON_COUNTERS(who_cant_lose)','26a3177')]:
                assert text.count(original)==1
                text=text.replace(original,replacement)
                changes.append(dict(original=original,replacement=replacement,occurrences=1,kind='verified unsigned player poison byte',machine_sites=[next(c for c in controls if c['address']==site)]))
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
            cannot_lose_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    sources={s:read(root/s) for s in {e['source'] for e in entries}}
    for source in sources:
        sources[source]+='\n\n#ifndef SHANDALAR\n/* Recovered October 2022 cannot-lose protection helpers. */\n'
        for e in entries:
            if e['source']==source:sources[source]+=f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n"+bodies[e['address']]+'\n'
        sources[source]+='#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+'/* October 2022 cannot-lose protection APIs. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if not e['static'])+'\n#endif\n\n'+header[at:]
    scratch=cache/'cannot-lose-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'))
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
        assert not any(c['address'] in names for c in citations)
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        manifest['accepted_functions']+=entries;manifest['accepted']+=3;manifest['helper_functions']+=3
        manifest['private_functions']+=1;manifest['private_helpers']+=1;manifest['shared_helper_interfaces']+=2
        for e in entries:manifest['source_files'][e['source']]=manifest['source_files'].get(e['source'],0)+1
        manifest['raw_export_entries']=len(rows);manifest['exported_functions']=len({r['name'] for r in rows})
        mp.write_text(json.dumps(manifest,indent=2));interfaces['cannot_lose_core_2022']=proof;ip.write_text(json.dumps(interfaces,indent=2))
        with export_path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
        for a in names:shutil.copyfile(directories[a]/(a+'.c'),export_path.parent/(a+'.c'))
    (cache/'cannot-lose-plan.json').write_text(json.dumps(proof,indent=2))
    print(json.dumps(dict(applied=args.apply,new_helpers=3,private_helpers=1,public_helpers=2)))


if __name__=='__main__':main()
