"""Recover native life-gain and loss-prevention workers, frontends and state."""
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

ADDRESSES = '2685c90 26927f0 268f0b0 268f730 268f900 268f9f0 2692810 268be60'.split()
PRIVATE = {'2685c90','268f730'}
CONTROL_SITES = '268f0b9 26927f1 2692801 268f74b 268f76c 268f779 268f77e 268f7b1 268f7c0 268f7a6 268f7d8 268f7e4 268f7e9 268f83c 268f845 268f90c 268f9ff 268f9f1 268f901 268f86c 268f885 268f8f0 268f7ca 269283d 2692848 268be67 268be8b 2692885 26928b9'.split()
ENUM_DECLARATION = '#ifndef SHANDALAR\n/* October 2022 life replacement trigger; retain the original event domain. */\nenum { XTRIGGER2022_REPLACE_GAIN_LIFE = 0x01000600 };\n#endif'
CONTEXT_DECLARATIONS = '#define life_gain_context_push_2022 EXE_FN(void, 0x435C80, void)\n#define life_gain_context_pop_2022 EXE_FN(void, 0x435CD0, void)'


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
    assert 'life_core_2022' not in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in manifest['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    names={a:meta[a]['name']+'_2022' for a in ADDRESSES}
    names['2685c90']='can_lose_the_game_impl_2022'
    mapping={meta[a]['name']:n for a,n in names.items()}
    directories={a:cache/('decompiled-life-frontends' if a in {'2692810','268be60'} else 'decompiled-life-core') for a in ADDRESSES}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug=Image(debug_data);dwarf=debug.dwarf();locations=LocationParser(dwarf.location_lists())
    parameters,locals_,enums={},{},{}
    global_types={}
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
            if die.tag=='DW_TAG_variable' and value(die,'DW_AT_name') in {'life_to_lose_on_gain_life','suspend_cannot_lose_the_game'}:
                name=value(die,'DW_AT_name');typ=type_of(die)
                assert type_name(typ)=='int' and value(typ,'DW_AT_byte_size')==4
                global_types[name]=dict(type='int',bytes=4,die=hex(die.offset),line=value(die,'DW_AT_decl_line'),location=value(die,'DW_AT_location'))
            if die.tag=='DW_TAG_enumeration_type':enums.update({value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in die.iter_children() if c.tag=='DW_TAG_enumerator'})
            pc=value(die,'DW_AT_low_pc');address=f'{pc:x}' if pc else ''
            if address not in ADDRESSES:continue
            parameters[address]=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p)) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter']
            locals_[address]=variables(die)
    assert set(parameters)==set(ADDRESSES) and [len(parameters[a]) for a in ADDRESSES]==[1,1,1,3,2,2,1,5]
    for a,registers in [('2685c90',['DW_OP_reg0 (r0)']),('268f730',['DW_OP_reg0 (r0)','DW_OP_reg2 (r2)','DW_OP_reg1 (r1)'])]:
        for p,expression in zip(parameters[a],registers):assert expression in str(p['location']),(a,p)
    global_records=[];global_proof={}
    for name,address in [('life_to_lose_on_gain_life','2797120'),('suspend_cannot_lose_the_game','2775460')]:
        symbol=next(s for s in debug.symbols if s[0]=='_'+name);section=debug.pe.sections[symbol[2]-1]
        assert symbol[4]==2 and section.Name.rstrip(b'\0')==b'.bss' and section.SizeOfRawData==0
        assert debug.pe.OPTIONAL_HEADER.ImageBase+section.VirtualAddress+symbol[1]==int(address,16)
        global_proof[name]=dict(address=address,bytes=4,coff_storage=2,section='.bss',zero_initialized=True,type_evidence=global_types[name])
        global_records.append(dict(source='functions/functions.c',name=name,declaration='int '+name+';',target_address=address,zero_initialized=True,linkage='external'))
    assert enums['XTRIGGER_REPLACE_GAIN_LIFE']==0x1000600 and enums['EA_LICH']==enums['PB_CANT_GAIN_LIFE']==0x200000
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    natives={a:runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)) for a in ADDRESSES}
    instructions={a:list(decoder.disasm(natives[a],int(a,16))) for a in ADDRESSES}
    by_site={f'{i.address:x}':i for items in instructions.values() for i in items}
    controls=[dict(address=a,bytes=by_site[a].bytes.hex(),instruction=by_site[a].mnemonic+' '+by_site[a].op_str) for a in CONTROL_SITES]
    proof=dict(debug_sha256=sha(debug_data),parameters=parameters,locals=locals_,enum_values={},control_sites=controls,functions={},globals=global_proof,
        global_declarations=global_records,source_state_owner='functions/functions.c; external BSS state reconstructed from COFF and header declarations',
        game_loss_frontend=dict(real_lose_address='4791f0',gain_trigger=224,gain_amount_address='73b6ac'),
        native_replacement_trigger=dict(name='XTRIGGER_REPLACE_GAIN_LIFE',value=0x1000600,source_name='XTRIGGER2022_REPLACE_GAIN_LIFE',legacy_value=0x1000800),
        event_flag=dict(address='4ef188',bytes=4,mask=0x200000),player_flag=dict(address='4ef1a4',stride=4,bytes=4,mask=0x200000),
        draw_counter=dict(name='life_gain_draw_count_2022',address='4ef538',elements=2,element_bytes=4,name_inferred=True),
        context=dict(push_address='435c80',pop_address='435cd0',declarations=CONTEXT_DECLARATIONS),
        scope='Full native life-gain replacement/context/notification worker, ordinary/lifelink frontends, gain-as-cost predicate, life-gain notification trigger, game-loss frontend and private/public loss-prevention checks. Preserve native replacement ordering, snapshot restore, both-player routing, existing interfaces and native public/private linkage.',
        integration_pending='Native lose_life, cannot_lose_the_game suppression worker, broader callback/lifecycle integration, native linking and gameplay verification remain pending.')
    targets={int(f['address'],16):f['name'] for f in manifest['accepted_functions']}
    targets.update({int(a,16):n for a,n in names.items()})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    bodies,entries={},[]
    for address,name in names.items():
        raw=(directories[address]/(address+'.c')).read_text();text=strip_comments(raw)
        proof['enum_values'].update({n:enums[n] for n in set(re.findall(r'\b(?:EVENT|AL|COUNTER|RESOLVE_TRIGGER)_\w+\b',text)) if n in enums})
        changes=[]
        def change(original,replacement,sites):
            nonlocal text
            count=text.count(original);assert count>0 and sites,(address,original)
            changes.append(dict(original=original,replacement=replacement,occurrences=count,kind='native life flag, context or executable-word reconstruction',machine_sites=sites))
            text=text.replace(original,replacement)
        by_address={c['address']:c for c in controls}
        if address in {'268f0b0','268f730'}:
            change('(*(byte *)(player * 4 + 0x4ef1a6) & 0x20)','((uint32_t)player_bits[player] & PB_CANT_GAIN_LIFE)',[by_address['268f0b9' if address=='268f0b0' else '268f74b']])
        if address=='268f730':
            change('(event_flags._2_1_ & 0x20)','((uint32_t)event_flags & EA_LICH)',[by_address[a] for a in ['268f76c','268f83c']])
            change('(*(code *)0x435c80)();','life_gain_context_push_2022();',[by_address[a] for a in ['268f779','268f77e']])
            change('(*(code *)0x435cd0)();','life_gain_context_pop_2022();',[by_address[a] for a in ['268f7e4','268f7e9']])
            change('*(int *)(player * 4 + 0x4ef538)','life_gain_draw_count_2022[player]',[by_address['268f845']])
            change('XTRIGGER_REPLACE_GAIN_LIFE','XTRIGGER2022_REPLACE_GAIN_LIFE',[by_address['268f7a6']])
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
            life_core_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    sources={s:read(root/s) for s in {e['source'] for e in entries}}
    for source in sources:
        sources[source]+='\n\n#ifndef SHANDALAR\n/* Recovered October 2022 life-gain and loss-prevention helpers. */\n'
        sources[source]+='\n'+CONTEXT_DECLARATIONS+'\n'+'\n'.join(g['declaration'] for g in global_records)+'\n'
        for e in entries:
            if e['source']==source:sources[source]+=f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n"+bodies[e['address']]+'\n'
        sources[source]+='#undef life_gain_context_push_2022\n#undef life_gain_context_pop_2022\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+'/* October 2022 life-gain and loss-prevention APIs. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if not e['static'])+'\n'+''.join('extern int '+g['name']+';\n' for g in global_records)+'extern int life_gain_draw_count_2022[2];\n#endif\n\n'+header[at:]
    definitions=read(root/'defs.h');at=definitions.rfind('\n#endif');definitions=definitions[:at]+'\n\n'+ENUM_DECLARATION+'\n'+definitions[at:]
    scratch=cache/'life-core-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'))
    (scratch/'defs.h').write_bytes(definitions.encode('latin1'))
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
        (root/'defs.h').write_bytes(definitions.replace('\n','\r\n').encode('latin1'))
        linker=read(root/'manalink.lds');assert '_life_gain_draw_count_2022' not in linker
        (root/'manalink.lds').write_bytes((linker+'\n/* October 2022 life-gain draw counter (inferred name). */\n_life_gain_draw_count_2022 = 0x4EF538;\n').replace('\n','\r\n').encode('latin1'))
        manifest['source_globals']+=global_records
        manifest['accepted_functions']+=entries;manifest['accepted']+=8;manifest['helper_functions']+=8
        manifest['private_functions']+=2;manifest['private_helpers']+=2;manifest['shared_helper_interfaces']+=6
        for e in entries:manifest['source_files'][e['source']]=manifest['source_files'].get(e['source'],0)+1
        manifest['raw_export_entries']=len(rows);manifest['exported_functions']=len({r['name'] for r in rows})
        mp.write_text(json.dumps(manifest,indent=2));interfaces['life_core_2022']=proof;ip.write_text(json.dumps(interfaces,indent=2))
        with export_path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
        for a in names:shutil.copyfile(directories[a]/(a+'.c'),export_path.parent/(a+'.c'))
    (cache/'life-core-plan.json').write_text(json.dumps(proof,indent=2))
    print(json.dumps(dict(applied=args.apply,new_helpers=8,private_helpers=2,public_helpers=6)))


if __name__=='__main__':main()
