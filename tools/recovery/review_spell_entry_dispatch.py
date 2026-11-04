"""Check only spell entry dispatch and the first target-definition call.

This deliberately does not certify the remaining handler body. The finite event
partition is complete because every event operation accepted here is equality
against one of the five listed constants. Player/controller and card/affected
card equalities are independent symbolic predicates, rather than sampled IDs.
"""
import argparse
import collections
import json
import sys
from pathlib import Path
import capstone

sys.path.insert(0, 'tools/recovery')
from canonicalize_helpers import read, sha, span
from inspect_release import Image
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM, X86_OP_MEM, X86_OP_REG

EVENTS = (0x74, 0x6c, 0x71, 0x400, 0x5400)
OTHER = ('other_event',)
P, C = ('player',), ('card',)
AC, AP = ('affected_card',), ('affected_controller',)
ALIASES = {}
for root, word, low, high in [('eax','ax','al','ah'), ('ebx','bx','bl','bh'),
                               ('ecx','cx','cl','ch'), ('edx','dx','dl','dh')]:
    for name, bits, shift in [(root,32,0), (word,16,0), (low,8,0), (high,8,8)]:
        ALIASES[name] = (root,bits,shift)
for root, word in [('esi','si'), ('edi','di'), ('ebp','bp'), ('esp','sp')]:
    ALIASES[root] = (root,32,0)
    ALIASES[word] = (root,16,0)


def add(a, b):
    if b == 0:
        return a
    if isinstance(a, int) and isinstance(b, int):
        return (a+b) & 0xffffffff
    assert isinstance(a, tuple) and a[0] == 'stack' and isinstance(b,int), 'Non-stack address arithmetic'
    return ('stack',a[1]+b)


def extract(value, bits, shift=0):
    if bits == 32 and shift == 0:
        return value
    if isinstance(value,int):
        return (value >> shift) & ((1 << bits)-1)
    if value[0] == 'insert' and value[3:] == (bits,shift):
        return value[2]
    raise AssertionError('Unresolved partial-register read')


def walk(code, start, event, player_equal, card_equal, target):
    regs = {name:('initial',name) for name in ['eax','ebx','ecx','edx','esi','edi','ebp']}
    regs['esp'] = ('stack',0)
    initial = dict(regs)
    memory = {('stack',4):P, ('stack',8):C, ('stack',12):event,
              ('absolute',0x7a398c):AC, ('absolute',0x738c84):AP}
    zero = None
    seen_events = set()
    path = []

    def getreg(name):
        root,bits,shift = ALIASES[name]
        return extract(regs[root],bits,shift)

    def setreg(name,value):
        root,bits,shift = ALIASES[name]
        if bits == 32:
            regs[root] = value
        else:
            assert isinstance(value,int) and 0 <= value < (1 << bits), 'Nonconstant partial write'
            old = regs[root]
            regs[root] = ((old & ~(((1 << bits)-1) << shift)) | (value << shift)) if isinstance(old,int) else ('insert',old,value,bits,shift)

    def address(ins, op):
        m = op.mem
        assert m.segment == 0, 'Segment override'
        if not m.base and not m.index:
            return ('absolute',m.disp & 0xffffffff)
        value = getreg(ins.reg_name(m.base)) if m.base else 0
        if m.index:
            index = getreg(ins.reg_name(m.index))
            assert isinstance(index,int), 'Symbolic indexed address'
            value = add(value,index*m.scale)
        return add(value,m.disp)

    def load(ins,op):
        if op.type == X86_OP_REG:
            return getreg(ins.reg_name(op.reg))
        if op.type == X86_OP_IMM:
            return op.imm & 0xffffffff
        assert op.type == X86_OP_MEM and op.size == 4, 'Unsupported memory width'
        key = address(ins,op)
        assert key in memory, ('Unbound memory read',key)
        return memory[key]

    def store(ins,op,value):
        if op.type == X86_OP_REG:
            setreg(ins.reg_name(op.reg),value)
            return
        assert op.type == X86_OP_MEM and op.size == 4, 'Unsupported store'
        key = address(ins,op)
        assert key[0] == 'stack', 'Observable memory write before first call'
        assert regs['esp'][0]=='stack' and regs['esp'][1] <= key[1] and key[1]+4 <= 0, 'Store outside private callee stack frame'
        memory[key] = value

    def equal(a,b):
        if a == b:
            return True
        if {repr(a),repr(b)} == {repr(P),repr(AP)}:
            return player_equal
        if {repr(a),repr(b)} == {repr(C),repr(AC)}:
            return card_equal
        if a == OTHER or b == OTHER:
            other = b if a == OTHER else a
            assert other in EVENTS, 'Event comparison outside the finite partition'
            return False
        if isinstance(a,int) and isinstance(b,int):
            return a == b
        raise AssertionError(('Unmodeled equality',a,b))

    pc = start
    for _ in range(100):
        assert pc in code, 'Entry path left the function bounds'
        ins = code[pc]
        assert not any(ins.prefix), 'Prefixed entry instruction'
        path.append(pc)
        ops = ins.operands
        following = pc+ins.size
        if ins.mnemonic == 'push':
            value = load(ins,ops[0]);regs['esp'] = add(regs['esp'],-4);assert regs['esp'][1]<0, 'Push overwrites caller stack';memory[regs['esp']] = value
        elif ins.mnemonic == 'pop':
            assert regs['esp'][0]=='stack' and regs['esp'][1]<0 and regs['esp'] in memory, 'Unbound or caller-stack pop'
            value = memory[regs['esp']];regs['esp'] = add(regs['esp'],4);store(ins,ops[0],value)
        elif ins.mnemonic == 'leave':
            regs['esp'] = regs['ebp'];assert regs['esp'][0]=='stack' and regs['esp'][1]<0 and regs['esp'] in memory
            regs['ebp'] = memory[regs['esp']];regs['esp'] = add(regs['esp'],4)
        elif ins.mnemonic == 'mov':
            store(ins,ops[0],load(ins,ops[1]))
        elif ins.mnemonic == 'lea':
            store(ins,ops[0],address(ins,ops[1]))
        elif ins.mnemonic in ['add','sub']:
            assert ops[0].type == X86_OP_REG and ins.reg_name(ops[0].reg) == 'esp' and ops[1].type == X86_OP_IMM, 'Non-stack arithmetic'
            regs['esp'] = add(regs['esp'],ops[1].imm * (1 if ins.mnemonic == 'add' else -1));zero = None
        elif ins.mnemonic == 'xor':
            assert ops[0].type == ops[1].type == X86_OP_REG and ops[0].reg == ops[1].reg, 'Nonzero XOR'
            store(ins,ops[0],0);zero = True
        elif ins.mnemonic == 'cmp':
            a,b = load(ins,ops[0]),load(ins,ops[1])
            if ops[1].type == X86_OP_IMM:
                assert ops[1].imm in EVENTS, 'Non-partition comparison constant'
                assert a == event, 'Comparison does not read the event'
                seen_events.add(ops[1].imm)
            zero = equal(a,b)
        elif ins.mnemonic == 'test':
            a,b = load(ins,ops[0]),load(ins,ops[1]);assert isinstance(a,int) and isinstance(b,int), 'Nonconstant TEST'
            zero = (a & b) == 0
        elif ins.mnemonic in ['sete','setne']:
            assert zero is not None, 'Undefined input flags'
            store(ins,ops[0],int(zero if ins.mnemonic == 'sete' else not zero))
        elif ins.mnemonic in ['je','jne']:
            assert zero is not None and ops[0].type == X86_OP_IMM, 'Unresolved branch'
            if zero if ins.mnemonic == 'je' else not zero:
                following = ops[0].imm
        elif ins.mnemonic == 'jmp':
            assert ops[0].type == X86_OP_IMM, 'Indirect jump'
            following = ops[0].imm
        elif ins.mnemonic == 'nop':
            pass
        elif ins.mnemonic == 'call':
            assert ops[0].type == X86_OP_IMM and ops[0].imm in target, 'Different first callee'
            args = [memory[add(regs['esp'],offset)] for offset in [0,4,8,12]]
            return dict(kind='call',callee=target[ops[0].imm],args=args,stack_pointer=regs['esp'],preserved={r:regs[r] for r in ['ebx','esi','edi','ebp']},memory=sorted(memory.items(),key=repr)),path,seen_events
        elif ins.mnemonic == 'ret':
            assert not ops and regs['eax'] == 0, 'Nonzero or non-C return'
            assert regs['esp'] == ('stack',0) and all(regs[r] == initial[r] for r in ['ebx','esi','edi','ebp']), 'Return ABI differs'
            return dict(kind='return',result=0),path,seen_events
        else:
            raise AssertionError(('Unsupported entry instruction',ins.mnemonic))
        pc = following
    raise AssertionError('Entry state limit exceeded')


def prove(codes,bases,targets):
    cases=[];seen=set()
    for event in (*EVENTS,OTHER):
        for pe in [False,True]:
            for ce in [False,True]:
                states=[];paths=[]
                expected = event in (0x74,0x71,0x400,0x5400) or event == 0x6c and pe and ce
                for code,base,target in zip(codes,bases,targets):
                    state,path,observed = walk(code,base,event,pe,ce,target)
                    assert (state['kind'] == 'call') == expected, 'Spell-entry event partition changed'
                    states.append(state);paths.append([hex(a) for a in path]);seen.update(observed)
                assert states[0] == states[1], 'First call packet or private stack/preserved state differs'
                cases.append(dict(event=event,player_equal=pe,card_equal=ce,outcome=states[0]['kind'],state_sha256=sha(repr(states[0]).encode()),paths=paths))
    assert seen == set(EVENTS), 'Event partition was not fully exercised'
    return cases


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--queue',type=Path,required=True)
    parser.add_argument('--build-evidence',type=Path,required=True)
    parser.add_argument('--bounds-cache',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--groups',type=int,default=5)
    args=parser.parse_args()
    assert args.groups>0
    queuepath=args.queue;queue=json.loads(queuepath.read_bytes())
    reportpath=Path(queue['report']);receiptpath=reportpath.with_suffix('.inputs.json');receipt=json.loads(receiptpath.read_bytes())
    assert sha(reportpath.read_bytes())==receipt['report_sha256']==queue['report_sha256']
    assert sha(receiptpath.read_bytes())==queue['receipt_sha256']
    assert sha(Path(receipt['metadata_snapshot']).read_bytes())==receipt['metadata_sha256']
    build=json.loads(args.build_evidence.read_bytes())['compiled_verification']
    groups=collections.defaultdict(list)
    for row in queue['functions']:
        if not row.get('change_site_review'):groups[row['change_group']].append(row)
    selected=[r for _,rows in sorted(groups.items(),key=lambda item:len(item[1]),reverse=True)[:args.groups] for r in rows]
    assert selected
    paths=['.recovery/2022-ManalinkEh.dll','G:/Manalink/ManalinkEh_10_22_Hotfix.dbg','ManalinkEh.dll','ManalinkEh.dbg']
    inputs={p:sha(Path(p).read_bytes()) for p in paths};images=[Image(Path(paths[s*2]).read_bytes()) for s in [0,1]]
    assert inputs[paths[0]]=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert inputs[paths[1]]=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    assert build['build']=='passed' and build['fixed_entry_slots']==15169
    assert inputs[paths[2]]==build['dll_sha256'] and inputs[paths[3]]==build['debug_sha256']
    for p in [queuepath,reportpath,receiptpath,Path(receipt['metadata_snapshot']),args.build_evidence,args.bounds_cache,Path(__file__),Path('tools/recovery/canonicalize_helpers.py'),Path('tools/recovery/inspect_release.py'),Path(capstone.__file__),Path(capstone._cs._name)]+[Path(r['source']) for r in selected]:
        inputs[str(p)]=sha(p.read_bytes())
    targets=[];function_symbols=[]
    for s in [0,1]:
        dbg=Image(Path(paths[s*2+1]).read_bytes());side_targets={}
        entries=collections.defaultdict(set)
        for name,offset,section,kind,storage in dbg.symbols:
            if section>0 and kind==32:
                entries[name].add(0x2000000+dbg.pe.sections[section-1].VirtualAddress+offset)
        function_symbols.append(entries)
        for callee in ['_default_target_definition','_counterspell_target_definition']:
            values={0x2000000+dbg.pe.sections[section-1].VirtualAddress+offset for name,offset,section,kind,storage in dbg.symbols if name==callee and section>0 and kind==32};assert len(values)==1;side_targets[values.pop()]=callee
        targets.append(side_targets)
    bounds=json.loads(args.bounds_cache.read_bytes());assert bounds['sha256']==inputs[paths[3]]
    md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True;reviewed=[];unresolved=[];negative=None
    for row in selected:
        try:
            normalize=lambda source:source.replace('cards/odyssey.c','cards/odissey.c')
            entries=[r for r in bounds['functions'] if r['address']==int(row['recomp'],0) and normalize(r['source'])==normalize(row['source']) and r['name']==row['name']];assert entries and len({r['size'] for r in entries})==1
            assert function_symbols[0]['_'+row['native_name']]=={int(row['address'],0)}, 'Native source-name/COFF identity differs'
            assert function_symbols[1]['_'+row['name']]=={int(row['recomp'],0)}, 'Discovery entry moved or rebuilt source-name/COFF identity differs'
            ranges=[];codes=[];bases=[]
            for s,(base,size) in enumerate([(int(row['address'],0),row['native_size']),(int(row['recomp'],0),entries[0]['size'])]):
                raw=images[s].pe.get_data(base-0x2000000,size);code={i.address:i for i in md.disasm(raw,base)};assert sum(i.size for i in code.values())==size
                ranges.append(dict(address=hex(base),size=size,sha256=sha(raw),bytes_hex=raw.hex()));codes.append(code);bases.append(base)
            cases=prove(codes,bases,targets)
            text=read(row['source']);body=text[slice(*span(text,row['name']))]
            reviewed.append(dict(name=row['name'],address=row['address'],source=row['source'],source_definition_sha256=sha(body.encode()),group=row['change_group'],ranges=ranges,cases=cases))
            if negative is None:
                # Independent negative controls: corrupt an outgoing target type,
                # then invert the first event branch, keeping instruction sizes.
                controls=[]
                for kind in ['type_argument','event_branch','caller_stack_store']:
                    raw=bytearray.fromhex(ranges[1]['bytes_hex']);candidate=None
                    if kind in ['type_argument','caller_stack_store']:
                        candidate=next(i for i in codes[1].values() if i.mnemonic=='mov' and len(i.operands)==2 and i.operands[0].type==X86_OP_MEM and i.reg_name(i.operands[0].mem.base)=='esp' and i.operands[0].mem.disp==12 and i.operands[1].type==X86_OP_IMM)
                        if kind=='type_argument':
                            raw[candidate.address-bases[1]+candidate.imm_offset] ^= 0x40
                        else:
                            # Change the SIB base from ESP to EBP. The store now
                            # corrupts an incoming argument instead of a private
                            # outgoing slot, without changing instruction size.
                            offset=candidate.address-bases[1]+candidate.disp_offset-1
                            assert raw[offset]==0x24
                            raw[offset]=0x25
                    else:
                        candidate=next(i for i in codes[1].values() if i.mnemonic in ['je','jne']);offset=candidate.address-bases[1];raw[offset+(1 if raw[offset]==0x0f else 0)] ^= 1
                    changed={i.address:i for i in md.disasm(bytes(raw),bases[1])}
                    try:prove([codes[0],changed],bases,targets)
                    except (AssertionError,KeyError) as e:controls.append(dict(kind=kind,rejected=True,reason=str(e),instruction=hex(candidate.address)))
                    else:raise AssertionError('Negative control was accepted')
                negative=dict(name=row['name'],controls=controls)
        except (AssertionError,KeyError,StopIteration) as e:
            unresolved.append(dict(name=row['name'],address=row['address'],reason=str(e)))
    assert negative is not None and len(negative['controls'])==3 and all(r['rejected'] for r in negative['controls'])
    for path,h in inputs.items():assert sha(Path(path).read_bytes())==h,path
    result=dict(format=1,inputs=inputs,decoder_version=capstone.__version__,queue_sha256=sha(queuepath.read_bytes()),script_sha256=sha(Path(__file__).read_bytes()),first_callee_targets=[{hex(k):v for k,v in side.items()} for side in targets],selected=len(selected),reviewed=reviewed,unresolved=unresolved,negative_controls=negative,scope='Entry dispatch only, stopping before the first default_target_definition/counterspell_target_definition call or at the zero return. All five event equalities plus their full32bit complement and the independent player/controller and card/affected equalities are covered symbolically. At the first call, named callee identity, four arguments, private stack state and callee-saved registers must agree. No earlier calls or observable non-stack writes are allowed. Requires the ordinary32bit C ABI, valid nonvolatile stack and fixed engine globals, without asynchronous mutation or read/fault-order observation. This does NOT certify the rest of any handler, callee semantics or gameplay, and does not close a function-level difference review.')
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(dict(selected=len(selected),verified_entry_fragments=len(reviewed),unresolved=dict(collections.Counter(r['reason'] for r in unresolved)),negative_controls=negative),indent=2))


if __name__=='__main__':main()
