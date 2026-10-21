"""Upgrade two recovered cast-trigger entries to native tests; retain legacy bodies."""
import argparse,copy,csv,hashlib,json,re,shutil,subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image
from promote_recovery import clean,strip_comments,UNRESOLVED
from recover_test_consumers import read,rename,format_c
from audit_source import function_body
from format_recovery import definition_span

ADDRESSES=['2684f40','26a8120']
CONTROL_SITES = [
    dict(address='2685004',bytes='8b402c',role='extension pointer at instance offset 44'),
    dict(address='2685007',bytes='8b80b8000000',role='raw copied-spell word at extension offset 184'),
    dict(address='268500d',bytes='85c0',role='test raw signed word'),
    dict(address='268500f',bytes='790b',role='skip copy rejection when sign bit is clear'),
    dict(address='2685014',bytes='85c0',role='test allow_copies'),
    dict(address='2685016',bytes='0f8451ffffff',role='return when copied and copies disallowed'),
    dict(address='268505b',bytes='6683bbaa00000001',role='16-bit not_me field at test offset 170'),
]


def sha(data):return hashlib.sha256(data).hexdigest()


def main():
    ap=argparse.ArgumentParser(__doc__);ap.add_argument('--apply',action='store_true');args=ap.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'spell_trigger_native_2022' not in interfaces
    by_address={f['address']:f for f in m['accepted_functions']};old={a:copy.deepcopy(by_address[a]) for a in ADDRESSES}
    assert not any(f.get('reconstruction_body_sha256') for f in old.values())
    names={a:old[a]['name']+'_2022' for a in ADDRESSES};mapping={old[a]['name']:n for a,n in names.items()}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    source=read(root/'functions/functions.c');header=read(root/'manalink.h');fragments=[];entries=[];backups={};bodies={}
    proof=dict(functions={},previous_records=old,legacy_fragments=[],control_sites=CONTROL_SITES,test_bytes=216,not_me_offset=170,copied_spell_word=dict(extension_offset=184,mask=0x80000000),
        scope='Full native spell-cast trigger core/public wrapper with 216-byte tests and raw bit-31 copy filtering. Retain both earlier 208-byte source bodies under their legacy names as recorded compatibility fragments.')
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']};targets.update({int(a,16):n for a,n in names.items()})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    for address,name in names.items():
        previous=old[address];marker=f'/* ManalinkEh 2022: 0x{address};';at=source.index(marker);marker_end,start,end=definition_span(source[at:],previous['name'])
        old_marker=source[at:at+marker_end];payload=source[at+marker_end:at+end].strip()
        start_marker=f'/* Retained legacy spell-trigger interface: {previous["name"]} @ 0x{address}. */'
        end_marker=f'/* End retained legacy spell-trigger interface: {address}. */'
        old_raw=(cache/'decompiled-abi'/(address+'.c')).read_text();assert sha(old_raw.encode())==previous['raw_sha256']
        backup='decompiled-support/'+address+'-legacy-spell-trigger.c';backups[backup]=old_raw
        fragment=dict(source='functions/functions.c',original_function=previous['name'],address=address,raw_export=backup,raw_sha256=sha(old_raw.encode()),start_marker=start_marker,end_marker=end_marker,body_sha256=sha(payload.encode()),scope='Retained earlier 208-byte test interface; excluded from native implementation counts.')
        fragments.append(fragment)
        source=source[:at]+start_marker+source[at+marker_end:at+end]+'\n'+end_marker+source[at+end:]
        raw=(cache/'decompiled-phyrexian-consumers'/(address+'.c')).read_text() if address=='26a8120' else (cache/'decompiled-phyrexian-consumer-dependencies'/(address+'.c')).read_text()
        original=strip_comments(raw);text=original;changes=[]
        if address=='2684f40':
            expr='(((pcVar2->field_21).ext)->special_flags2 < SF2_0)';assert text.count(expr)==1
            text=text.replace(expr,'((((uint32_t)((pcVar2->field_21).ext)->special_flags2) & 0x80000000U) != 0)')
            changes=[dict(original=original,replacement=text,occurrences=1,kind='native raw copied-spell bit 31',machine_sites=[])]
        text,_=rename(text,mapping)
        body=clean(text,overlap,use_test_api_2022=True,use_extension_flags_2022=True,use_keyword_accessors_2022=True)
        assert not UNRESOLVED.search(body) and '::' not in body
        if previous['static']:body='static '+body
        body=format_c(body);bodies[address]=body
        native=runtime.pe.get_data(int(address,16)-0x2000000,int(meta[address]['high_pc'],16)-int(address,16));instructions=list(decoder.disasm(native,int(address,16)));calls=[]
        for i in instructions:
            if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm in targets:
                destination=targets[i.operands[0].imm];assert re.search(r'\b'+destination+r'\s*\(',body),(address,destination)
                calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{i.operands[0].imm:x}',name=destination))
        detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,canonical_calls=calls)
        proof['functions'][address]=detail
        entries.append(dict(name=name,address=address,source=previous['source'],line=previous['line'],static=previous['static'],prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],spell_trigger_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    proof['legacy_fragments']=fragments
    source+='\n\n#ifndef SHANDALAR\n/* Recovered native October 2022 spell-cast trigger interfaces. */\n'+'\n'.join(f'\n/* ManalinkEh 2022: 0x{a}; original functions/functions.c:{old[a]["line"]}. */\n'+bodies[a] for a in ADDRESSES)+'\n#endif\n'
    at=header.rfind('#ifdef __cplusplus');header=header[:at]+'/* October 2022 native spell-trigger API. */\n#ifndef SHANDALAR\n'+next(e['prototype'] for e in entries if not e['static'])+'\n#endif\n\n'+header[at:]
    scratch=cache/'spell-trigger-native-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'));(scratch/'functions.c').write_bytes(source.encode('latin1'))
    r=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(scratch/'functions.c')],capture_output=True,text=True)
    (scratch/'functions.log').write_text(r.stderr)
    if r.returncode:print(r.stderr)
    r.check_returncode()
    if args.apply:
        citations=m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not any(c['address'] in ADDRESSES for c in citations)
        (root/'functions/functions.c').write_bytes(source.replace('\n','\r\n').encode('latin1'));(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        for backup,text in backups.items():p=cache/backup;p.parent.mkdir(exist_ok=True);p.write_text(text)
        for entry in entries:
            by_address[entry['address']]=entry
            directory='decompiled-phyrexian-consumers' if entry['address']=='26a8120' else 'decompiled-phyrexian-consumer-dependencies'
            shutil.copyfile(cache/directory/(entry['address']+'.c'),cache/'decompiled-abi'/(entry['address']+'.c'))
        m['accepted_functions']=list(by_address.values());m['source_fragments']+=fragments;mp.write_text(json.dumps(m,indent=2));interfaces['spell_trigger_native_2022']=proof;ip.write_text(json.dumps(interfaces,indent=2))
    (cache/'spell-trigger-native-plan.json').write_text(json.dumps(proof,indent=2));print(json.dumps(dict(applied=args.apply,new_implementations=0,upgraded_native_entries=2,retained_legacy_interfaces=2)))


if __name__=='__main__':main()
