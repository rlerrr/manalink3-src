"""Symbolic action-trace equivalence with fresh arbitrary globals after actions."""
import collections,copy,json,re
from pycparser import c_ast,c_generator
from source_record_flow import inspect
from call_condition_source import walk
from keyword_lookup_analysis import BDD,Integer
from canonicalize_helpers import sha

GEN=c_generator.CGenerator()


def verify(before,after,constants):
    bdd=BDD();sp=Integer(tuple(bdd.node(31-i,0,1) for i in range(32)));ability=bdd.node(32,0,1);combat=bdd.node(33,0,1);facts={key:bdd.node(i,0,1) for i,key in enumerate(['card','controller','turn','attack_allowed'],34)};valid=bdd.neg(bdd.op('&',ability,combat));globals_=set(range(34,38));exists_cache={0:0,1:1}
    def refresh(u):
        if u not in exists_cache:
            v,lo,hi=bdd.nodes[u];a,b=refresh(lo),refresh(hi);exists_cache[u]=bdd.op('|',a,b) if v in globals_ else bdd.node(v,a,b)
        return exists_cache[u]
    def cst(v,width=32,signed=True):return Integer(tuple((v>>i)&1 for i in range(width)),signed)
    def cast(a,width,signed):return Integer((a.bits+(a.bits[-1] if a.signed else 0,)*max(0,width-len(a.bits)))[:width],signed)
    def promoted(a):return cast(a,32,True) if len(a.bits)<32 else a
    def truth(a):return bdd.either(a.bits)
    def boolean(a):return Integer((a,)+(0,)*31)
    def number(n):
        if isinstance(n,c_ast.ID):assert n.name in constants;return constants[n.name]
        assert isinstance(n,c_ast.Constant);return int(re.sub(r'[uUlL]+$','',n.value),0)
    def expression(n,env):
        if isinstance(n,c_ast.Constant):return cst(number(n),signed=n.type!='unsigned int')
        if isinstance(n,c_ast.ID):
            if n.name=='sp_abilities':return sp
            if n.name in constants:return cst(constants[n.name])
            return env[n.name]
        if isinstance(n,c_ast.Cast):
            names=n.to_type.type.type.names;assert names in [['unsigned','int'],['unsigned','char']]
            return cast(expression(n.expr,env),8 if 'char' in names else 32,False)
        if isinstance(n,c_ast.UnaryOp):assert n.op=='!';return boolean(bdd.neg(truth(expression(n.expr,env))))
        assert isinstance(n,c_ast.BinaryOp);op=n.op
        if op in ['==','!=']:
            ids={a.name for a in [n.left,n.right] if isinstance(a,c_ast.ID)};value=None
            if 'event' in ids:
                other=n.right if isinstance(n.left,c_ast.ID) and n.left.name=='event' else n.left;v=number(other);assert v in [constants['EVENT_ABILITIES'],constants['EVENT_BEGIN_COMBAT']];value=ability if v==constants['EVENT_ABILITIES'] else combat
            elif ids=={'card','affected_card'}:value=facts['card']
            elif ids=={'player','affected_card_controller'}:value=facts['controller']
            elif ids=={'player','current_turn'}:value=facts['turn']
            elif ids=={'forbid_attack'}:
                other=n.right if isinstance(n.left,c_ast.ID) else n.left;assert number(other)==0;value=facts['attack_allowed']
            if value is not None:return boolean(value if op=='==' else bdd.neg(value))
        a,b=promoted(expression(n.left,env)),promoted(expression(n.right,env))
        if op in ['&&','||']:return boolean(bdd.op('&' if op=='&&' else '|',truth(a),truth(b)))
        if op in ['&','|','^']:return Integer(tuple(bdd.op(op,x,y) for x,y in zip(a.bits,b.bits)),a.signed and b.signed)
        if op=='>>':
            assert all(v in [0,1] for v in b.bits);shift=sum(v<<i for i,v in enumerate(b.bits));assert 0<=shift<32
            return Integer((a.bits[shift:]+(a.bits[-1] if a.signed else 0,)*shift),a.signed)
        if op in ['==','!=']:
            equal=bdd.both(bdd.neg(bdd.op('^',x,y)) for x,y in zip(a.bits,b.bits));return boolean(equal if op=='==' else bdd.neg(equal))
        raise AssertionError(op)
    def normalized(n):
        if n is None:return None
        if isinstance(n,c_ast.ID):
            if n.name in constants:return c_ast.Constant('int',str(constants[n.name]))
            if n.name=='target':return c_ast.ID('pcVar6')
            if n.name=='NULL':return c_ast.Constant('int','0')
        if isinstance(n,c_ast.Constant):return c_ast.Constant('int',str(number(n)))
        if isinstance(n,c_ast.Cast) and GEN.visit(n.to_type)=='void *' and isinstance(n.expr,c_ast.Constant) and number(n.expr)==0:return c_ast.Constant('int','0')
        out=copy.copy(n)
        for field in n.__slots__:
            if field in ['coord','__weakref__']:continue
            v=getattr(n,field)
            if isinstance(v,c_ast.Node):setattr(out,field,normalized(v))
            elif isinstance(v,list):setattr(out,field,[normalized(x) if isinstance(x,c_ast.Node) else x for x in v])
        if isinstance(out,c_ast.Assignment) and out.op=='|=':out=c_ast.Assignment('=',out.lvalue,c_ast.BinaryOp('|',copy.deepcopy(out.lvalue),out.rvalue))
        return out
    def prepare(text):
        g=inspect(text,cfg_only=True);fn=g['function'];nodes=list(walk(fn));assert [p.name for p in fn.decl.type.args.params]==['player','card','event','sp_abilities','source_player','source_card']
        assert not any(isinstance(n,(c_ast.While,c_ast.DoWhile,c_ast.For)) or isinstance(n,c_ast.UnaryOp) and n.op=='&' for n in nodes)
        declarations=[n for n in nodes if isinstance(n,c_ast.Decl)];assert all(n.init is None for n in declarations) and len({n.name for n in declarations})==len(declarations)
        allowed={'get_card_instance','vigilance_part','deathtouch_part','attack_if_able_part','indestructible_part','hexproof_part','lure_effect','granted_flanking','block_if_able'}
        assert all(isinstance(n.name,c_ast.ID) and n.name.name in allowed for n in nodes if isinstance(n,c_ast.FuncCall))
        integers={n.name for n in nodes if isinstance(n,c_ast.Decl) and re.fullmatch(r'bVar\d+',n.name or '')};assert all(not n.quals and not n.storage and n.init is None for n in nodes if isinstance(n,c_ast.Decl) and n.name in integers)
        bools=set()
        for n in declarations:
            if n.name not in integers:continue
            assert isinstance(n.type,c_ast.TypeDecl) and isinstance(n.type.type,c_ast.IdentifierType)
            names=n.type.type.names;assert names in [['unsigned','char'],['bool'],['_Bool']]
            if names in [['bool'],['_Bool']]:bools.add(n.name)
        memo={};active=set()
        def keyenv(env):return tuple(sorted(env.items()))
        def look(at,env):
            key=(at,keyenv(env))
            if key in memo:return memo[key]
            assert key not in active,'Cycle in dispatch';active.add(key);n=g['nodes'][at];edges=g['edges'][at];out={}
            def add(result,condition=1):
                for endpoint,guard in result.items():
                    guard=bdd.op('&',guard,condition)
                    if guard:out[endpoint]=bdd.op('|',out.get(endpoint,0),guard)
            if not edges:assert n is None;out[(('return',),None,keyenv(env))]=1
            elif n is None or isinstance(n,c_ast.Decl):assert len(edges)==1;add(look(edges[0],env))
            elif isinstance(n,c_ast.Assignment) and isinstance(n.lvalue,c_ast.ID) and n.lvalue.name in integers:
                assert n.op=='=';name=n.lvalue.name;value=expression(n.rvalue,env)
                if name in bools:value=boolean(truth(value))
                value=cast(value,8,False)
                # No local snapshot of mutable engine facts may survive a call.
                def private(u,seen):
                    if u in seen:return True
                    seen.add(u)
                    if u<2:return True
                    v,lo,hi=bdd.nodes[u];return v<34 and private(lo,seen) and private(hi,seen)
                assert all(private(u,set()) for u in value.bits);new=dict(env);new[name]=value;assert len(edges)==1;add(look(edges[0],new))
            elif isinstance(n,(c_ast.Assignment,c_ast.FuncCall)):
                if isinstance(n,c_ast.Assignment):
                    assert n.op in ['=','|=']
                    if isinstance(n.lvalue,c_ast.ID):assert n.lvalue.name in ['instance','pcVar6','target'] and isinstance(n.rvalue,c_ast.FuncCall)
                    else:assert isinstance(n.lvalue,c_ast.StructRef) and n.lvalue.field.name=='sp_keywords'
                token=('action',GEN.visit(normalized(n)));assert len(edges)==1;out[(token,edges[0],keyenv(env))]=1
            else:
                assert len(edges)==2;test=truth(expression(n,env));add(look(edges[0],env),test);add(look(edges[1],env),bdd.neg(test))
            active.remove(key);memo[key]=out;return out
        return g,look
    old,oldlook=prepare(before);new,newlook=prepare(after);initial=(old['entry'],new['entry'],(),());constraints={initial:valid};todo=collections.deque([initial]);processed=0;matches=0;transcript=[]
    def witness(u):
        values={}
        while u>=2:
            v,lo,hi=bdd.nodes[u];values[v]=0 if lo else 1;u=lo if lo else hi
        return dict(requested=hex(sum(values.get(31-i,0)<<i for i in range(32))),abilities=values.get(32,0),combat=values.get(33,0),globals={n:values.get(i,0) for i,n in enumerate(facts,34)})
    while todo:
        state=todo.popleft();a,b,ae,be=state;guard=constraints[state];left=oldlook(a,dict(ae));right=newlook(b,dict(be));assert bdd.op('&',guard,bdd.neg(bdd.either(left.values())))==0 and bdd.op('&',guard,bdd.neg(bdd.either(right.values())))==0
        for (ta,na,nea),ga in left.items():
            for (tb,nb,neb),gb in right.items():
                joint=bdd.both([guard,ga,gb])
                if not joint:continue
                assert ta==tb,(ta,tb,witness(joint));matches+=1
                if na is None or nb is None:assert na is nb;continue
                successor=(na,nb,nea,neb);fresh=refresh(joint);prior=constraints.get(successor,0);merged=bdd.op('|',prior,fresh)
                if merged!=prior:constraints[successor]=merged;todo.append(successor)
                transcript.append((a,b,na,nb,ta))
        processed+=1;assert processed<20000,'Dispatch comparison size guard'
    return dict(product_states=len(constraints),processed_states=processed,matched_actions=matches,bdd_nodes=len(bdd.nodes),transition_sha256=sha(json.dumps(transcript,separators=(',',':')).encode()),scope='Symbolic next-action equivalence over every32-bit keyword mask and all event categories used by this routine. Card/controller/turn/forbid facts are independent and refreshed arbitrarily after every matched call or record write. Only nonescaping private flag stores and declarations are silent. Calls, pointer-result assignments, record updates and returns match in order; target/pcVar6 is an alpha rename. Local byte conversions and unsigned shifts are preserved. No callback-purity or stable-global assumption across actions.')
