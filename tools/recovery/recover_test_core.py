"""Recover the 216-byte test definition and versioned initialization/scoring/matching core."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM, X86_OP_MEM
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, strip_comments, UNRESOLVED
from audit_source import function_body

ADDRESSES=['26be510','26be6e0','26bea60','26bebb0','26bebe0','26bf6c0','26bf750','26c0130']


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';source='functions/global_checking.c';directory=cache/'decompiled-test-core'
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes())
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    layout=None;callback=None;parameters={}
    widths={'char[100]':100,'unsigned int':4,'short unsigned int':2,'int':4,'int16_t[2]':4,'unsigned char':1,'UNRESOLVED *':4}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!=source:continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_typedef' and value(die,'DW_AT_name')=='test_definition_t':
                record=type_of(die);layout=dict(die=hex(record.offset),bytes=value(record,'DW_AT_byte_size'),members=[])
                for m in record.iter_children():
                    if m.tag!='DW_TAG_member':continue
                    name=value(m,'DW_AT_name');typ=type_of(m);spelling=type_name(typ)
                    assert spelling in widths
                    layout['members'].append(dict(name=name,type=spelling,offset=value(m,'DW_AT_data_member_location'),bytes=widths[spelling]))
                    if name=='special_selection_function':
                        signature=type_of(typ);assert typ.tag=='DW_TAG_pointer_type' and signature.tag=='DW_TAG_subroutine_type'
                        callback=dict(return_type=type_name(type_of(signature)),arguments=[type_name(type_of(p)) for p in signature.iter_children() if p.tag=='DW_TAG_formal_parameter'])
            at=value(die,'DW_AT_low_pc');address=f'{at:x}' if at else ''
            if address not in ADDRESSES:continue
            ps=[]
            for p in die.iter_children():
                if p.tag!='DW_TAG_formal_parameter':continue
                location=p.attributes.get('DW_AT_location');assert location and value(p,'DW_AT_const_value') is None
                name=inherited_value(p,'DW_AT_name');t=type_of(p)
                readonly_message=name=='msg';assert not readonly_message or (t.tag=='DW_TAG_pointer_type' and type_of(t).tag=='DW_TAG_const_type')
                ps.append(dict(name=name,type=type_name(t),location=location.value,location_form=location.form,readonly_message=readonly_message))
            parameters[address]=ps
        break
    assert layout['bytes']==216 and callback==dict(return_type='int',arguments=['int']*4) and set(parameters)==set(ADDRESSES)
    storage=json.loads((root/'docs/recovery-evidence/stack-storage.json').read_text());native=storage['target_layouts']['test_definition_t']
    assert native['members']=={m['name']:m['offset'] for m in layout['members']} and native['member_sizes']=={m['name']:m['bytes'] for m in layout['members']}
    declarations=[]
    for m in layout['members']:
        name,typ=m['name'],m['type']
        if name=='special_selection_function':decl='test_selection_predicate_2022_t '+name
        elif '[' in typ:
            base,bounds=typ.split('[',1);decl=base+' '+name+'['+bounds
        else:decl={'unsigned int':'uint32_t','short unsigned int':'uint16_t','int':'int32_t','unsigned char':'uint8_t'}[typ]+' '+name
        declarations.append('  '+decl+';')
    block='/* October 2022 test ABI; the original 208-byte definition remains available. */\n#ifndef SHANDALAR\ntypedef int (*test_selection_predicate_2022_t)(int,int,int,int);\ntypedef struct\n{\n'+'\n'.join(declarations)+'\n} test_definition_2022_t;\n#endif'
    defs=(root/'defs.h').read_bytes().decode('latin1').replace('\r\n','\n');assert defs.count('} test_definition_t;')==1 and 'test_definition_2022_t;' not in defs
    defs=defs.replace('} test_definition_t;','} test_definition_t;\n\n'+block)
    metadata={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    manifest_path=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(manifest_path.read_text());assert not set(ADDRESSES)&{f['address'] for f in manifest['accepted_functions']}
    mapping={metadata[a]['name']:metadata[a]['name']+'_2022' for a in ADDRESSES};mapping['test_definition_t']='test_definition_2022_t'
    targets={int(a,16):mapping[metadata[a]['name']] for a in ADDRESSES}
    targets.update({int(f['address'],16):f['name'] for f in manifest['accepted_functions'] if f['name'] in {'check_for_special_ability_2022','has_subtype_by_id_2022'}})
    proof=dict(debug_sha256=hashlib.sha256(debug_data).hexdigest(),layout=layout,callback=callback,parameters=parameters,functions={},
               scope='Separate 216-byte test ABI with sp_keyword at +140 and sp_keyword_flag at +144. Original 208-byte tests and all existing callers stay intact. Eight versioned constructors, scorers and in-play/internal-ID matchers preserve native branches, callback arguments, special-ability queries and owner handling.',
               integration_pending='Test consumers such as sacrifice/grave/tutor APIs, generic-spell and Aura casting still require migration; no game execution performed.')
    entries=[];bodies={}
    for address in ADDRESSES:
        row=metadata[address];original=row['name'];name=mapping[original];raw=(directory/(address+'.c')).read_text();text=strip_comments(raw);changes=[]
        assert any(s==2 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off==int(address,16) for n,off,sec,k,s in debug.functions())
        if address in ['26bebe0','26bf750']:
            old='(short)*(undefined4 *)&this_test->sp_keyword_flag';new='(int16_t)this_test->sp_keyword_flag'
            assert text.count(old)==2;text=text.replace(old,new);changes.append(dict(original=old,replacement=new,occurrences=2,kind='low signed flag word',offset=144,bytes=2))
        if address=='26bf750':
            old='uVar18._0_2_ = cards_data[to_examine].id;\n  uVar18._2_2_ = cards_data[to_examine].reserved2;'
            new='uVar18 = (uint32_t)cards_data[to_examine].id | ((uint32_t)cards_data[to_examine].reserved2 << 16);'
            assert text.count(old)==1;text=text.replace(old,new);changes.append(dict(original=old,replacement=new,occurrences=1,kind='native ID word',id_offset=36,adjacent_offset=38,bytes=4))
        text=re.sub(r'_func_int_int_int_int_int\s*\*','test_selection_predicate_2022_t ',text)
        text=re.sub(r'\b(?:'+'|'.join(mapping)+r')\b',lambda m:mapping[m[0]],text)
        if address=='26bea60':text,count=re.subn(r'\bchar\s*\*\s*msg\b','const char *msg',text);assert count==1
        body=clean(text,use_keyword_accessors_2022=True,use_extension_flags_2022=True)
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d',body)
        body=subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe','-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],input=body.encode(),capture_output=True,check=True).stdout.decode().replace('\r\n','\n')
        code=runtime.pe.get_data(int(address,16)-0x2000000,int(row['high_pc'],16)-int(address,16));instructions=list(decoder.disasm(code,int(address,16)));calls=[]
        for i in instructions:
            if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm in targets:
                destination=targets[i.operands[0].imm];assert re.search(r'\b'+destination+r'\s*\(',body)
                calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{i.operands[0].imm:x}',name=destination))
        sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),instruction=i.mnemonic+' '+i.op_str) for i in instructions if any(op.type==X86_OP_MEM and op.mem.disp in [140,144,212] for op in i.operands)]
        for change in changes:
            if change['kind']=='low signed flag word':
                ats=['26bed36','26bed3c','26bed45','26bf4a8'] if address=='26bebe0' else ['26bf8b6','26bf8bc','26bf8c5','26bff7c']
                change['machine_sites']=[]
                for at in ats:
                    i=next(decoder.disasm(runtime.pe.get_data(int(at,16)-0x2000000,15),int(at,16),count=1))
                    change['machine_sites'].append(dict(address=at,bytes=i.bytes.hex(),instruction=i.mnemonic+' '+i.op_str))
                assert change['machine_sites'][1]['instruction'] in ['test dx, dx','test cx, cx']
            else:
                loads=[i for i in instructions if i.mnemonic=='mov' and len(i.operands)==2 and i.operands[1].type==X86_OP_MEM and i.operands[1].mem.disp==0x7e7034 and i.operands[1].size==4]
                assert len(loads)==1
                change['machine_sites']=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),instruction=i.mnemonic+' '+i.op_str) for i in loads]
        detail=dict(machine_bytes=len(code),machine_sha256=hashlib.sha256(code).hexdigest(),changes=changes,canonical_calls=calls,new_field_sites=sites)
        proof['functions'][address]=detail
        entries.append(dict(name=name,address=address,source=source,line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),recovered_literals=[],test_core_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=hashlib.sha256(function_body(body,name).encode()).hexdigest()))
        bodies[address]=body
    text=(root/source).read_bytes().decode('latin1').replace('\r\n','\n')+'\n\n#ifndef SHANDALAR\n'
    for e in entries:text+=f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n"+bodies[e['address']]+'\n'
    text+='#endif\n';header=(root/'manalink.h').read_bytes().decode('latin1').replace('\r\n','\n');at=header.rfind('#ifdef __cplusplus')
    header_block='/* October 2022 test constructors and matchers; require the 216-byte definition. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries)+'\n#endif\n\n'
    header=header[:at]+header_block+header[at:];scratch=cache/'test-core-check';scratch.mkdir(exist_ok=True)
    for name,content in [('defs.h',defs),('manalink.h',header),('global_checking.c',text)]: (scratch/name).write_bytes(content.encode('latin1'))
    result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(scratch/'global_checking.c')],capture_output=True,text=True)
    (scratch/'check.log').write_text(result.stderr);result.check_returncode()
    export_path=cache/'decompiled-abi/export.tsv';rows=list(csv.DictReader(export_path.open(),delimiter='\t'));focused={r['address']:r for r in csv.DictReader((directory/'export.tsv').open(),delimiter='\t')};existing={r['address'] for r in rows}
    rows=[focused[r['address']] if r['address'] in ADDRESSES else r for r in rows];rows.extend(focused[a] for a in ADDRESSES if a not in existing)
    if args.apply:
        citations=manifest['source_fragments']+[s for f in manifest['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not any(c['address'] in ADDRESSES for c in citations), 'Preserve cited raw exports before refreshing this component'
        for name,content in [('defs.h',defs),('manalink.h',header),(source,text)]: (root/name).write_bytes(content.replace('\n','\r\n').encode('latin1'))
        manifest['accepted_functions'].extend(entries);manifest['accepted']+=8;manifest['helper_functions']+=8;manifest['shared_helper_interfaces']+=8;manifest['source_files'][source]=manifest['source_files'].get(source,0)+8
        manifest['raw_export_entries']=len(rows);manifest['exported_functions']=len({r['name'] for r in rows});manifest_path.write_text(json.dumps(manifest,indent=2))
        path=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(path.read_text());interfaces['test_core_2022']=proof;path.write_text(json.dumps(interfaces,indent=2))
        with export_path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
        for address in ADDRESSES:shutil.copyfile(directory/(address+'.c'),export_path.parent/(address+'.c'))
    print(json.dumps(dict(applied=args.apply,new_helpers=8,legacy_test_bytes=208,test_2022_bytes=216,core_matchers=2)))


if __name__=='__main__':main()
