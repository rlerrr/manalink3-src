"""Stable scalar roles and capture-free native-vocabulary local spellings.

Names are supported by source roles and native local vocabulary. This does
not assert that optimized native and reconstructed C variables are identical.
"""
import collections,re
from pycparser import c_ast
from canonicalize_helpers import mask
from source_record_flow import parse
from verify_source_spelling_edits import substitute

CALLS={'get_power':['power','pow'], 'get_toughness':['toughness','tgh'], 'get_cmc':['cmc','mana_value'],'get_id':['csvid','csv_id','card_id'],'get_internal_card_id':['iid','internal_card_id'],'dialog_impl':['choice','selected_choice'],'do_dialog':['choice'], 'count_deck':['deck_size','deck_count','count','num_cards'],'count_graveyard':['graveyard_count','gy_count','count','num_cards'],'count_graveyard_by_type':['count','num_cards'],'count_graveyard_by_subtype':['count','num_cards'],'count_subtype':['count','num_cards'],'count_permanents_by_type':['count','num_cards'],'count_counters':['count','num_counters'],'get_sleighted_color_test':['color_test','color'],'get_sleighted_protection':['protection'],'get_hacked_subtype':['subtype'], 'new_sacrifice':['sac','sacrificed'], 'mark_sacrifice':['sac','sacrificed'], 'charge_mana_multi':['charged','paid']}
FIELDS={'damage_target_card':['t_card','target_card','enchanted_card'],'damage_target_player':['t_player','target_player','enchanted_player'],'parent_controller':['parent_controller','parent_player'],'parent_card':['parent_card'],'number_of_targets':['num_targets','target_count']}
def walk(n):
 if n is None:return
 yield n
 for _,c in n.children():yield from walk(c)
def plain(n):
 while isinstance(n,c_ast.Cast):n=n.expr
 return n

def integer_literal(node):
 if isinstance(node,c_ast.Constant):
  try:return int(re.sub('[uUlL]+$','',node.value),0)
  except ValueError:return None
 if isinstance(node,c_ast.UnaryOp) and node.op in ['+','-']:
  value=integer_literal(node.expr)
  if value is not None:return value if node.op=='+' else -value
 return None

def candidates(text):
 fn,offset=parse(text);code=mask(text);nodes=list(walk(fn));decls=collections.defaultdict(list);auto={id(d) for d in fn.body.block_items or [] if isinstance(d,c_ast.Decl)}
 for d in nodes:
  if isinstance(d,c_ast.Decl):decls[d.name].append(d)
 escaped={d.expr.name for d in nodes if isinstance(d,c_ast.UnaryOp) and d.op=='&' and isinstance(d.expr,c_ast.ID)};out=[]
 for name,ds in decls.items():
  if name is None or not re.fullmatch(r'[A-Za-z]*Var\d+|[A-Za-z]*Stack_[a-fA-F0-9]+|result',name) or len(ds)!=1 or name in escaped:continue
  d=ds[0]
  if not (id(d) in auto and not d.storage and not d.quals and d.init is None and isinstance(d.type,c_ast.TypeDecl) and not d.type.quals and isinstance(d.type.type,c_ast.IdentifierType) and d.type.type.names==['int']):continue
  if re.search(r'\b'+re.escape(name)+r'\b',code[:offset(d)]):continue
  if re.search(r'(?:\.|->)\s*'+re.escape(name)+r'\b|\b(?:struct|union|enum|goto)\s+'+re.escape(name)+r'\b|\b'+re.escape(name)+r'\s*:',code):continue
  if any(isinstance(n,c_ast.StructRef) and n.field.name==name for n in nodes):continue
  writes=[n for n in nodes if isinstance(n,c_ast.Assignment) and isinstance(n.lvalue,c_ast.ID) and n.lvalue.name==name];steps=[n for n in nodes if isinstance(n,c_ast.UnaryOp) and n.op in ['p++','p--','++','--'] and isinstance(n.expr,c_ast.ID) and n.expr.name==name]
  if not writes:continue
  rhs=[plain(w.rvalue) for w in writes];options=[];role=None
  if not steps and all(w.op=='=' for w in writes) and all(isinstance(v,c_ast.FuncCall) and isinstance(v.name,c_ast.ID) for v in rhs) and len({v.name.name for v in rhs})==1:
   callee=rhs[0].name.name;options=CALLS.get(callee, ['count','qty','num'] if callee.startswith('count_') else []);role='call:'+callee
  if not options and not steps and all(w.op=='=' for w in writes) and all(isinstance(v,c_ast.StructRef) for v in rhs) and len({v.field.name for v in rhs})==1:
   field=rhs[0].field.name;options=FIELDS.get(field,[]);role='field:'+field
  if not options:
   loop_use=any(isinstance(n,(c_ast.While,c_ast.DoWhile,c_ast.For)) and n.cond is not None and any(isinstance(b,c_ast.BinaryOp) and b.op in ['<','<=','>','>=','!='] and any(isinstance(t,c_ast.ID) and t.name==name for t in [b.left,b.right]) for b in walk(n.cond)) for n in nodes)
   inits=0;updates=len(steps);valid=True
   for w,v in zip(writes,rhs):
    ids={n.name for n in walk(v) if isinstance(n,c_ast.ID)}
    if w.op=='=' and name not in ids and not any((isinstance(n,(c_ast.FuncCall,c_ast.Assignment)) or (isinstance(n,c_ast.UnaryOp) and integer_literal(n) is None)) for n in walk(v)):inits+=1
    elif w.op=='=' and isinstance(v,c_ast.BinaryOp) and v.op in ['+','-'] and isinstance(v.left,c_ast.ID) and v.left.name==name and integer_literal(v.right) in [-1,1]:updates+=1
    elif w.op in ['+=','-='] and integer_literal(v) in [-1,1]:updates+=1
    else:valid=False
   if loop_use and inits and updates and valid:
    card_index=any(isinstance(n,c_ast.FuncCall) and isinstance(n.name,c_ast.ID) and n.name.name in ['get_card_instance','in_play','is_what','kill_card','get_id'] and n.args is not None and len(n.args.exprs)>1 and isinstance(n.args.exprs[1],c_ast.ID) and n.args.exprs[1].name==name for n in nodes)
    options=['i']+(['c','current_card','which_card'] if card_index else []);role='loop_index'
  if options:out.append(dict(old_name=name,options=options,role=role,writes=len(writes),unary_updates=len(steps)))
 return out

def project(before,selected):
 facts={v['old_name']:v for v in candidates(before)};code=mask(before);mapping={};count=0
 for item in selected:
  old,new=item['old_name'],item['new_name'];assert old not in mapping
  fact=facts[old]
  assert item['role']==fact['role'] and new in fact['options']
  assert not re.search(r'\b'+re.escape(new)+r'\b',code) and new not in mapping.values()
  mapping[old]=new;count+=len(re.findall(r'\b'+re.escape(old)+r'\b',code))
 after=substitute(before,mapping)
 assert before.count('\n')==after.count('\n')
 return after,count
