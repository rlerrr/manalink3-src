"""Recover the native 2022 sacrifice family with its separate flags and typed storage."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from elftools.dwarf.descriptions import describe_DWARF_expr
from elftools.dwarf.locationlists import LocationParser,LocationExpr,BaseAddressEntry
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import clean,strip_comments,UNRESOLVED
from recover_test_consumers import format_c,read,rename
from audit_source import function_body
from recover_literals import recover_strings
from sacrifice_core_bodies import reconstruct,STORAGE


def sha(data):return hashlib.sha256(data).hexdigest()


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';directory=cache/'decompiled-sacrifice-core';source='functions/sacrifices.c'
    metadata={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc'] and r['source']==source}
    addresses=sorted(set(metadata)-{'26e36e0'},key=lambda x:int(x,16));assert len(addresses)==36
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);dwarf=debug.dwarf()
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());locations=LocationParser(dwarf.location_lists())
    cu=next(cu for cu in dwarf.iter_CUs() if value(cu.get_top_DIE(),'DW_AT_name')==source)
    def location(die):
        attr=die.attributes.get('DW_AT_location')
        if not attr:return None
        result=locations.parse_from_attribute(attr,cu['version'],die=die)
        if isinstance(result,LocationExpr):return describe_DWARF_expr(result.loc_expr,dwarf.structs)
        return [dict(start=hex(x.begin_offset),end=hex(x.end_offset),expression=describe_DWARF_expr(x.loc_expr,dwarf.structs))
                if not isinstance(x,BaseAddressEntry) else dict(base=hex(x.base_address)) for x in result]
    def variables(die):
        result=[]
        for child in die.iter_children():
            if child.tag=='DW_TAG_variable':result.append(dict(name=inherited_value(child,'DW_AT_name'),type=type_name(type_of(child)),location=location(child)))
            elif child.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:result.extend(variables(child))
        return result
    parameters={};locals_by_address={};sacrifice_enum=None;token_layout=None;global_proof=None;readonly_callbacks={}
    for die in cu.get_top_DIE().iter_children():
        name=inherited_value(die,'DW_AT_name')
        if die.tag=='DW_TAG_typedef' and name in {'sacrifice_t','token_generation_t'}:
            typ=type_of(die)
            if name=='sacrifice_t':
                sacrifice_enum=dict(bytes=value(typ,'DW_AT_byte_size'),members={value(x,'DW_AT_name'):value(x,'DW_AT_const_value') for x in typ.iter_children() if x.tag=='DW_TAG_enumerator'})
            else:token_layout=dict(bytes=value(typ,'DW_AT_byte_size'),members={value(x,'DW_AT_name'):value(x,'DW_AT_data_member_location') for x in typ.iter_children() if x.tag=='DW_TAG_member'})
        if die.tag=='DW_TAG_variable' and name=='sacrifice_test':
            typ=type_of(die);assert typ.tag=='DW_TAG_pointer_type' and type_name(type_of(typ))=='test_definition_t'
            assert location(die)=='(DW_OP_addr: 279529c)' and not value(die,'DW_AT_external')
            global_proof=dict(original_name=name,name='sacrifice_test_2022',type='test_definition_2022_t *',address='279529c',pointer_bytes=4,
                declaration='static test_definition_2022_t *sacrifice_test_2022;',location=location(die))
            symbols=[dict(name=n,address=f'{debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off:x}',storage=s)
                     for n,off,sec,k,s in debug.symbols if sec>0 and n=='_sacrifice_test']
            assert symbols==[dict(name='_sacrifice_test',address='279529c',storage=3)]
            global_proof['symbols']=symbols
        at=value(die,'DW_AT_low_pc');address=f'{at:x}' if at else ''
        if address not in addresses:continue
        ps=[]
        for p in die.iter_children():
            if p.tag=='DW_TAG_formal_parameter':ps.append(dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p),constant=value(p,'DW_AT_const_value')))
        parameters[address]=ps;locals_by_address[address]=variables(die)
        if address in {'26e38d0','26e3940'}:
            typ=type_of(die);assert typ.tag=='DW_TAG_pointer_type' and type_of(typ).tag=='DW_TAG_const_type'
            readonly_callbacks[address]=dict(return_type='const char *',parameters=len(ps))
    assert set(parameters)==set(addresses) and global_proof
    assert sacrifice_enum==dict(bytes=4,members=dict(SAC_NO_CANCEL=1,SAC_DONE=2,SAC_JUST_MARK=4,SAC_ALL_OR_NONE=8,SAC_AS_COST=16,SAC_CAUSED=32,SAC_RETURN_CHOICE=64,SAC_OPTIONAL=128,SAC_SET_TARGETS=256))
    assert token_layout['bytes']==76 and {n:token_layout['members'][n] for n in ['t_player','qty','action']}==dict(t_player=20,qty=43,action=48)
    # The private cause checker has one physical parameter; the source player
    # is retained only as a GNU parameter-reference expression. The prompt
    # helper's source maxlen is an abstract constant with no physical storage.
    cause=parameters['26e3990'];prompt=parameters['26e3a30']
    assert cause[0]['name']=='t_player' and cause[0]['location'][0]['expression']=='(DW_OP_reg0 (r0))'
    assert cause[1]['name']=='player' and all('DW_OP_GNU_parameter_ref' in x['expression'] for x in cause[1]['location'])
    assert [p['name'] for p in prompt]==['prompt','type','invert','maxlen'] and prompt[-1]['constant']==200 and prompt[-1]['location'] is None
    assert [p['location'][0]['expression'] for p in prompt[:3]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))']
    known_locals={(x['name'],x['type'],x['location']) for x in locals_by_address['26e4110'] if isinstance(x['location'],str)}
    assert {('rval','int','(DW_OP_fbreg: -532)'),('tgt','target_t','(DW_OP_fbreg: -532)'),('token','token_generation_t','(DW_OP_fbreg: -532)'),
            ('td','target_definition_t','(DW_OP_fbreg: -456)'),('marked','char[151]','(DW_OP_fbreg: -371)'),('default_prompt','char[200]','(DW_OP_fbreg: -220)')}<=known_locals
    assert {('prompt','char[200]','(DW_OP_fbreg: -236)'),('test','test_definition_t','(DW_OP_fbreg: -236)')}<={(x['name'],x['type'],x['location']) for x in locals_by_address['26e4c50'] if isinstance(x['location'],str)}
    path=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(path.read_text());accepted={f['address']:f for f in manifest['accepted_functions']}
    assert not set(addresses)&set(accepted) and accepted['26e36e0']['name']=='types_cannot_sac'
    private={a for a in addresses if any(s==3 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off==int(a,16) for n,off,sec,k,s in debug.functions())}
    assert private=={'26e37b0','26e38d0','26e3940','26e3990','26e3a30','26e4110'}
    names={a:metadata[a]['name']+'_2022' for a in addresses}
    names.update({'26e3990':'can_cause_sacrifice_part_2022','26e3a30':'get_sacrifice_prompt_text_part_2022'})
    mapping={metadata[a]['name']:names[a] for a in addresses if a not in {'26e3990','26e3a30'}}
    mapping.update(sacrifice_t='sacrifice_2022_t',sacrifice_test='sacrifice_test_2022')
    mapping.update({n:n.replace('SAC_','SAC2022_') for n in sacrifice_enum['members']})
    targets={int(a,16):n for a,n in names.items()}
    targets.update({int(f['address'],16):f['name'] for f in accepted.values() if f['name'].endswith('_2022') or f['address']=='26e36e0'})
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    proof=dict(debug_sha256=sha(debug_data),enum=sacrifice_enum,retained_set_targets_value=128,token_layout=token_layout,
        parameters=parameters,locals=locals_by_address,readonly_callbacks=readonly_callbacks,test_global=global_proof,
        storage_declarations=STORAGE,functions={},scope='Full native 2022 sacrifice family uses the 216-byte test, 84-byte target and 76-byte token layouts. Native SAC_OPTIONAL=128 and SAC_SET_TARGETS=256 have a separate enum. Two private physical signatures are corrected from DWARF before export. Shared selection/target/token and prompt/test stack objects use explicit typed unions; marked clear/output and packed return preserve exact native extents and byte placement.',
        integration_pending='Generic-spell/Aura and existing card callers still need migration; original APIs and legacy flag values are preserved. No native DLL build or game execution performed.')
    entries=[];bodies={}
    for address in addresses:
        raw=(directory/(address+'.c')).read_text();text,changes=reconstruct(strip_comments(raw),address)
        if address in readonly_callbacks:
            old='char * '+metadata[address]['name'];assert text.count(old)==1
            changes.append(dict(original=old,replacement='const '+old,occurrences=1,kind='DWARF readonly callback result'))
            text=text.replace(old,'const '+old)
        text,literals=recover_strings(text,runtime,{})
        text,_=rename(text,mapping)
        body=clean(text,use_test_api_2022=True,use_target_api_2022=True,use_keyword_accessors_2022=True,
                   use_extension_flags_2022=True,use_token_api_2022=True)
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x',body),address
        if address in private:body='static '+body
        body=format_c(body);bodies[address]=body
        code=runtime.pe.get_data(int(address,16)-0x2000000,int(metadata[address]['high_pc'],16)-int(address,16));instructions=list(decoder.disasm(code,int(address,16)))
        for change in changes:
            change['machine_sites']=[]
            change['machine_ranges']=[]
            for start,end in change.pop('ranges',[]):
                data=runtime.pe.get_data(int(start,16)-0x2000000,int(end,16)-int(start,16))
                change['machine_ranges'].append(dict(start=start,end=end,bytes=len(data),sha256=sha(data)))
        calls=[]
        for i in instructions:
            if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm in targets:
                destination=targets[i.operands[0].imm]
                assert re.search(r'\b'+destination+r'\s*\(',body),(address,destination)
                calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{i.operands[0].imm:x}',name=destination))
        operands=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),instruction=i.mnemonic+' '+i.op_str) for i in instructions
                  if any(op.type==X86_OP_MEM and ((op.mem.disp==0x279529c) or (address=='26e4110' and op.mem.disp in [-0x1f8,-0x1e1,-0x1dc,-0x20a,-0x209])) for op in i.operands)]
        detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=calls,typed_storage_sites=operands)
        proof['functions'][address]=detail
        entries.append(dict(name=names[address],address=address,source=source,line=metadata[address]['line'],static=address in private,
            prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,
            sacrifice_core_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,names[address]).encode())))
    defs=read(root/'defs.h');assert 'sacrifice_2022_t;' not in defs
    enum_block='/* October 2022 sacrifice flags; retain the original flag domain above. */\n#ifndef SHANDALAR\ntypedef enum\n{\n'+''.join('  '+name.replace('SAC_','SAC2022_')+' = '+str(number)+',\n' for name,number in sacrifice_enum['members'].items())+'} sacrifice_2022_t;\n#endif'
    assert defs.count('} sacrifice_t;')==1;defs=defs.replace('} sacrifice_t;','} sacrifice_t;\n\n'+enum_block)
    text=read(root/source)+'\n\n#ifndef SHANDALAR\n/* Recovered October 2022 sacrifice family. */\n'+STORAGE+'\n'+global_proof['declaration']+'\n'
    for entry in entries:text+=f"\n/* ManalinkEh 2022: 0x{entry['address']}; original {source}:{entry['line']}. */\n"+bodies[entry['address']]+'\n'
    text+='#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    block='/* October 2022 sacrifice family; native flag and aggregate layouts. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if not e['static'])+'\n#endif\n\n'
    header=header[:at]+block+header[at:]
    scratch=cache/'sacrifice-core-check';scratch.mkdir(exist_ok=True)
    for name,content in [('defs.h',defs),('manalink.h',header),('sacrifices.c',text)]: (scratch/name).write_bytes(content.encode('latin1'))
    result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0',
        '-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format',
        '-I',str(scratch),'-I',str(root),str(scratch/'sacrifices.c')],capture_output=True,text=True)
    (scratch/'check.log').write_text(result.stderr)
    if result.returncode:print(result.stderr)
    result.check_returncode()
    export_path=cache/'decompiled-abi/export.tsv';rows=list(csv.DictReader(export_path.open(),delimiter='\t'));existing={r['address'] for r in rows}
    focused={r['address']:r for r in csv.DictReader((directory/'export.tsv').open(),delimiter='\t')}
    rows=[focused[r['address']] if r['address'] in addresses else r for r in rows];rows.extend(focused[a] for a in addresses if a not in existing)
    if args.apply:
        citations=manifest['source_fragments']+[s for f in manifest['accepted_functions'] for s in f.get('supporting_exports',[])]
        for citation in citations:
            a=citation['address']
            if a not in addresses or citation.get('raw_export'):continue
            old=export_path.parent/(a+'.c')
            if sha(old.read_text().encode())==sha((directory/(a+'.c')).read_text().encode()):continue
            support=cache/'decompiled-support'/(a+'-'+citation['raw_sha256']+'.c');support.parent.mkdir(exist_ok=True)
            shutil.copyfile(old,support);citation['raw_export']=support.relative_to(cache).as_posix()
        for name,content in [('defs.h',defs),('manalink.h',header),(source,text)]: (root/name).write_bytes(content.replace('\n','\r\n').encode('latin1'))
        manifest['accepted_functions'].extend(entries);manifest['accepted']+=36;manifest['helper_functions']+=36;manifest['private_functions']+=6;manifest['private_helpers']+=6;manifest['shared_helper_interfaces']+=30;manifest['source_files'][source]+=36
        manifest['source_globals'].append(dict(source=source,name=global_proof['name'],declaration=global_proof['declaration'],target_address=global_proof['address'],zero_initialized=True,linkage='static'))
        manifest['raw_export_entries']=len(rows);manifest['exported_functions']=len({r['name'] for r in rows});path.write_text(json.dumps(manifest,indent=2))
        path=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(path.read_text());interfaces['sacrifice_core_2022']=proof;path.write_text(json.dumps(interfaces,indent=2))
        with export_path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
        for a in addresses:shutil.copyfile(directory/(a+'.c'),export_path.parent/(a+'.c'))
    print(json.dumps(dict(applied=args.apply,new_helpers=36,private_helpers=6,public_helpers=30,legacy_set_targets=128,native_optional=128,native_set_targets=256)))


if __name__=='__main__':main()
