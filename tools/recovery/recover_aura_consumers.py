"""Recover native Aura targeting, attachment, control and death-return helpers.

Keep the old aggregate APIs for original callers.  Only the already recovered
native Aura worker is rebound to the native attachment implementation.
"""
import argparse,csv,hashlib,json,re,shutil,subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import clean,strip_comments,UNRESOLVED
from recover_test_consumers import read,format_c,rename
from recover_literals import recover_strings
from audit_source import function_body

ADDRESSES='26e6850 26e7160 26e7730 26e8860 26e8890 26e88c0 26e8920 26e8980 26e89e0 26e8c90 26e8d90 26e8f40 26e9110 26e6500 26e94c0 26e9630 26e9710 26e9750'.split()
PRIVATE={'26e7160','26e6500'}

def sha(data):return hashlib.sha256(data).hexdigest()

def main():
    ap=argparse.ArgumentParser(__doc__);ap.add_argument('--apply',action='store_true');args=ap.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';directory=cache/'decompiled-aura-core';source='functions/auras.c'
    mp=root/'docs/recovery-evidence/recovered-functions.json';m=json.loads(mp.read_text());accepted={f['address']:f for f in m['accepted_functions']}
    assert not set(ADDRESSES)&set(accepted)
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    names={a:meta[a]['name']+'_2022' for a in ADDRESSES}
    mapping={meta[a]['name']:n for a,n in names.items()}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes())
    parameters={};enums={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!=source:continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':
                for c in die.iter_children():
                    if c.tag=='DW_TAG_enumerator':enums[value(c,'DW_AT_name')]=value(c,'DW_AT_const_value')
            at=value(die,'DW_AT_low_pc');address=f'{at:x}' if at else ''
            if address not in ADDRESSES:continue
            parameters[address]=[]
            for p in die.iter_children():
                if p.tag!='DW_TAG_formal_parameter':continue
                location=p.attributes.get('DW_AT_location');assert location and value(p,'DW_AT_const_value') is None
                parameters[address].append(dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location.value,location_form=location.form))
        break
    assert set(parameters)==set(ADDRESSES)
    assert {n:enums[n] for n in ['AL_TRIG','AL_NM_ACT','AL_ALL_ACT','AL_ALL','SP_KEYWORD2_GRANTED_PROT_MISC']}==dict(AL_TRIG=2,AL_NM_ACT=40,AL_ALL_ACT=56,AL_ALL=126,SP_KEYWORD2_GRANTED_PROT_MISC=2)
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    def code(a):return runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16))
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions'] if f['name'].endswith('_2022') or f['name']=='aura_impl'}
    targets.update({int(a,16):n for a,n in names.items()})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    proof=dict(debug_sha256=sha(debug_data),parameters=parameters,enum_values={},functions={},repairs={},
        scope='Native 84-byte target and 216-byte test Aura consumers, extension-backed flags, attachment, stealing, protection and death-return callbacks. Preserve legacy APIs. Animation and pump record migration remain separate.',
        integration_pending='Original card callers still need native interface migration. No native DLL build or game execution performed.')
    entries=[];bodies={}
    for address,name in names.items():
        raw=(directory/(address+'.c')).read_text();text=strip_comments(raw)
        assert any(s==(3 if address in PRIVATE else 2) and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off==int(address,16) for n,off,sec,k,s in debug.functions()),address
        used={n:enums[n] for n in set(re.findall(r'\b(?:EVENT|AL|SP_KEYWORD2|KILL|RESOLVE_TRIGGER|REANIMATE|SUBTYPE)_\w+\b',text)) if n in enums}
        for n,v in used.items():assert n not in proof['enum_values'] or proof['enum_values'][n]==v
        proof['enum_values'].update(used)
        bindings=dict(mapping)
        if address=='26e88c0':bindings['disabling_aura_ai']='disabling_aura_ai_part_2022'
        text,_=rename(text,bindings);text,literals=recover_strings(text,runtime,{})
        body=clean(text,overlap,use_test_api_2022=True,use_target_api_2022=True,use_sacrifice_api_2022=True,use_spell_api_2022=True,use_extension_flags_2022=True,use_keyword_accessors_2022=True)
        if address in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x',body),address
        body=format_c(body);bodies[address]=body;native=code(address);instructions=list(decoder.disasm(native,int(address,16)))
        calls=[]
        for i in instructions:
            if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm in targets:
                destination=targets[i.operands[0].imm];assert re.search(r'\b'+destination+r'\s*\(',body),(address,destination)
                calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{i.operands[0].imm:x}',name=destination))
        detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=[],canonical_calls=calls)
        if address=='26e7730':
            sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex()) for i in instructions if any(o.type==X86_OP_MEM and o.mem.disp==0x7a2d78 for o in i.operands)]
            assert len(sites)==1
            proof['trigger_condition']=dict(address='7a2d78',sites=sites,existing_linker_symbol='_trigger_condition')
        proof['functions'][address]=detail
        entries.append(dict(name=name,address=address,source=source,line=meta[address]['line'],static=address in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,
            aura_consumer_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    text=read(root/source);worker=accepted['26e68d0'];before=function_body(text,worker['name']);after,count=rename(before,{'attach_aura_to_target':names['26e6850']});assert count>0
    native=code('26e68d0');calls=[]
    for i in decoder.disasm(native,int('26e68d0',16)):
        if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==int('26e6850',16):
            calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target='26e6850',name=names['26e6850']))
    assert len(calls)==count
    repair=dict(name=worker['name'],source=source,previous_record=dict(worker),previous_body_sha256=sha(before.encode()),body_sha256=sha(after.encode()),
        replacement=names['26e6850'],machine_bytes=len(native),machine_sha256=sha(native),changes=[],canonical_calls=calls)
    proof['repairs']['26e68d0']=repair;worker['aura_attachment_binding_2022']=repair.copy();worker['aura_attachment_binding_2022'].pop('previous_record');worker['reconstruction_body_sha256']=repair['body_sha256'];worker['reconstruction']='Native Aura attachment callee migration; original target/spell interface evidence retained.'
    text=text.replace(before,after)
    text+='\n\n#ifndef SHANDALAR\n/* Recovered October 2022 Aura targeting and attachment helpers. */\n'
    text+='\n'.join(e['prototype'] for e in entries if e['static'])+'\n'
    for e in entries:text+=f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n"+bodies[e['address']]+'\n'
    text+='#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+'/* October 2022 Aura targeting and attachment APIs. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if not e['static'])+'\n#endif\n\n'+header[at:]
    scratch=cache/'aura-consumers-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'));(scratch/'auras.c').write_bytes(text.encode('latin1'))
    result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(scratch/'auras.c')],capture_output=True,text=True)
    (scratch/'auras.log').write_text(result.stderr)
    if result.returncode:print(result.stderr)
    result.check_returncode()
    export_path=cache/'decompiled-abi/export.tsv';rows=list(csv.DictReader(export_path.open(),delimiter='\t'));existing={r['address'] for r in rows};focused={r['address']:r for r in csv.DictReader((directory/'export.tsv').open(),delimiter='\t')}
    rows=[focused[r['address']] if r['address'] in names else r for r in rows];rows.extend(focused[a] for a in names if a not in existing)
    if args.apply:
        citations=m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])]
        for citation in citations:
            a=citation['address']
            if a not in names or citation.get('raw_export'):continue
            old=export_path.parent/(a+'.c')
            if sha(old.read_text().encode())==sha((directory/(a+'.c')).read_text().encode()):continue
            support=cache/'decompiled-support'/(a+'-'+citation['raw_sha256']+'.c');support.parent.mkdir(exist_ok=True);shutil.copyfile(old,support);citation['raw_export']=support.relative_to(cache).as_posix()
        (root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'));(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']=list(accepted.values())+entries;m['accepted']+=len(entries);m['helper_functions']+=len(entries);m['private_functions']+=len(PRIVATE);m['private_helpers']+=len(PRIVATE);m['shared_helper_interfaces']+=len(entries)-len(PRIVATE);m['source_files'][source]+=len(entries)
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows});mp.write_text(json.dumps(m,indent=2))
        path=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(path.read_text());interfaces['aura_consumers_2022']=proof;path.write_text(json.dumps(interfaces,indent=2))
        with export_path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
        for a in names:shutil.copyfile(directory/(a+'.c'),export_path.parent/(a+'.c'))
    print(json.dumps(dict(applied=args.apply,new_helpers=len(entries),private_helpers=len(PRIVATE),rebound_native_aura_workers=1)))

if __name__=='__main__':main()
