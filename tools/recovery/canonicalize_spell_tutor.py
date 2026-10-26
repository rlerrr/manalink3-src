"""Adopt native spell flags and tutoring values throughout the rebuilt tree."""
import argparse,json,re,shutil
from pathlib import Path
from canonicalize_reanimation import block,values
from unify_scalar_helpers import read,sha

ROOT=Path(__file__).resolve().parents[2]
EVIDENCE='docs/recovery-evidence/canonical-spell-tutor.json'
SPECS=[('generic_spell_flags_t','generic_spell_flags_2022_t','GS_','GS2022_'),
       ('tutor_t','tutor_destination_2022_t','TUTOR_','TUTOR2022_'),
       ('tutor_from_t','tutor_from_2022_t','TUTOR_FROM_','TUTOR_FROM2022_')]

def domains(interfaces):
    return [interfaces['generic_spell_2022']['flags']['members'],
            interfaces['global_tutor_core_2022']['destinations'],interfaces['global_tutor_core_2022']['origins']]

def project(predecessor,typename,members):
    old=values(predecessor);assert set(old)<=set(members)
    text=predecessor
    for name,number in members.items():
        if name in old:
            pattern=r'(?m)^(\s*'+re.escape(name)+r'\s*=\s*)[^,\n]+(,)'
            text,count=re.subn(pattern,lambda m:m[1]+str(number)+m[2],text);assert count==1,name
        else:text=text.replace('} '+typename+';',f'  {name} = {number},\n}} {typename};')
    assert values(text)==members
    return text

def aliases(spec,members):
    old,modern,prefix,replacement=spec
    return 'typedef '+old+' '+modern+';\n'+'\n'.join('#define '+n.replace(prefix,replacement,1)+' '+n for n in members)

def verify(root=ROOT):
    path=root/EVIDENCE
    if not path.exists():return {}
    proof=json.loads(path.read_text());assert proof['format']==1
    interfaces=json.loads((root/'docs/recovery-evidence/interface-recovery.json').read_text());text=read(root/'defs.h')
    assert len(proof['domains'])==len(SPECS)
    for entry,spec,members in zip(proof['domains'],SPECS,domains(interfaces)):
        old,modern,_,_=spec
        assert entry['canonical_type']==old and entry['recovery_type']==modern and entry['native_members']==members
        assert entry['native_bytes']==4 and entry['native_debug_sha256']==interfaces['generic_spell_2022']['debug_sha256']==interfaces['global_tutor_core_2022']['debug_sha256']
        assert sha(entry['predecessor_definition'].encode())==entry['predecessor_sha256']
        canonical=project(entry['predecessor_definition'],old,members)
        assert canonical==entry['canonical_definition'] and sha(canonical.encode())==entry['canonical_sha256']
        assert block(text,old)==canonical and text.count(aliases(spec,members))==1
        assert not re.search(r'\}\s*'+modern+r'\s*;',text)
    for name in ['recovered_spell_definition','recovered_tutor_definitions']:
        assert sha(proof[name].encode())==proof[name+'_sha256']
    return dict(canonical_domains=3,values={e['canonical_type']:e['native_members'] for e in proof['domains']},
                scope='Canonical native enum values and typedef/constant spelling aliases; not old-value runtime conversion adapters')

def main():
    p=argparse.ArgumentParser(__doc__);p.add_argument('--apply',action='store_true');p.add_argument('--verify',action='store_true');args=p.parse_args()
    if args.verify:print(json.dumps(verify(),indent=2));return
    assert not (ROOT/EVIDENCE).exists(),'Already canonicalized'
    interfaces=json.loads((ROOT/'docs/recovery-evidence/interface-recovery.json').read_text())
    text=read(ROOT/'defs.h');before=text;native_path=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg');native=native_path.read_bytes();digest=sha(native)
    from inspect_release import Image,value
    expected=domains(interfaces);found={};dwarf=Image(native).dwarf()
    for cu in dwarf.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {'functions/functions.c','functions/tutors.c'}:continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag!='DW_TAG_enumeration_type':continue
            members={value(d,'DW_AT_name'):value(d,'DW_AT_const_value') for d in die.iter_children() if d.tag=='DW_TAG_enumerator'}
            for index,key in enumerate(['GS_CAN_TARGET','TUTOR_HAND','TUTOR_FROM_DECK']):
                if key in members:
                    assert members==expected[index] and value(die,'DW_AT_byte_size')==4
                    found[index]=dict(die=hex(die.offset),source=value(cu.get_top_DIE(),'DW_AT_name'))
    assert set(found)=={0,1,2}
    spell=block(text,'generic_spell_flags_2022_t')
    from recover_global_tutor import definitions
    tutor=definitions(expected[1],expected[2]);assert text.count(tutor)==1
    entries=[]
    for spec,members in zip(SPECS,expected):
        old,modern,_,_=spec;predecessor=block(text,old);canonical=project(predecessor,old,members)
        text=text.replace(predecessor,canonical,1)
        entries.append(dict(canonical_type=old,recovery_type=modern,native_members=members,native_bytes=4,native_debug_sha256=digest,
            native_enum=found[len(entries)],predecessor_definition=predecessor,predecessor_sha256=sha(predecessor.encode()),
            canonical_definition=canonical,canonical_sha256=sha(canonical.encode())))
    text=text.replace(spell,aliases(SPECS[0],expected[0]),1)
    text=text.replace(tutor,'#ifndef SHANDALAR\n/* Recovery spellings alias native canonical tutor domains. */\n'+
        aliases(SPECS[1],expected[1])+'\n'+aliases(SPECS[2],expected[2])+'\n#endif',1)
    text=text.replace('/* October 2022 generic-spell flags; original values remain above. */','/* Recovery spelling aliases native canonical generic-spell flags. */',1)
    proof=dict(format=1,domains=entries,recovered_spell_definition=spell,recovered_spell_definition_sha256=sha(spell.encode()),
        recovered_tutor_definitions=tutor,recovered_tutor_definitions_sha256=sha(tutor.encode()),
        predecessor_defs_sha256=sha(before.encode()),canonical_defs_sha256=sha(text.encode()),
        scope='Whole-tree native value migration. GS bits 13/14 move to 14/15; native graveyard flag occupies bit13. Tutor destinations shift up by one with reserved zero, exile alias and Opposition Agent; origins retain values with DECK_AUTO added. Accepted native implementations remain unchanged. Literal caller operands remain subject to comparison/recovery review.')
    if args.apply:
        backup=ROOT/'.recovery/canonical-spell-tutor-before';assert not backup.exists();backup.mkdir()
        for name in ['defs.h','manalink.h','docs/recovery-evidence/recovered-functions.json','docs/recovery-evidence/interface-recovery.json']:
            target=backup/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(ROOT/name,target)
        shutil.copy2(ROOT/'.git/index',backup/'git-index')
        (ROOT/'defs.h').write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (ROOT/EVIDENCE).write_text(json.dumps(proof,indent=2)+'\n');verify()
    print(json.dumps(dict(applied=args.apply,domains=3,native_enum_identity=found)))

if __name__=='__main__':main()
