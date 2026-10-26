"""Replay native source-pair witnesses for legacy inline counter call sites."""
import json,re
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span,mask
from unify_scalar_helpers import parameters
from inventory_inline_counter_arguments import INLINE,TARGETS,inspect_row

OUTPUTS={'add_counters':'add_counters_from_source','remove_counters':'remove_counters_from_source'}

def replay(before):
    edits=[];code=mask(before)
    for m in re.finditer(r'\b('+'|'.join(sorted(INLINE,key=len,reverse=True))+r')\s*\(',code):
        name=m[1];callee,arity=INLINE[name];depth=1
        for at in range(m.end(),len(code)):
            depth+=(code[at]=='(')-(code[at]==')')
            if not depth:break
        args=[a.strip() for a in parameters(before[m.end():at])];assert len(args)==arity
        if name in {'add_counter','remove_counter'}:target=args[:2];kind=args[2];num='1'
        else:
            target=args[:2];kind='COUNTER_M1_M1' if name=='add_minus1_minus1_counters' else 'COUNTER_P1_P1';num=args[2] if arity==3 else '1'
        args=['player','card',*target,kind,num]
        edits.append((m.start(),at+1,OUTPUTS[callee]+'('+', '.join(args)+')',callee))
    after=before
    for lo,hi,new,_ in reversed(edits):after=after[:lo]+new+after[hi:]
    return after,edits

def eligible(row,analysis,before):
    if analysis['status']!='analyzed' or not analysis['sites']:return False,'no analyzed sites'
    if not all(s.get('values')==[['parameter','player'],['parameter','card']] for s in analysis['sites']):return False,'nonuniform or unresolved source pair'
    wanted={c['helper'] for c in row['source_calls'] if c['helper'] in INLINE}
    if not wanted:return False,'no inline calls'
    # Require complete direct-call coverage for both counter families. Equality
    # here is a coverage check, in addition to the CFG argument-value witness.
    from collections import Counter
    count=Counter(INLINE[c['helper']][0] for c in row['source_calls'] if c['helper'] in INLINE)
    native=Counter(s['callee'] for s in analysis['sites'])
    if count!=native:return False,'source/native family coverage differs'
    head=before.split('{',1)[0];args=parameters(head[head.index('(')+1:head.rindex(')')])
    if not all(re.search(r'\bint\s+'+n+r'\b',head) for n in ['player','card']):return False,'different source parameter names'
    code=mask(before.split('{',1)[1])
    if re.search(r'(?<![\w.>])(?:player|card)\s*(?:\+\+|--|(?:[+*/&|^%-]|-(?!>))?=(?!=))|(?:\+\+|--)\s*\b(?:player|card)\b|&\s*(?:player|card)\b',code):return False,'source formal may change or escape'
    after,edits=replay(before)
    if Counter(e[3] for e in edits)!=count:return False,'unmatched source call scope'
    return True,'uniform source pair'

def verify_entry_inventory(report,receipts,root=ROOT):
    """Re-read native location lists before accepting resolved entry bindings."""
    if not report.get('entry_location_inputs'):return
    import copy
    from inspect_release import Image,value,inherited_value
    from elftools.dwarf.locationlists import LocationParser,BaseAddressEntry,LocationExpr
    from elftools.dwarf.dwarf_expr import DWARFExprParser
    refs=report['entry_location_inputs'];assert len(refs)==2
    for ref in refs:
        assert receipts[ref['path']]==ref['sha256']==sha((root/ref['path']).read_bytes())
    expected=json.loads((root/refs[0]['path']).read_text());entry=json.loads((root/refs[1]['path']).read_text())
    assert entry['inventory_sha256']==refs[0]['sha256'] and entry['debug_sha256']==expected['debug_sha256']
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    assert sha(data)==expected['debug_sha256']=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dwarf=Image(data).dwarf();locations=LocationParser(dwarf.location_lists());parser=DWARFExprParser(dwarf.structs)
    entries={(r['source'],r['caller']):r for r in entry['callers']}
    for row in expected['callers']:
        e=entries.get((row['source'],row['caller']))
        if not e:continue
        assert len(row['native_ranges'])==1
        native=row['native_ranges'][0];lo=int(native['low_pc'],16)
        assert lo==int(e['address'],16)
        ps={p['name']:p for p in e['parameters']}
        for p in native['parameters']:
            q=ps.get(p['name'])
            if not isinstance(p['location'],dict) or not q:continue
            die=dwarf.get_DIE_from_refaddr(int(q['die_offset'],16))
            assert die.get_parent().offset==int(native['die_offset'],16)
            assert inherited_value(die,'DW_AT_name')==p['name']
            attr=die.attributes['DW_AT_location'];assert attr.value==p['location']['location_list_offset']
            loc=locations.parse_from_attribute(attr,die.cu['version'],die);assert not isinstance(loc,LocationExpr)
            base=value(die.cu.get_top_DIE(),'DW_AT_low_pc',0) or 0;entry_expressions=[]
            for item in loc:
                if isinstance(item,BaseAddressEntry):base=item.base_address;continue
                begin=item.begin_offset if item.is_absolute else base+item.begin_offset
                end=item.end_offset if item.is_absolute else base+item.end_offset
                if begin<=lo<end:entry_expressions.append([dict(operation=o.op_name,arguments=o.args) for o in parser.parse_expr(item.loc_expr)])
            assert entry_expressions==q['entry_expressions']
            if len(entry_expressions)!=1:continue
            ops=entry_expressions[0]
            if len(ops)!=1 or ops[0]['operation'] not in {'DW_OP_fbreg','DW_OP_reg0','DW_OP_reg1','DW_OP_reg2'}:continue
            p['original_location_list']=p['location'];p['entry_location_die']=q['die_offset'];p['location']=ops
    expected['entry_location_inputs']=refs
    assert expected==report


def verify(proof,image,root=ROOT):
    inputs={r['path']:r['sha256'] for r in proof.get('interface_supporting_inputs',[])};cache={};count=calls=0
    for edit in proof.get('source_edits',[]):
        w=edit.get('native_inline_counter_argument_witness')
        if not w:continue
        paths=[w['inventory'],w['analysis']]
        for path in paths:
            if path not in cache:
                assert sha((root/path).read_bytes())==inputs[path]
                obj=json.loads((root/path).read_text());verify_entry_inventory(obj,inputs,root);cache[path]={(r['source'],r['caller']):r for r in obj['callers']}
        key=(edit['source'],w['caller']);row=cache[paths[0]][key];analysis=cache[paths[1]][key]
        actual=inspect_row(image,row)
        assert json.loads(json.dumps(actual))=={k:v for k,v in analysis.items() if k not in {'source','caller'}}
        okay,reason=eligible(row,analysis,edit['before']);assert okay,(key,reason)
        after,edits=replay(edit['before']);assert after==edit['after']
        assert [s['site'] for s in analysis['sites']]==w['native_site_dies']
        count+=1;calls+=len(edits)
    return dict(callers=count,calls=calls,scope='Added source pair and exact inline expansion only. Native CFG/DWARF witnesses cover every direct counter-family call; other caller behavior is not claimed equivalent.')
