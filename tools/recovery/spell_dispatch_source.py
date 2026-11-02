"""Structure one exact shared-tail spell dispatcher without adding helper calls."""
import re
from canonicalize_helpers import mask

PATTERN=re.compile(r'''  if \(event != EVENT_CAN_CAST\)
  \{
    if \(event == EVENT_CAST_SPELL\)
    \{
      if \(\(card == affected_card\) && \(player == affected_card_controller\)\)
        goto (?P<label>LAB_[0-9a-fA-F]+);
    \}
    else if \(\(event == EVENT_RESOLVE_SPELL\) \|\|
             \(\(event == EVENT_CAN_CHANGE_TARGET \|\| \(event == EVENT_CHANGE_TARGET\)\)\)\)
      goto (?P=label);
    if \(\(event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE\) && \(event != EVENT_QUERY\)\)
    \{
      return 0;
    \}
  \}
(?P=label):
  (?P<result>\w+) = (?P<call>\w+\([^;]+\));
  return (?P=result);
\}''')


def project(before):
    m=PATTERN.search(before);assert m and m.end()==len(before)
    assert len(re.findall(r'\b'+m['label']+r'\b',mask(before)))==3
    new='''  if (event == EVENT_CAN_CAST || event == EVENT_RESOLVE_SPELL ||
      event == EVENT_CAN_CHANGE_TARGET || event == EVENT_CHANGE_TARGET ||
      (event == EVENT_CAST_SPELL && card == affected_card &&
       player == affected_card_controller) ||
      event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE || event == EVENT_QUERY)
  {
    return '''+m['call']+''';
  }
  return 0;
}'''
    after=before[:m.start()]+new;result=m['result'];unused=len(re.findall(r'\b'+result+r'\b',mask(after)))==1
    if unused:
        declaration='  int '+result+';\n';assert after.count(declaration)==1;after=after.replace(declaration,'')
    # Extract the common prefix separately so the dispatch checker does not
    # infer purity of lifelink/deathtouch/death-trigger helpers in that prefix.
    prefix=before[:m.start()];prefix_after=prefix.replace('  int '+result+';\n','') if unused else prefix
    assert after.startswith(prefix_after)
    assert not re.search(r'\bevent\s*(?:=(?!=)|[+*/%&|^-]=|\+\+|--)|&\s*event\b',mask(prefix))
    return after,dict(old_dispatch='int dispatch(int player, int card, event_t event)\n{\n  int '+result+';\n'+before[m.start():],
                     new_dispatch='int dispatch(int player, int card, event_t event)\n{\n'+new,
                     common_helper_call=m['call'],removed_gotos=2,removed_labels=1,removed_dead_result_local=unused,
                     prefix_preserved_except_dead_result_declaration=True)


def trace_proof(facts,event_values):
    """Interpret both parsed statement CFGs over the complete event partition."""
    from pycparser import c_ast,c_generator
    from source_record_flow import inspect
    used=set(re.findall(r'\bEVENT_\w+\b',facts['old_dispatch']+facts['new_dispatch']));assert used<=set(event_values)
    events=sorted({event_values[n] for n in used});assert len(events)==len(used)==8
    other=0
    while other in events:other+=1
    events.append(other);gen=c_generator.CGenerator()
    def run(text,event,card_match,player_match):
        cfg=inspect(text,cfg_only=True);env=dict(event=event,card=4,player=1,affected_card=4 if card_match else 5,affected_card_controller=1 if player_match else 0);calls=[]
        def evaluate(d):
            if isinstance(d,c_ast.ID):return env[d.name] if d.name in env else event_values[d.name]
            if isinstance(d,c_ast.Constant):return int(re.sub('[uUlL]+$','',d.value),0)
            if isinstance(d,c_ast.BinaryOp):
                a=evaluate(d.left)
                if d.op=='&&':return int(bool(a) and bool(evaluate(d.right)))
                if d.op=='||':return int(bool(a) or bool(evaluate(d.right)))
                b=evaluate(d.right)
                if d.op=='==':return int(a==b)
                if d.op=='!=':return int(a!=b)
                raise AssertionError(d.op)
            if isinstance(d,c_ast.FuncCall):
                call=gen.visit(d);calls.append(call);return ['arbitrary_call_result',call]
            raise AssertionError(type(d))
        at=cfg['entry'];seen=set()
        while True:
            assert at not in seen;seen.add(at);node=cfg['nodes'][at];edges=cfg['edges'][at]
            # source_record_flow stores a return's expression in a terminal
            # node; the separate null exit must remain unreachable here.
            if not edges:
                assert node is not None
                result=evaluate(node);return dict(calls=calls,returned=result)
            if len(edges)==2:at=edges[0 if evaluate(node) else 1];continue
            if isinstance(node,c_ast.Assignment):assert node.op=='=' and isinstance(node.lvalue,c_ast.ID);env[node.lvalue.name]=evaluate(node.rvalue)
            elif isinstance(node,c_ast.Decl):assert node.init is None
            else:assert node is None,type(node)
            assert len(edges)==1;at=edges[0]
    rows=[]
    for event in events:
        for card_match in [False,True]:
            for player_match in [False,True]:
                a=run(facts['old_dispatch'],event,card_match,player_match);b=run(facts['new_dispatch'],event,card_match,player_match);assert a==b
                rows.append(dict(event=event,card_match=card_match,player_match=player_match,trace=a))
    return dict(cases=rows,event_classes=len(events),scope='Parsed source CFG interpretation for all eight compared event values plus a distinct other value, and both independent affected-card/player equalities. The shared helper call and arbitrary return value are preserved. Prefix effects remain source-identical and can change the globals before these equalities are observed. Native enum values are independently bound; this is local dispatch equivalence, not complete native behavior or gameplay.')
