"""Recover the finite activation exclusion list and small ability flag queries."""
import copy,collections,re
from pycparser import c_ast,c_generator
from canonicalize_helpers import mask,sha
from source_record_flow import parse
from call_condition_source import walk

GEN=c_generator.CGenerator();U32=(1<<32)-1
EXPECTED={'has_an_activated_ability': '0cbd209ec48fd8bab8e55590ab5dfef3b45ec5fca670e2d108133d941d22f359', 'is_humiliated_by_instance': '1d66b900e50fad55677931d9efd374bd2e07455d7b9c63bc9361c1ca1107a6d8', 'can_use_activated_abilities_by_instance': '2821b9817f01f10390656c85f23c97507e6eacad3564227a82e261a6add3191b', 'can_use_activated_abilities': 'a63cdfeeb8098780ec1adb4c31f2dc54ebc5cf089c00b61b0555192830e9d632', 'can_produce_mana': 'ed847c9c5af1791cb4bd71348bd2e6abf4cd5ead9b37135e7739f14046765272', 'get_special_abilities_by_instance': 'd5fe7c237cbc9f7af95212dc13c6b52c79f37fba17740ff634bc1e11f38ef3a2', 'remove_abilities': 'ac3ace619e619e6a660a43c8d8e064c8a24f88a4228659888dab6aa07ac38b4d'}


def canonical(text):
    fn,_=parse(text)
    def visit(n):
        if isinstance(n,c_ast.ID) and n.name=='NULL':return c_ast.Constant('int','0')
        if isinstance(n,c_ast.Cast) and GEN.visit(n.to_type)=='void *' and isinstance(n.expr,c_ast.Constant) and n.expr.value=='0':return c_ast.Constant('int','0')
        out=copy.copy(n)
        for field in n.__slots__:
            if field in ['coord','__weakref__']:continue
            v=getattr(n,field)
            if isinstance(v,c_ast.Node):setattr(out,field,visit(v))
            elif isinstance(v,list):setattr(out,field,[visit(x) if isinstance(x,c_ast.Node) else x for x in v])
        return out
    return sha(GEN.visit(visit(fn)).encode())


def literal(n):
    assert isinstance(n,c_ast.Constant) and n.type in ['int','unsigned int'];v=int(re.sub(r'[uUlL]+$','',n.value),0)
    assert 0<=v<=U32
    return v,n.type=='int' and v<1<<31


def excluded_cards(before):
    assert canonical(before)==EXPECTED['has_an_activated_ability'];fn,_=parse(before);outer=fn.body.block_items[-2];inner=outer.iftrue.block_items[-1];tree=inner.iftrue.block_items[1]
    assert GEN.visit(inner.cond)=='uVar5 != 0x2baa'
    nodes=list(walk(tree));assert {n.name for n in nodes if isinstance(n,c_ast.ID)}=={'uVar5','cVar2','iVar1'}
    assert all(isinstance(n,(c_ast.If,c_ast.Compound,c_ast.Return,c_ast.BinaryOp,c_ast.ID,c_ast.Constant)) for n in nodes)
    assert all(isinstance(n.expr,c_ast.Constant) and n.expr.value=='0' for n in nodes if isinstance(n,c_ast.Return))
    # Dependency support for each bit proves that the full 32-bit load's upper
    # half cannot affect any decision, even its masked subtraction expression.
    # Known bits remove dependencies through masks. Carry flows only upward.
    def support(n):
        if isinstance(n,c_ast.Constant):
            v,_=literal(n);return tuple(((v>>i)&1,0) for i in range(32))
        if isinstance(n,c_ast.ID):
            if n.name=='iVar1':return tuple((None,1<<i) for i in range(32))
            if n.name=='uVar5':return tuple((None,1<<i) if i<16 else (0,0) for i in range(32))
            assert n.name=='cVar2';return tuple((None,1<<min(i,7)) for i in range(32))
        assert isinstance(n,c_ast.BinaryOp);a,b=support(n.left),support(n.right);op=n.op
        if op in ['&','|','^']:
            out=[]
            for (x,dx),(y,dy) in zip(a,b):
                if x is not None and y is not None:out.append(({'&':lambda:x&y,'|':lambda:x|y,'^':lambda:x^y}[op](),0))
                elif op=='&' and (x==0 or y==0):out.append((0,0))
                elif op=='|' and (x==1 or y==1):out.append((1,0))
                else:out.append((None,dx|dy))
            return tuple(out)
        if op in ['+','-']:
            deps=0;out=[]
            for (_,x),(_,y) in zip(a,b):deps|=x|y;out.append((None,deps))
            return tuple(out)
        if op=='<<':
            assert all(v==0 for v,_ in b[5:]),'Unbounded shift count'
            deps=0
            for _,d in (*a,*b):deps|=d
            return ((None,deps),)*32
        assert op in ['==','!=','<','<=','>','>=','&&','||'];deps=0
        for _,d in (*a,*b):deps|=d
        return ((None,deps),)+((0,0),)*31
    conditions=[n.cond for n in nodes if isinstance(n,c_ast.If)]
    for n in conditions:assert not any(deps&0xffff0000 for _,deps in support(n)),'Upper id word observed'
    def expression(n,word):
        if isinstance(n,c_ast.Constant):return literal(n)
        if isinstance(n,c_ast.ID):
            if n.name=='iVar1':return word&U32,True
            if n.name=='uVar5':return word&0xffff,True
            assert n.name=='cVar2';v=word&255;return (v if v<128 else v-256)&U32,True
        assert isinstance(n,c_ast.BinaryOp);a,sa=expression(n.left,word);op=n.op
        if op=='&&' and not a:return 0,True
        if op=='||' and a:return 1,True
        b,sb=expression(n.right,word);signed=sa and sb
        if op in ['==','!=','<','<=','>','>=']:
            x=a-(1<<32) if signed and a&(1<<31) else a;y=b-(1<<32) if signed and b&(1<<31) else b
            return int({'==':lambda:x==y,'!=':lambda:x!=y,'<':lambda:x<y,'<=':lambda:x<=y,'>':lambda:x>y,'>=':lambda:x>=y}[op]()),True
        if op=='&&':return int(bool(a) and bool(b)),True
        if op=='||':return int(bool(a) or bool(b)),True
        if op=='<<':assert 0<=b<32;return (a<<b)&U32,sa
        if op=='+':return (a+b)&U32,signed
        if op=='-':return (a-b)&U32,signed
        if op=='&':return a&b,signed
        if op=='|':return a|b,signed
        if op=='^':return a^b,signed
        raise AssertionError(op)
    def excluded(n,word):
        if n is None:return False
        if isinstance(n,c_ast.Compound):return any(excluded(v,word) for v in n.block_items or [])
        if isinstance(n,c_ast.Return):return True
        assert isinstance(n,c_ast.If);return excluded(n.iftrue if expression(n.cond,word)[0] else n.iffalse,word)
    cards=[v for v in range(65536) if v==0x2baa or excluded(tree,v)]
    return cards,dict(exhaustive_uint16_inputs=65536,upper_word_unobserved=True,predicate_conditions=len(conditions),excluded_cards=len(cards),semantics='32-bit native integer bit tests, including signed-left-shift bit31 idioms; the resulting switch has no such shifts. All decision dependencies are confined to the low16 bits, which are exhaustively classified. Native comparisons separately validate the complete helper and opaque rules-engine call trace.')


BODIES={
'is_humiliated_by_instance':'''int is_humiliated_by_instance(card_instance_t *instance, ability_loss_t mode)
{
  int flags = instance->targets[17].card;
  return flags > 0 ? flags & mode : 0;
}''',
'can_use_activated_abilities_by_instance':'''int can_use_activated_abilities_by_instance(card_instance_t *instance)
{
  int mode = instance->targets[17].card;
  return mode <= 0 || !(mode & AL_NM_ACT);
}''',
'can_use_activated_abilities':'''int can_use_activated_abilities(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  int mode = instance->targets[17].card;
  return mode <= 0 || !(mode & AL_NM_ACT);
}''',
'can_produce_mana':'''int can_produce_mana(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  int mode = instance->targets[17].card;
  int result = mode <= 0 || !(mode & AL_MANA);
  if (player == AI && result)
  {
    result = !(instance->ext->special_flags & SF_MANA_PRODUCER_DISABLED_FOR_AI);
  }
  return result;
}''',
'get_special_abilities_by_instance':'''int get_special_abilities_by_instance(card_instance_t *instance)
{
  unsigned int keyword = instance->ext->sp_keywords;
  if (instance->state & STATE_VIGILANCE)
  {
    keyword |= SP_KEYWORD_VIGILANCE;
    instance->ext->sp_keywords = keyword;
  }
  int mode = instance->targets[17].card;
  if (mode > 0)
  {
    if (mode & AL_KEY)
      keyword = 0;
    if (mode & AL_INDESTRUCTIBLE)
      keyword |= SP_KEYWORD_INDESTRUCTIBLE;
  }
  return keyword;
}''',
'remove_abilities':'''void remove_abilities(int player, int card, event_t event, ability_loss_t mode)
{
  card_instance_t *instance = in_play(affected_card_controller, affected_card);
  if (instance != NULL)
  {
    int flags = instance->targets[17].card;
    if (flags >= 0)
      mode |= flags;
    instance->targets[17].card = mode;
  }
}'''
}


def project(before,name,cards=None,card_names=None,*,expanded=False):
    assert canonical(before)==EXPECTED[name]
    if name!='has_an_activated_ability':return BODIES[name].replace('NULL','((void *)0)') if expanded else BODIES[name]
    found,_=excluded_cards(before);assert found==cards and set(card_names)==set(cards)|{0x36eb}
    lines=['int has_an_activated_ability(iid_t iid)','{','  if (!(cards_data[iid].type & (TYPE_LAND | TYPE_CREATURE | TYPE_ENCHANTMENT | TYPE_ARTIFACT)))','    return 0;','','  int csvid = cards_data[iid].id;','  switch (csvid)','  {']
    for v in cards:lines.append('  case '+card_names[v]+':')
    lines += ['    return 0;','  default:','    break;','  }','','  if ((cards_data[iid].extra_ability & EA_MANA_SOURCE) || csvid == '+card_names[0x36eb]+')','    return 2;','  if (cards_data[iid].extra_ability & (EA_ACT_ABILITY | EA_ACT_INTERRUPT))','    return 1;','  return ((unsigned int)check_card_for_rules_engine(iid) >> 12) & 1;','}']
    return '\n'.join(lines)
