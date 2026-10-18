"""Recover dice modifier globals, recalculation reset and both AI snapshot pairs."""
import argparse
import csv
import hashlib
import json
from pathlib import Path

import capstone
from capstone.x86_const import X86_OP_REG, X86_OP_MEM, X86_OP_IMM
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes())
    globals_at=dict(add_to_die_rolls=0x2797190,roll_extra_dice=0x2797198,
        backup_add_to_die_rolls=0x2784e60,backup_roll_extra_dice=0x2784e68,
        backup0_add_to_die_rolls=0x2775bc0,backup0_roll_extra_dice=0x2775bc8)
    evidence=dict(globals={},copies={},reset=[])
    for name,at in globals_at.items():
        matches=[s for s in debug.symbols if s[0]=='_'+name]
        assert len(matches)==1
        symbol=matches[0];section=debug.pe.sections[symbol[2]-1]
        private=name.startswith('backup')
        assert symbol[4]==(3 if private else 2) and section.Name.rstrip(b'\0')==b'.bss'
        assert debug.pe.OPTIONAL_HEADER.ImageBase+section.VirtualAddress+symbol[1]==at
        evidence['globals'][name]=dict(address=f'{at:x}',bytes=8,private=private,zero_initialized=True)
    found=set()
    for cu in debug.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in ['functions/engine.c','functions/ai.c']:continue
        for die in cu.get_top_DIE().iter_children():
            name=inherited_value(die,'DW_AT_name')
            if die.tag=='DW_TAG_variable' and name in globals_at and value(die,'DW_AT_location'):
                typ=type_name(type_of(die));assert typ==('char[8]' if name.startswith('backup') else 'int[2]')
                assert value(die,'DW_AT_location')==[3,*globals_at[name].to_bytes(4,'little')]
                evidence['globals'][name].update(type=typ,die=hex(die.offset),source=source);found.add(name)
        cu._dielist.clear();cu._diemap.clear()
    assert found==set(globals_at)
    rows={r['name']:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc']}
    decoder=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32);decoder.detail=True
    def instructions(name):
        row=rows[name];at=int(row['low_pc'],16);end=int(row['high_pc'],16)
        return list(decoder.disasm(runtime.pe.get_data(at-runtime.pe.OPTIONAL_HEADER.ImageBase,end-at),at))
    def absolute(op):
        return op.type==X86_OP_MEM and not op.mem.base and not op.mem.index
    reset=instructions('recalculate_all_cards_in_play')
    reset_values={}
    for ins in reset:
        if ins.mnemonic=='mov' and absolute(ins.operands[0]) and ins.operands[1].type==X86_OP_IMM:
            at=ins.operands[0].mem.disp
            if 0x2797190<=at<0x27971a0:
                assert ins.operands[0].size==4 and ins.operands[1].imm==0
                reset_values[at]=0;evidence['reset'].append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex()))
    assert set(reset_values)=={0x2797190,0x2797194,0x2797198,0x279719c}
    snapshots=[('backup_data_for_ai','backup_',False),('restore_data_for_ai','backup_',True),
               ('backup_data_for_ai_0','backup0_',False),('restore_data_for_ai_0','backup0_',True)]
    for function,prefix,restoring in snapshots:
        pairs={}
        for name in ['add_to_die_rolls','roll_extra_dice']:
            source,dest=(globals_at[prefix+name],globals_at[name]) if restoring else (globals_at[name],globals_at[prefix+name])
            for offset in [0,4]:pairs[dest+offset]=source+offset
        regs={};stores={};proof=[]
        for ins in instructions(function):
            ops=ins.operands;new=None
            if ins.mnemonic=='mov' and len(ops)==2:
                if ops[0].type==X86_OP_REG and absolute(ops[1]) and ops[1].size==4 and ops[1].mem.disp in pairs.values():
                    new=(ops[0].reg,ops[1].mem.disp)
                    proof.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),role='source word load'))
                if absolute(ops[0]) and ops[0].mem.disp in pairs:
                    at=ops[0].mem.disp
                    assert ops[0].size==4 and ops[1].type==X86_OP_REG and regs.get(ops[1].reg)==pairs[at]
                    stores[at]=regs[ops[1].reg]
                    proof.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),role='matching destination word store'))
            for reg in ins.regs_access()[1]:regs.pop(reg,None)
            if ins.mnemonic=='call':
                for reg in [capstone.x86.X86_REG_EAX,capstone.x86.X86_REG_ECX,capstone.x86.X86_REG_EDX]:regs.pop(reg,None)
            if new:regs[new[0]]=new[1]
        assert stores==pairs
        evidence['copies'][function]=dict(bytes=16,word_pairs={f'{d:x}':f'{s:x}' for d,s in pairs.items()},instructions=proof)
    save_raw=(cache/'decompiled-abi/26b1f10.c').read_text()
    assert 'save_or_load_data(roll_extra_dice,8)' in save_raw and 'save_or_load_data(add_to_die_rolls,8)' in save_raw
    evidence['serialization']='Target saves both eight-byte arrays in its larger supplemental record. Save-format migration remains deferred, matching prior cache recoveries.'
    path=root/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(path.read_text())
    fragments=[]
    engine=(root/'functions/engine.c').read_bytes().decode('latin1').replace('\r\n','\n')
    ai=(root/'functions/ai.c').read_bytes().decode('latin1').replace('\r\n','\n')
    declarations=[]
    for name,at in globals_at.items():
        private=name.startswith('backup');source='functions/ai.c' if private else 'functions/engine.c'
        declaration=f'static uint8_t {name}[8];' if private else f'int {name}[2];'
        declarations.append(dict(source=source,name=name,declaration=declaration,target_address=f'{at:x}',zero_initialized=True,linkage='static' if private else 'external'))
    assert all(d['name'] not in {g['name'] for g in manifest.get('source_globals',[])} for d in declarations)
    for source in ['functions/engine.c','functions/ai.c']:
        addition='\n/* October 2022 dice modifier cache storage. */\n'+'\n'.join(d['declaration'] for d in declarations if d['source']==source)+'\n'
        if source=='functions/engine.c':engine=engine.replace('#include "manalink.h"\n','#include "manalink.h"\n'+addition,1)
        else:ai=ai.replace('#include "manalink.h"\n','#include "manalink.h"\n'+addition,1)
    def fragment(source,function,body):
        address=rows[function]['low_pc'][2:]
        start=f'/* Recovered 2022 dice-cache fragment: {function} @ 0x{address}. */'
        end=f'/* End recovered 2022 dice-cache fragment: {function}. */'
        raw=(cache/'decompiled-abi'/(address+'.c')).read_text()
        fragments.append(dict(source=source,original_function=function,address=address,
            raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),start_marker=start,end_marker=end,
            body_sha256=hashlib.sha256(body.strip().encode()).hexdigest(),
            recovery='Only independently verified dice modifier resets or complete AI copies; other engine/AI code retained.'))
        return f'  {start}\n  {body}\n  {end}\n'
    needle='void recalculate_all_cards_in_play(void)\n{\n';assert engine.count(needle)==1
    engine=engine.replace(needle,needle+fragment('functions/engine.c','recalculate_all_cards_in_play',
        'memset(add_to_die_rolls, 0, sizeof(add_to_die_rolls));\n  memset(roll_extra_dice, 0, sizeof(roll_extra_dice));'),1)
    for function,prefix,restoring in snapshots:
        body=[]
        for name in ['add_to_die_rolls','roll_extra_dice']:
            source,dest=(prefix+name,name) if restoring else (name,prefix+name)
            body.append(f'memcpy({dest}, {source}, sizeof({name}));')
        needle=f'void {function}(void)\n{{\n';assert ai.count(needle)==1
        ai=ai.replace(needle,needle+fragment('functions/ai.c',function,'\n  '.join(body)),1)
    candidate=cache/'dice-cache-check';candidate.mkdir(exist_ok=True)
    import subprocess
    for name,text in [('engine.c',engine),('ai.c',ai)]:
        p=candidate/name;p.write_text(text)
        result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99',
            '-fsyntax-only','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-I',str(root),str(p)],capture_output=True,text=True)
        (candidate/(name+'.log')).write_text(result.stderr);result.check_returncode()
    if args.apply:
        (root/'functions/engine.c').write_bytes(engine.replace('\n','\r\n').encode('latin1'))
        (root/'functions/ai.c').write_bytes(ai.replace('\n','\r\n').encode('latin1'))
        manifest.setdefault('source_globals',[]).extend(declarations);manifest.setdefault('source_fragments',[]).extend(fragments)
        path.write_text(json.dumps(manifest,indent=2))
        path=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(path.read_text());interfaces['dice_modifier_caches']=evidence;path.write_text(json.dumps(interfaces,indent=2))
    print(json.dumps(dict(applied=args.apply,globals=len(declarations),fragments=len(fragments),live_bytes=16,snapshot_copy_words=16),indent=2))


if __name__=='__main__':
    main()
