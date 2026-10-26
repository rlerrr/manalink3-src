"""Use native storm mode bits for original and recovered helper callers."""
import argparse,json,re,shutil
from pathlib import Path
from unify_scalar_helpers import ROOT,read,sha
from canonicalize_reanimation import block,values
from canonicalize_spell_tutor import project

EVIDENCE='docs/recovery-evidence/canonical-storm.json'
RULES='functions/rules_engine.c'
OLD_ASSERT='STATIC_ASSERT(STORM_CREATURE == 1, storm_legacy_creature_mode_preserved);'
NEW_ASSERT='STATIC_ASSERT(STORM_CREATURE == 64, storm_canonical_creature_mode);'

def aliases(members):
    return 'typedef storm_mode_t storm_mode_2022_t;\n'+'\n'.join('#define '+n.replace('STORM_','STORM2022_',1)+' '+n for n in members)

def verify(root=ROOT):
    path=root/EVIDENCE
    if not path.exists():return {}
    e=json.loads(path.read_text());i=json.loads((root/'docs/recovery-evidence/interface-recovery.json').read_text())['storm_family_native_2022']
    members=i['state']['enum_values'];assert e['native_members']==members and e['native_debug_sha256']==i['debug_sha256']
    old=e['predecessor_definition'];assert sha(old.encode())==e['predecessor_sha256']
    # The five unchanged color bits were named through their shared constants.
    expanded=old
    for name,number in members.items():
        if name.startswith('STORM_') and name not in ['STORM_CREATURE','STORM_ENCHANTMENT','STORM_INSTANT_SORCERY']:
            expanded=expanded.replace(name.replace('STORM_','COLOR_TEST_'),str(number))
    prior=values(expanded);assert prior=={n:(1 if n=='STORM_CREATURE' else v) for n,v in members.items() if n not in ['STORM_ENCHANTMENT','STORM_INSTANT_SORCERY']}
    canonical=project(expanded,'storm_mode_t',members)
    assert canonical==e['canonical_definition'] and sha(canonical.encode())==e['canonical_sha256']
    assert values(e['recovered_definition'])=={n.replace('STORM_','STORM2022_',1):v for n,v in members.items()}
    assert sha(e['recovered_definition'].encode())==e['recovered_sha256']
    text=read(root/'defs.h');assert block(text,'storm_mode_t')==canonical and text.count(aliases(members))==1
    assert not re.search(r'\}\s*storm_mode_2022_t\s*;',text)
    rules=read(root/RULES);assert rules.count(NEW_ASSERT)==1 and OLD_ASSERT not in rules
    return dict(canonical_type='storm_mode_t',recovery_type='storm_mode_2022_t',native_values=members,bytes=4,
                scope='Native mode bits for all rebuilt callers; versioned type/constants are aliases. Only a compile-time assertion changes in the rules source.')

def main():
    p=argparse.ArgumentParser(__doc__);p.add_argument('--apply',action='store_true');p.add_argument('--verify',action='store_true');args=p.parse_args()
    if args.verify:print(json.dumps(verify(),indent=2));return
    assert not (ROOT/EVIDENCE).exists()
    i=json.loads((ROOT/'docs/recovery-evidence/interface-recovery.json').read_text())['storm_family_native_2022'];members=i['state']['enum_values']
    from inspect_release import Image
    from recover_storm_family import state_evidence,DEFINITIONS
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();assert sha(data)==i['debug_sha256']
    assert state_evidence(Image(data))==i['state']
    text=read(ROOT/'defs.h');old=block(text,'storm_mode_t');modern=block(text,'storm_mode_2022_t');expanded=old
    for name,number in members.items():expanded=expanded.replace(name.replace('STORM_','COLOR_TEST_'),str(number))
    canonical=project(expanded,'storm_mode_t',members)
    text=text.replace(old,canonical,1).replace(modern,aliases(members),1).replace('/* October 2022 storm mode domain; retain the legacy storm_mode_t. */','/* Recovery spelling aliases the canonical native storm modes. */',1)
    rules=read(ROOT/RULES);assert rules.count(OLD_ASSERT)==1;rules=rules.replace(OLD_ASSERT,NEW_ASSERT,1)
    e=dict(predecessor_definition=old,predecessor_sha256=sha(old.encode()),recovered_definition=modern,recovered_sha256=sha(modern.encode()),
        canonical_definition=canonical,canonical_sha256=sha(canonical.encode()),native_members=members,native_debug_sha256=sha(data),
        source_assertion=dict(original=OLD_ASSERT,replacement=NEW_ASSERT),scope='Canonical native storm mode plus updated compile-time assertion; accepted native implementation records unchanged.')
    if args.apply:
        backup=ROOT/'.recovery/canonical-storm-before';assert not backup.exists();backup.mkdir()
        for name in ['defs.h',RULES]:
            target=backup/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(ROOT/name,target)
        (ROOT/'defs.h').write_bytes(text.replace('\n','\r\n').encode('latin1'));(ROOT/RULES).write_bytes(rules.replace('\n','\r\n').encode('latin1'))
        (ROOT/EVIDENCE).write_text(json.dumps(e,indent=2)+'\n');verify()
    print(json.dumps(dict(applied=args.apply,canonical_storm_modes=members)))

if __name__=='__main__':main()
