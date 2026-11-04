"""Review paired card-data base/field offsets without changing source or scores.

Run from the source root with a full JSON queue and the successful build receipt
pinning the current DLL/debug pair. Frame declarations are cached by debug bytes,
selected functions and reader implementation. Unsupported cases remain unresolved.
"""
import argparse,collections,csv,json,sys
import capstone
import elftools,pefile
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parent))
from inspect_release import Image,value,inherited_value
from canonicalize_helpers import sha,read,span
from review_event_guards import roots
from shared_stack_test_analysis import walk
from global_initializers import inherited_type
from object_initializers import sizeof,unqualify,member_offset
from array_census_analysis import instance_locations
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_AC_READ,CS_AC_WRITE,CS_GRP_JUMP
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_OP_REG

md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True
registers=('eax','ebx','ecx','edx','esi','edi','esp','ebp')
index={name:n for n,name in enumerate(registers)}
def delta(state,name):return state[index[roots[name][0]]] if name in roots else 0
def memory_delta(i,op,state):return delta(state,i.reg_name(op.mem.base))+delta(state,i.reg_name(op.mem.index))*op.mem.scale
def full_register(i,op):return op.type==X86_OP_REG and roots.get(i.reg_name(op.reg),(None,0,0))[2]==32
def set_register(state,name,value):
    root,shift,bits=roots[name]
    assert value==0 or bits==32,('partial pointer write',name)
    assert bits==32 or state[index[root]]==0,('partial pointer overwrite',name)
    state[index[root]]=value

def pair_shape(old,new,base0,base1,size,selected):
    assert old.mnemonic==new.mnemonic and old.size==new.size and len(old.operands)==len(new.operands),('instruction shape',old.address,old.op_str,new.op_str)
    for a,b in zip(old.operands,new.operands):
        assert a.type==b.type and a.size==b.size
        if a.type==X86_OP_REG:assert old.reg_name(a.reg)==new.reg_name(b.reg)
        if a.type==X86_OP_MEM:
            assert (old.reg_name(a.mem.base),old.reg_name(a.mem.index),a.mem.scale)==(new.reg_name(b.mem.base),new.reg_name(b.mem.index),b.mem.scale)
            if a.mem.disp!=b.mem.disp and old.address-base0 not in selected:
                assert 0x2000000<=a.mem.disp<0x3000000 and 0x2000000<=b.mem.disp<0x3000000,('unlisted displacement',hex(old.address))
        if a.type==X86_OP_IMM:
            if old.group(CS_GRP_JUMP) or old.mnemonic=='call':
                internal0=base0<=a.imm<base0+size;internal1=base1<=b.imm<base1+size
                assert internal0==internal1
                if internal0:assert a.imm-base0==b.imm-base1
            elif a.imm!=b.imm:
                assert 0x2000000<=a.imm<0x3000000 and 0x2000000<=b.imm<0x3000000,('unlisted immediate',hex(old.address))

def check_flow(old,new,base0,base1,size,selected,frame_aliases=(),unknown_aliases=(),standard_frame=True,public_symbols=None,frame_objects=()):
    pending=[(0,(0,)*8,(),4 if standard_frame else None)];seen=set();loads=set();introductions=set();calls=set();terminals=0;spill_accesses=set();cdecl_boundaries=set()
    def private_slot(offset):
        assert standard_frame,'Compiler spill requires a standard frame-pointer prologue'
        assert offset<0,'Compiler-private spills cannot occupy incoming arguments or the return address'
        assert not unknown_aliases,('unbounded frame alias',unknown_aliases)
        assert not any(max(offset,a['start'])<min(offset+4,a['end']) for a in frame_aliases),('spill overlaps address-taken object',offset)
    while pending:
        offset,state,slot_state,stack_offset=pending.pop()
        if (offset,state,slot_state,stack_offset) in seen:continue
        seen.add((offset,state,slot_state,stack_offset));assert len(seen)<100000,'Unbounded relational states'
        i,j=old.get(base0+offset),new.get(base1+offset);assert i is not None and j is not None,('CFG outside code',hex(offset))
        state=list(state);slots=dict(slot_state);addresses=set();handled_writes=set();spill_operation=False
        def frame_offset(op):
            if op.type!=X86_OP_MEM or op.mem.index:return None
            name=i.reg_name(op.mem.base)
            if name=='ebp':return op.mem.disp
            if name=='esp' and stack_offset is not None:return stack_offset+op.mem.disp
            return None
        def frame_operand(op):
            return frame_offset(op) is not None
        if i.mnemonic.startswith('pop') and slots:
            assert stack_offset is not None,('unbounded implicit stack read',hex(i.address))
            overlap=[at for at in slots if max(at,stack_offset)<min(at+4,stack_offset+4)]
            if overlap:
                assert i.mnemonic=='pop' and len(i.operands)==1 and full_register(i,i.operands[0]) and overlap==[stack_offset],('partial implicit spill read',hex(i.address))
                name=i.reg_name(i.operands[0].reg);set_register(state,name,slots[stack_offset]);handled_writes.add(roots[name][0]);spill_operation=True
                spill_accesses.add((offset,'implicit_load',stack_offset,slots[stack_offset]))
        if i.mnemonic=='mov' and frame_operand(i.operands[0]) and i.operands[0].size==4:
            dest=frame_offset(i.operands[0]);source=i.operands[1]
            if source.type==X86_OP_REG and delta(state,i.reg_name(source.reg)):
                assert i.reg_name(i.operands[0].mem.base)=='ebp',('biased outgoing argument or unclassified stack store',hex(i.address))
                assert full_register(i,source);private_slot(dest);slots[dest]=delta(state,i.reg_name(source.reg));spill_operation=True
                spill_accesses.add((offset,'store',dest,slots[dest]))
            else:
                for at in list(slots):
                    if max(at,dest)<min(at+4,dest+4):
                        assert at==dest,('partial overlapping spill overwrite',hex(i.address))
                        del slots[at]
        elif i.mnemonic=='mov' and full_register(i,i.operands[0]) and frame_operand(i.operands[1]) and frame_offset(i.operands[1]) in slots:
            source=frame_offset(i.operands[1]);name=i.reg_name(i.operands[0].reg);set_register(state,name,slots[source]);handled_writes.add(roots[name][0]);spill_operation=True
            spill_accesses.add((offset,'load',source,slots[source]))
        for a,b in zip(i.operands,j.operands):
            if a.type!=X86_OP_MEM:continue
            addresses.update(n for n in (i.reg_name(a.mem.base),i.reg_name(a.mem.index)) if n in roots)
            if i.mnemonic=='lea':continue
            if frame_operand(a):
                location=frame_offset(a)
                overlaps=[at for at in slots if max(at,location)<min(at+4,location+a.size)]
                assert not overlaps or spill_operation,('unclassified spill use',hex(i.address),overlaps)
            elif i.reg_name(a.mem.base)=='esp' and slots:
                raise AssertionError(('unclassified stack access with live spills',hex(i.address)))
            elif i.reg_name(a.mem.base)=='ebp' and a.mem.index and slots:
                objects=[v for v in frame_objects if v['start']<=a.mem.disp<v['end']]
                assert objects and all(not any(max(at,v['start'])<min(at+4,v['end']) for at in slots) for v in objects),('unbounded indexed frame access',hex(i.address))
            active=memory_delta(i,a,state)
            if active or offset in selected:
                displacement=b.mem.disp-a.mem.disp
                assert (active+displacement)&0xffffffff==0,('effective address differs',hex(i.address),active,displacement,i.op_str,j.op_str)
                loads.add((offset,active,displacement,a.size))
        if i.mnemonic=='lea':
            a,b=i.operands,j.operands;assert full_register(i,a[0])
            change=b[1].mem.disp-a[1].mem.disp if offset in selected or memory_delta(i,a[1],state) else 0
            result=memory_delta(i,a[1],state)+change
            assert result in (0,32,48),('unexpected pointer relation',hex(i.address),result)
            if offset in selected:
                assert change in (32,48) and a[1].mem.disp==0x7e7010 and b[1].mem.disp in (0x7e7030,0x7e7040)
                introductions.add(offset)
            name=i.reg_name(a[0].reg);set_register(state,name,result);handled_writes.add(roots[name][0])
        elif i.mnemonic=='mov' and all(op.type==X86_OP_REG for op in i.operands):
            target,source=(i.reg_name(op.reg) for op in i.operands)
            value=delta(state,source)
            if value or delta(state,target):assert full_register(i,i.operands[0]) and full_register(i,i.operands[1]),('partial alias copy',hex(i.address))
            set_register(state,target,value);handled_writes.add(roots[target][0])
        elif i.mnemonic.startswith('cmov'):
            target=i.reg_name(i.operands[0].reg)
            value=delta(state,i.reg_name(i.operands[1].reg)) if i.operands[1].type==X86_OP_REG else 0
            assert delta(state,target)==value,('conditional pointer definition',hex(i.address))
            set_register(state,target,value)
        elif not spill_operation:
            zero=(i.mnemonic in ('xor','sub') and len(i.operands)==2 and all(op.type==X86_OP_REG for op in i.operands) and i.operands[0].reg==i.operands[1].reg)
            for op in i.operands:
                if op.type==X86_OP_REG and op.access&CS_AC_READ and not zero:
                    assert delta(state,i.reg_name(op.reg))==0,('biased pointer used as a value',hex(i.address),i.mnemonic,i.op_str)
            reads,writes=i.regs_access()
            for r in reads:
                name=i.reg_name(r)
                if name in roots and name not in addresses and not zero:
                    assert delta(state,name)==0,('implicit biased read',hex(i.address),name)
            for r in writes:
                name=i.reg_name(r)
                if name in roots:
                    root=roots[name][0]
                    if root not in handled_writes:set_register(state,name,0)
        if i.mnemonic=='call':
            if slots:
                assert stack_offset is not None and not any(max(at,stack_offset-4)<min(at+4,stack_offset) for at in slots),('call return word overlaps a spill',hex(i.address))
            if any(state[index[r]] for r in ('eax','ecx','edx')):
                assert i.operands[0].type==j.operands[0].type==X86_OP_IMM
                known={'remove_card_from_rfg','check_rfg','get_cmc_by_id','create_may_play_card_from_exile_effect'}
                matched=public_symbols[0].get(i.operands[0].imm,set())&public_symbols[1].get(j.operands[0].imm,set())&known if public_symbols is not None else set()
                assert matched,('biased volatile live at unclassified call',hex(i.address))
                cdecl_boundaries.add((offset,tuple(sorted(matched))))
            calls.add(offset)
            for r in ('eax','ecx','edx'):state[index[r]]=0
        if i.mnemonic=='ret':
            if standard_frame:assert stack_offset==4,('unresolved stack cleanup',hex(i.address),stack_offset)
            assert not any(state[index[r]] for r in ('eax','edx','ebx','esi','edi','esp','ebp')),('live pointer at return',hex(i.address),state)
            terminals+=1;continue
        if i.group(CS_GRP_JUMP):
            assert i.operands[0].type==X86_OP_IMM,('indirect jump',hex(i.address))
            target=i.operands[0].imm-base0
            if not 0<=target<size:
                assert i.mnemonic=='jmp' and not any(state),('biased tail call',hex(i.address))
                terminals+=1;continue
            pending.append((target,tuple(state),tuple(sorted(slots.items())),stack_offset))
            if i.mnemonic=='jmp':continue
        if i.mnemonic=='push':
            stack_offset=stack_offset-4 if stack_offset is not None else None
            if slots:
                assert stack_offset is not None,('unbounded implicit stack write',hex(i.address))
                for at in list(slots):
                    if max(at,stack_offset)<min(at+4,stack_offset+4):
                        assert at==stack_offset,'Partial push into a spill'
                        del slots[at]
        elif i.mnemonic=='pop':stack_offset=stack_offset+4 if stack_offset is not None else None
        elif i.mnemonic=='leave':stack_offset=4 if standard_frame else None
        elif i.operands and i.operands[0].type==X86_OP_REG and i.reg_name(i.operands[0].reg)=='esp':
            if i.mnemonic in ('add','sub') and i.operands[1].type==X86_OP_IMM:
                if stack_offset is not None:stack_offset+=i.operands[1].imm*(1 if i.mnemonic=='add' else -1)
            elif i.mnemonic=='lea' and i.operands[1].type==X86_OP_MEM and i.reg_name(i.operands[1].mem.base)=='ebp' and not i.operands[1].mem.index:stack_offset=i.operands[1].mem.disp if standard_frame else None
            elif i.mnemonic=='mov' and i.operands[1].type==X86_OP_REG and i.reg_name(i.operands[1].reg)=='ebp':stack_offset=0 if standard_frame else None
            elif i.mnemonic not in ('cmp','test'):stack_offset=None
        pending.append((offset+i.size,tuple(state),tuple(sorted(slots.items())),stack_offset))
    assert introductions and loads
    assert introductions|{r[0] for r in loads}==selected,('unvisited changed sites',selected,introductions,loads)
    return dict(cfg_states=len(seen),introductions=[hex(x) for x in sorted(introductions)],
                matching_accesses=[dict(offset=hex(at),register_delta=reg,displacement_delta=disp,width=width) for at,reg,disp,width in sorted(loads)],
                calls_checked=len(calls),terminal_states=terminals,
                spill_accesses=[dict(offset=hex(at),kind=kind,ebp_offset=slot,bias=bias) for at,kind,slot,bias in sorted(spill_accesses)],frame_alias_regions=frame_aliases,
                cdecl_boundaries=[dict(offset=hex(at),names=list(names)) for at,names in sorted(cdecl_boundaries)])

def collect_frames(rows,cache):
    wanted={(r['source'],r['name']) for r in rows}
    paths=['G:/Manalink/ManalinkEh_10_22_Hotfix.dbg','ManalinkEh.dbg']
    key=dict(debug={path:sha(Path(path).read_bytes()) for path in paths},
             functions=sorted(wanted),reader_sha256=sha(Path(__file__).read_bytes()),
             library_versions=dict(pyelftools=elftools.__version__,pefile=pefile.__version__),
             dependencies={name:sha(Path(__file__).with_name(name).read_bytes()) for name in
                           ('inspect_release.py','shared_stack_test_analysis.py','global_initializers.py','object_initializers.py','array_census_analysis.py','verify_inline_helpers.py')})
    if cache.is_file():
        saved=json.loads(cache.read_bytes())
        if saved.get('key')==key:return saved['frames']
    out=[]
    for side,path in enumerate(['G:/Manalink/ManalinkEh_10_22_Hotfix.dbg','ManalinkEh.dbg'] if rows else []):
        image=Image(Path(path).read_bytes());dw=image.dwarf();functions=[];layouts=[]
        for cu in dw.iter_CUs():
            source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/').replace('cards/odyssey.c','cards/odissey.c')
            if source not in {s for s,n in wanted}:continue
            for die in cu.get_top_DIE().iter_children():
                if die.tag=='DW_TAG_typedef' and value(die,'DW_AT_name')=='card_data_t':
                    record=unqualify(die);fields={value(m,'DW_AT_name'):dict(offset=member_offset(m),size=sizeof(inherited_type(m))) for m in record.iter_children() if m.tag=='DW_TAG_member' and value(m,'DW_AT_name') in ('id','static_ability','extra_ability')}
                    expected={'id':dict(offset=0x24,size=2),'static_ability':dict(offset=0x38,size=4),'extra_ability':dict(offset=0x3c,size=4)}
                    assert sizeof(record)==72 and fields==expected
                    layouts.append(dict(source=source,type_die=hex(die.offset),size=72,fields=fields))
                name=inherited_value(die,'DW_AT_name') if die.tag=='DW_TAG_subprogram' else None
                if (source,name) not in wanted or value(die,'DW_AT_low_pc') is None:continue
                assert value(die,'DW_AT_frame_base')==[156],(source,name)
                objects=[];unknown=[]
                for variable in walk(die):
                    if variable.tag not in ('DW_TAG_variable','DW_TAG_formal_parameter'):continue
                    try:locations=instance_locations(dw,die,variable)
                    except (AssertionError,ValueError,KeyError,AttributeError,TypeError):continue
                    if not locations:continue
                    try:width=sizeof(inherited_type(variable))
                    except (ValueError,KeyError,TypeError,RecursionError):unknown.append(dict(name=inherited_value(variable,'DW_AT_name'),locations=locations));continue
                    for loc in locations:
                        objects.append(dict(name=inherited_value(variable,'DW_AT_name'),die=hex(variable.offset),
                                            ebp_offset=loc['frame_offset']+8,size=width,start=hex(loc['start']),end=hex(loc['end'])))
                functions.append(dict(source=source,name=name,address=hex(value(die,'DW_AT_low_pc')),objects=objects,unknown=unknown))
            cu._dielist.clear();cu._diemap.clear()
        assert {(r['source'],r['name']) for r in functions}==wanted,(side,wanted-{(r['source'],r['name']) for r in functions})
        assert layouts
        out.append(dict(side=side,debug=path,sha256=sha(image.data),functions=functions,card_data_layouts=layouts))

    result=dict(inputs=out,reader_sha256=sha(Path(__file__).read_bytes()),
                scope='Declared fixed frame objects with single DW_OP_fbreg locations. Standard frame-pointer prologues are required before spill geometry uses CFA=EBP+8. Indexed frame accesses are restricted to declared object extents; invalid bounds/arbitrary stack inspection are not covered.')
    cache.parent.mkdir(parents=True,exist_ok=True)
    pending=cache.with_suffix('.pending.json');pending.write_text(json.dumps(dict(key=key,frames=result),indent=2)+'\n',encoding='utf-8');pending.replace(cache)
    return result

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--queue',type=Path,required=True)
    parser.add_argument('--build-evidence',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--frame-cache',type=Path,default=Path('.recovery/reccmp/card-data-frame-cache.json'))
    args=parser.parse_args()
    queue_path=args.queue;queue=json.loads(queue_path.read_bytes())
    report_path=Path(queue['report']);receipt_path=report_path.with_suffix('.inputs.json');receipt=json.loads(receipt_path.read_bytes())
    assert sha(report_path.read_bytes())==receipt['report_sha256']==queue['report_sha256']
    assert sha(receipt_path.read_bytes())==queue['receipt_sha256']
    metadata_path=Path(receipt['metadata_snapshot']);assert sha(metadata_path.read_bytes())==receipt['metadata_sha256']
    metadata={row['address']:row for row in csv.DictReader(metadata_path.open(newline='',encoding='utf-8'))}
    report={row['address']:row for row in json.loads(report_path.read_bytes())['data']}
    for row in queue['functions']:
        original=report[row['address']];m=metadata[row['address']]
        expected=[dict(original=b.get('orig',[]),rebuilt=b.get('recomp',[])) for _,blocks in original['diff'] for b in blocks if 'both' not in b]
        assert expected==row['changes'] and original['name']==row['name']
        assert m['source'].replace('cards/odyssey.c','cards/odissey.c')==row['source'] and int(m['size'])==row['native_size']
    build=json.loads(args.build_evidence.read_bytes())['compiled_verification']
    assert build['build']=='passed' and build['fixed_entry_slots']==15169
    assert sha(Path('ManalinkEh.dll').read_bytes())==build['dll_sha256']
    assert sha(Path('ManalinkEh.dbg').read_bytes())==build.get('debug_sha256',build.get('dbg_sha256'))
    assert sha(Path('.recovery/2022-ManalinkEh.dll').read_bytes())=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert sha(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'

    rows=[r for r in queue['functions'] if any('0x7e7010' in s for b in r['changes'] for _,s in b['original'])]
    input_paths=[Path(path) for path in ('.recovery/2022-ManalinkEh.dll','ManalinkEh.dll','G:/Manalink/ManalinkEh_10_22_Hotfix.dbg','ManalinkEh.dbg','manalink.h','defs.h','Makefile','Makefile-common','Makefile-targets','tools/build-recovery.ps1')]
    input_paths.extend([queue_path,report_path,receipt_path,metadata_path,args.build_evidence,Path(__file__),Path(capstone.__file__),Path(capstone._cs._name),Path(elftools.__file__),Path(pefile.__file__)])
    input_paths.extend(Path(__file__).with_name(name) for name in ('inspect_release.py','canonicalize_helpers.py','unify_scalar_helpers.py','review_event_guards.py','shared_stack_test_analysis.py','global_initializers.py','object_initializers.py','array_census_analysis.py','verify_inline_helpers.py'))
    input_paths.extend(Path(row['source']) for row in rows)
    input_state={str(path):sha(path.read_bytes()) for path in input_paths}
    assert input_state['ManalinkEh.dll']==build['dll_sha256'] and input_state['ManalinkEh.dbg']==build.get('debug_sha256',build.get('dbg_sha256'))

    images=[Image(Path(p).read_bytes()) for p in ('.recovery/2022-ManalinkEh.dll','ManalinkEh.dll')]
    debugs=[Image(Path(p).read_bytes()) for p in ('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg','ManalinkEh.dbg')]
    tables=[];public=[]
    for image in debugs:
        table=collections.defaultdict(set);p=collections.defaultdict(set)
        for n,o,s,k,t in image.symbols:
            if s>0 and k==32:
                address=0x2000000+image.pe.sections[s-1].VirtualAddress+o
                table[n.lstrip('_')].add(address)
                if t==2:p[address].add(n.lstrip('_'))
        tables.append(table);public.append(p)
    # Display ordinals such as "helper(1)" are not DWARF/COFF identities.
    # Keep those rows for the per-row unresolved result, without making a
    # missing frame declaration abort reviews of unrelated candidates.
    frame_rows=[r for r in rows if all(len(table[r['name']])==1 for table in tables)]
    frames=collect_frames(frame_rows,args.frame_cache)
    input_state[str(args.frame_cache)]=sha(args.frame_cache.read_bytes())
    frame_functions=[{f['name']:f for f in side['functions']} for side in frames['inputs']]
    reviewed=[];unresolved=[]
    for row in rows:
        try:
            maps=[];bases=[];ranges=[];selected=set();aliases=[];unknown=[];standard_frame=True;objects=[]
            for side in (0,1):
                assert len(tables[side][row['name']])==1
                base=next(iter(tables[side][row['name']]))
                size=row['native_size'];raw=images[side].pe.get_data(base-0x2000000,size)
                code={i.address:i for i in md.disasm(raw,base)};assert sum(i.size for i in code.values())==size
                maps.append(code);bases.append(base);ranges.append(dict(side=side,address=hex(base),size=size,sha256=sha(raw)))
                standard_frame &= raw[:3]==bytes.fromhex('5589e5')
                assert frame_functions[side][row['name']]['address']==hex(base)
                objects.extend(dict(side=side,start=v['ebp_offset'],end=v['ebp_offset']+v['size'],name=v['name']) for v in frame_functions[side][row['name']]['objects'])
                for inst in code.values():
                    if any(inst.reg_name(reg)=='ebp' for reg in inst.regs_access()[1]):
                        standard_frame &= ((inst.address==base+1 and inst.mnemonic=='mov' and inst.op_str=='ebp, esp') or inst.mnemonic=='leave' or (inst.mnemonic=='pop' and inst.op_str=='ebp'))
                    if inst.mnemonic=='lea' and inst.operands[1].type==X86_OP_MEM and inst.reg_name(inst.operands[0].reg)!='esp':
                        mem=inst.operands[1].mem
                        if inst.reg_name(mem.base)=='esp':unknown.append((side,hex(inst.address),'esp-relative frame address'))
                        if inst.reg_name(mem.base)=='ebp':
                            variables=[v for v in frame_functions[side][row['name']]['objects'] if v['ebp_offset']<=mem.disp<v['ebp_offset']+v['size'] and int(v['start'],16)<=inst.address<int(v['end'],16)]
                            if not variables:unknown.append((side,hex(inst.address),'undeclared frame address',mem.disp))
                            for v in variables:aliases.append(dict(side=side,instruction=hex(inst.address),start=v['ebp_offset'],end=v['ebp_offset']+v['size'],name=v['name'],indexed=bool(mem.index)))
                    if inst.mnemonic=='mov' and any(op.type==X86_OP_REG and inst.reg_name(op.reg)=='ebp' for op in inst.operands[1:]):
                        if not (inst.operands[0].type==X86_OP_REG and inst.reg_name(inst.operands[0].reg)=='esp'):unknown.append((side,hex(inst.address),'raw frame address escapes'))
                    if inst.mnemonic=='mov' and any(op.type==X86_OP_REG and inst.reg_name(op.reg)=='esp' for op in inst.operands[1:]):
                        if not (inst.address==base+1 and inst.op_str=='ebp, esp'):unknown.append((side,hex(inst.address),'raw stack address escapes'))
                if side==0:
                    for block in row['changes']:
                        assert len(block['original'])==len(block['rebuilt']) and block['original'], 'Mixed instruction reordering is outside address-only review'
                        for a,b in zip(block['original'],block['rebuilt']):selected.add(int(a[0],16)-base)
            assert set(at-bases[0] for at in maps[0])==set(at-bases[1] for at in maps[1])
            for at,i in maps[0].items():pair_shape(i,maps[1][bases[1]+at-bases[0]],bases[0],bases[1],size,selected)
            result=check_flow(*maps,*bases,size,selected,aliases,unknown,standard_frame,public,objects)
            text=read(row['source']);body=text[slice(*span(text,row['name']))]
            reviewed.append(dict(name=row['name'],address=row['address'],source=row['source'],source_definition_sha256=sha(body.encode()),
                                 native_and_rebuilt_ranges=ranges,discovery_changes=row['changes'],**result))
        except (AssertionError,KeyError,ValueError) as error:unresolved.append(dict(name=row['name'],reason=str(error)))
    assert all(sha(Path(path).read_bytes())==expected for path,expected in input_state.items()),'Review input changed during analysis'
    result=dict(format=1,reviewed=reviewed,unresolved=unresolved,inputs=input_state,
        discovery_report_sha256=sha(report_path.read_bytes()),queue_sha256=sha(queue_path.read_bytes()),
        script_sha256=sha(Path(__file__).read_bytes()),decoder_version=capstone.__version__,
        build_evidence=dict(path=str(args.build_evidence),sha256=sha(args.build_evidence.read_bytes())),
        frames_sha256=sha(args.frame_cache.read_bytes()),frame_declarations=frames,
        scope='Local card-data address bias only, under existing callee/global correspondences and GCC i386 ABI rules. A CFG worklist tracks modulo-32-bit differences in registers and compiler-private frame words. '
              'Every affected memory access must cancel the actual displacement change, with unchanged width. Pointer values cannot escape into arguments/comparisons or partial writes. '
              'Address-taken frame objects must be disjoint from biased spill slots; indexed frame accesses must remain within the pinned DWARF object extents. Invalid bounds, arbitrary memory aliases and stack inspection are outside this source-level review. '
              'Only four named, cross-CU public C helpers discharge caller-saved pointer temporaries; other calls require equal volatile registers. '
              'This is not whole-handler or gameplay equivalence, and does not certify other mapped code/data identities. Unsupported mixed changes remain unresolved; no source edits or score changes.')
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(dict(reviewed=len(reviewed),unresolved=unresolved),indent=2))

if __name__=="__main__":
    main()
