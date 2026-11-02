"""Offset-preserving syntax views for four previously deferred call-result sites.

These views only feed statement-CFG/liveness analysis. Product C and compiler
inputs retain the real offsetof, GNU minimum expression and executable casts.
"""
import re
from canonicalize_helpers import mask


def analysis_view(text,name,expanded=False):
    edits=[]
    if name=='card_torgaar_famine_incarnate' and expanded:
        original='({ __typeof__(max_required) macro_compare_a = (max_required); __typeof__(available) macro_compare_b = (available); macro_compare_a < macro_compare_b ? macro_compare_a : macro_compare_b; })'
        assert text.count(original)==1
        # Both operands are ordinary private int locals. This expression
        # introduces no control transfer, calls or references to result locals.
        assert re.search(r'\bint max_required = round_up_value\(',text)
        assert re.search(r'\bint available = max_can_sacrifice_as_cost\(',text)
        assert not re.search(r'&\s*(?:max_required|available)\b',mask(text))
        at=text.index(original);edits.append((at,at+len(original),'(max_required < available ? max_required : available)'))
    elif name=='card_karn_liberated' and expanded:
        pattern=r'__builtin_offsetof\s*\(\s*karn_commander_snapshot_t\s*,\s*general1_ebk\s*\)'
        hits=list(re.finditer(pattern,text));assert len(hits)==1
        m=hits[0];prefix=text[max(0,m.start()-100):m.start()]
        assert 'enum { static_assertion__karn_native_commander_flag_offset' in prefix
        # The existing compile-time assertion and successful compiler build
        # bind this field offset to 12. No runtime expression is omitted.
        assert re.match(r'\s*== 12\)',text[m.end():])
        edits.append((m.start(),m.end(),'12'))
    elif name=='charge_spell_cost':
        pattern=(r'\(\((?:int|void)\s*\(\*\)\([^()]*\)\)\(0x[0-9a-fA-F]+\)\)' if expanded else r'\bEXE_FN\([^()]*\)')
        hits=list(re.finditer(pattern,text));assert len(hits)>1
        for m in hits:
            # Every replaced callee is a literal-address cast. All actual
            # argument expressions, assignments and control flow stay visible.
            if not expanded:assert re.fullmatch(r'EXE_FN\((?:int|void), 0x[0-9a-fA-F]+(?:, [A-Za-z_][A-Za-z_0-9* ]*)*\)',m[0]),m[0]
            edits.append((m.start(),m.end(),'exe_call_stub'))
    else:assert name in {'card_torgaar_famine_incarnate','card_karn_liberated'}
    adapted=text
    for start,end,replacement in reversed(edits):
        room=text[start:end];slots=[i for i,c in enumerate(room) if c!='\n'];assert len(slots)>=len(replacement)
        padded=['\n' if c=='\n' else ' ' for c in room]
        # Keep the replacement token contiguous; the multiline offsetof case
        # starts with enough space before its first newline for the constant.
        assert '\n' not in room[:len(replacement)]
        padded[:len(replacement)]=replacement
        adapted=adapted[:start]+''.join(padded)+adapted[end:]
    assert len(adapted)==len(text) and [i for i,c in enumerate(text) if c=='\n']==[i for i,c in enumerate(adapted) if c=='\n']
    return adapted,[dict(start=a,end=b,original=text[a:b],analysis_spelling=c) for a,b,c in edits]
