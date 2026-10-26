"""Verify shared counter/extended-trigger domains against native DWARF."""
import json,re
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha
from canonicalize_reanimation import values


def migrations(proof):
    return [r for r in proof.get('source_domain_migrations',[]) if r['kind']=='counter_trigger_domains']


def native_values(definition):
    # These shared typedefs retain the existing Shandalar branch verbatim.
    text,n=re.subn(r'#ifdef SHANDALAR\n.*?#else\n(.*?)#endif',r'\1',definition,flags=re.S)
    assert n==1
    return values(text)


def verify(proof,root=ROOT):
    rows=migrations(proof)
    if not rows:return {}
    from inspect_release import Image,value
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    assert sha(data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dwarf=Image(data).dwarf();count=0;types={}
    inputs={r['path']:r['sha256'] for r in proof.get('interface_supporting_inputs',[])}
    for row in rows:
        assert inputs[row['path']]==row['sha256']==sha((root/row['path']).read_bytes())
        record=json.loads((root/row['path']).read_text());ref=record['native_input']
        assert inputs[ref['path']]==ref['sha256']==sha((root/ref['path']).read_bytes())
        native=json.loads((root/ref['path']).read_text());assert native['debug_sha256']==sha(data)
        for name,t in record['types'].items():
            assert sha(t['canonical_definition'].encode())==t['canonical_sha256']
            assert sha(t['predecessor_definition'].encode())==t['predecessor_sha256']
            assert read(root/'defs.h').count(t['canonical_definition'])==1
            assert native_values(t['canonical_definition'])==t['native_values']
            matching=[r for r in native['records'] if r['name']==name];assert len(matching)==4
            for n in matching:
                die=dwarf.get_DIE_from_refaddr(int(n['enum_die'],16))
                observed={value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in die.iter_children() if c.tag=='DW_TAG_enumerator'}
                assert observed==n['values']==t['native_values'] and value(die,'DW_AT_byte_size')==n['bytes']==4
                count+=1
            types[name]=dict(bytes=4,values=t['native_values'])
    return dict(types=types,native_enum_instances=count,scope='Exact current shared definitions and native DWARF enum values across four owning CUs; no whole-function equivalence claim.')


def verify_compiled(debug,proof,root=ROOT):
    rows=migrations(proof)
    if not rows:return {}
    from inspect_release import value
    from export_abi import type_of
    expected={}
    for row in rows:
        record=json.loads((root/row['path']).read_text())
        expected.update({n:t['native_values'] for n,t in record['types'].items()})
    observed={}
    for cu in debug.dwarf().iter_CUs():
        source=str(value(cu.get_top_DIE(),'DW_AT_name')).replace('\\','/')
        if source!='functions/counters.c' and not source.endswith('/functions/counters.c'):continue
        for d in cu.iter_DIEs():
            name=value(d,'DW_AT_name')
            if name not in expected or d.tag not in {'DW_TAG_typedef','DW_TAG_enumeration_type'}:continue
            t=d
            while t.tag in {'DW_TAG_typedef','DW_TAG_const_type','DW_TAG_volatile_type'}:t=type_of(t)
            assert t.tag=='DW_TAG_enumeration_type' and value(t,'DW_AT_byte_size')==4
            members={value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in t.iter_children() if c.tag=='DW_TAG_enumerator'}
            assert members==expected[name],(name,members,expected[name])
            observed[name]=members
    assert set(observed)==set(expected),set(expected)-set(observed)
    return dict(enum_types=len(observed),owner='functions/counters.c',values=observed)
