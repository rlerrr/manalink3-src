"""Associate expanded function statements with their original source lines."""
import bisect,re
from canonicalize_helpers import mask


def preprocess_view(text):
    text=text.replace('\r\n','\n')
    origins=[];clean=[];source=None;number=0;macros={}
    for line in text.splitlines(keepends=True):
        m=re.match(r'^#\s+(\d+)\s+"((?:\\.|[^"])*)"',line)
        if m:
            number=int(m[1]);source=m[2].replace('\\\\','\\').replace('\\','/');clean.append('\n');origins.append(None);continue
        if line.startswith('#'):
            d=re.match(r'^#define\s+(\w+)(?:\([^\n]*?\))?\s+(.*)',line)
            if d:macros.setdefault(d[1],[]).append(d[2])
            clean.append('\n');origins.append(None);number+=1;continue
        clean.append(line);origins.append((source,number));number+=1
    sensitive={'__LINE__','__builtin_LINE','__builtin_COLUMN'};changed=True
    while changed:
        changed=False
        for name,bodies in macros.items():
            if name not in sensitive and any(set(re.findall(r'\b\w+\b',mask(b)))&sensitive for b in bodies):sensitive.add(name);changed=True
    clean=''.join(clean);lines=[0]+[m.end() for m in re.finditer('\n',clean)];code=mask(clean)
    def origin(at):return origins[bisect.bisect_right(lines,at)-1]
    def function(name):
        hits=[]
        for m in re.finditer(r'\b'+re.escape(name)+r'\s*\(',code):
            at=code.index('(',m.start())+1;depth=1
            while depth:depth+=(code[at]=='(')-(code[at]==')');at+=1
            while at<len(code) and code[at].isspace():at+=1
            if at==len(code) or code[at]!='{':continue
            start=code.rfind('\n',0,m.start())+1;end=at+1;depth=1
            while depth:depth+=(code[end]=='{')-(code[end]=='}');end+=1
            hits.append((start,end))
        assert len(hits)==1,(name,hits)
        a,b=hits[0];return clean[a:b],lambda at:origin(a+at)
    return function,sensitive
