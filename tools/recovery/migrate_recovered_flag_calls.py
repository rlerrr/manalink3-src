"""Use verified 2022 flag callees within already recovered functions only."""
import argparse
import csv
import hashlib
import json
import re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from inspect_release import Image
from promote_recovery import EXTENSION_FLAG_NAMES,LITERAL,code_only,strip_comments
from format_recovery import definition_span
from audit_source import function_body


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';path=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(path.read_text())
    rows=list(csv.DictReader((cache/'decompiled-extension-flags/export.tsv').open(),delimiter='\t'));targets={r['name']:int(r['address'],16) for r in rows};assert set(targets)==set(EXTENSION_FLAG_NAMES)
    metadata={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    image=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32)
    sources={};changes=[];skipped=[];pattern=re.compile(r'\b('+'|'.join(EXTENSION_FLAG_NAMES)+r')(?=\s*\()')
    storage_path=root/'docs/recovery-evidence/stack-storage.json';storage=json.loads(storage_path.read_text())
    for function in manifest['accepted_functions']:
        a=function['address'];raw=(cache/'decompiled-abi'/(a+'.c')).read_text()
        if not pattern.search(code_only(strip_comments(raw))):continue
        source=function['source']
        if source not in sources:sources[source]=(root/source).read_bytes().decode('latin1').replace('\r\n','\n')
        text=sources[source];at=text.index('/* ManalinkEh 2022: 0x'+a+';');_,begin,end=definition_span(text[at:],function['name']);chunk=text[at+begin:at+end]
        masked=re.sub(LITERAL+r'|/\*.*?\*/|//[^\r\n]*',lambda m:' '*len(m[0]),chunk,flags=re.S)
        matches=list(pattern.finditer(masked))
        if not matches:continue
        start=int(a,16);length=int(metadata[a]['high_pc'],16)-start;machine=image.pe.get_data(start-0x2000000,length)
        instructions=list(decoder.disasm(machine,start));callees=[]
        for name in sorted({m[1] for m in matches}):
            sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{targets[name]:x}') for i in instructions if i.mnemonic in {'call','jmp'} and i.op_str==hex(targets[name])]
            inline=None
            if a=='26955c0' and name=='get_special_flags3_by_instance':
                expression='((pcVar3->field_21).ext)->special_flags3'
                assert raw.count(expression)==1 and re.search(r'\bget_special_flags3_by_instance\(pcVar3\)',chunk)
                checks={'2695619':'e892d40100','269561e':'8b402c','2695621':'f680bd00000002'}
                for address,expected in checks.items():assert image.pe.get_data(int(address,16)-0x2000000,len(bytes.fromhex(expected))).hex()==expected
                inline=dict(raw_expression=expression,instance_offset=44,member_offset=188,test_byte_offset=189,mask=512,machine_checks=checks)
            if (not sites and not inline) or (not inline and not re.search(r'\b'+name+r'\s*\(',code_only(strip_comments(raw)))):
                skipped.append(dict(address=a,name=function['name'],callee=name,reason='No direct canonical callee proof'));break
            callees.append(dict(original=name,replacement=name+'_2022',target=f'{targets[name]:x}',calls=sites,inline_read=inline,source_occurrences=sum(m[1]==name for m in matches)))
        else:
            replacement=chunk
            for match in reversed(matches):replacement=replacement[:match.start()]+match[1]+'_2022'+replacement[match.end():]
            old_body=function_body(chunk,function['name']);new_body=function_body(replacement,function['name'])
            old_hash=hashlib.sha256(old_body.encode()).hexdigest();new_hash=hashlib.sha256(new_body.encode()).hexdigest()
            sources[source]=text[:at+begin]+replacement+text[at+end:]
            proof=dict(previous_body_sha256=old_hash,body_sha256=new_hash,callees=callees,scope='Only existing recovered function bodies, retaining all arguments/control flow. Renamed APIs have direct canonical call/tail-jump proof or a separately verified inline field read.')
            function['extension_flag_call_migration']=proof
            if function.get('reconstruction_body_sha256'):assert function['reconstruction_body_sha256']==old_hash
            function['reconstruction_body_sha256']=new_hash
            function['reconstruction']=function.get('reconstruction','')+' Verified extension-backed flag callee migration.'
            stack=function.get('stack_storage_reconstruction')
            if stack:
                assert stack['body_sha256']==old_hash;stack['flag_migration_previous_body_sha256']=old_hash;stack['body_sha256']=new_hash
                saved=storage['functions'][a];assert saved['body_sha256']==old_hash;saved['flag_migration_previous_body_sha256']=old_hash;saved['body_sha256']=new_hash
            changes.append(dict(name=function['name'],address=a,source=source,calls=sum(e['source_occurrences'] for e in callees)))
    if args.apply:
        assert changes
        for source in {f['source'] for f in changes}:(root/source).write_bytes(sources[source].replace('\n','\r\n').encode('latin1'))
        path.write_text(json.dumps(manifest,indent=2));storage_path.write_text(json.dumps(storage,indent=2))
        evidence=root/'docs/recovery-evidence/extension-flag-callers.json'
        previous=json.loads(evidence.read_text()) if evidence.exists() else {}
        merged={f['address']:f for f in previous.get('functions',[])};merged.update({f['address']:f for f in changes})
        evidence.write_text(json.dumps(dict(scope='Previously recovered implementations only; original functions and source fragments are not migrated.',functions=list(merged.values()),skipped=skipped),indent=2))
    (cache/'flag-migration-report.json').write_text(json.dumps(dict(applied=args.apply,functions=len(changes),call_expressions=sum(f['calls'] for f in changes),source_files=len({f['source'] for f in changes}),skipped=skipped),indent=2))
    print((cache/'flag-migration-report.json').read_text())


if __name__=='__main__':main()
