"""Player-valued int components supported by their origins and player uses."""
from pycparser import c_ast
from scalar_lifetime_source import analyze as values,project as split
from getter_lifetime_source import GETTER_CALLS,name_available
from role_local_source import plain,integer_literal

PLAYER_CALLS={'get_card_instance','in_play','in_hand','is_what','kill_card','get_id','get_power','get_toughness','gain_life','lose_life','draw_cards','count_deck','count_graveyard','get_owner'}
PLAYER_ARRAYS={'active_cards_count','hand_count','life','deck_ptr','graveyard_ptr','rfg_ptr','mana_pool','player_bits'}
PLAYER_FIELDS={'player','damage_target_player','parent_controller','controller','owner'}


def role(*,name,definitions,references,function,nodes,offset):
    params={d.name for d in function.decl.type.args.params if isinstance(d,c_ast.Decl) and isinstance(d.type,c_ast.TypeDecl) and isinstance(d.type.type,c_ast.IdentifierType) and d.type.type.names==['int']} if function.decl.type.args else set()
    roots={'current_turn'}|({'player'} if 'player' in params else set())
    def origin(expr):
        expr=plain(expr)
        if integer_literal(expr) in [0,1]:return 'player_literal'
        if isinstance(expr,c_ast.ID) and expr.name in roots:return expr.name
        if isinstance(expr,c_ast.StructRef) and expr.field.name in PLAYER_FIELDS:return 'field:'+expr.field.name
        if isinstance(expr,c_ast.FuncCall) and isinstance(expr.name,c_ast.ID) and expr.name.name=='get_owner':return 'owner_lookup'
        if isinstance(expr,c_ast.BinaryOp) and expr.op=='-' and integer_literal(expr.left)==1:
            right=plain(expr.right)
            if isinstance(right,c_ast.ID) and right.name==name:return 'other_component_player'
            r=origin(right)
            if r:return 'other:'+r
        if isinstance(expr,c_ast.TernaryOp):
            a,b=origin(expr.iftrue),origin(expr.iffalse)
            if a and b:return 'choice:'+a+':'+b
        return None
    origins=[]
    for kind,node in definitions:
        if kind!='plain':return None
        o=origin(node.rvalue)
        if o is None:return None
        origins.append(dict(offset=offset(node.lvalue),origin=o))
    uses=[]
    for node in nodes:
        if isinstance(node,c_ast.FuncCall) and isinstance(node.name,c_ast.ID) and node.name.name in PLAYER_CALLS and node.args is not None and node.args.exprs:
            arg=node.args.exprs[0]
            if isinstance(arg,c_ast.ID) and arg.name==name and offset(arg) in references:uses.append(dict(offset=offset(arg),kind='player_argument',callee=node.name.name))
        if isinstance(node,c_ast.ArrayRef) and isinstance(node.name,c_ast.ID) and node.name.name in PLAYER_ARRAYS and isinstance(node.subscript,c_ast.ID) and node.subscript.name==name and offset(node.subscript) in references:uses.append(dict(offset=offset(node.subscript),kind='player_array_index',array=node.name.name))
    if not uses:return None
    options=['p','current_player','which_player']
    if all(o['origin']=='other:player' for o in origins):options=['opponent','other_player',*options]
    if all(o['origin']=='owner_lookup' for o in origins):options=['owner',*options]
    return dict(roles=['player_selection'],options=options,origins=origins,player_uses=sorted(uses,key=lambda v:v['offset']))


def analyze(text):return [g for g in values(text,calls=GETTER_CALLS,component_role=role) if g['roles']==['player_selection']]


def project(before,selected):return split(before,selected,calls=GETTER_CALLS,allow_field_names=True,component_role=role)
