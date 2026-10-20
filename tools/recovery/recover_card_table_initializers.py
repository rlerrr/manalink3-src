"""Recover Garth/Serra's copied integer tables, preserving their writable storage."""
import argparse
import csv
import hashlib
import json
import re
import struct
import subprocess
from pathlib import Path
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import clean,UNRESOLVED
from recover_literals import recover_strings
from audit_source import function_body


CARDS=[dict(address='2235c00',name='card_garth_one_eye',local='csvids',count=7,table=0x27210c0,global_name='garth_card_ids_2022',
    original='piVar5 = (int *)&_data;',replacement='piVar5 = garth_card_ids_2022;',checks={'2235cf6':'bec0107202','2235cfb':'b907000000','2235d00':'f3a5'}),
    dict(address='223dfe0',name='card_serras_emissary',local='type_array',count=9,table=0x27210e0,global_name='serras_emissary_types_2022',
    original='piVar4 = (int *)0x27210e0;',replacement='piVar4 = serras_emissary_types_2022;',checks={'223e17b':'bee0107202','223e180':'b909000000','223e185':'f3a5','223e2e4':'8b149da4f14e00','223e2eb':'81ca00000004','223e2f1':'89149da4f14e00','223e35c':'81cf00004000'})]


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';source='cards/modern_horizons_2.c'
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);layouts={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!=source:continue
        def walk(die,entry):
            for child in die.iter_children():
                if child.tag=='DW_TAG_variable' and inherited_value(child,'DW_AT_name')==entry['local']:
                    assert type_name(type_of(child))==f"int[{entry['count']}]"
                    layouts[entry['address']]=dict(die=hex(child.offset),type=type_name(type_of(child)))
                walk(child,entry)
        for die in cu.get_top_DIE().iter_children():
            entry=next((c for c in CARDS if value(die,'DW_AT_low_pc')==int(c['address'],16)),None)
            if entry:walk(die,entry)
        break
    assert len(layouts)==2
    image=Image((cache/'2022-ManalinkEh.dll').read_bytes());metadata={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    proof=dict(debug_sha256=hashlib.sha256(debug_data).hexdigest(),source=source,functions={},scope='Recover the copied seven/nine-int initial tables as file-private writable arrays, retaining native contents and per-call local copies. The DLL section is writable; no immutability claim is made.',integration_pending='Native bindings and complete protection/mana behavior remain downstream work.')
    declarations=[];bodies={};entries=[]
    for entry in CARDS:
        a=entry['address'];data=image.pe.get_data(entry['table']-0x2000000,entry['count']*4);values=list(struct.unpack('<'+'i'*entry['count'],data))
        section=next(s for s in image.pe.sections if s.VirtualAddress<=entry['table']-0x2000000<s.VirtualAddress+s.Misc_VirtualSize);assert section.Characteristics&0x80000000
        for address,expected in entry['checks'].items():assert image.pe.get_data(int(address,16)-0x2000000,len(bytes.fromhex(expected))).hex()==expected
        declaration=f"static int {entry['global_name']}[{entry['count']}] = {{"+', '.join(str(v) for v in values)+'};'
        declarations.append(declaration)
        raw=(cache/'decompiled-abi'/(a+'.c')).read_text();assert raw.count(entry['original'])==1
        resolved,literals=recover_strings(raw,image,{})
        resolved=resolved.replace(entry['original'],entry['replacement'])
        if a=='223dfe0':
            assert resolved.count('*(uint *)(&player_bits + player * 4)')==2
            resolved=resolved.replace('*(uint *)(&player_bits + player * 4)','player_bits[player]')
        body=clean(resolved,use_keyword_accessors_2022=True,use_extension_flags_2022=True)
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|\.field_',body)
        body=subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe','-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],input=body.encode(),capture_output=True,check=True).stdout.decode().replace('\r\n','\n');bodies[a]=body
        length=int(metadata[a]['high_pc'],16)-int(a,16)
        detail=dict(table_address=f"{entry['table']:x}",bytes=data.hex(),values=values,mutable=True,global_name=entry['global_name'],declaration=declaration,array_type=layouts[a],machine_checks=entry['checks'],machine_bytes=length,machine_sha256=hashlib.sha256(image.pe.get_data(int(a,16)-0x2000000,length)).hexdigest())
        proof['functions'][a]=detail
        entries.append(dict(name=entry['name'],address=a,source=source,line=metadata[a]['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),recovered_literals=literals,reconstruction=proof['scope'],card_table_evidence=detail,reconstruction_body_sha256=hashlib.sha256(function_body(body,entry['name']).encode()).hexdigest()))
    path=root/source;candidate=path.read_bytes().decode('latin1').replace('\r\n','\n')+'\n\n/* October 2022 copied local table sources (.data, writable). */\n'+'\n'.join(declarations)+'\n'
    for entry in entries:candidate+=f"\n/* ManalinkEh 2022: 0x{entry['address']}; original {source}:{entry['line']}. */\n"+bodies[entry['address']]
    scratch=cache/'card-table-candidate.c';scratch.write_bytes((candidate+'\n'+ '\n'.join(f'_Static_assert(sizeof({e["global_name"]}) == {e["count"]*4} && sizeof({e["global_name"]}[0]) == 4, "copied table width");' for e in CARDS)+'\n').encode('latin1'))
    result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(root),str(scratch)],capture_output=True,text=True)
    (cache/'card-table-check.log').write_text(result.stderr);result.check_returncode()
    if args.apply:
        p=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(p.read_text());assert not {e['address'] for e in entries}&{f['address'] for f in manifest['accepted_functions']}
        path.write_bytes(candidate.replace('\n','\r\n').encode('latin1'));manifest['accepted_functions'].extend(entries);manifest['accepted']=len(manifest['accepted_functions'])
        for entry,declaration in zip(CARDS,declarations):manifest.setdefault('source_globals',[]).append(dict(source=source,name=entry['global_name'],declaration=declaration,target_address=f"{entry['table']:x}",zero_initialized=False,linkage='static'))
        p.write_text(json.dumps(manifest,indent=2));p=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(p.read_text());interfaces['copied_card_tables_2022']=proof;p.write_text(json.dumps(interfaces,indent=2))
    print(json.dumps(dict(applied=args.apply,new_cards=2,writable_tables=2,total_table_bytes=64)))


if __name__=='__main__':main()
