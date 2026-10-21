"""Recover specialized sacrifice/graveyard/tap activation costs and multi-GY selection."""
import argparse,csv,hashlib,json,re,shutil,struct,subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from elftools.dwarf.locationlists import LocationParser,LocationExpr,BaseAddressEntry
from elftools.dwarf.descriptions import describe_DWARF_expr
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import clean,strip_comments,UNRESOLVED
from recover_test_consumers import read,format_c,rename
from recover_literals import recover_strings,readonly_string,byte_literal
from audit_source import function_body
from activation_cost_bodies import GRAVE_STORAGE,TAP_BODY,reconstruct

ADDRESSES=['26a4540','26a4c00','26a66a0','2652be0']
CONTROL_SITES=['26a677a','26a677d','26a6785','26a67ba','26a67be','26a67c8','26a67cc','26a67d6','26a67e0','26a67e4','26a67e8','26a67f0','26a67fb','26a6806','26a680e','26a681d','26a6836','26a683c','26a6988','26a69b1','26a69b4','26a6901','26a6908','26a690f','26a6913','26a6919','26a691d','2652d09','2652d14','2652d1e','2652d29','26531ad','26531b8','2652dd2','2652dd8','2652de0','2652ec6','2652ece','2652ed1','26530f9','26530fc','2653101','2653110']

def sha(data):return hashlib.sha256(data).hexdigest()

def main():
    ap=argparse.ArgumentParser(__doc__);ap.add_argument('--apply',action='store_true');args=ap.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';m=json.loads(mp.read_text());assert not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    directories={a:cache/('decompiled-activated-graveyard' if a=='2652be0' else 'decompiled-activated-core') for a in ADDRESSES}
    names={a:meta[a]['name']+'_2022' for a in ADDRESSES};mapping={meta[a]['name']:n for a,n in names.items()}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);dw=debug.dwarf();lp=LocationParser(dw.location_lists());parameters={};locals_by_address={};arrays={};enums={};inline_names={}
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {'functions/functions.c','functions/tutors.c'}:continue
        def location(p):
            attr=p.attributes.get('DW_AT_location')
            if attr is None:return None
            loc=lp.parse_from_attribute(attr,cu['version'],die=p)
            if isinstance(loc,LocationExpr):return describe_DWARF_expr(loc.loc_expr,dw.structs)
            return [dict(base=hex(x.base_address)) if isinstance(x,BaseAddressEntry) else dict(start=hex(x.begin_offset),end=hex(x.end_offset),expression=describe_DWARF_expr(x.loc_expr,dw.structs)) for x in loc]
        def locals(die,address):
            out=[]
            for c in die.iter_children():
                if c.tag=='DW_TAG_variable':
                    n=inherited_value(c,'DW_AT_name');out.append(dict(name=n,type=type_name(type_of(c)),location=location(c)))
                    if n in {'to_tap','tcigfu'}:
                        typ=type_of(c);assert typ.tag=='DW_TAG_array_type' and type_name(type_of(typ))=='int' and value(type_of(typ),'DW_AT_byte_size')==4
                        bounds=[s.attributes['DW_AT_upper_bound'] for s in typ.iter_children() if s.tag=='DW_TAG_subrange_type']
                        assert len(bounds)==1 and bounds[0].form=='DW_FORM_exprloc'
                        arrays[address]=dict(name=n,element_type='int',element_bytes=4,upper_bound=bounds[0].value,expression=describe_DWARF_expr(bounds[0].value,dw.structs),location=location(c))
                elif c.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:
                    if c.tag=='DW_TAG_inlined_subroutine':inline_names.setdefault(address,[]).append(inherited_value(c,'DW_AT_name'))
                    out.extend(locals(c,address))
            return out
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':enums.update({value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in die.iter_children() if c.tag=='DW_TAG_enumerator'})
            pc=value(die,'DW_AT_low_pc');address=f'{pc:x}' if pc else ''
            if address not in ADDRESSES:continue
            parameters[address]=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p),constant=value(p,'DW_AT_const_value')) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter']
            locals_by_address[address]=locals(die,address)
    assert set(parameters)==set(ADDRESSES) and [len(parameters[a]) for a in ADDRESSES]==[16,16,15,8] and enums['SARV_REPORT_COUNT']==8
    assert arrays['26a66a0']['upper_bound']==[145,56,6,49,28] and arrays['2652be0']['upper_bound']==[145,24,6,49,28]
    fixed=lambda a:{(v['name'],v['type'],v['location']) for v in locals_by_address[a] if isinstance(v['location'],str)}
    assert {('buffer','char[100]','(DW_OP_fbreg: -552)'),('td2','target_definition_t','(DW_OP_fbreg: -552)')}<=fixed('26a4c00')
    assert {('marked_for_sacrifice','char[151]','(DW_OP_fbreg: -603)'),('buffer','char[100]','(DW_OP_fbreg: -603)')}<=fixed('26a4540')
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    def code(a):return runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16))
    instructions={a:list(decoder.disasm(code(a),int(a,16))) for a in ADDRESSES};all_ins={f'{i.address:x}':i for items in instructions.values() for i in items}
    controls=[dict(address=a,bytes=all_ins[a].bytes.hex(),instruction=all_ins[a].mnemonic+' '+all_ins[a].op_str) for a in CONTROL_SITES]
    native_texts={a:readonly_string(runtime,int(a,16)) for a in ['27646a0','275fa77','276469b','275cc18','275cc29','275ce0f']}
    assert native_texts=={'27646a0':b'Select an untapped %s%s%s%s%s to tap (%d of %d)','275fa77':b'','276469b':b' ','275cc18':b'Select %d cards.','275cc29':b'Select up to %d cards.','275ce0f':b'TARGET_GRAVEYARD'}
    original_color=function_body(read(root/'functions/functions.c'),'get_sleighted_color_text')
    color_table=runtime.pe.get_data(0x2764900-0x2000000,64);color_words=[readonly_string(runtime,p).decode() for p in struct.unpack('<16I',color_table)]
    assert color_words==[w for n in ['colorless','black','blue','green','red','white','artifact','any'] for w in [n,n.upper()]] and 'get_sleighted_color_text' in inline_names['26a66a0']
    report_sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),instruction=i.mnemonic+' '+i.op_str) for i in instructions['26a4540'] if any(o.type==X86_OP_MEM and o.mem.disp==56 for o in i.operands)];assert report_sites
    proof=dict(debug_sha256=sha(debug_data),parameters=parameters,locals=locals_by_address,arrays=arrays,enum_values={},added_flag_members=dict(SARV_REPORT_COUNT=8),storage_declaration=GRAVE_STORAGE,control_sites=controls,native_texts=[dict(address=a,bytes=v.hex()) for a,v in native_texts.items()],functions={},
        source_color_helper=dict(source='functions/functions.c',name='get_sleighted_color_text',body_sha256=sha(original_color.encode()),preserves_native_uninitialized_format=True,inline_names=inline_names['26a66a0'],word_table_address='2764900',word_table_bytes=color_table.hex(),words=color_words),
        scalar_report=dict(bytes=4,field_offset=56,variable_type='int',sites=report_sites),
        tap_selection=dict(target_index=9,target_card_offset=192,vla_element_bytes=4,cleanup_uses_number_of_targets=True),
        grave_selection=dict(maximum_targets=19,retries=3,packed_player_shift=16,index_mask=65535,face_down_iid=904,allow_cancel_card_bias=-1000),
        scope='Full native specialized activation costs. Use integer sacrifice reporting, 216-byte tests/84-byte targets, a shared 100-byte text/target union, packed graveyard modes and dynamic int arrays. Reconstruct variadic arguments and outgoing VLA call frames; preserve native slot-9 selection, cleanup bounds, retry/fizzle behavior and uninitialized color-format argument.',
        integration_pending='Earlier aggregate/manual callers, lifecycle/dispatch integration, native DLL linkage and gameplay verification remain pending.')
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']};targets.update({int(a,16):n for a,n in names.items()});overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    entries=[];bodies={}
    for address,name in names.items():
        raw=(directories[address]/(address+'.c')).read_text();original=strip_comments(raw);native=code(address)
        assert any(s==2 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off==int(address,16) for n,off,sec,k,s in debug.functions()),address
        text=reconstruct(address,original,byte_literal(native_texts['27646a0']))
        assert text!=original
        changes=[dict(original=original,replacement=text,occurrences=1,kind='native activation cost/VLA reconstruction',machine_sites=[],machine_range=[address,meta[address]['high_pc'][2:]],machine_bytes=len(native),machine_sha256=sha(native))]
        proof['enum_values'].update({n:enums[n] for n in set(re.findall(r'\b(?:EVENT|GAA|SARV|SFG|TYPE|TYPETEXT|GS)_\w+\b',text)) if n in enums})
        text,_=rename(text,mapping);text,literals=recover_strings(text,runtime,{})
        body=clean(text,overlap,use_test_api_2022=True,use_target_api_2022=True,use_sacrifice_api_2022=True,use_spell_api_2022=True,use_extension_flags_2022=True,use_keyword_accessors_2022=True,use_ability_api_2022=True,use_pump_api_2022=True,use_activated_api_2022=True)
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x|CardFunction.*p_Stack',body),(address,UNRESOLVED.search(body).group() if UNRESOLVED.search(body) else re.findall(r'.*\._\d.*',body))
        body=format_c(body);bodies[address]=body;calls=[]
        for i in instructions[address]:
            if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm in targets:
                destination=targets[i.operands[0].imm];assert re.search(r'\b'+destination+r'\s*\(',body),(address,destination)
                calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{i.operands[0].imm:x}',name=destination))
        detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,canonical_calls=calls);proof['functions'][address]=detail
        entries.append(dict(name=name,address=address,source=meta[address]['source'],line=meta[address]['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,activation_cost_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    sources={s:read(root/s) for s in {e['source'] for e in entries}}
    for source in sources:
        sources[source]+='\n\n#ifndef SHANDALAR\n/* Recovered October 2022 specialized activation costs. */\n'
        if source=='functions/functions.c':sources[source]+=GRAVE_STORAGE+'\n'
        for e in entries:
            if e['source']==source:sources[source]+=f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n"+bodies[e['address']]+'\n'
        sources[source]+='#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+'/* October 2022 specialized activation cost APIs. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries)+'\n#endif\n\n'+header[at:]
    defs=read(root/'defs.h')
    if not re.search(r'\bSARV_REPORT_COUNT\s*=',defs):
        at=defs.index('\tSARV_REPORT_CMC');defs=defs[:at]+'\tSARV_REPORT_COUNT = 1<<3, /* October 2022 DWARF; previously unnamed bit. */\n'+defs[at:]
    scratch=cache/'activation-costs-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'));(scratch/'defs.h').write_bytes(defs.encode('latin1'))
    for source,content in sources.items():
        target=scratch/Path(source).name;target.write_bytes(content.encode('latin1'))
        r=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(target)],capture_output=True,text=True)
        (scratch/(Path(source).stem+'.log')).write_text(r.stderr)
        if r.returncode:print(r.stderr)
        r.check_returncode()
    export_path=cache/'decompiled-abi/export.tsv';rows=list(csv.DictReader(export_path.open(),delimiter='\t'));existing={r['address'] for r in rows};focused={r['address']:r for directory in set(directories.values()) for r in csv.DictReader((directory/'export.tsv').open(),delimiter='\t')}
    rows=[focused[r['address']] if r['address'] in names else r for r in rows];rows.extend(focused[a] for a in names if a not in existing)
    if args.apply:
        citations=m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not any(c['address'] in names for c in citations),'Preserve cited support before refreshing these exports.'
        for source,content in sources.items():(root/source).write_bytes(content.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));(root/'defs.h').write_bytes(defs.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=4;m['helper_functions']+=4;m['shared_helper_interfaces']+=4
        for e in entries:m['source_files'][e['source']]+=1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows});mp.write_text(json.dumps(m,indent=2))
        path=root/'docs/recovery-evidence/interface-recovery.json';i=json.loads(path.read_text());i['activation_costs_2022']=proof;path.write_text(json.dumps(i,indent=2))
        with export_path.open('w',newline='') as stream:
            w=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');w.writeheader();w.writerows(rows)
        for a in names:shutil.copyfile(directories[a]/(a+'.c'),export_path.parent/(a+'.c'))
    print(json.dumps(dict(applied=args.apply,new_helpers=4,dynamic_integer_arrays=2,grave_scratch_bytes=100)))

if __name__=='__main__':main()
