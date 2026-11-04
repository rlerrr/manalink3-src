"""Check swapped retarget-event predicates without certifying whole handlers.

Run from the source root after generating a full JSON and its small-diff queue.
The current build can be newer: debug symbols rebind each current entry and both
actual instruction ranges are checked. No candidate is removed or source edited.
"""
import argparse,collections,csv,json,re,sys
import capstone
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parent))
from inspect_release import Image
from canonicalize_helpers import read,span,sha
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_OP_REG

md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True
roots={n:(n,0,32) for n in ('eax','ebx','ecx','edx','esi','edi','esp','ebp')}
for full,short,low,high in [('eax','ax','al','ah'),('ebx','bx','bl','bh'),('ecx','cx','cl','ch'),('edx','dx','dl','dh')]:
    roots[short]=(full,0,16);roots[low]=(full,0,8);roots[high]=(full,8,8)
for full,short in [('esi','si'),('edi','di'),('esp','sp'),('ebp','bp')]:roots[short]=(full,0,16)

def regname(i,op):return i.reg_name(op.reg)
def regread(registers,name):
    root,shift,bits=roots[name];return registers[root]>>shift&((1<<bits)-1)
def regwrite(registers,name,value):
    root,shift,bits=roots[name];mask=((1<<bits)-1)<<shift
    registers[root]=(registers[root]&~mask)|((value<<shift)&mask)
def operand(i,op):
    if op.type==X86_OP_REG:return ('reg',regname(i,op))
    if op.type==X86_OP_MEM:
        return ('mem',i.reg_name(op.mem.base),i.reg_name(op.mem.index),op.mem.scale,op.mem.disp)
    raise ValueError('Expected event register/stack operand')
def same_event_operand(i,identity):return i.mnemonic=='cmp' and operand(i,i.operands[0])==identity and i.operands[1].type==X86_OP_IMM

def walk(code,start,identity,event,incoming,incoming_reg):
    registers={r:0x13579bdf^(n*0x1020304) for n,r in enumerate(('eax','ebx','ecx','edx','esi','edi','esp','ebp'))}
    if identity[0]=='reg':regwrite(registers,identity[1],event)
    if incoming_reg:regwrite(registers,incoming_reg,incoming)
    pc=start;zf=None;visited=[];changed=set()
    for _ in range(30):
        i=code.get(pc)
        if i is None:raise ValueError('Guard left mapped function')
        visited.append(pc)
        if same_event_operand(i,identity) and i.operands[1].imm in (0x400,0x5400):
            if identity[0]=='reg' and regread(registers,identity[1])!=event:raise ValueError('Event input was overwritten')
            zf=event==(i.operands[1].imm&0xffffffff)
        elif i.mnemonic in ('sete','setne'):
            if zf is None:raise ValueError('Uninitialized predicate flags')
            name=regname(i,i.operands[0]);assert roots[name][2]==8
            regwrite(registers,name,int(zf if i.mnemonic=='sete' else not zf));changed.add(name)
        elif i.mnemonic in ('test','or'):
            if any(op.type!=X86_OP_REG or op.size!=1 for op in i.operands):break
            a,b=(regname(i,op) for op in i.operands);av,bv=regread(registers,a),regread(registers,b)
            result=av&bv if i.mnemonic=='test' else av|bv;zf=result==0
            if i.mnemonic=='or':regwrite(registers,a,result);changed.add(a)
        elif i.mnemonic in ('je','jne'):
            if zf is None:raise ValueError('Uninitialized branch flags')
            if zf==(i.mnemonic=='je'):pc=i.operands[0].imm;continue
        elif i.mnemonic=='jmp' and i.operands[0].type==X86_OP_IMM:
            pc=i.operands[0].imm;continue
        elif i.mnemonic=='nop' or (i.mnemonic=='lea' and i.op_str=='esi, [esi]'):
            pass
        else:break
        pc=i.address+i.size
    else:raise ValueError('Guard did not terminate')
    return pc,registers,changed,visited

def public_cdecl(i,symbols):
    if i.mnemonic!='call' or i.operands[0].type!=X86_OP_IMM:return False
    # These declared public C helpers are defined in another compilation unit;
    # their calls use stack arguments and caller-saved eax/ecx/edx, without LTO.
    names=symbols.get(i.operands[0].imm,set())
    return bool(names & {'get_card_instance','default_target_definition','counterspell_target_definition','default_test_definition','new_default_test_definition','check_battlefield_for_id','has_mana','can_target','has_mana_for_activated_ability','new_global_tutor','generic_spell','is_what','in_play','new_select_target_from_grave','get_sleighted_color_text','cycling'})

def temps_dead(code,start,byte_differences,symbols):
    live=dict(byte_differences);pc=start;seen=set();trace=[]
    for _ in range(80):
        if not live:return True,trace
        if pc in seen:return False,trace
        seen.add(pc);i=code.get(pc)
        if i is None:return False,trace
        trace.append((hex(pc),i.mnemonic+' '+i.op_str))
        if i.mnemonic=='ret':
            return not (set(live)-{'ecx','edx'}),trace
        if public_cdecl(i,symbols):
            for root in ('eax','ecx','edx'):live.pop(root,None)
            return not live,trace
        if i.mnemonic in ('call','jmp') or i.mnemonic.startswith('j'):return False,trace
        reads,writes=i.regs_access()
        zero=(i.mnemonic in ('xor','sub') and len(i.operands)==2 and all(op.type==X86_OP_REG for op in i.operands) and i.operands[0].reg==i.operands[1].reg)
        for reg in reads:
            name=i.reg_name(reg)
            if name not in roots or zero:continue
            root,shift,bits=roots[name]
            if live.get(root,0)&(((1<<bits)-1)<<shift):return False,trace
        for reg in writes:
            name=i.reg_name(reg)
            if name in roots:
                root,shift,bits=roots[name]
                if root in live:
                    live[root]&=~(((1<<bits)-1)<<shift)
                    if not live[root]:del live[root]
        pc=i.address+i.size
    return False,trace

def flags_dead(code,start,symbols):
    pc=start;seen=set();trace=[]
    for _ in range(80):
        if pc in seen:return False,trace
        seen.add(pc);i=code.get(pc)
        if i is None:return False,trace
        trace.append((hex(pc),i.mnemonic+' '+i.op_str))
        if i.mnemonic in ('cmp','test','and','or','xor','add','sub','neg','ret') or public_cdecl(i,symbols):return True,trace
        if i.mnemonic=='jmp' and i.operands[0].type==X86_OP_IMM:pc=i.operands[0].imm;continue
        if i.mnemonic not in ('mov','lea','push','pop','leave','nop','movzx','movsx'):return False,trace
        pc=i.address+i.size
    return False,trace

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--queue',type=Path,required=True)
    parser.add_argument('--build-evidence',type=Path,required=True,
                        help='Successful source-batch receipt pinning the current DLL/debug pair')
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    queue_path=args.queue
    queue=json.loads(queue_path.read_bytes())
    report_path=Path(queue['report'])
    report_receipt_path=report_path.with_suffix('.inputs.json')
    receipt=json.loads(report_receipt_path.read_bytes())
    assert sha(report_path.read_bytes())==queue['report_sha256']==receipt['report_sha256']
    assert sha(report_receipt_path.read_bytes())==queue['receipt_sha256']
    metadata_path=Path(receipt['metadata_snapshot'])
    assert sha(metadata_path.read_bytes())==receipt['metadata_sha256']
    metadata={row['address']:row for row in csv.DictReader(metadata_path.open(newline='',encoding='utf-8'))}
    report={row['address']:row for row in json.loads(report_path.read_bytes())['data']}
    for row in queue['functions']:
        expected=[dict(original=b.get('orig',[]),rebuilt=b.get('recomp',[]))
                  for _,blocks in report[row['address']]['diff'] for b in blocks if 'both' not in b]
        assert expected==row['changes']
        assert row['name']==report[row['address']]['name']
        assert row['source']==metadata[row['address']]['source'].replace('cards/odyssey.c','cards/odissey.c')
    build=json.loads(args.build_evidence.read_bytes())['compiled_verification']
    assert build['build']=='passed' and build['fixed_entry_slots']==15169
    assert sha(Path('ManalinkEh.dll').read_bytes())==build['dll_sha256']
    assert sha(Path('ManalinkEh.dbg').read_bytes())==build.get('debug_sha256',build.get('dbg_sha256'))
    assert sha(Path('.recovery/2022-ManalinkEh.dll').read_bytes())=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert sha(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'

    rows=[r for r in queue['functions'] if r['review_pattern']=='retarget_event_comparison_order']
    input_paths=[Path(path) for path in ('.recovery/2022-ManalinkEh.dll','ManalinkEh.dll','G:/Manalink/ManalinkEh_10_22_Hotfix.dbg','ManalinkEh.dbg')]
    input_paths.extend([queue_path,report_path,report_receipt_path,metadata_path,args.build_evidence,Path(__file__)])
    input_paths.extend(Path(__file__).with_name(name) for name in ('inspect_release.py','canonicalize_helpers.py','unify_scalar_helpers.py'))
    input_paths.extend(Path(path) for path in ('manalink.h','defs.h','Makefile','Makefile-common','Makefile-targets','tools/build-recovery.ps1'))
    input_paths.extend([Path(capstone.__file__),Path(capstone._cs._name)])
    input_paths.extend(Path(row['source']) for row in rows)
    input_state={str(path):sha(path.read_bytes()) for path in input_paths}
    assert input_state['ManalinkEh.dll']==build['dll_sha256']
    assert input_state['ManalinkEh.dbg']==build.get('debug_sha256',build.get('dbg_sha256'))
    images=[Image(Path(path).read_bytes()) for path in ('.recovery/2022-ManalinkEh.dll','ManalinkEh.dll')]
    debugs=[Image(Path(path).read_bytes()) for path in ('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg','ManalinkEh.dbg')]
    tables=[];public=[]
    for debug in debugs:
        table=collections.defaultdict(set);symbols=collections.defaultdict(set)
        for n,o,s,k,t in debug.symbols:
            if s>0:
                address=0x2000000+debug.pe.sections[s-1].VirtualAddress+o
                table[n.lstrip('_')].add(address)
                if t==2 and k==32:symbols[address].add(n.lstrip('_'))
        tables.append(table);public.append(symbols)
    reviewed=[];unresolved=[]
    for row in rows:
        try:
            maps=[];bases=[];ranges=[];starts=[];identity=None;incoming_reg=None
            for side in (0,1):
                assert len(tables[side][row['name']])==1
                base=next(iter(tables[side][row['name']]))
                next_entry=min(a for values in tables[side].values() for a in values if a>base)
                size=row['native_size'] if side==0 else next_entry-base
                raw=images[side].pe.get_data(base-0x2000000,size)
                code={i.address:i for i in md.disasm(raw,base)};maps.append(code);bases.append(base)
                ranges.append(dict(side=side,address=hex(base),size=size,sha256=sha(raw)))
                discovery_base=int(row['address'] if side==0 else row['recomp'],16)
                slots=[base+int(b['original' if side==0 else 'rebuilt'][0][0],16)-discovery_base for b in row['changes']]
                assert len(slots)==2 and all(code[a].mnemonic=='cmp' for a in slots)
                assert sorted(code[a].operands[1].imm for a in slots)==[0x400,0x5400]
                assert code[slots[0]].operands[1].imm==(0x400 if side==0 else 0x5400)
                this_identity=operand(code[slots[0]],code[slots[0]].operands[0])
                assert all(operand(code[a],code[a].operands[0])==this_identity for a in slots)
                if identity is None:identity=this_identity
                assert identity==this_identity
                starts.append(slots[0])
                n=code[slots[0]].address+code[slots[0]].size;following=code[n]
                if following.mnemonic in ('sete','setne'):
                    assigned=regname(following,following.operands[0]);combine=code[following.address+following.size]
                    assert combine.mnemonic in ('test','or')
                    other=[regname(combine,op) for op in combine.operands if regname(combine,op)!=assigned]
                    assert len(other)==1
                    if incoming_reg is None:incoming_reg=other[0]
                    assert incoming_reg==other[0]
            destinations={};cases=0;live_proofs={}
            for event in (0x400,0x5400,0x80000000):
                for incoming in (range(256) if incoming_reg else (0,)):
                    outcomes=[walk(code,start,identity,event,incoming,incoming_reg) for code,start in zip(maps,starts)]
                    a,b=outcomes
                    assert a[0]-bases[0]==b[0]-bases[1],('routing',event,incoming,hex(a[0]),hex(b[0]))
                    relative=a[0]-bases[0];destinations.setdefault(relative,set()).add(event)
                    differences={root:a[1][root]^b[1][root] for root in a[1] if a[1][root]!=b[1][root]}
                    for side,outcome in enumerate(outcomes):
                        ok,temp_trace=temps_dead(maps[side],outcome[0],differences,public[side]);assert ok,('live temporary',side,temp_trace)
                        ok,flag_trace=flags_dead(maps[side],outcome[0],public[side]);assert ok,('live flags',side,flag_trace)
                        live_proofs[(side,relative,tuple(sorted(differences.items())))]=dict(side=side,continuation=hex(outcome[0]),differences=differences,temp_trace=temp_trace,flag_trace=flag_trace)
                    cases+=1
            source=read(row['source']);body=source[slice(*span(source,row['name']))]
            reviewed.append(dict(name=row['name'],address=row['address'],source=row['source'],source_definition_sha256=sha(body.encode()),
                native_and_rebuilt_ranges=ranges,event_operand=identity,incoming_byte=incoming_reg,cases=cases,
                continuations=[dict(offset=hex(k),events=[hex(x) if x!=0x80000000 else 'all other 32-bit values' for x in sorted(v)]) for k,v in sorted(destinations.items())],
                liveness=list(live_proofs.values()),discovery_changes=row['changes']))
        except (AssertionError,ValueError,KeyError) as error:
            unresolved.append(dict(name=row['name'],reason=str(error)))
    assert all(sha(Path(path).read_bytes())==expected for path,expected in input_state.items()),'Review input changed during analysis'
    result=dict(format=1,reviewed=reviewed,unresolved=unresolved,inputs=input_state,decoder_version=capstone.__version__,
        discovery_report_sha256=sha(report_path.read_bytes()),queue_sha256=sha(queue_path.read_bytes()),build_evidence=dict(path=str(args.build_evidence),sha256=sha(args.build_evidence.read_bytes())),script_sha256=sha(Path(__file__).read_bytes()),
        scope='Local event-comparison permutation only. Guard instructions consume equality/zero flags and compare only the same unchanged operand against 0x400/0x5400; these two values plus their complement partition all 32-bit inputs. Every incoming Boolean-byte pattern is covered. '
              'Both sides reach the same continuation and differing temporaries/flags must die before use; whitelisted cross-CU public C calls use the default stack ABI and clobber caller-saved temporaries. '
              'This neither certifies whole handler bodies nor hides other call/global-identity differences. Source is unchanged and unresolved cases remain in the queue.')
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(dict(reviewed=len(reviewed),unresolved=unresolved,cases=sum(r['cases'] for r in reviewed)),indent=2))

if __name__=="__main__":
    main()
