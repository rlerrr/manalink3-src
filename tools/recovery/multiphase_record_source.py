"""Restore typed records using source CFG constructor ownership, never stack unions."""
import re
from canonicalize_helpers import mask
from source_record_flow import inspect,UNION_PATTERN,parse
from disentangle_union_source import TYPES,literal
from pycparser import c_ast


def rewrite_fields(text,storage,role,name,layouts):
    fields=layouts[role];changes=[];code=mask(text)
    if role!='test':
        pattern=re.compile(r'(?m)^( +)'+storage+r'\.test\.message\[(0x[0-9a-fA-F]+|\d+)\]\s*=\s*([^;]+);\n?')
        writes=list(pattern.finditer(text));done=set()
        for index,m in enumerate(writes):
            if index in done:continue
            off=int(m[2],0);field=next((f for f in fields if f['offset']<=off<f['offset']+f['size']),None);assert field,(role,off,m[0]);size=field['size']
            if size==1:old=m[0];expression=m[3];group=[m];kind='byte_field'
            else:
                assert size in [2,4] and off==field['offset'];group=writes[index:index+size];assert len(group)==size
                assert all(int(g[2],0)==off+j and g[1]==m[1] and (j==0 or group[j-1].end()==g.start()) for j,g in enumerate(group)),('noncontiguous',name,group)
                old=''.join(g[0] for g in group)
                value=sum((literal(g[3].strip())&255)<<(8*j) for j,g in enumerate(group));value=value-(1<<(size*8)) if value>>(size*8-1) else value;expression=str(value);kind='complete_little_endian_field'
            new=m[1]+name+'.'+field['name']+' = '+expression+';'+('\n' if group[-1][0].endswith('\n') else '')
            changes.append(dict(before=old,after=new,kind=kind,offset=off,size=size));done.update(range(index,index+len(group)))
        for change in changes:assert text.count(change['before'])==1;text=text.replace(change['before'],change['after'])
    pattern=re.compile(r'\b'+storage+r'\.('+'|'.join(r for r in TYPES if r!=role)+r')\.([A-Za-z_]\w*)')
    def field(m):
        src=next(v for v in layouts[m[1]] if v['name']==m[2]);dest=[v for v in fields if (v['offset'],v['size'])==(src['offset'],src['size'])];assert len(dest)==1,(role,m[0],src)
        change=dict(before=m[0],after=name+'.'+dest[0]['name'],kind='same_offset_typed_field',offset=src['offset'],size=src['size']);changes.append(change);return change['after']
    text=pattern.sub(field,text);text=re.sub(r'\b'+storage+r'\.'+role+r'\b',name,text)
    assert not re.search(r'\b'+storage+r'\b',mask(text));return text,changes


def project(before,native,layouts):
    flow=inspect(before);assert not flow['unresolved'] and not any(a['possibly_uninitialized'] for a in flow['accesses']);union=UNION_PATTERN.search(before);storage=flow['storage']
    groups=flow['groups'];roles={g['constructors'][0]['role'] for g in groups};frame=set.intersection(*[{v['frame_offset'] for v in native['variables'] if v['kind']=='DW_TAG_structure_type' and v['bytes']==TYPES[r][1]} for r in roles]);assert len(frame)==1;frame=frame.pop()
    available={r:[v for v in native['variables'] if v['kind']=='DW_TAG_structure_type' and v['bytes']==TYPES[r][1] and v['frame_offset']==frame] for r in roles}
    code=mask(before);without=code[:union.start()]+code[union.end():];without=re.sub(r'\b'+storage+r'\.(?:test|target|token)\b','',without);used=set(re.findall(r'\b[A-Za-z_]\w*\b',without));bindings=[];group_binding={}
    for role in sorted(roles):
        owned=sorted([g for g in groups if g['constructors'][0]['role']==role],key=lambda g:g['constructors'][0]['offset']);variables=available[role];assert len(variables)==len(owned) or len(variables)==1,(role,len(variables),len(owned))
        for index,v in enumerate(variables):
            selected=owned if len(variables)==1 else [owned[index]];name=v['name'];base=re.sub(r'\d+$','',name);suffix=2
            while name in used:name=base+str(suffix);suffix+=1
            used.add(name);number=len(bindings);item=dict(role=role,name=name,native_name=v['name'],native_variable_die=v['variable_die'],groups=[g['id'] for g in selected],temporary='recovery_record_'+str(number));bindings.append(item)
            for g in selected:group_binding[g['id']]=number
    edits=[]
    for access in flow['accesses']:
        at=access['offset'];prefix=storage+'.'+access['member'];assert before[at:at+len(prefix)]==prefix;item=bindings[group_binding[access['group']]];edits.append((at,at+len(prefix),item['temporary']+'.'+access['member']))
    assert len({a for a,b,s in edits})==len(edits)
    text=before
    for a,b,replacement in sorted(edits,reverse=True):text=text[:a]+replacement+text[b:]
    union=UNION_PATTERN.search(text);assert union;text=text[:union.start()]+text[union.end():];field_changes=[]
    for item in bindings:
        text,changes=rewrite_fields(text,item['temporary'],item['role'],item['name'],layouts);field_changes.extend(changes)
    assert not re.search(r'\b'+storage+r'\b',mask(text))
    # Syntactic pointer aliases require a lifetime covering later consumers.
    # Keep such records at function scope; ordinary immediate call arguments
    # do not require extending beyond the compound containing all direct uses.
    tree,_=parse(text);names={v['name'] for v in bindings};alias_records=set()
    def walk(node,parents=()):
        if isinstance(node,c_ast.UnaryOp) and node.op=='&' and isinstance(node.expr,c_ast.ID) and node.expr.name in names:
            wrappers=[];call=None
            for p in reversed(parents):
                if isinstance(p,c_ast.FuncCall):call=p;break
                if isinstance(p,(c_ast.Compound,c_ast.FuncDef)):break
                wrappers.append(type(p).__name__)
            if call is None or any(p not in ['Cast','ExprList'] for p in wrappers):alias_records.add(node.expr.name)
        for _,child in node.children():walk(child,parents+(node,))
    walk(tree)
    code=mask(text);stack=[];paths={};root=None
    for m in re.finditer(r'[{}]|\b(?:'+'|'.join(re.escape(n) for n in names)+r')\b',code):
        if m[0]=='{':stack.append(m.start());root=root if root is not None else m.start()
        elif m[0]=='}':assert stack;stack.pop()
        else:assert stack;paths.setdefault(m[0],[]).append(list(stack))
    declarations={}
    for item in bindings:
        name=item['name'];common=paths[name][0]
        for path in paths[name][1:]:common=common[:next((i for i,(a,b) in enumerate(zip(common,path)) if a!=b),min(len(common),len(path)))]
        at=root if name in alias_records else common[-1];line=code[code.rfind('\n',0,at)+1:at];indent=re.match(r' *',line)[0]+'  ';declarations.setdefault(at,[]).append(indent+TYPES[item['role']][0]+' '+name+';');item['enclosing_function_lifetime']=name in alias_records
    for at,decls in sorted(declarations.items(),reverse=True):text=text[:at+1]+'\n'+'\n'.join(decls)+text[at+1:]
    assert re.findall(r'"(?:\\.|[^"\\])*"',before)==re.findall(r'"(?:\\.|[^"\\])*"',text)
    # Removing a declaration should not manufacture large blank runs.
    text=re.sub(r'\n(?:[ \t]*\n){2,}','\n\n',text)
    return text,dict(source_flow=flow,restored_locals=bindings,field_rewrites=field_changes,declaration_scopes=[dict(offset=at,declarations=decls) for at,decls in declarations.items()])
