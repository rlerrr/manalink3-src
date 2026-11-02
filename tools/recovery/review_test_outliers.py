"""Read-only scope-aware replay of the25 old single-frame test outliers."""
import argparse,collections,csv,json,sys
from pathlib import Path
sys.path.insert(0,'tools/recovery')
from capstone.x86 import X86_OP_MEM,X86_OP_REG,X86_OP_IMM,X86_REG_EBP
from canonicalize_helpers import sha,read,span
from global_initializers import inherited_type
from inspect_release import Image,inherited_value,value
from object_initializers import unqualify,sizeof,member_offset
from shared_stack_test_analysis import walk,frame_offset,test_layout,FAMILIES as BASE_FAMILIES
# This review owns its lane model; historical tool bytes/receipts stay unchanged.
FAMILIES=dict(BASE_FAMILIES)
for base,short in [("esi","si"),("edi","di"),("ebp","bp"),("esp","sp")]:FAMILIES[short]=(base,16,0)
from verify_inline_helpers import ranges
from array_role_analysis import effects
import array_role_analysis as frame_effects
from verify_wide_stack_test_recovery import decode,graph_for

parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--output',default='docs/recovery-evidence/test-outlier-scope-review.json');args=parser.parse_args()
seeds_path=Path('docs/recovery-evidence/test-outlier-review-seeds.json');seeds=json.loads(seeds_path.read_bytes())
ni=Image(Path('.recovery/2022-ManalinkEh.dll').read_bytes());nd=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
ci=Image(Path('ManalinkEh.dll').read_bytes());cd=Image(Path('ManalinkEh.dbg').read_bytes())
mapping_path=Path('config/reccmp/functions.csv');mapping=list(csv.DictReader(mapping_path.open(encoding='utf-8-sig')))
native_names={int(r['address'],16):r['name'] for r in mapping if r['type']=='function'}


def collect(image,dw,die,frame,fields):
    """Basic-block constants with partial lanes and all explicit/implicit kills."""
    parts=ranges(dw,die);assert len(parts)==1;lo,hi=parts[0]
    raw=image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo)
    view=dict(address=hex(lo),end=hex(hi),bytes_sha256=sha(raw))
    md,ins=decode(view,image)
    assert any(i.mnemonic=='mov' and i.op_str=='ebp, esp' for i in ins[:8])
    leaders={lo};known={};stores=[]
    for i in ins:
        if i.mnemonic.startswith('j'):
            if i.operands[0].type==X86_OP_IMM:leaders.add(i.operands[0].imm)
            leaders.add(i.address+i.size)
    def get(reg):
        f=FAMILIES.get(md.reg_name(reg))
        if not f:return None
        name,bits,shift=f;mask=((1<<bits)-1)<<shift;old=known.get(name,(0,0))
        return old[1]>>shift&((1<<bits)-1) if old[0]&mask==mask else None
    def put(reg,val):
        f=FAMILIES.get(md.reg_name(reg))
        if not f:return
        name,bits,shift=f;mask=((1<<bits)-1)<<shift;old=known.get(name,(0,0))
        known[name]=((old[0]&~mask)|(mask if val is not None else 0),(old[1]&~mask)|((val or 0)<<shift&mask))
    def operand(op):
        return op.imm if op.type==X86_OP_IMM else get(op.reg) if op.type==X86_OP_REG else None
    for i in ins:
        if i.address in leaders:known.clear()
        ops=i.operands
        if i.mnemonic=='call':
            for n in ['eax','ecx','edx']:known.pop(n,None)
            continue
        if len(ops)==2 and ops[0].type==X86_OP_MEM and i.mnemonic in ['mov','and','or','add','sub']:
            m=ops[0].mem;off=m.disp-8-frame
            if m.base==X86_REG_EBP and not m.index and 100<=off<216:
                val=operand(ops[1]) if i.mnemonic=='mov' else None
                stores.append(dict(address=hex(i.address),offset=off,size=ops[0].size,
                    value=None if val is None else val&((1<<(8*ops[0].size))-1),instruction=[i.mnemonic,i.op_str]))
        answer=None;recognized=False
        if len(ops)==2 and ops[0].type==X86_OP_REG:
            if i.mnemonic=='mov':answer=operand(ops[1]);recognized=True
            elif i.mnemonic=='xor' and ops[1].type==X86_OP_REG and ops[0].reg==ops[1].reg:answer=0;recognized=True
        for reg in i.regs_access()[1]:put(reg,None)
        if recognized:put(ops[0].reg,answer)
    return dict(view,stores=stores)


def symbols(image,debug):
    names=collections.defaultdict(set)
    for n,v,s,k,c in debug.symbols:
        if k==32 and 0<s<=len(image.pe.sections):names[image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[s-1].VirtualAddress+v].add(n.lstrip('_'))
    return names


def is_test(t):
    t=unqualify(t)
    if t is None or t.tag!='DW_TAG_structure_type' or sizeof(t)!=216:return False
    fields={value(m,'DW_AT_name'):member_offset(m) for m in t.iter_children() if m.tag=='DW_TAG_member'}
    return fields.get('type_flag')==108 and fields.get('special_selection_function')==196 and fields.get('message')==0


def snapshot(image,debug,dw,d,v,member,native,cache):
    frame=frame_offset(dw,v);t=unqualify(inherited_type(v))
    if member is not None:
        assert member_offset(member)==0;t=unqualify(inherited_type(member))
    assert is_test(t);fields=test_layout(t)
    scope=v.get_parent();lineage=[]
    while not ranges(dw,scope):scope=scope.get_parent()
    p=v.get_parent()
    while p!=d:
        if p.tag=='DW_TAG_inlined_subroutine':lineage.append(inherited_value(p,'DW_AT_name'))
        p=p.get_parent();assert p is not None
    key=d.offset,frame
    if key not in cache:cache[key]=collect(image,dw,d,frame,fields)
    data=cache[key];scope_ranges=ranges(dw,scope)
    all_stores=data['stores']
    def make_events(stores):
        events=collections.Counter();sm=(ns if native else cs)
        for s in stores:
            tag=normalized_value(s,native) if s['offset']==196 and s['size']==4 else None
            if not isinstance(tag,str):tag=None
            for f in fields:
                for offset in range(max(s['offset'],f['offset']),min(s['offset']+s['size'],f['offset']+f['size'])):
                    val=None if s['value'] is None else s['value']>>(8*(offset-s['offset']))&255
                    if tag:val=tag+':byte'+str(offset-s['offset'])
                    events[f['name'],offset-f['offset'],val]+=1
        return serial(events)
    stores=[s for s in all_stores if any(a<=int(s['address'],16)<b for a,b in scope_ranges)]
    events=collections.Counter();sm=(ns if native else cs)
    for s in stores:
        tag=None
        if s['offset']==196 and s['size']==4 and s['value'] not in [None,0]:
            addr=s['value'];names={native_names[addr]} if native and addr in native_names else sm.get(addr,set())
            if len(names)==1:tag='callback:'+next(iter(names))
        for f in fields:
            for offset in range(max(s['offset'],f['offset']),min(s['offset']+s['size'],f['offset']+f['size'])):
                val=None if s['value'] is None else s['value']>>(8*(offset-s['offset']))&255
                if tag:val=tag+':byte'+str(offset-s['offset'])
                events[f['name'],offset-f['offset'],val]+=1
    return dict(owner_die=hex(d.offset),variable_die=hex(v.offset),name=inherited_value(v,'DW_AT_name'),
        kind='typed' if member is None else 'union',inline_lineage=lineage,frame_offset=frame,
        owner_ranges=[[a,b] for a,b in ranges(dw,d)],scope_ranges=[[a,b] for a,b in scope_ranges],
        view={k:data[k] for k in ['address','end','bytes_sha256']},stores=stores,events=serial(events),all_stores=all_stores,all_events=make_events(all_stores))


def normalized_value(store,native):
    if store['offset']!=196 or store['size']!=4 or store['value'] in [None,0]:return store['value']
    addr=store['value'];names={native_names[addr]} if native and addr in native_names else (ns if native else cs).get(addr,set())
    return 'callback:'+next(iter(names)) if len(names)==1 else store['value']


def review_graph(ins,image):
    """Add only readonly switch tables with an immediate CMP/JA range guard."""
    by={i.address:i for i in ins};graph={};tables=[]
    for j,i in enumerate(ins):
        edges=[];fall=i.address+i.size
        if i.mnemonic.startswith('loop'):raise AssertionError('LOOP needs separate analysis')
        if i.mnemonic.startswith('ret'):pass
        elif i.mnemonic.startswith('j'):
            op=i.operands[0]
            if op.type==X86_OP_IMM:
                if op.imm in by:edges.append(op.imm)
                if i.mnemonic!='jmp' and fall in by:edges.append(fall)
            else:
                assert i.mnemonic=='jmp' and op.type==X86_OP_MEM and op.size==4
                m=op.mem;assert not m.base and m.index and m.scale==4 and j>=2
                cmp,guard=ins[j-2:j]
                assert cmp.mnemonic=='cmp' and guard.mnemonic=='ja'
                assert cmp.operands[0].type==X86_OP_REG and cmp.operands[0].size==4 and cmp.operands[0].reg==m.index
                assert cmp.operands[1].type==X86_OP_IMM and 0<=cmp.operands[1].imm<=255
                assert guard.operands[0].type==X86_OP_IMM and guard.operands[0].imm!=i.address
                assert cmp.address+cmp.size==guard.address and guard.address+guard.size==i.address
                n=cmp.operands[1].imm+1;rva=m.disp-image.pe.OPTIONAL_HEADER.ImageBase
                section=image.pe.get_section_by_rva(rva);assert section is not None and not section.Characteristics&0x80000000
                raw=image.pe.get_data(rva,4*n);assert len(raw)==4*n
                targets=[int.from_bytes(raw[k:k+4],'little') for k in range(0,len(raw),4)]
                assert all(a in by for a in targets)
                edges=list(set(targets));tables.append(dict(jump=hex(i.address),compare=hex(cmp.address),guard=hex(guard.address),
                    table_address=hex(m.disp),entries=n,bytes_sha256=sha(raw),targets=[hex(a) for a in targets]))
        elif fall in by:edges.append(fall)
        graph[i.address]=edges
    pred=collections.defaultdict(set)
    for a,bs in graph.items():
        for b in bs:pred[b].add(a)
    for t in tables:
        assert pred[int(t['jump'],16)]=={int(t['guard'],16)}
        assert pred[int(t['guard'],16)]=={int(t['compare'],16)}
    return graph,tables


def clone_sites(snapshot,image,debug,native,keys):
    if not keys:return dict(initializer_barriers=[],groups=[])
    md,ins=decode(snapshot['view'],image)
    try:graph,tables=review_graph(ins,image)
    except AssertionError as exc:
        return dict(unsupported=str(exc),initializer_barriers=[],groups=[dict(offset=k[0],bytes=k[1],value=k[2],
            mutually_exclusive_before_reinitialization=False,no_repeat_before_reinitialization=False) for k in keys])
    # Run the existing frame-effect engine against this explicitly checked CFG
    # and the corrected lane map, only in this independent review process.
    old_graph,old_lanes=frame_effects.graph_for,frame_effects.FAMILIES
    try:
        frame_effects.graph_for=lambda _:graph;frame_effects.FAMILIES=FAMILIES
        init=[c for c in effects(image,debug,snapshot['view'])['calls']
            if c['name'] in ['_default_test_definition','_new_default_test_definition'] and c['arguments'][0]==['frame',snapshot['frame_offset']+8]]
    finally:frame_effects.graph_for,frame_effects.FAMILIES=old_graph,old_lanes
    barriers={int(c['address'],16) for c in init};groups=collections.defaultdict(list)
    for st in snapshot['all_stores']:groups[st['offset'],st['size'],normalized_value(st,native)].append(int(st['address'],16))
    def reach(a,b):
        todo=list(graph[a]);seen=set()
        while todo:
            at=todo.pop()
            if at in barriers or at in seen:continue
            if at==b:return True
            seen.add(at);todo.extend(graph[at])
        return False
    def idempotent_segment(a,b,key):
        if key[2] is None:return None
        forward=set();todo=list(graph[a])
        while todo:
            at=todo.pop()
            if at in barriers or at in forward:continue
            forward.add(at)
            if at!=b:todo.extend(graph[at])
        pred=collections.defaultdict(set)
        for at,nexts in graph.items():
            for nxt in nexts:
                if nxt not in barriers and at!=b:pred[nxt].add(at)
        back={b};todo=[b]
        while todo:
            at=todo.pop()
            for p in pred[at]:
                if p not in back:back.add(p);todo.append(p)
        segment=(forward&back)-{b}
        safe={'mov','lea','cmp','test','and','or','xor','add','sub','shl','shr','sar','inc','dec','nop','xchg'}
        selected=[i for i in ins if i.address in segment]
        for i in selected:
            if i.mnemonic not in safe and not i.mnemonic.startswith(('j','set')):return None
            if any(op.type==X86_OP_MEM and op.access&2 for op in i.operands):return None
        return dict(first_store=hex(a),second_store=hex(b),intervening_instructions=[[hex(i.address),i.mnemonic,i.op_str] for i in selected],
            reason='Same known field value; every connecting path has no intervening memory write or call.')
    result=[]
    for key in keys:
        sites=groups.get(key,[])
        between=[(a,b) for a in sites for b in sites if a!=b and reach(a,b)]
        loops=[a for a in sites if reach(a,a)]
        repeats=[idempotent_segment(a,b,key) for a,b in between]
        result.append(dict(offset=key[0],bytes=key[1],value=key[2],sites=[hex(a) for a in sites],
            mutually_exclusive_before_reinitialization=not between,no_repeat_before_reinitialization=not loops,
            reachable_store_pairs=[[hex(a),hex(b)] for a,b in between],repeating_sites=[hex(a) for a in loops],
            connecting_pairs_are_idempotent=all(p is not None for p in repeats),idempotent_pair_witnesses=[p for p in repeats if p is not None]))
    return dict(initializer_barriers=init,checked_switch_tables=tables,groups=result)


def serial(c):return [dict(field=f,byte=b,value=v,count=n) for (f,b,v),n in sorted(c.items(),key=lambda kv:(kv[0][0],kv[0][1],str(kv[0][2])))]
def counter(v):return collections.Counter({(e['field'],e['byte'],e['value']):e['count'] for e in v['events']})
ns=symbols(ni,nd);cs=symbols(ci,cd);native={};current=collections.defaultdict(list);dw=nd.dwarf();cache={}
for r in seeds:
    rows=[]
    for at in r['native_variables']:
        v=dw.get_DIE_from_refaddr(int(at,16));d=v.get_parent()
        while d.tag!='DW_TAG_subprogram':d=d.get_parent()
        rows.append(snapshot(ni,nd,dw,d,v,None,True,cache))
    native[r['source'],r['name']]=rows
dw=cd.dwarf();wanted={(r['source'],r['name']) for r in seeds}
for cu in dw.iter_CUs():
    source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')
    if source not in {s for s,n in wanted}:continue
    cache={}
    for d in cu.get_top_DIE().iter_children():
        key=source,inherited_value(d,'DW_AT_name')
        if d.tag!='DW_TAG_subprogram' or key not in wanted or not ranges(dw,d):continue
        for v in walk(d):
            if v.tag!='DW_TAG_variable':continue
            t=unqualify(inherited_type(v));members=[]
            if is_test(t):members=[None]
            elif t is not None and t.tag=='DW_TAG_union_type':
                members=[m for m in t.iter_children() if m.tag=='DW_TAG_member' and member_offset(m)==0 and is_test(inherited_type(m))]
            for m in members:
                try:frame_offset(dw,v)
                except AssertionError:continue
                current[key].append(snapshot(ci,cd,dw,d,v,m,False,cache))
    cu._dielist.clear();cu._diemap.clear()
rows=[];counts=collections.Counter()
for seed in seeds:
    key=seed['source'],seed['name'];csnap=current[key];comparisons=[]
    for n in native[key]:
        if not csnap:comparisons.append(dict(native=n,classification='unbound'));continue
        a=counter(n);exact=[c for c in csnap if a==counter(c)]
        same=[c for c in csnap if set(a)==set(counter(c))]
        aggregate=[c for c in csnap if c['kind']=='union' and not a-counter(c)]
        chosen=min(exact or same or aggregate or csnap,key=lambda c:sum((a-counter(c)).values())+sum((counter(c)-a).values()))
        b=counter(chosen)
        category='exact_scoped_scalar_events' if a==b else 'same_events_different_multiplicity' if set(a)==set(b) else 'aggregate_contains_native_events' if chosen in aggregate else 'needs_value_or_scope_review'
        comparisons.append(dict(native=n,current=chosen,classification=category,native_missing=serial(a-b),current_extra=serial(b-a)))
        counts[category]+=1
    n=native[key][0];whole_a=collections.Counter({(e['field'],e['byte'],e['value']):e['count'] for e in n['all_events']})
    def whole(v):return collections.Counter({(e['field'],e['byte'],e['value']):e['count'] for e in v['all_events']})
    c=min(csnap,key=lambda c:sum((whole_a-whole(c)).values())+sum((whole(c)-whole_a).values()))
    whole_b=whole(c)
    raw_kind='exact_whole_frame_scalar_events' if whole_a==whole_b else 'same_whole_frame_event_set' if set(whole_a)==set(whole_b) else 'dynamic_value_or_other_difference'
    def groups(v,nat):return collections.Counter((s['offset'],s['size'],normalized_value(s,nat)) for s in v['all_stores'])
    ng,cg=groups(n,True),groups(c,False);changed=[k for k in set(ng)|set(cg) if ng[k]!=cg[k]];changed.sort(key=lambda k:(k[0],k[1],str(k[2])))
    clone=dict(classification=raw_kind,native_owner_view=n['view'],current_owner_view=c['view'],native_missing=serial(whole_a-whole_b),current_extra=serial(whole_b-whole_a),
        native=clone_sites(n,ni,nd,True,changed),current=clone_sites(c,ci,cd,False,changed))
    if raw_kind=='same_whole_frame_event_set':
        exclusive=all(g['mutually_exclusive_before_reinitialization'] and g['no_repeat_before_reinitialization'] for s in ['native','current'] for g in clone[s]['groups'])
        good=all((g['mutually_exclusive_before_reinitialization'] or g.get('connecting_pairs_are_idempotent',False)) and g['no_repeat_before_reinitialization'] for s in ['native','current'] for g in clone[s]['groups'])
        clone['classification']='mutually_exclusive_store_clones' if exclusive else 'store_clones_and_idempotent_repeats' if good else 'store_multiplicity_needs_further_analysis'
    text=read(seed['source']);a,b=span(text,seed['name'])
    rows.append(dict(source=key[0],name=key[1],source_definition_sha256=sha(text[a:b].encode()),comparisons=comparisons,current_candidates=csnap,whole_frame_review=clone))
    print('WHOLE',key,clone['classification'],flush=True)
    print(key,'native scopes',len(comparisons),'current',len(csnap),'class',[c['classification'] for c in comparisons],flush=True)
    for c in comparisons:
        if c['classification']=='needs_value_or_scope_review':print('  missing',c['native_missing'],'extra',c['current_extra'],flush=True)
record=dict(native_dll_sha256=sha(ni.data),native_dbg_sha256=sha(nd.data),current_dll_sha256=sha(ci.data),current_dbg_sha256=sha(cd.data),mapping_sha256=sha(mapping_path.read_bytes()),seeds_path=seeds_path.as_posix(),seeds_sha256=sha(seeds_path.read_bytes()),tool_sha256=sha(Path(__file__).read_bytes()),counts=dict(counts),rows=rows,
    register_lane_extension=dict(si=['esi',16,0],di=['edi',16,0],bp=['ebp',16,0],sp=['esp',16,0]),whole_frame_counts=dict(collections.Counter(r['whole_frame_review']['classification'] for r in rows)),supporting_tool_sha256={p:sha(Path(p).read_bytes()) for p in ['tools/recovery/shared_stack_test_analysis.py','tools/recovery/array_role_analysis.py','tools/recovery/verify_wide_stack_test_recovery.py']},
    historical_checker_limits='The older basic-block collector omitted SI/DI/BP/SP16 lanes and did not invalidate every implicit register definition. A concrete Soul Spike SI load could therefore retain a stale whole-ESI zero. This independent collector handles all these lanes and explicit/implicit kills; prior artifacts stay immutable. Its selected25 rows supersede the old discovery inferences for those owners only. Broader old screens have not been replayed with this corrected model.',
    scope='Independent read-only review of25 old outlier owners, including six previously repaired functions. Direct scalar parameter events use complete partial lanes and explicit/implicit register invalidation. Differing store groups are accepted as clones only if mutually unreachable and nonrepeating before verified reinitialization of that same frame. One additional Revoker repeat writes an identical known value across paths with no intervening call or memory write. Readonly switch tables require adjacent CMP/JA bounds, exact index agreement, in-function entry targets and no bypass of the guard. These are direct-CFG store-shape proofs, not unknown dynamic value equivalence. Callback words use verified native mappings/COFF identities. Scope candidate matching by event sets is not semantic scope identity. Dynamic equal-unknowns, aliases, indexed stores, complete behavior and gameplay remain unproven.')
out=Path(args.output);assert not out.exists();out.write_bytes((json.dumps(record,indent=2)+'\n').encode());print('COUNTS',dict(counts),flush=True)
