"""Make two recovered empty terminal labels valid in GCC 9 C."""
import argparse,copy,json,re
from pathlib import Path
from audit_source import function_body
from recover_test_consumers import read
from recover_temporary_exile import save_json,sha

LABELS={'222eed0':'LAB_0222efdd','25d0d20':'LAB_025d0d5c'}


def historical_view(entry,body,binding=None):
    migration=entry['gnu_empty_label_portability_2022'];previous=migration['previous_record'];body=body.replace('\r\n','\n')
    if sha(body.encode())!=migration['body_sha256']:raise ValueError('GNU empty-label current body mismatch')
    if body.count(migration['replacement'])!=1:raise ValueError('GNU empty-label replacement membership mismatch')
    restored=body.replace(migration['replacement'],migration['original'],1)
    if sha(restored.encode())!=migration['previous_body_sha256']:raise ValueError('GNU empty-label exact predecessor mismatch')
    if binding and (binding not in previous or entry[binding]!=previous[binding]):raise ValueError('GNU empty-label historical interface mismatch')
    return previous,restored


def audit_component(component,manifest,sources):
    entries={f['address']:f for f in manifest['accepted_functions']};assert set(component['functions'])==set(LABELS)
    for a,label in LABELS.items():
        entry=entries[a];migration=component['functions'][a];assert entry['gnu_empty_label_portability_2022']==migration
        body=function_body(sources[entry['source']],entry['name']);previous,restored=historical_view(entry,body)
        assert migration['label']==label and migration['replacement']==migration['original']+migration['indent']+'  ;\n'
        assert re.fullmatch(r'[ \t]*'+label+r':\s*\n',migration['original'])
        assert entry['reconstruction_body_sha256']==migration['body_sha256']
        if a=='25d0d20':assert previous['native_life_callee_binding_2022']==entry['native_life_callee_binding_2022']


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'gnu_empty_labels_2022' not in interfaces
    entries={f['address']:f for f in m['accepted_functions']};sources={entries[a]['source']:read(root/entries[a]['source']) for a in LABELS}
    component=dict(functions={},reason='GCC 9 C requires a statement after the final label in a compound statement. Append only a null statement; preserve the native branch target and all existing expressions. No native behavior is added.')
    for a,label in LABELS.items():
        entry=entries[a];source=entry['source'];before=function_body(sources[source],entry['name']);match=re.search(r'(?m)^([ \t]*)'+label+r':[ \t]*\n(?=[ \t]*\})',before);assert match
        original=match[0];replacement=original+match[1]+'  ;\n';after=before.replace(original,replacement,1)
        migration=dict(label=label,indent=match[1],original=original,replacement=replacement,previous_record=copy.deepcopy(entry),previous_body_sha256=sha(before.encode()),body_sha256=sha(after.encode()))
        component['functions'][a]=migration;entry['gnu_empty_label_portability_2022']=migration;entry['reconstruction_body_sha256']=migration['body_sha256']
        sources[source]=sources[source].replace(before,after,1)
    audit_component(component,m,sources)
    if args.apply:
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        save_json(mp,m);interfaces['gnu_empty_labels_2022']=component;save_json(ip,interfaces)
    print(json.dumps(dict(applied=args.apply,terminal_labels=2,added_statements='null only',historical_scalar_bindings_preserved=1)))


if __name__=='__main__':main()
