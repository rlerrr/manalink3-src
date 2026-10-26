"""Replay narrow counter-source witnesses from native outgoing argument blocks.

Frame variables, dialogs and unrelated caller behavior are outside this proof.
Unknown registers remain symbolic; they never become guessed source arguments.
"""
import json,re
from collections import Counter
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_REG,X86_OP_MEM
from canonicalize_helpers import ROOT,sha,mask
from inventory_inline_counter_arguments import REGS,INLINE,TARGETS,dataflow,execute,operand
from verify_inline_counter_edits import replay as replay_formals,verify_entry_inventory,eligible


def inspect(image,row,spec):
    assert len(row['native_ranges'])==1
    r=row['native_ranges'][0];lo,hi=int(r['low_pc'],16),int(r['high_pc'],16)
    native=image.pe.get_data(lo-0x2000000,hi-lo)
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    ins=list(decoder.disasm(native,lo));by={i.address:i for i in ins}
    assert ins[-1].address+ins[-1].size==hi
    start,at=int(spec['block_start'],16),int(spec['call'],16)
    call=by[at];assert call.mnemonic=='call' and call.operands[0].type==X86_OP_IMM
    assert call.operands[0].imm==TARGETS['add_counters'] and call.size==5
    sites=[s for s in row['native_sites'] if s['helper'] in TARGETS]
    calls=[i.address for i in ins if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm in TARGETS.values()]
    assert len(sites)==1 and calls==[at] and int(sites[0]['return_pc'],16)==at+5
    assert ins[0].mnemonic=='push' and ins[0].op_str=='ebp'
    assert ins[1].mnemonic=='mov' and ins[1].op_str=='ebp, esp'
    computed,status=dataflow(image,row)
    # The original incoming frame remains stable until an immediate return.
    # No local/source adapter is used to translate the caller's parameters.
    for index,i in enumerate(ins):
        writes=i.regs_access()[1]
        if any(i.reg_name(reg)=='ebp' for reg in writes) and i.address!=ins[1].address:
            assert i.mnemonic in {'pop','leave'} and ins[index+1].mnemonic=='ret',(hex(i.address),i.op_str)
        for op in i.operands:
            if op.type==X86_OP_MEM and op.access&2 and i.reg_name(op.mem.base)=='ebp' and not op.mem.index:
                if 8<=op.mem.disp<16:
                    assert computed is not None and i.mnemonic=='mov' and op.size==4
                    expected={8:('parameter','player'),12:('parameter','card')}[op.mem.disp]
                    assert operand(i,i.operands[1],computed[0][i.address])==expected,'Incoming source formal changed'
        if i.mnemonic.startswith('j') and i.operands[0].type==X86_OP_IMM:
            assert not start<i.operands[0].imm<=at,'Control flow enters argument block midway'
    # The only indirect branch among these callers is Faerie Dragon's native
    # bounded switch. Check its exact bounds and every actual table target.
    indirect=[(n,i) for n,i in enumerate(ins) if i.mnemonic.startswith('j') and i.operands[0].type!=X86_OP_IMM]
    for index,i in indirect:
        assert spec.get('switch_entries')==20 and i.mnemonic=='jmp'
        assert ins[index-2].mnemonic=='cmp' and ins[index-2].op_str=='ebx, 0x13'
        assert ins[index-1].mnemonic=='ja'
        mem=i.operands[0].mem
        assert i.operands[0].type==X86_OP_MEM and not mem.base and i.reg_name(mem.index)=='ebx' and mem.scale==4
        table=image.pe.get_data(mem.disp-0x2000000,80);assert len(table)==80
        targets=[int.from_bytes(table[n:n+4],'little') for n in range(0,80,4)]
        assert all(t in by and not start<t<=at for t in targets)
    # Bind formal values from the original physical entry locations. These
    # six card callbacks use the verified cdecl stack pair, not register debug
    # locations from an optimized interior range.
    if spec['mode']=='formal_pair':
        for name,offset in [('player',0),('card',4)]:
            p=next(p for p in r['parameters'] if p['name']==name)
            assert p['location']==[dict(operation='DW_OP_fbreg',arguments=[offset])]
    state={reg:('opaque_register',reg) for reg in REGS}
    if computed is not None:
        reached=computed[0].get(start);assert reached is not None
        for reg in REGS:
            v=reached.get(reg)
            if isinstance(v,tuple) and v[0]=='parameter':state[reg]=v
    else:assert indirect and status=='indirect branch'
    state.update(esp=('sp',0),ebp=('sp',0x10000))
    if spec['mode']=='formal_pair':
        state[('mem',('sp',0x10008))]=('parameter','player')
        state[('mem',('sp',0x1000c))]=('parameter','card')
    block=[i for i in ins if start<=i.address<at];assert block and block[0].address==start
    for i in block:
        assert i.mnemonic not in {'call','ret','leave','push','pop'} and not i.mnemonic.startswith('j')
        assert all(i.reg_name(reg) not in {'esp','ebp'} for reg in i.regs_access()[1])
        state=execute(i,state)
    observed=[state.get(('mem',('sp',offset))) for offset in range(0,24,4)]
    if spec['mode']=='formal_pair':assert observed[:2]==[('parameter','player'),('parameter','card')],observed
    else:
        assert spec['mode']=='target_pair' and all(x is not None for x in observed[:4])
        assert observed[:2]==observed[2:4],observed
    return dict(range_sha256=sha(native),block_sha256=sha(image.pe.get_data(start-0x2000000,at+5-start)),
                values=observed,site=sites[0]['die_offset'])


def replay(before,mode):
    after,edits=replay_formals(before);assert len(edits)==1
    if mode=='target_pair':
        old=edits[0][2];arguments=old[len('add_counters_from_source('):-1].split(', ')
        assert len(arguments)==6 and arguments[:2]==['player','card']
        assert all(re.fullmatch(r'[A-Za-z_]\w*',a) for a in arguments[2:4])
        new='add_counters_from_source('+', '.join(arguments[2:4]+arguments[2:])+')'
        after=before[:edits[0][0]]+new+before[edits[0][1]:]
    return after


def verify(proof,image,root=ROOT):
    receipts={r['path']:r['sha256'] for r in proof.get('interface_supporting_inputs',[])}
    cache={};count=0
    for edit in proof.get('source_edits',[]):
        witness=edit.get('native_counter_argument_block_witness')
        if not witness:continue
        path=witness['inventory']
        if path not in cache:
            assert receipts[path]==sha((root/path).read_bytes())
            report=json.loads((root/path).read_text());verify_entry_inventory(report,receipts,root)
            cache[path]={(r['source'],r['caller']):r for r in report['callers']}
        row=cache[path][(edit['source'],witness['caller'])]
        actual=inspect(image,row,witness['spec'])
        assert json.loads(json.dumps(actual))==witness['result']
        if witness['spec']['mode']=='formal_pair':
            okay,reason=eligible(row,dict(status='analyzed',sites=[dict(values=[['parameter','player'],['parameter','card']],callee='add_counters')]),edit['before'])
            assert okay,(witness['caller'],reason)
        assert replay(edit['before'],witness['spec']['mode'])==edit['after']
        assert Counter(c['helper'] for c in row['source_calls'] if c['helper'] in INLINE)==Counter({witness['old_helper']:1})
        count+=1
    inlines=[e for e in proof.get('source_edits',[]) if e.get('native_counter_inline_contract')]
    if inlines:
        from inspect_release import Image,value,inherited_value
        from export_abi import type_of,type_name
        data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
        assert sha(data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
        dwarf=Image(data).dwarf()
        for edit in inlines:
            w=edit['native_counter_inline_contract'];path=w['input']
            assert receipts[path]==sha((root/path).read_bytes())
            record=json.loads((root/path).read_text());assert record['debug_sha256']==sha(data)
            f=record['functions'][w['name']];die=dwarf.get_DIE_from_refaddr(int(f['die_offset'],16))
            params=[dict(name=inherited_value(c,'DW_AT_name'),type=type_name(type_of(c))) for c in die.iter_children() if c.tag=='DW_TAG_formal_parameter']
            assert params==f['parameters'] and inherited_value(die,'DW_AT_name')==w['name'] and type_name(type_of(die))==f['return_type']=='void'
            assert value(die.cu.get_top_DIE(),'DW_AT_name')==f['source']
            head='static inline void '+w['name']+'('+', '.join(p['type']+' '+p['name'] for p in params)+')'
            assert edit['after']==head+' { '+w['body']+' }'
    return dict(callers=count,calls=count,inline_contracts=len(inlines),scope='Exact source-pair insertion and native inline contracts only; native outgoing blocks, stable incoming frame and complete direct counter-call coverage. No other caller behavior is claimed equivalent.')
