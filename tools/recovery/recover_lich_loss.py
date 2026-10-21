"""Recover the Lichs Mastery life-loss callback and native target predicate."""
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
from recover_literals import recover_strings,readonly_string,byte_literal
from lich_loss_body import BODY,STORAGE
from audit_source import function_body

ADDRESSES = '22ca570 22d0100'.split()
PRIVATE = {'22ca570'}
CONTROL_SITES = '22d0133 22d0174 22d0179 22d0188 22d018f 22d0194 22d016a 22d0160 22d02bd 22d02d3 22d02e6 22d031c 22d0351 22d0364 22d0394 22d03c3 22d042f 22d047d 22d0489 22d04b6'.split()
TEXT_ADDRESSES = '273cdb8 273cd85 273ceea 273d151 273cd55 273d139 273d124 273d154 273d1a4'.split()
DIALOG_STORES = '22d026c 22d0268 22d0260 22d0258 22d0250 22d0248 22d0240 22d0238 22d0230 22d0219 22d0228 22d0220 22d0215 22d028f 22d0287 22d027f 22d0277 22d026f'.split()


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
    assert 'lichs_mastery_loss_2022' not in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in manifest['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    names={a:meta[a]['name']+'_2022' for a in ADDRESSES}
    mapping={meta[a]['name']:n for a,n in names.items()}
    directories={a:cache/'decompiled-lich-loss' for a in ADDRESSES}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug=Image(debug_data);dwarf=debug.dwarf();locations=LocationParser(dwarf.location_lists())
    parameters,locals_,enums={},{},{}
    arrays={}
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
                    name=inherited_value(c,'DW_AT_name');typ=type_of(c)
                    out.append(dict(name=name,type=type_name(typ),location=location(c)))
                    if name in {'grave','tgts'}:
                        bounds=[p.attributes['DW_AT_upper_bound'] for p in typ.iter_children() if 'DW_AT_upper_bound' in p.attributes]
                        assert len(bounds)==1 and bounds[0].form=='DW_FORM_exprloc'
                        arrays[name]=dict(element_type=type_name(type_of(typ)),upper_bound=bounds[0].value,expression=describe_DWARF_expr(bounds[0].value,dwarf.structs))
                elif c.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:out.extend(variables(c))
            return out
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':enums.update({value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in die.iter_children() if c.tag=='DW_TAG_enumerator'})
            pc=value(die,'DW_AT_low_pc');address=f'{pc:x}' if pc else ''
            if address not in ADDRESSES:continue
            parameters[address]=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p)) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter']
            locals_[address]=variables(die)
    assert set(parameters)==set(ADDRESSES) and [len(parameters[a]) for a in ADDRESSES]==[3,3]
    assert arrays['grave']['upper_bound']==[145,8,6,49,28] and arrays['tgts']['upper_bound']==[145,8,6]
    fixed={(v['name'],v['type'],v['location']) for v in locals_['22d0100'] if isinstance(v['location'],str)}
    assert {('td','target_definition_t','(DW_OP_fbreg: -320)'),('buffer','char[100]','(DW_OP_fbreg: -236)'),('test','test_definition_t','(DW_OP_fbreg: -236)')}<=fixed
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    natives={a:runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)) for a in ADDRESSES}
    instructions={a:list(decoder.disasm(natives[a],int(a,16))) for a in ADDRESSES}
    by_site={f'{i.address:x}':i for items in instructions.values() for i in items}
    controls=[dict(address=a,bytes=by_site[a].bytes.hex(),instruction=by_site[a].mnemonic+' '+by_site[a].op_str) for a in CONTROL_SITES]
    texts={a:readonly_string(runtime,int(a,16)) for a in TEXT_ADDRESSES};assert all(x is not None for x in texts.values())
    executable=Image((cache/'2022-Magic.exe').read_bytes());error_rva=0x728f6c-executable.pe.OPTIONAL_HEADER.ImageBase
    error_section=next(s for s in executable.pe.sections if s.VirtualAddress<=error_rva<s.VirtualAddress+s.Misc_VirtualSize)
    assert error_section.Characteristics & 0x80000000 and error_rva-error_section.VirtualAddress>=error_section.SizeOfRawData
    dialog_stores=[dict(argument=n,address=a,bytes=by_site[a].bytes.hex()) for n,a in enumerate(DIALOG_STORES)]
    proof=dict(debug_sha256=sha(debug_data),parameters=parameters,locals=locals_,enum_values={},control_sites=controls,functions={},arrays=arrays,
        storage_declaration=STORAGE,shared_storage=dict(bytes=216,text_bytes=100,text_offset=0,test_offset=0,test_message_offset=0),
        call_frame=dict(dialog_words=18,outgoing_bytes=72,tutor_words=7,scnprintf_words=5),
        dialog_argument_stores=dialog_stores,
        native_texts=[dict(address=a,bytes=b.hex()) for a,b in texts.items()],
        error_text=dict(address='728f6c',expression='(char *)EXE_STR(0x728F6C)',kind='runtime_executable_buffer',section='.data',file_backed=False,executable_sha256=sha(executable.data)),
        native_edge_cases=dict(target_array_extra_element=1,failed_select_advances_index=True,failed_select_keeps_count=True,grave_tutor_return_ignored=True,unmark_all_before_exiling=True),
        scope='Full native Lichs Mastery life-loss callback with amount-sized grave flags, amount-plus-one targets, shared 216-byte test/100-byte text storage and private hand/permanent predicate. Reconstruct VLA call frames and preserve native selection failure, counting, marking, tutor and exile order.',
        integration_pending='Native lose_life routing, earlier manually reconstructed consumers, native linking and gameplay verification remain pending.')
    targets={int(f['address'],16):f['name'] for f in manifest['accepted_functions']}
    targets.update({int(a,16):n for a,n in names.items()})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    bodies,entries={},[]
    for address,name in names.items():
        raw=(directories[address]/(address+'.c')).read_text();text=strip_comments(raw)
        proof['enum_values'].update({n:enums[n] for n in set(re.findall(r'\b(?:EVENT|AL|COUNTER|RESOLVE_TRIGGER)_\w+\b',text)) if n in enums})
        changes=[]
        if address=='22d0100':
            replacement=BODY.replace('NATIVE_TARGET_FORMAT',byte_literal(texts['273d154'])).replace('NATIVE_GRAVE_FORMAT',byte_literal(texts['273d1a4']))
            changes.append(dict(original=text,replacement=replacement,occurrences=1,kind='native shared storage and VLA call-frame reconstruction',machine_sites=controls))
            text=replacement
        else:
            token='(char *)0x728f6c';assert text.count(token)==1
            replacement='(char *)EXE_STR(0x728F6C)'
            sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex()) for i in instructions[address] if any(o.type==X86_OP_IMM and o.imm==0x728f6c for o in i.operands)]
            assert sites
            changes.append(dict(original=token,replacement=replacement,occurrences=1,kind='verified executable target-error text',machine_sites=sites))
            text=text.replace(token,replacement)
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
            lichs_mastery_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    sources={s:read(root/s) for s in {e['source'] for e in entries}}
    for source in sources:
        sources[source]+='\n\n#ifndef SHANDALAR\n/* Recovered October 2022 Lichs Mastery life-loss helpers. */\n'
        sources[source]+='\n'+STORAGE+'\n'
        for e in entries:
            if e['source']==source:sources[source]+=f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n"+bodies[e['address']]+'\n'
        sources[source]+='#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+'/* October 2022 Lichs Mastery life-loss API. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if not e['static'])+'\n#endif\n\n'+header[at:]
    scratch=cache/'lich-loss-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'))
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
        manifest['accepted_functions']+=entries;manifest['accepted']+=2;manifest['helper_functions']+=2
        manifest['private_functions']+=1;manifest['private_helpers']+=1;manifest['shared_helper_interfaces']+=1
        for e in entries:manifest['source_files'][e['source']]=manifest['source_files'].get(e['source'],0)+1
        manifest['raw_export_entries']=len(rows);manifest['exported_functions']=len({r['name'] for r in rows})
        mp.write_text(json.dumps(manifest,indent=2));interfaces['lichs_mastery_loss_2022']=proof;ip.write_text(json.dumps(interfaces,indent=2))
        with export_path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
        for a in names:shutil.copyfile(directories[a]/(a+'.c'),export_path.parent/(a+'.c'))
    (cache/'lich-loss-plan.json').write_text(json.dumps(proof,indent=2))
    print(json.dumps(dict(applied=args.apply,new_helpers=2,private_helpers=1,public_helpers=1)))


if __name__=='__main__':main()
