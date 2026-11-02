"""Split two-initializer synthetic unions using native member offsets and names."""
import ast,re
from canonicalize_helpers import mask

PATTERN=re.compile(r'  /\* DWARF locals share this frame offset; keep every typed view\. \*/\n  union\n  \{\n(?P<members>.*?)\n  \} (?P<storage>recovered_stack_\w+);\n',re.S)
TYPES={'target':('target_definition_t',84),'test':('test_definition_t',216),'token':('token_generation_t',76)}
CONSTRUCTORS={'target':['default_target_definition','base_target_definition'],
              'test':['default_test_definition','new_default_test_definition'],
              'token':['default_token_definition','copy_token_definition']}


def literal(expr):
    if expr.startswith("'"):
        v=ast.literal_eval(expr);assert isinstance(v,str) and len(v)==1;return ord(v)
    return int(expr,0)


def project(before,native,layouts):
    matches=list(PATTERN.finditer(before));assert len(matches)==1;union=matches[0];storage=union['storage'];code=mask(before)
    roles=[role for role,(ctype,size) in TYPES.items() if re.search(r'\b'+ctype+r'\s+'+role+r';',union['members'])]
    assert len(roles)==2 and all(ctype+' ' not in union['members'] for role,(ctype,size) in TYPES.items() if role not in roles)
    variables={};names={};facts=[];candidates_by_role={}
    for role in roles:
        candidates_by_role[role]=[v for v in native['variables'] if v['kind']=='DW_TAG_structure_type' and v['bytes']==TYPES[role][1]]
    common=set.intersection(*[{v['frame_offset'] for v in group} for group in candidates_by_role.values()]);assert len(common)==1,common
    common_frame=common.pop();phases=[]
    for role in roles:
        kind,size=TYPES[role];candidates=[v for v in candidates_by_role[role] if v['frame_offset']==common_frame]
        assert len(candidates)==1,(role,len(candidates));v=candidates[0];name=v['name'];assert re.fullmatch(r'[A-Za-z_]\w*',name)
        without=code[:union.start()]+code[union.end():];without=re.sub(r'\b'+storage+r'\.(?:target|test|token)\b','',without)
        if re.search(r'\b'+name+r'\b',without):
            suffix=2
            while re.search(r'\b'+name+str(suffix)+r'\b',without):suffix+=1
            name+=str(suffix)
        variables[role]=v;names[role]=name
        constructors=[]
        for callee in CONSTRUCTORS[role]:
            constructors.extend(re.finditer(r'\b'+callee+r'\([^;]*?&'+storage+r'\.'+role+r'\b[^;]*?\);',code))
        assert len(constructors)==1,(role,len(constructors));phases.append((constructors[0].start(),role));facts.append(dict(role=role,name=name,native_name=v['name'],initializer=code[constructors[0].start():constructors[0].end()],native_variable_die=v['variable_die']))
    text=before;edits=[]
    phases.sort()
    for index,(start,active) in reversed(list(enumerate(phases))):
        if active=='test':continue
        end=phases[index+1][0] if index+1<len(phases) else len(before)
        region=text[start:end];fields=layouts[active];changes=[];phase_edits=[]
        bytepat=re.compile(r'(?m)^( +)'+storage+r'\.test\.message\[(0x[0-9a-fA-F]+|\d+)\]\s*=\s*([^;]+);\n?')
        bytewrites=list(bytepat.finditer(region));done=set()
        for index,m in enumerate(bytewrites):
            if index in done:continue
            off=int(m[2],0);field=next((f for f in fields if f['offset']<=off<f['offset']+f['size']),None);assert field,(off,m[0])
            if field['size']==1:
                phase_edits.append(dict(before=m[0],after=m[1]+names[active]+'.'+field['name']+' = '+m[3]+';'+('\n' if m[0].endswith('\n') else ''),kind='byte_field',offset=off,size=1));done.add(index);continue
            assert off==field['offset'] and field['size'] in [2,4],('partial field',field,m[0])
            group=bytewrites[index:index+field['size']];assert len(group)==field['size']
            assert all(int(g[2],0)==off+j and g[1]==m[1] and (j==0 or group[j-1].end()==g.start()) for j,g in enumerate(group))
            try:
                value=sum((literal(g[3].strip())&255)<<(j*8) for j,g in enumerate(group));value=value-(1<<(field['size']*8)) if value>>(field['size']*8-1) else value;expression=str(value);kind='complete_little_endian_field'
            except (ValueError,SyntaxError):
                first=re.fullmatch(r'\(uint8_t\)([A-Za-z_]\w*)',re.sub(r'\s+','',group[0][3]));assert first and field['size']==4
                expression=first[1];assert all(re.sub(r'\s+','',g[3])=='((unsignedint)'+expression+'>>'+str(j*8)+'&0xff)' for j,g in enumerate(group[1:],1));value=None;kind='complete_little_endian_expression'
            old=''.join(g[0] for g in group);new=m[1]+names[active]+'.'+field['name']+' = '+expression+';'+('\n' if group[-1][0].endswith('\n') else '')
            phase_edits.append(dict(before=old,after=new,kind=kind,offset=off,size=field['size'],value=value,expression=expression));done.update(range(index,index+field['size']))
        for e in phase_edits:
            assert region.count(e['before'])==1;region=region.replace(e['before'],e['after'])
        alias=re.compile(r'\b'+storage+r'\.('+'|'.join(r for r in roles if r!=active)+r')\.([A-Za-z_]\w*)')
        def translate(m):
            field=next(f for f in layouts[m[1]] if f['name']==m[2]);dest=[f for f in fields if (f['offset'],f['size'])==(field['offset'],field['size'])]
            assert len(dest)==1,(m[0],field);changes.append(dict(before=m[0],after=names[active]+'.'+dest[0]['name'],kind='same_offset_typed_field',offset=field['offset'],size=field['size']));return changes[-1]['after']
        region=alias.sub(translate,region)
        # Constructor/consumer addresses must name their actual view. Field
        # aliases are translated only when the exact byte geometry agrees.
        assert not re.search(r'\b'+storage+r'\.(?:'+'|'.join(r for r in roles if r!=active)+r')\b',mask(region))
        text=text[:start]+region+text[end:];edits.extend(phase_edits+changes)
    union=PATTERN.search(text);assert union;text=text[:union.start()]+text[union.end():]
    for role,name in names.items():text=re.sub(r'\b'+storage+r'\.'+role+r'\b',name,text)
    assert not re.search(r'\b'+storage+r'\b',mask(text))
    # Lowest common lexical compound contains every use, including initializer
    # and any pointer consumed after a branch. Never infer C lifetime merely
    # from an optimized initializer's native instruction window.
    code=mask(text);stack=[];owners={};opened={}
    for m in re.finditer(r'[{}]|\b(?:'+'|'.join(re.escape(n) for n in names.values())+r')\b',code):
        if m[0]=='{':stack.append(m.start());opened[m.start()]=list(stack)
        elif m[0]=='}':assert stack;stack.pop()
        else:assert stack;owners.setdefault(m[0],[]).append(list(stack))
    declarations={}
    for role,name in names.items():
        paths=owners[name];common=paths[0]
        for path in paths[1:]:
            common=common[:next((i for i,(a,b) in enumerate(zip(common,path)) if a!=b),min(len(common),len(path)))]
        assert common;at=common[-1];line=code[code.rfind('\n',0,at)+1:at];indent=re.match(r' *',line)[0]+'  '
        declarations.setdefault(at,[]).append(indent+TYPES[role][0]+' '+name+';')
    for at,decls in sorted(declarations.items(),reverse=True):text=text[:at+1]+'\n'+'\n'.join(decls)+text[at+1:]
    assert '__LINE__' not in code
    assert re.findall(r'"(?:\\.|[^"\\])*"',before)==re.findall(r'"(?:\\.|[^"\\])*"',text)
    return text,dict(storage=storage,restored_locals=facts,field_rewrites=edits,declaration_scopes=[dict(offset=at,declarations=decls) for at,decls in declarations.items()])
