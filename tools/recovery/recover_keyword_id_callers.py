"""Route already recovered literal card-ID queries through the recovered 2022 API."""
import argparse
import csv
import hashlib
import json
import re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from inspect_release import Image
from audit_source import function_body


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';path=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(path.read_text())
    metadata={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    image=Image((cache/'2022-ManalinkEh.dll').read_bytes());sources={};repairs=[]
    targets={'get_special_abilities':0x26df840,'check_for_special_ability':0x26df890}
    for function in manifest['accepted_functions']:
        source=function['source']
        if source not in sources:sources[source]=(root/source).read_bytes().decode('latin1').replace('\r\n','\n')
        old=function_body(sources[source],function['name']);body=old;changes=[]
        raw=(cache/'decompiled-abi'/(function['address']+'.c')).read_text()
        for name,target in targets.items():
            pattern=r'\b'+name+r'(?=\s*\(\s*-1\s*,)'
            if not re.search(pattern,body):continue
            count=len(re.findall(pattern,body));assert len(re.findall(pattern,raw))==count
            start=int(function['address'],16);end=int(metadata[function['address']]['high_pc'],16)
            instructions=list(Cs(CS_ARCH_X86,CS_MODE_32).disasm(image.pe.get_data(start-0x2000000,end-start),start))
            sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{target:x}') for i in instructions if i.mnemonic=='call' and i.op_str==hex(target)]
            assert len(sites)>=count
            body=re.sub(pattern,name+'_2022',body)
            changes.append(dict(original=name,replacement=name+'_2022',literal_card_id_calls=count,machine_calls=sites))
        if body==old:continue
        assert sources[source].count(old)==1;sources[source]=sources[source].replace(old,body,1)
        function['keyword_id_query_repair']=dict(changes=changes,previous_body_sha256=hashlib.sha256(old.encode()).hexdigest(),scope='Only recovered literal player=-1 queries: use the 2022 card-ID path rather than the original live-instance-only API.')
        function['reconstruction_body_sha256']=hashlib.sha256(body.encode()).hexdigest()
        function['reconstruction']=function.get('reconstruction','')+' Verified literal card-ID query interface repair.'
        repairs.append(dict(name=function['name'],source=source,address=function['address']))
    assert len(repairs)==2
    if args.apply:
        for source in {f['source'] for f in repairs}:(root/source).write_bytes(sources[source].replace('\n','\r\n').encode('latin1'))
        path.write_text(json.dumps(manifest,indent=2))
    print(json.dumps(dict(applied=args.apply,repairs=repairs),indent=2))


if __name__=='__main__':main()
