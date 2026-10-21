"""Recover physical Level Up and Buddy compiler parts and their public frontends."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import struct
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

ADDRESSES = '24ac850 24ace40 258e270 258f6c0'.split()
PRIVATE = {'24ac850','258e270'}
CONTROL_SITES = '258f70c 258f711 258f6d5 258e441 258e43b 258e482 258e3bd 258e4e5 24ace97'.split()


def sha(data):
    return hashlib.sha256(data).hexdigest()


def direct_reference_sites(image,target):
    """Exhaustively find rel32 call/jump encodings in executable PE sections."""
    sites=[]
    for section in image.pe.sections:
        if not section.Characteristics&0x20000000:continue
        data=section.get_data();base=image.pe.OPTIONAL_HEADER.ImageBase+section.VirtualAddress
        for opcode in [b'\xe8',b'\xe9']:
            at=data.find(opcode)
            while at>=0:
                if at+5<=len(data) and base+at+5+struct.unpack_from('<i',data,at+1)[0]==target:
                    sites.append(f'{base+at:x}')
                at=data.find(opcode,at+1)
    return sorted(sites)


def main():
    parser=argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply',action='store_true')
    args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    mp=root/'docs/recovery-evidence/recovered-functions.json'
    ip=root/'docs/recovery-evidence/interface-recovery.json'
    manifest=json.loads(mp.read_text());interfaces=json.loads(ip.read_text())
    assert 'level_buddy_2022' not in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in manifest['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    names={a:meta[a]['name']+('_part_2022' if a in PRIVATE else '_2022') for a in ADDRESSES}
    mapping={meta[a]['name']:n for a,n in names.items() if a not in PRIVATE}
    directories={a:cache/'decompiled-level-buddy' for a in ADDRESSES}
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
            parameters[address]=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p)) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter']
            locals_[address]=variables(die)
    assert set(parameters)==set(ADDRESSES) and [len(parameters[a]) for a in ADDRESSES]==[15,21,5,5]
    physical_names=['player','card','event','l2','l2power','l2toughness','l2key','l2sp','l3','l3power','l3toughness','l3key','l3sp']
    level_parameters=parameters['24ac850']
    assert [p['name'] for p in level_parameters[:13]]==physical_names
    assert {p['name'] for p in level_parameters[13:]}=={'l1power','l1toughness'}
    assert all(all('DW_OP_GNU_parameter_ref' in x['expression'] for x in p['location']) for p in level_parameters[13:])
    for a in PRIVATE:
        for p,expr in zip(parameters[a][:3],['DW_OP_reg0 (r0)','DW_OP_reg2 (r2)','DW_OP_reg1 (r1)']):
            assert p['location'][0]['expression']=='('+expr+')'
    fixed={(v['name'],v['type'],v['location']) for v in locals_['258e270'] if isinstance(v['location'],str)}
    assert {('test','test_definition_t','(DW_OP_fbreg: -236)'),('msg2','char[100]','(DW_OP_fbreg: -236)'),('msg','char[100]','(DW_OP_fbreg: -336)')}<=fixed
    console_data=(cache/'level-buddy-console.log').read_bytes()
    console=console_data.decode('utf-16') if console_data.startswith(b'\xff\xfe') else console_data.decode('utf-8')
    assert 'RECOVERY_LEVEL_BUDDY_COMPLETE' in console
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    buddy_references=direct_reference_sites(runtime,0x258e270)
    assert buddy_references==['258f70c'],buddy_references
    natives={a:runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)) for a in ADDRESSES}
    instructions={a:list(decoder.disasm(natives[a],int(a,16))) for a in ADDRESSES}
    by_site={f'{i.address:x}':i for items in instructions.values() for i in items}
    controls=[dict(address=a,bytes=by_site[a].bytes.hex(),instruction=by_site[a].mnemonic+' '+by_site[a].op_str) for a in CONTROL_SITES]
    proof=dict(debug_sha256=sha(debug_data),parameters=parameters,locals=locals_,enum_values={},control_sites=controls,functions={},
        level_physical_parameters=physical_names,level_nonphysical_parameters=['l1power','l1toughness'],
        buddy_private_return=dict(source_type='int',modeled_type='void',result_discarded=True,caller='258f6c0',call='258f70c',zero_return='258f6d5',native_direct_references=buddy_references),
        shared_prompt=dict(test_bytes=216,message_offset=0,message_bytes=100,frame_offset=-236),
        scope='Full native Level Up effects, base-PT setter, combined upper-level keywords, activation and AI scoring; Buddy reveal/pay-cost choices and its public cost modifier. Use physical compiler-part parameters and an unobserved private Buddy result.',
        integration_pending='Earlier manually reconstructed callers, native linkage, lifecycle/dispatch integration and gameplay verification remain pending.')
    targets={int(f['address'],16):f['name'] for f in manifest['accepted_functions']}
    targets.update({int(a,16):n for a,n in names.items()})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    bodies,entries={},[]
    for address,name in names.items():
        raw=(directories[address]/(address+'.c')).read_text();original=strip_comments(raw);text=original
        changes=[]
        if address=='258e270':
            assert text.count('msg,1,5,&test,1,1,0,0,0,0')==1
            text=text.replace('msg,1,5,&test,1,1,0,0,0,0','msg,1,5,test.message,1,1,0,0,0,0')
            changes=[dict(original=original,replacement=text,occurrences=1,kind='shared test/prompt prefix',machine_sites=[],
                machine_range=[address,meta[address]['high_pc'][2:]],machine_bytes=len(natives[address]),machine_sha256=sha(natives[address]))]
        proof['enum_values'].update({n:enums[n] for n in set(re.findall(r'\b(?:EVENT|AL|COUNTER|KEYWORD|SP_KEYWORD)_\w+\b',text)) if n in enums})
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
            level_buddy_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    sources={s:read(root/s) for s in {e['source'] for e in entries}}
    for source in sources:
        sources[source]+='\n\n#ifndef SHANDALAR\n/* Recovered October 2022 Level Up/Buddy helpers. */\n'
        for e in entries:
            if e['source']==source:sources[source]+=f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n"+bodies[e['address']]+'\n'
        sources[source]+='#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+'/* October 2022 Level Up and Buddy APIs. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if not e['static'])+'\n#endif\n\n'+header[at:]
    scratch=cache/'level-buddy-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'))
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
        manifest['accepted_functions']+=entries;manifest['accepted']+=4;manifest['helper_functions']+=4
        manifest['private_functions']+=2;manifest['private_helpers']+=2;manifest['shared_helper_interfaces']+=2
        for e in entries:manifest['source_files'][e['source']]=manifest['source_files'].get(e['source'],0)+1
        manifest['raw_export_entries']=len(rows);manifest['exported_functions']=len({r['name'] for r in rows})
        mp.write_text(json.dumps(manifest,indent=2));interfaces['level_buddy_2022']=proof;ip.write_text(json.dumps(interfaces,indent=2))
        with export_path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
        for a in names:shutil.copyfile(directories[a]/(a+'.c'),export_path.parent/(a+'.c'))
    (cache/'level-buddy-plan.json').write_text(json.dumps(proof,indent=2))
    print(json.dumps(dict(applied=args.apply,new_helpers=4,private_helpers=2,public_helpers=2)))


if __name__=='__main__':main()
