"""Share activation bodies with a const-correct source view of native prompts.

DWARF/decompiler signatures and accepted recovery records remain unchanged.
The only C definition change is the prompt pointee qualifier, not its pointer
width, argument position, assignments, calls, or any executable expression.
"""
import argparse,json,shutil
from pathlib import Path
from unify_scalar_helpers import (ROOT,EVIDENCE,ACTIVATION_PROMPTS,read,sha,span,
                                 qualified_activation_definition,audit_entry,verify_source)

def audit_sources(sources,root=ROOT):
    data=json.loads((root/EVIDENCE).read_text());entries=[e for e in data['helpers'] if e.get('source_qualification') and e['family']=='activation_prompts']
    if not entries:return dict(functions=0)
    assert {e['retained_name'] for e in entries}==set(ACTIVATION_PROMPTS)
    context={}
    for e in entries:
        audit_entry(e,root,context)
        native,qualified=qualified_activation_definition(e)
        if e['source'] in sources:
            text=sources[e['source']].replace('\r\n','\n')
            b=span(text,e['recovered_name']);assert b and text[b[0]:b[1]]==qualified
            sources[e['source']]=text[:b[0]]+native+text[b[1]:]
    return dict(functions=len(entries),scope='Only the explicit prompt pointee qualifiers are reversed for historical source proofs. Actual definitions, declarations, aliases and all executable expressions are checked independently.')

def main():
    p=argparse.ArgumentParser(__doc__);p.add_argument('--apply',action='store_true');p.add_argument('--verify',action='store_true');args=p.parse_args()
    if args.verify:print(json.dumps(audit_sources({}),indent=2));return
    data=json.loads((ROOT/EVIDENCE).read_text());entries=[e for e in data['helpers'] if e.get('family')=='activation_prompts']
    assert len(entries)==3 and {e['retained_name'] for e in entries}==set(ACTIVATION_PROMPTS)
    header=read(ROOT/'manalink.h');sources={}
    for e in entries:
        assert not e.get('source_qualification'),'Already annotated'
        source=e['source'];text=sources.setdefault(source,read(ROOT/source));b=span(text,e['recovered_name']);assert b
        native=text[b[0]:b[1]];head,body=native.split('{',1)
        assert head.count('char *prompt')==1
        qualified=head.replace('char *prompt','const char *prompt')+'{'+body
        old=e.pop('canonical_retained_prototype');assert header.count(old)==1
        header=header.replace(old,e['retained_prototype'],1)
        assert header.count(e['recovered_prototype'])==1
        e['source_recovered_prototype']=e['recovered_prototype'].replace('char *prompt','const char *prompt')
        header=header.replace(e['recovered_prototype'],e['source_recovered_prototype'],1)
        e['source_qualification']=dict(parameter='prompt',native_pointee='char',source_pointee='const char',
            native_definition=native,native_definition_sha256=sha(native.encode()),qualified_definition_sha256=sha(qualified.encode()),
            reason='Generic and attachment entry points forward prompt to the granted-ability implementation. The granted implementation only passes it to load_text_2022 and select_target_2022, whose input declarations are const char pointers; its assignment changes the pointer to text_lines[0], not the pointed-to text.')
        qualified_activation_definition(e)
        e['rationale']='One recovered algorithm for both spellings. The prompt has a checked read-only source annotation; native pointer ABI and accepted recovery records are retained. No runtime adapter or caller casts.'
        sources[source]=text[:b[0]]+qualified+text[b[1]:]
    if args.apply:
        backup=ROOT/'.recovery/activation-prompt-qualification-before';assert not backup.exists();backup.mkdir()
        for source in [*sources,'manalink.h',str(EVIDENCE)]:
            dst=backup/source;dst.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(ROOT/source,dst)
        for source,text in sources.items():(ROOT/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (ROOT/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        (ROOT/EVIDENCE).write_text(json.dumps(data,indent=2)+'\n')
        verify_source();audit_sources({})
    print(json.dumps(dict(applied=args.apply,read_only_prompts=len(entries),runtime_expression_changes=0,accepted_record_changes=0),indent=2))

if __name__=='__main__':main()
