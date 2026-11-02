"""Scoped record geometry, filter parameters and mutually exclusive store clones."""
import collections
from token_aggregate_analysis import validate_locals
from verify_wide_stack_test_recovery import decode
from bounded_switch_analysis import graph


def events(v):
    # A filter's first100 bytes are its prompt, and optimized shared-frame
    # lexical ranges can include unrelated target/token stores there. Filter
    # scalar parameters start at100, as in the original parameter audits.
    return collections.Counter({(a,b):n for a,b,n in v['byte_events'] if v['bytes']!=216 or a>=100})


def validate(native,current,image,bindings):
    geometry=validate_locals(native,current);comparisons=[]
    for old in native['variables']:
        if 'layout' not in old:continue
        expected_name=bindings.get(old['variable_die'],old['name'])
        candidates=[v for v in current['variables'] if v.get('layout')==old['layout'] and v['name']==expected_name]
        assert len(candidates)==1,(old['name'],expected_name,len(candidates))
        exact=[v for v in candidates if events(v)==events(old)]
        if exact:comparisons.append(dict(native_die=old['variable_die'],kind='exact_scoped_scalar_events',current_die=exact[0]['variable_die']));continue
        clones=[v for v in candidates if events(v)==collections.Counter({k:n*2 for k,n in events(old).items()})]
        assert len(clones)==1 and events(old),(old['name'],old['bytes']);new=clones[0]
        _,instructions=decode(current['view'],image);edges,tables=graph(instructions,image)
        stores=collections.defaultdict(list)
        for s in new['stores']:
            if new['bytes']==216 and s['offset']<100:continue
            stores[s['offset'],s['size']].append(int(s['address'],16))
        def reachable(start,target):
            seen=set();todo=list(edges[start])
            while todo:
                at=todo.pop()
                if at==target:return True
                if at in seen:continue
                seen.add(at);todo.extend(edges[at])
            return False
        pairs=[]
        for key,sites in stores.items():
            assert len(sites)==2,(key,sites)
            assert not reachable(sites[0],sites[1]) and not reachable(sites[1],sites[0])
            pairs.append(dict(offset=key[0],size=key[1],sites=[hex(a) for a in sites]))
        comparisons.append(dict(native_die=old['variable_die'],current_die=new['variable_die'],kind='mutually_exclusive_identical_store_copies',store_pairs=pairs,switch_tables=tables))
    return dict(geometry=geometry,scalar_comparisons=comparisons)
