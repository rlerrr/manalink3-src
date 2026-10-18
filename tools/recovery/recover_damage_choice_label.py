"""Recover Syr Carah's native dialog-prefix pointer as a typed string argument."""
import argparse
import csv
import hashlib
import json
import subprocess
from pathlib import Path

from inspect_release import Image
from promote_recovery import clean,rewrite_argument
from audit_source import function_body,literal_tokens


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';address='25d4b90';name='card_syr_carah_the_bold'
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());site=0x25d4d5e;code='c744240c348f7300'
    assert runtime.pe.get_data(site-runtime.pe.OPTIONAL_HEADER.ImageBase,8).hex()==code
    magic_bytes=(cache/'2022-Magic.exe').read_bytes();assert hashlib.sha256(magic_bytes).hexdigest()=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    magic=Image(magic_bytes);rva=0x738f34-magic.pe.OPTIONAL_HEADER.ImageBase
    segment=next(s for s in magic.pe.sections if s.VirtualAddress<=rva<s.VirtualAddress+s.Misc_VirtualSize)
    assert segment.Characteristics&0x40000000
    raw=(cache/'decompiled-abi'/(address+'.c')).read_text();assert 'load_text_2022(' in raw
    globals=[r[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines()
             if (r:=line.split('\t'))[2]=='global']
    assert 'trigger_condition' in globals
    body=clean(raw,overlapping_globals=globals);assert body.count('0x738f34')==1
    body=rewrite_argument(body,'scnprintf',7,3,{'0x738f34':'(const char *)0x738f34'})
    assert '(const char *)0x738f34' in body
    body=subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
        '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
        input=body.encode(),capture_output=True,check=True).stdout.decode().replace('\r\n','\n')
    assert literal_tokens(raw)==literal_tokens(body)
    row=next(r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc']=='0x'+address)
    source=root/'cards/throne_of_eldraine.c';original=source.read_bytes()
    addition=f'\n\n/* ManalinkEh 2022: 0x{address}; original cards/throne_of_eldraine.c:{row["line"]}. */\n'
    addition+='/* The prefix at 0x738f34 is an engine-owned runtime string buffer. */\n'+body
    candidate=cache/'damage-choice-label-check.c';candidate.write_bytes(original+addition.encode())
    result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99',
        '-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type',
        '-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(root),str(candidate)],capture_output=True,text=True)
    (cache/'damage-choice-label-check.log').write_text(result.stderr);result.check_returncode()
    path=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(path.read_text());assert address not in {f['address'] for f in manifest['accepted_functions']}
    entry=dict(name=name,address=address,source='cards/throne_of_eldraine.c',line=row['line'],static=False,
        prototype=body[:body.index('{')].strip()+';',raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),recovered_literals=[],
        reconstruction='Restore one native runtime-string pointer passed to scnprintf. Keep its address and live contents; use the recovered text loader and first native text row.',
        reconstruction_body_sha256=hashlib.sha256(function_body(body,name).encode()).hexdigest(),
        native_string_view=dict(address='738f34',section=segment.Name.rstrip(b'\0').decode(),load_address=f'{site:x}',
            load_bytes=code,call='scnprintf',argument=3,type='const char *',scope='Read view only; does not invent static text or initialization.'))
    if args.apply:
        source.write_bytes(original.rstrip()+addition.replace('\n','\r\n').encode());manifest['accepted_functions'].append(entry);manifest['accepted']=len(manifest['accepted_functions']);path.write_text(json.dumps(manifest,indent=2))
    print(json.dumps(dict(applied=args.apply,handler=name),indent=2))


if __name__=='__main__':main()
