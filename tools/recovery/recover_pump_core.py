"""Recover native pump effects, cleanup writers and their full record consumers."""
import argparse,copy,csv,hashlib,json,re,shutil,subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from elftools.dwarf.locationlists import LocationParser,LocationExpr,BaseAddressEntry
from elftools.dwarf.descriptions import describe_DWARF_expr
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import clean,strip_comments,UNRESOLVED
from recover_test_consumers import read,format_c,rename
from recover_literals import recover_strings
from audit_source import function_body
from format_recovery import definition_span
from pump_core_bodies import reconstruct

CORE='26775c0 267a790 267ab60 267ac00 267ad30 267ad70 267ae90 267af00 267af90 267b1b0 267b300 26825d0 2682750 2682890 2685dd0 2686030 26860b0 2686520 26a1550 26a1900 26a2390'.split()
REMOVAL='2681550 26dda00 26dd4d0 26dd460 26dd890'.split()
DEPENDENCIES='26e0300 21ecf20 22006a0'.split()
SUBTYPES='2672f30 2672f70'.split()
PRIVATE={'26775c0','267ad70','21ecf20'}
REPLACED={'267ac00','267b1b0'}
REPAIRS={'26e7bf0':{'add_a_subtype':'add_a_subtype_2022'},'26e7cc0':{'force_a_subtype':'force_a_subtype_2022'},'26e7250':{'add_a_subtype':'add_a_subtype_2022'}}

def sha(data):return hashlib.sha256(data).hexdigest()

def main():
    ap=argparse.ArgumentParser(__doc__);ap.add_argument('--apply',action='store_true');args=ap.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json'
    m=json.loads(mp.read_text());accepted={f['address']:f for f in m['accepted_functions']};addresses=CORE+REMOVAL+DEPENDENCIES+SUBTYPES
    assert set(addresses)&set(accepted)==REPLACED
    directories={**{a:cache/'decompiled-pump-core' for a in CORE},**{a:cache/'decompiled-pump-removal' for a in REMOVAL},**{a:cache/'decompiled-pump-dependencies' for a in DEPENDENCIES},**{a:cache/'decompiled-pump-subtypes' for a in SUBTYPES}}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    names={a:meta[a]['name'] if a in REPLACED else meta[a]['name']+'_2022' for a in addresses};names['21ecf20']='spirit_link_effect_part_2022'
    mapping={meta[a]['name']:n for a,n in names.items() if a!='21ecf20'}
    mapping.update({'pump_ability_t':'pump_ability_2022_t','default_pump_ability_definition':'default_pump_ability_definition_2022'})
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);dw=debug.dwarf();lp=LocationParser(dw.location_lists())
    parameters={};enums={};locals_by_address={}
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {meta[a]['source'] for a in addresses}:continue
        def location(p):
            attr=p.attributes.get('DW_AT_location')
            if attr is None:return None
            loc=lp.parse_from_attribute(attr,cu['version'],die=p)
            if isinstance(loc,LocationExpr):return describe_DWARF_expr(loc.loc_expr,dw.structs)
            return [dict(base=hex(x.base_address)) if isinstance(x,BaseAddressEntry) else dict(start=hex(x.begin_offset),end=hex(x.end_offset),expression=describe_DWARF_expr(x.loc_expr,dw.structs)) for x in loc]
        def locals(die):
            result=[]
            for child in die.iter_children():
                if child.tag=='DW_TAG_variable':result.append(dict(name=inherited_value(child,'DW_AT_name'),type=type_name(type_of(child)),location=location(child)))
                elif child.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:result.extend(locals(child))
            return result
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':
                for c in die.iter_children():
                    if c.tag=='DW_TAG_enumerator':enums[value(c,'DW_AT_name')]=value(c,'DW_AT_const_value')
            pc=value(die,'DW_AT_low_pc');address=f'{pc:x}' if pc else ''
            if address not in addresses:continue
            parameters[address]=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p),constant=value(p,'DW_AT_const_value')) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter']
            locals_by_address[address]=locals(die)
    assert set(parameters)==set(addresses)
    ps=parameters['21ecf20'];assert [p['name'] for p in ps]==['player','card','who_gains','event']
    assert [p['location'][0]['expression'] for p in ps[:3]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))'] and all('DW_OP_GNU_parameter_ref' in x['expression'] for x in ps[3]['location'])
    assert len(parameters['267ad70'])==5 and len(parameters['26a2390'])==3 and len(parameters['26a1550'])==8
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    def code(a):return runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16))
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']};targets.update({int(a,16):n for a,n in names.items()})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    proof=dict(debug_sha256=sha(debug_data),parameters=parameters,locals=locals_by_address,enum_values={},functions={},repairs={},replacements={},previous_records={a:copy.deepcopy(accepted[a]) for a in REPLACED},
        scope='Native full pump-effect engine: 48-byte record, 216-byte test and 84-byte target consumers, keyword/color/subtype effects, expiration/cleanup, direct subtype frontends and ability-removal writers. Replace the earlier neutral-field pump adapter with the full native implementation. Retain original aggregate interfaces and native color-slot behavior.',
        integration_pending='Earlier aggregate callers, activated-ability pump frontends, attack-trigger consumers and native DLL/lifecycle integration remain pending. No game execution performed.')
    entries=[];bodies={}
    for address,name in names.items():
        raw=(directories[address]/(address+'.c')).read_text();native=code(address);instructions=list(decoder.disasm(native,int(address,16)))
        assert any(s==(3 if address in PRIVATE else 2) and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off==int(address,16) for n,off,sec,k,s in debug.functions()),address
        text,changes=reconstruct(address,strip_comments(raw),instructions)
        proof['enum_values'].update({n:enums[n] for n in set(re.findall(r'\b(?:EVENT|AL|SP_KEYWORD|KEYWORD|COLOR_TEST|KILL|SUBTYPE)_\w+\b',text)) if n in enums})
        text,_=rename(text,mapping);text,literals=recover_strings(text,runtime,{})
        body=clean(text,overlap,use_test_api_2022=True,use_target_api_2022=True,use_sacrifice_api_2022=True,use_spell_api_2022=True,use_extension_flags_2022=True,use_keyword_accessors_2022=True,use_ability_api_2022=True)
        if address in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x',body),address
        body=format_c(body)
        if address=='26a2390':
            assert body.count('color = pcVar3->targets[8].player;')==1
            body=body.replace('color = pcVar3->targets[8].player;','/* Native 2022 code tests the color slot, then reads this subtype slot. */\n    color = pcVar3->targets[8].player;')
        bodies[address]=body;calls=[];references=[]
        for i in instructions:
            if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm in targets:
                destination=targets[i.operands[0].imm];assert re.search(r'\b'+destination+r'\s*\(',body),(address,destination)
                calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{i.operands[0].imm:x}',name=destination))
            elif i.mnemonic in {'mov','push','cmp'}:
                for operand in i.operands:
                    if operand.type==X86_OP_IMM and operand.imm in targets:
                        destination=targets[operand.imm]
                        assert re.search(r'\b'+destination+r'\b',body),(address,destination)
                        references.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{operand.imm:x}',name=destination))
        detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,canonical_calls=calls,function_references=references)
        proof['functions'][address]=detail
        entries.append(dict(name=name,address=address,source=meta[address]['source'],line=meta[address]['line'],static=address in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,
            pump_core_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    def site(a):
        i=next(decoder.disasm(runtime.pe.get_data(int(a,16)-0x2000000,15),int(a,16),count=1))
        return dict(address=a,bytes=i.bytes.hex(),instruction=i.mnemonic+' '+i.op_str)
    proof['control_sites']=[site(a) for a in ['26a1551','26a157a','26a162b','26a1635','26a24a1','26a24ab','267ae15','267ae25']]
    proof['native_color_slot']=dict(tested_offset=184,loaded_offset=180,tested_target='targets[8].card',loaded_target='targets[8].player',scope='Preserve the native color branch reading the subtype slot after testing the color slot; no gameplay fix invented.')
    proof['pump_serialization']=dict(record_bytes=48,subtype_offset=36,color_offset=40,subtype_target_offset=180,color_target_offset=184,unserialized_disabling_offset=44)
    sources={s:read(root/s) for s in {e['source'] for e in entries}|{accepted[a]['source'] for a in REPAIRS}}
    # Existing scalar native entry points keep their public names; the partial
    # neutral-field adapter is replaced by its full 48-byte implementation.
    for e in entries:
        a=e['address']
        if a not in REPLACED:continue
        source=e['source'];before=function_body(sources[source],accepted[a]['name']);marker='/* ManalinkEh 2022: 0x'+a+';';at=sources[source].index(marker)
        _,start,end=definition_span(sources[source][at:],accepted[a]['name']);old_definition=sources[source][at+start:at+end]
        proof['replacements'][a]=dict(name=e['name'],source=source,previous_body_sha256=sha(before.encode()),body_sha256=e['reconstruction_body_sha256'])
        sources[source]=sources[source][:at]+'#ifndef SHANDALAR\n'+sources[source][at:at+start]+bodies[a]+'\n#endif'+sources[source][at+end:]
    forward='\n'.join(e['prototype'] for e in entries if e['static'] and e['source']=='functions/functions.c');source='functions/functions.c';at=sources[source].index('/* ManalinkEh 2022: 0x267ac00;')
    # Insert after the wrapper's new configuration guard and before its marker.
    sources[source]=sources[source][:at]+forward+'\n\n'+sources[source][at:]
    proof['early_private_forward']=forward
    for address,mapping_call in REPAIRS.items():
        f=accepted[address];source=f['source'];before=function_body(sources[source],f['name']);after,count=rename(before,mapping_call);assert count
        native=code(address);calls=[]
        for i in decoder.disasm(native,int(address,16)):
            if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm in targets and targets[i.operands[0].imm] in mapping_call.values():
                calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{i.operands[0].imm:x}',name=targets[i.operands[0].imm]))
        assert len(calls)==count
        d=dict(name=f['name'],source=source,previous_record=copy.deepcopy(f),previous_body_sha256=sha(before.encode()),body_sha256=sha(after.encode()),machine_bytes=len(native),machine_sha256=sha(native),changes=[],canonical_calls=calls,calls=mapping_call)
        proof['repairs'][address]=d;f['pump_callee_binding_2022']={k:v for k,v in d.items() if k!='previous_record'};f['reconstruction_body_sha256']=d['body_sha256'];f['reconstruction']='Native subtype writer binding; earlier recovery evidence retained.'
        assert sources[source].count(before)==1;sources[source]=sources[source].replace(before,after)
    for source in sources:
        group=[e for e in entries if e['source']==source and e['address'] not in REPLACED]
        if not group:continue
        sources[source]+='\n\n#ifndef SHANDALAR\n/* Recovered October 2022 pump effects and cleanup helpers. */\n'+'\n'.join(e['prototype'] for e in group if e['static'])+'\n'
        for e in group:sources[source]+=f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n"+bodies[e['address']]+'\n'
        sources[source]+='#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+'/* October 2022 pump engine and cleanup APIs. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if not e['static'] and e['address'] not in REPLACED)+'\n#endif\n\n'+header[at:]
    scratch=cache/'pump-core-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'))
    for source,content in sources.items():
        target=scratch/Path(source).name;target.write_bytes(content.encode('latin1'))
        result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(target)],capture_output=True,text=True)
        (scratch/(Path(source).stem+'.log')).write_text(result.stderr)
        if result.returncode:print(result.stderr)
        result.check_returncode()
    export_path=cache/'decompiled-abi/export.tsv';rows=list(csv.DictReader(export_path.open(),delimiter='\t'));existing={r['address'] for r in rows};focused={r['address']:r for directory in set(directories.values()) for r in csv.DictReader((directory/'export.tsv').open(),delimiter='\t')}
    rows=[focused[r['address']] if r['address'] in names else r for r in rows];rows.extend(focused[a] for a in names if a not in existing)
    if args.apply:
        citations=m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])]
        for citation in citations:
            a=citation['address']
            if a not in names or citation.get('raw_export'):continue
            old=export_path.parent/(a+'.c')
            if sha(old.read_text().encode())==sha((directories[a]/(a+'.c')).read_text().encode()):continue
            support=cache/'decompiled-support'/(a+'-'+citation['raw_sha256']+'.c');support.parent.mkdir(exist_ok=True);shutil.copyfile(old,support);citation['raw_export']=support.relative_to(cache).as_posix()
        for source,content in sources.items():(root/source).write_bytes(content.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        new=[e for e in entries if e['address'] not in REPLACED]
        for e in entries:
            if e['address'] in REPLACED:accepted[e['address']]=e
        m['accepted_functions']=list(accepted.values())+new;m['accepted']+=len(new);m['helper_functions']+=len(new);m['private_functions']+=len(PRIVATE);m['private_helpers']+=len(PRIVATE);m['shared_helper_interfaces']+=len(new)-len(PRIVATE)
        for e in new:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows});mp.write_text(json.dumps(m,indent=2))
        path=root/'docs/recovery-evidence/interface-recovery.json';i=json.loads(path.read_text());i['pump_core_2022']=proof;path.write_text(json.dumps(i,indent=2))
        with export_path.open('w',newline='') as stream:
            w=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');w.writeheader();w.writerows(rows)
        for a in names:shutil.copyfile(directories[a]/(a+'.c'),export_path.parent/(a+'.c'))
    print(json.dumps(dict(applied=args.apply,new_helpers=len(entries)-len(REPLACED),private_helpers=len(PRIVATE),replaced_native_implementations=2,native_subtype_binding_repairs=len(REPAIRS))))

if __name__=='__main__':main()
