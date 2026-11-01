"""Check additional shared test unions, callbacks and recovered value flows."""
import collections,json
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86 import X86_OP_REG,X86_OP_MEM,X86_OP_IMM,X86_REG_EBP,X86_REG_ESP
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,value,inherited_value
from global_initializers import inherited_type
from object_initializers import sizeof,unqualify
from shared_stack_test_analysis import collect,compiled_functions,frame_offset,test_layout,walk,FAMILIES


def decode(view,image):
    lo,hi=int(view['address'],16),int(view['end'],16)
    md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True
    ins=list(md.disasm(image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo),lo))
    assert sum(i.size for i in ins)==hi-lo
    return md,ins


def graph_for(ins):
    by_at={i.address:i for i in ins};graph={}
    for at,i in by_at.items():
        assert not i.mnemonic.startswith('loop')
        edges=[];fallthrough=at+i.size
        if i.mnemonic.startswith('ret'):pass
        elif i.mnemonic.startswith('j'):
            assert i.operands[0].type==X86_OP_IMM,'Indirect branch requires separate analysis'
            target=i.operands[0].imm
            if target in by_at:edges.append(target)
            if i.mnemonic!='jmp' and fallthrough in by_at:edges.append(fallthrough)
        elif fallthrough in by_at:edges.append(fallthrough)
        graph[at]=edges
    return graph


def symbol_at(image,debug,address):
    names={n for n,v,s,k,c in debug.symbols if s>0 and k==32 and
        image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[s-1].VirtualAddress+v==address}
    assert len(names)==1,(hex(address),names)
    return next(iter(names))


def normalized_events(view,image,debug):
    """Relocate only complete nonzero callback stores, using exact COFF identity."""
    replacements={}
    for s in view['stores']:
        if s['offset']==196 and s['size']==4 and s['value'] not in [None,0]:
            name=symbol_at(image,debug,s['value'])
            assert name in {'_has_mana_cost_0_or_1','_is_basic_land_by_internal_id',
                            '_test_has_watermark_val','_not_marked'},name
            for b in range(4):
                key=('special_selection_function',b,(s['value']>>(8*b))&255)
                tag='callback:'+name+':byte'+str(b)
                assert key not in replacements or replacements[key]==tag
                replacements[key]=tag
    result=collections.Counter()
    for row in view['byte_events']:
        key=(row['field'],row['byte'],row['value'])
        result[(key[0],key[1],replacements.get(key,key[2]))]+=row['count']
    return result


def compare(name,native,current,image,native_image,debug,native_debug):
    left=normalized_events(native,native_image,native_debug)
    right=normalized_events(current,image,debug)
    if left==right:
        return dict(kind='relocated_callback_identity' if native['byte_events']!=current['byte_events']
                    else 'exact_scalar_byte_events')
    assert name=='card_sai_master_thopterist',name
    assert len(native['stores'])==1 and len(current['stores'])==2
    assert all((s['offset'],s['size'],s['value'])==(174,2,2) for s in native['stores']+current['stores'])
    assert collections.Counter({k:v*2 for k,v in left.items()})==right
    _,ins=decode(current,image);graph=graph_for(ins)
    sites=[int(s['address'],16) for s in current['stores']]
    def reachable(start,target):
        seen=set();todo=list(graph[start])
        while todo:
            at=todo.pop()
            if at==target:return True
            if at in seen:continue
            seen.add(at);todo.extend(graph[at])
        return False
    assert not reachable(sites[0],sites[1]) and not reachable(sites[1],sites[0])
    return dict(kind='mutually_exclusive_sai_quantity_clone',sites=[hex(a) for a in sites])


def value_flow(name,view,image,debug,die,dw):
    """Conservative CFG reaching-value analysis for the four lost value flows.

    Unknown definitions remain unknown through joins. Calls clobber caller-save
    registers and outgoing argument slots; only named helper return identities
    and direct frame addresses acquire provenance. Unsupported operations kill
    their register values. No loop/indirect-branch shortcut is accepted.
    """
    assert name in {'card_turn_burn','card_reign_of_terror','card_dual_nature','card_slaughter_the_strong'}
    md,ins=decode(view,image);graph=graph_for(ins);by_at={i.address:i for i in ins}
    unknown=frozenset({'unknown'});states={ins[0].address:{}};todo=collections.deque([ins[0].address])
    def regkey(reg):
        family=FAMILIES.get(md.reg_name(reg))
        return ('reg',family[0]) if family else None
    def memkey(op):
        if op.type!=X86_OP_MEM or op.mem.index:return None
        if op.mem.base in [X86_REG_EBP,X86_REG_ESP]:
            return ('ebp' if op.mem.base==X86_REG_EBP else 'esp',op.mem.disp,op.size)
    def operand(state,op):
        if op.type==X86_OP_IMM:return frozenset({'const:'+str(op.imm&0xffffffff)})
        if op.type==X86_OP_REG:return state.get(regkey(op.reg),unknown)
        if op.type==X86_OP_MEM:return state.get(memkey(op),unknown)
        return unknown
    symbols={}
    for i in ins:
        if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM:
            try:symbols[i.address]=symbol_at(image,debug,i.operands[0].imm)
            except AssertionError:pass # EXE/import calls have unknown returns.
    updates=0
    while todo:
        at=todo.popleft();i=by_at[at];state=dict(states[at]);ops=i.operands
        if i.mnemonic=='call':
            args=state.get(('esp',8,4),unknown)
            state={k:v for k,v in state.items() if k[0]!='esp'}
            for n in ['eax','ecx','edx']:state[('reg',n)]=unknown
            callee=symbols.get(at)
            if callee=='_get_card_name':state[('reg','eax')]=frozenset({'return:_get_card_name'})
            elif callee=='_get_sleighted_color_test':
                state[('reg','eax')]=frozenset('color_return:'+a.removeprefix('const:')
                    if a.startswith('const:') else 'unknown' for a in args)
        elif i.mnemonic=='mov' and len(ops)==2:
            key=regkey(ops[0].reg) if ops[0].type==X86_OP_REG else memkey(ops[0])
            if key:
                state[key]=operand(state,ops[1]) if ops[0].size==4 else unknown
                if key[0] in ['ebp','esp']:
                    for other in list(state):
                        if other!=key and other[0]==key[0] and other[1]<key[1]+key[2] and key[1]<other[1]+other[2]:
                            state[other]=unknown
        elif i.mnemonic=='lea' and len(ops)==2 and ops[0].type==X86_OP_REG:
            src=ops[1]
            state[regkey(ops[0].reg)]=frozenset({'frame_address:'+str(src.mem.disp-8)}) if (
                src.type==X86_OP_MEM and src.mem.base==X86_REG_EBP and not src.mem.index) else unknown
        elif i.mnemonic=='xor' and len(ops)==2 and ops[0].type==X86_OP_REG and ops[1].type==X86_OP_REG and ops[0].reg==ops[1].reg:
            state[regkey(ops[0].reg)]=frozenset({'const:0'})
        else:
            for reg in i.regs_access()[1]:
                key=regkey(reg)
                if key:state[key]=unknown
            # An unsupported memory-writing instruction invalidates overlapping
            # direct frame/argument cells as well as its destination register.
            if ops and ops[0].type==X86_OP_MEM and ops[0].access&2:
                key=memkey(ops[0])
                if key:
                    for other in list(state):
                        if other[0]==key[0] and other[1]<key[1]+key[2] and key[1]<other[1]+other[2]:state[other]=unknown
        for target in graph[at]:
            old=states.get(target)
            merged=state if old is None else {k:old.get(k,unknown)|state.get(k,unknown) for k in old.keys()|state.keys()}
            if old!=merged:
                states[target]=dict(merged);todo.append(target);updates+=1
                assert updates<100000,'Value-flow analysis failed to converge'
    field=112 if name in ['card_turn_burn','card_reign_of_terror'] else 116 if name=='card_dual_nature' else 200
    stores=[s for s in view['stores'] if s['offset']==field and s['value'] is None]
    assert len(stores)==1 and stores[0]['size']==4
    site=int(stores[0]['address'],16);i=by_at[site]
    provenance=operand(states[site],i.operands[1])
    if field==112:
        expected={'color_return:16'} if name=='card_turn_burn' else {'color_return:8','color_return:32'}
    elif field==116:expected={'return:_get_card_name'}
    else:
        locals=[v for v in walk(die) if v.tag=='DW_TAG_variable' and inherited_value(v,'DW_AT_name')=='marked']
        assert len(locals)==1
        array=unqualify(inherited_type(locals[0]));assert sizeof(array)==302
        dims=[value(c,'DW_AT_upper_bound') for c in array.iter_children() if c.tag=='DW_TAG_subrange_type']
        assert dims==[1,150],dims
        expected={'frame_address:'+str(frame_offset(dw,locals[0]))}
    assert provenance==frozenset(expected),(name,hex(site),provenance,expected)
    return dict(field_offset=field,width=4,store=hex(site),provenance=sorted(provenance),
                method='CFG reaching values with unknown-preserving joins and call clobbers')


def verify_source(row,root,proof=None):
    text=read(root/row['source']);a,b=span(text,row['name'])
    assert sha(row['after_definition'].encode())==row['after_sha256']
    if text[a:b]!=row['after_definition']:
        from verify_card_array_locals_recovery import has_source_repair
        assert proof is not None and has_source_repair(proof,root,row['source'],row['name'],row['after_sha256'])
    assert sha(row['before_definition'].encode())==row['before_sha256']


def verify_superseded_definition(proof,image,root,source,name,old_hash):
    assert (source,name)==('cards/m15.c','card_goblin_rabblemaster')
    record=json.loads((root/proof['native_wide_stack_test_recovery']).read_bytes())
    assert sha(image.data)==record['native_dll_sha256']
    row=next(r for r in record['repairs'] if (r['source'],r['name'])==(source,name))
    assert row['before_sha256']==old_hash;verify_source(row,root)
    native=next(r for r in record['rows'] if r['name']==name)['native']
    lo,hi=int(native['address'],16),int(native['end'],16)
    assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==native['bytes_sha256']
    assert {(s['offset'],s['size'],s['value']) for s in native['stores']}=={(104,4,75),(170,2,1),(160,4,4)}


def verify(proof,image,root=ROOT):
    path=proof.get('native_wide_stack_test_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    original=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)==record['native_dbg_sha256'];dw=original.dwarf()
    assert len(record['rows'])==68 and len(record['repairs'])==12 and not record['unresolved']
    repairs={r['name']:r for r in record['repairs']}
    for row in repairs.values():verify_source(row,root,proof)
    edits=[e for e in proof['source_edits'] if e.get('native_wide_stack_test_extension')==path]
    assert len(edits)==12
    assert {(e['source'],e['before'],e['after']) for e in edits}=={
        (r['source'],r['before_definition'],r['after_definition']) for r in repairs.values()}
    builds=[]
    for key in ['runtime_before','runtime_after']:
        artifact=record[key];binary=(root/artifact['path']).read_bytes();debug=(root/artifact['debug_path']).read_bytes()
        assert sha(binary)==artifact['dll_sha256'] and sha(debug)==artifact['dbg_sha256']
        im,dbg=Image(binary),Image(debug);cd,functions=compiled_functions(dbg,record['rows'])
        builds.append((im,dbg,cd,functions))
    current=Image((root/'ManalinkEh.dll').read_bytes());current_debug=Image((root/'ManalinkEh.dbg').read_bytes())
    plain=[]
    if proof.get('native_card_array_locals_recovery'):
        from card_array_locals_analysis import TEST_NAMES,plain_record
        from verify_card_array_locals_recovery import has_source_repair
        plain=[r for r in record['rows'] if r['name'] in TEST_NAMES]
        assert len(plain)==5
    if proof.get('native_token_scope_recovery'):
        plain.extend(r for r in record['rows'] if r['name']=='card_the_circle_of_loyalty')
    current_dw,current_functions=compiled_functions(current_debug,[r for r in record['rows'] if r not in plain])
    for row in plain:
        old=repairs[row['name']]['after_sha256'] if row['name'] in repairs else row['retained_definition_sha256']
        if row['name']=='card_the_circle_of_loyalty':
            from token_scope_analysis import plain_test_record as scoped_test
            from verify_token_scope_recovery import has_source_repair as scope_repair
            assert scope_repair(proof,root,row['source'],row['name'],old)
            current_functions[row['name']]=scoped_test(current_dw,row)
        else:
            assert has_source_repair(proof,root,row['source'],row['name'],old)
            current_functions[row['name']]=plain_record(current_dw,row)
    kinds=collections.Counter();dynamic=0;restored=0
    for row in record['rows']:
        d=dw.get_DIE_from_refaddr(int(row['native_die'],16))
        variable=dw.get_DIE_from_refaddr(int(row['native_variable_die'],16))
        assert inherited_value(d,'DW_AT_name')==row['name'] and value(d,'DW_AT_frame_base')==[156]
        assert frame_offset(dw,variable)==row['storage']['frame_offset']
        assert test_layout(inherited_type(variable))==row['layout']
        native=collect(image,dw,d,row['storage']['frame_offset'],row['layout']);assert native==row['native']
        for j,(im,dbg,cd,functions) in enumerate(builds):
            f=functions[row['name']];assert f['layout']==row['layout']
            local=[v for v in walk(f['die']) if v.tag=='DW_TAG_variable' and
                   inherited_value(v,'DW_AT_name')==row['storage']['storage']]
            assert len(local)==1 and sizeof(inherited_type(local[0]))==row['source_union_size']
            v=collect(im,cd,f['die'],f['frame_offset'],f['layout'])
            assert v==row['before_compiled' if j==0 else 'after_compiled']
        f=current_functions[row['name']];assert f['layout']==row['layout']
        fresh=collect(current,current_dw,f['die'],f['frame_offset'],f['layout'])
        compare(row['name'],native,fresh,current,image,current_debug,original)
        result=compare(row['name'],native,row['after_compiled'],builds[1][0],image,builds[1][1],original)
        assert result==row['after_comparison'];kinds[result['kind']]+=1
        if row['name'] in repairs:
            assert not row['before_compiled']['stores'];restored+=len(native['stores'])
        else:
            t=read(root/row['source']);a,b=span(t,row['name'])
            if sha(t[a:b].encode())!=row['retained_definition_sha256']:
                from verify_card_array_locals_recovery import has_source_repair
                from verify_token_scope_recovery import has_source_repair as scope_repair
                if scope_repair(proof,root,row['source'],row['name'],row['retained_definition_sha256']):
                    from verify_token_scope_recovery import verify_superseded_definition
                elif has_source_repair(proof,root,row['source'],row['name'],row['retained_definition_sha256']):
                    from verify_card_array_locals_recovery import verify_superseded_definition
                else:
                    from verify_target_role_recovery import verify_superseded_definition
                verify_superseded_definition(proof,image,root,row['source'],row['name'],row['retained_definition_sha256'])
            compare(row['name'],native,row['before_compiled'],builds[0][0],image,builds[0][1],original)
        if row['name'] in {'card_turn_burn','card_reign_of_terror','card_dual_nature','card_slaughter_the_strong'}:
            assert value_flow(row['name'],native,image,original,d,dw)==row['native_value_flow']
            im,dbg,cd,functions=builds[1];fn=functions[row['name']]
            assert value_flow(row['name'],row['after_compiled'],im,dbg,fn['die'],cd)==row['after_value_flow']
            value_flow(row['name'],fresh,current,current_debug,f['die'],current_dw);dynamic+=1
    assert restored==27 and dynamic==4
    left,right=record['before_link_inputs'],record['after_link_inputs'];assert set(left)==set(right)
    changed=sorted(p for p in left if left[p]!=right[p])
    assert changed==sorted('.recovery/build-gcc93/obj/'+s[:-2]+'.o' for s in {r['source'] for r in repairs.values()})
    assert len(changed)==12
    return dict(reviewed_handlers=68,corrected_handlers=12,retained_handlers=56,restored_scalar_stores=27,
                recovered_value_flows=dynamic,comparison_kinds=dict(kinds),changed_linker_objects=12,
                unchanged_linker_objects=len(left)-12,scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
