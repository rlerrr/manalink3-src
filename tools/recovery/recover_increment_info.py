"""Complete the information API with its 32-bit data increment worker."""
import argparse
import csv
import hashlib
import json
import subprocess
from pathlib import Path

from audit_source import function_body,literal_tokens

BODY='''int increment_data(card_instance_t *inst, int cid)
{
  inst_info_t initial = inst_info(inst, cid);
  inst_info_t second = inst_info(inst, cid);
  return set_inst_info(inst, cid, initial.data + 1u, second.data2);
}
'''


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';address='26b37f0';name='increment_data'
    raw=(cache/'decompiled-abi'/(address+'.c')).read_text()
    assert raw.count('= inst_info(inst,cid)')==2 and 'iVar3.data + 1' in raw and 'iVar4.data2' in raw
    assert literal_tokens(raw)==literal_tokens(BODY)
    row=next(r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc']=='0x'+address)
    source=root/'functions/deck.c';old=source.read_bytes();addition=f'\n\n/* ManalinkEh 2022: 0x{address}; original functions/deck.c:{row["line"]}. */\n'+BODY
    candidate=cache/'increment-info-check.c';candidate.write_bytes(old+addition.encode())
    result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99',
        '-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type',
        '-Werror=uninitialized','-I',str(root),str(candidate)],capture_output=True,text=True)
    (cache/'increment-info-check.log').write_text(result.stderr);result.check_returncode()
    path=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(path.read_text());assert address not in {f['address'] for f in manifest['accepted_functions']}
    entry=dict(name=name,address=address,source='functions/deck.c',line=row['line'],static=False,
        prototype=BODY[:BODY.index('{')].strip()+';',raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),recovered_literals=[],
        reconstruction='Preserve two information reads: increment the first returned 32-bit data word and retain the second returned data2 halfword. Unsigned addition retains x86 wraparound.',
        reconstruction_body_sha256=hashlib.sha256(function_body(BODY,name).encode()).hexdigest())
    if args.apply:
        source.write_bytes(old.rstrip()+addition.replace('\n','\r\n').encode())
        header=root/'manalink.h';data=header.read_bytes();at=data.rfind(b'#ifdef __cplusplus')
        header.write_bytes(data[:at]+('#ifndef SHANDALAR\r\n/* October 2022 information-data increment. */\r\n'+entry['prototype']+'\r\n#endif\r\n\r\n').encode()+data[at:])
        manifest['accepted_functions'].append(entry);manifest['accepted']=len(manifest['accepted_functions']);path.write_text(json.dumps(manifest,indent=2))
    print(json.dumps(dict(applied=args.apply,helper=name),indent=2))


if __name__=='__main__':main()
