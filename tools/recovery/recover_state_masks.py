"""Prepare scoped state-mask names backed by each caller's DWARF and instructions."""
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path

import capstone
from inspect_release import Image, value, inherited_value
from export_abi import type_of
from promote_recovery import strip_comments


NAMES = {2:'STATE_IN_PLAY',4:'STATE_ATTACKING',8:'STATE_BLOCKING',16:'STATE_TAPPED',
         32:'STATE_INVISIBLE',0x100000:'STATE_CANNOT_TARGET',0x200000:'STATE_TARGETTED',
         0x400000:'STATE_POWER_STRUGGLE',0x800000:'STATE_OUBLIETTED'}
PATTERN = re.compile(r'\b\w+->state\s*[&|]\s*(0x[0-9a-fA-F]+)\b')


def main():
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    manifest=json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text())
    accepted={f['address'] for f in manifest['accepted_functions']}
    requests={};wanted={}
    for row in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t'):
        if row['address'] in accepted or row['status']!='ok' or not row['source'].endswith('.c'):continue
        raw=(cache/'decompiled-abi'/(row['address']+'.c')).read_text();replacements=[]
        for match in PATTERN.finditer(strip_comments(raw)):
            mask=int(match[1],16)
            if not 0x400000<=mask<0xa00000:continue
            bits=[b for b in sorted(NAMES,reverse=True) if b&mask]
            if sum(bits)!=mask:continue
            expression=' | '.join(NAMES[b] for b in bits)
            if len(bits)>1:expression='('+expression+')'
            replacements.append(dict(original=match[0],replacement=match[0][:match.start(1)-match.start()]+expression,
                mask=mask,bits={NAMES[b]:b for b in bits}))
        if replacements:
            requests[row['address']]=dict(name=row['name'],source=row['source'],
                raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),replacements=replacements)
            wanted.setdefault(row['source'],set()).update(n for r in replacements for n in r['bits'])
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(data);units={}
    for cu in debug.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in wanted:continue
        enums={};member=None
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':
                for c in die.iter_children():
                    name=value(c,'DW_AT_name')
                    if name in wanted[source]:
                        entry=dict(value=value(c,'DW_AT_const_value'),die=hex(c.offset),enum_die=hex(die.offset))
                        assert name not in enums or enums[name]['value']==entry['value'];enums[name]=entry
            if die.tag=='DW_TAG_typedef' and value(die,'DW_AT_name')=='card_instance_t':
                structure=type_of(die)
                for c in structure.iter_children():
                    if c.tag=='DW_TAG_member' and value(c,'DW_AT_name')=='state':
                        typ=type_of(c)
                        while typ.tag=='DW_TAG_typedef':typ=type_of(typ)
                        assert value(c,'DW_AT_data_member_location')==8 and value(typ,'DW_AT_byte_size')==4
                        member=dict(offset=8,bytes=4,die=hex(c.offset))
        assert member and wanted[source]<=set(enums)
        for name in wanted[source]:assert enums[name]['value']==next(b for b,n in NAMES.items() if n==name)
        units[source]=dict(enumerators=enums,state_member=member)
        cu._dielist.clear();cu._diemap.clear()
    assert set(units)==set(wanted)
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes())
    functions={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc']}
    decoder=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32);decoder.detail=True
    rejected={}
    for address,request in list(requests.items()):
        row=functions[address];start=int(address,16);end=int(row['high_pc'],16)
        code=runtime.pe.get_data(start-runtime.pe.OPTIONAL_HEADER.ImageBase,end-start)
        instructions=list(decoder.disasm(code,start));proof=[]
        for replacement in request['replacements']:
            mask=replacement['mask'];sites=[]
            for ins in instructions:
                if ins.mnemonic not in {'and','or','test'}:continue
                for operand in ins.operands:
                    if operand.type==capstone.x86.X86_OP_IMM and operand.imm==mask:
                        sites.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex()));break
                    if (mask in {0x400000,0x800000} and operand.type==capstone.x86.X86_OP_IMM and operand.imm==mask>>16 and
                        any(o.type==capstone.x86.X86_OP_MEM and o.size==1 and o.mem.disp==10 for o in ins.operands)):
                        sites.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex()));break
            if not sites:
                rejected[address]=request['name'];break
            replacement['machine_sites']=sites
        else:
            request['machine_sha256']=hashlib.sha256(code).hexdigest();request['unit_evidence']=units[request['source']];continue
        del requests[address]
    aliases={n:b for r in requests.values() for replacement in r['replacements'] for n,b in replacement['bits'].items()}
    probe=cache/'state-mask-layout.c';probe.write_text('#include "manalink.h"\n#include <stddef.h>\n'+
        '_Static_assert(offsetof(card_instance_t,state)==8 && sizeof(((card_instance_t*)0)->state)==4,"state word");\n'+
        '\n'.join(f'_Static_assert({n} == {v}, "state mask");' for n,v in aliases.items()))
    result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99',
        '-fsyntax-only','-I',str(root),str(probe)],capture_output=True,text=True);result.check_returncode()
    path=root/'docs/recovery-evidence/state-masks.json'
    old=json.loads(path.read_text()) if path.exists() else dict(debug_sha256=hashlib.sha256(data).hexdigest(),functions={})
    assert old['debug_sha256']==hashlib.sha256(data).hexdigest()
    old['functions']={a:e for a,e in old['functions'].items() if a in accepted}
    old['functions'].update(requests)
    old['scope']='Only numeric bit masks applied directly to the verified four-byte state member. Preserve each bit and operator; never reinterpret numeric addresses or other fields.'
    path.write_text(json.dumps(old,indent=2))
    print(json.dumps(dict(prepared=len(requests),replacements=sum(len(r['replacements']) for r in requests.values()),rejected=rejected),indent=2))


if __name__=='__main__':main()
