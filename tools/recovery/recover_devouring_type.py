"""Recover the private devour selector's verified grammatical string pointers."""
import argparse
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path

from inspect_release import Image,value
from promote_recovery import clean,rewrite_argument
from recover_literals import readonly_string,byte_literal
from audit_source import function_body,literal_tokens


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';address='222a950';name='devouring_type'
    raw=(cache/'decompiled-abi'/(address+'.c')).read_text();debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert any(n=='_'+name and storage==3 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[s-1].VirtualAddress+off==int(address,16)
        for n,off,s,kind,storage in debug.functions())
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes())
    # This caller passes literal zero; TYPETEXT_0 is Ghidra's synthetic
    # rendering of the callee's enum type, absent from this source unit.
    assert runtime.pe.get_data(0x222a9ae-runtime.pe.OPTIONAL_HEADER.ImageBase,8).hex()=='c744240c00000000'
    assert runtime.pe.get_data(0x222a9cb-runtime.pe.OPTIONAL_HEADER.ImageBase,5).hex()=='e800bc4600'
    row=next(r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc']=='0x'+address)
    end=row['high_pc'];constants=[];normalized=raw
    for symbol,at,site,code in [('_rdata',0x2736c1c,0x222ab60,'ba1c6c7302'),('UNK_02736c1e',0x2736c1e,0x222a9e8,'ba1e6c7302')]:
        assert runtime.pe.get_data(site-runtime.pe.OPTIONAL_HEADER.ImageBase,len(bytes.fromhex(code))).hex()==code
        literal=readonly_string(runtime,at);assert literal==(b'n' if symbol=='_rdata' else b'')
        token='&'+symbol;assert normalized.count(token)==1
        constants.append(dict(symbol=symbol,address=hex(at),bytes=literal.hex(),raw_occurrences=1,function_end=end,
            materializations=[dict(address=f'{site:x}',bytes=code)]));normalized=normalized.replace(token,byte_literal(literal))
    normalized=normalized.replace('undefined *puVar7;','const char *puVar7;')
    normalized=rewrite_argument(normalized,'type_text',4,3,{'TYPETEXT_0':'0'})
    body='static '+clean(normalized)
    body=subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
        '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
        input=body.encode(),capture_output=True,check=True).stdout.decode().replace('\r\n','\n')
    assert literal_tokens(normalized)==literal_tokens(body)
    source=root/'cards/modern_horizons_2.c';old=source.read_bytes()
    addition=f'\n\n/* ManalinkEh 2022: 0x{address}; original cards/modern_horizons_2.c:{row["line"]}. */\n'+body
    candidate=cache/'devour-selector-check.c';candidate.write_bytes(old+addition.encode())
    result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99',
        '-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type',
        '-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(root),str(candidate)],capture_output=True,text=True)
    (cache/'devour-selector-check.log').write_text(result.stderr);result.check_returncode()
    path=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(path.read_text());assert address not in {f['address'] for f in manifest['accepted_functions']}
    entry=dict(name=name,address=address,source='cards/modern_horizons_2.c',line=row['line'],static=True,
        prototype=body[:body.index('{')].strip()+';',raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),recovered_literals=[],
        reconstructed_literal_constants=constants,reconstruction='Private devour selector: restore const string-pointer local and two grammatical suffixes from exact immediate loads. Selection and sacrifice logic unchanged.',
        interface_evidence=dict(call='type_text',argument=3,value=0,load_address='222a9ae',load_bytes='c744240c00000000',call_address='222a9cb',call_bytes='e800bc4600'),
        reconstruction_body_sha256=hashlib.sha256(function_body(body,name).encode()).hexdigest())
    if args.apply:
        source.write_bytes(old.rstrip()+addition.replace('\n','\r\n').encode());manifest['accepted_functions'].append(entry);manifest['accepted']=len(manifest['accepted_functions']);path.write_text(json.dumps(manifest,indent=2))
    print(json.dumps(dict(applied=args.apply,helper=name,literal_references=2),indent=2))


if __name__=='__main__':main()
