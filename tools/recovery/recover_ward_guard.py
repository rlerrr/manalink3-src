"""Recover the protection-Aura recursion guard and its native static target pair."""
import argparse,csv,hashlib,json,re,struct,subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import clean,strip_comments,UNRESOLVED
from recover_test_consumers import read,format_c
from recover_literals import recover_strings
from audit_source import function_body

ADDRESS='26e7fd0'
GLOBAL='ward_recalculation_2022'
DECLARATION='static target_t ward_recalculation_2022 = {-1, -1};'
SOURCE='functions/auras.c'


def sha(data):return hashlib.sha256(data).hexdigest()


def main():
    ap=argparse.ArgumentParser(__doc__);ap.add_argument('--apply',action='store_true');args=ap.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'ward_recursion_guard_2022' not in interfaces
    assert ADDRESS not in {f['address'] for f in m['accepted_functions']}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);dwarf=debug.dwarf();parameters=None;global_die=None;events={}
    for cu in dwarf.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!=SOURCE:continue
        def visit(die):
            nonlocal global_die
            for c in die.iter_children():
                if c.tag=='DW_TAG_variable' and inherited_value(c,'DW_AT_name')=='recalculating_ward':global_die=c
                elif c.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:visit(c)
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':events.update({value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in die.iter_children() if c.tag=='DW_TAG_enumerator'})
            if value(die,'DW_AT_low_pc')!=int(ADDRESS,16):continue
            parameters=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=p.attributes['DW_AT_location'].value) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter'];visit(die)
        break
    assert [p['name'] for p in parameters]==['player','card','event'] and global_die is not None
    assert type_name(type_of(global_die))=='target_t'
    record=type_of(global_die)
    while record.tag=='DW_TAG_typedef':record=type_of(record)
    members={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in record.iter_children() if c.tag=='DW_TAG_member'}
    assert value(record,'DW_AT_byte_size')==8 and members==dict(player=0,card=4)
    symbols=[]
    for name,offset,section,kind,storage in debug.symbols:
        if name.startswith('_recalculating_ward.'):
            a=debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[section-1].VirtualAddress+offset
            symbols.append(dict(name=name,address=f'{a:x}',storage=storage))
    assert len(symbols)==1 and symbols[0]['storage']==3
    global_address=int(symbols[0]['address'],16)
    assert global_die.attributes['DW_AT_location'].value==[3]+list(struct.pack('<I',global_address))
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());initial=runtime.pe.get_data(global_address-0x2000000,8);assert initial==b'\xff'*8
    meta=next(r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc']=='0x'+ADDRESS)
    native=runtime.pe.get_data(int(ADDRESS,16)-0x2000000,int(meta['high_pc'],16)-int(ADDRESS,16));decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;instructions=list(decoder.disasm(native,int(ADDRESS,16)))
    function_symbols=[dict(name=n,storage=k) for n,v,s,t,k in debug.functions() if s>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[s-1].VirtualAddress+v==int(ADDRESS,16)]
    assert function_symbols==[dict(name='_this_effect_doesnt_remove_this',storage=2)]
    sites=[]
    for instruction in instructions:
        for operand in instruction.operands:
            if operand.type==X86_OP_MEM and operand.mem.disp in {global_address,global_address+4}:
                assert operand.size==4
                sites.append(dict(address=f'{instruction.address:x}',bytes=instruction.bytes.hex(),field_offset=operand.mem.disp-global_address,field_bytes=4))
    assert len(sites)==7 and {s['field_offset'] for s in sites}=={0,4}
    raw=(cache/'decompiled-abi'/(ADDRESS+'.c')).read_text();original=strip_comments(raw);text=original.replace('this_effect_doesnt_remove_this::recalculating_ward',GLOBAL);assert text!=original
    text,literals=recover_strings(text,runtime,{})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    body=clean(text,overlap,use_target_api_2022=True,use_test_api_2022=True,use_extension_flags_2022=True,use_keyword_accessors_2022=True,use_ability_api_2022=True,use_pump_api_2022=True)
    assert not UNRESOLVED.search(body) and '::' not in body
    body=format_c(body);targets={int(f['address'],16):f['name'] for f in m['accepted_functions']};calls=[]
    for i in instructions:
        if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm in targets:
            name=targets[i.operands[0].imm];assert re.search(r'\b'+name+r'\s*\(',body)
            calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{i.operands[0].imm:x}',name=name))
    detail=dict(machine_bytes=len(native),machine_sha256=sha(native),canonical_calls=calls,changes=[dict(original=original,replacement=text,occurrences=1,kind='native static recursion pair',machine_sites=sites)])
    entry=dict(name='this_effect_doesnt_remove_this',address=ADDRESS,source=SOURCE,line=meta['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,
        ward_guard_evidence=detail,reconstruction='Native protection-Aura recursion guard, initialized static target pair and 84-byte target flags. Preserve native card-only static-event recursion check and state-based removal behavior.',reconstruction_body_sha256=sha(function_body(body,'this_effect_doesnt_remove_this').encode()))
    declaration=dict(source=SOURCE,name=GLOBAL,declaration=DECLARATION,target_address=f'{global_address:x}',zero_initialized=False,initial_bytes=initial.hex(),linkage='static')
    proof=dict(debug_sha256=sha(debug_data),parameters=parameters,function_symbols=function_symbols,global_record=dict(bytes=8,members=members,symbols=symbols,location=global_die.attributes['DW_AT_location'].value,initial_bytes=initial.hex(),declaration=declaration,sites=sites),
        target_flags=dict(bytes=84,offset=80,mask=3),regen_context=dict(offset=40,mask=0x8000000),
        events={n:events[n] for n in ['EVENT_STATIC_EFFECTS','EVENT_ABILITIES']},function=detail)
    source=read(root/SOURCE)+'\n\n#ifndef SHANDALAR\n/* Recovered October 2022 protection-Aura recursion guard. */\n'+DECLARATION+'\n'+f"/* ManalinkEh 2022: 0x{ADDRESS}; original {SOURCE}:{meta['line']}. */\n"+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+'/* October 2022 protection-Aura guard API. */\n#ifndef SHANDALAR\n'+entry['prototype']+'\n#endif\n\n'+header[at:]
    scratch=cache/'ward-guard-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'));(scratch/'auras.c').write_bytes(source.encode('latin1'))
    r=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(scratch/'auras.c')],capture_output=True,text=True)
    (scratch/'auras.log').write_text(r.stderr)
    if r.returncode:print(r.stderr)
    r.check_returncode()
    if args.apply:
        (root/SOURCE).write_bytes(source.replace('\n','\r\n').encode('latin1'));(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions'].append(entry);m['accepted']+=1;m['helper_functions']+=1;m['shared_helper_interfaces']+=1;m['source_files'][SOURCE]+=1;m['source_globals'].append(declaration)
        mp.write_text(json.dumps(m,indent=2));interfaces['ward_recursion_guard_2022']=proof;ip.write_text(json.dumps(interfaces,indent=2))
    (cache/'ward-guard-plan.json').write_text(json.dumps(proof,indent=2));print(json.dumps(dict(applied=args.apply,new_helpers=1,static_global_bytes=8,native_global_word_references=len(sites))))


if __name__=='__main__':main()
