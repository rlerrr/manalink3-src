"""Read direct-call integer result types from GCC9's original-tree dump."""
import collections,re


def inspect(text,wanted):
    out={}
    parts=re.split(r'(?m)^;; Function (\w+) [^\n]*\n',text)
    for name,block in zip(parts[1::2],parts[2::2]):
        if name not in wanted:continue
        assert name not in out;nodes={}
        for m in re.finditer(r'(?ms)^@(\d+)\s+(\w+)\s+(.*?)(?=^@\d+\s|\Z)',block):nodes[m[1]]=dict(kind=m[2],text=re.sub(r'\s+',' ',m[3]).strip())
        def ref(n,key):
            m=re.search(r'\b'+key+r'\s*:\s*@(\d+)\b',nodes[n]['text']);return m[1] if m else None
        calls=collections.defaultdict(list)
        for at,node in nodes.items():
            if node['kind']!='call_expr':continue
            result=ref(at,'type');fn=ref(at,'fn');path=[at]
            if not fn or nodes[fn]['kind']!='addr_expr':continue
            path.append(fn);decl=ref(fn,r'op 0')
            if not decl or nodes[decl]['kind']!='function_decl':continue
            path.append(decl);identifier=ref(decl,'name')
            if not identifier or nodes[identifier]['kind']!='identifier_node':continue
            path.append(identifier);m=re.search(r'\bstrg:\s*(.*?)\s+lngt:',nodes[identifier]['text']);assert m;callee=m[1]
            shape=nodes[result] if result else None;is_int=bool(shape and shape['kind']=='integer_type' and re.search(r'\bprec:\s*32\b',shape['text']) and re.search(r'\bsign:\s*signed\b',shape['text']))
            calls[callee].append(dict(signed_int32=is_int,call_node=at,result_type_node=result,nodes={n:nodes[n] for n in dict.fromkeys(path+([result] if result else []))}))
        out[name]=dict(calls)
    assert set(out)==set(wanted),set(wanted)-set(out);return out
