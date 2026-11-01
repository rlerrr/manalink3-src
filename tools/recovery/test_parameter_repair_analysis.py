"""Frame pairing and direct argument provenance for plain test repairs."""
import collections
from capstone.x86 import X86_OP_MEM,X86_OP_REG,X86_REG_EBP
from canonicalize_helpers import sha
from inspect_release import value,inherited_value
from global_initializers import inherited_type
from object_initializers import unqualify,sizeof
from shared_stack_test_analysis import walk,frame_offset,test_layout,collect,FAMILIES
from verify_inline_helpers import ranges
from verify_wide_stack_test_recovery import decode,graph_for,symbol_at


def inspect(image,debug,rows):
    dw=debug.dwarf();wanted={(r['source'],r['name']):r for r in rows};result={}
    for cu in dw.iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')
        if source not in {s for s,n in wanted}:continue
        for die in cu.get_top_DIE().iter_children():
            key=(source,inherited_value(die,'DW_AT_name'))
            if die.tag!='DW_TAG_subprogram' or key not in wanted or not ranges(dw,die):continue
            assert key not in result
            assert value(die,'DW_AT_frame_base')==[156]
            if key[1]=='card_saheeli_rai':
                parameters=[d for d in die.iter_children() if d.tag=='DW_TAG_formal_parameter' and inherited_value(d,'DW_AT_name')=='card']
                assert len(parameters)==1 and frame_offset(dw,parameters[0])==4 and sizeof(inherited_type(parameters[0]))==4
            variables=[v for v in walk(die) if v.tag=='DW_TAG_variable' and
                inherited_value(v,'DW_AT_name')==wanted[key]['variable'] and
                unqualify(inherited_type(v)).tag=='DW_TAG_structure_type' and sizeof(inherited_type(v))==216]
            assert len(variables)==1
            variable=variables[0];offset=frame_offset(dw,variable);fields=test_layout(inherited_type(variable))
            result[key]=dict(die_offset=hex(die.offset),frame_offset=offset,layout=fields,
                view=collect(image,dw,die,offset,fields))
        cu._dielist.clear();cu._diemap.clear()
    assert set(result)==set(wanted),set(wanted)-set(result)
    return result


def events(view,image,debug):
    patches={}
    for store in view['stores']:
        if store['offset']==196 and store['size']==4 and store['value'] not in [None,0]:
            name=symbol_at(image,debug,store['value']);assert name=='_test_not_in_array3'
            for b in range(4):patches[('special_selection_function',b,store['value']>>(8*b)&255)]=name+':byte'+str(b)
    return collections.Counter({(r['field'],r['byte'],patches.get((r['field'],r['byte'],r['value']),r['value'])):r['count']
        for r in view['byte_events']})


def card_argument_proof(view,image,frame):
    """Prove Saheeli's storage field receives the unchanged card argument.

    Conservative direct-CFG register provenance. Calls preserve only x86
    callee-save registers; unsupported definitions and partial writes kill
    provenance. Every incoming path must carry the same card argument.
    """
    md,ins=decode(view,image);graph=graph_for(ins);by_at={i.address:i for i in ins}
    for i in ins:
        for op in i.operands:
            if op.type==X86_OP_MEM and op.mem.base==X86_REG_EBP and not op.mem.index and op.access&2:
                assert not (op.mem.disp<16 and 12<op.mem.disp+op.size)
        if i.mnemonic=='lea' and len(i.operands)==2:
            op=i.operands[1]
            assert not(op.type==X86_OP_MEM and op.mem.base==X86_REG_EBP and not op.mem.index and op.mem.disp==12)
    unknown=frozenset({'unknown'});states={ins[0].address:{}};todo=collections.deque([ins[0].address]);updates=0
    def family(reg):return FAMILIES.get(md.reg_name(reg))
    def operand(state,op):
        if op.type==X86_OP_MEM and op.size==4 and op.mem.base==X86_REG_EBP and not op.mem.index and op.mem.disp==12:
            return frozenset({'formal:card'})
        if op.type==X86_OP_REG:
            f=family(op.reg)
            if f and f[1:]==(32,0):return state.get(f[0],unknown)
        return unknown
    while todo:
        at=todo.popleft();i=by_at[at];state=dict(states[at]);ops=i.operands
        written={family(r)[0] for r in i.regs_access()[1] if family(r)}
        if i.mnemonic=='call':
            for name in ['eax','ecx','edx']:state[name]=unknown
        else:
            for name in written:state[name]=unknown
            if i.mnemonic=='mov' and len(ops)==2 and ops[0].type==X86_OP_REG:
                f=family(ops[0].reg)
                if f and f[1:]==(32,0):state[f[0]]=operand(states[at],ops[1])
        for dest in graph[at]:
            if dest not in states:states[dest]=dict(state);todo.append(dest);continue
            previous=states[dest];keys=set(previous)|set(state)
            merged={k:previous.get(k,unknown)|state.get(k,unknown) for k in keys}
            if merged!=previous:states[dest]=merged;todo.append(dest)
        updates+=1;assert updates<100000
    stores=[s for s in view['stores'] if s['offset']==212]
    assert len(stores)==1 and stores[0]['size']==4
    store=stores[0];i=by_at[int(store['address'],16)]
    assert i.mnemonic=='mov' and i.operands[0].mem.disp==frame+8+212
    assert operand(states[i.address],i.operands[1])==frozenset({'formal:card'})
    return dict(kind='all_direct_cfg_paths_preserve_card_formal',store=store['address'],
        frame_argument_displacement=12,storage_offset=212,width=4,visited_instructions=len(states))
