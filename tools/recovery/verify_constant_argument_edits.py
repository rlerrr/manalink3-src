"""Replay narrow added-argument migrations using sealed native call sites."""
import json,re
from unify_scalar_helpers import ROOT,mask,parameters,sha

FAMILIES={'leaves_play':('leaves_play_2022',3),
          'other_leaves_play':('other_leaves_play_2022',5),
          'get_subtypes_by_csvid':('get_subtypes_by_csvid_2022',2),
          'has_colorless_only_mana_in_casting_cost':('has_colorless_only_mana_in_casting_cost',4)}
TARGETS={'leaves_play':'0x26a2080','other_leaves_play':'0x26a3630',
         'get_subtypes_by_csvid':'0x2673320','has_colorless_only_mana_in_casting_cost':'0x225a7d0'}
ARGUMENT_POSITIONS={'leaves_play':[3,4],'other_leaves_play':[5,6],
                    'get_subtypes_by_csvid':[0],'has_colorless_only_mana_in_casting_cost':[3]}
# This private GCC worker passes player/card/event in EAX/EDX/ECX. Its fourth
# source parameter is the first physical stack word, proved from concrete DIEs.
PHYSICAL_ARGUMENT_POSITIONS={'has_colorless_only_mana_in_casting_cost':[0]}

def constant_argument(site,index):
    for argument in site['arguments']:
        location=argument['location'];value=argument['value']
        if not isinstance(location,list) or len(location)!=1:continue
        op=location[0]
        if op['operation'] not in {'DW_OP_breg4','DW_OP_fbreg'} or op['arguments']!=[index*4]:continue
        if not isinstance(value,list) or len(value)!=1:continue
        op=value[0];name=op['operation']
        if name.startswith('DW_OP_lit'):return int(name[9:])
        if name.startswith('DW_OP_const'):return op['arguments'][0]
    return None

def replay(before,helper,values):
    target,arity=FAMILIES[helper];code=mask(before);edits=[]
    for match in re.finditer(r'\b'+helper+r'\s*\(',code):
        depth=1
        for at in range(match.end(),len(code)):
            depth+=(code[at]=='(')-(code[at]==')')
            if not depth:break
        assert not depth
        args=parameters(before[match.end():at]);assert len(args)==arity
        combined=[a.strip() for a in args]
        for position,value in zip(ARGUMENT_POSITIONS[helper],values):combined.insert(position,str(value))
        edits.append((match.start(),at+1,target+'('+', '.join(combined)+')'))
    after=before
    for lo,hi,new in reversed(edits):after=after[:lo]+new+after[hi:]
    return after,len(edits)

def verify(proof,runtime,root=ROOT):
    receipts={e['path']:e['sha256'] for e in proof.get('interface_supporting_inputs',[])}
    inventories={};count=calls=0
    for edit in proof.get('source_edits',[]):
        witness=edit.get('native_constant_argument_witnesses')
        if not witness:continue
        path=witness['inventory'];assert path in receipts
        if path not in inventories:
            data=(root/path).read_bytes();assert sha(data)==receipts[path]
            report=json.loads(data)
            assert report['debug_sha256']=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
            assert report['dll_sha256']==sha(runtime.data)
            inventories[path]={(r['source'],r['caller']):r for r in report['callers']}
        row=inventories[path][(witness['source'],witness['caller'])];helper=witness['helper']
        _,arity=FAMILIES[helper];sites=[s for s in row['native_sites'] if s['helper']==helper]
        assert sites and [s['die_offset'] for s in sites]==witness['site_die_offsets']
        assert all(s['direct_call_target']==TARGETS[helper] for s in sites)
        assert len(witness['values'])==len(ARGUMENT_POSITIONS[helper])
        physical=PHYSICAL_ARGUMENT_POSITIONS.get(helper,ARGUMENT_POSITIONS[helper])
        if helper in PHYSICAL_ARGUMENT_POSITIONS:
            param_path=witness['native_parameter_evidence'];assert param_path in receipts
            param_data=(root/param_path).read_bytes();assert sha(param_data)==receipts[param_path]
            params=json.loads(param_data)
            assert params['debug_sha256']=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
            ps=params['functions']['225a7d0']['parameters']
            assert [p['name'] for p in ps]==['player','card','event','generic','number_of_colorless_only_mana']
            assert all(p['location_expressions'][0]==[dict(operation='DW_OP_reg'+str(reg),arguments=[])] for p,reg in zip(ps,[0,2,1]))
            assert ps[3]['location_expressions']==[[dict(operation='DW_OP_fbreg',arguments=[0])]]
            assert ps[4]['location_expressions']==[[dict(operation='DW_OP_fbreg',arguments=[4])]]
        assert all([constant_argument(s,i) for i in physical]==witness['values'] for s in sites)
        for site in sites:
            pc=int(site['return_pc'],16)
            assert runtime.pe.get_data(pc-5-0x2000000,5).hex()==site['preceding_bytes']
        after,n=replay(edit['before'],helper,witness['values'])
        assert n==len(sites)==len([c for c in row['source_calls'] if c['helper']==helper])
        assert after==edit['after'] and sha(after.encode())==edit['after_sha256']
        count+=1;calls+=n
    return dict(caller_edits=count,calls=calls,scope='Exact constant argument insertion, unchanged existing arguments; no whole-caller equivalence claim.')
