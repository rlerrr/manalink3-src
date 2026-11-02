"""Recover a common spell-event guard/setup/resolution layout from shared tails."""
import collections,re
from pycparser import c_ast,c_generator
from canonicalize_helpers import mask
from source_record_flow import parse

GEN=c_generator.CGenerator()
GUARD='((event == EVENT_CAN_CAST || (event == EVENT_CAST_SPELL && affect_me(player, card)) || event == EVENT_RESOLVE_SPELL) || event == EVENT_CHANGE_TARGET || event == EVENT_CAN_CHANGE_TARGET)'
EVENTS=['EVENT_CAN_CAST','EVENT_CAST_SPELL','EVENT_RESOLVE_SPELL','EVENT_CAN_CHANGE_TARGET','EVENT_CHANGE_TARGET']


def walk(node):
    if node is None:return
    yield node
    for _,child in node.children():yield from walk(child)


def same(node,text):return GEN.visit(node)==text


def items(node):
    return node.block_items or [] if isinstance(node,c_ast.Compound) else [node]


def zero_return(node):
    s=items(node)
    return len(s)==1 and isinstance(s[0],c_ast.Return) and isinstance(s[0].expr,c_ast.Constant) and s[0].expr.value=='0'


def affinity_test(node,card_match,player_match):
    if isinstance(node,c_ast.Constant):assert node.value in ['0','1'];return int(node.value)
    if isinstance(node,c_ast.UnaryOp):assert node.op=='!';return not affinity_test(node.expr,card_match,player_match)
    assert isinstance(node,c_ast.BinaryOp)
    if node.op=='&&':return bool(affinity_test(node.left,card_match,player_match)) and bool(affinity_test(node.right,card_match,player_match))
    if node.op=='||':return bool(affinity_test(node.left,card_match,player_match)) or bool(affinity_test(node.right,card_match,player_match))
    assert node.op in ['==','!='] and isinstance(node.left,c_ast.ID) and isinstance(node.right,c_ast.ID)
    pair={node.left.name,node.right.name}
    assert pair in [{'card','affected_card'},{'player','affected_card_controller'}]
    equal=card_match if 'card' in pair else player_match
    return equal if node.op=='==' else not equal


def _project_original(before,*,expanded=False):
    assert '\\\n' not in before and not re.search(r'\b(?:EXE_FN|asm|__asm__|__builtin_frame_address|__builtin_return_address)\b',mask(before))
    fn,offset=parse(before);code=mask(before);nodes=list(walk(fn));body=fn.body.block_items or []
    assert isinstance(fn.decl.type.type,c_ast.TypeDecl) and isinstance(fn.decl.type.type.type,c_ast.IdentifierType) and fn.decl.type.type.type.names==['int']
    assert fn.decl.type.args and [d.name for d in fn.decl.type.args.params]==['player','card','event']
    assert all(not d.quals and not getattr(d.type,'quals',[]) for d in fn.decl.type.args.params)
    assert not any(isinstance(n,c_ast.UnaryOp) and n.op=='&' and isinstance(n.expr,c_ast.ID) and n.expr.name in ['player','card','event'] for n in nodes)
    assert not any(isinstance(n,(c_ast.Decl,c_ast.Typedef,c_ast.Enumerator)) and n.name in ['affect_me',*EVENTS] for n in nodes)
    labels=[n for n in nodes if isinstance(n,c_ast.Label)];jumps=[n for n in nodes if isinstance(n,c_ast.Goto)]
    assert len(labels)==len(jumps)==1 and labels[0].name==jumps[0].name
    label=labels[0];assert body[-1] is label and isinstance(label.stmt,c_ast.Return)
    assert isinstance(label.stmt.expr,c_ast.FuncCall) and isinstance(label.stmt.expr.name,c_ast.ID)
    guards=[(i,n) for i,n in enumerate(body) if isinstance(n,c_ast.If) and same(n.cond,'event != EVENT_CAN_CAST') and n.iffalse is None]
    assert len(guards)==1;i,guard=guards[0];outer=items(guard.iftrue);assert len(outer)==1
    casting=outer[0];assert isinstance(casting,c_ast.If) and same(casting.cond,'event == EVENT_CAST_SPELL') and casting.iffalse is not None
    cast_guards=items(casting.iftrue)
    assert cast_guards and all(isinstance(n,c_ast.If) and n.iffalse is None and zero_return(n.iftrue) for n in cast_guards)
    for card_match in [False,True]:
        for player_match in [False,True]:
            assert (not any(affinity_test(n.cond,card_match,player_match) for n in cast_guards))==(card_match and player_match)
    alternative=items(casting.iffalse);assert len(alternative)==2
    resolving,other=alternative
    assert isinstance(resolving,c_ast.If) and same(resolving.cond,'(event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)') and resolving.iffalse is None
    assert isinstance(other,c_ast.If) and same(other.cond,'event != EVENT_CHANGE_TARGET') and other.iffalse is None and zero_return(other.iftrue)
    resolution=items(resolving.iftrue);assert len(resolution)>=2 and resolution[-1] is jumps[0]
    payload=resolution[-2];assert isinstance(payload,c_ast.If) and same(payload.cond,'event == EVENT_RESOLVE_SPELL') and payload.iffalse is None
    setup1=resolution[:-2];setup2=body[i+1:-1]
    assert len(setup1)==len(setup2)
    assert all(isinstance(n,(c_ast.FuncCall,c_ast.Assignment)) for n in setup1+setup2)
    assert not any(isinstance(n,c_ast.Assignment) and isinstance(n.lvalue,c_ast.ID) and n.lvalue.name in ['player','card','event'] for s in setup1+setup2 for n in walk(s))
    # Constructor calls may change arbitrary global state. The extra event
    # test after their shared setup is safe only for a private unchanged event.
    assert not any(isinstance(n,c_ast.Assignment) and isinstance(n.lvalue,c_ast.ID) and n.lvalue.name=='event' for s in setup1+setup2 for n in walk(s))
    declarations=collections.defaultdict(list)
    for n in nodes:
        if isinstance(n,c_ast.Decl):declarations[n.name].append(n)
    captures=[]
    for first,second in zip(setup1,setup2):
        if GEN.visit(first)==GEN.visit(second):continue
        assert isinstance(first,c_ast.Assignment) and first.op=='=' and isinstance(first.lvalue,c_ast.ID) and isinstance(first.rvalue,c_ast.FuncCall)
        assert isinstance(second,c_ast.FuncCall) and GEN.visit(first.rvalue)==GEN.visit(second)
        assert same(first.rvalue.name,'get_card_instance') and same(first.rvalue.args,'player, card')
        name=first.lvalue.name;decls=declarations[name];assert len(decls)==1;decl=decls[0]
        assert decl in body and decl.init is None and not decl.storage and not decl.quals and isinstance(decl.type,c_ast.PtrDecl) and not decl.type.quals
        assert isinstance(decl.type.type,c_ast.TypeDecl) and not decl.type.type.quals and isinstance(decl.type.type.type,c_ast.IdentifierType) and decl.type.type.type.names==['card_instance_t']
        assert not any(isinstance(n,c_ast.UnaryOp) and n.op=='&' and isinstance(n.expr,c_ast.ID) and n.expr.name==name for n in nodes)
        # Every reference outside the unchanged resolution body must be the
        # declaration or this assignment. In particular the tail cannot observe
        # the additional private pointer assignment on non-resolution events.
        uses=[n for n in nodes if isinstance(n,c_ast.ID) and n.name==name]
        allowed={id(n) for n in walk(payload)}|{id(first.lvalue)}
        assert all(id(n) in allowed for n in uses)
        assert not re.search(r'(?:\.|->)\s*'+re.escape(name)+r'\b',code)
        captures.append(name)

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

    def region(node):return before[before.rfind('\n',0,offset(node))+1:end(node)]
    def shifted(text,new_indent=2):
        first=text.splitlines()[0];old_indent=len(first)-len(first.lstrip());delta=old_indent-new_indent
        assert delta>=0
        return '\n'.join(line[delta:] if line.startswith(' '*delta) else line for line in text.split('\n'))
    setup='\n'.join(shifted(region(n)) for n in setup1)
    payload_text=shifted(region(payload));tail=shifted(region(label.stmt))
    start=before.rfind('\n',0,offset(guard))+1
    assert not before[start:offset(guard)].strip()
    prefix=before[:start]
    after=prefix+'  if (!'+(GUARD if expanded else 'IS_GS_EVENT(player, card, event)')+')\n  {\n    return 0;\n  }\n'
    if setup:after+=setup+'\n'
    after+=payload_text+'\n'+tail+'\n}'
    # A compact parsed proof retains the complete original event control flow,
    # while identical setup and payload blocks are treated as opaque actions.
    skeleton=before[start:];edits=[]
    for block in [setup1,setup2]:
        if block:edits.append((offset(block[0])-start,end(block[-1])-start,'dispatch_setup();'))
    edits.append((offset(payload.iftrue)-start,end(payload.iftrue)-start,'{ dispatch_payload(); }'))
    edits.append((offset(label.stmt)-start,end(label.stmt)-start,'return dispatch_tail();'))
    for a,b,new in sorted(edits,reverse=True):skeleton=skeleton[:a]+new+skeleton[b:]
    old_dispatch='int dispatch(int player, int card, event_t event)\n{\n'+skeleton
    new_dispatch='int dispatch(int player, int card, event_t event)\n{\n  if (!'+GUARD+') return 0;\n'+('  dispatch_setup();\n' if setup1 else '')+'  if (event == EVENT_RESOLVE_SPELL) { dispatch_payload(); }\n  return dispatch_tail();\n}'
    return after,dict(old_dispatch=old_dispatch,new_dispatch=new_dispatch,private_captures=captures,setup_statements=len(setup1),common_setup=setup,resolution_block=payload_text,tail_return=tail,removed_gotos=1,removed_labels=1)


def project(before,*,expanded=False):
    try:
        return _project_original(before,expanded=expanded)
    except AssertionError:
        from shared_spell_dispatch_source import project as project_dispatch
        return project_dispatch(before,expanded=expanded)


def guard_definition(expanded):
    """Bind the pure inline predicate actually visible in this compiler unit."""
    from preprocessed_conditions import preprocess_view
    get_function,_=preprocess_view(expanded);definition,_=get_function('affect_me');fn,_=parse(definition)
    assert fn.decl.storage==['static'] and 'inline' in fn.decl.funcspec
    assert [d.name for d in fn.decl.type.args.params]==['player','card']
    assert all(isinstance(d.type,c_ast.TypeDecl) and isinstance(d.type.type,c_ast.IdentifierType) and d.type.type.names==['int'] and not d.quals for d in fn.decl.type.args.params)
    assert len(fn.body.block_items)==1 and isinstance(fn.body.block_items[0],c_ast.Return)
    assert GEN.visit(fn.body.block_items[0].expr)=='(card == affected_card) && (player == affected_card_controller)'
    for name in ['affected_card','affected_card_controller']:
        assert re.search(r'(?m)^extern\s+int\s+'+name+r'\s*;',expanded)
    return definition


def trace_proof(facts,event_values):
    """Compare parsed dispatches for every event/affinity/action outcome class.

    Setup can change both affected-card globals. The unchanged payload can
    either return from the function or fall through with arbitrary effects.
    The only elided store is a nonescaping pointer unread outside that payload.
    """
    from itertools import product
    from source_record_flow import inspect
    values={n:event_values[n] for n in EVENTS};assert len(set(values.values()))==5
    classes=list(values.values());other=0
    while other in classes:other+=1
    classes.append(other)
    old=inspect(facts['old_dispatch'],cfg_only=True);new=inspect(facts['new_dispatch'],cfg_only=True)
    class PayloadReturn(Exception):pass
    def run(cfg,event,card_match,player_match,post_card,post_player,early_return):
        env=dict(event=event,player=1,card=4,affected_card=4 if card_match else 5,affected_card_controller=1 if player_match else 0)
        actions=[];reads=[]
        def evaluate(n):
            if isinstance(n,c_ast.Constant):return int(n.value,0)
            if isinstance(n,c_ast.ID):
                if n.name in ['affected_card','affected_card_controller']:reads.append(n.name)
                return env[n.name] if n.name in env else values[n.name]
            if isinstance(n,c_ast.UnaryOp):assert n.op=='!';return int(not evaluate(n.expr))
            if isinstance(n,c_ast.BinaryOp):
                left=evaluate(n.left)
                if n.op=='&&':return int(bool(left) and bool(evaluate(n.right)))
                if n.op=='||':return int(bool(left) or bool(evaluate(n.right)))
                right=evaluate(n.right)
                if n.op=='==':return int(left==right)
                if n.op=='!=':return int(left!=right)
                raise AssertionError(n.op)
            assert isinstance(n,c_ast.FuncCall) and isinstance(n.name,c_ast.ID)
            name=n.name.name
            if name=='affect_me':
                assert GEN.visit(n.args)=='player, card'
                reads.append('affected_card')
                if env['card']!=env['affected_card']:return 0
                reads.append('affected_card_controller');return int(env['player']==env['affected_card_controller'])
            assert name in ['dispatch_setup','dispatch_payload','dispatch_tail'] and n.args is None
            actions.append(name)
            if name=='dispatch_setup':env.update(affected_card=4 if post_card else 5,affected_card_controller=1 if post_player else 0)
            if name=='dispatch_payload' and early_return:raise PayloadReturn()
            return ['arbitrary_tail_result'] if name=='dispatch_tail' else 0
        at=cfg['entry'];seen=set()
        try:
            while True:
                assert at not in seen;seen.add(at);node=cfg['nodes'][at];nexts=cfg['edges'][at]
                if not nexts:
                    assert node is not None
                    returned=evaluate(node);break
                if len(nexts)==2:at=nexts[0 if evaluate(node) else 1];continue
                if node is not None:evaluate(node)
                assert len(nexts)==1;at=nexts[0]
        except PayloadReturn:returned=['arbitrary_payload_return']
        return dict(actions=actions,global_guard_reads=reads,returned=returned)
    rows=[]
    for event in classes:
        for card,player,post_card,post_player,early in product([False,True],repeat=5):
            a=run(old,event,card,player,post_card,post_player,early);b=run(new,event,card,player,post_card,post_player,early);assert a==b,(event,card,player,a,b)
            rows.append(dict(event=event,card_match=card,player_match=player,post_card_match=post_card,post_player_match=post_player,payload_returns=early,trace=a))
    return dict(cases=rows,event_classes=6,case_count=len(rows),scope='Parsed statement CFGs: five distinct native event values plus other; both initial affinity facts; arbitrary post-setup affinity changes; and either opaque-payload fallthrough or function return. Observable action order, guard-global read order and arbitrary return values match. Setup and payload source/bindings remain checked separately; no helper-purity, full-native-semantic or gameplay claim.')
