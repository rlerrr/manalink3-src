"""Candidate projection for shared spell dispatches with varied branch layouts."""
import collections,re
from pycparser import c_ast
from canonicalize_helpers import mask
from source_record_flow import parse,inspect
from shared_spell_source import GEN,GUARD,EVENTS,walk,items,same

def project(before,*,expanded=False):
    assert '\\\n' not in before and not re.search(r'\b(?:EXE_FN|asm|__asm__|__builtin_frame_address|__builtin_return_address)\b',mask(before))
    fn,offset=parse(before);code=mask(before);nodes=list(walk(fn));body=fn.body.block_items or []
    assert isinstance(fn.decl.type.type,c_ast.TypeDecl) and isinstance(fn.decl.type.type.type,c_ast.IdentifierType) and fn.decl.type.type.type.names==['int']
    assert fn.decl.type.args and [d.name for d in fn.decl.type.args.params]==['player','card','event']
    assert all(not d.quals and not getattr(d.type,'quals',[]) for d in fn.decl.type.args.params)
    assert not any(isinstance(n,c_ast.UnaryOp) and n.op=='&' and isinstance(n.expr,c_ast.ID) and n.expr.name in ['player','card','event'] for n in nodes)
    assert not any(isinstance(n,(c_ast.Decl,c_ast.Typedef,c_ast.Enumerator)) and n.name in ['affect_me','affected_card','affected_card_controller',*EVENTS] for n in nodes)
    # The terminal label can also precede several straight-line tail statements.
    labels=[(i,n) for i,n in enumerate(body) if isinstance(n,c_ast.Label)]
    assert labels;i,label=labels[-1];tail_nodes=[label.stmt,*body[i+1:]]
    assert isinstance(tail_nodes[-1],c_ast.Return) and tail_nodes[-1].expr is not None
    assert all(isinstance(n,(c_ast.Assignment,c_ast.FuncCall)) for n in tail_nodes[:-1])
    guards=[(j,n) for j,n in enumerate(body[:i]) if isinstance(n,c_ast.If) and 'EVENT_CAN_CAST' in GEN.visit(n.cond)]
    assert guards;j,guard=guards[0];dispatch=body[j:i];dispatch_nodes=[n for s in dispatch for n in walk(s)]
    payloads=[n for n in dispatch_nodes if isinstance(n,c_ast.If) and same(n.cond,'event == EVENT_RESOLVE_SPELL') and n.iffalse is None]
    assert len(payloads)==1;payload=payloads[0];inside={id(n) for n in walk(payload.iftrue)}
    all_labels=[n for n in nodes if isinstance(n,c_ast.Label)];jumps=[n for n in nodes if isinstance(n,c_ast.Goto)]
    tail_jumps=[n for n in jumps if n.name==label.name];assert tail_jumps and all(n in dispatch_nodes and id(n) not in inside for n in tail_jumps)
    assert all(id(n) in inside for n in all_labels if n is not label)
    inner_labels={n.name for n in all_labels if id(n) in inside}
    assert all(n.name in inner_labels for n in jumps if id(n) in inside)
    assert all(n.name==label.name for n in jumps if id(n) not in inside)
    # Branch tests in the dispatcher are pure event/affinity predicates. Calls,
    # declarations, loops, switches and arbitrary expressions remain opaque only
    # inside the source-identical resolution block or terminal tail.
    def pure(n):
        if isinstance(n,c_ast.Constant):assert n.type=='int' and n.value in ['0','1'];return
        if isinstance(n,c_ast.UnaryOp):assert n.op=='!';pure(n.expr);return
        assert isinstance(n,c_ast.BinaryOp) and n.op in ['==','!=','&&','||']
        if n.op in ['&&','||']:pure(n.left);pure(n.right);return
        assert isinstance(n.left,c_ast.ID) and isinstance(n.right,c_ast.ID)
        pair={n.left.name,n.right.name}
        assert pair in [{'card','affected_card'},{'player','affected_card_controller'}] or ('event' in pair and len(pair & set(EVENTS))==1)
    parents={id(c):n for n in nodes for _,c in n.children()}
    actions=[]
    for n in dispatch_nodes:
        if id(n) in inside:continue
        if isinstance(n,c_ast.If):pure(n.cond)
        if isinstance(n,(c_ast.For,c_ast.While,c_ast.DoWhile,c_ast.Switch,c_ast.Label,c_ast.Decl,c_ast.Typedef,c_ast.Break,c_ast.Continue)):raise AssertionError('non-dispatch structure')
        if isinstance(n,c_ast.Return):assert isinstance(n.expr,c_ast.Constant) and n.expr.value=='0'
        p=parents.get(id(n))
        if isinstance(p,c_ast.Compound) and isinstance(n,(c_ast.FuncCall,c_ast.Assignment)):actions.append(n)
    # No action can hide in an unbraced arm or in a branch condition.
    for n in dispatch_nodes:
        if id(n) in inside:continue
        if isinstance(n,(c_ast.FuncCall,c_ast.Assignment)):
            assert any(n is c for a in actions for c in walk(a))
    groups=[];seen=set()
    for a in actions:
        if id(a) in seen:continue
        p=parents[id(a)];siblings=p.block_items;at=next(k for k,n in enumerate(siblings) if n is a);group=[]
        while at<len(siblings) and isinstance(siblings[at],(c_ast.Assignment,c_ast.FuncCall)):
            group.append(siblings[at]);seen.add(id(siblings[at]));at+=1
        groups.append(group)
    if groups:
        # Prefer the resolution copy so private instance captures stay in their
        # original position. The complete sequence must repeat as a unit.
        p=parents[id(payload)];assert isinstance(p,c_ast.Compound);siblings=p.block_items;k=next(k for k,n in enumerate(siblings) if n is payload)
        candidates=[g for g in groups if g[-1] is siblings[k-1]] if k else []
        assert len(candidates)==1;setup=candidates[0]
    else:setup=[]
    declarations=collections.defaultdict(list)
    for n in nodes:
        if isinstance(n,c_ast.Decl):declarations[n.name].append(n)
    captures=[];capture_stores=collections.defaultdict(set)
    for group in groups:
        assert len(group)==len(setup)
        for first,second in zip(setup,group):
            if GEN.visit(first)==GEN.visit(second):continue
            assert isinstance(first,c_ast.Assignment) and first.op=='=' and isinstance(first.lvalue,c_ast.ID) and isinstance(first.rvalue,c_ast.FuncCall)
            assert isinstance(second,c_ast.FuncCall) and GEN.visit(first.rvalue)==GEN.visit(second)
            assert same(first.rvalue.name,'get_card_instance') and same(first.rvalue.args,'player, card')
            name=first.lvalue.name;decls=declarations[name];assert len(decls)==1;decl=decls[0]
            assert decl in body and decl.init is None and not decl.storage and not decl.quals and isinstance(decl.type,c_ast.PtrDecl) and not decl.type.quals
            assert isinstance(decl.type.type,c_ast.TypeDecl) and not decl.type.type.quals and isinstance(decl.type.type.type,c_ast.IdentifierType) and decl.type.type.type.names==['card_instance_t']
            if name not in captures:captures.append(name)
    for name in captures:
        for group in groups:
            for a in group:
                if isinstance(a,c_ast.Assignment) and isinstance(a.lvalue,c_ast.ID) and a.lvalue.name==name:capture_stores[name].add(id(a.lvalue))
        assert not any(isinstance(n,c_ast.UnaryOp) and n.op=='&' and isinstance(n.expr,c_ast.ID) and n.expr.name==name for n in nodes)
        assert all(id(n) in inside or id(n) in capture_stores[name] for n in nodes if isinstance(n,c_ast.ID) and n.name==name)
        assert not re.search(r'(?:\.|->)\s*'+re.escape(name)+r'\b',code)
    for group in groups:
        assert not any(isinstance(n,c_ast.Assignment) and isinstance(n.lvalue,c_ast.ID) and n.lvalue.name in ['player','card','event'] for a in group for n in walk(a))
        assert not any(isinstance(n,c_ast.UnaryOp) and n.op in ['++','--','p++','p--'] and isinstance(n.expr,c_ast.ID) and n.expr.name in ['player','card','event'] for a in group for n in walk(a))

    def end(node):
        if isinstance(node,c_ast.Label):return end(node.stmt)
        if isinstance(node,c_ast.If):return end(node.iffalse if node.iffalse is not None else node.iftrue)
        start=offset(node)
        if isinstance(node,c_ast.Compound):
            at=code.index('{',start);assert not code[start:at].strip();depth=1;at+=1
            while depth:depth+=(code[at]=='{')-(code[at]=='}');at+=1
            return at
        at=start;parens=braces=0
        while True:
            c=code[at]
            if c==';' and parens==braces==0:return at+1
            parens+=(c=='(')-(c==')');braces+=(c=='{')-(c=='}');at+=1
    def region(n):return before[before.rfind('\n',0,offset(n))+1:end(n)]
    def shifted(text):
        first=text.splitlines()[0];delta=len(first)-len(first.lstrip())-2;assert delta>=0
        return '\n'.join(line[delta:] if line.startswith(' '*delta) else line for line in text.split('\n'))
    setup_text='\n'.join(shifted(region(n)) for n in setup)
    payload_text=shifted(region(payload));tail_text='\n'.join(shifted(region(n)) for n in tail_nodes)
    start=before.rfind('\n',0,offset(guard))+1;assert not before[start:offset(guard)].strip()
    tail_start=before.rfind('\n',0,offset(label))+1
    assert not before[tail_start:offset(label)].strip()
    skeleton=before[start:];edits=[]
    for group in groups:edits.append((offset(group[0])-start,end(group[-1])-start,'dispatch_setup();'))
    edits.append((offset(payload.iftrue)-start,end(payload.iftrue)-start,'{ dispatch_payload(); }'))
    edits.append((offset(label)-start,end(tail_nodes[-1])-start,label.name+': return dispatch_tail();'))
    for a,b,new in sorted(edits,reverse=True):skeleton=skeleton[:a]+new+skeleton[b:]
    old_dispatch='int dispatch(int player, int card, event_t event)\n{\n'+skeleton
    new_dispatch='int dispatch(int player, int card, event_t event)\n{\n  if (!'+GUARD+') return 0;\n'+('  dispatch_setup();\n' if setup else '')+'  if (event == EVENT_RESOLVE_SPELL) { dispatch_payload(); }\n  return dispatch_tail();\n}'
    # The payload may alter parameters as well as global state. Its fallthrough
    # must lead directly to the common tail, with no intervening predicate.
    cfg=inspect(old_dispatch,cfg_only=True)
    payload_calls=[at for at,n in cfg['nodes'].items() if isinstance(n,c_ast.FuncCall) and same(n.name,'dispatch_payload')]
    assert len(payload_calls)==1;at=payload_calls[0];seen=set()
    while True:
        assert at not in seen;seen.add(at);edges=cfg['edges'][at];assert len(edges)==1;at=edges[0];n=cfg['nodes'][at]
        if n is not None:assert isinstance(n,c_ast.FuncCall) and same(n.name,'dispatch_tail') and not cfg['edges'][at];break
    after=before[:start]+'  if (!'+(GUARD if expanded else 'IS_GS_EVENT(player, card, event)')+')\n  {\n    return 0;\n  }\n'+(setup_text+'\n' if setup else '')+payload_text+'\n'+tail_text+'\n}'
    return after,dict(old_dispatch=old_dispatch,new_dispatch=new_dispatch,private_captures=captures,setup_statements=len(setup),common_setup=setup_text,resolution_block=payload_text,tail_return=tail_text,removed_gotos=len(tail_jumps),removed_labels=1)
