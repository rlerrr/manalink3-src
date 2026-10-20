"""Recover all three extension flag-word APIs without replacing the legacy APIs."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_MEM
from inspect_release import Image,value
from promote_recovery import clean,strip_comments,UNRESOLVED,project_extension_flags,EXTENSION_FLAG_NAMES
from audit_source import function_body


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(data)
    foundation=json.loads((root/'docs/recovery-evidence/extension-foundation.json').read_text())
    assert foundation['debug_sha256']==hashlib.sha256(data).hexdigest() and foundation['source_recovered']
    fields={m['name']:(m['offset'],m['bytes']) for m in foundation['types']['card_ext_t']['members']}
    assert {n:fields[n] for n in ('special_flags','special_flags2','special_flags3')}=={'special_flags':(180,4),'special_flags2':(184,4),'special_flags3':(188,4)}
    values={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/functions.c':continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag!='DW_TAG_enumeration_type':continue
            members={value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in die.iter_children()}
            if any(n in members for n in ('SF_LETHAL_DAMAGE_DESTROY','SF2_KIRA_GREAT_GLASS_SPINNER','SF3_0')):
                assert value(die,'DW_AT_byte_size')==4;values.update(members)
        break
    assert values['SF3_0']==0
    exports=cache/'decompiled-extension-flags';rows=list(csv.DictReader((exports/'export.tsv').open(),delimiter='\t'))
    assert len(rows)==13 and {r['name'] for r in rows}==set(EXTENSION_FLAG_NAMES)
    metadata={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    image=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    bodies={};proof=dict(debug_sha256=hashlib.sha256(data).hexdigest(),layout_evidence='extension-foundation.json',functions={},
        source_enum_values=values,
        scope='All three extension flag words: getters/checks/setters/removers. Preserve single-card and all-in-play branches. Old target-slot APIs remain available; new symbols have a _2022 suffix.',
        integration_pending='Earlier recovered and original callers, native hooks and flag refresh still need coordinated migration.')
    for row in rows:
        a=row['address'];raw=(exports/(a+'.c')).read_text();body=raw;complements=[]
        def complement(match):
            names=re.findall(r'\bSF2?_\w+\b',match[1]);assert len(names)==32 and len(set(names))==32
            mask=0
            for name in names:mask|=values[name]&0xffffffff
            assert mask==0xffffffff
            complements.append(dict(original=match[0],mask=mask,enumerators={name:values[name] for name in names}))
            return '(~(uint32_t)value)'
        body,count=re.subn(r'\(value\s*\^\s*\((SF2?_\w+[\s|\w]*)\)\)',complement,body)
        if row['name'] in ('remove_special_flags','remove_special_flags2'):assert count==2
        else:assert count==0
        body=re.sub(r'\bSF3_0\b','0U',body)
        body=clean(body,use_keyword_accessors_2022=True,use_extension_flags_2022=True)
        assert not UNRESOLVED.search(body)
        body=subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe','-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],input=body.encode(),capture_output=True,check=True).stdout.decode().replace('\r\n','\n')
        name=row['name']+'_2022';assert name+'(' in body;bodies[a]=body
        _,projections=project_extension_flags(strip_comments(raw));offsets={p['extension_offset'] for p in projections}
        start=int(a,16);length=int(metadata[a]['high_pc'],16)-start;machine=image.pe.get_data(start-0x2000000,length)
        instructions=list(decoder.disasm(machine,start));assert sum(i.size for i in instructions)==length
        sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),instruction=i.mnemonic+' '+i.op_str,offset=o.mem.disp)
               for i in instructions for o in i.operands if o.type==X86_OP_MEM and o.size==4 and o.mem.disp in offsets]
        assert {s['offset'] for s in sites}==offsets
        proof['functions'][a]=dict(machine_sha256=hashlib.sha256(machine).hexdigest(),bytes=length,member_sites=sites,projections=projections,complements=complements)
    source='functions/functions.c';path=root/source;original=path.read_bytes().decode('latin1').replace('\r\n','\n')
    manifest_path=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(manifest_path.read_text())
    known={f['address']:f for f in manifest['accepted_functions']}
    assert set(bodies)&set(known)=={'2679db0'}
    prior=known['2679db0'];assert prior['name']=='get_special_flags_by_instance' and 'compatibility adaptation' in prior['reconstruction']
    original,count=re.subn(r'/\* ManalinkEh 2022: 0x2679db0;[^*]*\*/',
        '/* Legacy compatibility accessor: original target-slot storage.\n * The recovered extension-backed getter is below. */',original);assert count==1
    proof['corrected_compatibility_getter']=dict(address='2679db0',previous_name=prior['name'],previous_raw_sha256=prior['raw_sha256'],previous_reconstruction=prior['reconstruction'])
    prototypes=[bodies[r['address']][:bodies[r['address']].index('{')].strip()+';' for r in rows]
    addition='\n\n/* October 2022 extension-backed flag APIs. */\n'+'\n'.join(prototypes)+'\n'
    for row in rows:addition+=f"\n/* ManalinkEh 2022: 0x{row['address']}; original {source}:{metadata[row['address']]['line']}. */\n"+bodies[row['address']]
    candidate=original+addition;scratch=cache/'extension-flags-candidate.c';scratch.write_bytes(candidate.encode('latin1'))
    result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(root),str(scratch)],capture_output=True,text=True)
    (cache/'extension-flags-check.log').write_text(result.stderr);result.check_returncode()
    if args.apply:
        p=manifest_path
        path.write_bytes(candidate.replace('\n','\r\n').encode('latin1'))
        h=root/'manalink.h';text=h.read_bytes().decode('latin1').replace('\r\n','\n');at=text.rfind('#ifdef __cplusplus')
        h.write_bytes((text[:at]+'/* October 2022 extension-backed flag interfaces. */\n#ifndef SHANDALAR\n'+'\n'.join(prototypes)+'\n#endif\n\n'+text[at:]).replace('\n','\r\n').encode('latin1'))
        for row,prototype in zip(rows,prototypes):
            a=row['address'];raw=(exports/(a+'.c')).read_text();name=row['name']+'_2022'
            entry=dict(name=name,original_name=row['name'],address=a,source=source,line=metadata[a]['line'],static=False,prototype=prototype,raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),recovered_literals=[],reconstruction=proof['scope'],extension_flags_evidence=proof['functions'][a],reconstruction_body_sha256=hashlib.sha256(function_body(bodies[a],name).encode()).hexdigest())
            if a=='2679db0':
                entry['previous_recovery']=proof['corrected_compatibility_getter'];prior.clear();prior.update(entry)
            else:manifest['accepted_functions'].append(entry)
        manifest['accepted']=len(manifest['accepted_functions']);p.write_text(json.dumps(manifest,indent=2))
        p=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(p.read_text());interfaces['extension_flags_2022']=proof;p.write_text(json.dumps(interfaces,indent=2))
        p=cache/'decompiled-abi/export.tsv';existing=list(csv.DictReader(p.open(),delimiter='\t'));known_rows={r['address']:r for r in existing};assert set(known_rows)&set(bodies)=={'2679db0'}
        for row in rows:
            if row['address'] in known_rows:known_rows[row['address']].update(row)
            else:existing.append(row)
        with p.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=existing[0],delimiter='\t');writer.writeheader();writer.writerows(existing)
        for row in rows:shutil.copyfile(exports/(row['address']+'.c'),p.parent/(row['address']+'.c'))
    print(json.dumps(dict(applied=args.apply,new_helpers=12,corrected_compatibility_getters=1,words=3,complement_expressions=sum(len(e['complements']) for e in proof['functions'].values()))))


if __name__=='__main__':main()
