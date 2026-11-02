"""Remove private int temporaries used only by immediate comma comparisons."""
import bisect,collections,re
from pycparser import c_ast
from canonicalize_helpers import mask
from source_record_flow import parse
from call_condition_source import walk

def literal(node,text,offset):
    constant=node.expr if isinstance(node,c_ast.UnaryOp) and node.op in ['+','-'] else node
    if not isinstance(constant,c_ast.Constant) or not (constant.type=='char' or re.fullmatch(r'(?:unsigned\s+)?(?:long(?:\s+long)?\s+)?int',constant.type)):return None
    start=offset(node);at=offset(constant);end=at+len(constant.value)
    assert text[at:end]==constant.value
    if node is not constant:
        # pycparser normally gives a unary operator its operand's coordinate.
        if not re.fullmatch(re.escape(node.op)+r'\s*',text[start:at]):
            start=at
            while start and text[start-1].isspace():start-=1
            if not start or text[start-1]!=node.op:return None
            start-=1
    return start,end

def analyze(text):
    fn,offset=parse(text);nodes=list(walk(fn));decls=collections.defaultdict(list);parents={id(c):n for n in nodes for _,c in n.children()};code=mask(text)
    assert not re.search(r'\b(?:asm|__asm__|__builtin_frame_address|__builtin_return_address)\b',code)
    for n in nodes:
        if isinstance(n,c_ast.Decl):decls[n.name].append(n)
    pairs={};stack=[]
    for at,c in enumerate(code):
        if c=='(':stack.append(at)
        elif c==')':
            assert stack;pairs[stack.pop()]=at
    assert not stack
    found=collections.defaultdict(list)
    for n in nodes:
        if not isinstance(n,c_ast.ExprList) or len(n.exprs)!=2:continue
        parent=parents.get(id(n))
        if isinstance(parent,c_ast.FuncCall) and parent.args is n:continue
        assignment,test=n.exprs
        if not isinstance(assignment,c_ast.Assignment) or assignment.op!='=' or not isinstance(assignment.lvalue,c_ast.ID) or not isinstance(assignment.rvalue,c_ast.FuncCall) or not isinstance(assignment.rvalue.name,c_ast.ID):continue
        name=assignment.lvalue.name
        if not isinstance(test,c_ast.BinaryOp) or test.op not in ['==','!=','<','<=','>','>='] or not isinstance(test.left,c_ast.ID) or test.left.name!=name:continue
        value=literal(test.right,text,offset)
        if value is None:continue
        ds=decls[name]
        if len(ds)!=1:continue
        d=ds[0]
        if d not in (fn.body.block_items or []) or d.storage or d.quals or d.init is not None or not isinstance(d.type,c_ast.TypeDecl) or d.type.quals or not isinstance(d.type.type,c_ast.IdentifierType) or d.type.type.names!=['int']:continue
        if text.count('  int '+name+';\n')!=1:continue
        start=offset(assignment);value_start,value_end=value
        enclosing=[(a,b) for a,b in pairs.items() if a<start and b>=value_end]
        assert enclosing;a,b=max(enclosing)
        assert re.fullmatch(r'\s*\(*\s*',code[a+1:start]) and re.fullmatch(r'\s*\)*\s*',code[value_end:b])
        call=assignment.rvalue;call_start=offset(call);opening=code.index('(',call_start);call_end=pairs[opening]+1
        assert start<call_start<call_end<offset(test.left)<value_start
        # Every token discarded here belongs to the assignment/comma/test. A
        # nested argument ExprList is explicitly excluded above.
        assert re.fullmatch(re.escape(name)+r'\s*=\s*',code[start:call_start])
        assert re.fullmatch(r'\s*\)*\s*,\s*\(*\s*',code[call_end:offset(test.left)])
        assert re.fullmatch(r'\s*'+re.escape(test.op)+r'\s*\(*\s*',code[offset(test.left)+len(name):value_start])
        if re.search(r'/\*|//',text[a:call_start]+text[call_end:b+1]):continue
        fact=dict(variable=name,callee=call.name.name,expression_start=a,expression_end=b+1,assignment_offset=start,call_start=call_start,call_end=call_end,comparison_offset=offset(test.left),operator=test.op)
        if text[value_start:value_end]!='0':fact['comparison_literal']=text[value_start:value_end]
        found[name].append((fact,assignment.lvalue,test.left))
    result=[]
    for name,values in found.items():
        allowed={id(n) for _,left,right in values for n in [left,right]}
        if any(id(n) not in allowed for n in nodes if isinstance(n,c_ast.ID) and n.name==name):continue
        result.extend(fact for fact,_,_ in values)
    result.sort(key=lambda v:v['expression_start'])
    assert all(a['expression_end']<=b['expression_start'] for a,b in zip(result,result[1:]))
    return result

def project(before,selected,*,remove_only=None):
    found=analyze(before);assert selected and all(v in found for v in selected)
    names={v['variable'] for v in selected}
    # A local's entire use set must be handled in one projection.
    assert [v for v in found if v['variable'] in names]==selected
    after=before
    for v in reversed(selected):
        call=before[v['call_start']:v['call_end']]
        after=after[:v['expression_start']]+'('+call+' '+v['operator']+' '+v.get('comparison_literal','0')+')'+after[v['expression_end']:]
    removed=[]
    for name in sorted(names if remove_only is None else names & set(remove_only)):
        assert len(re.findall(r'\b'+re.escape(name)+r'\b',mask(after)))==1
        old='  int '+name+';\n';assert after.count(old)==1;after=after.replace(old,'');removed.append(name)
    return after,dict(inlined_assignments=len(selected),removed_local_declarations=removed,source_candidates=selected)

def type_probe(definition,selected):
    """GCC-only syntax probe at the exact expanded expression sites; no execution."""
    assert all(v in analyze(definition) for v in selected)
    probe=definition
    for i,v in reversed(list(enumerate(selected))):
        original=definition[v['expression_start']:v['expression_end']];call=definition[v['call_start']:v['call_end']]
        replacement='(({ _Static_assert(__builtin_types_compatible_p(__typeof__('+call+'), int), "comma_result_type_'+str(i)+'"); '+original+'; }))'
        probe=probe[:v['expression_start']]+replacement+probe[v['expression_end']:]
    return probe

def type_probe_unit(expanded,rows):
    """Retain unit directives, replacing selected function spans via line maps."""
    expanded=expanded.replace('\r\n','\n');lines=expanded.splitlines(keepends=True)
    clean_lines=['\n' if line.startswith('#') else line for line in lines]
    clean=''.join(clean_lines);raw_offsets=[0];clean_offsets=[0]
    for original,stripped in zip(lines,clean_lines):
        raw_offsets.append(raw_offsets[-1]+len(original));clean_offsets.append(clean_offsets[-1]+len(stripped))
    def raw_at(at):
        line=bisect.bisect_right(clean_offsets,at)-1
        return raw_offsets[line]+at-clean_offsets[line]
    edits=[]
    for row in rows:
        before=row['expanded_before_definition'];assert clean.count(before)==1;start=clean.index(before);end=start+len(before);a,b=raw_at(start),raw_at(end)
        assert all(not line.startswith('#') or re.match(r'#\s*\d+\s+"',line) for line in expanded[a:b].splitlines())
        selected=[c['expanded_fact'] for c in row['checks']]
        edits.append((a,b,type_probe(before,selected)))
    edits.sort();assert all(a[1]<=b[0] for a,b in zip(edits,edits[1:]))
    for a,b,text in reversed(edits):expanded=expanded[:a]+text+expanded[b:]
    return expanded+'\n_Static_assert(sizeof(int) == 4, "comma_int_width");\n'
