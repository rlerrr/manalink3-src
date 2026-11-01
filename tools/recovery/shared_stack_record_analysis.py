"""Native/rebuilt frame geometry and scalar stores for shared target/token records."""
from canonicalize_helpers import sha
from inspect_release import value,inherited_value
from global_initializers import inherited_type
from object_initializers import unqualify,sizeof,member_offset
from shared_stack_test_analysis import walk,frame_offset,collect
from verify_inline_helpers import ranges


def layout(d):
    t=unqualify(d);assert t.tag=='DW_TAG_structure_type'
    fields=[dict(name=value(f,'DW_AT_name'),offset=member_offset(f),size=sizeof(inherited_type(f)))
            for f in t.iter_children() if f.tag=='DW_TAG_member']
    assert fields and all(isinstance(f['name'],str) for f in fields)
    return dict(size=sizeof(t),fields=fields)


def compiled_records(debug,records):
    dw=debug.dwarf();wanted={(r['source'],r['name']):r for r in records};result={}
    for cu in dw.iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')
        if source not in {r['source'] for r in records}:continue
        for d in cu.get_top_DIE().iter_children():
            name=inherited_value(d,'DW_AT_name');key=(source,name)
            if d.tag!='DW_TAG_subprogram' or key not in wanted or not ranges(dw,d):continue
            assert key not in result
            row=wanted[key]
            variables=[v for v in walk(d) if v.tag=='DW_TAG_variable' and inherited_value(v,'DW_AT_name')==row['storage']]
            assert len(variables)==1,(key,len(variables))
            variable=variables[0];union=unqualify(inherited_type(variable));assert union.tag=='DW_TAG_union_type'
            members={}
            for member in union.iter_children():
                if member.tag!='DW_TAG_member':continue
                t=unqualify(inherited_type(member))
                if t.tag=='DW_TAG_structure_type' and sizeof(t) in [8,48,76,84]:
                    assert member_offset(member)==0
                    members[value(member,'DW_AT_name')]=layout(t)
            assert members,(key,sizeof(union))
            assert value(d,'DW_AT_frame_base')==[156]
            result[key]=dict(die=d,variable=variable,frame_offset=frame_offset(dw,variable),
                             size=sizeof(union),members=members)
    assert set(result)==set(wanted),set(wanted)-set(result)
    return dw,result


def frame_locals(dw,d):
    result=[]
    for v in walk(d):
        if v.tag!='DW_TAG_variable':continue
        try:
            offset=frame_offset(dw,v);t=unqualify(inherited_type(v));size=sizeof(t)
        except (AssertionError,KeyError,AttributeError,TypeError,ValueError):continue
        row=dict(name=inherited_value(v,'DW_AT_name'),die_offset=hex(v.offset),frame_offset=offset,size=size)
        if t.tag=='DW_TAG_structure_type' and size in [8,48,76,84]:row['layout']=layout(t)
        result.append(row)
    return result


def collect_bytes(image,dw,die,cfa_offset,size):
    return collect(image,dw,die,cfa_offset,[dict(name='shared_storage',offset=0,size=size)],start=0,end=size)
