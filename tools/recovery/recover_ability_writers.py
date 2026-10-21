"""Recover native keyword writers, lure/flanking workers and pump/animation Auras."""
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

TWO_PARAMETER_PARTS={'26dd080':'deathtouch_part_2022','26dd0c0':'hexproof_part_2022','26dd110':'indestructible_part_2022','26dd150':'attack_if_able_part_2022','26dd1a0':'vigilance_part_2022'}
PRIVATE={**TWO_PARAMETER_PARTS,'26df8e0':'granted_flanking_part_2022','26dcca0':'lure_effect_part_2022','26dcfb0':'lure_effect_2022','26e7250':'generic_animating_aura_impl_2022'}
ADDRESSES='267a730 26dcca0 26dcfb0 26dd080 26dd0c0 26dd110 26dd150 26dd1a0 26de480 26de500 26de580 26dedc0 26def40 26df490 26df8e0 26dfa50 26dfaf0 26e7250 26e8180 26e8740 26e9370 26e93e0 26e9450'.split()
PUMP_TYPE='''/* October 2022 pump record; legacy 36-byte record remains above. */
#ifndef SHANDALAR
typedef struct pump_ability_arguments_2022_t
{
  int source_p;
  int source_c;
  int pow_mod;
  int tou_mod;
  keyword_t key_mod;
  sp_keyword_t skey_mod;
  int paue_flags;
  int can_block_additional_creatures;
  int eot_removal_method;
  subtype_t subtype;
  color_test_t color;
  int disabling_mode;
} pump_ability_2022_t;
#endif'''

def sha(data):return hashlib.sha256(data).hexdigest()

def main():
    ap=argparse.ArgumentParser(__doc__);ap.add_argument('--apply',action='store_true');args=ap.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';directory=cache/'decompiled-ability-writers'
    mp=root/'docs/recovery-evidence/recovered-functions.json';m=json.loads(mp.read_text());accepted={f['address']:f for f in m['accepted_functions']};assert not set(ADDRESSES)&set(accepted)
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    names={a:PRIVATE.get(a,meta[a]['name']+'_2022') for a in ADDRESSES}
    mapping={meta[a]['name']:n for a,n in names.items() if a not in PRIVATE}
    mapping.update({'pump_ability_t':'pump_ability_2022_t','generic_animating_aura_impl':names['26e7250'],'targeted_aura_impl':'targeted_aura_impl_2022'})
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);dw=debug.dwarf();lp=LocationParser(dw.location_lists())
    parameters={};layouts=[];enums={};locals_by_address={}
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {'functions/abilities.c','functions/auras.c','functions/functions.c'}:continue
        def location(p):
            attr=p.attributes.get('DW_AT_location')
            if attr is None:return None
            loc=lp.parse_from_attribute(attr,cu['version'],die=p)
            if isinstance(loc,LocationExpr):return describe_DWARF_expr(loc.loc_expr,dw.structs)
            return [dict(base=hex(x.base_address)) if isinstance(x,BaseAddressEntry) else dict(start=hex(x.begin_offset),end=hex(x.end_offset),expression=describe_DWARF_expr(x.loc_expr,dw.structs)) for x in loc]
        def size(t):
            while t.tag in {'DW_TAG_typedef','DW_TAG_const_type'}:t=type_of(t)
            return value(t,'DW_AT_byte_size')
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':
                for c in die.iter_children():
                    if c.tag=='DW_TAG_enumerator':enums[value(c,'DW_AT_name')]=value(c,'DW_AT_const_value')
            if die.tag=='DW_TAG_typedef' and value(die,'DW_AT_name')=='pump_ability_t':
                t=type_of(die);members=[dict(name=value(c,'DW_AT_name'),offset=value(c,'DW_AT_data_member_location'),type=type_name(type_of(c)),bytes=size(type_of(c))) for c in t.iter_children() if c.tag=='DW_TAG_member']
                layouts.append(dict(bytes=value(t,'DW_AT_byte_size'),members=members))
            pc=value(die,'DW_AT_low_pc');address=f'{pc:x}' if pc else ''
            if address not in ADDRESSES:continue
            parameters[address]=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p),constant=value(p,'DW_AT_const_value')) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter']
            locals_by_address[address]=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p)) for p in die.iter_children() if p.tag=='DW_TAG_variable']
    assert set(parameters)==set(ADDRESSES) and layouts and all(l==layouts[0] for l in layouts)
    layout=layouts[0];assert layout['bytes']==48 and [x['offset'] for x in layout['members']]==list(range(0,48,4)) and all(x['bytes']==4 for x in layout['members'])
    assert [x['name'] for x in layout['members']]==['source_p','source_c','pow_mod','tou_mod','key_mod','skey_mod','paue_flags','can_block_additional_creatures','eot_removal_method','subtype','color','disabling_mode']
    for a in TWO_PARAMETER_PARTS:
        ps=parameters[a];assert len(ps)==3 and [p['location'][0]['expression'] for p in ps[:2]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))']
        assert all('DW_OP_GNU_parameter_ref' in x['expression'] for x in ps[2]['location'])
    ps=parameters['26df8e0'];assert len(ps)==5 and [p['location'][0]['expression'] for p in ps[:3]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))']
    assert ps[3]['location'][0]['expression']=='(DW_OP_fbreg: 0)' and all('DW_OP_GNU_parameter_ref' in x['expression'] for x in ps[4]['location'])
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    def code(a):return runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16))
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions'] if f['name'].endswith('_2022') or f['name'] in {'aura','attached_gets_kw'}};targets.update({int(a,16):n for a,n in names.items()})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    proof=dict(debug_sha256=sha(debug_data),parameters=parameters,locals=locals_by_address,pump_layout=layout,pump_type_declaration=PUMP_TYPE,retained_pump_bytes=36,enum_values={},functions={},repairs={},
        scope='Native extension-backed first-word ability writers, physical compiler parts, nine-argument lure and flanking consumers, 48-byte pump constructor and attached/animation Aura consumers. Retain original aggregate APIs and implementations.',
        integration_pending='Earlier callers still need coordinated native API/record migration. Native DLL linkage and game execution remain untested.')
    entries=[];bodies={}
    for address,name in names.items():
        raw=(directory/(address+'.c')).read_text();text=strip_comments(raw);native=code(address);instructions=list(decoder.disasm(native,int(address,16)));changes=[]
        assert any(s==(3 if address in PRIVATE else 2) and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off==int(address,16) for n,off,sec,k,s in debug.functions()),address
        def replace(old,new,kind,sites):
            nonlocal text
            count=text.count(old);assert count,(address,old);text=text.replace(old,new)
            changes.append(dict(original=old,replacement=new,occurrences=count,kind=kind,machine_sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),instruction=i.mnemonic+' '+i.op_str) for i in sites]))
        if address in {'26dd080','26dd0c0'}:
            original='(&player_bits)[player * 4]' if address=='26dd080' else '*(byte *)(player * 4 + 0x4ef1a5)'
            replacement='(uint32_t)player_bits[player]' if address=='26dd080' else '((uint32_t)player_bits[player] >> 8 & 0xffU)'
            displacement=0x4ef1a4 if address=='26dd080' else 0x4ef1a5
            sites=[i for i in instructions if any(o.type==X86_OP_MEM and o.mem.disp==displacement for o in i.operands)];assert len(sites)==1
            replace(original,replacement,'verified player-bit word/byte projection',sites)
            sites=[i for i in instructions if any(o.type==X86_OP_MEM and o.mem.disp==44 for o in i.operands)];assert sites
            replace('(pcVar2->field_21).ext','pcVar2->ext','verified extension pointer at instance offset 44',sites)
        if address in {'26dcfb0','26e8180'}:
            stem='EVENT_TRIGGER' if address=='26dcfb0' else 'EVENT_POWER'
            sites=[i for i in instructions if i.mnemonic in {'jbe','ja'}];assert sites
            replace('event - '+stem+' < 2','(uint32_t)event - (uint32_t)'+stem+' < 2U','native unsigned two-event range',sites)
        used={n:enums[n] for n in set(re.findall(r'\b(?:EVENT|AL|SP_KEYWORD|KEYWORD|COLOR_TEST|KILL|SUBTYPE)_\w+\b',text)) if n in enums}
        proof['enum_values'].update(used)
        bindings=dict(mapping)
        if address=='26e8180':bindings.update(attached_gets_pt='attached_gets_pt_part_2022',attached_set_pt='attached_set_pt_part_2022')
        text,_=rename(text,bindings);text,literals=recover_strings(text,runtime,{})
        body=clean(text,overlap,use_test_api_2022=True,use_target_api_2022=True,use_sacrifice_api_2022=True,use_spell_api_2022=True,use_extension_flags_2022=True,use_keyword_accessors_2022=True)
        if address in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x',body),address
        body=format_c(body);bodies[address]=body;calls=[]
        for i in instructions:
            if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm in targets:
                destination=targets[i.operands[0].imm];assert re.search(r'\b'+destination+r'\s*\(',body),(address,destination)
                calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{i.operands[0].imm:x}',name=destination))
        detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,canonical_calls=calls)
        proof['functions'][address]=detail
        entries.append(dict(name=name,address=address,source=meta[address]['source'],line=meta[address]['line'],static=address in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,
            ability_writer_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    sources={s:read(root/s) for s in {e['source'] for e in entries}}
    worker=accepted['26e78f0'];before=function_body(sources[worker['source']],worker['name']);after,count=rename(before,{'special_abilities':names['26dfaf0']});assert count==1
    native=code('26e78f0');calls=[]
    for i in decoder.disasm(native,int('26e78f0',16)):
        if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==int('26dfaf0',16):calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target='26dfaf0',name=names['26dfaf0']))
    assert len(calls)==1
    repair=dict(name=worker['name'],source=worker['source'],previous_record=copy.deepcopy(worker),previous_body_sha256=sha(before.encode()),body_sha256=sha(after.encode()),machine_bytes=len(native),machine_sha256=sha(native),changes=[],canonical_calls=calls)
    proof['repairs']['26e78f0']=repair;worker['ability_writer_binding_2022']={k:v for k,v in repair.items() if k!='previous_record'};worker['reconstruction_body_sha256']=repair['body_sha256'];worker['reconstruction']='Native first-word ability writer callee migration; original interface evidence retained.'
    assert sources[worker['source']].count(before)==1;sources[worker['source']]=sources[worker['source']].replace(before,after)
    for source in sources:
        sources[source]+='\n\n#ifndef SHANDALAR\n/* Recovered October 2022 ability writers and Aura pump consumers. */\n'
        sources[source]+='\n'.join(e['prototype'] for e in entries if e['static'] and e['source']==source)+'\n'
        for e in entries:
            if e['source']==source:sources[source]+=f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n"+bodies[e['address']]+'\n'
        sources[source]+='#endif\n'
    defs=read(root/'defs.h');assert defs.count('} pump_ability_t;')==1;defs=defs.replace('} pump_ability_t;','} pump_ability_t;\n\n'+PUMP_TYPE)
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+'/* October 2022 ability writers and Aura pump APIs. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if not e['static'])+'\n#endif\n\n'+header[at:]
    scratch=cache/'ability-writers-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'));(scratch/'defs.h').write_bytes(defs.encode('latin1'))
    for source,content in sources.items():
        target=scratch/Path(source).name;target.write_bytes(content.encode('latin1'))
        result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(target)],capture_output=True,text=True)
        (scratch/(Path(source).stem+'.log')).write_text(result.stderr)
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
        for source,content in sources.items():(root/source).write_bytes(content.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));(root/'defs.h').write_bytes(defs.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']=list(accepted.values())+entries;m['accepted']+=len(entries);m['helper_functions']+=len(entries);m['private_functions']+=len(PRIVATE);m['private_helpers']+=len(PRIVATE);m['shared_helper_interfaces']+=len(entries)-len(PRIVATE)
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows});mp.write_text(json.dumps(m,indent=2))
        path=root/'docs/recovery-evidence/interface-recovery.json';i=json.loads(path.read_text());i['ability_writers_2022']=proof;path.write_text(json.dumps(i,indent=2))
        with export_path.open('w',newline='') as stream:
            w=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');w.writeheader();w.writerows(rows)
        for a in names:shutil.copyfile(directory/(a+'.c'),export_path.parent/(a+'.c'))
    print(json.dumps(dict(applied=args.apply,new_helpers=len(entries),private_helpers=len(PRIVATE),rebound_attached_keyword_writers=1,pump_bytes=48)))

if __name__=='__main__':main()
