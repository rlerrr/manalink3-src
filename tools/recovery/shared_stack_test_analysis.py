"""Read scalar frame writes to the named parameter region of test definitions."""
import collections
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86 import X86_OP_REG,X86_OP_MEM,X86_OP_IMM,X86_REG_EBP
from elftools.dwarf.dwarf_expr import DWARFExprParser
from inspect_release import value,inherited_value
from global_initializers import inherited_type
from object_initializers import unqualify,sizeof
from verify_inline_helpers import ranges
from canonicalize_helpers import sha


def walk(d):
    for c in d.iter_children():
        yield c
        if c.has_children:yield from walk(c)


def frame_offset(dw,die):
    location=die.attributes.get('DW_AT_location')
    assert location and location.form=='DW_FORM_exprloc'
    expression=DWARFExprParser(dw.structs).parse_expr(location.value)
    assert len(expression)==1 and expression[0].op_name=='DW_OP_fbreg',expression
    return expression[0].args[0]


def test_layout(t):
    t=unqualify(t)
    assert value(t,'DW_AT_byte_size')==216
    fields=[]
    for f in t.iter_children():
        if f.tag=='DW_TAG_member':
            fields.append(dict(name=value(f,'DW_AT_name'),offset=value(f,'DW_AT_data_member_location'),
                               size=sizeof(inherited_type(f))))
    assert next(f for f in fields if f['name']=='type_flag')==dict(name='type_flag',offset=108,size=2)
    return fields


def compiled_functions(debug,records):
    dw=debug.dwarf();wanted={r['name']:r for r in records};result={}
    for cu in dw.iter_CUs():
        top=cu.get_top_DIE()
        source=(value(top,'DW_AT_name') or '').replace('\\','/')
        if not any(source.endswith(r['source']) for r in records):continue
        for d in top.iter_children():
            name=inherited_value(d,'DW_AT_name')
            if d.tag!='DW_TAG_subprogram' or name not in wanted or not ranges(dw,d):continue
            assert name not in result
            variables=[v for v in walk(d) if v.tag=='DW_TAG_variable' and
                       inherited_value(v,'DW_AT_name')==wanted[name]['storage']['storage']]
            assert len(variables)==1,(name,len(variables))
            union=unqualify(inherited_type(variables[0]))
            member_name=wanted[name].get('test_member','test')
            members=[m for m in union.iter_children() if m.tag=='DW_TAG_member' and value(m,'DW_AT_name')==member_name]
            assert len(members)==1 and value(members[0],'DW_AT_data_member_location',0)==0
            assert value(d,'DW_AT_frame_base')==[156]
            result[name]=dict(die=d,frame_offset=frame_offset(dw,variables[0]),layout=test_layout(inherited_type(members[0])))
    assert set(result)==set(wanted),(set(wanted)-set(result))
    return dw,result


FAMILIES={}
for base,short,low,high in [('eax','ax','al','ah'),('ebx','bx','bl','bh'),('ecx','cx','cl','ch'),('edx','dx','dl','dh')]:
    FAMILIES.update({base:(base,32,0),short:(base,16,0),low:(base,8,0),high:(base,8,8)})
for base in ['esi','edi','ebp','esp']:
    FAMILIES[base]=(base,32,0)


def collect(image,dw,die,cfa_offset,layout,*,start=100,end=216):
    assert 0<=start<end
    parts=ranges(dw,die);assert len(parts)==1
    lo,hi=parts[0];base=image.pe.OPTIONAL_HEADER.ImageBase
    data=image.pe.get_data(lo-base,hi-lo)
    md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True
    instructions=list(md.disasm(data,lo));assert sum(i.size for i in instructions)==hi-lo
    assert any(i.mnemonic=='mov' and i.op_str=='ebp, esp' for i in instructions[:8])
    leaders={lo}
    for i in instructions:
        if i.mnemonic.startswith('j'):
            if i.operands[0].type==X86_OP_IMM:leaders.add(i.operands[0].imm)
            leaders.add(i.address+i.size)
    known={}
    def get(reg):
        family=FAMILIES.get(md.reg_name(reg))
        if not family:return None
        name,bits,shift=family;mask=((1<<bits)-1)<<shift
        current=known.get(name)
        if current is None or current[0]&mask!=mask:return None
        return (current[1]>>shift)&((1<<bits)-1)
    def put(reg,val):
        family=FAMILIES.get(md.reg_name(reg))
        if not family:return
        name,bits,shift=family;mask=((1<<bits)-1)<<shift
        old=known.get(name,(0,0))
        known[name]=((old[0]&~mask)|(mask if val is not None else 0),
                     (old[1]&~mask)|(((val or 0)<<shift)&mask))
    stores=[]
    for i in instructions:
        if i.address in leaders:known.clear()
        ops=i.operands
        if i.mnemonic=='call':
            for n in ['eax','ecx','edx']:known.pop(n,None)
            continue
        if len(ops)==2 and ops[0].type==X86_OP_MEM and i.mnemonic in ['mov','and','or','add','sub']:
            memory=ops[0].mem
            offset=memory.disp-8-cfa_offset
            if memory.base==X86_REG_EBP and memory.index==0 and start<=offset<end:
                val=ops[1].imm if ops[1].type==X86_OP_IMM else get(ops[1].reg) if ops[1].type==X86_OP_REG else None
                if i.mnemonic!='mov':val=None
                stores.append(dict(address=hex(i.address),offset=offset,size=ops[0].size,
                    value=None if val is None else val&((1<<(ops[0].size*8))-1),
                    instruction=[i.mnemonic,i.op_str]))
        if ops and ops[0].type==X86_OP_REG:
            if i.mnemonic=='mov' and len(ops)==2:
                put(ops[0].reg,ops[1].imm if ops[1].type==X86_OP_IMM else get(ops[1].reg) if ops[1].type==X86_OP_REG else None)
            elif i.mnemonic=='xor' and len(ops)==2 and ops[1].type==X86_OP_REG and ops[0].reg==ops[1].reg:
                put(ops[0].reg,0)
            elif i.mnemonic not in ['cmp','test','push']:
                put(ops[0].reg,None)
    events=collections.Counter()
    for store in stores:
        for field in layout:
            if field['offset']<start:continue
            overlap_start=max(store['offset'],field['offset']);overlap_end=min(store['offset']+store['size'],field['offset']+field['size'])
            for offset in range(overlap_start,overlap_end):
                val=None if store['value'] is None else (store['value']>>(8*(offset-store['offset'])))&255
                events[(field['name'],offset-field['offset'],val)]+=1
    return dict(address=hex(lo),end=hex(hi),bytes_sha256=sha(data),
        instructions=[[hex(i.address),i.mnemonic,i.op_str] for i in instructions],stores=stores,
        byte_events=[dict(field=k[0],byte=k[1],value=k[2],count=v)
                     for k,v in sorted(events.items(),key=lambda kv:(kv[0][0],kv[0][1],str(kv[0][2])))])


def compare_parameter_stores(name,native,current,image):
    if native['byte_events']==current['byte_events']:
        return dict(kind='exact_scalar_byte_events')
    # GCC duplicated Guild Summit's single source block for its two event
    # paths. Accept precisely this case only after proving that neither store
    # can reach the other within this function's control-flow graph.
    assert name=='card_guild_summit',name
    assert len(native['stores'])==1 and len(current['stores'])==2
    assert all((s['offset'],s['size'],s['value'])==(104,4,0x500c) for s in native['stores']+current['stores'])
    left=[dict(r,count=r['count']*2) for r in native['byte_events']]
    assert left==current['byte_events']
    lo,hi=int(current['address'],16),int(current['end'],16)
    data=image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo)
    md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True
    instructions={i.address:i for i in md.disasm(data,lo)}
    assert not any(i.mnemonic.startswith('loop') for i in instructions.values())
    graph={}
    for at,i in instructions.items():
        fallthrough=at+i.size;edges=[]
        if i.mnemonic.startswith('ret'):
            pass
        elif i.mnemonic.startswith('j'):
            assert i.operands[0].type==X86_OP_IMM,'Indirect jump needs independent CFG analysis'
            target=i.operands[0].imm
            if target in instructions:edges.append(target)
            if i.mnemonic!='jmp' and fallthrough in instructions:edges.append(fallthrough)
        elif fallthrough in instructions:
            edges.append(fallthrough)
        graph[at]=edges
    sites=[int(s['address'],16) for s in current['stores']]
    def reachable(start,target):
        todo=list(graph[start]);seen=set()
        while todo:
            at=todo.pop()
            if at==target:return True
            if at in seen:continue
            seen.add(at);todo.extend(graph[at])
        return False
    assert not reachable(sites[0],sites[1]) and not reachable(sites[1],sites[0])
    return dict(kind='mutually_exclusive_guild_summit_compiler_clone',native_store_sites=1,
                compiled_store_sites=2,sites=[hex(a) for a in sites])


def dynamic_value_flow(name,view,image,debug):
    """Prove the two recovered return-value expressions in emitted code."""
    assert name in ['card_torgaar_famine_incarnate','card_glissa_sunseeker']
    lo,hi=int(view['address'],16),int(view['end'],16)
    md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True
    ins=list(md.disasm(image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo),lo))
    offset=174 if name=='card_torgaar_famine_incarnate' else 128
    stores=[s for s in view['stores'] if s['offset']==offset and s['value'] is None]
    assert len(stores)==1
    site=int(stores[0]['address'],16);index=next(j for j,i in enumerate(ins) if i.address==site)
    def writes(i,names):
        return any(md.reg_name(r) in names for r in i.regs_access()[1])
    if name=='card_glissa_sunseeker':
        call_index=max(j for j in range(index) if ins[j].mnemonic=='call')
        assert ins[call_index].op_str=='0x49d510'
        window=ins[call_index+1:index]
        changes=[i for i in window if writes(i,{'eax','ax','al','ah'})]
        assert len(changes)==1 and changes[0].mnemonic=='inc' and changes[0].op_str=='eax'
        assert ins[index].mnemonic=='mov' and ins[index].op_str.endswith(', eax')
        args=[i.op_str for i in ins[max(0,call_index-8):call_index] if i.mnemonic=='mov']
        assert 'dword ptr [esp + 8], 0xffffffff' in args and 'dword ptr [esp + 4], 7' in args
        return dict(kind='has_mana_player_total_probe_plus_one',call=hex(ins[call_index].address),
                    increment=hex(changes[0].address),store=hex(site),field_offset=128,width=4)
    # Native and rebuilt code use the same signed-CMP/JLE/select shape.
    assert ins[index].mnemonic=='mov' and ins[index].op_str.endswith(', bx')
    assert ins[index-1].mnemonic=='mov' and ins[index-1].op_str=='ebx, eax'
    assert ins[index-2].mnemonic=='jle' and ins[index-2].operands[0].imm==site
    assert ins[index-3].mnemonic=='mov'
    assert not writes(ins[index-3],{'eax','ax','al','ah','ebx','bx','bl','bh'})
    assert ins[index-4].mnemonic=='cmp' and ins[index-4].op_str=='ebx, eax'
    calls=[(j,i) for j,i in enumerate(ins[:index]) if i.mnemonic=='call'][-3:]
    expected=['round_up_value','new_default_test_definition','max_can_sacrifice_as_cost']
    assert len(calls)==3
    for (_,call),callee in zip(calls,expected):
        addresses={image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[s-1].VirtualAddress+v
                   for n,v,s,k,c in debug.symbols if n=='_'+callee and s>0 and k==32}
        assert len(addresses)==1 and call.operands[0].type==X86_OP_IMM and call.operands[0].imm in addresses
    first,second,third=[j for j,i in calls]
    captures=[j for j in range(first+1,second) if ins[j].mnemonic=='mov' and ins[j].op_str=='ebx, eax']
    assert len(captures)==1
    assert not any(writes(i,{'eax','ax','al','ah'}) for i in ins[first+1:captures[0]])
    assert not any(writes(i,{'ebx','bx','bl','bh'}) for i in ins[captures[0]+1:index-4] if i.mnemonic!='call')
    assert not any(writes(i,{'eax','ax','al','ah'}) for i in ins[third+1:index-4])
    return dict(kind='signed_minimum_then_uint16',calls=[dict(name=n,address=hex(i.address)) for n,(j,i) in zip(expected,calls)],
                comparison=hex(ins[index-4].address),select_branch=hex(ins[index-2].address),
                store=hex(site),field_offset=174,width=2)
