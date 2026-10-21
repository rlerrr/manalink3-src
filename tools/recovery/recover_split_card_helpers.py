"""Recover two card-set compiler parts and repair their distinct public wrappers."""
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
from recover_literals import recover_strings
from audit_source import function_body

NAMES={'26c0860':'check_battlefield_for_special_card_2022',
       '2199560':'player_sacrifices_the_creature_with_highest_power_part_2022',
       '2569100':'ninjutsu_custom_target_part_2022','25696b0':'ninjutsu_custom_target_2022',
       '2569740':'ninjutsu_2022','21a3da0':'player_sacrifices_the_creature_with_highest_power'}

def sha(data):return hashlib.sha256(data).hexdigest()

def main():
    ap=argparse.ArgumentParser(__doc__);ap.add_argument('--apply',action='store_true');args=ap.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';directory=cache/'decompiled-compiler-parts'
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);dw=debug.dwarf();lp=LocationParser(dw.location_lists());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes())
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    mp=root/'docs/recovery-evidence/recovered-functions.json';m=json.loads(mp.read_text());accepted={f['address']:f for f in m['accepted_functions']};assert set(NAMES)&set(accepted)=={'25696b0','21a3da0'}
    parameters={};events={}
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {meta[a]['source'] for a in NAMES}:continue
        def location(die):
            attr=die.attributes.get('DW_AT_location')
            if not attr:return None
            loc=lp.parse_from_attribute(attr,cu['version'],die=die)
            if isinstance(loc,LocationExpr):return describe_DWARF_expr(loc.loc_expr,dw.structs)
            return [dict(start=hex(x.begin_offset),end=hex(x.end_offset),expression=describe_DWARF_expr(x.loc_expr,dw.structs)) if not isinstance(x,BaseAddressEntry) else dict(base=hex(x.base_address)) for x in loc]
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':
                for member in die.iter_children():
                    name=value(member,'DW_AT_name')
                    if name in {'EVENT_CAN_ACTIVATE_FROM_HAND','EVENT_ACTIVATE_FROM_HAND','EVENT_RESOLVE_ACTIVATION_FROM_HAND'}:
                        number=value(member,'DW_AT_const_value');assert name not in events or events[name]==number;events[name]=number
            pc=value(die,'DW_AT_low_pc');address=f'{pc:x}' if pc else ''
            if address not in NAMES:continue
            ps=[]
            for p in die.iter_children():
                if p.tag!='DW_TAG_formal_parameter':continue
                loc=location(p);assert loc is not None and value(p,'DW_AT_const_value') is None
                ps.append(dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=loc))
            parameters[address]=ps
    assert events==dict(EVENT_CAN_ACTIVATE_FROM_HAND=177,EVENT_ACTIVATE_FROM_HAND=178,EVENT_RESOLVE_ACTIVATION_FROM_HAND=179)
    assert {a:len(p) for a,p in parameters.items()}=={'26c0860':5,'2199560':3,'2569100':11,'25696b0':11,'2569740':10,'21a3da0':3}
    for a in ['2199560','2569100']:
        assert [p['location'][0]['expression'] for p in parameters[a][:3]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))']
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    def code(a):return runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16))
    proof=dict(debug_sha256=sha(debug_data),parameters=parameters,events=events,functions={},repairs={},previous_records={a:accepted[a] for a in ['25696b0','21a3da0']},
        scope='Native private implementations of ninjutsu and highest-power sacrifice have distinct addresses and register parameters. Recover the native battlefield test consumer, both private parts and the plain ninjutsu frontend. Migrate the native custom-target wrapper to the 84-byte API without double counting it; retain a separately labeled uncounted 80-byte compatibility body. Repair highest-power sacrifice in place using the native restriction guard and private callee.',
        integration_pending='Earlier card callers still require native interface migration. No native DLL build or game execution performed.')
    mapping={'check_battlefield_for_special_card':NAMES['26c0860'],'ninjutsu':'ninjutsu_2022'}
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions'] if f['name'].endswith('_2022')}
    targets.update({int(a,16):name for a,name in NAMES.items()})
    entries=[];bodies={};texts={meta[a]['source']:read(root/meta[a]['source']) for a in NAMES}
    for address,name in NAMES.items():
        raw=(directory/(address+'.c')).read_text();text=strip_comments(raw);changes=[];private=address in {'2199560','2569100'}
        assert any(s==(3 if private else 2) and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off==int(address,16) for n,off,sec,k,s in debug.functions())
        if address in {'25696b0','2569740'}:
            old='2 < event - EVENT_CAN_ACTIVATE_FROM_HAND';new='(uint32_t)event - (uint32_t)EVENT_CAN_ACTIVATE_FROM_HAND > 2U'
            assert text.count(old)==1;text=text.replace(old,new);changes.append(dict(original=old,replacement=new,occurrences=1,kind='native unsigned hand-event range'))
        if address=='2569100':text,count=rename(text,{'ninjutsu_custom_target':name});assert count==1
        elif address in {'25696b0','21a3da0'}:
            original=meta[address]['name'];head,tail=text.split('{',1)
            head,count=rename(head,{original:name});assert count==1
            tail,count=rename(tail,{original:NAMES['2569100' if address=='25696b0' else '2199560']});assert count==1
            text=head+'{'+tail
        elif address=='2199560':text,count=rename(text,{meta[address]['name']:name});assert count==1
        if address=='2569740':text,count=rename(text,{'ninjutsu_custom_target':NAMES['2569100']});assert count==1
        text,_=rename(text,mapping);text,literals=recover_strings(text,runtime,{})
        body=clean(text,use_test_api_2022=True,use_target_api_2022=True,use_sacrifice_api_2022=True,use_spell_api_2022=True,use_extension_flags_2022=True,use_keyword_accessors_2022=True)
        if private:body='static '+body
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x',body),address
        body=format_c(body);bodies[address]=body;native=code(address);instructions=list(decoder.disasm(native,int(address,16)))
        for change in changes:change['machine_sites']=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),instruction=i.mnemonic+' '+i.op_str) for i in instructions if i.mnemonic=='jbe']
        calls=[]
        for i in instructions:
            if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm in targets:
                destination=targets[i.operands[0].imm];assert re.search(r'\b'+destination+r'\s*\(',body),(address,destination)
                calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{i.operands[0].imm:x}',name=destination))
        detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,canonical_calls=calls)
        proof['functions'][address]=detail
        entries.append(dict(name=name,address=address,source=meta[address]['source'],line=meta[address]['line'],static=private,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,
            split_helper_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    # The scalar highest-power API keeps its public name and physical signature.
    address='21a3da0';source=meta[address]['source'];original=NAMES[address];before=function_body(texts[source],original);after=function_body(bodies[address],original)
    assert re.search(r'\b'+original+r'\s*\(',before[before.index('{'):]) and not re.search(r'\b'+original+r'\s*\(',after[after.index('{'):])
    texts[source]=texts[source].replace(before,after)
    at=texts[source].index('/* ManalinkEh 2022: 0x'+address+';');end=texts[source].index(after)+len(after)
    forward=next(e['prototype'] for e in entries if e['address']=='2199560')
    texts[source]=texts[source][:at]+'#ifndef SHANDALAR\n'+forward+'\n\n'+texts[source][at:end]+'\n#endif'+texts[source][end:]
    proof['repairs'][address]=dict(name=original,source=source,previous_body_sha256=sha(before.encode()),body_sha256=sha(after.encode()),private_address='2199560',private_name=NAMES['2199560'])
    # The old custom-target signature still accepts an 80-byte record. Reuse the
    # selector algorithm over old constructors/selection APIs without record
    # conversion or cross-layout pointers. Only the separate native entry counts.
    source=meta['25696b0']['source'];original='ninjutsu_custom_target';before=function_body(texts[source],original)
    legacy=bodies['2569100'].replace('static int '+NAMES['2569100'],'int '+original)
    legacy,_=rename(legacy,{'target_definition_2022_t':'target_definition_t','base_target_definition_2022':'base_target_definition','can_target_2022':'can_target','pick_target_2022':'pick_target'})
    brace=legacy.index('{');legacy=legacy[:brace+1]+'\n  if ((uint32_t)event - (uint32_t)EVENT_CAN_ACTIVATE_FROM_HAND > 2U) return 0;\n'+legacy[brace+1:]
    legacy=format_c(legacy);legacy_body=function_body(legacy,original);assert not re.search(r'\b'+original+r'\s*\(',legacy_body[legacy_body.index('{'):])
    texts[source]=texts[source].replace(before,legacy_body)
    marker=re.search(r'/\* ManalinkEh 2022: 0x25696b0;[^*]*\*/',texts[source]);assert marker
    texts[source]=texts[source].replace(marker[0],'/* Legacy 80-byte target compatibility for custom-target ninjutsu.\n * Native 2022 wrapper and private implementation are recovered below. */')
    proof['legacy_compatibility']=dict(source=source,name=original,previous_body_sha256=sha(before.encode()),body_sha256=sha(legacy_body.encode()),retained_target_bytes=80,native_target_bytes=84,counted_implementation=False)
    for source in texts:
        texts[source]+='\n\n#ifndef SHANDALAR\n/* Recovered October 2022 split card helpers. */\n'
        for e in entries:
            if e['source']!=source or e['address']=='21a3da0':continue
            texts[source]+=f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n"+bodies[e['address']]+'\n'
        texts[source]+='#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+'/* October 2022 battlefield tests and split ninjutsu helpers. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if not e['static'] and e['address']!='21a3da0')+'\n#endif\n\n'+header[at:]
    scratch=cache/'split-card-helpers-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'))
    for source,content in texts.items():
        target=scratch/Path(source).name;target.write_bytes(content.encode('latin1'))
        result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(target)],capture_output=True,text=True)
        (scratch/(Path(source).stem+'.log')).write_text(result.stderr)
        if result.returncode:print(result.stderr)
        result.check_returncode()
    export_path=cache/'decompiled-abi/export.tsv';rows=list(csv.DictReader(export_path.open(),delimiter='\t'));existing={r['address'] for r in rows};focused={r['address']:r for r in csv.DictReader((directory/'export.tsv').open(),delimiter='\t')}
    rows=[focused[r['address']] if r['address'] in NAMES else r for r in rows];rows.extend(focused[a] for a in NAMES if a not in existing)
    if args.apply:
        citations=m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])]
        for citation in citations:
            a=citation['address']
            if a not in NAMES or citation.get('raw_export'):continue
            old=export_path.parent/(a+'.c')
            if sha(old.read_text().encode())==sha((directory/(a+'.c')).read_text().encode()):continue
            support=cache/'decompiled-support'/(a+'-'+citation['raw_sha256']+'.c');support.parent.mkdir(exist_ok=True);shutil.copyfile(old,support);citation['raw_export']=support.relative_to(cache).as_posix()
        for source,content in texts.items():(root/source).write_bytes(content.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        new=[e for e in entries if e['address'] not in accepted]
        for e in entries:
            if e['address'] in accepted:accepted[e['address']]=e
        m['accepted_functions']=list(accepted.values())+new;m['accepted']+=4;m['helper_functions']+=4;m['private_helpers']+=2;m['private_functions']+=2;m['shared_helper_interfaces']+=2
        for e in new:m['source_files'][e['source']]+=1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows});mp.write_text(json.dumps(m,indent=2))
        path=root/'docs/recovery-evidence/interface-recovery.json';i=json.loads(path.read_text());i['split_card_helpers_2022']=proof;path.write_text(json.dumps(i,indent=2))
        with export_path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
        for a in NAMES:shutil.copyfile(directory/(a+'.c'),export_path.parent/(a+'.c'))
    print(json.dumps(dict(applied=args.apply,new_helpers=4,repaired_existing_wrappers=2,migrated_native_wrapper=1,legacy_compatibility_bodies=1)))

if __name__=='__main__':main()
