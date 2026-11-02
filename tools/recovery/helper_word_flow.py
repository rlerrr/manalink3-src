"""Check byte-lane reaching definitions of the three generic-spell word unions."""
import collections,re
from pycparser import c_ast
from source_record_flow import inspect

WORDS={'pcStack_fc','pcStack_f8','pcStack_f4'}
MEMBERS={'number','bits','instance','low'}


def analyze(text,event_values):
    cfg=inspect(text,cfg_only=True);nodes,edges=cfg['nodes'],cfg['edges'];offset=cfg['offset'];writes={};reads={}
    def word(d):return (d.name.name,d.field.name) if isinstance(d,c_ast.StructRef) and isinstance(d.name,c_ast.ID) and d.name.name in WORDS else None
    def references(d):
        if d is None:return
        found=word(d)
        if found:
            assert found[1] in MEMBERS;yield dict(word=found[0],member=found[1],offset=offset(d));return
        for _,c in d.children():yield from references(c)
    def descendants(d):
        if d is None:return
        yield d
        for _,c in d.children():yield from descendants(c)
    for at,d in nodes.items():
        for x in descendants(d):
            assert not (isinstance(x,c_ast.Assignment) and isinstance(x.lvalue,c_ast.ID) and x.lvalue.name=='event')
            assert not (isinstance(x,c_ast.UnaryOp) and x.op in ['p++','p--','++','--'] and isinstance(x.expr,c_ast.ID) and x.expr.name=='event')
            assert not (isinstance(x,c_ast.UnaryOp) and x.op=='&' and (word(x.expr) or isinstance(x.expr,c_ast.ID) and x.expr.name=='event'))
        if isinstance(d,c_ast.Assignment) and word(d.lvalue):
            assert d.op=='=';w,m=word(d.lvalue);writes[at]=dict(word=w,member=m,offset=offset(d.lvalue));reads[at]=list(references(d.rvalue))
        else:reads[at]=list(references(d))
    def value(d,event):
        if isinstance(d,c_ast.ID):return event if d.name=='event' else event_values.get(d.name,1 if d.name=='true' else 0 if d.name=='false' else None)
        if isinstance(d,c_ast.Constant) and d.type in ['int','unsigned int','long int','unsigned long int']:
            try:return int(re.sub('[uUlL]+$','',d.value),0)
            except ValueError:return None
        if isinstance(d,c_ast.UnaryOp) and d.op=='!':
            v=value(d.expr,event);return int(not v) if v is not None else None
        if isinstance(d,c_ast.BinaryOp):
            a,b=value(d.left,event),value(d.right,event)
            if d.op=='&&':return 0 if a==0 or b==0 else 1 if a is not None and b is not None else None
            if d.op=='||':return 1 if a not in [0,None] or b not in [0,None] else 0 if a==b==0 else None
            if a is not None and b is not None:
                if d.op=='==':return int(a==b)
                if d.op=='!=':return int(a!=b)
        return None
    used_events=set(re.findall(r'\bevent\s*[!=]=\s*(EVENT_\w+)',text));assert used_events<=set(event_values);values=sorted({event_values[n] for n in used_events});other=0
    while other in values:other+=1
    values.append(other);cases=[];observed=collections.defaultdict(set);bad=[]
    for event in values:
        initial={w:frozenset({(0,0,0,0)}) for w in WORDS};states={cfg['entry']:initial};todo=collections.deque([cfg['entry']]);updates=0
        while todo:
            at=todo.popleft();state=dict(states[at])
            if at in writes:
                w=writes[at];old=state[w['word']];state[w['word']]=frozenset((at,*lanes[1:]) for lanes in old) if w['member']=='low' else frozenset({(at,at,at,at)})
            nexts=edges[at]
            if len(nexts)==2:
                test=value(nodes[at],event)
                if test is not None:nexts=[nexts[0 if test else 1]]
            else:assert len(nexts)<=1,'Switch requires an explicit event-dispatch model'
            for nxt in nexts:
                old=states.get(nxt);merged=state if old is None else {w:old[w]|state[w] for w in WORDS}
                if old!=merged:states[nxt]=dict(merged);todo.append(nxt);updates+=1;assert updates<100000
        checked=0
        for at,refs in reads.items():
            if at not in states:continue
            for ref in refs:
                for lanes in states[at][ref['word']]:
                    needed=lanes[:1] if ref['member']=='low' else lanes
                    if 0 in needed or len(set(needed))!=1:
                        bad.append(dict(event=event,reader=ref,writer_nodes=list(lanes),reason='unknown_or_partially_rewritten_full_word'));continue
                    origin=needed[0];producer=writes[origin];observed[ref['offset'],ref['word'],ref['member']].add(origin)
                    if ref['member']=='instance' and producer['member']!='instance':bad.append(dict(event=event,reader=ref,writer_nodes=list(lanes),reason='pointer_from_other_member'))
                    if ref['member'] in ['number','bits'] and producer['member']=='instance':bad.append(dict(event=event,reader=ref,writer_nodes=list(lanes),reason='integer_from_pointer'))
                    if ref['member']=='low' and producer['member']=='instance':bad.append(dict(event=event,reader=ref,writer_nodes=list(lanes),reason='byte_from_pointer'))
                    checked+=1
        cases.append(dict(event=event,reachable_nodes=len(states),checked_reaching_reads=checked))
    return dict(event_classes=cases,writes=[dict(v,node=k) for k,v in writes.items()],reads=[dict(offset=o,word=w,member=m,producer_nodes=sorted(s)) for (o,w,m),s in sorted(observed.items())],unresolved=bad,
                scope='Native event-value specialization of the exact source statement CFG; every remaining branch successor is retained. Each full-word read must have one complete, nonzero reaching write for all four byte lanes. Byte reads must have a definite low-byte producer, and pointer/integer/byte cross-interpretation is rejected. This checks local reuse/partial-write dependencies, not flags satisfiability, complete call effects or whole-function equivalence.')
