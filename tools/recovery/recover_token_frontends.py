"""Recover versioned ID/copy token entry points and their two guarded compiler parts."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from recover_token_core import rewrite
from promote_recovery import clean,strip_comments,UNRESOLVED,LITERAL
from audit_source import function_body

ENTRIES={'2666580':('generate_token_by_id_part_2022','_generate_token_by_id_impl.part.0',True),
         '2666640':('copy_token_part_2022','_copy_token.part.0',True),
         '2666680':('copy_token_2022','_copy_token',False),
         '26666b0':('generate_token_by_id_2022','_generate_token_by_id',False),
         '2666720':('generate_tokens_by_id_2022','_generate_tokens_by_id',False),
         '2666790':('generate_reserved_token_by_id_2022','_generate_reserved_token_by_id',False)}


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';directory=cache/'decompiled-token-frontends';source='functions/token_generation.c'
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data)
    parameters={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!=source:continue
        for die in cu.get_top_DIE().iter_children():
            at=value(die,'DW_AT_low_pc');address=f'{at:x}' if at else ''
            if address not in ENTRIES:continue
            ps=[]
            for p in die.iter_children():
                if p.tag!='DW_TAG_formal_parameter':continue
                attribute=p.attributes.get('DW_AT_location');assert attribute and value(p,'DW_AT_const_value') is None
                assert type_name(type_of(p))=='int'
                ps.append(dict(name=inherited_value(p,'DW_AT_name'),type='int',location=attribute.value,location_form=attribute.form))
            assert len(ps)=={'2666580':5,'2666640':4,'2666680':4,'26666b0':3,'2666720':4,'2666790':2}[address]
            parameters[address]=dict(return_type=type_name(type_of(die)),parameters=ps)
        break
    assert set(parameters)==set(ENTRIES) and parameters['26666b0']['return_type']=='int'
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());metadata={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    targets={0x2665470:'default_token_definition_2022',0x2665580:'copy_token_definition_2022',0x26659d0:'generate_token_2022',0x2664f30:'resolve_token_iid_2022',0x2664eb0:'set_token_2022',0x2666580:'generate_token_by_id_part_2022',0x2666640:'copy_token_part_2022'}
    proof=dict(debug_sha256=hashlib.sha256(debug_data).hexdigest(),functions={},scope='Versioned 2022 token ID/copy APIs. Preserve the public negative-source/count guards and the native integer result of single-token creation. Two private compiler parts retain their complete physical parameters; public copy never recurses into itself.',integration_pending='Existing legacy callers and native jump-table/card dispatch bindings still require downstream migration.')
    entries=[];bodies={}
    for address,(name,symbol,private) in ENTRIES.items():
        assert any(n==symbol and s==(3 if private else 2) and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+offset==int(address,16) for n,offset,sec,kind,s in debug.functions())
        row=metadata[address];assert row['source']==source
        raw=(directory/(address+'.c')).read_text();text=rewrite(strip_comments(raw))
        mapping={'generate_token_by_id_impl':'generate_token_by_id_part_2022','generate_token_by_id':'generate_token_by_id_2022','generate_tokens_by_id':'generate_tokens_by_id_2022','generate_reserved_token_by_id':'generate_reserved_token_by_id_2022','copy_token':('copy_token_part_2022' if address=='2666640' else 'copy_token_2022')}
        pattern=re.compile(LITERAL+r'|\b(?:'+'|'.join(mapping)+r')\b')
        text=pattern.sub(lambda m:mapping.get(m[0],m[0]),text)
        if address=='2666680':
            assert text.count('copy_token_2022(player,card,tok_player,tok_card)')==1
            text=text.replace('copy_token_2022(player,card,tok_player,tok_card)','copy_token_part_2022(player,card,tok_player,tok_card)',1)
        text=clean(text,use_keyword_accessors_2022=True,use_extension_flags_2022=True,use_mana_api_2022=True)
        if private:text='static '+text
        assert not UNRESOLVED.search(strip_comments(text))
        text=subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe','-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],input=text.encode(),capture_output=True,check=True).stdout.decode().replace('\r\n','\n')
        code=runtime.pe.get_data(int(address,16)-0x2000000,int(row['high_pc'],16)-int(address,16));instructions=list(Cs(CS_ARCH_X86,CS_MODE_32).disasm(code,int(address,16)))
        assert sum(i.size for i in instructions)==len(code)
        calls=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{target:x}',replacement=replacement) for i in instructions for target,replacement in targets.items() if i.mnemonic in {'call','jmp'} and i.op_str==hex(target)]
        assert calls
        if address=='2666680':assert len(calls)==1 and calls[0]['target']=='2666640'
        detail=dict(**parameters[address],machine_bytes=len(code),machine_sha256=hashlib.sha256(code).hexdigest(),canonical_calls=calls)
        proof['functions'][address]=detail
        prototype=text[:text.index('{')].strip()+';'
        entries.append(dict(name=name,original_name=row['name'],address=address,source=source,line=row['line'],static=private,prototype=prototype,raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),recovered_literals=[],token_frontend_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=hashlib.sha256(function_body(text,name).encode()).hexdigest()))
        bodies[address]=text
    candidate=(root/source).read_bytes().decode('latin1').replace('\r\n','\n')+'\n\n/* October 2022 token ID/copy entry points. */\n'+'\n'.join(e['prototype'] for e in entries)+'\n'
    for entry in entries:candidate+=f"\n/* ManalinkEh 2022: 0x{entry['address']}; original {source}:{entry['line']}. */\n"+bodies[entry['address']]
    header=(root/'manalink.h').read_bytes().decode('latin1').replace('\r\n','\n');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+'/* October 2022 token ID/copy APIs; legacy signatures retained. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if not e['static'])+'\n#endif\n\n'+header[at:]
    scratch=cache/'token-frontends-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'));(scratch/'token_generation.c').write_bytes(candidate.encode('latin1'))
    result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(scratch/'token_generation.c')],capture_output=True,text=True)
    (scratch/'check.log').write_text(result.stderr);result.check_returncode()
    if args.apply:
        p=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(p.read_text());assert not set(bodies)&{f['address'] for f in manifest['accepted_functions']}
        (root/source).write_bytes(candidate.replace('\n','\r\n').encode('latin1'));(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        citations=[*manifest.get('source_fragments',[])];citations.extend(s for f in manifest['accepted_functions'] for s in f.get('supporting_exports',[]))
        for citation in citations:
            if citation['address'] not in ENTRIES:continue
            old=cache/citation.get('raw_export','decompiled-abi/'+citation['address']+'.c');assert hashlib.sha256(old.read_text().encode()).hexdigest()==citation['raw_sha256']
            saved=cache/'decompiled-support'/(citation['address']+'-'+citation['raw_sha256']+'.c');saved.parent.mkdir(exist_ok=True)
            if old!=saved:shutil.copyfile(old,saved)
            citation['raw_export']=saved.relative_to(cache).as_posix()
        manifest['accepted_functions'].extend(entries);manifest['accepted']=len(manifest['accepted_functions']);p.write_text(json.dumps(manifest,indent=2))
        p=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(p.read_text());interfaces['token_frontends_2022']=proof;p.write_text(json.dumps(interfaces,indent=2))
        p=cache/'decompiled-abi/export.tsv';rows=list(csv.DictReader(p.open(),delimiter='\t'));old={r['address']:r for r in rows}
        for row in csv.DictReader((directory/'export.tsv').open(),delimiter='\t'):
            if row['address'] in old:old[row['address']].update(row)
            else:rows.append(row)
            shutil.copyfile(directory/(row['address']+'.c'),p.parent/(row['address']+'.c'))
        with p.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
    print(json.dumps(dict(applied=args.apply,implementations=6,public_apis=4,private_parts=2)))


if __name__=='__main__':main()
