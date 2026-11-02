"""Bounded frame-store/value analysis for target flags and sacrifice tests."""
import collections
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86 import X86_OP_REG,X86_OP_MEM,X86_OP_IMM,X86_REG_EBP
from elftools.dwarf.dwarf_expr import DWARFExprParser
from canonicalize_helpers import sha
from inspect_release import value,inherited_value
from global_initializers import inherited_type
from object_initializers import unqualify,sizeof,member_offset
from shared_stack_test_analysis import walk,frame_offset,collect,test_layout,compiled_functions,FAMILIES
from shared_stack_record_analysis import layout
from verify_inline_helpers import ranges
from verify_split_cards_recovery import contract
from verify_wide_stack_test_recovery import graph_for


CASES={
    'autoselect_target':'functions/targets.c',
    'this_effect_doesnt_remove_this':'functions/auras.c',
    **{n:'functions/equipments.c' for n in ['altar_equipment','basic_equipment',
        'resolve_activation_basic_equipment','activate_basic_equipment',
        'can_activate_basic_equipment','check_for_equipment_targets']},
    'pick_special_permanent_for_sacrifice':'functions/sacrifices.c',
}


def is_target_type(t):
    t=unqualify(t)
    if t is None or t.tag!='DW_TAG_structure_type' or sizeof(t)!=84:return False
    fields={value(f,'DW_AT_name'):f for f in t.iter_children() if f.tag=='DW_TAG_member'}
    return {'required_type','allowed_controller','preferred_controller','flags'}<=fields.keys() and \
        member_offset(fields['flags'])==80 and sizeof(inherited_type(fields['flags']))==4


def census(image,debug):
    """Enumerate direct EBP writes and explicitly classify location-less locals.

    This intentionally does not infer target aliases, pointer stores or storage
    for declarations whose optimized debug records have no emitted location.
    """
    dw=debug.dwarf();cache={};count=0;flag_writers=[];other=[];split=[]
    md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True
    for cu in dw.iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')
        if not source.startswith(('cards/','functions/')):continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag!='DW_TAG_subprogram':continue
            parts=ranges(dw,die)
            if not parts:continue
            targets=[]
            for variable in walk(die):
                if variable.tag!='DW_TAG_variable':continue
                t=unqualify(inherited_type(variable))
                if t is None:continue
                if t.offset not in cache:cache[t.offset]=is_target_type(t)
                if not cache[t.offset]:continue
                try:offset=frame_offset(dw,variable)
                except AssertionError:
                    # The release's entire skipped set has no location. Fail
                    # rather than silently ignore a new list/expression shape.
                    assert 'DW_AT_location' not in variable.attributes
                    other.append(dict(source=source,function=inherited_value(die,'DW_AT_name'),
                        function_die=hex(die.offset),variable=inherited_value(variable,'DW_AT_name'),
                        variable_die=hex(variable.offset),category='no_emitted_location'))
                    continue
                targets.append(dict(name=inherited_value(variable,'DW_AT_name'),die_offset=hex(variable.offset),frame_offset=offset))
            if not targets:continue
            count+=1
            if len(parts)!=1:
                split.append(dict(source=source,name=inherited_value(die,'DW_AT_name'),ranges=parts));continue
            lo,hi=parts[0];raw=image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo)
            instructions=list(md.disasm(raw,lo));assert sum(i.size for i in instructions)==len(raw)
            writes=[]
            for i in instructions:
                if not i.operands or i.operands[0].type!=X86_OP_MEM or not i.operands[0].access&2:continue
                op=i.operands[0];m=op.mem
                if m.base!=X86_REG_EBP or m.index:continue
                variables=[v for v in targets if m.disp<v['frame_offset']+8+84 and v['frame_offset']+8+80<m.disp+op.size]
                if variables:writes.append(dict(address=hex(i.address),instruction=[i.mnemonic,i.op_str],variables=variables))
            if writes:flag_writers.append(dict(source=source,name=inherited_value(die,'DW_AT_name'),
                die_offset=hex(die.offset),address=hex(lo),end=hex(hi),bytes_sha256=sha(raw),writes=writes))
        cu._dielist.clear();cu._diemap.clear()
    return dict(fixed_frame_target_functions=count,flag_writers=flag_writers,other_locations=other,split_functions=split)


def cases(image,debug,*,native=False):
    dw=debug.dwarf();result={}
    for cu in dw.iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')
        if source not in set(CASES.values()):continue
        for die in cu.get_top_DIE().iter_children():
            name=inherited_value(die,'DW_AT_name')
            if die.tag!='DW_TAG_subprogram' or CASES.get(name)!=source or not ranges(dw,die):continue
            assert name not in result
            variables=[v for v in walk(die) if v.tag=='DW_TAG_variable' and inherited_value(v,'DW_AT_name')=='td']
            assert len(variables)==1 and is_target_type(inherited_type(variables[0]))
            assert value(die,'DW_AT_frame_base')==[156]
            offset=frame_offset(dw,variables[0]);shape=layout(inherited_type(variables[0]))
            flags=collect(image,dw,die,offset,shape['fields'],start=80,end=84)
            row=dict(source=source,name=name,die_offset=hex(die.offset),contract=contract(dw,die),
                td_frame_offset=offset,td_layout=shape,flags=flags)
            row['flag_values']=store_values(image,dw,die,flags,offset,flags['stores'])
            if name=='basic_equipment':row['exclusive_flag_stores']=exclusive_sites(image,flags)
            if name=='pick_special_permanent_for_sacrifice':
                if native:
                    variables=[v for v in walk(die) if v.tag=='DW_TAG_variable' and
                        unqualify(inherited_type(v)).tag=='DW_TAG_structure_type' and sizeof(inherited_type(v))==216]
                    assert len(variables)==1
                    test_offset=frame_offset(dw,variables[0]);fields=test_layout(inherited_type(variables[0]))
                    row['native_test_variable']=inherited_value(variables[0],'DW_AT_name')
                else:
                    typed=[v for v in walk(die) if v.tag=='DW_TAG_variable' and unqualify(inherited_type(v)).tag=='DW_TAG_structure_type' and sizeof(inherited_type(v))==216]
                    if typed:
                        assert len(typed)==1 and inherited_value(typed[0],'DW_AT_name')=='test'
                        test_offset=frame_offset(dw,typed[0]);fields=test_layout(inherited_type(typed[0]))
                        row['current_test_variable']='test'
                    else:
                        _,functions=compiled_functions(debug,[dict(source=source,name=name,storage=dict(storage='storage'),test_member='test')])
                        test_offset=functions[name]['frame_offset'];fields=functions[name]['layout']
                row['test_frame_offset']=test_offset;row['test_layout']=fields
                aliases=[]
                for variable in walk(die):
                    if variable.tag!='DW_TAG_variable':continue
                    try:alias_offset=frame_offset(dw,variable)
                    except AssertionError:continue
                    if alias_offset!=test_offset:continue
                    t=unqualify(inherited_type(variable))
                    alias=dict(name=inherited_value(variable,'DW_AT_name'),size=sizeof(t),kind=t.tag)
                    if t.tag=='DW_TAG_union_type':
                        alias['members']=[dict(name=value(m,'DW_AT_name'),offset=member_offset(m),size=sizeof(inherited_type(m)))
                            for m in t.iter_children() if m.tag=='DW_TAG_member']
                    aliases.append(alias)
                row['test_storage_aliases']=aliases
                row['test']=collect(image,dw,die,test_offset,fields)
                row['test_values']=store_values(image,dw,die,row['test'],offset,row['test']['stores'])
            result[name]=row
        cu._dielist.clear();cu._diemap.clear()
    assert set(result)==set(CASES),set(CASES)-set(result)
    return result


def decode(image,view):
    lo,hi=int(view['address'],16),int(view['end'],16)
    md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True
    instructions=list(md.disasm(image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo),lo))
    assert sum(i.size for i in instructions)==hi-lo
    return md,instructions


def store_values(image,dw,die,view,target_frame,stores):
    """Trace every stored bit within a single basic block, failing on unknowns.

    MOV/register aliases and immediate OR/AND are the supported expressions.
    Input stack slots must be fixed DWARF formal locations and cannot be
    directly overwritten or have their address taken anywhere in the body.
    Old target bits are read explicitly; no call may intervene in a slice.
    """
    md,ins=decode(image,view);graph=graph_for(ins);incoming=collections.Counter(b for edges in graph.values() for b in edges)
    parser=DWARFExprParser(dw.structs);parameters={}
    for parameter in die.iter_children():
        if parameter.tag!='DW_TAG_formal_parameter':continue
        loc=parameter.attributes.get('DW_AT_location')
        if loc is None or loc.form!='DW_FORM_exprloc':continue
        expression=parser.parse_expr(loc.value)
        if len(expression)==1 and expression[0].op_name=='DW_OP_fbreg':
            at=expression[0].args[0]+8;size=sizeof(inherited_type(parameter))
            for byte in range(size):parameters[at+byte]=(inherited_value(parameter,'DW_AT_name'),byte)
    result=[]
    for store in stores:
        idx=next(j for j,i in enumerate(ins) if hex(i.address)==store['address']);trail=set()
        low=idx
        while low>0 and graph[ins[low-1].address]==[ins[low].address] and incoming[ins[low].address]==1:low-=1
        def bit(before,op,n):
            assert n<op.size*8
            if op.type==X86_OP_IMM:return str((op.imm>>n)&1)
            if op.type==X86_OP_MEM:
                assert op.mem.base==X86_REG_EBP and not op.mem.index
                byte=op.mem.disp+n//8
                if byte in parameters:
                    name,b=parameters[byte]
                    for i in ins:
                        if i.mnemonic=='lea' and len(i.operands)==2:
                            m=i.operands[1]
                            assert not(m.type==X86_OP_MEM and m.mem.base==X86_REG_EBP and not m.mem.index and m.mem.disp==byte)
                        for written in i.operands:
                            if written.type==X86_OP_MEM and written.access&2 and written.mem.base==X86_REG_EBP and not written.mem.index:
                                assert not written.mem.disp<=byte<written.mem.disp+written.size
                    return 'parameter:'+name+':bit'+str(8*b+n%8)
                offset=byte-8-target_frame
                assert 80<=offset<84,(op.mem.disp,offset)
                return 'old_flags:bit'+str((offset-80)*8+n%8)
            assert op.type==X86_OP_REG
            family,width,shift=FAMILIES[md.reg_name(op.reg)];wanted=n+shift
            for j in range(before-1,low-1,-1):
                i=ins[j];ops=i.operands
                assert i.mnemonic!='call','A value slice may not cross a call'
                written={FAMILIES[md.reg_name(r)][0] for r in i.regs_access()[1] if md.reg_name(r) in FAMILIES}
                if family not in written:continue
                assert ops and ops[0].type==X86_OP_REG,(i.mnemonic,i.op_str)
                dest=FAMILIES[md.reg_name(ops[0].reg)]
                if dest[0]!=family:raise AssertionError((i.mnemonic,i.op_str,family))
                if not dest[2]<=wanted<dest[2]+dest[1]:continue
                trail.add(j);nbit=wanted-dest[2]
                if i.mnemonic=='mov':return bit(j,ops[1],nbit)
                if i.mnemonic=='movzx':return bit(j,ops[1],nbit) if nbit<ops[1].size*8 else '0'
                if i.mnemonic in ['or','and'] and ops[1].type==X86_OP_IMM:
                    immediate=(ops[1].imm>>nbit)&1
                    if i.mnemonic=='or' and immediate:return '1'
                    if i.mnemonic=='and' and not immediate:return '0'
                    return bit(j,ops[0],nbit)
                raise AssertionError(('Unsupported bit definition',i.mnemonic,i.op_str))
            raise AssertionError(('No in-block reaching definition',family,wanted,hex(ins[before].address)))
        instruction=ins[idx];assert instruction.mnemonic=='mov' and len(instruction.operands)==2
        bits=[bit(idx,instruction.operands[1],n) for n in range(store['size']*8)]
        if trail:
            first=min(trail)
            assert not any(i.mnemonic=='call' for i in ins[first:idx+1])
            assert not any(int(s['address'],16) in {i.address for i in ins[first:idx]} for s in stores)
        result.append(dict(address=store['address'],offset=store['offset'],size=store['size'],bits=bits,
            instruction_slice=[[hex(ins[j].address),ins[j].mnemonic,ins[j].op_str] for j in sorted(trail|{idx})]))
    return result


def exclusive_sites(image,view):
    _,ins=decode(image,view);graph=graph_for(ins);sites=[int(s['address'],16) for s in view['stores']]
    for a in sites:
        seen=set();todo=list(graph[a])
        while todo:
            at=todo.pop()
            assert at not in sites,'Two target flag writes can execute in one invocation'
            if at in seen:continue
            seen.add(at);todo.extend(graph[at])
    return dict(kind='pairwise_unreachable_direct_cfg',sites=[hex(a) for a in sites])
