"""Inline a private plain-int call result into its sole immediately following return."""
import collections,re
from pycparser import c_ast
from canonicalize_helpers import mask
from source_record_flow import inspect
from call_condition_source import walk


def analyze(text):
    code=mask(text)
    assert re.match(r'(?:static\s+)?int\s+\w+\s*\(',code),'Caller must return plain int'
    assert not re.search(r'\b(?:EXE_FN|asm|__asm__)\b',code),'Unsupported executable/assembly syntax'
    cfg=inspect(text,cfg_only=True);decls=collections.defaultdict(list)
    locals={id(d) for d in walk(cfg['function'].body) if isinstance(d,c_ast.Decl)}
    for d in walk(cfg['function']):
        if isinstance(d,c_ast.Decl):decls[d.name].append(d)
    names=set()
    for name,ds in decls.items():
        if name is None or not re.fullmatch(r'[A-Za-z]*Var\d+|[A-Za-z]*Stack_[a-fA-F0-9]+|result',name) or len(ds)!=1:continue
        d=ds[0]
        if id(d) in locals and not d.storage and not d.quals and d.init is None and isinstance(d.type,c_ast.TypeDecl) and not d.type.quals and isinstance(d.type.type,c_ast.IdentifierType) and d.type.type.names==['int']:names.add(name)
    names-={d.expr.name for d in walk(cfg['function']) if isinstance(d,c_ast.UnaryOp) and d.op=='&' and isinstance(d.expr,c_ast.ID)}
    preds=collections.defaultdict(set)
    for at,nexts in cfg['edges'].items():
        for nxt in nexts:preds[nxt].add(at)
    found=[]
    for at,d in cfg['nodes'].items():
        if not (isinstance(d,c_ast.Assignment) and d.op=='=' and isinstance(d.lvalue,c_ast.ID) and d.lvalue.name in names and isinstance(d.rvalue,c_ast.FuncCall) and isinstance(d.rvalue.name,c_ast.ID) and d.rvalue.name.name not in decls and len(cfg['edges'][at])==1):continue
        ret=cfg['edges'][at][0];node=cfg['nodes'][ret]
        if not (isinstance(node,c_ast.ID) and node.name==d.lvalue.name and not cfg['edges'][ret] and preds[ret]=={at}):continue
        start=cfg['offset'](d);semi=code.find(';',start);ret_at=cfg['offset'](node)
        if semi<start or semi>=ret_at or not re.fullmatch(r'\s*return\s+',code[semi+1:ret_at]):continue
        ret_start=code.rfind('return',semi+1,ret_at);ret_end=code.find(';',ret_at)
        if ret_end<ret_at or code[ret_at+len(node.name):ret_end].strip():continue
        call_start=cfg['offset'](d.rvalue);opening=code.index('(',call_start);end=opening+1;depth=1
        while depth:depth+=(code[end]=='(')-(code[end]==')');end+=1
        if code[end:semi].strip():continue
        if not re.fullmatch(re.escape(node.name)+r'\s*=\s*',text[start:call_start]):continue
        found.append(dict(variable=node.name,callee=d.rvalue.name.name,assignment_offset=start,assignment_end=semi+1,call_start=call_start,call_end=end,return_offset=ret_start,return_end=ret_end+1,assignment_node=at,return_node=ret,sole_return_predecessor=True,private_plain_int=True))
    return sorted(found,key=lambda v:v['assignment_offset'])


def project(before,selected,*,analysis_text=None,remove_only=None):
    view=before if analysis_text is None else analysis_text
    assert len(view)==len(before) and [i for i,c in enumerate(view) if c=='\n']==[i for i,c in enumerate(before) if c=='\n']
    facts={v['assignment_offset']:v for v in analyze(view)};edits=[];variables=set()
    for v in selected:
        assert v==facts[v['assignment_offset']];variables.add(v['variable'])
        start,end=v['assignment_offset'],v['assignment_end'];call=before[v['call_start']:v['call_end']]
        assert not before[before.rfind('\n',0,start)+1:start].strip()
        line=before.rfind('\n',0,v['return_offset'])+1;line_end=before.index('\n',v['return_end'])+1
        assert not before[line:v['return_offset']].strip() and not before[v['return_end']:line_end].strip()
        assert not before[end:before.index('\n',end)].strip()
        edits.extend([(start,end,'return '+call+';'),(line,line_end,'')])
    edits.sort();assert all(a[1]<=b[0] for a,b in zip(edits,edits[1:]))
    after=before
    for start,end,new in reversed(edits):after=after[:start]+new+after[end:]
    removed=[]
    for name in sorted(variables if remove_only is None else variables&set(remove_only)):
        if len(re.findall(r'\b'+re.escape(name)+r'\b',mask(after)))==1:
            declaration='  int '+name+';\n'
            if after.count(declaration)==1:after=after.replace(declaration,'');removed.append(name)
    return after,dict(inlined_assignments=len(selected),removed_local_declarations=removed,source_candidates=selected)

