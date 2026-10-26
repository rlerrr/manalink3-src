"""Remove the now-unreferenced eight-argument predecessor of the native worker."""
import argparse,json,re,shutil
from pathlib import Path
from unify_scalar_helpers import ROOT,read,sha,span,mask
from audit_source import function_body

EVIDENCE='docs/recovery-evidence/retired-reanimation-worker.json'
SOURCE='functions/functions.c'
OLD='new_reanimate_all_impl'
NEW='new_reanimate_all_impl_2022'
REPLACEMENT='/* The unused eight-argument predecessor was retired; native mass reanimation uses the five-argument recovered worker. */'

def verify(root=ROOT):
    path=root/EVIDENCE
    if not path.exists():return {}
    e=json.loads(path.read_text());assert e['source']==SOURCE and e['retained_name']==OLD and e['recovered_name']==NEW
    assert sha(e['predecessor_definition'].encode())==e['predecessor_definition_sha256']
    assert function_body(e['predecessor_definition'],OLD)==e['predecessor_body']
    assert sha(e['predecessor_body'].encode())==e['predecessor_body_sha256']
    text=read(root/SOURCE)
    assert span(text,OLD) is None and not re.search(r'\b'+OLD+r'\b',mask(text)) and text.count(REPLACEMENT)==1
    m=json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text())
    f=next(f for f in m['accepted_functions'] if f['name']==NEW)
    assert f==e['accepted_recovery_record'] and f['static'] and f['address']=='269ba70'
    b=span(text,NEW);assert b
    assert sha(function_body(text[b[0]:b[1]],NEW).encode())==f['reconstruction_body_sha256']
    from canonicalize_reanimation import verify as verify_domain
    assert verify_domain(root)
    # The native proof explicitly distinguishes the five physical arguments
    # from three optimized-away source-only debug parameters.
    i=json.loads((root/'docs/recovery-evidence/interface-recovery.json').read_text())['reanimation_2022']
    p=i['parameters']['269ba70'];assert len(p)==8
    assert [x['name'] for x in p[:5]]==['player','card','targ_player','this_test','action']
    assert all('DW_OP_GNU_parameter_ref' in str(x['location']) for x in p[5:])
    return dict(retired_unused_private_bodies=1,retained_name=OLD,recovered_name=NEW,
                scope='No compatibility alias or callback adapter: no remaining C reference to the private predecessor; native five-argument implementation preserved and independently audited')

def main():
    p=argparse.ArgumentParser(__doc__);p.add_argument('--apply',action='store_true');p.add_argument('--verify',action='store_true');args=p.parse_args()
    if args.verify:print(json.dumps(verify(),indent=2));return
    assert not (ROOT/EVIDENCE).exists()
    text=read(ROOT/SOURCE);b=span(text,OLD);assert b
    definition=text[b[0]:b[1]];after=text[:b[0]]+REPLACEMENT+text[b[1]:]
    assert not re.search(r'\b'+OLD+r'\b',mask(after))
    assert len(re.findall(r'\b(?:int|void)\s*\(\*',definition))==2
    m=json.loads((ROOT/'docs/recovery-evidence/recovered-functions.json').read_text())
    f=next(f for f in m['accepted_functions'] if f['name']==NEW)
    e=dict(source=SOURCE,retained_name=OLD,recovered_name=NEW,predecessor_definition=definition,
           predecessor_definition_sha256=sha(definition.encode()),predecessor_body=function_body(definition,OLD),
           predecessor_body_sha256=sha(function_body(definition,OLD).encode()),accepted_recovery_record=f,
           source_replacement=REPLACEMENT,reason='Public mass-reanimation frontends now share their native recovered bodies and call the five-argument worker. No live C token refers to this private eight-argument predecessor.')
    if args.apply:
        backup=ROOT/'.recovery/retired-reanimation-worker-before';assert not backup.exists();backup.mkdir()
        shutil.copy2(ROOT/SOURCE,backup/'functions.c')
        (ROOT/SOURCE).write_bytes(after.replace('\n','\r\n').encode('latin1'))
        (ROOT/EVIDENCE).write_text(json.dumps(e,indent=2)+'\n');verify()
    print(json.dumps(dict(applied=args.apply,retired_private_bodies=1)))

if __name__=='__main__':main()
