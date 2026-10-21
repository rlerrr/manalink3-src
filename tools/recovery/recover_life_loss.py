"""Recover native life-loss dispatch and connect the recovered gain/loss cycle."""
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
from elftools.dwarf.descriptions import describe_DWARF_expr
from elftools.dwarf.locationlists import LocationParser, LocationExpr, BaseAddressEntry
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, strip_comments, UNRESOLVED
from recover_test_consumers import read, rename, format_c
from recover_literals import recover_strings
from audit_source import function_body

ADDRESSES = ['268f1b0']
PRIVATE = set()
CONTROL_SITES = '268f1ef 268f203 268f22b 268f234 268f236 268f23b 268f241 268f3e5 268f404 268f4d0 268f56f 268f59a 268f5c1 268f611 268f63a 268f656'.split()


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
    assert 'life_loss_2022' not in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in manifest['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    names={a:meta[a]['name']+'_2022' for a in ADDRESSES}
    mapping={meta[a]['name']:n for a,n in names.items()}
    mapping.update(gain_life_impl='gain_life_impl_2022',lichs_mastery_lifeloss_trigger='lichs_mastery_lifeloss_trigger_2022')
    if 'oath_life_loss_2022' in interfaces:mapping['oath_of_lim_dul_lifeloss_trigger']='oath_of_lim_dul_lifeloss_trigger_2022'
    directories={a:cache/'decompiled-life-core' for a in ADDRESSES}
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
                if c.tag=='DW_TAG_variable':
                    typ=type_of(c);item=dict(name=inherited_value(c,'DW_AT_name'),type=type_name(typ),location=location(c))
                    if item['name']=='marked':item['bounds']=[value(p,'DW_AT_upper_bound') for p in typ.iter_children() if p.tag=='DW_TAG_subrange_type']
                    out.append(item)
                elif c.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:out.extend(variables(c))
            return out
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':enums.update({value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in die.iter_children() if c.tag=='DW_TAG_enumerator'})
            pc=value(die,'DW_AT_low_pc');address=f'{pc:x}' if pc else ''
            if address not in ADDRESSES:continue
            parameters[address]=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p)) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter']
            locals_[address]=variables(die)
    assert set(parameters)==set(ADDRESSES) and len(parameters['268f1b0'])==2
    assert [p['name'] for p in parameters['268f1b0']]==['player','amount']
    marked=next(v for v in locals_['268f1b0'] if v['name']=='marked')
    assert (marked['type'],marked['location'],marked['bounds'])==('int[2][151]','(DW_OP_fbreg: -1228)',[1,150])
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    natives={a:runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)) for a in ADDRESSES}
    instructions={a:list(decoder.disasm(natives[a],int(a,16))) for a in ADDRESSES}
    by_site={f'{i.address:x}':i for items in instructions.values() for i in items}
    controls=[dict(address=a,bytes=by_site[a].bytes.hex(),instruction=by_site[a].mnemonic+' '+by_site[a].op_str) for a in CONTROL_SITES]
    proof=dict(debug_sha256=sha(debug_data),parameters=parameters,locals=locals_,enum_values={},control_sites=controls,functions={},
        marked_layout=dict(rows=2,columns=151,element_bytes=4,bytes=1208,zero_words=302,frame_offset=-1228),
        routes=dict(gain_worker='268f730',mastery_callback='22d0100',oath_callback='24040a0',sacrifice='26e5080'),
        scope='Full native life-loss engine with 2-by-151 saved callback IDs, all-player recursion, life-total permission, humiliation checks, trap/sound updates and native self/opponent callback order. Replace only the proven 302-word zeroing loop, bind native gain and Mastery callbacks, and connect both loss calls in the earlier recovered gain worker.',
        integration_pending='Earlier manually reconstructed consumers, original Oath callback comparison, broader lifecycle/dispatch integration, native linking and gameplay verification remain pending.')
    targets={int(f['address'],16):f['name'] for f in manifest['accepted_functions']}
    targets.update({int(a,16):n for a,n in names.items()})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    bodies,entries={},[]
    for address,name in names.items():
        raw=(directories[address]/(address+'.c')).read_text();text=strip_comments(raw)
        proof['enum_values'].update({n:enums[n] for n in set(re.findall(r'\b(?:EVENT|AL|COUNTER|RESOLVE_TRIGGER|WAV)_\w+\b',text)) if n in enums})
        changes=[]
        clearing=[next(c for c in controls if c['address']==a) for a in ['268f234','268f236','268f23b','268f241']]
        zeroing=re.search(r'  paiVar8 = marked;\n  for \(iVar3 = 0x12e;.*?\n  }\n',text,re.S)
        assert zeroing
        replacements=[(zeroing[0],'  memset(marked, 0, sizeof(marked));\n'),('  int (*paiVar8) [151];\n',''),('  byte bVar9;\n',''),('  bVar9 = 0;\n','')]
        for original,replacement in replacements:
            assert text.count(original)==1
            changes.append(dict(original=original,replacement=replacement,occurrences=1,kind='native 302-word matrix zeroing',machine_sites=clearing))
            text=text.replace(original,replacement)
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
            life_loss_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    sources={s:read(root/s) for s in {e['source'] for e in entries}}
    for source in sources:
        sources[source]+='\n\n#ifndef SHANDALAR\n/* Recovered October 2022 native life-loss engine. */\n'
        for e in entries:
            if e['source']==source:sources[source]+=f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n"+bodies[e['address']]+'\n'
        sources[source]+='#endif\n'
    gain=next(f for f in manifest['accepted_functions'] if f['address']=='268f730')
    before=function_body(sources['functions/functions.c'],gain['name']);after,count=rename(before,{'lose_life':'lose_life_2022'})
    assert count==2 and sources['functions/functions.c'].count(before)==1
    gain_native=runtime.pe.get_data(0x268f730-0x2000000,0x268f900-0x268f730)
    loss_calls=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target='268f1b0',name='lose_life_2022') for i in decoder.disasm(gain_native,0x268f730) if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==0x268f1b0]
    assert [c['address'] for c in loss_calls]==['268f809','268f8a8']
    gain_binding=dict(address=gain['address'],source=gain['source'],name=gain['name'],previous_record=copy.deepcopy(gain),
        previous_body_sha256=sha(before.encode()),body_sha256=sha(after.encode()),calls={'lose_life':'lose_life_2022'},
        machine_bytes=len(gain_native),machine_sha256=sha(gain_native),canonical_calls=loss_calls)
    proof['gain_binding']=gain_binding
    sources['functions/functions.c']=sources['functions/functions.c'].replace(before,after)
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+'/* October 2022 native life-loss API. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if not e['static'])+'\n#endif\n\n'+header[at:]
    scratch=cache/'life-loss-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'))
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
        updated=copy.deepcopy(gain['life_core_evidence']);updated['canonical_calls']=sorted(updated['canonical_calls']+loss_calls,key=lambda c:int(c['address'],16))
        gain['life_core_evidence']=updated;interfaces['life_core_2022']['functions']['268f730']=updated
        gain['life_loss_gain_binding_2022']={k:v for k,v in gain_binding.items() if k!='previous_record'}
        gain['reconstruction_body_sha256']=gain_binding['body_sha256']
        manifest['accepted_functions']+=entries;manifest['accepted']+=1;manifest['helper_functions']+=1
        manifest['shared_helper_interfaces']+=1
        for e in entries:manifest['source_files'][e['source']]=manifest['source_files'].get(e['source'],0)+1
        manifest['raw_export_entries']=len(rows);manifest['exported_functions']=len({r['name'] for r in rows})
        mp.write_text(json.dumps(manifest,indent=2));interfaces['life_loss_2022']=proof;ip.write_text(json.dumps(interfaces,indent=2))
        with export_path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
        for a in names:shutil.copyfile(directories[a]/(a+'.c'),export_path.parent/(a+'.c'))
    (cache/'life-loss-plan.json').write_text(json.dumps(proof,indent=2))
    print(json.dumps(dict(applied=args.apply,new_helpers=1,private_helpers=0,public_helpers=1,coordinated_gain_worker_bindings=2)))


if __name__=='__main__':main()
