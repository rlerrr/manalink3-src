"""Recover the five-argument blue/red Replicate specialization with DWARF locations."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path
from elftools.dwarf.locationlists import LocationParser,LocationExpr,BaseAddressEntry
from inspect_release import Image,value,inherited_value
from promote_recovery import clean,UNRESOLVED,project_replicate_count,strip_comments
from recover_literals import recover_strings
from audit_source import function_body


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--prepare',action='store_true');parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(data);dwarf=debug.dwarf();locations=LocationParser(dwarf.location_lists())
    parameters=[]
    for cu in dwarf.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='cards/guildpact.c':continue
        for die in cu.get_top_DIE().iter_children():
            if value(die,'DW_AT_low_pc')!=0x252b8d0:continue
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
                            start=entry.begin_offset if entry.is_absolute else base+entry.begin_offset
                            end=entry.end_offset if entry.is_absolute else base+entry.end_offset
                            if start<=0x252b8d0<end:expression=entry.loc_expr;break
                parameters.append(dict(name=inherited_value(p,'DW_AT_name'),location=expression,constant=value(p,'DW_AT_const_value')))
        break
    expected={'player':[80],'card':[82],'cless':[81],'blue':[145,0],'red':[145,4]}
    assert {p['name']:p['location'] for p in parameters if p['name'] in expected}==expected
    assert {p['name']:p['constant'] for p in parameters if p['name'] in {'black','green','white'}}=={'black':0,'green':0,'white':0}
    assert next(p for p in parameters if p['name']=='event')['location']==[250,139,29,12,0,159]
    proof=dict(debug_sha256=hashlib.sha256(data).hexdigest(),parameters=parameters,
        scope='Five physical arguments: EAX player, EDX card, ECX generic cost, stack blue/red costs. Black/green/white are constant zero; event has only an entry-value debug expression and no ABI storage.',
        integration_pending='Old Replicate callers and the engine copy trigger remain downstream work.')
    image=Image((cache/'2022-ManalinkEh.dll').read_bytes())
    checks={'252b8d5':'89ce','252b8d8':'89c3','252b8dd':'8955f0','252b94f':'8b450c',
        '252b95e':'8b4508','252b98e':'8b472c','252b991':'8a909f000000','252b998':'88909f000000'}
    for address,expected_bytes in checks.items():assert image.pe.get_data(int(address,16)-0x2000000,len(bytes.fromhex(expected_bytes))).hex()==expected_bytes
    proof['machine_checks']=checks;proof['machine_sha256']=hashlib.sha256(image.pe.get_data(0x52b8d0,0x110)).hexdigest()
    name='replicate_blue_red_2022_impl';source='cards/guildpact.c'
    if args.prepare:
        (cache/'replicate-clone-interfaces.tsv').write_text('252b8d0\t'+name+'\tvoid\t__regparm3\tint:player\tint:card\tint:cless\tint:blue\tint:red\n')
        (cache/'export-request-replicate.tsv').write_text('252b8d0\t'+source+'\treplicate\t252b9e0\n')
        assert re.search(r'(?m)^_do_dialog\s*=\s*0x471eb0;', (root/'manalink.lds').read_text(),re.I)
        row=next(l for l in (cache/'engine-interfaces.tsv').read_text().splitlines() if l.startswith('0x471eb0\tdo_dialog\t'))
        assert row.split('\t')[2:]==['int','int','int','int','int','int','char *','int']
        (cache/'replicate-runtime-interfaces.tsv').write_text(row+'\n')
        print('Prepared verified five-argument specialization and dialog signature');return
    raw=(cache/'decompiled-replicate/252b8d0.c').read_text()
    resolved,literals=recover_strings(raw,image,{})
    body=clean(resolved,use_keyword_accessors_2022=True)
    assert name in body
    _,projections=project_replicate_count(strip_comments(raw));assert len(projections)==2
    body='static '+body;assert not UNRESOLVED.search(body)
    body=subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe','-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],input=body.encode(),capture_output=True,check=True).stdout.decode().replace('\r\n','\n')
    path=root/source;candidate=path.read_bytes().decode('latin1').replace('\r\n','\n')+'\n\n/* ManalinkEh 2022: 0x252b8d0; original cards/guildpact.c:70. */\n'+body
    scratch=cache/'replicate-worker-check.c';scratch.write_bytes(candidate.encode('latin1'))
    result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(root),str(scratch)],capture_output=True,text=True)
    (cache/'replicate-worker-check.log').write_text(result.stderr);result.check_returncode()
    if args.apply:
        p=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(p.read_text());assert '252b8d0' not in {f['address'] for f in manifest['accepted_functions']}
        path.write_bytes(candidate.replace('\n','\r\n').encode('latin1'))
        manifest['accepted_functions'].append(dict(name=name,original_name='replicate',address='252b8d0',source=source,line='70',static=True,prototype=body[:body.index('{')].strip()+';',raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),recovered_literals=literals,reconstruction=proof['scope'],replicate_worker_evidence=proof,reconstruction_body_sha256=hashlib.sha256(function_body(body,name).encode()).hexdigest()))
        manifest['accepted']=len(manifest['accepted_functions']);p.write_text(json.dumps(manifest,indent=2))
        shutil.copyfile(cache/'decompiled-replicate/252b8d0.c',cache/'decompiled-abi/252b8d0.c')
        p=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(p.read_text());interfaces['replicate_specialization']=proof;p.write_text(json.dumps(interfaces,indent=2))
    print(json.dumps(dict(applied=args.apply,new_helpers=1)))


if __name__=='__main__':main()
