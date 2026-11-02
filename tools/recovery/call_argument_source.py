"""Inline adjacent int call results into one consumer argument, preserving casts."""
import collections,copy,json,re
from pycparser import c_ast,c_generator
from canonicalize_helpers import mask,sha
from source_record_flow import inspect,parse
from call_condition_source import walk
from target_loop_source import epsilon

GEN=c_generator.CGenerator()


def active(n):
    if n is None:return
    yield n
    if isinstance(n,(c_ast.Case,c_ast.Default)):return
    for _,child in n.children():yield from active(child)


def analyze(text):
    code=mask(text);assert not re.search(r'\b(?:EXE_FN|asm|__asm__|__attribute__|__builtin_frame_address|__builtin_return_address|setjmp|longjmp|_Atomic)\b',code)
    cfg=inspect(text,cfg_only=True);fn=cfg['function'];off=cfg['offset'];allnodes=list(walk(fn));decls=collections.defaultdict(list)
    for n in allnodes:
        if isinstance(n,c_ast.Decl):decls[n.name].append(n)
    top={id(d) for d in fn.body.block_items or [] if isinstance(d,c_ast.Decl)}
    params={id(d) for d in fn.decl.type.args.params} if fn.decl.type.args else set()
    escaped={n.expr.name for n in allnodes if isinstance(n,c_ast.UnaryOp) and n.op=='&' and isinstance(n.expr,c_ast.ID)}
    ordinary={name for name,ds in decls.items() if len(ds)==1 and id(ds[0]) in top|params and not ds[0].storage and not ds[0].quals and isinstance(ds[0].type,c_ast.TypeDecl) and not ds[0].type.quals and isinstance(ds[0].type.type,c_ast.IdentifierType)}
    names={name for name in ordinary if name and re.fullmatch(r'[A-Za-z]*Var\d+|[A-Za-z]*Stack_[0-9a-fA-F]+',name) and name not in escaped and id(decls[name][0]) in top and decls[name][0].init is None and decls[name][0].type.type.names==['int']}
    names-={n.field.name for n in allnodes if isinstance(n,c_ast.StructRef)}
    refs=lambda n:{v.name for v in active(n) if isinstance(v,c_ast.ID)}&names
    uses={};defs={};pred=collections.defaultdict(set)
    for at,n in cfg['nodes'].items():
        if isinstance(n,c_ast.Assignment) and isinstance(n.lvalue,c_ast.ID):uses[at]=refs(n.rvalue)|(refs(n.lvalue) if n.op!='=' else set());defs[at]={n.lvalue.name}&names
        elif isinstance(n,c_ast.Decl):uses[at]=refs(n.init);defs[at]={n.name}&names if n.init is not None else set()
        else:uses[at]=refs(n);defs[at]=set()
        for nxt in cfg['edges'][at]:pred[nxt].add(at)
    live={at:set() for at in cfg['nodes']};change=True
    while change:
        change=False
        for at in reversed(cfg['nodes']):
            out=set().union(*(live[n] for n in cfg['edges'][at])) if cfg['edges'][at] else set();new=uses[at]|(out-defs[at])
            if new!=live[at]:live[at]=new;change=True
    def safe(n,locals_,constants,addresses):
        if isinstance(n,c_ast.Constant):return n.type not in ['float','double','long double']
        if isinstance(n,c_ast.ID):
            if n.name in ordinary:
                if n.name in escaped:return False
                locals_.add(n.name)
            else:constants.add(n.name)
            return True
        if isinstance(n,c_ast.UnaryOp):
            if n.op=='sizeof':constants.add(GEN.visit(n));return True
            if n.op=='&' and isinstance(n.expr,c_ast.ID) and len(decls[n.expr.name])==1 and id(decls[n.expr.name][0]) in top|params:addresses.add(n.expr.name);return True
            return n.op in ['+','-','!','~'] and safe(n.expr,locals_,constants,addresses)
        if isinstance(n,c_ast.BinaryOp):return n.op in ['+','-','*','&','|','^','==','!=','<','<=','>','>=','&&','||'] and safe(n.left,locals_,constants,addresses) and safe(n.right,locals_,constants,addresses)
        if isinstance(n,c_ast.Cast):
            typ=n.to_type.type
            okay=isinstance(typ,c_ast.PtrDecl) or isinstance(typ,c_ast.TypeDecl) and isinstance(typ.type,c_ast.IdentifierType) and set(typ.type.names)<=set(['int','unsigned','signed','long','short','char'])
            return okay and safe(n.expr,locals_,constants,addresses)
        return False
    def call_end(call):
        p=code.index('(',off(call));at=p+1;depth=1
        while depth:depth+=(code[at]=='(')-(code[at]==')');at+=1
        return at
    found=[]
    for at,n in cfg['nodes'].items():
        if not (isinstance(n,c_ast.Assignment) and n.op=='=' and isinstance(n.lvalue,c_ast.ID) and n.lvalue.name in names and isinstance(n.rvalue,c_ast.FuncCall) and isinstance(n.rvalue.name,c_ast.ID) and n.rvalue.name.name not in decls and len(cfg['edges'][at])==1):continue
        target=cfg['edges'][at][0];consumer=cfg['nodes'][target];call=consumer.rvalue if isinstance(consumer,c_ast.Assignment) and consumer.op=='=' and isinstance(consumer.lvalue,c_ast.ID) else consumer
        if pred[target]!={at} or not isinstance(call,c_ast.FuncCall) or not isinstance(call.name,c_ast.ID) or call.name.name in decls or call.args is None:continue
        name=n.lvalue.name;ids=[v for v in walk(call) if isinstance(v,c_ast.ID) and v.name==name]
        args=call.args.exprs;hits=[i for i,a in enumerate(args) if isinstance(a,c_ast.ID) and a.name==name]
        if len(ids)!=1 or len(hits)!=1:continue
        if name not in defs[target] and any(name in live[x] for x in cfg['edges'][target]):continue
        start=off(n);end=code.index(';',start)+1;where=off(consumer);between=code[end:where]
        if not re.fullmatch(r'\s*(?:return\s+)?',between):continue
        line=text.rfind('\n',0,start)+1;last=text.find('\n',end)
        if last<0 or text[line:start].strip() or text[end:last].strip():continue
        ce=call_end(n.rvalue)
        if code[ce:end-1].strip():continue
        locals_=set();constants=set();addresses=set();okay=True
        for i,arg in enumerate(args):
            if i!=hits[0] and not safe(arg,locals_,constants,addresses):okay=False;break
        if not okay:continue
        found.append(dict(variable=name,producer=n.rvalue.name.name,consumer=call.name.name,producer_node=at,consumer_node=target,assignment_start=start,assignment_end=end,remove_start=line,remove_end=last+1,call_start=off(n.rvalue),call_end=ce,argument_offset=off(args[hits[0]]),argument_index=hits[0],consumer_offset=off(call),stable_local_reads=sorted(locals_),constant_checks=sorted(constants),stable_addresses=sorted(addresses),consumer_kills_value=name in defs[target]))
    return sorted(found,key=lambda c:c['assignment_start'])


def project(before,selected):
    found=analyze(before)
    assert selected and all({k:v for k,v in c.items() if k!='omit_int_cast'} in found for c in selected);byarg={c['argument_offset']:c for c in selected};memo={}
    def expression(c):
        value=call(c)
        return value if c.get('omit_int_cast') else '((int)('+value+'))'
    def call(c):
        key=c['assignment_start']
        if key in memo:return memo[key]
        start,end=c['call_start'],c['call_end'];value=before[start:end]
        for pos,child in sorted(byarg.items(),reverse=True):
            if start<=pos<end:value=value[:pos-start]+expression(child)+value[pos-start+len(child['variable']):]
        memo[key]=value;return value
    removals=[(c['remove_start'],c['remove_end'],'') for c in selected];edits=list(removals)
    for c in selected:
        at=c['argument_offset']
        if not any(a<=at<b for a,b,_ in removals):edits.append((at,at+len(c['variable']),expression(c)))
    edits.sort();assert all(a[1]<=b[0] for a,b in zip(edits,edits[1:]));after=before
    for a,b,s in reversed(edits):after=after[:a]+s+after[b:]
    removed=[]
    for name in sorted({c['variable'] for c in selected}):
        if len(re.findall(r'\b'+re.escape(name)+r'\b',mask(after)))!=1:continue
        declaration='  int '+name+';\n'
        if after.count(declaration)==1:after=after.replace(declaration,'');removed.append(name)
    return after,dict(inlined_call_results=len(selected),removed_locals=removed,source_candidates=selected,cfg_contraction=cfg_proof(before,after,selected,removed))


def cfg_proof(before,after,selected,removed):
    old=inspect(before,cfg_only=True);new=inspect(after,cfg_only=True);arguments={c['argument_offset']:c for c in selected};calls={c['assignment_start']:old['nodes'][c['producer_node']].rvalue for c in selected};cast=c_ast.Typename(None,[],None,c_ast.TypeDecl(None,[],None,c_ast.IdentifierType(['int'])))
    def clone(n):
        if n is None:return None
        if isinstance(n,c_ast.ID) and old['offset'](n) in arguments:
            c=arguments[old['offset'](n)];value=clone(calls[c['assignment_start']])
            return value if c.get('omit_int_cast') else c_ast.Cast(copy.deepcopy(cast),value)
        out=copy.copy(n)
        for field in n.__slots__:
            if field in ['coord','__weakref__']:continue
            v=getattr(n,field)
            if isinstance(v,c_ast.Node):setattr(out,field,clone(v))
            elif isinstance(v,list):setattr(out,field,[clone(x) if isinstance(x,c_ast.Node) else x for x in v])
        return out
    skip={c['producer_node'] for c in selected}
    for at,n in old['nodes'].items():
        if at in skip or isinstance(n,c_ast.Decl) and n.name in removed:old['nodes'][at]=None
        else:old['nodes'][at]=clone(n)
    def observe(g,at):
        at=epsilon(g,at);n=g['nodes'][at];edges=g['edges'][at]
        if isinstance(n,c_ast.Case):tag=('case',GEN.visit(n.expr))
        elif isinstance(n,c_ast.Default):tag=('default','')
        elif n is None:tag=('fallthrough','')
        else:tag=('condition' if len(edges)==2 else 'expression',GEN.visit(n))
        return at,tag,edges
    todo=[(old['entry'],new['entry'])];pairs=set();receipts=[]
    while todo:
        a,b=todo.pop();a,sa,ea=observe(old,a);b,sb,eb=observe(new,b)
        if (a,b) in pairs:continue
        assert sa==sb and len(ea)==len(eb),(sa,sb)
        pairs.add((a,b));receipts.append((a,b,sa,len(ea)));todo.extend(zip(ea,eb))
    return dict(state_pairs=len(pairs),graph_pair_sha256=sha(json.dumps(sorted(receipts),separators=(',',':')).encode()),scope='Whole-function CFG bisimulation after adjacent call-result contractions and dead ordinary-int store/declaration removal. Explicit int casts retain producer assignment conversions. Commutation of other arguments requires the separately checked stable locals, addresses and actual compiler constants.')


def probe_definition(before,row,*,exact_int=False):
    """Unevaluated checks at the actual expanded assignment's lexical scope."""
    edits=[]
    for check in row['checks']:
        c=check['expanded_fact'];call=before[c['call_start']:c['call_end']];label=check['probe_id']
        conditions=['sizeof(int) == 4',f'__builtin_types_compatible_p(__typeof__(&{c["variable"]}), int *)',f'__builtin_classify_type(({call})) == 1',f'__builtin_types_compatible_p(__typeof__({c["consumer"]}), __typeof__(*{c["consumer"]}))']
        for name in c['stable_local_reads']:
            conditions += [f'__builtin_classify_type({name}) == 1',f'!__builtin_types_compatible_p(__typeof__(&{name}), volatile __typeof__({name}) *)']
        for expr in c['constant_checks']:
            conditions += [f'__builtin_choose_expr(__builtin_constant_p(({expr})), 1, 0)',f'(__builtin_classify_type(({expr})) == 1 || __builtin_classify_type(({expr})) == 5)']
        conditions += [f'__builtin_classify_type(&{name}) == 5' for name in c['stable_addresses']]
        if c.get('omit_int_cast'):conditions.append(f'__builtin_types_compatible_p(__typeof__(({call})), int)')
        assertions='_Static_assert('+ ' && '.join('('+x+')' for x in conditions)+', "argument_safe_'+label+'"); '
        if exact_int:assertions+=f'_Static_assert(__builtin_types_compatible_p(__typeof__(({call})), int), "argument_int_{label}"); '
        original=before[c['assignment_start']:c['assignment_end']]
        edits.append((c['assignment_start'],c['assignment_end'],'{ '+assertions+original+' }'))
    for a,b,s in reversed(sorted(edits)):before=before[:a]+s+before[b:]
    return before
