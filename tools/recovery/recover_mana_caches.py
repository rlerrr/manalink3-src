"""Recover mana-modifier globals and both AI snapshot pairs from word/copy proofs."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_REG,X86_OP_MEM,X86_OP_IMM,X86_REG_EAX,X86_REG_ECX,X86_REG_EDX,X86_REG_ESI,X86_REG_EDI
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name


LIVE={'mana_colors_added':('int[2]',8,0x2797210),'mana_color_override':('int[2][8]',64,0x2797140),
      'mana_quantity_override':('int[2][8]',64,0x27971c0),'multi_mana_transmutation':('int[2][2]',16,0x2797180)}


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes())
    proof=dict(debug_sha256=hashlib.sha256(debug_data).hexdigest(),globals={},copies={},scope='Four live mana modifier tables and both complete AI snapshot pairs. Reset/refresh belongs to the separately recovered count_mana_2022 implementation.',serialization='All four arrays are in the target supplemental save record; full save-format integration remains deferred.')
    for cu in debug.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in {'functions/engine.c','functions/ai.c'}:continue
        for die in cu.get_top_DIE().iter_children():
            name=inherited_value(die,'DW_AT_name','');base=name.removeprefix('backup0_').removeprefix('backup_')
            if die.tag!='DW_TAG_variable' or base not in LIVE or not value(die,'DW_AT_location'):continue
            location=value(die,'DW_AT_location');assert location[0]==3 and len(location)==5
            at=int.from_bytes(bytes(location[1:]),'little');typ=type_name(type_of(die));size=LIVE[base][1];private=name!=base
            assert typ==(f'char[{size}]' if private else LIVE[base][0])
            if not private:assert at==LIVE[base][2]
            symbol=next(s for s in debug.symbols if s[0]=='_'+name);section=debug.pe.sections[symbol[2]-1]
            assert symbol[4]==(3 if private else 2) and section.Name.rstrip(b'\0')==b'.bss' and section.SizeOfRawData==0
            assert debug.pe.OPTIONAL_HEADER.ImageBase+section.VirtualAddress+symbol[1]==at
            proof['globals'][name]=dict(address=f'{at:x}',type=typ,bytes=size,source=source,private=private,zero_initialized=True,die=hex(die.offset))
        cu._dielist.clear();cu._diemap.clear()
    assert len(proof['globals'])==12
    rows={r['name']:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    snapshots=[('backup_data_for_ai','backup_',False),('restore_data_for_ai','backup_',True),('backup_data_for_ai_0','backup0_',False),('restore_data_for_ai_0','backup0_',True)]
    for function,prefix,restoring in snapshots:
        pairs={}
        for name,(typ,size,live) in LIVE.items():
            backup=int(proof['globals'][prefix+name]['address'],16);src,dst=(backup,live) if restoring else (live,backup)
            pairs.update({dst+i:src+i for i in range(0,size,4)})
        row=rows[function];start=int(row['low_pc'],16);machine=runtime.pe.get_data(start-0x2000000,int(row['high_pc'],16)-start);regs={};constants={};stores={};sites=[];blocks=[]
        for ins in decoder.disasm(machine,start):
            ops=ins.operands;new_source=None;new_constant=None
            if ins.mnemonic=='mov' and len(ops)==2:
                if ops[0].type==X86_OP_REG and ops[1].type==X86_OP_IMM:new_constant=(ops[0].reg,ops[1].imm&0xffffffff)
                if ops[0].type==X86_OP_REG and ops[1].type==X86_OP_MEM and not ops[1].mem.base and not ops[1].mem.index and ops[1].size==4 and ops[1].mem.disp in pairs.values():
                    new_source=(ops[0].reg,ops[1].mem.disp);sites.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),role='source word load'))
                if ops[0].type==X86_OP_MEM and not ops[0].mem.base and not ops[0].mem.index and ops[0].mem.disp in pairs:
                    at=ops[0].mem.disp;assert ops[0].size==4 and ops[1].type==X86_OP_REG and regs.get(ops[1].reg)==pairs[at]
                    stores[at]=pairs[at];sites.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),role='destination word store'))
            if 'rep movsd' in ins.mnemonic:
                src=constants.get(X86_REG_ESI);dst=constants.get(X86_REG_EDI);count=constants.get(X86_REG_ECX)
                if src in pairs.values() or dst in pairs:
                    assert count==16
                    for off in range(0,count*4,4):assert pairs[dst+off]==src+off;stores[dst+off]=src+off
                    blocks.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),source=f'{src:x}',destination=f'{dst:x}',words=count))
                    sites.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),role='complete 64-byte REP MOVSD copy'))
            for reg in ins.regs_access()[1]:regs.pop(reg,None);constants.pop(reg,None)
            if ins.mnemonic=='call':
                for reg in [X86_REG_EAX,X86_REG_ECX,X86_REG_EDX]:regs.pop(reg,None);constants.pop(reg,None)
            if new_source:regs[new_source[0]]=new_source[1]
            if new_constant:constants[new_constant[0]]=new_constant[1]
        assert stores==pairs and len(blocks)==2
        proof['copies'][function]=dict(bytes=152,function_address=f'{start:x}',machine_bytes=len(machine),word_pairs={f'{d:x}':f'{s:x}' for d,s in stores.items()},blocks=blocks,instructions=sites,machine_sha256=hashlib.sha256(machine).hexdigest())
    raw_save=(cache/'decompiled-abi/26b1f10.c').read_text()
    for name,(typ,size,at) in LIVE.items():assert re.search(r'\bsave_or_load_data\('+name+','+(str(size) if size==8 else hex(size))+r'\)',raw_save)
    manifest_path=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(manifest_path.read_text());assert not set(proof['globals'])&{g['name'] for g in manifest.get('source_globals',[])}
    declarations=[]
    for name,g in proof['globals'].items():
        if g['private']:declaration=f'static uint8_t {name}[{g["bytes"]}];'
        else:declaration='int '+name+g['type'][3:]+';'
        declarations.append(dict(source=g['source'],name=name,declaration=declaration,target_address=g['address'],zero_initialized=True,linkage='static' if g['private'] else 'external'))
    texts={source:(root/source).read_bytes().decode('latin1').replace('\r\n','\n') for source in ['functions/engine.c','functions/ai.c']}
    for source in texts:
        needle='#include "manalink.h"\n';assert texts[source].count(needle)==1
        texts[source]=texts[source].replace(needle,needle+'\n/* October 2022 mana modifier table storage. */\n'+'\n'.join(d['declaration'] for d in declarations if d['source']==source)+'\n',1)
    fragments=[]
    for function,prefix,restoring in snapshots:
        body=[]
        for name in LIVE:
            src,dst=(prefix+name,name) if restoring else (name,prefix+name);body.append(f'memcpy({dst}, {src}, sizeof({name}));')
        body='\n  '.join(body);address=rows[function]['low_pc'][2:];start=f'/* Recovered 2022 mana-cache fragment: {function} @ 0x{address}. */';end=f'/* End recovered 2022 mana-cache fragment: {function}. */'
        raw=(cache/'decompiled-abi'/(address+'.c')).read_text();fragments.append(dict(source='functions/ai.c',original_function=function,address=address,raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),start_marker=start,end_marker=end,body_sha256=hashlib.sha256(body.strip().encode()).hexdigest(),recovery='Independently verified complete 152-byte mana modifier snapshot copy.'))
        needle=f'void {function}(void)\n{{\n';assert texts['functions/ai.c'].count(needle)==1
        texts['functions/ai.c']=texts['functions/ai.c'].replace(needle,needle+f'  {start}\n  {body}\n  {end}\n',1)
    header=(root/'manalink.h').read_bytes().decode('latin1').replace('\r\n','\n');at=header.rfind('#ifdef __cplusplus')
    addition='/* October 2022 mana modifier caches. */\n#ifndef SHANDALAR\n'+'\n'.join('extern '+d['declaration'] for d in declarations if d['linkage']=='external')+'\n#endif\n\n';header=header[:at]+addition+header[at:]
    checkdir=cache/'mana-cache-check';checkdir.mkdir(exist_ok=True);(checkdir/'manalink.h').write_bytes(header.encode('latin1'))
    for source,text in texts.items():
        p=checkdir/Path(source).name;p.write_bytes(text.encode('latin1'))
        result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-I',str(checkdir),'-I',str(root),str(p)],capture_output=True,text=True)
        (checkdir/(p.name+'.log')).write_text(result.stderr);result.check_returncode()
    if args.apply:
        for source,text in texts.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        manifest.setdefault('source_globals',[]).extend(declarations);manifest.setdefault('source_fragments',[]).extend(fragments);manifest_path.write_text(json.dumps(manifest,indent=2))
        p=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(p.read_text());interfaces['mana_modifier_caches_2022']=proof;p.write_text(json.dumps(interfaces,indent=2))
    print(json.dumps(dict(applied=args.apply,globals=12,fragments=4,live_bytes=152,total_snapshot_bytes=608)))


if __name__=='__main__':main()
