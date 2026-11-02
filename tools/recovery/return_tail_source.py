"""Replace jumps to a bare return with that return, preserving expression scope."""
import collections,json,re
from pycparser import c_ast,c_generator
from canonicalize_helpers import mask,sha
from call_condition_source import walk
from source_record_flow import parse,inspect
from target_loop_source import epsilon

GEN=c_generator.CGenerator()


def analyze(text):
    code=mask(text)
    assert not re.search(r'\b(?:EXE_FN|asm|__asm__|__attribute__|__builtin_frame_address|__builtin_return_address|setjmp|longjmp)\b',code)
    fn,offset=parse(text);nodes=list(walk(fn));decls=collections.defaultdict(list)
    for n in nodes:
        if isinstance(n,c_ast.Decl):decls[n.name].append(n)
    outer={id(n) for n in fn.body.block_items or [] if isinstance(n,c_ast.Decl)}
    if fn.decl.type.args:outer.update(id(n) for n in fn.decl.type.args.params)
    returns={}
    for label in nodes:
        if not isinstance(label,c_ast.Label) or not re.fullmatch(r'LAB_[a-fA-F0-9]+',label.name) or not isinstance(label.stmt,c_ast.Return):continue
        ret=label.stmt;expr=ret.expr
        # Duplicating a string/compound literal can create a different object.
        if any(isinstance(n,(c_ast.CompoundLiteral,c_ast.InitList)) or isinstance(n,c_ast.Constant) and n.type=='string' for n in walk(expr)):continue
        ids={n.name for n in walk(expr) if isinstance(n,c_ast.ID)}
        if any(name.startswith('__') for name in ids):continue
        types={name for n in walk(expr) if isinstance(n,c_ast.IdentifierType) for name in n.names}
        tags={n.name for n in walk(expr) if isinstance(n,(c_ast.Struct,c_ast.Union,c_ast.Enum)) and n.name}
        if any(isinstance(n,c_ast.Typedef) and n.name in types for n in nodes):continue
        if any(isinstance(n,(c_ast.Struct,c_ast.Union)) and n.name in tags and n.decls is not None or isinstance(n,c_ast.Enum) and n.name in tags and n.values is not None for n in nodes):continue
        # Every declared identifier in the expression has one function-wide
        # binding. Nested declarations cannot capture it at a former jump site.
        if any(name in decls and (len(decls[name])!=1 or id(decls[name][0]) not in outer) for name in ids):continue
        start=offset(ret);end=code.find(';',start)+1
        assert end>start and re.fullmatch(r'return\b[\s\S]*;',code[start:end])
        original=text[start:end]
        if re.search(r'/\*|//',original):continue
        returns[label.name]=dict(return_start=start,return_end=end,return_statement=original,return_expression=GEN.visit(expr) if expr is not None else None,expression_identifiers=sorted(ids))
    found=[]
    for n in nodes:
        if not isinstance(n,c_ast.Goto) or n.name not in returns:continue
        start=offset(n);end=code.find(';',start)+1
        if not re.fullmatch(r'goto\s+'+re.escape(n.name)+r'\s*;',text[start:end]):continue
        found.append(dict(label=n.name,goto_start=start,goto_end=end,**returns[n.name]))
    return sorted(found,key=lambda c:c['goto_start'])


def cfg_proof(before,after):
    """Compare every reachable action and ordered successor after silent jumps."""
    old=inspect(before,cfg_only=True);new=inspect(after,cfg_only=True)
    def observe(cfg,at):
        at=epsilon(cfg,at);n=cfg['nodes'][at];edges=cfg['edges'][at]
        if isinstance(n,c_ast.Case):tag=('case',GEN.visit(n.expr))
        elif isinstance(n,c_ast.Default):tag=('default','')
        elif n is None:tag=('fallthrough','')
        else:tag=('condition' if len(edges)==2 else 'expression',GEN.visit(n))
        return at,tag,edges
    todo=[(old['entry'],new['entry'])];pairs=set();receipts=[]
    while todo:
        a,b=todo.pop();a,sa,ea=observe(old,a);b,sb,eb=observe(new,b)
        if (a,b) in pairs:continue
        assert sa==sb and len(ea)==len(eb),(sa,sb,len(ea),len(eb))
        pairs.add((a,b));receipts.append((a,b,sa,len(ea)));todo.extend(zip(ea,eb))
    return dict(state_pairs=len(pairs),graph_pair_sha256=sha(json.dumps(sorted(receipts),separators=(',',':')).encode()),scope='Reachable whole-function statement CFG bisimulation; only label/jump nodes are silent. Every action, condition, call, return expression and ordered branch is preserved. Expression identifiers retain their unique outer binding; string and compound literal objects are excluded.')


def project(before,selected=None):
    found=analyze(before)
    if selected is None:selected=found
    assert selected and all(c in found for c in selected)
    edits=[(c['goto_start'],c['goto_end'],c['return_statement']) for c in selected]
    assert all(a[1]<=b[0] for a,b in zip(edits,edits[1:]));after=before
    for a,b,s in reversed(edits):after=after[:a]+s+after[b:]
    removed=[]
    for label in sorted({c['label'] for c in selected}):
        code=mask(after)
        if re.search(r'\bgoto\s+'+re.escape(label)+r'\s*;',code):continue
        line=re.search(r'(?m)^[ \t]*'+re.escape(label)+r':[ \t]*\n',code)
        if line and re.fullmatch(r'[ \t]*'+re.escape(label)+r':[ \t]*\n',after[line.start():line.end()]):a,b=line.span()
        else:
            hits=list(re.finditer(r'\b'+re.escape(label)+r'\s*:',code));assert len(hits)==1;a,b=hits[0].span()
        after=after[:a]+after[b:];removed.append(label)
    return after,dict(removed_gotos=len(selected),removed_labels=removed,source_candidates=selected,cfg_bisimulation=cfg_proof(before,after))
