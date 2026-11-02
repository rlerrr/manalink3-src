"""Bounded native/rebuilt store profiles for the restored card records."""
import collections,itertools,csv
from pathlib import Path
from canonicalize_helpers import ROOT
from token_aggregate_analysis import validate_locals
from verify_wide_stack_test_recovery import decode
from bounded_switch_analysis import graph


def inside(parts,other):
    return all(any(a<=x and y<=b for a,b in other) for x,y in parts)


def exclusive_view(snapshot,v,image,debug,native=False):
    children=[c for c in snapshot['variables'] if c is not v and c['frame_offset']==v['frame_offset'] and c['scope_ranges']!=v['scope_ranges'] and inside(c['scope_ranges'],v['scope_ranges'])]
    removed=[];stores=[]
    for s in v.get('stores',[]):
        at=int(s['address'],16)
        owner=[c for c in children if 0<=s['offset'] and s['offset']+s['size']<=c['bytes'] and any(a<=at<b for a,b in c['scope_ranges'])]
        if owner:removed.append(dict(store=s,contained_scope_dies=[c['variable_die'] for c in owner]))
        else:stores.append(s)
    cache=getattr(debug,'_record_profile_symbols',None)
    if cache is None:cache={};debug._record_profile_symbols=cache
    if native not in cache:
        symbols=collections.defaultdict(set)
        for n,value,section,kind,storage in debug.symbols:
            if kind==32 and 0<section<=len(image.pe.sections):symbols[image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[section-1].VirtualAddress+value].add(n.lstrip('_'))
        if native:
            for row in csv.DictReader((ROOT/'config/reccmp/functions.csv').open(encoding='utf-8-sig')):
                if row['type']=='function':symbols[int(row['address'],16)]={row['name']}
        cache[native]=symbols
    symbols=cache[native]
    events=collections.Counter()
    for s in stores:
        tag='callback:'+next(iter(symbols[s['value']])) if s['size']==4 and len(symbols.get(s['value'],set()))==1 else None
        for j in range(s['size']):
            off=s['offset']+j
            if v['bytes']==216 and off<100:continue
            if off>=v['bytes']:continue
            value=tag+':byte'+str(j) if tag else None if s['value'] is None else s['value']>>(8*j)&255;events[off,value]+=1
    return dict(events=events,stores=[s for s in stores if v['bytes']!=216 or s['offset']>=100],excluded_contained_scope_stores=removed)


def clone_proof(snapshot,view,image):
    _,ins=decode(snapshot['view'],image);edges,tables=graph(ins,image);by_field=collections.defaultdict(list)
    for s in view['stores']:by_field[s['offset'],s['size']].append(int(s['address'],16))
    pairs=[]
    def reachable(start,target):
        todo=list(edges[start]);seen=set()
        while todo:
            at=todo.pop()
            if at==target:return True
            if at in seen:continue
            seen.add(at);todo.extend(edges[at])
        return False
    for key,sites in by_field.items():
        assert len(sites)==2,(key,sites)
        assert not reachable(sites[0],sites[1]) and not reachable(sites[1],sites[0]);pairs.append(dict(offset=key[0],size=key[1],sites=[hex(a) for a in sites]))
    assert pairs;return dict(pairs=pairs,switch_tables=tables)


def validate(row,current,before,ni,nd,ci,cd):
    native=row['native_snapshot'];bindings=row['facts']['restored_locals'];wanted={f['native_variable_die'] for f in bindings};native_vars=[v for v in native['variables'] if v['variable_die'] in wanted];current_vars=[v for v in current['variables'] if v['name'] in {f['name'] for f in bindings}]
    assert len(native_vars)==len(current_vars)==len(bindings)
    def geometry(v):
        return (v['kind'],v['bytes'],tuple((f['name'],f['offset'],f['size']) for f in v.get('layout',{}).get('fields',[])),tuple(v.get('dimensions',[])),v.get('element_bytes'))
    baseline=collections.Counter(geometry(v) for v in before['variables'] if v['kind']!='DW_TAG_union_type');expected=baseline+collections.Counter(geometry(v) for v in native_vars);actual=collections.Counter(geometry(v) for v in current['variables']);assert actual==expected
    assert not any(v['kind']=='DW_TAG_union_type' for v in current['variables'])
    left={v['variable_die']:exclusive_view(native,v,ni,nd,True) for v in native_vars};right={v['variable_die']:exclusive_view(current,v,ci,cd) for v in current_vars};comparisons=[]
    for size in sorted({v['bytes'] for v in native_vars}):
        ns=[v for v in native_vars if v['bytes']==size];cs=[v for v in current_vars if v['bytes']==size];assert len(ns)==len(cs);solutions=[]
        for perm in itertools.permutations(cs):
            result=[]
            for n,c in zip(ns,perm):
                a,b=left[n['variable_die']],right[c['variable_die']];le,re=a['events'],b['events'];kind=None;proof=None
                if le==re:kind='exact_exclusive_scope_scalar_events'
                elif le and re==collections.Counter({k:2*v for k,v in le.items()}):
                    try:proof=clone_proof(current,b,ci);kind='exclusive_compiled_store_copies'
                    except AssertionError:pass
                elif re and le==collections.Counter({k:2*v for k,v in re.items()}):
                    try:proof=clone_proof(native,a,ni);kind='exclusive_native_store_copies'
                    except AssertionError:pass
                if kind is None:break
                result.append(dict(native_die=n['variable_die'],current_die=c['variable_die'],current_name=c['name'],kind=kind,clone_proof=proof,native_scope_exclusions=a['excluded_contained_scope_stores'],current_scope_exclusions=b['excluded_contained_scope_stores']))
            else:solutions.append(result)
        assert solutions,('No complete native/current record profile matching',row['name'],size)
        solutions.sort(key=lambda rs:sum(v['kind']!='exact_exclusive_scope_scalar_events' for v in rs));comparisons.extend(solutions[0])
    return dict(restored_records=len(bindings),geometry='before typed declarations plus restored native common-frame declarations',scalar_profile_comparisons=comparisons,
                limits='Contained same-frame record scopes are excluded from broader lexical store profiles. Complete one-to-one event-profile matching is bounded evidence, not unique variable identity or constructor argument/value provenance. Filter comparisons exclude the100-byte prompt region; unknown scalar values remain unknown.')
