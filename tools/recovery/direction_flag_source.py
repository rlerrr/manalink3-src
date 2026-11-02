"""Eliminate private zero direction-state locals from unsigned stride math."""
import bisect,collections,re
from pycparser import c_ast,c_generator
from canonicalize_helpers import mask
from source_record_flow import parse
from call_condition_source import walk

GEN=c_generator.CGenerator()
PATTERN=re.compile(r'\(unsigned int\)\s*(?P<flag>bVar\d+)\s*\*\s*-(?P<scale>8|2)\s*\+\s*(?P<step>4|1)\b')

def stride_facts(text,flag,uses,parents,ids,offset,code,parens):
    strides=[]
    for m in uses:
        refs=[n for n in ids[flag] if offset(n)==m.start('flag')];assert len(refs)==1;ref=refs[0];cast=parents[id(ref)]
        assert isinstance(cast,c_ast.Cast) and cast.expr is ref and GEN.visit(cast.to_type)=='unsigned int'
        mul=parents[id(cast)];assert isinstance(mul,c_ast.BinaryOp) and mul.op=='*' and mul.left is cast and GEN.visit(mul.right)=='-'+m['scale']
        add=parents[id(mul)];step=m['step'];base=None
        if isinstance(add,c_ast.BinaryOp) and add.op=='+' and add.left is mul and isinstance(add.right,c_ast.Constant) and add.right.value==step:
            kind='constant';whole=add;new=step+'u';last=add.right
        else:
            assert isinstance(add,c_ast.BinaryOp) and add.op=='+' and add.right is mul
            whole=parents[id(add)];assert isinstance(whole,c_ast.BinaryOp) and whole.op=='+' and whole.left is add and isinstance(whole.right,c_ast.Constant) and whole.right.value==step
            base=add.left
            assert not any(isinstance(n,(c_ast.FuncCall,c_ast.Assignment)) or isinstance(n,c_ast.UnaryOp) and n.op in ['++','--','p++','p--'] for n in walk(base))
            kind='linear';new=GEN.visit(c_ast.BinaryOp('+',base,c_ast.Constant('unsigned int',step+'u')));last=whole.right
        assert offset(last)==m.end()-len(step)
        fold_start,fold_end=m.start(),m.end();fold_value=int(step)
        factor=parents.get(id(whole))
        if kind=='constant' and isinstance(factor,c_ast.BinaryOp) and factor.op=='*' and factor.left is whole and isinstance(factor.right,c_ast.Constant) and factor.right.value=='4':
            tail=offset(factor.right);groups=[(a,b) for a,b in parens.items() if a<m.start() and b>=m.end() and b<tail and re.fullmatch(r'\s*\(*\s*',code[a+1:m.start()]) and re.fullmatch(r'\s*\)*\s*',code[m.end():b]) and re.fullmatch(r'\s*\*\s*',code[b+1:tail])]
            assert groups;fold_start=min(a for a,b in groups);fold_end=tail+1;whole=factor;fold_value*=4;new=str(fold_value)+'u'
        original=text[fold_start:fold_end];assert not re.search(r'/\*|//',original)
        strides.append(dict(flag_offset=m.start('flag'),expression_start=m.start(),expression_end=m.end(),fold_start=fold_start,fold_end=fold_end,fold_literal=str(fold_value)+'u',scale=int(m['scale']),forward_step=int(step),kind=kind,old_expression=GEN.visit(whole),new_expression=new,base_expression=GEN.visit(base) if base is not None else None,constant_zero_expression=original[:m.start('flag')-fold_start]+'0'+original[m.end('flag')-fold_start:]))
    return strides

def analyze(text):
    fn,offset=parse(text);code=mask(text);assert not re.search(r'\b(?:EXE_FN|asm|__asm__|__builtin_frame_address|__builtin_return_address)\b',code)
    nodes=list(walk(fn));parents={id(c):n for n in nodes for _,c in n.children()};decls=collections.defaultdict(list);ids=collections.defaultdict(list)
    parens={};stack=[]
    for at,ch in enumerate(code):
        if ch=='(':stack.append(at)
        elif ch==')':assert stack;parens[stack.pop()]=at
    assert not stack
    for n in nodes:
        if isinstance(n,c_ast.Decl):decls[n.name].append(n)
        if isinstance(n,c_ast.ID):ids[n.name].append(n)
    body=[n for n in fn.body.block_items or [] if not isinstance(n,(c_ast.Decl,c_ast.Typedef))];matches=list(PATTERN.finditer(code));found=[]
    for flag in sorted({m['flag'] for m in matches}):
        ds=decls[flag]
        if len(ds)!=1:continue
        d=ds[0]
        if d not in (fn.body.block_items or []) or d.storage or d.quals or d.init is not None or not isinstance(d.type,c_ast.TypeDecl) or d.type.quals or not isinstance(d.type.type,c_ast.IdentifierType) or d.type.type.names not in [['unsigned','char'],['char'],['int'],['unsigned','int'],['_Bool']]:continue
        if not body:continue
        zero=body[0]
        if not isinstance(zero,c_ast.Assignment) or zero.op!='=' or not isinstance(zero.lvalue,c_ast.ID) or zero.lvalue.name!=flag or not isinstance(zero.rvalue,c_ast.Constant) or zero.rvalue.value!='0':continue
        uses=[m for m in matches if m['flag']==flag and (int(m['scale']),int(m['step'])) in [(8,4),(2,1)]]
        allowed={offset(zero.lvalue),*[m.start('flag') for m in uses]}
        if any(offset(n) not in allowed for n in ids[flag]):continue
        strides=stride_facts(text,flag,uses,parents,ids,offset,code,parens)
        # Both removed statements must occupy complete source lines.
        declaration_start=text.rfind('\n',0,offset(d))+1;declaration_end=text.find('\n',offset(d))+1
        setter_start=text.rfind('\n',0,offset(zero))+1;setter_end=text.find('\n',offset(zero))+1
        assert declaration_end and setter_end
        assert re.fullmatch(r'\s*(?:unsigned\s+char|char|int|unsigned\s+int|bool|_Bool)\s+'+re.escape(flag)+r'\s*;\s*',text[declaration_start:declaration_end])
        assert re.fullmatch(r'\s*'+re.escape(flag)+r'\s*=\s*0\s*;\s*',text[setter_start:setter_end])
        found.append(dict(variable=flag,zero_assignment_offset=offset(zero),declaration_start=declaration_start,declaration_end=declaration_end,setter_start=setter_start,setter_end=setter_end,stride_uses=strides))
    return found

def project(before,selected):
    found=analyze(before);assert selected and all(v in found for v in selected);edits=[]
    for c in selected:
        edits.extend([(c['declaration_start'],c['declaration_end'],''),(c['setter_start'],c['setter_end'],'')])
        edits.extend((v['fold_start'],v['fold_end'],v['fold_literal']) for v in c['stride_uses'])
    edits.sort();assert all(a[1]<=b[0] for a,b in zip(edits,edits[1:]))
    after=before
    for a,b,text in reversed(edits):after=after[:a]+text+after[b:]
    assert not any(re.search(r'\b'+re.escape(c['variable'])+r'\b',mask(after)) for c in selected)
    return after,dict(removed_flags=len(selected),folded_strides=sum(len(v['stride_uses']) for v in selected),source_candidates=selected,scope='Unique private ordinary integer flags are zeroed by the first executable statement and have no other references except recognized unsigned stride reads. Removing this unobservable state and folding to unsigned literals retains unsigned arithmetic. Constant subtrees keep their value/type; flattened base+zero+step expressions require actual integer/pointer base types and identical whole-expression types. Clear/copy loops and their memory actions remain in place.')

def render_stride_probes(before,selected):
    edits=[]
    for c in selected:
        for i,v in enumerate(c['stride_uses']):
            step=v['fold_literal'];checks=[f'__builtin_classify_type({c["variable"]}) == 1',f'__builtin_types_compatible_p(__typeof__(({v["old_expression"]})), __typeof__(({v["new_expression"]})))',f'({v["constant_zero_expression"]}) == {step}']
            checks.append('('+' || '.join('__builtin_types_compatible_p(__typeof__(&'+c['variable']+'), '+t+'*)' for t in ['unsigned char','char','int','unsigned int','_Bool'])+')')
            if v['base_expression'] is not None:checks.append(f'(__builtin_classify_type(({v["base_expression"]})) == 1 || __builtin_classify_type(({v["base_expression"]})) == 5)')
            assertion='_Static_assert('+ ' && '.join('('+x+')' for x in checks)+', "direction_stride_'+str(i)+'");'
            original=before[v['fold_start']:v['fold_end']];replacement='(({ '+assertion+' '+original+'; }))'
            edits.append((v['fold_start'],v['fold_end'],replacement))
    edits.sort();assert all(a[1]<=b[0] for a,b in zip(edits,edits[1:]))
    for a,b,text in reversed(edits):before=before[:a]+text+before[b:]
    return before

def probe_definition(before,selected):
    assert all(v in analyze(before) for v in selected)
    return render_stride_probes(before,selected)

def probe_unit(expanded,rows,definition_probe=None):
    expanded=expanded.replace('\r\n','\n');lines=expanded.splitlines(keepends=True);clean_lines=['\n' if line.startswith('#') else line for line in lines];clean=''.join(clean_lines);raw_offsets=[0];clean_offsets=[0]
    for original,stripped in zip(lines,clean_lines):raw_offsets.append(raw_offsets[-1]+len(original));clean_offsets.append(clean_offsets[-1]+len(stripped))
    def raw_at(at):
        line=bisect.bisect_right(clean_offsets,at)-1;return raw_offsets[line]+at-clean_offsets[line]
    edits=[]
    for row in rows:
        before=row['expanded_before_definition'];assert clean.count(before)==1;start=clean.index(before);a,b=raw_at(start),raw_at(start+len(before));assert all(not line.startswith('#') or re.match(r'#\s*\d+\s+"',line) for line in expanded[a:b].splitlines())
        edits.append((a,b,definition_probe(before,row) if definition_probe is not None else probe_definition(before,[c['expanded_fact'] for c in row['checks']])))
    edits.sort();assert all(a[1]<=b[0] for a,b in zip(edits,edits[1:]))
    for a,b,text in reversed(edits):expanded=expanded[:a]+text+expanded[b:]
    return expanded+'\n_Static_assert(sizeof(unsigned int) == 4 && sizeof(void*) == 4, "direction_stride_model");\n'
