"""Inspect actual native/current locals and scoped scalar writes after union removal."""
import collections,csv
from pathlib import Path
from canonicalize_helpers import sha
from inspect_release import value,inherited_value
from global_initializers import inherited_type
from object_initializers import unqualify,sizeof,member_offset
from shared_stack_test_analysis import frame_offset,test_layout
from shared_stack_record_analysis import layout
from scalar_frame_analysis import collect
from verify_inline_helpers import ranges


def locals_walk(d):
    for c in d.iter_children():
        if c.tag=='DW_TAG_subprogram':continue
        yield c
        if c.has_children:yield from locals_walk(c)


def inspect(image,debug,rows,*,native=False):
    dw=debug.dwarf();wanted={(r['source'],r['name']) for r in rows};out={}
    fnames=collections.defaultdict(set)
    for n,v,s,k,c in debug.symbols:
        if k==32 and 0<s<=len(image.pe.sections):fnames[image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[s-1].VirtualAddress+v].add(n.lstrip('_'))
    if native:
        for r in csv.DictReader(Path('config/reccmp/functions.csv').open(encoding='utf-8-sig')):
            if r['type']=='function':fnames[int(r['address'],16)]={r['name']}
    for cu in dw.iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')
        if source not in {s for s,n in wanted}:continue
        for d in cu.get_top_DIE().iter_children():
            key=source,inherited_value(d,'DW_AT_name')
            if d.tag!='DW_TAG_subprogram' or key not in wanted or not ranges(dw,d):continue
            assert key not in out and value(d,'DW_AT_frame_base')==[156]
            parts=ranges(dw,d);assert len(parts)==1;lo,hi=parts[0];variables=[];cache={}
            for v in locals_walk(d):
                if v.tag!='DW_TAG_variable':continue
                t=unqualify(inherited_type(v))
                if t is None or t.tag not in ['DW_TAG_structure_type','DW_TAG_array_type','DW_TAG_union_type']:continue
                try:frame=frame_offset(dw,v)
                except (AssertionError,KeyError):continue
                size=sizeof(t);scope=v.get_parent()
                while not ranges(dw,scope):scope=scope.get_parent()
                scope_parts=ranges(dw,scope);item=dict(name=inherited_value(v,'DW_AT_name'),kind=t.tag,
                    bytes=size,variable_die=hex(v.offset),frame_offset=frame,scope_ranges=[[a,b] for a,b in scope_parts])
                if t.tag=='DW_TAG_structure_type' and size in [8,76,84,216]:
                    item['layout']=layout(t);ckey=frame,size
                    if ckey not in cache:cache[ckey]=collect(image,dw,d,frame,item['layout']['fields'],end=size)
                    stores=[s for s in cache[ckey]['stores'] if any(a<=int(s['address'],16)<b for a,b in scope_parts)]
                    events=collections.Counter()
                    for s in stores:
                        tag=None
                        if s['size']==4 and s['value'] not in [None,0] and len(fnames.get(s['value'],set()))==1:
                            tag='callback:'+next(iter(fnames[s['value']]))
                        for j in range(s['size']):
                            if s['offset']+j<size:
                                val=tag+':byte'+str(j) if tag else None if s['value'] is None else s['value']>>(j*8)&255
                                events[s['offset']+j,val]+=1
                    item.update(stores=stores,byte_events=[[o,v,n] for (o,v),n in sorted(events.items(),key=lambda kv:(kv[0][0],str(kv[0][1])))])
                if t.tag=='DW_TAG_array_type':
                    bounds=[m for m in t.iter_children() if m.tag=='DW_TAG_subrange_type'];item['dimensions']=[value(m,'DW_AT_upper_bound')-value(m,'DW_AT_lower_bound',0)+1 for m in bounds]
                    item['element_bytes']=sizeof(inherited_type(t))
                if t.tag=='DW_TAG_union_type':
                    item['members']=[dict(name=value(m,'DW_AT_name'),offset=member_offset(m),bytes=sizeof(inherited_type(m))) for m in t.iter_children() if m.tag=='DW_TAG_member']
                variables.append(item)
            out[key]=dict(owner_die=hex(d.offset),view=dict(address=hex(lo),end=hex(hi),bytes_sha256=sha(image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo))),variables=variables)
        cu._dielist.clear();cu._diemap.clear()
    assert set(out)==wanted,wanted-set(out)
    return out


def validate_locals(native,current):
    """Declaration geometry and token-write matches; other roles remain bounded."""
    assert not any(v['kind']=='DW_TAG_union_type' for v in current['variables'])
    def geometry(v):
        if 'layout' in v:return ('record',v['bytes'],tuple((f['name'],f['offset'],f['size']) for f in v['layout']['fields']))
        return (v['kind'],v['bytes'],tuple(v.get('dimensions',[])),v.get('element_bytes'))
    assert collections.Counter(geometry(v) for v in native['variables'])==collections.Counter(geometry(v) for v in current['variables'])
    for old in native['variables']:
        if 'layout' in old:
            assert any(v.get('layout')==old['layout'] for v in current['variables']),(old['name'],old['bytes'])
        if old['kind']=='DW_TAG_array_type':
            assert any(v['kind']=='DW_TAG_array_type' and v['bytes']==old['bytes'] and v.get('dimensions')==old.get('dimensions') for v in current['variables']),(old['name'],old['bytes'])
    tokens=[]
    for old in [v for v in native['variables'] if v['kind']=='DW_TAG_structure_type' and v['bytes']==76]:
        candidates=[v for v in current['variables'] if v.get('layout')==old['layout']]
        exact=[v for v in candidates if v['byte_events']==old['byte_events']]
        def events(v):return {(o,x) for o,x,n in v['byte_events']}
        same=[v for v in candidates if events(v)==events(old)]
        assert same,('Token scalar events need additional analysis',old['variable_die'])
        tokens.append(dict(native_variable_die=old['variable_die'],exact_event_candidates=[v['variable_die'] for v in exact],
            same_event_set_candidates=[v['variable_die'] for v in same]))
    return dict(native_token_scopes=len(tokens),current_token_scopes=sum(v['kind']=='DW_TAG_structure_type' and v['bytes']==76 for v in current['variables']),
        token_write_pairings=tokens,geometry_restored=True)


def plain_test_record(dw,row):
    """Bind actual typed tests when replaying a historical whole-frame audit."""
    found=[]
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')!=row['source']:continue
        for d in cu.get_top_DIE().iter_children():
            if d.tag!='DW_TAG_subprogram' or inherited_value(d,'DW_AT_name')!=row['name'] or not ranges(dw,d):continue
            tests=[v for v in locals_walk(d) if v.tag=='DW_TAG_variable' and unqualify(inherited_type(v)) is not None
                and unqualify(inherited_type(v)).tag=='DW_TAG_structure_type' and sizeof(inherited_type(v))==216]
            assert tests
            offsets={frame_offset(dw,v) for v in tests};assert len(offsets)==1
            layouts=[test_layout(inherited_type(v)) for v in tests];assert all(t==layouts[0] for t in layouts)
            found.append(dict(die=d,frame_offset=offsets.pop(),layout=layouts[0]))
    assert len(found)==1
    return found[0]
