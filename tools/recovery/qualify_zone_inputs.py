"""Express three native read-only ID arrays without changing runtime expressions."""
import argparse,json,shutil,subprocess
from pathlib import Path
from unify_scalar_helpers import ROOT,read,sha,span
from audit_source import function_body

EVIDENCE='docs/recovery-evidence/zone-input-qualifiers.json'
SPECS={'get_average_value_from_zone':('s_location','piVar1'),
       'select_card_from_zone':('s_location',None),'count_zone_by_type':('zone','piVar1')}

def check_native_body(root,f,native):
    body=function_body(native,f['name'])
    if 'reconstruction_body_sha256' in f:
        assert sha(body.encode())==f['reconstruction_body_sha256']
    else:
        assert f['address']=='26e5c70' and f['name']=='count_zone_by_type_2022'
        from promote_recovery import clean
        raw=(root/'.recovery/decompiled-clones/26e5c70.c').read_text();assert sha(raw.encode())==f['raw_sha256']
        r=subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe','-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
                         input=('static '+clean(raw)).encode(),capture_output=True,check=True)
        assert function_body(r.stdout.decode().replace('\r\n','\n'),f['name'])==body
    return sha(body.encode())

def project(name,native):
    parameter,local=SPECS[name];head,body=native.split('{',1)
    token='int *'+parameter;assert head.count(token)==1 and 'const '+token not in head
    head=head.replace(token,'const '+token,1)
    if local:
        declaration='int *'+local+';';assert body.count(declaration)==1
        body=body.replace(declaration,'const '+declaration,1)
    return head+'{'+body

def definition(entry):
    q=entry['source_qualification'];name=entry['retained_name'];assert name in SPECS and entry['family']=='zone_inputs'
    assert q['parameter']==SPECS[name][0] and q['local_end_pointer']==SPECS[name][1]
    native=q['native_definition'];qualified=project(name,native)
    assert sha(native.encode())==q['native_definition_sha256'] and sha(qualified.encode())==q['qualified_definition_sha256']
    assert entry['source_recovered_prototype']==entry['recovered_prototype'].replace('int *'+q['parameter'],'const int *'+q['parameter'])
    return native,qualified

def audit_sources(sources,root=ROOT):
    path=root/EVIDENCE
    if not path.exists():return dict(functions=0)
    proof=json.loads(path.read_text());assert proof['format']==1 and set(proof['functions'])==set(SPECS)
    manifest=json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text())
    for name,e in proof['functions'].items():
        f=next(f for f in manifest['accepted_functions'] if f['name']==name+'_2022');assert f==e['accepted_recovery_record']
        native=e['native_definition'];qualified=project(name,native)
        assert sha(native.encode())==e['native_definition_sha256'] and sha(qualified.encode())==e['qualified_definition_sha256']
        assert check_native_body(root,f,native)==e['native_body_sha256']
        text=read(root/f['source']);b=span(text,f['name']);assert b and text[b[0]:b[1]]==qualified
        parameter,local=SPECS[name]
        assert e['parameter']==parameter and e['local_end_pointer']==local
        prototype=f['prototype'].replace('int *'+parameter,'const int *'+parameter)
        if not f['static']:assert prototype in read(root/'manalink.h')
        if f['source'] in sources:
            view=sources[f['source']].replace('\r\n','\n');b=span(view,f['name']);assert b and view[b[0]:b[1]]==qualified
            sources[f['source']]=view[:b[0]]+native+view[b[1]:]
    return dict(functions=3,scope='Only checked input/end-pointer const qualifiers; all executable expressions and accepted recovery records remain unchanged')

def main():
    p=argparse.ArgumentParser(__doc__);p.add_argument('--apply',action='store_true');p.add_argument('--verify',action='store_true');args=p.parse_args()
    if args.verify:print(json.dumps(audit_sources({}),indent=2));return
    assert not (ROOT/EVIDENCE).exists()
    manifest=json.loads((ROOT/'docs/recovery-evidence/recovered-functions.json').read_text());sources={};header=read(ROOT/'manalink.h');entries={}
    for name,(parameter,local) in SPECS.items():
        f=next(f for f in manifest['accepted_functions'] if f['name']==name+'_2022');text=sources.setdefault(f['source'],read(ROOT/f['source']));b=span(text,f['name']);assert b
        native=text[b[0]:b[1]];body_hash=check_native_body(ROOT,f,native)
        qualified=project(name,native);sources[f['source']]=text[:b[0]]+qualified+text[b[1]:]
        if not f['static']:
            assert header.count(f['prototype'])==1
            header=header.replace(f['prototype'],f['prototype'].replace('int *'+parameter,'const int *'+parameter),1)
        entries[name]=dict(parameter=parameter,local_end_pointer=local,native_definition=native,
            native_definition_sha256=sha(native.encode()),qualified_definition_sha256=sha(qualified.encode()),
            accepted_recovery_record=f,native_body_sha256=body_hash,
            reason='Zone counter and average-value scanner read IDs and advance local pointers. Selection reads indexed IDs and forwards the array only to the read-only display and average-value APIs; its writable AI selection array is separate.')
    if args.apply:
        backup=ROOT/'.recovery/zone-input-qualifiers-before';assert not backup.exists();backup.mkdir()
        for name in [*sources,'manalink.h','docs/recovery-evidence/recovered-functions.json','docs/recovery-evidence/interface-recovery.json','defs.h']:
            target=backup/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(ROOT/name,target)
        shutil.copy2(ROOT/'.git/index',backup/'git-index')
        for source,text in sources.items():(ROOT/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (ROOT/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        (ROOT/EVIDENCE).write_text(json.dumps(dict(format=1,functions=entries),indent=2)+'\n');audit_sources({})
    print(json.dumps(dict(applied=args.apply,read_only_inputs=3,runtime_expression_changes=0)))

if __name__=='__main__':main()
