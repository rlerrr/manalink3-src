"""Structure bounded retry loops without moving their effects or valid payloads."""
import bisect,collections,json,re
from pycparser import c_ast,c_generator
from canonicalize_helpers import mask,sha
from source_record_flow import parse,inspect
from call_condition_source import walk

GEN=c_generator.CGenerator()

def epsilon(cfg,at):
    seen=set()
    while (cfg['nodes'][at] is None or isinstance(cfg['nodes'][at],c_ast.EmptyStatement)) and len(cfg['edges'][at])==1:
        assert at not in seen,'Silent control cycle';seen.add(at);at=cfg['edges'][at][0]
    return at

def condition_node(cfg,offset):
    hits=[at for at,n in cfg['nodes'].items() if n is not None and cfg['offset'](n)==offset and len(cfg['edges'][at])==2]
    assert len(hits)==1;return hits[0]

def analyze(text):
    fn,offset=parse(text);code=mask(text);assert not re.search(r'\b(?:EXE_FN|asm|__asm__|__builtin_frame_address|__builtin_return_address)\b',code)
    nodes=list(walk(fn));decls=collections.defaultdict(list);found=[]
    for n in nodes:
        if isinstance(n,c_ast.Decl):decls[n.name].append(n)
    def end(n):
        if isinstance(n,c_ast.If):return end(n.iffalse if n.iffalse is not None else n.iftrue)
        if isinstance(n,c_ast.Compound):
            start=offset(n);brace=code.index('{',start);assert not code[start:brace].strip();at=brace+1;depth=1
            while depth:depth+=(code[at]=='{')-(code[at]=='}');at+=1
            return at
        at=offset(n);parens=0
        while True:
            c=code[at]
            if c==';' and not parens:return at+1
            parens+=(c=='(')-(c==')');at+=1
    for loop in nodes:
        if not isinstance(loop,c_ast.DoWhile) or not isinstance(loop.stmt,c_ast.Compound):continue
        body=loop.stmt.block_items or []
        if len(body)<2 or not isinstance(body[0],c_ast.While) or not isinstance(body[0].stmt,c_ast.Compound):continue
        retry=body[0];inside=retry.stmt.block_items or []
        if len(inside)!=2:continue
        inc,guard=inside
        if not isinstance(guard,c_ast.If) or guard.iffalse is not None:continue
        branch=guard.iftrue.block_items or [] if isinstance(guard.iftrue,c_ast.Compound) else [guard.iftrue]
        if len(branch)!=1 or not isinstance(branch[0],c_ast.Goto):continue
        if not isinstance(loop.cond,c_ast.BinaryOp) or loop.cond.op!='<' or not isinstance(loop.cond.left,c_ast.ID):continue
        counter=loop.cond.left.name;ds=decls[counter]
        if len(ds)!=1:continue
        d=ds[0]
        if d not in (fn.body.block_items or []) or d.storage or d.quals or not isinstance(d.type,c_ast.TypeDecl) or d.type.quals or not isinstance(d.type.type,c_ast.IdentifierType) or d.type.type.names!=['int']:continue
        if any(isinstance(n,c_ast.UnaryOp) and n.op=='&' and isinstance(n.expr,c_ast.ID) and n.expr.name==counter for n in nodes):continue
        if GEN.visit(inc) not in [counter+' = '+counter+' + 1',counter+' += 1','++'+counter,counter+'++']:continue
        if any(isinstance(n,c_ast.Decl) for n in body[1:]):continue
        bound=GEN.visit(loop.cond.right);g=guard.cond
        if any(isinstance(n,(c_ast.FuncCall,c_ast.Assignment)) or isinstance(n,c_ast.UnaryOp) and n.op in ['++','--','p++','p--'] for n in walk(loop.cond.right)):continue
        if not isinstance(g,c_ast.BinaryOp):continue
        if not ((g.op=='<=' and GEN.visit(g.left)==bound and GEN.visit(g.right)==counter) or (g.op=='>=' and GEN.visit(g.left)==counter and GEN.visit(g.right)==bound)):continue
        found.append(dict(counter=counter,bound=bound,label=branch[0].name,outer_condition_offset=offset(loop.cond),exit_guard_offset=offset(g),loop_offset=offset(loop),retry_offset=offset(retry),valid_statements=len(body)-1,guard_start=offset(guard),guard_end=end(guard),retry_body_start=offset(retry.stmt),outer_condition=GEN.visit(loop.cond)))
    if not found:return []
    cfg=inspect(text,cfg_only=True);accepted=[]
    for c in found:
        outer=condition_node(cfg,c['outer_condition_offset']);guard=condition_node(cfg,c['exit_guard_offset'])
        if epsilon(cfg,cfg['edges'][outer][1])==epsilon(cfg,cfg['edges'][guard][0]):accepted.append(c)
    return sorted(accepted,key=lambda v:v['retry_offset'])

def cfg_proof(before,after,selected):
    """Whole-function action/branch bisimulation, with integer bound complements.

    Only the admitted exit guards are rewritten in the old graph. Their
    branches are swapped and their condition becomes the exact outer bound.
    This requires integer operands, checked by actual GCC probes separately.
    Label/goto/continue nodes are silent; all other expressions stay ordered.
    """
    assert selected and all(v in analyze(before) for v in selected)
    old=inspect(before,cfg_only=True);new=inspect(after,cfg_only=True);rewrites={}
    for c in selected:
        guard=condition_node(old,c['exit_guard_offset']);rewrites[guard]=c['outer_condition']
    def observe(cfg,at,rewrite):
        at=epsilon(cfg,at);n=cfg['nodes'][at];edges=cfg['edges'][at]
        if at in rewrite:return at,('condition',rewrite[at]),list(reversed(edges))
        if isinstance(n,c_ast.Case):tag=('case',GEN.visit(n.expr))
        elif isinstance(n,c_ast.Default):tag=('default','')
        elif n is None:tag=('fallthrough','')
        else:tag=('condition' if len(edges)==2 else 'expression',GEN.visit(n))
        return at,tag,edges
    todo=[(old['entry'],new['entry'])];pairs=set();old_nodes=set();new_nodes=set();receipts=[]
    while todo:
        a,b=todo.pop();a,sa,ea=observe(old,a,rewrites);b,sb,eb=observe(new,b,{})
        if (a,b) in pairs:continue
        assert sa==sb and len(ea)==len(eb),(sa,sb,len(ea),len(eb))
        pairs.add((a,b));old_nodes.add(a);new_nodes.add(b);receipts.append((a,b,sa,len(ea)));todo.extend(zip(ea,eb))
    assert set(rewrites)<=old_nodes
    return dict(state_pairs=len(pairs),old_observable_nodes=len(old_nodes),new_observable_nodes=len(new_nodes),integer_bound_complements=len(rewrites),graph_pair_sha256=sha(json.dumps(sorted(receipts),separators=(',',':')).encode()),scope='Whole parsed statement CFG bisimulation after exact integer exit-guard complementation and silent label/jump removal. All other expressions, calls, bound reads, branches and returns retain their order; no call-purity or fixed-bound assumption. Actual integer operand types are checked separately.')

def project(before,selected):
    found=analyze(before);assert selected and all(v in found for v in selected);edits=[]
    for c in selected:
        start=c['retry_offset'];assert before[start:start+5]=='while';edits.extend([(start,start+5,'if'),(c['guard_start'],c['guard_end'],'continue;')])
    edits.sort();assert all(a[1]<=b[0] for a,b in zip(edits,edits[1:]))
    after=before
    for a,b,text in reversed(edits):after=after[:a]+text+after[b:]
    removed=[]
    for label in sorted({v['label'] for v in selected}):
        code=mask(after)
        if re.search(r'\bgoto\s+'+re.escape(label)+r'\s*;',code):continue
        line=re.search(r'(?m)^[ \t]*'+re.escape(label)+r':[ \t]*\n',code)
        if line and re.fullmatch(r'[ \t]*'+re.escape(label)+r':[ \t]*\n',after[line.start():line.end()]):a,b=line.span()
        else:
            hits=list(re.finditer(r'\b'+re.escape(label)+r'\s*:',code));assert len(hits)==1;a,b=hits[0].span()
        after=after[:a]+after[b:];removed.append(label)
    return after,dict(structured_loops=len(selected),removed_gotos=len(selected),removed_labels=removed,source_candidates=selected,cfg_bisimulation=cfg_proof(before,after,selected))

def probe_definition(before,selected):
    assert all(v in analyze(before) for v in selected);edits=[]
    for i,c in enumerate(selected):
        at=c['retry_body_start'];code=mask(before);brace=code.index('{',at);assert not code[at:brace].strip();at=brace
        assertion='_Static_assert(__builtin_classify_type(('+c['bound']+')) == 1 && __builtin_classify_type(('+c['counter']+')) == 1, "target_loop_integer_'+str(i)+'");'
        edits.append((at+1,' '+assertion+' '))
    for at,text in sorted(edits,reverse=True):before=before[:at]+text+before[at:]
    return before

def probe_unit(expanded,rows):
    expanded=expanded.replace('\r\n','\n');lines=expanded.splitlines(keepends=True);clean_lines=['\n' if line.startswith('#') else line for line in lines];clean=''.join(clean_lines);raw_offsets=[0];clean_offsets=[0]
    for original,stripped in zip(lines,clean_lines):raw_offsets.append(raw_offsets[-1]+len(original));clean_offsets.append(clean_offsets[-1]+len(stripped))
    def raw_at(at):
        line=bisect.bisect_right(clean_offsets,at)-1;return raw_offsets[line]+at-clean_offsets[line]
    edits=[]
    for row in rows:
        before=row['expanded_before_definition'];assert clean.count(before)==1;start=clean.index(before);a,b=raw_at(start),raw_at(start+len(before))
        assert all(not line.startswith('#') or re.match(r'#\s*\d+\s+"',line) for line in expanded[a:b].splitlines())
        edits.append((a,b,probe_definition(before,[c['expanded_fact'] for c in row['checks']])))
    edits.sort();assert all(a[1]<=b[0] for a,b in zip(edits,edits[1:]))
    for a,b,text in reversed(edits):expanded=expanded[:a]+text+expanded[b:]
    return expanded+'\n_Static_assert(sizeof(int) == 4 && __builtin_classify_type(0) == 1, "target_loop_integer_model");\n'
