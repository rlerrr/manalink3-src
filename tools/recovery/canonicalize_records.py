"""Use the recovered October 2022 records throughout the rebuilt source tree.

Versioned typedefs remain spelling aliases for the immutable recovery records;
there is one structure definition and one layout for each runtime record.
"""
import argparse,json,re,shutil
from pathlib import Path
from unify_scalar_helpers import read,sha

ROOT=Path(__file__).resolve().parents[2]
EVIDENCE='docs/recovery-evidence/canonical-records.json'
NAMES=['target_definition','test_definition','token_generation','pump_ability']

def block(text,name):
    matches=list(re.finditer(r'typedef struct(?:\s+\w+)?\s*(?://[^\n]*)?\s*\{[^{}]*\}\s*'+name+r'\s*;',text))
    assert len(matches)==1,(name,len(matches))
    return matches[0]

def verify(root=ROOT):
    path=root/EVIDENCE
    if not path.exists():return {}
    proof=json.loads(path.read_text());text=read(root/'defs.h')
    interfaces=json.loads((root/'docs/recovery-evidence/interface-recovery.json').read_text())
    layouts={name+'_t':layout for name,layout in zip(NAMES,[interfaces['target_initializers_2022']['layout'],
             interfaces['test_core_2022']['layout'],interfaces['token_core_2022']['layout'],interfaces['ability_writers_2022']['pump_layout']])}
    for record in proof['records']:
        old=record['canonical_type'];modern=record['recovery_type']
        assert record['native_layout']==layouts[old] and record['bytes']==layouts[old]['bytes']
        assert sha(record['predecessor_definition'].encode())==record['predecessor_sha256']
        assert sha(record['recovered_definition'].encode())==record['recovered_sha256']
        assert block(text,old)[0]==record['canonical_definition']
        assert not re.search(r'\}\s*'+modern+r'\s*;',text)
        assert text.count('typedef '+old+' '+modern+';')==1
        assert sha(record['canonical_definition'].encode())==record['canonical_sha256']
    assert text.count(proof['token_action_definition'])==1
    assert text.count(proof['token_action_aliases'])==1
    assert proof['token_action_values']==interfaces['token_core_2022']['actions']['members']
    return dict(canonical_records=len(proof['records']),runtime_bytes={r['canonical_type']:r['bytes'] for r in proof['records']},
                versioned_types='typedef aliases to canonical records; no separate storage layouts',token_actions='October 2022 values')

def main():
    p=argparse.ArgumentParser(__doc__);p.add_argument('--apply',action='store_true');p.add_argument('--verify',action='store_true');a=p.parse_args()
    if a.verify:print(json.dumps(verify(),indent=2));return
    assert not (ROOT/EVIDENCE).exists(),'Already canonicalized'
    text=read(ROOT/'defs.h');before=text;records=[]
    interfaces=json.loads((ROOT/'docs/recovery-evidence/interface-recovery.json').read_text())
    layouts=[interfaces['target_initializers_2022']['layout'],interfaces['test_core_2022']['layout'],
             interfaces['token_core_2022']['layout'],interfaces['ability_writers_2022']['pump_layout']]
    for name,layout in zip(NAMES,layouts):
        old=name+'_t';modern=name+'_2022_t';legacy=block(text,old)[0];recovered=block(text,modern)[0]
        canonical=legacy
        if name=='target_definition':canonical=canonical.replace('} '+old+';','  int32_t flags;\n} '+old+';')
        elif name=='test_definition':canonical=canonical.replace('\tunsigned short int keyword_flag;','\tunsigned short int keyword_flag;\n\tunsigned int sp_keyword;\n\tunsigned short int sp_keyword_flag;')
        elif name=='token_generation':canonical=canonical.replace('\tint special_flags2;','\tint special_flags2;\n\tint special_flags3;')
        else:
            canonical=canonical.replace('\tint key_mod;','\tkeyword_t key_mod;').replace('\tint skey_mod;','\tsp_keyword_t skey_mod;')
            canonical=canonical.replace('} '+old+';','\tsubtype_t subtype;\n\tcolor_test_t color;\n\tint disabling_mode;\n} '+old+';')
        assert canonical!=legacy
        text=text.replace(legacy,canonical,1).replace(recovered,'typedef '+old+' '+modern+';',1)
        records.append(dict(canonical_type=old,recovery_type=modern,bytes=layout['bytes'],native_layout=layout,
                            predecessor_definition=legacy,predecessor_sha256=sha(legacy.encode()),
                            recovered_definition=recovered,recovered_sha256=sha(recovered.encode()),
                            canonical_definition=canonical,canonical_sha256=sha(canonical.encode())))
    actions=interfaces['token_core_2022']['actions']['members']
    action_block=re.search(r'typedef enum\s*\{[^{}]*\}\s*token_action_t;',text)[0]
    modern_block=re.search(r'typedef enum\s*\{[^{}]*\}\s*token_action_2022_t;',text)[0]
    # Keep the original field/action documentation while adopting native bits.
    canonical_action=action_block
    for name,number in actions.items():
        pattern=r'(?m)^(\s*'+re.escape(name)+r'\s*=\s*)[^,\n]+(,)'
        canonical_action,count=re.subn(pattern,lambda m:m[1]+str(number)+m[2],canonical_action)
        if not count:canonical_action=canonical_action.replace('} token_action_t;',f'\t{name} = {number},\n}} token_action_t;')
    aliases='typedef token_action_t token_action_2022_t;\n'+ '\n'.join('#define '+name.replace('TOKEN_ACTION_','TOKEN2022_ACTION_')+' '+name for name in actions)
    text=text.replace(action_block,canonical_action,1).replace(modern_block,aliases,1)
    for old,new in {
        '/* October 2022 test ABI; the original 208-byte definition remains available. */':'/* Recovery spelling aliases the canonical October 2022 test record. */',
        '/* October 2022 targeting layout; the original target_definition_t remains 80 bytes. */':'/* Recovery spelling aliases the canonical October 2022 target record. */',
        '/* October 2022 pump record; legacy 36-byte record remains above. */':'/* Recovery spelling aliases the canonical October 2022 pump record. */',
        '/* October 2022 token creation ABI; legacy layout/actions retained above. */':'/* Recovery spellings alias the canonical October 2022 token record/actions. */',
    }.items():assert old in text;text=text.replace(old,new,1)
    proof=dict(format=1,records=records,token_action_definition=canonical_action,token_action_aliases=aliases,
               token_action_values=actions,predecessor_defs_sha256=sha(before.encode()),canonical_defs_sha256=sha(text.encode()),
               scope='All rebuilt C callers use the recovered runtime layouts. Versioned typedefs are provenance spelling aliases, not old-layout ABI adapters. Historical recovery input records remain immutable.')
    if a.apply:
        backup=ROOT/'.recovery/canonical-records-before';assert not backup.exists();backup.mkdir()
        shutil.copy2(ROOT/'defs.h',backup/'defs.h')
        (ROOT/'defs.h').write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (ROOT/EVIDENCE).write_text(json.dumps(proof,indent=2)+'\n')
        verify()
    print(json.dumps(dict(applied=a.apply,records=[(r['canonical_type'],r['bytes']) for r in records]),indent=2))

if __name__=='__main__':main()
