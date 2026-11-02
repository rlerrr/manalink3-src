"""Reaching-zero and liveness proofs for direction bytes with mixed lifetimes."""
import collections,re
from pycparser import c_ast
from canonicalize_helpers import mask
from source_record_flow import parse,inspect
from call_condition_source import walk
from direction_flag_source import GEN,PATTERN,stride_facts,render_stride_probes,probe_unit as basic_probe_unit
from special_call_condition_source import analysis_view
from verify_source_spelling_edits import substitute

def view(text,name,expanded=False):
    if name=='charge_spell_cost':return analysis_view(text,name,expanded)
    return text,[]

def active_walk(node):
    if node is None:return
    yield node
    if isinstance(node,(c_ast.Case,c_ast.Default)):return
    for _,child in node.children():yield from active_walk(child)

def writes(node,flag):
    return [n for n in active_walk(node) if isinstance(n,c_ast.Assignment) and isinstance(n.lvalue,c_ast.ID) and n.lvalue.name==flag or isinstance(n,c_ast.UnaryOp) and n.op in ['++','--','p++','p--'] and isinstance(n.expr,c_ast.ID) and n.expr.name==flag]

def zero_node(node,flag):
    return bool(isinstance(node,c_ast.Assignment) and node.op=='=' and isinstance(node.lvalue,c_ast.ID) and node.lvalue.name==flag and isinstance(node.rvalue,c_ast.Constant) and node.rvalue.value=='0' or isinstance(node,c_ast.Decl) and node.name==flag and isinstance(node.init,c_ast.Constant) and node.init.value=='0')

def states(cfg,flag):
    incoming={cfg['entry']:frozenset({'uninitialized'})};todo=collections.deque([cfg['entry']]);updates=0
    while todo:
        at=todo.popleft();node=cfg['nodes'][at];out=incoming[at]
        if zero_node(node,flag):out=frozenset({'zero'})
        elif writes(node,flag) or isinstance(node,c_ast.Decl) and node.name==flag and node.init is not None:out=frozenset({'unknown'})
        for nxt in cfg['edges'][at]:
            merged=incoming.get(nxt,frozenset())|out
            if incoming.get(nxt)!=merged:incoming[nxt]=merged;todo.append(nxt);updates+=1;assert updates<100000
    return incoming,updates

def liveness(cfg,flag):
    uses={};defs={}
    for at,node in cfg['nodes'].items():
        def refs(n):return any(isinstance(v,c_ast.ID) and v.name==flag for v in active_walk(n))
        if isinstance(node,c_ast.Assignment) and node.op=='=' and isinstance(node.lvalue,c_ast.ID) and node.lvalue.name==flag:uses[at]=refs(node.rvalue);defs[at]=True
        elif isinstance(node,c_ast.Decl) and node.name==flag:uses[at]=refs(node.init);defs[at]=node.init is not None
        else:uses[at]=refs(node);defs[at]=False
    live={at:False for at in cfg['nodes']};changed=True
    while changed:
        changed=False
        for at in reversed(cfg['nodes']):
            out=any(live[n] for n in cfg['edges'][at]);value=uses[at] or out and not defs[at]
            if value!=live[at]:live[at]=value;changed=True
    return live

def analyze(text,name,expanded=False):
    adapted,adapters=view(text,name,expanded);code=mask(adapted);assert not re.search(r'\b(?:EXE_FN|asm|__asm|__asm__|__builtin_frame_address|__builtin_return_address|setjmp|longjmp|_setjmp|_longjmp|sigsetjmp|siglongjmp)\b',code)
    fn,offset=parse(adapted);nodes=list(walk(fn));parents={id(c):n for n in nodes for _,c in n.children()};decls=collections.defaultdict(list);ids=collections.defaultdict(list);parens={};stack=[]
    for at,ch in enumerate(code):
        if ch=='(':stack.append(at)
        elif ch==')':assert stack;parens[stack.pop()]=at
    assert not stack
    for n in nodes:
        if isinstance(n,c_ast.Decl):decls[n.name].append(n)
        if isinstance(n,c_ast.ID):ids[n.name].append(n)
    cfg=inspect(adapted,cfg_only=True);found=[];matches=list(PATTERN.finditer(code))
    for flag in sorted({m['flag'] for m in matches}):
        ds=decls[flag]
        if len(ds)!=1:continue
        d=ds[0]
        if d not in (fn.body.block_items or []) or d.storage or d.quals or not isinstance(d.type,c_ast.TypeDecl) or d.type.quals or not isinstance(d.type.type,c_ast.IdentifierType) or d.type.type.names!=['unsigned','char']:continue
        if any(isinstance(n,c_ast.UnaryOp) and n.op=='&' and isinstance(n.expr,c_ast.ID) and n.expr.name==flag for n in nodes):continue
        assert all(not re.search(r'\b'+re.escape(flag)+r'\b',a['original']) for a in adapters)
        incoming,updates=states(cfg,flag);uses=[m for m in matches if m['flag']==flag and (int(m['scale']),int(m['step'])) in [(8,4),(2,1)]];owners=[]
        for m in uses:
            at=[at for at,node in cfg['nodes'].items() if any(isinstance(n,c_ast.ID) and n.name==flag and cfg['offset'](n)==m.start('flag') for n in active_walk(node))];assert len(at)==1;at=at[0]
            assert incoming.get(at)==frozenset({'zero'}) and not writes(cfg['nodes'][at],flag)
            owners.append(dict(flag_offset=m.start('flag'),node=at,incoming_states=sorted(incoming[at])))
        if not uses:continue
        strides=stride_facts(text,flag,uses,parents,ids,offset,code,parens);edits=[(u['fold_start'],u['fold_end'],u['fold_literal']) for u in strides];folded=text
        for a,b,new in sorted(edits,reverse=True):folded=folded[:a]+new+folded[b:]
        folded_view,_=view(folded,name,expanded);after_cfg=inspect(folded_view,cfg_only=True);live=liveness(after_cfg,flag);initials=[at for at,node in after_cfg['nodes'].items() if zero_node(node,flag)];assert len(initials)==1;initial=initials[0];assert not any(live[n] for n in after_cfg['edges'][initial])
        originals=[n for n in fn.body.block_items or [] if zero_node(n,flag)];assert len(originals)==1;init=originals[0];assert offset(init)<min(u['fold_start'] for u in strides)
        a=text.rfind('\n',0,offset(init))+1;b=text.find('\n',offset(init))+1;assert b
        if isinstance(init,c_ast.Decl):assert re.fullmatch(r'\s*unsigned char '+re.escape(flag)+r'\s*=\s*0;\s*',text[a:b])
        else:assert re.fullmatch(r'\s*'+re.escape(flag)+r'\s*=\s*0;\s*',text[a:b])
        no_init=folded[:a]+folded[b:];remaining=any(isinstance(n,c_ast.ID) and n.name==flag for n in walk(parse(view(no_init,name,expanded)[0])[0]))
        assert not (remaining and isinstance(init,c_ast.Decl))
        declaration=None
        if not remaining and not isinstance(init,c_ast.Decl):
            lo=text.rfind('\n',0,offset(d))+1;hi=text.find('\n',offset(d))+1;assert re.fullmatch(r'\s*unsigned char '+re.escape(flag)+r'\s*;\s*',text[lo:hi]);declaration=[lo,hi]
        rename=None
        if remaining:
            assert name=='card_bloodletter' and flag=='bVar6'
            assert not re.search(r'\bltr\b',mask(text))
            assignments=[n for n in nodes if isinstance(n,c_ast.Assignment) and isinstance(n.lvalue,c_ast.ID) and n.lvalue.name==flag and not zero_node(n,flag)]
            assert len(assignments)==1 and GEN.visit(assignments[0].rvalue)=='native_name_fold[*pbVar4]';rename='ltr'
        found.append(dict(variable=flag,stride_uses=strides,initialization_start=a,initialization_end=b,initialization_is_declaration=isinstance(init,c_ast.Decl),removed_declaration=declaration,remove_object=not remaining,rename_to=rename,reaching_zero=dict(reachable_nodes=len(incoming),updates=updates,stride_nodes=owners),initial_value_dead_after_folding=True,liveness_initializer_node=initial,syntax_adapters=adapters))
    return found

def project(before,selected,name,expanded=False):
    assert selected and all(v in analyze(before,name,expanded) for v in selected);edits=[]
    for c in selected:
        edits.append((c['initialization_start'],c['initialization_end'],''))
        if c['removed_declaration']:edits.append((*c['removed_declaration'],''))
        edits.extend((u['fold_start'],u['fold_end'],u['fold_literal']) for u in c['stride_uses'])
    edits.sort();assert all(a[1]<=b[0] for a,b in zip(edits,edits[1:]));after=before
    for a,b,new in reversed(edits):after=after[:a]+new+after[b:]
    for c in selected:
        if c['rename_to']:after=substitute(after,{c['variable']:c['rename_to']})
        assert not re.search(r'\b'+re.escape(c['variable'])+r'\b',mask(after))
    return after,dict(folded_strides=sum(len(c['stride_uses']) for c in selected),removed_flags=sum(c['remove_object'] for c in selected),removed_initializations=len(selected),renamed_role_locals=sum(c['rename_to'] is not None for c in selected),source_candidates=selected)

def probe_unit(expanded,rows):
    def definition(before,row):
        selected=[c['expanded_fact'] for c in row['checks']]
        assert all(v in analyze(before,row['name'],True) for v in selected)
        return render_stride_probes(before,selected)
    return basic_probe_unit(expanded,rows,definition_probe=definition)
