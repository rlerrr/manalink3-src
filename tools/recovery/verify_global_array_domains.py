"""Check canonical absolute arrays against native types and physical bindings."""
import json,re
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,mask


def records(proof,root):
    inputs={r['path']:r['sha256'] for r in proof.get('interface_supporting_inputs',[])}
    for ref in proof.get('source_domain_migrations',[]):
        if ref['kind']!='absolute_global_array':continue
        path=root/ref['path'];assert sha(path.read_bytes())==ref['sha256']==inputs[ref['path']]
        yield json.loads(path.read_text())


def shape(die):
    from inspect_release import value
    from export_abi import type_of,type_name
    t=type_of(die);assert t.tag=='DW_TAG_array_type'
    dims=[]
    for c in t.iter_children():
        if c.tag=='DW_TAG_subrange_type':
            lower=value(c,'DW_AT_lower_bound',0);upper=value(c,'DW_AT_upper_bound')
            assert isinstance(upper,int) and lower==0
            dims.append(upper+1)
    scalar=type_of(t)
    return dict(dimensions=dims,element_type=type_name(scalar),element_bytes=value(scalar,'DW_AT_byte_size'))


def verify(proof,root=ROOT):
    rows=list(records(proof,root))
    if not rows:return {}
    from inspect_release import Image,inherited_value,value
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    assert sha(data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    debug=Image(data);dwarf=debug.dwarf();result={}
    for row in rows:
        assert row['debug_sha256']==sha(data)
        name=row['canonical_name'];old=row['recovery_name'];expected=row['shape']
        assert [list(s) for s in debug.symbols if s[0]=='_'+name]==[row['native_symbol']]
        assert row['native_symbol'][1]==int(row['address'],16) and row['native_symbol'][2:]==[-1,0,2]
        for n in row['native_types']:
            die=dwarf.get_DIE_from_refaddr(int(n['die_offset'],16))
            assert inherited_value(die,'DW_AT_name')==name
            assert value(die.cu.get_top_DIE(),'DW_AT_name')==n['source']
            assert shape(die)==expected
        header=read(root/'manalink.h');assert header.count(row['canonical_declaration'])==1
        linker=read(root/'manalink.lds')
        assert len(re.findall(r'(?mi)^_'+name+r'\s*=\s*'+row['address']+r';',linker))==1
        assert not re.search(r'\b_'+old+r'\b',mask(linker))
        for file in proof['files']:
            if file['source'].endswith(('.h','.c','.cpp')):
                assert not re.search(r'\b'+old+r'\b',mask(read(root/file['source']))),file['source']
        result[name]=dict(address=row['address'],**expected,native_type_instances=len(row['native_types']))
    return result


def verify_compiled(debug,proof,root=ROOT):
    rows=list(records(proof,root))
    if not rows:return {}
    from inspect_release import inherited_value,value
    result={}
    for row in rows:
        name=row['canonical_name'];old=row['recovery_name']
        found=[s for s in debug.symbols if s[0]=='_'+name]
        assert len(found)==1 and found[0][1]==int(row['address'],16) and found[0][2:]==(-1,0,2)
        assert not any(s[0]=='_'+old for s in debug.symbols)
        units=set()
        for cu in debug.dwarf().iter_CUs():
            for die in cu.get_top_DIE().iter_children():
                if die.tag!='DW_TAG_variable' or inherited_value(die,'DW_AT_name')!=name:continue
                assert shape(die)==row['shape']
                units.add(value(cu.get_top_DIE(),'DW_AT_name'))
        assert units
        result[name]=dict(address=row['address'],shape=row['shape'],compiled_units=sorted(units))
    return result
