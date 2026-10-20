"""Recover the complete tapped-mana producer component and versioned front ends."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from elftools.dwarf.locationlists import LocationParser,LocationExpr,BaseAddressEntry
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import clean,strip_comments,code_only,UNRESOLVED,MANA_API_NAMES
from recover_literals import recover_strings
from audit_source import function_body


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';source='functions/produce_mana.c'
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);dwarf=debug.dwarf();locations=LocationParser(dwarf.location_lists());specialization={};mask_enum=None
    for cu in dwarf.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!=source:continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':
                for child in die.iter_children():
                    if value(child,'DW_AT_name')=='SF2_QUARUM_TRENCH_GNOMES':
                        assert value(child,'DW_AT_const_value')==0x800000;mask_enum=dict(value=0x800000,die=hex(child.offset),enum_die=hex(die.offset))
            if value(die,'DW_AT_low_pc')!=0x2670e00:continue
            for p in die.iter_children():
                if p.tag!='DW_TAG_formal_parameter':continue
                expression=[];attribute=p.attributes.get('DW_AT_location')
                if attribute:
                    entries=locations.parse_from_attribute(attribute,cu['version'],p)
                    if isinstance(entries,LocationExpr):expression=entries.loc_expr
                    else:
                        base=value(cu.get_top_DIE(),'DW_AT_low_pc',0)
                        for entry in entries:
                            if isinstance(entry,BaseAddressEntry):base=entry.base_address;continue
                            lo=entry.begin_offset if entry.is_absolute else base+entry.begin_offset;hi=entry.end_offset if entry.is_absolute else base+entry.end_offset
                            if lo<=0x2670e00<hi:expression=entry.loc_expr;break
                specialization[inherited_value(p,'DW_AT_name')]=dict(type=type_name(type_of(p)),location=expression,constant=value(p,'DW_AT_const_value'))
        break
    assert mask_enum
    assert {n:p['location'] for n,p in specialization.items() if n!='mca'}=={'player':[80],'card':[82],'available':[81],'num_mana':[145,0],'prompt':[145,4]}
    assert specialization['mca']==dict(type='int',location=[],constant=1)
    assert 'RECOVERY_EXPORT_COMPLETE success=2 failed=0' in (cache/'mana-different-corrected-console.log').read_text() and 'ERROR' not in (cache/'mana-different-corrected-console.log').read_text()
    image=Image((cache/'2022-ManalinkEh.dll').read_bytes());metadata={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    rows=list(csv.DictReader((cache/'decompiled-mana-frontends/export.tsv').open(),delimiter='\t'))
    canonical=list(csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t'))
    names={n+'_impl' for n in MANA_API_NAMES if n!='count_mana'}
    rows.extend(r for r in canonical if r['name'] in names)
    assert len(rows)==14 and len({r['address'] for r in rows})==14
    corrected={r['address']:r for r in csv.DictReader((cache/'decompiled-mana-different/export.tsv').open(),delimiter='\t')}
    rows=[corrected.get(r['address'],r) for r in rows]
    directories={r['address']:cache/('decompiled-abi' if r['name'] in names else 'decompiled-mana-frontends') for r in rows}
    directories.update({'2670e00':cache/'decompiled-mana-different','2671510':cache/'decompiled-mana-different'})
    proof=dict(debug_sha256=hashlib.sha256(debug_data).hexdigest(),specialized_different_colors=specialization,quarum_flag=mask_enum,functions={},scope='Seven tapped-mana cores and seven versioned public front ends. Retain override/transmutation rules, cancellation, chosen/tapped mana bookkeeping and prompt flow. Distinct-color worker has five physical arguments and mca=1.',integration_pending='Original 2020 callers and native hook routing remain a separate migration. No gameplay tests or DLL link performed.')
    bodies={};entries=[];prototypes=[];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for row in rows:
        a=row['address'];raw=(directories[a]/(a+'.c')).read_text();resolved,literals=recover_strings(strip_comments(raw),image,{})
        body=clean(resolved,use_keyword_accessors_2022=True,use_extension_flags_2022=True,use_mana_api_2022=True)
        # The prompt flows only to read-only formatters/choosers; C's const check
        # rejects mutation or forwarding it through a writable-buffer interface.
        signature,rest=body.split('{',1)
        prompt_read_only=bool(re.search(r'\bchar\s*\*\s*prompt\b',signature))
        if prompt_read_only:signature=re.sub(r'\bchar\s*\*\s*prompt\b','const char *prompt',signature)
        body=signature+'{'+rest
        static=a=='2670e00'
        if static:body='static '+body
        assert not UNRESOLVED.search(code_only(body)) and not re.search(r'\._\d|\.field_',code_only(body))
        body=subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe','-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],input=body.encode(),capture_output=True,check=True).stdout.decode().replace('\r\n','\n');bodies[a]=body
        prototype=body[:body.index('{')].strip()+';';prototypes.append(prototype)
        name=(row['name']+'_2022' if row['name'] in MANA_API_NAMES else row['name'])
        if static:name='produce_mana_tapped_all_different_colors_2022_impl'
        assert re.search(r'\b'+name+r'\s*\(',body)
        start=int(a,16);length=int(metadata[a]['high_pc'],16)-start;machine=image.pe.get_data(start-0x2000000,length);instructions=list(decoder.disasm(machine,start));assert sum(i.size for i in instructions)==length
        mask_sites=[]
        if 'SF2_QUARUM_TRENCH_GNOMES' in body:
            mask_sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),instruction=i.mnemonic+' '+i.op_str) for i in instructions if any(o.type==X86_OP_IMM and o.imm==0x800000 for o in i.operands)]
            assert mask_sites
        detail=dict(machine_bytes=length,machine_sha256=hashlib.sha256(machine).hexdigest(),mask_sites=mask_sites,prompt_read_only=prompt_read_only)
        proof['functions'][a]=detail
        entries.append(dict(name=name,original_name=row['name'],address=a,source=source,line=metadata[a]['line'],static=static,prototype=prototype,raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),recovered_literals=literals,reconstruction=proof['scope'],mana_production_evidence=detail,reconstruction_body_sha256=hashlib.sha256(function_body(body,name).encode()).hexdigest()))
    path=root/source;candidate=path.read_bytes().decode('latin1').replace('\r\n','\n')+'\n\n/* October 2022 tapped-mana producer component. */\n'+'\n'.join(prototypes)+'\n'
    for entry in entries:candidate+=f"\n/* ManalinkEh 2022: 0x{entry['address']}; original {source}:{entry['line']}. */\n"+bodies[entry['address']]
    scratch=cache/'mana-production-candidate.c';scratch.write_bytes(candidate.encode('latin1'))
    result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(root),str(scratch)],capture_output=True,text=True)
    (cache/'mana-production-check.log').write_text(result.stderr);result.check_returncode()
    if args.apply:
        p=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(p.read_text());assert not {e['address'] for e in entries}&{f['address'] for f in manifest['accepted_functions']}
        path.write_bytes(candidate.replace('\n','\r\n').encode('latin1'))
        h=root/'manalink.h';header=h.read_bytes().decode('latin1').replace('\r\n','\n');at=header.rfind('#ifdef __cplusplus');header=header[:at]+'/* October 2022 tapped-mana production interfaces. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if not e['static'])+'\n#endif\n\n'+header[at:];h.write_bytes(header.replace('\n','\r\n').encode('latin1'))
        manifest['accepted_functions'].extend(entries);manifest['accepted']=len(manifest['accepted_functions']);p.write_text(json.dumps(manifest,indent=2))
        p=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(p.read_text());interfaces['mana_production_2022']=proof;p.write_text(json.dumps(interfaces,indent=2))
        path=cache/'decompiled-abi/export.tsv';old={r['address']:r for r in canonical}
        for row in rows:
            if row['address'] in old:old[row['address']].update(row)
            else:canonical.append(row)
            shutil.copyfile(directories[row['address']]/(row['address']+'.c'),path.parent/(row['address']+'.c')) if directories[row['address']]!=path.parent else None
        with path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=canonical[0],delimiter='\t');writer.writeheader();writer.writerows(canonical)
    print(json.dumps(dict(applied=args.apply,new_helpers=14,cores=7,frontends=7)))


if __name__=='__main__':main()
