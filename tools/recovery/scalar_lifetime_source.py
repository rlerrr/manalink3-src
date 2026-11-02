"""Separate nonescaping int value lifetimes using conservative reaching definitions."""
import collections,re
from functools import lru_cache
from pycparser import c_ast
from canonicalize_helpers import mask
from source_record_flow import inspect,parse
from call_condition_source import walk
from role_local_source import CALLS,FIELDS,plain,candidates as whole_roles


@lru_cache(maxsize=32)
def member_positions(text):
    try:fn,offset=parse(text)
    except Exception:return {}
    fields=collections.defaultdict(set)
    for n in walk(fn):
        if isinstance(n,c_ast.StructRef):fields[n.field.name].add(offset(n.field))
    return fields


def name_available(text,name,*,allow_fields=False):
    code=mask(text);positions={m.start() for m in re.finditer(r'\b'+re.escape(name)+r'\b',code)}
    if not positions:return True
    if not allow_fields:return False
    for at in positions:
        prev=at-1
        while prev>=0 and code[prev].isspace():prev-=1
        if code[prev:prev+1]!='.' and code[max(0,prev-1):prev+1]!='->':return False
    fields=member_positions(text).get(name,set())
    # C members occupy their own namespace. A new local cannot capture these
    # member names; all existing ordinary identifier uses still block it.
    return positions<=fields


def active(n):
    if n is None:return
    yield n
    if isinstance(n,(c_ast.Case,c_ast.Default)):return
    for _,child in n.children():yield from active(child)


def analyze(text,*,calls=CALLS,loop_roles=False,component_role=None,declaration_type=None):
    code=mask(text)
    assert not re.search(r'\b(?:EXE_FN|asm|__asm__|__attribute__|__builtin_frame_address|__builtin_return_address|setjmp|longjmp)\b',code)
    cfg=inspect(text,cfg_only=True);fn=cfg['function'];offset=cfg['offset'];nodes=list(walk(fn));decls=collections.defaultdict(list);parents={id(c):n for n in nodes for _,c in n.children()}
    for n in nodes:
        if isinstance(n,c_ast.Decl):decls[n.name].append(n)
    outer={id(n) for n in fn.body.block_items or [] if isinstance(n,c_ast.Decl)};out=[]
    for name,ds in decls.items():
        if not name or not re.fullmatch(r'[A-Za-z]*Var\d+|[A-Za-z]*Stack_[0-9a-fA-F]+',name) or len(ds)!=1:continue
        d=ds[0]
        dtype=declaration_type(d) if declaration_type else 'int' if isinstance(d.type,c_ast.TypeDecl) and not d.type.quals and isinstance(d.type.type,c_ast.IdentifierType) and d.type.type.names==['int'] else None
        if not (id(d) in outer and not d.storage and not d.quals and d.init is None and dtype):continue
        declaration=offset(d.type.type) if declaration_type and isinstance(d.type,c_ast.PtrDecl) else offset(d)
        if code[declaration:declaration+len(name)]!=name:continue
        if any(isinstance(n,c_ast.UnaryOp) and n.op=='&' and isinstance(n.expr,c_ast.ID) and n.expr.name==name for n in nodes):continue
        if any(isinstance(n,c_ast.StructRef) and n.field.name==name for n in nodes):continue
        variable_ids=[n for n in nodes if isinstance(n,c_ast.ID) and n.name==name]
        if len(re.findall(r'\b'+re.escape(name)+r'\b',code))!=len(variable_ids)+1:continue
        records={};definitions={};bindings=collections.defaultdict(list)
        for at,n in cfg['nodes'].items():
            ns=list(active(n));ids=[v for v in ns if isinstance(v,c_ast.ID) and v.name==name]
            assignments=[v for v in ns if isinstance(v,c_ast.Assignment) and isinstance(v.lvalue,c_ast.ID) and v.lvalue.name==name]
            increments=[v for v in ns if isinstance(v,c_ast.UnaryOp) and v.op in ['++','--','p++','p--'] and isinstance(v.expr,c_ast.ID) and v.expr.name==name]
            writes=assignments+increments;kind='none';lhs=[]
            if writes:
                if len(writes)==1 and writes[0] is n and isinstance(n,c_ast.Assignment) and n.op=='=':kind='plain'
                elif len(writes)==1 and writes[0] is n:kind='rmw'
                else:kind='opaque'
                definitions[at]=(kind,n)
                lhs=[v.lvalue for v in assignments]+[v.expr for v in increments]
            simple_lhs={id(v.lvalue) for v in assignments if v.op=='='}
            reads=[v for v in ids if id(v) not in simple_lhs]
            records[at]=dict(kind=kind,reads=reads,writes=lhs)
        reaching={cfg['entry']:frozenset({0})};todo=collections.deque([cfg['entry']]);updates=0
        while todo:
            at=todo.popleft();state=reaching[at];kind=records[at]['kind']
            new=frozenset({at}) if kind in ['plain','rmw'] else state|{at} if kind=='opaque' else state
            for nxt in cfg['edges'][at]:
                merged=reaching.get(nxt,frozenset())|new
                if reaching.get(nxt)!=merged:reaching[nxt]=merged;todo.append(nxt);updates+=1;assert updates<100000
        roots={at:at for at in [0,*definitions]}
        def root(at):
            while roots[at]!=at:roots[at]=roots[roots[at]];at=roots[at]
            return at
        def join(items):
            items=sorted(items);first=root(items[0])
            for at in items:roots[root(at)]=first
            return first
        for at,r in records.items():
            state=reaching.get(at,frozenset({0}));keys=state
            if r['reads'] and r['kind'] in ['opaque','rmw']:keys=state|{at}
            if r['reads']:
                key=join(keys)
                for n in r['reads']:bindings[offset(n)].append(key)
            for n in r['writes']:bindings[offset(n)].append(at if at in reaching else 0)
        # Shared reads merge predecessor values. A read/modify/write token also
        # keeps its incoming value and definition in the same scalar object.
        for keys in bindings.values():join(keys)
        groups=collections.defaultdict(list);tokens=collections.defaultdict(list)
        for at in roots:groups[root(at)].append(at)
        for position,keys in bindings.items():tokens[root(keys[0])].append(position)
        all_refs=sorted(offset(n) for n in variable_ids)
        assert sorted(bindings)==all_refs
        loop_reads={offset(v) for n in nodes if isinstance(n,(c_ast.While,c_ast.DoWhile,c_ast.For)) and n.cond is not None for b in walk(n.cond) if isinstance(b,c_ast.BinaryOp) and b.op in ['<','<=','>','>=','!='] for v in [b.left,b.right] if isinstance(v,c_ast.ID) and v.name==name} if loop_roles else set()
        for key,defs in groups.items():
            if 0 in defs or len(tokens[key])<=len(defs) or any(at not in reaching for at in defs):continue
            options=None;roles=[];valid=True
            for at in defs:
                kind,n=definitions[at]
                if kind!='plain':valid=False;break
                value=plain(n.rvalue)
                if isinstance(value,c_ast.FuncCall) and isinstance(value.name,c_ast.ID) and value.name.name not in decls:
                    callee=value.name.name;choices=calls.get(callee,['count','qty','num'] if callee.startswith('count_') else []);role='call:'+callee
                elif isinstance(value,c_ast.StructRef):choices=FIELDS.get(value.field.name,[]);role='field:'+value.field.name
                else:valid=False;break
                if not choices:valid=False;break
                options=list(choices) if options is None else [v for v in options if v in choices];roles.append(role)
            loop_role=None
            if not (valid and options) and loop_roles and loop_reads.intersection(tokens[key]) and not any(definitions[at][0]=='opaque' for at in defs):
                keep=set(tokens[key]);view=text;other='unrelated_scalar_lifetime'
                assert not re.search(r'\b'+other+r'\b',code)
                for position in sorted(set(all_refs)-keep,reverse=True):view=view[:position]+other+view[position+len(name):]
                matches=[r for r in whole_roles(view) if r['old_name']==name and r['role']=='loop_index']
                if len(matches)==1:
                    loop_role=matches[0];options=loop_role['options'];roles=['loop_index'];valid=True
                    vf,_=parse(view)
                    if any(isinstance(n,c_ast.FuncCall) and isinstance(n.name,c_ast.ID) and n.name.name in ['get_card_instance','in_play','is_what','kill_card','get_id','get_power','get_toughness'] and n.args is not None and n.args.exprs and isinstance(n.args.exprs[0],c_ast.ID) and n.args.exprs[0].name==name for n in walk(vf)):
                        options=['p','current_player','which_player',*options]
            custom_role=component_role(name=name,definitions=[definitions[at] for at in sorted(defs)],references=set(tokens[key]),function=fn,nodes=nodes,offset=offset) if component_role else None
            if custom_role:valid=True;options=custom_role['options'];roles=custom_role['roles']
            if valid and options:
                fact=dict(old_name=name,options=options,roles=sorted(set(roles)),definition_offsets=sorted(offset(definitions[at][1].lvalue if isinstance(definitions[at][1],c_ast.Assignment) else definitions[at][1].expr) for at in defs),reference_offsets=sorted(tokens[key]),all_reference_offsets=all_refs,declaration_offset=declaration,reaching_definition_nodes=sorted(defs),dataflow_updates=updates,source_cfg_nodes=len(cfg['nodes']))
                if loop_role:fact['loop_role']=loop_role
                if custom_role:fact['component_role']=custom_role
                if declaration_type:fact['declaration_type']=dtype
                out.append(fact)
    return sorted(out,key=lambda g:(g['old_name'],g['definition_offsets']))


def project(before,selected,*,calls=CALLS,allow_field_names=False,loop_roles=False,component_role=None,declaration_type=None):
    found=analyze(before,calls=calls,loop_roles=loop_roles,component_role=component_role,declaration_type=declaration_type);code=mask(before);edits={};names={};by_old=collections.defaultdict(list)
    for item in selected:
        g,new=item['group'],item['new_name'];old=g['old_name'];assert g in found and new in g['options'] and re.fullmatch(r'[A-Za-z_]\w*',new)
        assert name_available(before,new,allow_fields=allow_field_names)
        assert new not in names or names[new]==old;names[new]=old;by_old[old].append(item)
        for at in g['reference_offsets']:
            assert code[at:at+len(old)]==old and at not in edits;edits[at]=(at+len(old),new)
    added=[];added_types={};reused=0
    for old,items in sorted(by_old.items()):
        g=items[0]['group'];bindings=list(dict.fromkeys(item['new_name'] for item in items));covered=sorted(at for item in items for at in item['group']['reference_offsets'])
        assert len(covered)==len(set(covered))
        if covered==g['all_reference_offsets']:
            at=g['declaration_offset'];edits[at]=(at+len(old),bindings.pop(0));reused+=1
        added.extend(bindings)
        for binding in bindings:added_types[binding]=g.get('declaration_type','int')
    after=before
    for a,(b,value) in sorted(edits.items(),reverse=True):after=after[:a]+value+after[b:]
    collapsed=after
    if added:
        fn,offset=parse(before);brace=offset(fn.body);assert after[brace]=='{'
        after=after[:brace+1]+''.join('\n  '+added_types[name]+('' if added_types[name].endswith('*') else ' ')+name+';' for name in added)+after[brace+1:]
    # Merging the disjoint scalar objects must recover the exact predecessor,
    # including every expression, call, branch, literal, comment and old local.
    inverse=[];delta=0
    for a,(b,value) in sorted(edits.items()):
        start=a+delta;assert collapsed[start:start+len(value)]==value
        inverse.append((start,start+len(value),before[a:b]));delta+=len(value)-(b-a)
    for a,b,value in reversed(inverse):collapsed=collapsed[:a]+value+collapsed[b:]
    assert collapsed==before
    facts=dict(value_groups=len(selected),named_bindings=len(names),renamed_identifier_occurrences=len(edits),reused_placeholder_declarations=reused)
    facts['added_typed_declarations' if declaration_type else 'added_int_declarations']=len(added)
    return after,facts
