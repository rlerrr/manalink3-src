"""Bounded liveness checks for removed loop temporaries and local copies."""
import re
from pycparser import c_ast
from canonicalize_helpers import mask
from source_record_flow import inspect


def prove(before,after,changes,event_values):
    cfg=inspect(before,cfg_only=True);nodes,edges=cfg['nodes'],cfg['edges'];offset=cfg['offset'];tracked={'iVar7','iVar6','iVar12','cVar13','bVar16','base_target','uStack_100'}
    def read(d):
        if d is None:return set()
        if isinstance(d,c_ast.ID):return {d.name}&tracked
        return set().union(*(read(c) for _,c in d.children())) if d.children() else set()
    def use_def(d):
        if isinstance(d,c_ast.Assignment) and isinstance(d.lvalue,c_ast.ID):return read(d.rvalue)|(read(d.lvalue) if d.op!='=' else set()),{d.lvalue.name}&tracked
        if isinstance(d,c_ast.Decl):return read(d.init),({d.name}&tracked if d.init is not None else set())
        return read(d),set()
    uses={};defs={}
    for at,d in nodes.items():uses[at],defs[at]=use_def(d)
    def truth(d):
        event=event_values['EVENT_CAN_CAST']
        def value(x):
            if isinstance(x,c_ast.ID):return event if x.name=='event' else event_values.get(x.name)
            if isinstance(x,c_ast.BinaryOp):
                a,b=value(x.left),value(x.right)
                if x.op=='&&':return 0 if a==0 or b==0 else 1 if a is not None and b is not None else None
                if x.op=='||':return 1 if a not in [0,None] or b not in [0,None] else 0 if a==b==0 else None
                if a is not None and b is not None:
                    if x.op=='==':return int(a==b)
                    if x.op=='!=':return int(a!=b)
            return None
        return value(d)
    pruned={at:[bs[0 if truth(nodes[at]) else 1]] if len(bs)==2 and truth(nodes[at]) is not None else bs for at,bs in edges.items()}
    def liveness(graph):
        live={at:set() for at in nodes};changed=True
        while changed:
            changed=False
            for at in reversed(nodes):
                out=set().union(*(live[n] for n in graph[at])) if graph[at] else set();new=uses[at]|(out-defs[at])
                if new!=live[at]:live[at]=new;changed=True
        return live
    live=liveness(pruned)
    checks=[]
    for change,names in [(changes[0],{'iVar7','iVar6','iVar12'}),(changes[1],{'iVar7','iVar6','iVar12','cVar13','bVar16'})]:
        old=change['before'];assert before.count(old)==1;lo=before.index(old);hi=lo+len(old)
        owned={at for at,d in nodes.items() if d is not None and d.coord is not None and lo<=offset(d)<hi};exits={n for at in owned for n in pruned[at] if n not in owned};observed=set().union(*(live[n] for n in exits));assert not names&observed,(names,observed)
        checks.append(dict(start=lo,end=hi,discarded_outer_temporaries=sorted(names),live_at_external_successors=sorted(observed),external_successors=sorted(exits)))
    # Snapshotting target zero no longer needs to overwrite base_target or
    # the targeting-mode word; neither is used after that dialog compound.
    marker='        if (t_player == 2)\n        {\n          target_definition_t td9;';assert before.count(marker)==1;lo=before.index(marker);code=mask(before);start=code.index('{',lo);end=start+1;depth=1
    while depth:depth+=(code[end]=='{')-(code[end]=='}');end+=1
    owned={at for at,d in nodes.items() if d is not None and d.coord is not None and lo<=offset(d)<end};exits={n for at in owned for n in edges[at] if n not in owned};all_live=liveness(edges);observed=set().union(*(all_live[n] for n in exits));assert not {'base_target','uStack_100'}&observed
    checks.append(dict(start=lo,end=end,discarded_outer_temporaries=['base_target','uStack_100'],live_at_external_successors=sorted(observed)))
    copy='      pcStack_f4.number = selected_targets;';lo=before.index(copy)+len(copy);hi=before.index('      pcStack_fc.number = 0x0;',lo);between=mask(before[lo:hi]);assert not re.search(r'\bselected_targets\s*(?:=(?!=)|[+*/%&|^-]=|\+\+|--)|&\s*selected_targets\b',between)
    for d in nodes.values():
        def walk(x):
            if x is None:return
            yield x
            for _,c in x.children():yield from walk(c)
        for x in walk(d):assert not (isinstance(x,c_ast.UnaryOp) and x.op=='&' and isinstance(x.expr,c_ast.ID) and x.expr.name in {'pcVar5','event','selected_targets'}),'Address-escaping local invalidates copy/liveness proof'
    assert len(re.findall(r'\binstance\s*=(?!=)',mask(after)))==1
    return dict(loop_exit_liveness=checks,selected_target_counter_unchanged_during_cleanup=True,main_instance_pointer_assigned_once=True,
                scope='Event-specialized syntactic CFG liveness shows discarded aura/grave-search outer temporaries are dead at every external successor. The target-zero dialog has no later base_target/targeting-word dependency. Copy elimination checks a stable, nonescaping counter, and the main instance/event/counter locals have no address escape. Call side effects on pointed-to card objects are retained; arbitrary undefined behavior and full function equivalence are not inferred.')


def loop_predicate_proofs():
    from itertools import product
    aura=grave=0
    for present,permanent,valid,humiliated,csvid in product([False,True],[False,True],[False,True],[False,True],[0,0x32b,0x242]):
        old=['in_play'];reject=False
        if present:
            old.append('is_what')
            if permanent:
                old.append('would_validate_arbitrary_target')
                if valid:
                    old.append('is_humiliated')
                    if not humiliated:old.append('get_id');reject=csvid in [0x32b,0x242]
        new=['in_play'];new_reject=False
        for condition,callee in [(present,'is_what'),(permanent,'would_validate_arbitrary_target'),(valid,'is_humiliated'),(not humiliated,'get_id')]:
            if not condition:break
            new.append(callee)
        if len(new)==5:new_reject=csvid==0x32b or csvid==0x242
        assert (old,reject)==(new,new_reject);aura+=1
    for player,opponent,both,c0,c1,s0,s1 in product([-1,0,1,2,3],[False,True],[False,True],[-1,0,1],[-1,0,1],[0,1],[0,1]):
        original_player=1-player if opponent else player;selected=2 if both else original_player;either=both or original_player==2;counts=[c0,c1];shrouds=[s0,s1];old=[];found=False
        for p in range(2):
            if selected==p or either:
                old.append(('count',p))
                if counts[p]>0:
                    old.append(('shroud',p))
                    if shrouds[p]==0:found=True;break
        selected=player
        if opponent:selected=1-player
        if both:selected=2
        new=[];new_found=False
        for p in range(2):
            if p==selected or selected==2:
                new.append(('count',p))
                if counts[p]>0:
                    new.append(('shroud',p))
                    if shrouds[p]==0:new_found=True;break
        assert (old,found)==(new,new_found);grave+=1
    return dict(aura_predicate_classes=aura,graveyard_predicate_classes=grave,
                loop_invariant='Both aura loops start p/c at zero, advance by one and reload active_cards_count[p] at each inner-loop admission. The same ordered helper calls can therefore change the next bound in the same way. No bound snapshot or callee purity assumption is introduced.',
                scope='Complete zero/nonzero/null and handled-ID predicate classes for one aura iteration, and grave-search player/opponent/both/count-sign/shroud classes. Player representatives cover0/1/2/other and the1-player mapping; undefined signed overflow is outside the C model. These are local relational checks, not gameplay fixtures or whole-function equivalence.')
