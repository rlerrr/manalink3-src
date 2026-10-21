"""Recover the 2022 generic spell core and its native deck-display frontend."""
import argparse,csv,hashlib,json,re,shutil,subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from elftools.dwarf.locationlists import LocationParser,LocationExpr,BaseAddressEntry
from elftools.dwarf.descriptions import describe_DWARF_expr
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import clean,strip_comments,UNRESOLVED
from recover_test_consumers import read,format_c,rename
from recover_literals import readonly_string,byte_literal,recover_strings
from audit_source import function_body
from generic_spell_body import reconstruct,STORAGE

def sha(data):return hashlib.sha256(data).hexdigest()

def main():
    ap=argparse.ArgumentParser(__doc__);ap.add_argument('--apply',action='store_true');args=ap.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    addresses={'26a0250':cache/'decompiled-sacrifice-core','2650c40':cache/'decompiled-test-consumers'}
    metadata={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);dw=debug.dwarf()
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());parser=LocationParser(dw.location_lists());parameters={};locals_by_address={};enum=None;extension_flags={}
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {'functions/functions.c','functions/tutors.c'}:continue
        def location(die):
            attr=die.attributes.get('DW_AT_location')
            if not attr:return None
            loc=parser.parse_from_attribute(attr,cu['version'],die=die)
            if isinstance(loc,LocationExpr):return describe_DWARF_expr(loc.loc_expr,dw.structs)
            return [dict(start=hex(x.begin_offset),end=hex(x.end_offset),expression=describe_DWARF_expr(x.loc_expr,dw.structs)) if not isinstance(x,BaseAddressEntry) else dict(base=hex(x.base_address)) for x in loc]
        def variables(die):
            result=[]
            for child in die.iter_children():
                if child.tag=='DW_TAG_variable':result.append(dict(name=inherited_value(child,'DW_AT_name'),type=type_name(type_of(child)),location=location(child)))
                elif child.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:result.extend(variables(child))
            return result
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type' and value(cu.get_top_DIE(),'DW_AT_name')=='functions/functions.c':
                extension_flags.update({value(x,'DW_AT_name'):value(x,'DW_AT_const_value') for x in die.iter_children() if x.tag=='DW_TAG_enumerator' and value(x,'DW_AT_name') in {'SF_0','SF2_0','SF2_REDIRECT','SF_TARGETS_ALREADY_SET','SF2_FACE_DOWN_DUE_TO_MANIFEST','SF2_QUICKSILVER_DRAGON','SF2_REBOUND','SF2_SPELLSKITE'}})
            if die.tag=='DW_TAG_enumeration_type' and any(value(x,'DW_AT_name')=='GS_CAN_TARGET' for x in die.iter_children()):
                item=dict(bytes=value(die,'DW_AT_byte_size'),members={value(x,'DW_AT_name'):value(x,'DW_AT_const_value') for x in die.iter_children() if x.tag=='DW_TAG_enumerator'})
                assert enum is None or enum==item;enum=item
            pc=value(die,'DW_AT_low_pc');address=f'{pc:x}' if pc else ''
            if address not in addresses:continue
            ps=[]
            for p in die.iter_children():
                if p.tag!='DW_TAG_formal_parameter':continue
                loc=location(p);assert loc is not None and value(p,'DW_AT_const_value') is None
                typ=type_of(p);readonly=typ.tag=='DW_TAG_pointer_type' and type_of(typ).tag=='DW_TAG_const_type'
                ps.append(dict(name=inherited_value(p,'DW_AT_name'),type=type_name(typ),location=loc,readonly_pointee=readonly))
            parameters[address]=ps;locals_by_address[address]=variables(die)
    assert set(parameters)==set(addresses) and len(parameters['26a0250'])==8 and len(parameters['2650c40'])==6
    assert next(p for p in parameters['26a0250'] if p['name']=='prompt')['readonly_pointee']
    assert enum['bytes']==4 and len(enum['members'])==16 and enum['members']['GS_ONE_OR_MULTIPLE_TARGETS']==16384 and enum['members']['GS_SAC_PERMANENT_AS_COST']==32768 and enum['members']['GS_GRAVE_RECYCLER_FROM_GRAVEYARDS']==8192
    assert extension_flags==dict(SF_0=0,SF2_0=0,SF2_REDIRECT=1073741824,SF_TARGETS_ALREADY_SET=536870912,SF2_FACE_DOWN_DUE_TO_MANIFEST=256,SF2_QUICKSILVER_DRAGON=16384,SF2_REBOUND=32768,SF2_SPELLSKITE=65536)
    fixed={(x['name'],x['type'],x['location']) for x in locals_by_address['26a0250'] if isinstance(x['location'],str)}
    assert {('test','test_definition_t','(DW_OP_fbreg: -236)'),('illegal_tgt','char[200]','(DW_OP_fbreg: -236)'),('td9','target_definition_t','(DW_OP_fbreg: -236)'),('targeted_player','int[2]','(DW_OP_fbreg: -244)')}<=fixed
    path=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(path.read_text());assert not set(addresses)&{f['address'] for f in manifest['accepted_functions']}
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    def machine(start,end):return runtime.pe.get_data(int(start,16)-0x2000000,int(end,16)-int(start,16))
    def site(address):
        i=next(decoder.disasm(runtime.pe.get_data(int(address,16)-0x2000000,15),int(address,16),count=1))
        return dict(address=address,bytes=i.bytes.hex(),instruction=i.mnemonic+' '+i.op_str)
    prompt_data=readonly_string(runtime,0x2764474);assert prompt_data==b' Select the target to change (Rules Engine = you, Deadbox = opponent)'
    prompt_site=site('26a11ba');assert prompt_site['bytes']=='c744240c74447602'
    proof=dict(debug_sha256=sha(debug_data),parameters=parameters,locals=locals_by_address,flags=enum,extension_flags=extension_flags,storage_declarations=STORAGE,
        functions={},retarget_prompt=dict(address='2764474',bytes=prompt_data.hex(),site=prompt_site),
        retained_flags=dict(GS_ONE_OR_MULTIPLE_TARGETS=8192,GS_SAC_PERMANENT_AS_COST=16384),
        scope='Native generic-spell control flow with 216-byte test/84-byte target and separate 16-value flag domain. Reconstruct the Spellskite VLA, reused pointer/integer/byte words and shared test/target/error scratch. Preserve native base=1 retarget storage, base+selected cleanup end, target restoration, cost and graveyard branches. Restore the two independently verified omitted sprintf string arguments and keep live engine text. The deck display resets the recovered native expansion word.',
        integration_pending='Aura and existing card callers still need native API/flag migration. Legacy APIs remain available; no DLL build or game execution performed.')
    proof['control_sites']=[site(a) for a in ['26a0dd1','26a0dd9','26a0dff','26a0e33','26a11aa','26a0cc5','26a1450','26a0e98','26a0eb0','26a0ecb','26a0ecf']]
    names={'26a0250':'generic_spell_2022','2650c40':'show_deck_2022'}
    targets={int(f['address'],16):f['name'] for f in manifest['accepted_functions'] if f['name'].endswith('_2022')}
    targets.update({int(a,16):n for a,n in names.items()});entries=[];bodies={};texts={}
    for address,directory in addresses.items():
        row=metadata[address];raw=(directory/(address+'.c')).read_text();text=strip_comments(raw);changes=[]
        assert any(s==2 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+offset==int(address,16) for n,offset,sec,k,s in debug.functions())
        if address=='26a0250':
            text,changes=reconstruct(text,byte_literal(prompt_data))
            for old,new in [('SF2_0','0'),('SF_0','0'),('SF2_REDIRECT','SF2_REDIRECT_2022')]:
                count=text.count(old);assert count
                changes.append(dict(original=old,replacement=new,occurrences=count,kind='verified extension flag spelling'))
                text=text.replace(old,new)
            brace=text.index('{');text=text[:brace+1]+'\n  enum { SF2_REDIRECT_2022 = 0x40000000 };\n'+text[brace+1:]
        else:
            for old,new,kind in [('EXPANSION_0','0','native zero expansion value'),('int *deck','const int *deck','readonly deck view'),('char *prompt','const char *prompt','readonly prompt view'),('char *done_label','int done_label','existing engine integer-address label ABI')]:
                assert text.count(old)==1;text=text.replace(old,new);changes.append(dict(original=old,replacement=new,occurrences=1,kind=kind))
        text,literals=recover_strings(text,runtime,{})
        text,_=rename(text,dict(generic_spell='generic_spell_2022',show_deck='show_deck_2022'))
        body=clean(text,use_test_api_2022=True,use_target_api_2022=True,use_sacrifice_api_2022=True,use_keyword_accessors_2022=True,use_extension_flags_2022=True)
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x',body)
        body=format_c(body);bodies[address]=body
        code=machine(address,row['high_pc'][2:]);instructions=list(decoder.disasm(code,int(address,16)))
        for change in changes:
            change['machine_sites']=[];change['machine_ranges']=[]
            for start,end in change.pop('ranges',[]):
                data=machine(start,end);change['machine_ranges'].append(dict(start=start,end=end,bytes=len(data),sha256=sha(data)))
        calls=[]
        for i in instructions:
            if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm in targets:
                destination=targets[i.operands[0].imm];assert re.search(r'\b'+destination+r'\s*\(',body),(address,destination)
                calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{i.operands[0].imm:x}',name=destination))
        detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=calls)
        proof['functions'][address]=detail
        entry=dict(name=names[address],address=address,source=row['source'],line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,
            generic_spell_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,names[address]).encode()))
        entries.append(entry)
        text=read(root/row['source'])+'\n\n#ifndef SHANDALAR\n'
        if address=='26a0250':text+=STORAGE+'\n'
        text+=f"/* ManalinkEh 2022: 0x{address}; original {row['source']}:{row['line']}. */\n"+body+'\n#endif\n';texts[row['source']]=text
    defs=read(root/'defs.h');assert 'generic_spell_flags_2022_t;' not in defs
    enum_block='/* October 2022 generic-spell flags; original values remain above. */\n#ifndef SHANDALAR\ntypedef enum\n{\n'+''.join('  '+name.replace('GS_','GS2022_')+' = '+str(number)+',\n' for name,number in enum['members'].items())+'} generic_spell_flags_2022_t;\n#endif'
    assert defs.count('} generic_spell_flags_t;')==1;defs=defs.replace('} generic_spell_flags_t;','} generic_spell_flags_t;\n\n'+enum_block)
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+'/* October 2022 generic spell and deck display APIs. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries)+'\n#endif\n\n'+header[at:]
    scratch=cache/'generic-spell-check';scratch.mkdir(exist_ok=True)
    for name,content in [('defs.h',defs),('manalink.h',header)]: (scratch/name).write_bytes(content.encode('latin1'))
    for source,content in texts.items():
        target=scratch/Path(source).name;target.write_bytes(content.encode('latin1'))
        result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0',
            '-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format',
            '-I',str(scratch),'-I',str(root),str(target)],capture_output=True,text=True)
        (scratch/(Path(source).stem+'.log')).write_text(result.stderr)
        if result.returncode:print(result.stderr)
        result.check_returncode()
    export_path=cache/'decompiled-abi/export.tsv';rows=list(csv.DictReader(export_path.open(),delimiter='\t'));focused={a:next(r for r in csv.DictReader((d/'export.tsv').open(),delimiter='\t') if r['address']==a) for a,d in addresses.items()};existing={r['address'] for r in rows}
    rows=[focused[r['address']] if r['address'] in addresses else r for r in rows];rows.extend(focused[a] for a in addresses if a not in existing)
    if args.apply:
        citations=manifest['source_fragments']+[s for f in manifest['accepted_functions'] for s in f.get('supporting_exports',[])]
        for citation in citations:
            a=citation['address']
            if a not in addresses or citation.get('raw_export'):continue
            old=export_path.parent/(a+'.c')
            if sha(old.read_text().encode())==sha((addresses[a]/(a+'.c')).read_text().encode()):continue
            support=cache/'decompiled-support'/(a+'-'+citation['raw_sha256']+'.c');support.parent.mkdir(exist_ok=True);shutil.copyfile(old,support);citation['raw_export']=support.relative_to(cache).as_posix()
        for name,content in [('defs.h',defs),('manalink.h',header),*texts.items()]: (root/name).write_bytes(content.replace('\n','\r\n').encode('latin1'))
        manifest['accepted_functions'].extend(entries);manifest['accepted']+=2;manifest['helper_functions']+=2;manifest['shared_helper_interfaces']+=2
        for e in entries:manifest['source_files'][e['source']]+=1
        manifest['raw_export_entries']=len(rows);manifest['exported_functions']=len({r['name'] for r in rows});path.write_text(json.dumps(manifest,indent=2))
        path=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(path.read_text());interfaces['generic_spell_2022']=proof;path.write_text(json.dumps(interfaces,indent=2))
        with export_path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
        for a,d in addresses.items():shutil.copyfile(d/(a+'.c'),export_path.parent/(a+'.c'))
    print(json.dumps(dict(applied=args.apply,new_helpers=2,native_flags=16,shared_scratch_bytes=216,retarget_base=1)))

if __name__=='__main__':main()
