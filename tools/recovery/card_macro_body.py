"""Physical bounds for the two explicit Arcane card-definition macros."""
import re
from canonicalize_helpers import mask,span


def physical_definition(text,name,expansion=None):
    if not expansion:
        return span(text,name)
    macro=expansion['macro'];public=expansion['function']
    assert macro in ('ARCANE','ARCANE_WITH_SPLICE') and name==public+'_impl'
    clean=mask(text)
    found=list(re.finditer(r'(?m)^[ \t]*'+macro+r'\s*\(\s*'+re.escape(public)+r'\b',clean))
    assert len(found)==1,(name,len(found))
    start=found[0].start();at=clean.index('(',start);depth=1;at+=1
    while depth:
        if clean[at]=='(':depth+=1
        elif clean[at]==')':depth-=1
        at+=1
    brace=clean.index('{',at);assert not clean[at:brace].strip(),name
    depth=1;end=brace+1
    while depth:
        if clean[end]=='{':depth+=1
        elif clean[end]=='}':depth-=1
        end+=1
    return start,end


def macro_definition(header,macro):
    found=re.findall(r'(?m)^#define[ \t]+'+re.escape(macro)+r'\([^\n]+$',header)
    assert len(found)==1 and not found[0].endswith('\\'),macro
    return found[0]
