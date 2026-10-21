"""Recover the twelve-slot extension subtype mutators, including native boundary reads."""
import argparse
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path
from inspect_release import Image,value
from export_abi import type_of,type_name
from promote_recovery import clean,strip_comments,UNRESOLVED
from audit_source import function_body


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';source='functions/has_creature_type.c'
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes())
    found={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!=source:continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag!='DW_TAG_typedef' or value(die,'DW_AT_name') not in {'card_ext_t','inst_info_t'}:continue
            name=value(die,'DW_AT_name');record=type_of(die)
            members={value(m,'DW_AT_name'):dict(offset=value(m,'DW_AT_data_member_location'),type=type_name(type_of(m))) for m in record.iter_children() if m.tag=='DW_TAG_member'}
            found[name]=dict(die=hex(record.offset),bytes=value(record,'DW_AT_byte_size'),members=members)
        break
    assert found['card_ext_t']['bytes']==192 and found['inst_info_t']['bytes']==8
    assert found['card_ext_t']['members']['subtype']==dict(offset=8,type='uint16_t[12]')
    assert found['card_ext_t']['members']['inst_info_data']==dict(offset=32,type='inst_info_t[6]')
    assert found['inst_info_t']['members']['id']==dict(offset=0,type='uint16_t')
    checks={'2672913':'8b462c','2672916':'8d5008','2672919':'8d7820','2672929':'83c202','267292c':'39fa','2672941':'83fa0c','2672950':'66895c5008',
            '2672da3':'8b472c','2672dca':'83f90b','2672ddd':'83fa0c','2672de2':'42','2672de3':'668b5c5008','2672dd8':'66895c5006',
            '2672ebf':'8b462c','2672ec8':'31d2','2672eca':'668b545808','2672ecf':'4a','2672ed0':'81fafe0f0000','2672ed6':'7610',
            '2672ef1':'83fb0b','2672efd':'83fa0c','2672f02':'42','2672f03':'668b4c5008','2672ef8':'66894c5006'}
    for at,expected_hex in checks.items():
        data=bytes.fromhex(expected_hex);assert runtime.pe.get_data(int(at,16)-0x2000000,len(data))==data
    proof=dict(debug_sha256=hashlib.sha256(debug_data).hexdigest(),types=found,machine_checks=checks,functions={},
               scope='Extension subtype[12] at +8 through instance.ext at +44. Addition scans exactly twelve words; removal retains the native last lookahead at +32 through inst_info_data[0].id. Creature-type classification uses the native unsigned 32-bit decrement/range comparison. No sentinel or compaction fix invented.',
               integration_pending='Remaining original subtype queries/writers and native lifecycle/dispatch bindings require downstream integration; no game execution performed.')
    metadata={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    bodies={};entries=[]
    for address in ['2672900','2672d90','2672eb0']:
        row=metadata[address];name=row['name'];assert row['source']==source
        assert any(n=='_'+name and s==2 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+offset==int(address,16) for n,offset,sec,kind,s in debug.functions())
        raw=(cache/'decompiled-abi'/(address+'.c')).read_text();text=strip_comments(raw)
        needle='(inst->field_21).ext';assert text.count(needle)==1;text=text.replace(needle,'inst->ext')
        adjustments=[]
        if address=='2672900':
            old='(inst_info_t *)puVar2 != pcVar1->inst_info_data';new='puVar2 != pcVar1->subtype + 12'
            assert text.count(old)==1;text=text.replace(old,new);adjustments.append(dict(original=old,replacement=new,kind='array endpoint',bytes=24))
        else:
            index='iVar4' if address=='2672d90' else 'iVar3'
            old='pcVar1->subtype['+index+' + 1]'
            new='('+index+' == 11 ? pcVar1->inst_info_data[0].id : pcVar1->subtype['+index+' + 1])'
            assert text.count(old)==2;text=text.replace(old,new)
            adjustments.append(dict(original=old,replacement=new,kind='adjacent information ID lookahead',occurrences=2,subtype_count=12,adjacent_offset=32,bytes=2))
        if address=='2672eb0':
            old='0xffe < pcVar1->subtype[iVar4] - 1';new='0xffeU < (uint32_t)pcVar1->subtype[iVar4] - 1U'
            assert text.count(old)==1;text=text.replace(old,new);adjustments.append(dict(original=old,replacement=new,kind='unsigned decrement/range',mask=4094))
        body=clean(text)
        assert not UNRESOLVED.search(strip_comments(body))
        body=subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe','-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],input=body.encode(),capture_output=True,check=True).stdout.decode().replace('\r\n','\n')
        code=runtime.pe.get_data(int(address,16)-0x2000000,int(row['high_pc'],16)-int(address,16))
        detail=dict(machine_bytes=len(code),machine_sha256=hashlib.sha256(code).hexdigest(),adjustments=adjustments)
        proof['functions'][address]=detail
        entries.append(dict(name=name,address=address,source=source,line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),recovered_literals=[],extension_subtype_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=hashlib.sha256(function_body(body,name).encode()).hexdigest()))
        bodies[address]=body
    candidate=(root/source).read_bytes().decode('latin1').replace('\r\n','\n')
    for entry in entries:candidate+=f"\n\n/* ManalinkEh 2022: 0x{entry['address']}; original {source}:{entry['line']}. */\n"+bodies[entry['address']]
    header=(root/'manalink.h').read_bytes().decode('latin1').replace('\r\n','\n');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+'/* October 2022 extension subtype mutation helpers. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries)+'\n#endif\n\n'+header[at:]
    scratch=cache/'extension-subtype-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'));(scratch/'has_creature_type.c').write_bytes(candidate.encode('latin1'))
    result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(scratch/'has_creature_type.c')],capture_output=True,text=True)
    (scratch/'check.log').write_text(result.stderr);result.check_returncode()
    if args.apply:
        p=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(p.read_text());assert not set(bodies)&{f['address'] for f in manifest['accepted_functions']}
        (root/source).write_bytes(candidate.replace('\n','\r\n').encode('latin1'));(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        manifest['accepted_functions'].extend(entries);manifest['accepted']=len(manifest['accepted_functions']);p.write_text(json.dumps(manifest,indent=2))
        p=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(p.read_text());interfaces['extension_subtype_mutators_2022']=proof;p.write_text(json.dumps(interfaces,indent=2))
    print(json.dumps(dict(applied=args.apply,new_helpers=3,subtype_slots=12,preserved_adjacent_word_bytes=2)))


if __name__=='__main__':main()
