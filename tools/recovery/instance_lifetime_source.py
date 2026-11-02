"""Names for nonescaping card-instance pointer lifetimes, with exact C types."""
from pycparser import c_ast
from scalar_lifetime_source import analyze as analyze_values,project as project_values,name_available as available
from role_local_source import plain
from canonicalize_helpers import mask


def instance_type(d):
    t=d.type
    if isinstance(t,c_ast.PtrDecl) and not t.quals and isinstance(t.type,c_ast.TypeDecl) and not t.type.quals and isinstance(t.type.type,c_ast.IdentifierType) and t.type.type.names==['card_instance_t']:
        return 'card_instance_t *'


def instance_role(*,definitions,nodes,**unused):
    if any(isinstance(n,c_ast.Typedef) and n.name=='card_instance_t' for n in nodes):return None
    roles=[];options=None
    for kind,n in definitions:
        if kind!='plain':return None
        value=plain(n.rvalue)
        if not isinstance(value,c_ast.FuncCall) or not isinstance(value.name,c_ast.ID) or value.name.name not in ['get_card_instance','in_play'] or value.args is None or len(value.args.exprs)!=2:return None
        player,card=map(plain,value.args.exprs)
        if isinstance(player,c_ast.ID) and player.name=='player' and isinstance(card,c_ast.ID) and card.name=='card':
            role='self_instance';choices=['instance','card_instance','self']
        elif isinstance(player,c_ast.StructRef) and isinstance(card,c_ast.StructRef) and isinstance(player.name,c_ast.ID) and isinstance(card.name,c_ast.ID) and player.name.name==card.name.name and (player.field.name,card.field.name)==('parent_controller','parent_card'):
            role='parent_instance';choices=['parent','parent_instance']
        else:
            role='other_instance';choices=['target_instance','tgt_instance','other_instance']
        # A generic native instance name can describe any card-instance record.
        choices=list(dict.fromkeys([*choices,'instance','card_instance']))
        options=choices if options is None else [v for v in options if v in choices];roles.append(role)
    return dict(options=options,roles=sorted(set(roles))) if options else None


def analyze(text):return [g for g in analyze_values(text,calls={},component_role=instance_role,declaration_type=instance_type) if 'component_role' in g]


def name_available(text,name):return available(text,name,allow_fields=True)


def project(before,selected):return project_values(before,selected,calls={},allow_field_names=True,component_role=instance_role,declaration_type=instance_type)


def probe_definition(before,row):
    """Prove that each original pointer and the added spelling have one C type."""
    code=mask(before);edits=[];seen=set()
    for rename in row['renames']:
        g=rename['expanded_group'];name=g['old_name']
        if name in seen:continue
        seen.add(name);start=g['definition_offsets'][0];end=code.index(';',start)+1
        condition=f'__builtin_types_compatible_p(__typeof__(&{name}), card_instance_t **) && __builtin_types_compatible_p(card_instance_t, struct card_instance_struct) && sizeof(card_instance_t) == 300 && sizeof(void *) == 4'
        edits.append((start,end,'{ _Static_assert('+condition+', "instance_pointer_type"); '+before[start:end]+' }'))
    for a,b,s in reversed(sorted(edits)):before=before[:a]+s+before[b:]
    return before
