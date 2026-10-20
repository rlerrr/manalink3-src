"""Recover the pure card-ID keyword lookup and both 2022 query front ends."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import struct
import subprocess
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_MEM, X86_OP_IMM, X86_REG_EBP, X86_REG_ESP
from inspect_release import Image
from promote_recovery import clean, strip_comments, code_only, UNRESOLVED
from audit_source import function_body


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    image=Image((cache/'2022-ManalinkEh.dll').read_bytes())
    assert hashlib.sha256(image.data).hexdigest()=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    metadata={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    names={'26d7530':'has_sp_key_by_csvid_2022_impl','26df840':'get_special_abilities_2022','26df890':'check_for_special_ability_2022'}
    directories={a:cache/('decompiled-abi' if a=='26d7530' else 'decompiled-keyword-foundation') for a in names}
    raw={a:(directories[a]/(a+'.c')).read_text() for a in names}
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    proof=dict(scope='Retain the complete integer-only lookup control flow. Two Ghidra CONCAT11 expressions are recovered from OR BH,1 as full-word bit-preserving OR operations; unsigned one-bit shifts retain x86 bit patterns.',
        integer_lowering_sites=[],jump_tables=[],functions={},integration_pending='Remaining earlier keyword callers and engine refresh/writer migration require downstream integration.')
    for a in names:
        start=int(a,16);length=int(metadata[a]['high_pc'],16)-start
        machine=image.pe.get_data(start-image.pe.OPTIONAL_HEADER.ImageBase,length)
        instructions=list(decoder.disasm(machine,start))
        assert sum(i.size for i in instructions)==length
        proof['functions'][a]=dict(bytes=length,instructions=len(instructions),machine_sha256=hashlib.sha256(machine).hexdigest())
        if a!='26d7530':continue
        assert not any(i.mnemonic=='call' for i in instructions)
        boundaries={i.address for i in instructions}
        for index,ins in enumerate(instructions):
            if ins.mnemonic not in ('lea','nop'):
                for operand in ins.operands:
                    if operand.type==X86_OP_MEM:
                        if ins.mnemonic=='jmp':
                            assert operand.mem.base==0 and operand.mem.scale==4
                            comparison,guard=instructions[index-2:index]
                            assert comparison.mnemonic=='cmp' and comparison.operands[0].reg==operand.mem.index
                            assert comparison.operands[1].type==X86_OP_IMM and guard.mnemonic=='ja'
                            count=comparison.operands[1].imm+1;assert 1<=count<=128
                            table=operand.mem.disp;data=image.pe.get_data(table-image.pe.OPTIONAL_HEADER.ImageBase,count*4)
                            targets=struct.unpack('<'+'I'*count,data);assert set(targets)<=boundaries
                            section=next(s for s in image.pe.sections if s.VirtualAddress<=table-image.pe.OPTIONAL_HEADER.ImageBase<s.VirtualAddress+s.Misc_VirtualSize)
                            assert not section.Characteristics & 0x80000000
                            proof['jump_tables'].append(dict(address=f'{table:x}',bytes=data.hex(),entries=count,targets=[f'{t:x}' for t in targets],dispatch=f'{ins.address:x}',bound=f'{comparison.address:x}',guard=f'{guard.address:x}'))
                        else:
                            assert operand.mem.base in (X86_REG_EBP,X86_REG_ESP) and operand.mem.index==0
            if ins.mnemonic.startswith('j'):
                if ins.operands[0].type==X86_OP_IMM:assert ins.operands[0].imm in boundaries
                else:assert ins.mnemonic=='jmp' and ins.operands[0].type==X86_OP_MEM
            if ins.address in (0x26d7962,0x26db12b):
                assert ins.bytes.hex()=='80cf01' and ins.mnemonic=='or' and ins.op_str=='bh, 1'
                proof['integer_lowering_sites'].append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),instruction='or bh, 1'))
        assert len(proof['integer_lowering_sites'])==2
        assert len(proof['jump_tables'])==7
        assert any(n=='_has_sp_key_by_csvid' and storage==3 and image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[s-1].VirtualAddress+v==start for n,v,s,t,storage in image.functions())
    lookup=raw['26d7530']
    lookup,count=re.subn(r'\(uint\)CONCAT11\(1,\(char\)uVar2\)', '(uVar2 | 0x100U)',lookup)
    assert count==2
    lookup,shifts=re.subn(r'\b1 <<', '1U <<', lookup)
    assert shifts>0
    proof['unsigned_one_bit_shifts']=shifts
    proof['functions']['26d7530'].update(jump_tables=proof['jump_tables'], integer_lowering_sites=proof['integer_lowering_sites'], unsigned_one_bit_shifts=shifts)
    bodies={};prototypes={}
    for a,name in names.items():
        body=lookup if a=='26d7530' else raw[a]
        body=clean(body,use_keyword_accessors_2022=True)
        body=re.sub(r'\bhas_sp_key_by_csvid\b','has_sp_key_by_csvid_2022_impl',body)
        if a=='26d7530':body='static '+body
        assert not UNRESOLVED.search(code_only(body)) and not re.search(r'\._\d|\.field_',body)
        body=subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe','-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],input=body.encode(),capture_output=True,check=True).stdout.decode().replace('\r\n','\n')
        bodies[a]=body;prototypes[a]=body[:body.index('{')].strip()+';'
    source='functions/abilities.c';path=root/source
    candidate=path.read_bytes().decode('latin1').replace('\r\n','\n')
    for a,body in bodies.items():candidate+=f"\n\n/* ManalinkEh 2022: 0x{a}; original {source}:{metadata[a]['line']}. */\n"+body
    check=cache/'keyword-lookup-candidate.c';check.write_bytes(candidate.encode('latin1'))
    result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(root),str(check)],capture_output=True,text=True)
    (cache/'keyword-lookup-check.log').write_text(result.stderr);result.check_returncode()
    if args.apply:
        p=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(p.read_text())
        assert not set(names)&{f['address'] for f in manifest['accepted_functions']}
        path.write_bytes(candidate.replace('\n','\r\n').encode('latin1'))
        h=root/'manalink.h';header=h.read_bytes().decode('latin1').replace('\r\n','\n');at=header.rfind('#ifdef __cplusplus')
        addition='/* October 2022 card-ID and live-instance keyword queries. */\n#ifndef SHANDALAR\n'+'\n'.join(prototypes[a] for a in names if a!='26d7530')+'\n#endif\n\n'
        h.write_bytes((header[:at]+addition+header[at:]).replace('\n','\r\n').encode('latin1'))
        for a,name in names.items():
            manifest['accepted_functions'].append(dict(name=name,original_name=metadata[a]['name'],address=a,source=source,line=metadata[a]['line'],static=a=='26d7530',prototype=prototypes[a],raw_sha256=hashlib.sha256(raw[a].encode()).hexdigest(),recovered_literals=[],reconstruction=proof['scope'],keyword_lookup_evidence=proof['functions'][a],reconstruction_body_sha256=hashlib.sha256(function_body(bodies[a],name).encode()).hexdigest()))
        manifest['accepted']=len(manifest['accepted_functions']);p.write_text(json.dumps(manifest,indent=2))
        exports=cache/'decompiled-abi/export.tsv';rows=list(csv.DictReader(exports.open(),delimiter='\t'));known={r['address'] for r in rows}
        extra=[r for r in csv.DictReader((cache/'decompiled-keyword-foundation/export.tsv').open(),delimiter='\t') if r['address'] in names and r['address'] not in known]
        assert {r['address'] for r in extra}=={'26df840','26df890'}
        rows.extend(extra)
        with exports.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
        for a in ('26df840','26df890'):shutil.copyfile(directories[a]/(a+'.c'),exports.parent/(a+'.c'))
        p=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(p.read_text());interfaces['keyword_lookup_2022']=proof;p.write_text(json.dumps(interfaces,indent=2))
    print(json.dumps(dict(applied=args.apply,new_helpers=3,lookup_instructions=proof['functions']['26d7530']['instructions'],unsigned_shifts=shifts,external_calls=0)))


if __name__=='__main__':main()
