"""Restore forward conditional guards without moving actions or narrowing locals."""
import json,re
from pycparser import c_ast,c_generator
from canonicalize_helpers import mask,sha
from call_condition_source import walk
from source_record_flow import parse,inspect
from target_loop_source import epsilon

GEN=c_generator.CGenerator()


def analyze(text,*,all_candidates=False):
    code=mask(text)
    assert not re.search(r'\b(?:EXE_FN|asm|__asm__|__attribute__|__builtin_frame_address|__builtin_return_address|setjmp|longjmp)\b',code)
    fn,offset=parse(text);nodes=list(walk(fn));gotos=[n for n in nodes if isinstance(n,c_ast.Goto)];ordinals={id(n):i for i,n in enumerate(gotos)}
    def top_decl(n):
        if isinstance(n,(c_ast.Decl,c_ast.Typedef)):return True
        if isinstance(n,c_ast.Label):return top_decl(n.stmt)
        if isinstance(n,(c_ast.Case,c_ast.Default)):return any(top_decl(s) for s in n.stmts or [])
        return False
    found=[]
    for block in nodes:
        if not isinstance(block,c_ast.Compound):continue
        items=block.block_items or []
        for i,n in enumerate(items):
            if not isinstance(n,c_ast.If) or n.iffalse is not None:continue
            branch=n.iftrue.block_items or [] if isinstance(n.iftrue,c_ast.Compound) else [n.iftrue]
            if len(branch)!=1 or not isinstance(branch[0],c_ast.Goto):continue
            jump=branch[0];label=jump.name
            if not re.fullmatch(r'LAB_[a-fA-F0-9]+',label):continue
            targets=[j for j in range(i+2,len(items)) if isinstance(items[j],c_ast.Label) and items[j].name==label]
            if len(targets)!=1:continue
            j=targets[0];payload=items[i+1:j]
            if any(top_decl(s) for s in payload):continue
            start=offset(n);line=text.rfind('\n',0,start)+1;indent=text[line:start]
            if indent.strip():continue
            opening=code.index('(',start);closing=opening+1;depth=1
            while depth:depth+=(code[closing]=='(')-(code[closing]==')');closing+=1
            branch_start=offset(n.iftrue)
            if code[closing:branch_start].strip():continue
            if isinstance(n.iftrue,c_ast.Compound):
                end=code.index('}',branch_start)+1
                expected=r'\{\s*goto\s+'+re.escape(label)+r'\s*;\s*\}'
            else:end=code.index(';',branch_start)+1;expected=r'goto\s+'+re.escape(label)+r'\s*;'
            if not re.fullmatch(expected,text[branch_start:end]):continue
            label_at=offset(items[j]);target_line=text.rfind('\n',0,label_at)+1
            if text[target_line:label_at].strip():continue
            first_newline=text.find('\n',end)
            if first_newline<0 or text[end:first_newline].strip():continue
            body=text[first_newline+1:target_line]
            if not body.strip():continue
            found.append(dict(label=label,goto_ordinal=ordinals[id(jump)],goto_start=offset(jump),if_start=line,condition=text[opening+1:closing-1],condition_start=opening+1,branch_end=end,payload_start=first_newline+1,target_line_start=target_line,indent=indent,payload_statements=len(payload)))
    if all_candidates:return sorted(found,key=lambda c:c['if_start'])
    # Shorter disjoint regions first. Further rounds expose enclosing guards.
    selected=[]
    for c in sorted(found,key=lambda c:(c['target_line_start']-c['if_start'],c['if_start'])):
        if any(c['if_start']<v['target_line_start'] and v['if_start']<c['target_line_start'] for v in selected):continue
        selected.append(c)
    return sorted(selected,key=lambda c:c['if_start'])


def cfg_proof(before,after):
    old=inspect(before,cfg_only=True);new=inspect(after,cfg_only=True)
    def conditions(cfg):
        return {cfg['offset'](n.cond) for n in walk(cfg['function']) if isinstance(n,(c_ast.If,c_ast.While,c_ast.DoWhile,c_ast.For)) and n.cond is not None}
    boolean={id(old):conditions(old),id(new):conditions(new)}
    def observe(cfg,at):
        at=epsilon(cfg,at);n=cfg['nodes'][at];edges=cfg['edges'][at]
        if n is not None and cfg['offset'](n) in boolean[id(cfg)] and len(edges)==2:
            while isinstance(n,c_ast.UnaryOp) and n.op=='!':n=n.expr;edges=list(reversed(edges))
            tag=('condition',GEN.visit(n))
        elif isinstance(n,c_ast.Case):tag=('case',GEN.visit(n.expr))
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
    return dict(state_pairs=len(pairs),graph_pair_sha256=sha(json.dumps(sorted(receipts),separators=(',',':')).encode()),scope='Whole-function statement CFG bisimulation. Boolean-condition ! swaps ordered successors; switch selectors and action expressions are not normalized. All actions, calls, predicate reads and returns stay ordered. No declarations acquire a narrower scope.')


def step(before,selected):
    found=analyze(before,all_candidates=True);assert selected and all(c in found for c in selected);after=before
    regions=sorted(selected,key=lambda c:c['if_start']);assert all(a['target_line_start']<=b['if_start'] for a,b in zip(regions,regions[1:]))
    for c in sorted(selected,key=lambda c:c['if_start'],reverse=True):
        body=before[c['payload_start']:c['target_line_start']];body=''.join('  '+line if line.strip() else line for line in body.splitlines(keepends=True));indent=c['indent']
        replacement=indent+'if (!('+c['condition']+'))\n'+indent+'{\n'+body+indent+'}\n'
        after=after[:c['if_start']]+replacement+after[c['target_line_start']:]
    removed=[]
    for label in sorted({c['label'] for c in selected}):
        code=mask(after)
        if re.search(r'\bgoto\s+'+re.escape(label)+r'\s*;',code):continue
        line=re.search(r'(?m)^[ \t]*'+re.escape(label)+r':[ \t]*\n',code)
        if not line or not re.fullmatch(r'[ \t]*'+re.escape(label)+r':[ \t]*\n',after[line.start():line.end()]):continue
        after=after[:line.start()]+after[line.end():];removed.append(label)
    return after,dict(removed_gotos=len(selected),removed_labels=removed,candidates=selected,cfg_bisimulation=cfg_proof(before,after))


def project(before,rounds=None):
    after=before;facts=[];index=0
    while True:
        found=analyze(after,all_candidates=rounds is not None)
        if rounds is None:selected=found
        elif index==len(rounds):break
        else:
            wanted=[(c['label'],c['goto_ordinal']) for c in rounds[index]['candidates']];selected=[c for c in found if (c['label'],c['goto_ordinal']) in wanted];assert [(c['label'],c['goto_ordinal']) for c in selected]==wanted
        if not selected:break
        after,fact=step(after,selected);facts.append(fact);index+=1
        assert index<=len(re.findall(r'\bgoto\b',mask(before)))
    assert facts
    if rounds is not None:assert index==len(rounds)
    return after,dict(removed_gotos=sum(c['removed_gotos'] for c in facts),removed_labels=[label for c in facts for label in c['removed_labels']],rounds=facts)
