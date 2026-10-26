"""Use one signed, native reanimation action domain in all rebuilt callers."""
import argparse,ast,json,re,shutil
from pathlib import Path
from unify_scalar_helpers import read,sha
from promote_recovery import REANIMATION_ENUM_VALUES,REANIMATION_DOMAIN_NAMES,strip_comments

ROOT=Path(__file__).resolve().parents[2]
EVIDENCE='docs/recovery-evidence/canonical-reanimation.json'

def block(text,name):
    matches=list(re.finditer(r'typedef enum\s*\{[^{}]*\}\s*'+name+r'\s*;',text))
    assert len(matches)==1,(name,len(matches))
    return matches[0][0]

def values(definition):
    def integer(node):
        if isinstance(node,ast.Constant) and type(node.value) is int:return node.value
        if isinstance(node,ast.UnaryOp) and isinstance(node.op,ast.USub):return -integer(node.operand)
        if isinstance(node,ast.BinOp) and isinstance(node.op,ast.LShift):return integer(node.left)<<integer(node.right)
        raise AssertionError(('Unreviewed enum expression',ast.dump(node)))
    result={}
    for entry in strip_comments(definition).split('{',1)[1].split('}',1)[0].split(','):
        if not entry.strip():continue
        name,expression=entry.split('=',1);name=name.strip();assert name not in result
        result[name]=integer(ast.parse(expression.strip(),mode='eval').body)
    return result

def project(predecessor):
    old=values(predecessor)
    assert all(REANIMATION_ENUM_VALUES[n]==v for n,v in old.items())
    missing={n:v for n,v in REANIMATION_ENUM_VALUES.items() if n not in old}
    assert missing=={'REANIMATE_ATTACH_AURA_TO_REANIMATOR':65536,'REANIMATEXTRA_EXILE_FROM_GRAVEYARD':-3}
    return predecessor.replace('} reanimate_mode_t;',
        '\tREANIMATE_ATTACH_AURA_TO_REANIMATOR = 65536,\n'
        '\tREANIMATEXTRA_EXILE_FROM_GRAVEYARD = -3,\n} reanimate_mode_t;')

def aliases():
    return 'typedef reanimate_mode_t reanimate_mode_2022_t;\n'+'\n'.join(
        '#define '+REANIMATION_DOMAIN_NAMES[n]+' '+n for n in REANIMATION_ENUM_VALUES)

def verify(root=ROOT):
    path=root/EVIDENCE
    if not path.exists():return {}
    proof=json.loads(path.read_text());assert proof['format']==1
    component=json.loads((root/'docs/recovery-evidence/interface-recovery.json').read_text())['reanimation_2022']
    assert proof['native_types']==component['types']
    assert proof['native_types']['enum_bytes']==4 and proof['native_types']['enum_values']==REANIMATION_ENUM_VALUES
    assert sha(proof['predecessor_definition'].encode())==proof['predecessor_sha256']
    assert sha(proof['recovered_definition'].encode())==proof['recovered_sha256']
    assert values(proof['recovered_definition'])=={REANIMATION_DOMAIN_NAMES[n]:v for n,v in REANIMATION_ENUM_VALUES.items()}
    canonical=project(proof['predecessor_definition'])
    assert canonical==proof['canonical_definition'] and sha(canonical.encode())==proof['canonical_sha256']
    text=read(root/'defs.h')
    assert block(text,'reanimate_mode_t')==canonical and values(canonical)==REANIMATION_ENUM_VALUES
    assert text.count(aliases())==1 and not re.search(r'\}\s*reanimate_mode_2022_t\s*;',text)
    header=read(root/'manalink.h')
    assert 'typedef int (*CardFunction)(int, int, event_t);' in text
    assert proof['native_debug_sha256']==component['debug_sha256']
    assert 'typedef void (*ReanimationBeforeEtb_2022)(int, int, int, int);' in header
    return dict(canonical_type='reanimate_mode_t',recovery_type='reanimate_mode_2022_t',
                bytes=4,signed=True,values=REANIMATION_ENUM_VALUES,callback_parameters=component['types'])

def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--verify',action='store_true');args=parser.parse_args()
    if args.verify:print(json.dumps(verify(),indent=2));return
    assert not (ROOT/EVIDENCE).exists(),'Already canonicalized'
    text=read(ROOT/'defs.h');before=text;old=block(text,'reanimate_mode_t');modern=block(text,'reanimate_mode_2022_t');canonical=project(old)
    component=json.loads((ROOT/'docs/recovery-evidence/interface-recovery.json').read_text())['reanimation_2022']
    # Re-read native DWARF, including the two callback types, before migration.
    from inspect_release import Image
    from recover_reanimation import type_evidence
    native=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    assert sha(native)==component['debug_sha256']
    assert type_evidence(Image(native))==component['types']
    assert values(modern)=={REANIMATION_DOMAIN_NAMES[n]:v for n,v in REANIMATION_ENUM_VALUES.items()}
    text=text.replace(old,canonical,1).replace(modern,aliases(),1)
    text=text.replace('/* October 2022 signed reanimation action domain. */','/* Recovery spellings alias the canonical signed reanimation domain. */',1)
    proof=dict(format=1,predecessor_definition=old,predecessor_sha256=sha(old.encode()),
        recovered_definition=modern,recovered_sha256=sha(modern.encode()),
        canonical_definition=canonical,canonical_sha256=sha(canonical.encode()),native_types=component['types'],
        predecessor_defs_sha256=sha(before.encode()),canonical_defs_sha256=sha(text.encode()),
        native_debug_sha256=sha(native),scope='Single signed native enum, unchanged old values plus the two missing native members. Versioned type and constants are spelling aliases. Native callback types re-read before migration; accepted recovery records unchanged.')
    if args.apply:
        backup=ROOT/'.recovery/canonical-reanimation-before';assert not backup.exists();backup.mkdir()
        for name in ['defs.h','manalink.h','docs/recovery-evidence/recovered-functions.json','docs/recovery-evidence/interface-recovery.json']:
            target=backup/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(ROOT/name,target)
        shutil.copy2(ROOT/'.git/index',backup/'git-index')
        (ROOT/'defs.h').write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (ROOT/EVIDENCE).write_text(json.dumps(proof,indent=2)+'\n');verify()
    print(json.dumps(dict(applied=args.apply,canonical_type='reanimate_mode_t',members=len(REANIMATION_ENUM_VALUES),new_members=2)))

if __name__=='__main__':main()
