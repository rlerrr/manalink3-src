"""Exact integer/BDD interpretation of the acyclic special-keyword lookup.

The admitted C subset has no memory, calls, loops or observable local addresses.
Each output bit must equal its requested bit AND a finite card-ID membership
set. Reduced ordered BDD identity checks that property over both full int32
inputs, including combinations of requested keywords and out-of-range IDs.
"""
import collections,json,re
from dataclasses import dataclass
from pycparser import c_ast
from canonicalize_helpers import sha
from source_record_flow import inspect
from call_condition_source import walk


class BDD:
    def __init__(self):
        self.nodes=[(64,0,0),(64,1,1)];self.unique={};self.cache={};self.inverted={0:1,1:0}

    def node(self,var,lo,hi):
        if lo==hi:return lo
        key=(var,lo,hi)
        if key not in self.unique:
            assert 0<=var<64 and self.nodes[lo][0]>var and self.nodes[hi][0]>var
            assert len(self.nodes)<3_000_000,'BDD size guard'
            self.unique[key]=len(self.nodes);self.nodes.append(key)
        return self.unique[key]

    def neg(self,u):
        if u not in self.inverted:
            v,lo,hi=self.nodes[u];out=self.node(v,self.neg(lo),self.neg(hi));self.inverted[u]=out;self.inverted[out]=u
        return self.inverted[u]

    def op(self,op,a,b):
        if a>b:a,b=b,a
        if op=='&':
            if a==0 or a==b:return a
            if a==1:return b
        elif op=='|':
            if a==0:return b
            if a==1 or a==b:return a
        elif op=='^':
            if a==0:return b
            if a==b:return 0
            if a==1:return self.neg(b)
        else:raise AssertionError(op)
        key=(op,a,b)
        if key not in self.cache:
            va,la,ha=self.nodes[a];vb,lb,hb=self.nodes[b];v=min(va,vb)
            if va!=v:la=ha=a
            if vb!=v:lb=hb=b
            self.cache[key]=self.node(v,self.op(op,la,lb),self.op(op,ha,hb))
        return self.cache[key]

    def either(self,items):
        result=0
        for item in items:result=self.op('|',result,item)
        return result

    def both(self,items):
        result=1
        for item in items:result=self.op('&',result,item)
        return result

    def restrict(self,u,values,cache=None):
        if cache is None:cache={0:0,1:1}
        if u not in cache:
            v,lo,hi=self.nodes[u]
            cache[u]=self.restrict(hi if values[v] else lo,values,cache) if v in values else self.node(v,self.restrict(lo,values,cache),self.restrict(hi,values,cache))
        return cache[u]

    def members(self,u):
        memo={}
        def count(n,level):
            if n<2:return n*(1<<(32-level))
            key=n,level
            if key not in memo:
                v,lo,hi=self.nodes[n];assert level<=v<32
                memo[key]=(count(lo,v+1)+count(hi,v+1))<<(v-level)
            return memo[key]
        total=count(u,0);assert total<=65536,('Nonfinite/oversized keyword membership',total)
        result=[]
        def visit(n,level,value):
            if n==0:return
            if n==1:
                result.extend(value|suffix for suffix in range(1<<(32-level)));return
            v,lo,hi=self.nodes[n]
            for skipped in range(1<<(v-level)):
                prefix=value|(skipped<<(32-v));visit(lo,v+1,prefix);visit(hi,v+1,prefix|(1<<(31-v)))
        visit(u,0,0);assert len(result)==total
        return sorted(v-(1<<32) if v&(1<<31) else v for v in result)

    def digest(self,u,memo=None):
        if u<2:return 'true' if u else 'false'
        if memo is None:memo={}
        if u not in memo:
            v,lo,hi=self.nodes[u];memo[u]=sha(json.dumps([v,self.digest(lo,memo),self.digest(hi,memo)],separators=(',',':')).encode())
        return memo[u]


@dataclass(frozen=True)
class Integer:
    bits:tuple
    signed:bool=True


def recover(text):
    g=inspect(text,cfg_only=True);fn=g['function'];nodes=list(walk(fn));assert fn.decl.name=='has_sp_key_by_csvid_impl'
    assert fn.decl.storage==['static'] and not fn.decl.quals and fn.decl.type.type.type.names==['int']
    assert not any(isinstance(n,(c_ast.FuncCall,c_ast.StructRef,c_ast.ArrayRef,c_ast.While,c_ast.DoWhile,c_ast.For,c_ast.Typedef,c_ast.PtrDecl)) for n in nodes)
    assert [d.name for d in fn.decl.type.args.params]==['csvid','sp_key']
    types={'csvid':(32,True),'sp_key':(32,True),'cVar1':(8,True),'uVar2':(32,False),'uVar3':(32,False)}
    actual={d.name:' '.join(d.type.type.names) for d in nodes if isinstance(d,c_ast.Decl) and isinstance(d.type,c_ast.TypeDecl)}
    assert actual==dict(csvid='int',sp_key='int',cVar1='char',uVar2='unsigned int',uVar3='unsigned int')
    for n in nodes:
        if isinstance(n,c_ast.Decl) and isinstance(n.type,c_ast.TypeDecl):assert not n.quals and not n.storage and n.init is None
        if isinstance(n,c_ast.Assignment):assert n.op=='=' and isinstance(n.lvalue,c_ast.ID) and n.lvalue.name in ['cVar1','uVar2','uVar3']
    bdd=BDD();params={name:Integer(tuple(bdd.node(base+31-i,0,1) for i in range(32))) for name,base in [('csvid',0),('sp_key',32)]}
    def constant(value,width=32,signed=True):return Integer(tuple((value>>i)&1 for i in range(width)),signed)
    def convert(a,width,signed):return Integer((a.bits+(a.bits[-1] if a.signed else 0,)*max(0,width-len(a.bits)))[:width],signed)
    def promote(a):return convert(a,32,True) if len(a.bits)<32 else a
    def truth(a):return bdd.either(a.bits)
    def boolean(v):return Integer((v,)+(0,)*31)
    def equal(a,b):return bdd.both(bdd.neg(bdd.op('^',x,y)) for x,y in zip(a.bits,b.bits))
    def addition(a,b):
        carry=0;out=[]
        for x,y in zip(a.bits,b.bits):
            xy=bdd.op('^',x,y);out.append(bdd.op('^',xy,carry));carry=bdd.op('|',bdd.op('&',x,y),bdd.op('&',xy,carry))
        return Integer(tuple(out),a.signed)
    def less(a,b):
        aa=list(a.bits);bb=list(b.bits)
        if a.signed:aa[-1]=bdd.neg(aa[-1]);bb[-1]=bdd.neg(bb[-1])
        result=0
        for x,y in zip(aa,bb):result=bdd.op('|',bdd.op('&',bdd.neg(x),y),bdd.op('&',bdd.neg(bdd.op('^',x,y)),result))
        return result
    guard=1;env={};defined={}
    def expression(n):
        if isinstance(n,c_ast.ID):
            if n.name in params:return params[n.name]
            assert bdd.op('&',guard,bdd.neg(defined[n.name]))==0,('Possibly undefined read',n.name,g['offset'](n))
            return env[n.name]
        if isinstance(n,c_ast.Constant):
            assert n.type in ['int','unsigned int'];value=int(re.sub(r'[uUlL]+$','',n.value),0);assert value<1<<32
            assert value<1<<31 or n.type=='unsigned int' or n.value.lower().startswith('0x')
            return constant(value,signed=n.type=='int' and value<1<<31)
        if isinstance(n,c_ast.Cast):
            names=n.to_type.type.type.names;assert names in [['char'],['unsigned','char'],['unsigned','int']]
            return convert(expression(n.expr),8 if 'char' in names else 32,'unsigned' not in names)
        if isinstance(n,c_ast.UnaryOp):
            a=promote(expression(n.expr))
            if n.op=='!':return boolean(bdd.neg(truth(a)))
            if n.op=='-':
                if a.signed:assert bdd.op('&',guard,equal(a,constant(1<<31)))==0,'Possible signed negation overflow'
                return addition(Integer(tuple(bdd.neg(v) for v in a.bits),a.signed),constant(1,signed=a.signed))
            raise AssertionError(n.op)
        assert isinstance(n,c_ast.BinaryOp),type(n).__name__
        a,b=promote(expression(n.left)),promote(expression(n.right));op=n.op
        if op in ['&&','||']:return boolean(bdd.op('&' if op=='&&' else '|',truth(a),truth(b)))
        if op=='<<':
            assert a.bits==(1,)+(0,)*31 and not a.signed
            assert bdd.op('&',guard,bdd.either(b.bits[5:]))==0,'Undefined shift count'
            return Integer(tuple(equal(Integer(b.bits[:5]),constant(i,5)) for i in range(32)),False)
        signed=a.signed and b.signed;a=convert(a,32,signed);b=convert(b,32,signed)
        if op in ['&','|','^']:return Integer(tuple(bdd.op(op,x,y) for x,y in zip(a.bits,b.bits)),signed)
        if op in ['+','-']:
            result=addition(a,b) if op=='+' else addition(a,addition(Integer(tuple(bdd.neg(v) for v in b.bits),signed),constant(1,signed=signed)))
            if signed:
                signs=bdd.op('^',a.bits[-1],b.bits[-1]);overflow=bdd.op('&',bdd.neg(signs) if op=='+' else signs,bdd.op('^',a.bits[-1],result.bits[-1]))
                assert bdd.op('&',guard,overflow)==0,'Possible signed arithmetic overflow'
            return result
        if op in ['==','!=']:return boolean(equal(a,b) if op=='==' else bdd.neg(equal(a,b)))
        if op=='<':return boolean(less(a,b))
        raise AssertionError(op)
    reachable=set();todo=[g['entry']]
    while todo:
        at=todo.pop()
        if at in reachable:continue
        reachable.add(at);todo.extend(g['edges'][at])
    predecessors=collections.Counter(n for at in reachable for n in g['edges'][at]);todo=[at for at in reachable if not predecessors[at]];order=[]
    while todo:
        at=todo.pop();order.append(at)
        for nxt in g['edges'][at]:
            predecessors[nxt]-=1
            if not predecessors[nxt]:todo.append(nxt)
    assert len(order)==len(reachable),'Lookup CFG has a cycle'
    temps=set(types)-set(params);live={};uses={};writes={}
    for at in reversed(order):
        n=g['nodes'][at];writes[at]={n.lvalue.name} if isinstance(n,c_ast.Assignment) else set()
        body=n.rvalue if isinstance(n,c_ast.Assignment) else None if isinstance(n,(c_ast.Case,c_ast.Default,c_ast.Decl)) else n
        uses[at]={v.name for v in walk(body) if isinstance(v,c_ast.ID)}&temps
        live[at]=uses[at]|(set().union(*(live[s] for s in g['edges'][at]))-writes[at]) if g['edges'][at] else uses[at]
    incoming=collections.defaultdict(list);incoming[g['entry']]=[(1,{},{})];result=[0]*32;returns=0;covered=0;processed=0
    switch_offsets={g['offset'](n.cond) for n in nodes if isinstance(n,c_ast.Switch)}
    for at in order:
        incoming_states=incoming.pop(at,[])
        if not incoming_states:continue
        guard=bdd.either(p for p,_,_ in incoming_states)
        if guard==0:continue
        env={};defined={}
        for name in live[at]:
            width,signed=types[name];values=[e.get(name,constant(0,width,signed)) for _,e,_ in incoming_states];defs=[d.get(name,0) for _,_,d in incoming_states]
            if all(v==values[0] for v in values):env[name]=values[0]
            else:env[name]=Integer(tuple(bdd.either(bdd.op('&',p,v.bits[i]) for (p,_,_),v in zip(incoming_states,values)) for i in range(width)),signed)
            defined[name]=defs[0] if all(d==defs[0] for d in defs) else bdd.either(bdd.op('&',p,d) for (p,_,_),d in zip(incoming_states,defs))
        n=g['nodes'][at];edges=g['edges'][at];processed+=1
        if not edges:
            assert n is not None,'Reachable implicit return';value=convert(expression(n),32,True);returns+=1;covered=bdd.op('|',covered,guard)
            result=[bdd.op('|',old,bdd.op('&',guard,v)) for old,v in zip(result,value.bits)];continue
        if isinstance(n,c_ast.Assignment):
            name=n.lvalue.name;env[name]=convert(expression(n.rvalue),*types[name]);defined[name]=1
        elif isinstance(n,c_ast.Decl):assert n.name in temps
        elif isinstance(n,(c_ast.Case,c_ast.Default)) or n is None:pass
        elif g['offset'](n) in switch_offsets:
            selector=expression(n);tests=[]
            for nxt in edges:
                case=g['nodes'][nxt]
                if isinstance(case,c_ast.Case):tests.append((nxt,equal(selector,expression(case.expr))))
            unmatched=bdd.neg(bdd.either(v for _,v in tests));tests.extend((nxt,unmatched) for nxt in edges if not isinstance(g['nodes'][nxt],c_ast.Case))
            for nxt,test in tests:incoming[nxt].append((bdd.op('&',guard,test),env,defined))
            continue
        else:
            assert len(edges)==2;test=truth(expression(n));incoming[edges[0]].append((bdd.op('&',guard,test),env,defined));incoming[edges[1]].append((bdd.op('&',guard,bdd.neg(test)),env,defined));continue
        assert len(edges)==1;incoming[edges[0]].append((guard,env,defined))
    assert covered==1 and not incoming
    table={};bits=[];memo={}
    for i,output in enumerate(result):
        membership=bdd.restrict(output,{32+31-j:int(i==j) for j in range(32)})
        assert output==bdd.op('&',params['sp_key'].bits[i],membership),('Keyword bits interact',i)
        members=bdd.members(membership)
        for card in members:table[card]=table.get(card,0)|(1<<i)
        bits.append(dict(bit=i,members=len(members),membership_sha256=bdd.digest(membership,memo),output_sha256=bdd.digest(output,memo)))
    rows=[dict(csvid=k,keywords=v) for k,v in sorted(table.items())]
    return dict(source_sha256=sha(text.encode()),table=rows,counts=dict(cards=len(rows),keyword_bits=sum(bool(v['members']) for v in bits),reachable_cfg_nodes=len(reachable),processed_cfg_nodes=processed,return_nodes=returns,bdd_nodes=len(bdd.nodes)),bits=bits,table_sha256=sha(json.dumps(rows,separators=(',',':')).encode()),scope='Exact admitted acyclic C CFG interpretation over signed int32 card/request inputs, unsigned int32 accumulators and signed 8-bit char. All reached local reads are defined and shifts are in range. Canonical BDD identity proves every output bit is its requested bit AND finite card membership; all other card IDs and all other keyword bits return zero. No calls, memory or parameter writes admitted.')
