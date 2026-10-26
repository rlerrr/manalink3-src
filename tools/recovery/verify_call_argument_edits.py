"""Replay narrow ability-mode edits against their sealed native call inventory."""
import json,re
from unify_scalar_helpers import ROOT,mask,parameters,sha

MODE_NAMES={2:'AL_TRIG',4:'AL_STATIC',8:'AL_NM_NL_ACT',16:'AL_MANA',64:'AL_KEY'}

def constant_mode(site):
    for argument in site['arguments']:
        location=argument['location'];value=argument['value']
        if not isinstance(location,list) or len(location)!=1:continue
        op=location[0]
        if op['operation'] not in {'DW_OP_breg4','DW_OP_fbreg'} or op['arguments']!=[8]:continue
        if not isinstance(value,list) or len(value)!=1:continue
        op=value[0]
        if op['operation'].startswith('DW_OP_lit'):return int(op['operation'][9:])
        if op['operation'] in {'DW_OP_constu','DW_OP_consts','DW_OP_const1u','DW_OP_const1s'}:
            return op['arguments'][0]
    return None

def replay(before,mode):
    code=mask(before);edits=[]
    for match in re.finditer(r'\bis_humiliated\s*\(',code):
        depth=1
        for at in range(match.end(),len(code)):
            depth+=(code[at]=='(')-(code[at]==')')
            if not depth:break
        assert not depth
        args=parameters(before[match.end():at]);assert len(args)==2
        edits.append((match.start(),at+1,'is_humiliated_for_ability('+', '.join(a.strip() for a in args)+', '+MODE_NAMES[mode]+')'))
    after=before
    for lo,hi,new in reversed(edits):after=after[:lo]+new+after[hi:]
    return after,len(edits)

def verify(proof,runtime,root=ROOT):
    inventories={};count=0;calls=0
    receipts={e['path']:e['sha256'] for e in proof.get('interface_supporting_inputs',[])}
    for edit in proof.get('source_edits',[]):
        witness=edit.get('native_call_argument_witnesses')
        if not witness:continue
        path=witness['inventory'];assert path in receipts
        if path not in inventories:
            data=(root/path).read_bytes();assert sha(data)==receipts[path]
            report=json.loads(data)
            assert report['debug_sha256']=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
            assert report['dll_sha256']==sha(runtime.data)
            inventories[path]={(r['source'],r['caller']):r for r in report['callers']}
        row=inventories[path][(witness['source'],witness['caller'])]
        sites=[s for s in row['native_sites'] if s['helper']=='is_humiliated']
        assert [s['die_offset'] for s in sites]==witness['site_die_offsets']
        assert sites and all(constant_mode(s)==witness['mode'] for s in sites)
        for site in sites:
            pc=int(site['return_pc'],16)
            assert runtime.pe.get_data(pc-5-0x2000000,5).hex()==site['preceding_bytes']
        after,n=replay(edit['before'],witness['mode'])
        assert n==len(sites)==len([c for c in row['source_calls'] if c['helper']=='is_humiliated'])
        assert after==edit['after'] and sha(after.encode())==edit['after_sha256']
        count+=1;calls+=n
    return dict(callers=count,calls=calls,scope='Exact constant-mode edit replay and native site-byte anchors; full caller equivalence is not claimed for these narrow edits.')
