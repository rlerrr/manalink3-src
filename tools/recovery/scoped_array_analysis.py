"""Inspect native scoped prompts, output arrays and counter matrices."""
import collections
from capstone.x86 import X86_OP_REG,X86_OP_MEM,X86_OP_IMM,X86_REG_EBP,X86_REG_ESP
from array_role_analysis import snapshots,effects
from array_census_analysis import symbol_map,call_targets
from canonicalize_helpers import read,span
from cast_stack_storage_analysis import lifetime,scoped_view
from global_initializers import inherited_type
from inspect_release import value,inherited_value
from object_initializers import unqualify,sizeof,member_offset
from shared_stack_test_analysis import walk,frame_offset,test_layout,FAMILIES
from verify_inline_helpers import ranges
from verify_wide_stack_test_recovery import decode,graph_for

NAMES={'card_tezzeret_the_seeker','do_multicascade','card_inniaz_the_gale_force',
       'card_tevesh_szat_doom_of_fools','card_nevinyrral_urborg_tyrant','card_go_to_jail'}
UNION='''  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_token;
'''


def source_changes(name,text):
    assert name in NAMES;changes=[]
    def replace(old,new,count=1):
        nonlocal text
        assert text.count(old)==count,(name,old,text.count(old),count)
        changes.append(dict(before=old,after=new,occurrences=count));text=text.replace(old,new)
    if name=='card_tezzeret_the_seeker':
        replace('  test_definition_t this_test_1;\n  test_definition_t this_test;\n','')
        replace('    if (event == EVENT_ACTIVATE)\n    {','    if (event == EVENT_ACTIVATE)\n    {\n      test_definition_t this_test;')
        replace('      else if (iVar1 == 2)\n      {\n        scnprintf',
                '      else if (iVar1 == 2)\n      {\n        char buffer[500];\n        test_definition_t this_test;\n        scnprintf')
        replace('this_test.message','buffer',2)
        replace('this_test_1','this_test',4)
    elif name=='do_multicascade':
        replace('  test_definition_t test;\n','')
        replace('      while (true)\n      {','      while (true)\n      {\n        test_definition_t test;')
        replace('      LAB_022faa64:\n','        char buffer[1000];\n      LAB_022faa64:\n')
        replace('test.message','buffer',2)
    elif name=='card_inniaz_the_gale_force':
        replace('  test_definition_t test;\n','')
        replace('  if (event == EVENT_DECLARE_ATTACKERS)\n  {','  if (event == EVENT_DECLARE_ATTACKERS)\n  {\n    test_definition_t test;')
        replace('    if (event == EVENT_DAT_ABILITY)\n    {','    if (event == EVENT_DAT_ABILITY)\n    {\n      char buffer[150];')
        replace('  if (event == EVENT_RESOLVE_ACTIVATION)\n  {','  if (event == EVENT_RESOLVE_ACTIVATION)\n  {\n    test_definition_t test;')
        replace('test.message','buffer',4)
    elif name=='card_tevesh_szat_doom_of_fools':
        replace(UNION,'')
        replace("    if (uVar1 == '\\x02')\n    {","    if (uVar1 == '\\x02')\n    {\n      test_definition_t test;")
        replace("    else if (uVar1 == '\\x03')\n    {","    else if (uVar1 == '\\x03')\n    {\n      int rv[3];")
        replace("    else if (uVar1 == '\\x01')\n    {","    else if (uVar1 == '\\x01')\n    {\n      token_generation_t token;")
        replace('recovered_stack_token.test','test',2)
        replace('&recovered_stack_token.token.id','rv')
        replace('recovered_stack_token.token.id','rv[0]')
        replace('recovered_stack_token.token.s_player','rv[1]',3)
        replace('recovered_stack_token.token.s_card','rv[2]',3)
        replace('recovered_stack_token.token','token',6)
    elif name=='card_nevinyrral_urborg_tyrant':
        replace(UNION,'')
        replace('  if (event == EVENT_ETB)\n  {','  if (event == EVENT_ETB)\n  {\n    token_generation_t token;')
        replace('    if (player == 1)\n    {','    if (player == 1)\n    {\n      int totals[2][2];')
        replace('    if (iVar1 != 0)\n    {','    if (iVar1 != 0)\n    {\n      test_definition_t this_test;')
        replace('(&recovered_stack_token.token.id)[iVar1 * 2]','totals[iVar1][0]',2)
        replace('(&recovered_stack_token.token.s_player)[iVar1 * 2]','totals[iVar1][1]',2)
        replace('*(int *)((int)&recovered_stack_token.token + iVar1 * 8)','totals[iVar1][0]',2)
        replace('*(int *)((int)&recovered_stack_token.token + iVar1 * 8 + 4)','totals[iVar1][1]',2)
        for field,row,col in [('id',0,0),('s_player',0,1),('s_card',1,0),('eff_player',1,1)]:
            replace('recovered_stack_token.token.'+field,'totals['+str(row)+']['+str(col)+']',2)
        replace('recovered_stack_token.test','this_test',2)
        replace('recovered_stack_token.token','token',7)
    else:
        replace('  target_definition_t td;\n','')
        replace('  if (event == EVENT_ETB)\n  {','  if (event == EVENT_ETB)\n  {\n    target_definition_t td;')
        replace('  else if (event == EVENT_UPKEEP_TRIGGER_ABILITY)\n  {','  else if (event == EVENT_UPKEEP_TRIGGER_ABILITY)\n  {\n    int r[2];')
        replace('&td.who_chooses, &td.allowed_controller','&r[0], &r[1]')
        replace('td.who_chooses == td.allowed_controller','r[0] == r[1]')
    return changes,text


def matrix_updates(image,debug,snapshot,frame):
    """Follow definite count+1 and count/CMC field read-modify-write operands.

    Selector values and bounds are abstract. A load/store pair must use the
    same unchanged index register. Calls kill all memory and caller-save
    registers; unsupported operations become unknown.
    """
    md,ins=decode(snapshot['view'],image);graph=graph_for(ins);by={i.address:i for i in ins};symbols=symbol_map(image,debug)
    def family(r):
        f=FAMILIES.get(md.reg_name(r));return f[0] if f and f[1:]==(32,0) else None
    def cell(op):
        if op.type!=X86_OP_MEM or op.size!=4:return None
        m=op.mem;offset=m.disp-8-frame
        if m.base==X86_REG_EBP and m.index and m.scale==8 and offset in [0,4]:return (offset,family(m.index))
    def operand(state,op,site):
        if op.type==X86_OP_REG:return state.get(family(op.reg))
        if op.type==X86_OP_IMM:return ('const',op.imm)
        c=cell(op)
        if c:return ('cell',c[0],c[1],hex(site))
    def plus(a,b):
        if a and a[0]=='cell' and b:
            if b==('const',1):return ('count_plus_one',a)
            if b[0]=='cmc':return ('cmc_sum',a,b[1])
        if b and b[0]=='cell' and a and a[0]=='cmc':return ('cmc_sum',b,a[1])
    def step(i,initial):
        state=dict(initial);ops=i.operands;writes=[]
        for r in i.regs_access()[1]:
            f=FAMILIES.get(md.reg_name(r))
            if f:state[f[0]]=None
        if i.mnemonic=='call':
            for r in ['eax','ecx','edx']:state[r]=None
            if ops[0].type==X86_OP_IMM and '_get_cmc' in symbols.get(ops[0].imm,set()):state['eax']=('cmc',hex(i.address))
        elif ops:
            val=None
            if i.mnemonic=='mov' and len(ops)==2:val=operand(initial,ops[1],i.address)
            elif i.mnemonic=='inc':val=plus(operand(initial,ops[0],i.address),('const',1))
            elif i.mnemonic=='add' and len(ops)==2:val=plus(operand(initial,ops[0],i.address),operand(initial,ops[1],i.address))
            if ops[0].type==X86_OP_REG and family(ops[0].reg) and ops[0].access&2:state[family(ops[0].reg)]=val
            dest=cell(ops[0])
            if dest and ops[0].access&2 and val and val[0] in ['count_plus_one','cmc_sum']:
                load=val[1];assert load[1:3]==dest
                writes.append(dict(address=hex(i.address),column=dest[0],index_register=dest[1],operation=val[0],load=load[3],
                    cmc_call=val[2] if val[0]=='cmc_sum' else None))
        return state,writes
    states={ins[0].address:{}};todo=collections.deque(states);updates=0
    while todo:
        at=todo.popleft();after,_=step(by[at],states[at])
        for dest in graph[at]:
            if dest not in states:states[dest]=dict(after);todo.append(dest);continue
            old=states[dest];merged={k:old.get(k) if old.get(k)==after.get(k) else None for k in set(old)|set(after)}
            if merged!=old:states[dest]=merged;todo.append(dest)
        updates+=1;assert updates<100000
    writes=[];positions={i.address:j for j,i in enumerate(ins)}
    for at,state in states.items():writes.extend(step(by[at],state)[1])
    for w in writes:
        lo,hi=positions[int(w['load'],16)],positions[int(w['address'],16)];assert lo<=hi
        for i in ins[lo:hi]:
            assert not (i.mnemonic.startswith('j') or i.mnemonic in ['call','ret'])
            assert w['index_register'] not in {family(r) for r in i.regs_access()[1]}
    return sorted(writes,key=lambda w:int(w['address'],16))


def output_words(image,debug,snapshot,frame,size):
    """Follow fixed array-word reads into comparisons and gain_control args."""
    md,ins=decode(snapshot['view'],image);graph=graph_for(ins);by={i.address:i for i in ins};symbols=symbol_map(image,debug)
    def family(r):
        f=FAMILIES.get(md.reg_name(r));return f[0] if f and f[1:]==(32,0) else None
    def key(op):
        if op.type==X86_OP_MEM and op.mem.base==X86_REG_ESP and not op.mem.index:return ('arg',op.mem.disp,op.size)
    def operand(state,op):
        if op.type==X86_OP_REG:return state.get(('reg',family(op.reg)))
        if op.type==X86_OP_MEM:
            m=op.mem;relative=m.disp-8-frame
            if op.size==4 and m.base==X86_REG_EBP and not m.index and 0<=relative<size and relative%4==0:return ('array_word',relative)
            k=key(op)
            if k:return state.get(k)
    def step(i,initial):
        state=dict(initial);ops=i.operands;calls=[];comparisons=[]
        for r in i.regs_access()[1]:
            f=FAMILIES.get(md.reg_name(r))
            if f:state['reg',f[0]]=None
        for op in ops:
            if op.type==X86_OP_MEM and op.access&2:
                k=key(op)
                for old in list(state):
                    if old[0]=='arg' and (k is None or max(old[1],k[1])<min(old[1]+old[2],k[1]+k[2])):state[old]=None
        if i.mnemonic=='call':
            names=symbols.get(ops[0].imm,set()) if ops[0].type==X86_OP_IMM else set()
            if '_gain_control' in names:
                a=initial.get(('arg',12,4));calls.append(dict(address=hex(i.address),card_argument=list(a) if a else None))
            state={k:v for k,v in state.items() if k[0]=='reg'}
            for r in ['eax','ecx','edx']:state['reg',r]=None
        elif i.mnemonic=='mov' and len(ops)==2:
            val=operand(initial,ops[1])
            if ops[0].type==X86_OP_REG and family(ops[0].reg):state['reg',family(ops[0].reg)]=val
            elif key(ops[0]):state[key(ops[0])]=val
        elif i.mnemonic=='cmp' and len(ops)==2:
            a,b=[operand(initial,o) for o in ops]
            if a and b and a[0]==b[0]=='array_word':comparisons.append(dict(address=hex(i.address),offsets=sorted([a[1],b[1]])))
        return state,calls,comparisons
    states={ins[0].address:{}};todo=collections.deque(states);updates=0
    while todo:
        at=todo.popleft();after,*_=step(by[at],states[at])
        for dest in graph[at]:
            if dest not in states:states[dest]=dict(after);todo.append(dest);continue
            old=states[dest];merged={k:old.get(k) if old.get(k)==after.get(k) else None for k in set(old)|set(after)}
            if merged!=old:states[dest]=merged;todo.append(dest)
        updates+=1;assert updates<100000
    calls=[];comparisons=[]
    for at,state in states.items():
        _,c,p=step(by[at],state);calls.extend(c);comparisons.extend(p)
    return dict(gain_control_calls=sorted(calls,key=lambda c:int(c['address'],16)),comparisons=sorted(comparisons,key=lambda c:int(c['address'],16)))


def inspect(image,debug,rows):
    result=snapshots(image,debug,rows,with_effects=True);dw=debug.dwarf();symbols=symbol_map(image,debug)
    relevant={'_scnprintf','_new_default_test_definition','_default_test_definition','_new_global_tutor','_select_card_from_zone',
        '_new_pick_target','_do_dialog','_default_token_definition','_generate_token','_new_sacrifice',
        '_put_all_commanders_from_command_zone_to','_gain_control','_roll_2d6t','_new_manipulate_all'}
    for key,snapshot in result.items():
        die=dw.get_DIE_from_refaddr(int(snapshot['die_offset'],16));md,ins=decode(snapshot['view'],image);targets=call_targets(md,ins)
        calls=[]
        for c in snapshot['effects']['calls']:
            names=symbols.get(targets[c['address']],set())&relevant
            if not names:continue
            assert len(names)==1;calls.append(dict(c,name=next(iter(names))))
        snapshot['storage_calls']=calls;records=[]
        for v in walk(die):
            if v.tag!='DW_TAG_variable' or value(v,'DW_AT_location') is None:continue
            t=unqualify(inherited_type(v))
            if t is None or t.tag!='DW_TAG_structure_type' or sizeof(t) not in [76,216]:continue
            offset=frame_offset(dw,v);scope=lifetime(dw,v,die);parts=ranges(dw,scope)
            item=dict(name=inherited_value(v,'DW_AT_name'),bytes=sizeof(t),frame_offset=offset,scope_ranges=[[a,b] for a,b in parts])
            if sizeof(t)==216:
                fields=test_layout(t);scoped=scoped_view(image,dw,die,scope,offset,fields)
                item.update(layout=fields,parameter_events=scoped['byte_events'])
                initializers=[c['name'] for c in calls if c['name'] in ['_new_default_test_definition','_default_test_definition'] and
                    c['arguments'][0]==['frame',offset+8] and any(a<=int(c['address'],16)<b for a,b in parts)]
                assert initializers,(key,item['name'],initializers)
                item['initializers']=initializers
                item['role']=initializers[0] if len(initializers)==1 else 'multiple_initializers'
            else:
                item['role']='token'
                item['constant_writes']=[dict(offset=w['frame_offset']-offset,size=w['size'],bytes=w['bytes']) for w in snapshot['effects']['writes']
                    if offset<=w['frame_offset'] and w['frame_offset']+w['size']<=offset+76 and any(a<=int(w['address'],16)<b for a,b in parts)]
            records.append(item)
        snapshot['typed_records']=records
        if key[1]=='card_nevinyrral_urborg_tyrant':
            arrays=[v for v in snapshot['variables'] if v['name']=='totals']
            snapshot['matrix_updates']=matrix_updates(image,debug,snapshot,arrays[0]['frame_offset']) if arrays else None
        if key[1] in ['card_tevesh_szat_doom_of_fools','card_go_to_jail']:
            role='rv' if key[1]=='card_tevesh_szat_doom_of_fools' else 'r'
            arrays=[v for v in snapshot['variables'] if v['name']==role]
            snapshot['output_word_flow']=output_words(image,debug,snapshot,arrays[0]['frame_offset'],arrays[0]['shape']['size']) if arrays else None
    return result


def plain_test_record(dw,row):
    """Use a real scoped test local when replaying the old shared-union audit."""
    name=row['name'];assert name in ['card_tevesh_szat_doom_of_fools','card_nevinyrral_urborg_tyrant']
    local='test' if name=='card_tevesh_szat_doom_of_fools' else 'this_test';found=[]
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')!=row['source']:continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag!='DW_TAG_subprogram' or inherited_value(die,'DW_AT_name')!=name or not ranges(dw,die):continue
            variables=[v for v in walk(die) if v.tag=='DW_TAG_variable' and inherited_value(v,'DW_AT_name')==local]
            assert len(variables)==1
            v=variables[0];t=unqualify(inherited_type(v));assert t.tag=='DW_TAG_structure_type' and sizeof(t)==216
            found.append(dict(die=die,frame_offset=frame_offset(dw,v),layout=test_layout(t)))
    assert len(found)==1
    return found[0]
