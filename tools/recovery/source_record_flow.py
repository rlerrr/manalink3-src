"""Conservative statement CFG and reaching constructor sites for source records."""
import collections,re
from pycparser import c_ast,c_parser
from canonicalize_helpers import mask
from promote_recovery import LITERAL
from disentangle_union_source import CONSTRUCTORS,PATTERN
UNION_PATTERN=re.compile(r'  (?:/\* DWARF locals share this frame offset; keep every typed view\. \*/\n  )?union\n  \{\n(?P<members>.*?)\n  \} (?P<storage>recovered_stack_\w+);\n',re.S)


def parse(text):
    typedefs=set(re.findall(r'\b[A-Za-z_]\w*_t\b',text))|{'bool'}
    prefix=''.join('typedef int '+t+';\n' for t in sorted(typedefs))
    clean=re.sub(LITERAL+r'|/\*.*?\*/|//[^\n]*',lambda m:re.sub(r'[^\n]',' ',m[0]) if m[0].startswith(('/*','//')) else m[0],text,flags=re.S)
    macros=re.findall(r'\bEXE_FN\([^)]*\)',clean);assert all('&' not in m for m in macros)
    clean=re.sub(r'\bEXE_FN\([^)]*\)','exe_call_stub',clean)
    tree=c_parser.CParser().parse(prefix+clean);functions=[v for v in tree.ext if isinstance(v,c_ast.FuncDef)];assert len(functions)==1
    lines=[0]+[m.end() for m in re.finditer('\n',text)]
    def offset(node):return lines[node.coord.line-prefix.count('\n')-1]+node.coord.column-1
    return functions[0],offset


def inspect(text,*,cfg_only=False):
    union=UNION_PATTERN.search(text)
    if not cfg_only:assert union
    storage=union['storage'] if union is not None else None;fn,offset=parse(text);nodes={};edges={};labels={};gotos=[];serial=0
    def node(expr,nexts=()):
        nonlocal serial
        serial+=1;nodes[serial]=expr;edges[serial]=list(nexts);return serial
    def build(stmt,nxt,brk=None,cont=None,switch_cases=None):
        if stmt is None:return nxt
        if isinstance(stmt,c_ast.Compound):
            entry=nxt
            for s in reversed(stmt.block_items or []):entry=build(s,entry,brk,cont,switch_cases)
            return entry
        if isinstance(stmt,c_ast.If):return node(stmt.cond,[build(stmt.iftrue,nxt,brk,cont,switch_cases),build(stmt.iffalse,nxt,brk,cont,switch_cases)])
        if isinstance(stmt,c_ast.While):
            at=node(stmt.cond);body=build(stmt.stmt,at,nxt,at,switch_cases);edges[at]=[body,nxt];return at
        if isinstance(stmt,c_ast.DoWhile):
            at=node(stmt.cond);body=build(stmt.stmt,at,nxt,at,switch_cases);edges[at]=[body,nxt];return body
        if isinstance(stmt,c_ast.For):
            at=node(stmt.cond);step=node(stmt.next,[at]) if stmt.next else at;body=build(stmt.stmt,step,nxt,step,switch_cases);edges[at]=[body,nxt] if stmt.cond else [body];return node(stmt.init,[at]) if stmt.init else at
        if isinstance(stmt,c_ast.Switch):
            cases=[];build(stmt.stmt,nxt,nxt,cont,cases);at=node(stmt.cond,cases)
            if not any(isinstance(nodes[c],c_ast.Default) for c in cases):edges[at].append(nxt)
            return at
        if isinstance(stmt,(c_ast.Case,c_ast.Default)):
            entry=nxt
            for s in reversed(stmt.stmts or []):entry=build(s,entry,brk,cont,switch_cases)
            at=node(stmt,[entry]);assert switch_cases is not None;switch_cases.append(at);return at
        if isinstance(stmt,c_ast.Label):
            at=node(None,[build(stmt.stmt,nxt,brk,cont,switch_cases)]);assert stmt.name not in labels;labels[stmt.name]=at;return at
        if isinstance(stmt,c_ast.Goto):at=node(None);gotos.append((at,stmt.name));return at
        if isinstance(stmt,c_ast.Break):assert brk is not None;return node(None,[brk])
        if isinstance(stmt,c_ast.Continue):assert cont is not None;return node(None,[cont])
        if isinstance(stmt,c_ast.Return):return node(stmt.expr)
        return node(stmt,[nxt])
    end=node(None);entry=build(fn.body,end)
    for at,label in gotos:edges[at]=[labels[label]]
    if cfg_only:return dict(nodes=nodes,edges=edges,entry=entry,function=fn,offset=offset)
    def walk(d):
        if d is None:return
        yield d
        # Cases' children are separate CFG nodes, not extra executions here.
        if isinstance(d,(c_ast.Case,c_ast.Default)):return
        for _,child in d.children():yield from walk(child)
    refs={};ctors={};reverse={callee:role for role,callees in CONSTRUCTORS.items() for callee in callees}
    for at,expr in nodes.items():
        references=[]
        for d in walk(expr):
            if isinstance(d,c_ast.StructRef) and isinstance(d.name,c_ast.ID) and d.name.name==storage:
                assert d.type=='.';references.append(dict(offset=offset(d),member=d.field.name))
            if isinstance(d,c_ast.FuncCall) and isinstance(d.name,c_ast.ID) and d.name.name in reverse:
                owned=[a for a in walk(d.args) if isinstance(a,c_ast.StructRef) and isinstance(a.name,c_ast.ID) and a.name.name==storage]
                if owned:
                    assert d is expr and len(owned)==1;role=reverse[d.name.name];assert owned[0].field.name==role
                    ctors[at]=dict(role=role,offset=offset(d),callee=d.name.name)
        refs[at]=references
    initial={r:frozenset({0}) for r in ['target','test','token','active']};states={entry:initial};todo=collections.deque([entry]);updates=0
    while todo:
        at=todo.popleft();state=dict(states[at])
        if at in ctors:state[ctors[at]['role']]=state['active']=frozenset({at})
        for nxt in edges[at]:
            old=states.get(nxt);merged=state if old is None else {r:state[r]|old[r] for r in initial}
            if old!=merged:states[nxt]=dict(merged);todo.append(nxt);updates+=1;assert updates<100000
    parent={at:at for at in ctors}
    def root(a):
        while parent[a]!=a:a=parent[a]
        return a
    def join(sites):
        first=root(next(iter(sites)))
        for at in sites:parent[root(at)]=first
    accesses=[];unresolved=[];code=mask(text)
    for at,items in refs.items():
        if not items:continue
        assert at in states,('unreachable record reference',at)
        state=dict(states[at])
        if at in ctors:state[ctors[at]['role']]=state['active']=frozenset({at})
        for ref in items:
            start=ref['offset'];m=re.match(re.escape(storage)+r'\.(target|test|token)(?:\.([A-Za-z_]\w*))?',code[start:]);assert m,(start,code[start:start+50]);member,field=m.groups();sites=state[member];active=state['active']
            # Cross-view scalar stores are accepted only with a unique reaching
            # record kind. Bare constructor/consumer addresses retain API role.
            if field and 0 not in active and len({ctors[s]['role'] for s in active})==1 and next(iter(active)) in ctors:
                actual=ctors[next(iter(active))]['role']
                if actual!=member:sites=active
            known=set(sites)-{0}
            if not known:unresolved.append(dict(ref,node=at,reason='no_reaching_constructor',field=field));continue
            assert len({ctors[s]['role'] for s in known})==1
            join(known);accesses.append(dict(ref,node=at,field=field,constructor_sites=sorted(known),possibly_uninitialized=0 in sites))
    groups=collections.defaultdict(list)
    for at,c in ctors.items():groups[root(at)].append(dict(c,node=at))
    for a in accesses:a['group']=root(a['constructor_sites'][0])
    return dict(storage=storage,constructors=[dict(c,node=at) for at,c in sorted(ctors.items(),key=lambda kv:kv[1]['offset'])],groups=[dict(id=k,constructors=sorted(v,key=lambda x:x['offset'])) for k,v in groups.items()],accesses=accesses,unresolved=unresolved,
                scope='Statement CFG with all syntactic branch successors; no satisfiability or call-purity assumption. Zero constructor state marks possible uninitialized/infeasible paths. Same-kind sites reaching a common reference are merged. This is an ownership discovery aid, not a C semantic equivalence proof.')
