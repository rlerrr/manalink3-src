"""Card-only source literal expansion; no unknown data/address wildcarding.

The stock comparator requires four-byte strings. Card dialog control packets
and short format/prompt strings need the same exact-source-literal check at
shorter lengths. Empty strings and arbitrary tables remain unresolved.
"""
import ast,re
from canonicalize_helpers import mask
from helper_fingerprints import LocalCode
from promote_recovery import strip_comments

_LITERAL=r'"(?:\\.|[^"\\])*"'


def dialog_macro_literals(body,header):
    """Read only used DLG macro definitions from the current source header.

    DLGIMPL is an object macro containing the exact one-byte prefix. Its
    adjacent literal suffixes concatenate as C string tokens. Parameter values
    stay in the caller and are not invented by this routine.
    """
    if not re.search(r'\bDLG_[A-Z0-9_]+\b',mask(body or '')):return set(),{}
    prefix=re.findall(r'(?m)^#define[ \t]+DLGIMPL[ \t]+([^\n]+)$',header)
    assert len(prefix)==1 and ast.literal_eval(prefix[0].strip())=='\x01'
    definitions={}
    for m in re.finditer(r'(?m)^#define[ \t]+(DLG_[A-Z0-9_]+)(\([^\n)]*\))?[ \t]+([^\n]*)$',header):
        assert m[1] not in definitions and not m[3].endswith('\\'),m[1]
        definitions[m[1]]=(m[2] is not None,m[3],m[0])
    clean=mask(body or '');todo=set()
    for name in re.findall(r'\bDLG_[A-Z0-9_]+\b',clean):
        if name not in definitions:continue
        function_like=definitions[name][0]
        if not function_like or re.search(r'\b'+name+r'\s*\(',clean):todo.add(name)
    result=set();used={}
    while todo:
        name=todo.pop()
        if name in used:continue
        _,text,physical=definitions[name];used[name]=physical
        todo.update(n for n in re.findall(r'\bDLG_[A-Z0-9_]+\b',mask(text)) if n in definitions and n not in used)
        text=re.sub(r'\bDLGIMPL\b',lambda _:prefix[0].strip(),strip_comments(text))
        for run in re.finditer(_LITERAL+r'(?:\s*'+_LITERAL+r')*',text):
            value=''.join(ast.literal_eval(x[0]) for x in re.finditer(_LITERAL,run[0])).encode('latin1')
            assert len(value)==2 and value[0]==1 and 2<=value[1]<=0x1c,(name,value)
            result.add(value)
    if used:used['DLGIMPL']='#define DLGIMPL '+prefix[0].strip()
    return result,used


class CardLiteralCode(LocalCode):
    def readonly_string(self,at,allowed):
        ordinary=super().readonly_string(at,allowed)
        if ordinary is not None:return ordinary
        rva=at-self.base;section=self.pe.get_section_by_rva(rva)
        if section is None or section.Name.rstrip(b'\0')!=b'.rdata' or section.Characteristics&0x80000000:return None
        raw=self.pe.get_data(rva,4);end=raw.find(b'\0')
        if not 0<end<4:return None
        value=raw[:end]
        # Membership is exact and caller-specific. No empty strings, prefix
        # matching, table guessing, writable data or short code references.
        return value if value in allowed else None
