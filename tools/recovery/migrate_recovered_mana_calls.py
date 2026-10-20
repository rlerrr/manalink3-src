"""Route recovered mana producers/count calls through verified 2022 implementations."""
import argparse
import csv
import hashlib
import json
import re
import struct
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from inspect_release import Image
from promote_recovery import MANA_API_NAMES,LITERAL,strip_comments,code_only
from format_recovery import definition_span
from audit_source import function_body


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';p=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(p.read_text())
    metadata={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    targets={r['name']:{int(r['address'],16)} for r in csv.DictReader((cache/'decompiled-mana-frontends/export.tsv').open(),delimiter='\t')};targets['count_mana']={0x26a9b30,0x472400};targets['choose_mana_all_one_color']={0x266ae20};assert set(targets)==set(MANA_API_NAMES)
    image=Image((cache/'2022-ManalinkEh.dll').read_bytes());magic=Image((cache/'2022-Magic.exe').read_bytes())
    native=magic.pe.get_data(0x472400-magic.pe.OPTIONAL_HEADER.ImageBase,5);stub=image.pe.get_data(0x20103e9-0x2000000,5)
    assert native.hex()=='e9e4dfb901' and stub.hex()=='e942976900'
    assert 0x472400+5+struct.unpack('<i',native[1:])[0]==0x20103e9 and 0x20103e9+5+struct.unpack('<i',stub[1:])[0]==0x26a9b30
    route=dict(executable_sha256=hashlib.sha256(magic.data).hexdigest(),entry=dict(address='472400',bytes=native.hex()),stub=dict(address='20103e9',bytes=stub.hex()),target='26a9b30')
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);pattern=re.compile(r'\b('+'|'.join(MANA_API_NAMES)+r')(?=\s*\()');sources={};changes=[];skipped=[]
    storage_path=root/'docs/recovery-evidence/stack-storage.json';storage=json.loads(storage_path.read_text())
    for f in manifest['accepted_functions']:
        a=f['address'];raw=(cache/'decompiled-abi'/(a+'.c')).read_text()
        if not pattern.search(code_only(strip_comments(raw))):continue
        source=f['source']
        if source not in sources:sources[source]=(root/source).read_bytes().decode('latin1').replace('\r\n','\n')
        text=sources[source];at=text.index('/* ManalinkEh 2022: 0x'+a+';');_,begin,end=definition_span(text[at:],f['name']);chunk=text[at+begin:at+end]
        masked=re.sub(LITERAL+r'|/\*.*?\*/|//[^\r\n]*',lambda m:' '*len(m[0]),chunk,flags=re.S);matches=list(pattern.finditer(masked))
        if not matches:continue
        start=int(a,16);instructions=list(decoder.disasm(image.pe.get_data(start-0x2000000,int(metadata[a]['high_pc'],16)-start),start));callees=[]
        for name in sorted({m[1] for m in matches}):
            sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{address:x}') for i in instructions for address in targets[name] if i.mnemonic in {'call','jmp'} and i.op_str==hex(address)]
            if not sites or not re.search(r'\b'+name+r'\s*\(',code_only(strip_comments(raw))):
                skipped.append(dict(address=a,name=f['name'],callee=name,reason='No canonical direct call or tail jump'));break
            callees.append(dict(original=name,replacement=name+'_2022',targets=[f'{v:x}' for v in sorted(targets[name])],calls=sites,source_occurrences=sum(m[1]==name for m in matches)))
        else:
            replacement=chunk
            for match in reversed(matches):replacement=replacement[:match.start()]+match[1]+'_2022'+replacement[match.end():]
            old=function_body(chunk,f['name']);body=function_body(replacement,f['name']);old_hash=hashlib.sha256(old.encode()).hexdigest();new_hash=hashlib.sha256(body.encode()).hexdigest()
            if f.get('reconstruction_body_sha256'):assert f['reconstruction_body_sha256']==old_hash
            sources[source]=text[:at+begin]+replacement+text[at+end:]
            previous_mana=f.get('mana_call_migration')
            if previous_mana:
                assert previous_mana['body_sha256']==old_hash
                assert not {c['original'] for c in previous_mana['callees']}&{c['original'] for c in callees}
                previous_mana.setdefault('additional_passes',[]).append(dict(previous_body_sha256=old_hash,body_sha256=new_hash))
                previous_mana['callees'].extend(callees);previous_mana['body_sha256']=new_hash
            else:
                f['mana_call_migration']=dict(previous_body_sha256=old_hash,body_sha256=new_hash,callees=callees,scope='Only recovered bodies; rename canonical producer/count calls with unchanged arguments and control flow. Native count-mana route independently verified in the matching executable and DLL.')
            f['reconstruction_body_sha256']=new_hash;f['reconstruction']=f.get('reconstruction','')+' Verified 2022 mana call migration.'
            if f.get('extension_flag_call_migration'):
                previous=f['extension_flag_call_migration'];assert previous['body_sha256']==old_hash;previous['mana_migration_previous_body_sha256']=old_hash;previous['body_sha256']=new_hash
            if f.get('stack_storage_reconstruction'):
                previous=f['stack_storage_reconstruction'];assert previous['body_sha256']==old_hash;previous['mana_migration_previous_body_sha256']=old_hash;previous['body_sha256']=new_hash
                saved=storage['functions'][a];assert saved['body_sha256']==old_hash;saved['mana_migration_previous_body_sha256']=old_hash;saved['body_sha256']=new_hash
            changes.append(dict(name=f['name'],source=source,address=a,calls=sum(c['source_occurrences'] for c in callees)))
    report=dict(applied=args.apply,functions=len(changes),call_expressions=sum(f['calls'] for f in changes),source_files=len({f['source'] for f in changes}),skipped=skipped)
    if args.apply:
        for source in {f['source'] for f in changes}:(root/source).write_bytes(sources[source].replace('\n','\r\n').encode('latin1'))
        p.write_text(json.dumps(manifest,indent=2));storage_path.write_text(json.dumps(storage,indent=2))
        p=root/'docs/recovery-evidence/mana-callers.json'
        prior=json.loads(p.read_text()) if p.exists() else dict(native_count_route=route,functions=[],skipped=[])
        assert prior['native_count_route']==route
        by_address={f['address']:f for f in prior['functions']}
        for change in changes:
            if change['address'] in by_address:by_address[change['address']]['calls']+=change['calls']
            else:by_address[change['address']]=change
        prior['functions']=list(by_address.values());prior['skipped']=skipped;p.write_text(json.dumps(prior,indent=2))
    (cache/'mana-migration-report.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))


if __name__=='__main__':main()
