"""Find and inline a dead, nonescaping integer call-result used by the next if."""
import collections,re
from pycparser import c_ast
from canonicalize_helpers import mask
from source_record_flow import inspect


def walk(d):
    if d is None:return
    yield d
    for _,child in d.children():yield from walk(child)


def analyze(text):
    cfg=inspect(text,cfg_only=True);nodes,edges=cfg['nodes'],cfg['edges'];decls=collections.defaultdict(list)
    for d in walk(cfg['function']):
        if isinstance(d,c_ast.Decl):decls[d.name].append(d)
    names=set()
    automatic_scope={id(d) for d in walk(cfg['function'].body) if isinstance(d,c_ast.Decl)}
    for n,ds in decls.items():
        if n is None or not re.fullmatch(r'[A-Za-z]*Var\d+|[A-Za-z]*Stack_[0-9a-fA-F]+',n) or len(ds)!=1:continue
        d=ds[0]
        if id(d) in automatic_scope and not d.storage and not d.quals and isinstance(d.type,c_ast.TypeDecl) and not d.type.quals and isinstance(d.type.type,c_ast.IdentifierType) and d.type.type.names==['int'] and d.init is None:names.add(n)
    names-={d.expr.name for d in walk(cfg['function']) if isinstance(d,c_ast.UnaryOp) and d.op=='&' and isinstance(d.expr,c_ast.ID)}
    def refs(d):return {v.name for v in walk(d) if isinstance(v,c_ast.ID)}&names
    uses={};defs={};preds=collections.defaultdict(set)
    for at,d in nodes.items():
        if isinstance(d,c_ast.Assignment) and isinstance(d.lvalue,c_ast.ID):uses[at]=refs(d.rvalue)|(refs(d.lvalue) if d.op!='=' else set());defs[at]={d.lvalue.name}&names
        elif isinstance(d,c_ast.Decl):uses[at]=refs(d.init);defs[at]={d.name}&names if d.init is not None else set()
        else:uses[at]=refs(d);defs[at]=set()
        for nxt in edges[at]:preds[nxt].add(at)
    live={at:set() for at in nodes};changed=True
    while changed:
        changed=False
        for at in reversed(nodes):
            out=set().union(*(live[n] for n in edges[at])) if edges[at] else set();new=uses[at]|(out-defs[at])
            if new!=live[at]:live[at]=new;changed=True
    code=mask(text);found=[]
    for at,d in nodes.items():
        if not (isinstance(d,c_ast.Assignment) and d.op=='=' and isinstance(d.lvalue,c_ast.ID) and d.lvalue.name in names and isinstance(d.rvalue,c_ast.FuncCall) and isinstance(d.rvalue.name,c_ast.ID) and len(edges[at])==1):continue
        n=d.lvalue.name;cond=edges[at][0];test=nodes[cond]
        if not (preds[cond]=={at} and len(edges[cond])==2 and isinstance(test,c_ast.BinaryOp) and test.op in ['==','!='] and isinstance(test.left,c_ast.ID) and test.left.name==n and isinstance(test.right,c_ast.Constant) and test.right.value=='0'):continue
        if any(n in live[nxt] for nxt in edges[cond]):continue
        start=cfg['offset'](d);semi=code.find(';',start);test_at=cfg['offset'](test)
        if semi<start or semi>=test_at or not re.fullmatch(r'\s*if\s*\(+\s*',code[semi+1:test_at]):continue
        call_start=cfg['offset'](d.rvalue);opening=code.index('(',call_start);end=opening+1;depth=1
        while depth:depth+=(code[end]=='(')-(code[end]==')');end+=1
        if code[end:semi].strip():continue
        # Function-pointer or block-local declaration binding is a separate case.
        if d.rvalue.name.name in decls:continue
        found.append(dict(variable=n,callee=d.rvalue.name.name,assignment_offset=start,assignment_end=semi+1,call_start=call_start,call_end=end,condition_offset=test_at,condition_operator=test.op,assignment_node=at,condition_node=cond,condition_successors=edges[cond],dead_at_successors=True,sole_condition_predecessor=True))
    return sorted(found,key=lambda v:v['assignment_offset'])


def project(before,selected,*,analysis_text=None):
    view=before if analysis_text is None else analysis_text
    assert len(view)==len(before) and [i for i,c in enumerate(view) if c=='\n']==[i for i,c in enumerate(before) if c=='\n']
    facts=analyze(view);by_offset={v['assignment_offset']:v for v in facts};edits=[];variables=set()
    for v in selected:
        assert v==by_offset[v['assignment_offset']];variables.add(v['variable']);start,end=v['assignment_offset'],v['assignment_end'];call=before[v['call_start']:v['call_end']]
        # Keep the original if and its indentation. Only the dead assignment
        # statement and its condition operand change; the call still runs once.
        line=before.rfind('\n',0,start)+1;statement_end=end
        assert not before[line:start].strip() and not before[end:before.find('\n',end)].strip()
        statement_end=before.index('\n',end)+1
        edits.extend([(line,statement_end,''),(v['condition_offset'],v['condition_offset']+len(v['variable']),call)])
    ordered=sorted(edits);assert all(a[1]<=b[0] for a,b in zip(ordered,ordered[1:]))
    after=before
    for a,b,new in reversed(ordered):after=after[:a]+new+after[b:]
    removed=[]
    for name in sorted(variables):
        code=mask(after)
        if len(re.findall(r'\b'+re.escape(name)+r'\b',code))==1:
            old='  int '+name+';\n'
            if after.count(old)==1:after=after.replace(old,'');removed.append(name)
    return after,dict(inlined_assignments=len(selected),removed_local_declarations=removed,source_candidates=selected,
        scope='Unique nonvolatile uninitialized int placeholder declarations, no direct address escape, a sole adjacent condition predecessor and no assigned-value use reachable from either condition successor. The helper runs exactly once at the same control-flow admission; precise expanded-code/type and line-sensitive-macro checks are recorded separately.')
