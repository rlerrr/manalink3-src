"""Inspect fixed-array declarations, scoped formatting and Multani value flow."""
import collections,re
from elftools.dwarf.locationlists import LocationParser,LocationExpr,BaseAddressEntry
from elftools.dwarf.dwarf_expr import DWARFExprParser
from capstone.x86 import X86_OP_REG,X86_OP_MEM,X86_OP_IMM,X86_REG_EBP,X86_REG_ESP
from array_role_analysis import snapshots,effects,shape
from canonicalize_helpers import sha
from global_initializers import inherited_type
from inspect_release import value,inherited_value
from object_initializers import unqualify,sizeof,member_offset
from shared_stack_test_analysis import walk,frame_offset,FAMILIES
from verify_inline_helpers import ranges
from verify_wide_stack_test_recovery import decode,graph_for

NAMES={'card_multani_yavimayas_avatar','card_faerie_dragon','put_card_on_stack',
       'resolve_trigger','new_reanimate_all_impl','card_dromokas_command'}


def source_changes(name,text):
    """Exact, bounded edits; all other source text is preserved by replay."""
    assert name in NAMES;changes=[]
    def replace(old,new,count=1):
        nonlocal text
        assert text.count(old)==count,(name,old,text.count(old),count)
        changes.append(dict(before=old,after=new,occurrences=count));text=text.replace(old,new)
    if name=='card_multani_yavimayas_avatar':
        replace('*(int *)(msg + iVar5 * 4 + -0xe) = iVar4;','to_bounce[iVar6] = iVar4;')
    elif name=='card_faerie_dragon':replace('char buf[300];','char buf[301];')
    elif name=='put_card_on_stack':
        replace('  char str[500];\n','')
        replace('      char trace[300];','      char str[500];')
        replace('sprintf(trace,','sprintf(str,')
        replace('const char *)(trace);','const char *)(str);')
        replace('      load_text(NULL, "PROMPT_CAST1");','      char str[300];\n      load_text(NULL, "PROMPT_CAST1");')
    elif name=='resolve_trigger':
        replace('  char buf[500];\n','')
        replace('  if (((uint8_t)trace_mode & 2) != 0)\n  {',
                '  if (((uint8_t)trace_mode & 2) != 0)\n  {\n    char buf[500];')
        replace('      load_text((char *)0x0, "PROMPT_PROC1");',
                '      char buf[300];\n      load_text((char *)0x0, "PROMPT_PROC1");')
    elif name=='new_reanimate_all_impl':
        for array in ['to_reanimate','auras']:
            replace('int '+array+'[1000];','int '+array+'[2][500];')
            matches=list(re.finditer(r'\b'+array+r'\[\((\w+)\) \* 500 \+ \((\w+)\)\]',text))
            assert len(matches)==2
            for m in matches:replace(m[0],array+'['+m[1]+']['+m[2]+']')
            matches=list(re.finditer(r'\('+array+r' \+ \((\w+)\) \* 500\)',text))
            forms=list(dict.fromkeys(m[0] for m in matches));assert len(forms)==4
            for old in forms:
                row=re.fullmatch(r'\('+array+r' \+ \((\w+)\) \* 500\)',old)[1]
                replace(old,array+'['+row+']',text.count(old))
            assert not re.search(r'\b'+array+r'\s*\+',text)
    else:
        replace('enum { CHOICE_PREVENT = 1, CHOICE_SAC, CHOICE_COUNTER, CHOICE_FIGHT };',
                'enum { CHOICE_PREVENT = 1, CHOICE_SAC, CHOICE_COUNTER, CHOICE_FIGHT } choices[2];')
        replace('  int choices[2];\n','')
    return changes,text


def symbol_map(image,debug):
    result=collections.defaultdict(set)
    for n,v,s,k,c in debug.symbols:
        at=v if s==-1 else image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[s-1].VirtualAddress+v if 0<s<=len(image.pe.sections) else None
        if at is not None:result[at].add(n)
    return result


def call_targets(md,ins):
    """Resolve direct calls and definite full-register immediate targets."""
    graph=graph_for(ins);by={i.address:i for i in ins}
    def family(r):
        f=FAMILIES.get(md.reg_name(r))
        return f[0] if f and f[1:]==(32,0) else None
    def step(i,initial):
        state=dict(initial);ops=i.operands
        for r in i.regs_access()[1]:
            f=FAMILIES.get(md.reg_name(r))
            if f:state[f[0]]=None
        if i.mnemonic=='call':
            for r in ['eax','ecx','edx']:state[r]=None
        elif i.mnemonic=='mov' and len(ops)==2 and ops[0].type==X86_OP_REG and family(ops[0].reg):
            state[family(ops[0].reg)]=ops[1].imm if ops[1].type==X86_OP_IMM else initial.get(family(ops[1].reg)) if ops[1].type==X86_OP_REG else None
        return state
    states={ins[0].address:{}};todo=collections.deque(states);updates=0
    while todo:
        at=todo.popleft();after=step(by[at],states[at])
        for dest in graph[at]:
            if dest not in states:states[dest]=dict(after);todo.append(dest);continue
            old=states[dest];merged={k:old.get(k) if old.get(k)==after.get(k) else None for k in set(old)|set(after)}
            if merged!=old:states[dest]=merged;todo.append(dest)
        updates+=1;assert updates<100000
    return {hex(i.address):i.operands[0].imm if i.operands[0].type==X86_OP_IMM else states[i.address].get(family(i.operands[0].reg)) if i.operands[0].type==X86_OP_REG else None
        for i in ins if i.mnemonic=='call' and i.address in states}


def instance_locations(dw,die,variable):
    """Read only single-DW_OP_fbreg locations of the typed instance local."""
    attr=variable.attributes.get('DW_AT_location')
    if attr is None:return []
    loc=LocationParser(dw.location_lists()).parse_from_attribute(attr,die.cu['version'],die=variable)
    base=value(die.cu.get_top_DIE(),'DW_AT_low_pc',0);entries=[]
    if isinstance(loc,LocationExpr):entries=[(lo,hi,loc.loc_expr) for lo,hi in ranges(dw,die)]
    else:
        for entry in loc:
            if isinstance(entry,BaseAddressEntry):base=entry.base_address;continue
            lo=entry.begin_offset if entry.is_absolute else base+entry.begin_offset
            hi=entry.end_offset if entry.is_absolute else base+entry.end_offset
            entries.append((lo,hi,entry.loc_expr))
    result=[]
    for lo,hi,expr in entries:
        ops=DWARFExprParser(dw.structs).parse_expr(expr)
        if len(ops)==1 and ops[0].op_name=='DW_OP_fbreg':result.append(dict(start=lo,end=hi,frame_offset=ops[0].args[0]))
    return result


def multani_flow(image,debug,snapshot,array_frame,geometry,instance_frames):
    """Definite instance/card tags and actual array-word bounce operands.

    Indexed target and destination expressions must use the same unchanged
    register and corresponding slot bias. This does not prove index bounds,
    loop counts, chosen values or arbitrary aliases. Calls kill all tracked
    memory and caller-save registers; unsupported definitions are unknown.
    A compiler spill reload can additionally use an explicit fixed-frame
    DWARF location of the typed instance variable at that instruction. This
    is declaration/location evidence, not a proof of arbitrary memory aliases.
    """
    md,ins=decode(snapshot['view'],image);graph=graph_for(ins);by_at={i.address:i for i in ins}
    symbols=symbol_map(image,debug)
    def family(reg):
        f=FAMILIES.get(md.reg_name(reg))
        return f[0] if f and f[1:]==(32,0) else None
    def memkey(op):
        if op.type!=X86_OP_MEM or op.mem.index:return None
        if op.mem.base==X86_REG_ESP:return ('arg',op.mem.disp,op.size)
        if op.mem.base==X86_REG_EBP:return ('frame',op.mem.disp,op.size)
    def operand(state,op,site):
        if op.type==X86_OP_REG:return state.get(('reg',family(op.reg)))
        if op.type==X86_OP_MEM:
            m=op.mem;k=memkey(op)
            if k and k in state and state[k] is not None:return state[k]
            if op.size==4 and m.base==X86_REG_EBP and not m.index and any(
                    p['start']<=site<p['end'] and p['frame_offset']==m.disp-8 for p in instance_frames):return ('instance',)
            if op.size==4 and m.base==X86_REG_EBP and not m.index:
                relative=m.disp-8-array_frame
                if relative in [0,4]:return ('array_word',relative)
            if op.size==4 and state.get(('reg',family(m.base)))==('instance',) and m.index and m.scale==geometry['target_bytes']:
                bias=m.disp-geometry['card_field_offset']
                if bias in [-geometry['target_bytes'],0]:return ('selected_card',hex(site),family(m.index),bias)
        return None
    def step(i,initial):
        state=dict(initial);ops=i.operands;stores=[];calls=[]
        for r in i.regs_access()[1]:
            f=FAMILIES.get(md.reg_name(r))
            if f:state['reg',f[0]]=None
        for op in ops:
            if op.type==X86_OP_MEM and op.access&2:
                k=memkey(op)
                for old in list(state):
                    if old[0]!='reg' and (k is None or old[0]==k[0] and max(old[1],k[1])<min(old[1]+old[2],k[1]+k[2])):state[old]=None
        if i.mnemonic=='call':
            names=symbols.get(ops[0].imm,set()) if ops[0].type==X86_OP_IMM else set()
            if '_bounce_permanent' in names:
                arg=initial.get(('arg',4,4));calls.append(dict(address=hex(i.address),card_argument=list(arg) if arg else None))
            state={k:v for k,v in state.items() if k[0]=='reg'}
            for r in ['eax','ecx','edx']:state['reg',r]=None
            if '_get_card_instance' in names:state['reg','eax']=('instance',)
        elif i.mnemonic=='mov' and len(ops)==2:
            val=operand(initial,ops[1],i.address)
            if ops[0].type==X86_OP_REG and family(ops[0].reg):state['reg',family(ops[0].reg)]=val
            elif ops[0].type==X86_OP_MEM:
                k=memkey(ops[0])
                if k:state[k]=val
                m=ops[0].mem
                if array_frame is not None and ops[0].size==4 and m.base==X86_REG_EBP and m.index and m.scale==4 and val and val[0]=='selected_card':
                    stores.append(dict(address=hex(i.address),selected_load=val[1],load_index=val[2],target_slot_bias=val[3],
                        store_index=family(m.index),array_bias=m.disp-8-array_frame,element_bytes=4))
        return state,stores,calls
    states={ins[0].address:{}};todo=collections.deque(states);updates=0
    while todo:
        at=todo.popleft();after,*_=step(by_at[at],states[at])
        for dest in graph[at]:
            if dest not in states:states[dest]=dict(after);todo.append(dest);continue
            old=states[dest];merged={k:old.get(k) if old.get(k)==after.get(k) else None for k in set(old)|set(after)}
            if merged!=old:states[dest]=merged;todo.append(dest)
        updates+=1;assert updates<100000
    stores=[];calls=[];positions={i.address:j for j,i in enumerate(ins)}
    for at,state in states.items():
        _,s,c=step(by_at[at],state);stores.extend(s);calls.extend(c)
    for s in stores:
        assert s['load_index']==s['store_index'] and s['array_bias'] in [-4,0]
        assert s['target_slot_bias']*4==s['array_bias']*8
        lo,hi=positions[int(s['selected_load'],16)],positions[int(s['address'],16)];assert lo<hi
        for i in ins[lo:hi]:
            assert not (i.mnemonic.startswith('j') or i.mnemonic in ['call','ret'])
            assert s['store_index'] not in {family(r) for r in i.regs_access()[1]}
    spill_reads=[dict(address=hex(i.address),frame_offset=o.mem.disp-8) for i in ins for o in i.operands
        if o.type==X86_OP_MEM and o.size==4 and o.access&1 and o.mem.base==X86_REG_EBP and not o.mem.index
        and any(p['start']<=i.address<p['end'] and p['frame_offset']==o.mem.disp-8 for p in instance_frames)]
    return dict(selected_card_stores=sorted(stores,key=lambda s:int(s['address'],16)),bounce_calls=sorted(calls,key=lambda c:int(c['address'],16)),
        dwarf_instance_spill_reads=spill_reads)


def inspect(image,debug,rows):
    result=snapshots(image,debug,rows,with_effects=False);dw=debug.dwarf();symbols=symbol_map(image,debug)
    for (source,name),snapshot in result.items():
        die=dw.get_DIE_from_refaddr(int(snapshot['die_offset'],16));decls=[];instance_type=None;instance_variable=None
        for v in walk(die):
            if v.tag!='DW_TAG_variable':continue
            t=unqualify(inherited_type(v))
            if inherited_value(v,'DW_AT_name')=='instance' and t.tag=='DW_TAG_pointer_type':instance_type=unqualify(inherited_type(t));instance_variable=v
            if t.tag!='DW_TAG_array_type':continue
            element=t
            while element.tag=='DW_TAG_array_type':
                for bound in element.iter_children():
                    if bound.tag=='DW_TAG_subrange_type':
                        assert not bound.attributes['DW_AT_upper_bound'].form.startswith('DW_FORM_ref')
                element=unqualify(inherited_type(element))
            item=dict(name=inherited_value(v,'DW_AT_name'),shape=shape(t),element_kind=element.tag)
            if element.tag=='DW_TAG_enumeration_type':
                item['values']={value(e,'DW_AT_name'):value(e,'DW_AT_const_value') for e in element.iter_children() if e.tag=='DW_TAG_enumerator'}
            decls.append(item)
        snapshot['array_declarations']=decls
        if name in ['put_card_on_stack','resolve_trigger']:
            eff=effects(image,debug,snapshot['view']);md,ins=decode(snapshot['view'],image);by={hex(i.address):i for i in ins};targets=call_targets(md,ins);calls=[]
            for c in eff['calls']:
                i=by[c['address']];at=targets[c['address']]
                callname='_sprintf' if '_sprintf' in symbols.get(at,set()) else '_raw_do_dialog' if at==0x471b60 else None
                if callname is None:continue
                arg=c['arguments'][0 if callname=='_sprintf' else 4]
                if not arg or arg[0]!='frame':continue
                owners=[v for v in snapshot['variables'] if v['shape']['kind']=='DW_TAG_array_type' and v['shape']['element_size']==1
                    and v['frame_offset']==arg[1]-8 and any(lo<=i.address<hi for lo,hi in v['scope_ranges'])]
                # The before build can have an outer str[500] and an inner
                # trace[300] sharing one frame address. Use actual lexical
                # ancestry, rather than attributing the inner call to both.
                def depth(v):
                    scope=dw.get_DIE_from_refaddr(int(v['die_offset'],16));n=0
                    while scope is not None:n+=1;scope=scope.get_parent()
                    return n
                if len(owners)>1:
                    deepest=max(depth(v) for v in owners);owners=[v for v in owners if depth(v)==deepest]
                assert len(owners)==1,(name,c,owners)
                item=dict(address=c['address'],name=callname,array_name=owners[0]['name'],array_bytes=owners[0]['shape']['size'],frame_offset=arg[1]-8)
                if callname=='_sprintf':
                    fmt=c['arguments'][1]
                    if fmt and fmt[0]=='const':
                        base=image.pe.OPTIONAL_HEADER.ImageBase
                        if base<=fmt[2]<base+image.pe.OPTIONAL_HEADER.SizeOfImage:item['literal_format']=image.pe.get_data(fmt[2]-base,128).split(b'\0',1)[0].decode('latin1')
                calls.append(item)
            snapshot['buffer_calls']=calls
        if name=='card_multani_yavimayas_avatar':
            assert instance_type is not None
            target=next(m for m in instance_type.iter_children() if m.tag=='DW_TAG_member' and value(m,'DW_AT_name')=='targets')
            element=unqualify(inherited_type(unqualify(inherited_type(target))))
            card=next(m for m in element.iter_children() if m.tag=='DW_TAG_member' and value(m,'DW_AT_name')=='card')
            geometry=dict(targets_offset=member_offset(target),target_bytes=sizeof(element),card_field_offset=member_offset(target)+member_offset(card))
            snapshot['target_geometry']=geometry
            snapshot['instance_frame_locations']=instance_locations(dw,die,instance_variable)
            array=[v for v in snapshot['variables'] if v['name']=='to_bounce'];assert len(array)<=1
            snapshot['selection_flow']=multani_flow(image,debug,snapshot,array[0]['frame_offset'],geometry,snapshot['instance_frame_locations']) if array else None
    return result
