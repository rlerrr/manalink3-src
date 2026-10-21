"""Recover three Aura compiler parts and repair public wrappers bound to their own names."""
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
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, strip_comments, UNRESOLVED
from audit_source import function_body

PAIRS = {'26e65a0': ('attached_gets_pt', '26e7870'),
         '26e6650': ('attached_set_pt', '26e78b0'),
         '26e6710': ('disabling_aura_ai', '26e8710')}


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';source='functions/auras.c';directory=cache/'decompiled-aura-core'
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data)
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    metadata={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    parameters={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!=source:continue
        for die in cu.get_top_DIE().iter_children():
            at=value(die,'DW_AT_low_pc');address=f'{at:x}' if at else ''
            if address not in set(PAIRS)|{p[1] for p in PAIRS.values()}:continue
            ps=[]
            for p in die.iter_children():
                if p.tag!='DW_TAG_formal_parameter':continue
                location=p.attributes.get('DW_AT_location');assert location and value(p,'DW_AT_const_value') is None
                ps.append(dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location.value,location_form=location.form))
            parameters[address]=ps
        break
    proof=dict(debug_sha256=hashlib.sha256(debug_data).hexdigest(),parameters=parameters,functions={},repairs={},
               scope='Three physical Aura compiler parts have distinct native entries and private COFF linkage. Their public wrappers call those entries after their existing guards. Keep the original public APIs and replace only the erroneous self-call bindings.',
               integration_pending='Aura casting and generic-spell reconstruction still need the 216-byte test family; no game execution performed.')
    path=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(path.read_text());accepted={f['address']:f for f in manifest['accepted_functions']}
    assert not set(PAIRS)&set(accepted)
    text=(root/source).read_bytes().decode('latin1').replace('\r\n','\n');entries=[];bodies={}
    for address,(original,wrapper) in PAIRS.items():
        assert len(parameters[address])==len(parameters[wrapper])==(3 if original=='disabling_aura_ai' else 5)
        for at,storage in [(address,3),(wrapper,2)]:
            assert any(s==storage and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off==int(at,16) for n,off,sec,k,s in debug.functions())
        name=original+'_part_2022';row=metadata[address];raw=(directory/(address+'.c')).read_text()
        body=clean(strip_comments(raw),use_keyword_accessors_2022=True,use_extension_flags_2022=True)
        body=re.sub(r'\b'+original+r'\b',name,body);body='static '+body
        assert not UNRESOLVED.search(body)
        body=subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe','-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],input=body.encode(),capture_output=True,check=True).stdout.decode().replace('\r\n','\n')
        code=runtime.pe.get_data(int(address,16)-0x2000000,int(row['high_pc'],16)-int(address,16))
        detail=dict(machine_bytes=len(code),machine_sha256=hashlib.sha256(code).hexdigest())
        proof['functions'][address]=detail
        entry=dict(name=name,address=address,source=source,line=row['line'],static=True,prototype=body[:body.index('{')].strip()+';',raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),recovered_literals=[],aura_part_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=hashlib.sha256(function_body(body,name).encode()).hexdigest())
        entries.append(entry);bodies[address]=body
        before=function_body(text,original);split=before.index('{');head,tail=before[:split],before[split:]
        assert len(re.findall(r'\b'+original+r'\s*\(',tail))==1
        after=head+re.sub(r'\b'+original+r'\b',name,tail)
        assert text.count(before)==1;text=text.replace(before,after)
        marker='/* ManalinkEh 2022: 0x'+wrapper+';'
        at=text.index(marker);text=text[:at]+entry['prototype']+'\n\n'+text[at:]
        wrapper_code=runtime.pe.get_data(int(wrapper,16)-0x2000000,int(metadata[wrapper]['high_pc'],16)-int(wrapper,16))
        calls=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=address) for i in decoder.disasm(wrapper_code,int(wrapper,16)) if i.mnemonic in {'jmp','call'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==int(address,16)]
        assert len(calls)==1
        repair=dict(name=original,part=name,part_address=address,machine_bytes=len(wrapper_code),machine_sha256=hashlib.sha256(wrapper_code).hexdigest(),calls=calls,previous_body_sha256=hashlib.sha256(before.encode()).hexdigest(),body_sha256=hashlib.sha256(after.encode()).hexdigest())
        proof['repairs'][wrapper]=repair
        accepted[wrapper]['aura_part_binding_2022']=repair;accepted[wrapper]['reconstruction_body_sha256']=repair['body_sha256']
    text+='\n\n/* Recovered October 2022 Aura compiler parts. */\n'
    for e in entries:text+=f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n"+bodies[e['address']]+'\n'
    scratch=cache/'aura-parts-check.c';scratch.write_bytes(text.encode('latin1'))
    result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(root),str(scratch)],capture_output=True,text=True)
    (cache/'aura-parts-check.log').write_text(result.stderr);result.check_returncode()
    export_path=cache/'decompiled-abi/export.tsv';rows=list(csv.DictReader(export_path.open(),delimiter='\t'));focused={r['address']:r for r in csv.DictReader((directory/'export.tsv').open(),delimiter='\t')};existing={r['address'] for r in rows}
    rows=[focused[r['address']] if r['address'] in PAIRS else r for r in rows];rows.extend(focused[a] for a in PAIRS if a not in existing)
    if args.apply:
        citations=manifest['source_fragments']+[s for f in manifest['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not any(c['address'] in PAIRS for c in citations), 'Preserve cited raw exports before refreshing this component'
        (root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'));manifest['accepted_functions']=list(accepted.values())+entries
        manifest['accepted']+=3;manifest['helper_functions']+=3;manifest['private_functions']+=3;manifest['private_helpers']+=3;manifest['source_files'][source]+=3
        manifest['raw_export_entries']=len(rows);manifest['exported_functions']=len({r['name'] for r in rows});path.write_text(json.dumps(manifest,indent=2))
        path=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(path.read_text());interfaces['aura_parts_2022']=proof;path.write_text(json.dumps(interfaces,indent=2))
        with export_path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
        for address in PAIRS:shutil.copyfile(directory/(address+'.c'),export_path.parent/(address+'.c'))
    print(json.dumps(dict(applied=args.apply,new_private_helpers=3,corrected_public_wrappers=3)))


if __name__=='__main__':main()
