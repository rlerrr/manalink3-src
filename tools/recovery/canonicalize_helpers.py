"""Mechanically retain recovered helper bodies under their original C names.

The candidate is built separately first. Every changed byte is reproduced from
checked predecessor files and a deterministic name/body projection. Native
addresses and recovery-ledger names remain separate provenance identities.
"""
import argparse,json,re,shutil,zipfile,inspect
from contextlib import contextmanager
from pathlib import Path
from unify_scalar_helpers import ROOT,read,sha,span,mask,scalar_signature
from audit_source import function_body

PLAN=Path('.recovery/canonical-helper-plan.json')
CANDIDATE=Path('.recovery/canonical-helper-candidate')
EVIDENCE=Path('docs/recovery-evidence/helper-canonicalization.json')
BEFORE=Path('.recovery/canonical-helper-before.zip')
_historical_active=False
# These units have local records, state, or private worker declarations between
# the retained and recovered definitions. Keep their native dependency ordering.
KEEP_NATIVE_ORDER=set('''cards/aether_revolt.c cards/betrayers_of_kamigawa.c
cards/legends.c cards/new_phyrexia.c functions/abilities.c functions/ai.c
functions/auras.c functions/engine.c functions/functions.c
functions/play_spell_for_free.c functions/produce_mana.c functions/rules_engine.c
functions/sacrifices.c functions/targets.c functions/token_generation.c
functions/tutors.c'''.split())

def rename(text,mapping):
    code=mask(text)
    pattern=re.compile(r'\b(?:'+'|'.join(re.escape(n) for n in sorted(mapping,key=len,reverse=True))+r')\b')
    for match in reversed(list(pattern.finditer(code))):
        text=text[:match.start()]+mapping[match[0]]+text[match.end():]
    return text

def declarations(text,name):
    """Function declarations only, excluding definitions and return statements."""
    if name not in text:return []
    code=mask(text);result=[]
    # Search the identifier first. A declaration-prefix regex applied to every
    # line can backtrack exponentially over the legacy files' long blank runs.
    for match in re.finditer(r'\b'+re.escape(name)+r'\s*\(',code):
        lo=code.rfind('\n',0,match.start())+1
        if not code[lo:match.start()].strip() and lo:
            lo=code.rfind('\n',0,lo-1)+1
        prefix=code[lo:match.start()]
        if re.search(r'\b(?:return|goto|case|else)\b',prefix):continue
        if not re.fullmatch(r'\s*(?:(?:static|extern)\s+)?(?:const\s+)?[A-Za-z_]\w*[\s*]+',prefix):continue
        depth=1;hi=match.end()
        while hi<len(code) and depth:
            depth+=(code[hi]=='(')-(code[hi]==')');hi+=1
        if depth:continue
        while hi<len(code) and code[hi].isspace():hi+=1
        if hi<len(code) and code[hi]==';':result.append(text[lo:hi+1])
    return result

def qualify_head(head,entry):
    name=entry['recovery_name']
    if name not in head:name=entry['canonical_name']
    if entry.get('const_return_annotation') and not re.search(r'\bconst\s+char\s*\*',head.split(name,1)[0]):
        head,count=re.subn(r'\bchar\s*\*','const char *',head,count=1);assert count==1
    for parameter in entry.get('const_parameters',[]):
        if re.search(r'\bconst\s+char\s*\*\s*'+re.escape(parameter)+r'\b',head):continue
        head,count=re.subn(r'\bchar\s*\*\s*'+re.escape(parameter)+r'\b','const char *'+parameter,head)
        assert count==1,(entry['recovery_name'],parameter,head)
    return head

def qualify_definition(definition,entry):
    head,body=definition.split('{',1)
    for name in entry.get('const_locals',[]):
        pattern=r'(?m)^([ \t]*)char\s*\*\s*'+re.escape(name)+r'\s*;'
        body,count=re.subn(pattern,lambda m:m[1]+'const char *'+name+';',body)
        assert count==1,(entry['recovery_name'],name)
    return qualify_head(head,entry)+'{'+body

def project(source,text,entries,mapping):
    original=text;edits=[];declarations=[]
    for e in entries:
        if e['source']!=source:continue
        old=e['canonical_name'];new=e['recovery_name'];b=span(original,new);assert b,new
        native=original[b[0]:b[1]];assert sha(native.encode())==e['live_definition_sha256']
        if e.get('rename_only'):
            if e.get('const_return_annotation') or e.get('const_parameters') or e.get('const_locals'):
                edits.append((b[0],b[1],qualify_definition(native,e)))
            continue
        body=native
        if e.get('const_return_annotation') or e.get('const_parameters') or e.get('const_locals'):
            body=qualify_definition(body,e)
        if e.get('alias_source_replacement'):
            old_text=e['alias_source_replacement'];assert original.count(old_text)==1
            lo=original.index(old_text);hi=lo+len(old_text)
        else:
            bounds=span(original,old);assert bounds,old
            lo,hi=bounds
            assert sha(original[lo:hi].encode())==e['retained_definition_sha256']
        if source in KEEP_NATIVE_ORDER or e.get('keep_native_order'):
            edits.extend([(b[0],b[1],body),(lo,hi,'')])
        else:
            edits.extend([(b[0],b[1],''),(lo,hi,body)])
        if not e.get('no_forward_declaration'):declarations.append(e['source_prototype'])
    ordered=sorted(edits)
    assert all(a[1]<=b[0] for a,b in zip(ordered,ordered[1:])),source
    for lo,hi,replacement in reversed(ordered):text=text[:lo]+replacement+text[hi:]
    # Definitions move to the old source positions. A compact declaration block
    # handles dependencies introduced by the recovered algorithms, including
    # private helpers whose old implementation had a different ordering.
    if declarations:
        include=re.search(r'(?m)^#include\s+"manalink\.h"[^\n]*(?:\n|$)',text);assert include,source
        block='\n/* Canonical recovered helper declarations. */\n'+'\n'.join(dict.fromkeys(declarations))+'\n'
        text=text[:include.end()]+block+text[include.end():]
    return rename(text,mapping)

def apply_source_edits(source,text,edits):
    """Replay reviewed interface/domain edits after canonical body placement."""
    for edit in edits:
        if edit['source']!=source:continue
        old=edit['before'];new=edit['after']
        assert sha(old.encode())==edit['before_sha256'],source
        assert sha(new.encode())==edit['after_sha256'],source
        assert text.count(old)==1,(source,edit['reason'])
        text=text.replace(old,new,1)
    return text

def project_file(source,before,entries,mapping,sharing,annotations=(),retirements=(),source_edits=()):
    after=project(source,before,entries,mapping)
    for e in entries:
        if e.get('cleanup_retained_declarations'):
            for declaration in declarations(before,e['canonical_name']):
                old=rename(declaration,mapping)
                assert old in after,(source,e['canonical_name'],declaration)
                after=after.replace(old,'',1)
        if not e.get('const_parameters') and not e.get('const_return_annotation'):continue
        # Include forward declarations and header declarations in the exact
        # qualifier-only projection. Definitions are handled above.
        for declaration in declarations(after,e['canonical_name']):
            after=after.replace(declaration,qualify_head(declaration,e),1)
    if source=='manalink.h':
        for e in sharing['helpers']:
            if e.get('prototype_scope','header')=='header':
                p=e.get('canonical_retained_prototype',e['retained_prototype'])
                if before.count(p)==1:after=after.replace(p+'\n','',1)
    for e in annotations:
        if e['source']!=source:continue
        original=span(before,e['recovery_name']);assert original
        assert sha(before[original[0]:original[1]].encode())==e['live_definition_sha256']
        bounds=span(after,e['recovery_name']);assert bounds
        after=after[:bounds[0]]+qualify_definition(after[bounds[0]:bounds[1]],e)+after[bounds[1]:]
        for declaration in declarations(after,e['recovery_name']):
            after=after.replace(declaration,qualify_head(declaration,e),1)
    for e in retirements:
        if e['source']!=source:continue
        b=span(before,e['name']);assert b and sha(before[b[0]:b[1]].encode())==e['definition_sha256']
        b=span(after,e['name']);assert b
        after=after[:b[0]]+after[b[1]:]
    return apply_source_edits(source,after,source_edits)

def verify_native_equivalences(proof,root=ROOT):
    entries=proof.get('native_equivalences',[])
    if not entries:return []
    from verify_helper_equivalences import verify_bounds
    verify_bounds(root,entries)
    from inspect_release import Image
    from helper_fingerprints import LocalCode
    native=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());native.pe.parse_data_directories()
    assert sha(native.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    result=[]
    for e in entries:
        functions={int(a,16):{int(a,16)} for a in e['callees']}
        code=LocalCode(native.pe,functions);fingerprints=[]
        for label in ['canonical','duplicate']:
            address=int(e[label+'_address'],16);size=e['bytes']
            raw=native.pe.get_data(address-native.pe.OPTIONAL_HEADER.ImageBase,size)
            assert len(raw)==size and sha(raw)==e[label+'_sha256']
            f=code.fingerprint(address,size,[s.encode('latin1') for s in e['readonly_strings']])
            assert not f.issues and f.sha256==e['fingerprint_sha256'],(label,f.issues)
            fingerprints.append(f.instructions)
        assert fingerprints[0]==fingerprints[1]
        result.append(dict(canonical_address=e['canonical_address'],duplicate_address=e['duplicate_address'],bytes=e['bytes'],fingerprint_sha256=e['fingerprint_sha256']))
    return result

def recipe_sha():
    return sha(('\n'.join(inspect.getsource(f) for f in [rename,declarations,qualify_head,qualify_definition,project,apply_source_edits,project_file])+'\n'+repr(sorted(KEEP_NATIVE_ORDER))).encode())

def load(root=ROOT):
    path=root/EVIDENCE
    return json.loads(path.read_text()) if path.exists() else None

def names(root=ROOT):
    proof=load(root)
    return {e['recovery_name']:e['canonical_name'] for e in proof['functions']} if proof else {}

def verify_interface_evidence(proof,root=ROOT):
    """Bind explicit caller edits to pinned release bytes and raw exports."""
    domain=proof.get('native_domain_evidence')
    if not domain:return {}
    assert sha((root/domain['path']).read_bytes())==domain['sha256']
    from verify_helper_domains import verify_source
    domains=verify_source(root)
    from inspect_release import Image
    image=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes())
    assert sha(image.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    ranges=set()
    for edit in proof.get('source_edits',[]):
        if 'native_address' in edit:
            lo=int(edit['native_address'],16);hi=int(edit['native_end'],16)
            assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==edit['native_bytes_sha256']
            ranges.add((lo,hi))
        for row in edit.get('native_caller_ranges',[]):
            lo=int(row['address'],16);hi=int(row['end'],16)
            assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==row['bytes_sha256']
            assert sha((root/row['raw_export']).read_bytes())==row['raw_export_sha256']
            ranges.add((lo,hi))
    return dict(native_domains=domains,caller_ranges=len(ranges),
                scope='Reviewed interface edits only; retained custom callers explicitly distinguished from native caller ranges.')

def verify(root=ROOT):
    proof=load(root)
    if not proof:return {}
    assert proof['recipe_sha256']==recipe_sha(),'Canonical projection recipe changed'
    assert sha((root/BEFORE).read_bytes())==proof['predecessor_archive_sha256']
    sharing=json.loads((root/'docs/recovery-evidence/helper-unification.json').read_text())
    assert sha(json.dumps(sharing['helpers'],sort_keys=True).encode())==proof['shared_helpers_sha256']
    ledger=json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text())
    accepted={f['name']:f for f in ledger['accepted_functions']}
    mapping=names(root)
    for e in proof['functions']:
        assert sha(json.dumps(accepted[e['recovery_name']],sort_keys=True).encode())==e['accepted_recovery_record_sha256']
    for e in proof.get('source_annotations',[]):
        assert sha(json.dumps(accepted[e['recovery_name']],sort_keys=True).encode())==e['accepted_recovery_record_sha256']
    with zipfile.ZipFile(root/BEFORE) as archive:
        for row in proof['files']:
            source=row['source'];before=archive.read(source).decode('latin1').replace('\r\n','\n')
            assert sha(before.encode())==row['predecessor_sha256'],source
            after=project_file(source,before,proof['functions'],mapping,sharing,proof.get('source_annotations',[]),proof.get('source_retirements',[]),proof.get('source_edits',[]))
            assert sha(after.encode())==row['canonical_sha256'],source
            assert read(root/source)==after,('Canonical source projection changed',source)
    return dict(canonical_functions=len(mapping),aliases_removed=proof['current_aliases_removed'],
                additional_direct_replacements=proof['additional_direct_replacements'],
                native_worker_names=sum(bool(e.get('rename_only')) for e in proof['functions']),
                equivalent_native_helpers=verify_native_equivalences(proof,root),
                native_interfaces=verify_interface_evidence(proof,root),
                changed_files=len(proof['files']),projection='exact replay from hash-checked predecessor archive',
                recovery_names='Historical ledger identities only; actual C definitions use canonical names')

@contextmanager
def historical_sources(root=ROOT):
    """Replay old algorithm proofs against checked predecessor C text.

    This view is explicitly limited to Python source readers; compilers and
    physical symbol checks always inspect the real canonical files.
    """
    global _historical_active
    proof=load(root)
    if not proof:
        yield;return
    verify(root)
    with zipfile.ZipFile(root/BEFORE) as archive:
        data={(root/r['source']).resolve():archive.read(r['source']) for r in proof['files']}
    old_bytes=Path.read_bytes;old_text=Path.read_text
    def read_bytes(path):return data.get(path.resolve(),None) if path.resolve() in data else old_bytes(path)
    def read_text(path,*args,**kwargs):
        if path.resolve() not in data:return old_text(path,*args,**kwargs)
        encoding=kwargs.get('encoding') or (args[0] if args else 'utf-8')
        errors=kwargs.get('errors') or (args[1] if len(args)>1 else 'strict')
        return data[path.resolve()].decode(encoding,errors).replace('\r\n','\n')
    Path.read_bytes=read_bytes;Path.read_text=read_text;_historical_active=True
    try:yield
    finally:Path.read_bytes=old_bytes;Path.read_text=old_text;_historical_active=False

def verify_symbols(image,root=ROOT):
    """Check physical canonical COFF entries, with no invented alias symbols."""
    proof=load(root)
    if not proof:return {}
    result={};symbols=image.symbols
    for e in proof['functions']:
        old='_'+e['canonical_name'];qualified='_'+e['recovery_name']
        assert not [s for s in symbols if s[0]==qualified or s[0].startswith(qualified+'.')],qualified
        found=[s for s in symbols if s[0]==old]
        if not found and e.get('allow_compiler_clone'):
            assert e['static']
            found=[s for s in symbols if re.fullmatch(re.escape(old)+r'\.(?:constprop|isra|part)\.\d+',s[0])]
        assert len(found)==1,(old,found)
        name,offset,section,kind,storage=found[0]
        assert section>0 and kind==32 and storage==(3 if e['static'] else 2),(old,found)
        assert image.pe.sections[section-1].Characteristics&0x20000000
        result[e['canonical_name']]=dict(coff_name=name,address=hex(image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[section-1].VirtualAddress+offset),recovery_name=e['recovery_name'])
    return dict(physical_canonical_symbols=len(result),removed_qualified_symbols=len(result),symbols=result)

def ledger_debug_view(image,root=ROOT):
    """Explicit logical-name view for existing per-family native range proofs.

    Physical symbols are checked first and reported separately. This does not
    modify a DLL, debug file, address, range, type, or instruction byte.
    """
    import copy
    physical=verify_symbols(image,root);proof=load(root)
    if not proof:return image,{}
    view=copy.copy(image);view.physical_image=image
    mapping=names(root);inverse={v:k for k,v in mapping.items()}
    view.symbols=list(image.symbols)
    for symbol in image.symbols:
        name=symbol[0]
        if not name.startswith('_'):continue
        base,*suffix=name[1:].split('.',1)
        if base in inverse:
            logical='_'+inverse[base]+('.'+suffix[0] if suffix else '')
            view.symbols.append((logical,*symbol[1:]))
    dwarf_cache=[]
    def dwarf():
        if dwarf_cache:return dwarf_cache[0]
        info=image.dwarf()
        for cu in info.iter_CUs():
            from inspect_release import value
            source=str(value(cu.get_top_DIE(),'DW_AT_name')).replace('\\','/')
            # Public declarations in callers' CUs refer to the same definition.
            # Private names are only translated in their owning source unit.
            owned={e['canonical_name']:e['recovery_name'] for e in proof['functions']
                   if not e['static'] or source==e['source'] or source.endswith('/'+e['source'])}
            if not owned:continue
            for die in cu.iter_DIEs():
                if die.tag!='DW_TAG_subprogram':continue
                attr=die.attributes.get('DW_AT_name')
                if attr and value(die,'DW_AT_name') in owned:
                    die.attributes['DW_AT_name']=attr._replace(value=owned[value(die,'DW_AT_name')].encode())
        dwarf_cache.append(info)
        return info
    view.dwarf=dwarf
    return view,dict(**physical,recovery_checks='Explicit ledger-name view over independently verified physical names; addresses, types, ranges and bytes are unchanged')

def apply(root=ROOT):
    assert not (root/EVIDENCE).exists(),'Canonical helpers already applied'
    proof=json.loads((root/PLAN).read_text())
    for row in proof['files']:
        assert sha(read(root/row['source']).encode())==row['predecessor_sha256']
        assert sha(read(root/CANDIDATE/row['source']).encode())==row['canonical_sha256']
    # Keep exact bytes, including encoding/newlines, for historical proof replay.
    with zipfile.ZipFile(root/BEFORE,'w',zipfile.ZIP_DEFLATED) as archive:
        for row in proof['files']:archive.write(root/row['source'],row['source'])
    proof.update(status='applied',recipe_sha256=recipe_sha(),predecessor_archive_sha256=sha((root/BEFORE).read_bytes()))
    sharing=json.loads((root/'docs/recovery-evidence/helper-unification.json').read_text())
    proof['shared_helpers_sha256']=sha(json.dumps(sharing['helpers'],sort_keys=True).encode())
    for row in proof['files']:shutil.copyfile(root/CANDIDATE/row['source'],root/row['source'])
    (root/EVIDENCE).write_text(json.dumps(proof,indent=2)+'\n')
    print(json.dumps(verify(root),indent=2))

def prepare():
    ledger=json.loads((ROOT/'docs/recovery-evidence/recovered-functions.json').read_text())
    accepted={f['name']:f for f in ledger['accepted_functions']}
    sharing=json.loads((ROOT/'docs/recovery-evidence/helper-unification.json').read_text())
    pairs=json.loads((ROOT/'.recovery/type-selection-pairs-preview.json').read_text())
    entries=[];used=set()
    for e in sharing['helpers']:
        f=accepted[e['recovered_name']];text=read(ROOT/e['source']);b=span(text,f['name']);assert b
        native=text[b[0]:b[1]]
        prototype=e.get('source_recovered_prototype',f['prototype'])
        annotate=e.get('method')=='const_return_adapter'
        if annotate:prototype=re.sub(r'\bchar\s*\*','const char *',prototype,count=1)
        replacement=e.get('source_replacement','/* Both public spellings share the recovered implementation; see helper-unification.json. */\n'+e['alias_declaration'])
        assert text.count(replacement)==1,e['retained_name']
        entries.append(dict(source=f['source'],canonical_name=e['retained_name'],recovery_name=f['name'],
            native_address=f['address'],static=f['static'],source_prototype=prototype,
            live_definition_sha256=sha(native.encode()),alias_source_replacement=replacement,
            const_return_annotation=annotate,accepted_recovery_record=f,origin='previously consolidated helper'))
        used.add(e['retained_name'])
    for row in pairs['functions']:
        if not row['status'].startswith('retained pair with matching'):continue
        old=row['retained_name'];new=row['recovered_name']
        if old in used or new!=old+'_2022':continue
        f=accepted[new];text=read(ROOT/f['source']);a=span(text,old);b=span(text,new);assert a and b
        definition=text[a[0]:a[1]];native=text[b[0]:b[1]]
        assert scalar_signature(definition.split('{',1)[0].strip()+';')==scalar_signature(f['prototype'])
        if 'reconstruction_body_sha256' in f:
            assert sha(function_body(native,new).encode())==f['reconstruction_body_sha256']
        else:
            assert new=='effect_this_dies_2022' and f['original_name']==old and f.get('interface_evidence')
            raw=(ROOT/'.recovery/decompiled-abi'/f"{f['address']}.c").read_text()
            assert sha(raw.encode())==f['raw_sha256']
            audit=json.loads((ROOT/'.recovery/recovery-audit.json').read_text())
            assert audit['functions']==len(ledger['accepted_functions']) and not audit['issues']
        entries.append(dict(source=f['source'],canonical_name=old,recovery_name=new,native_address=f['address'],static=f['static'],
            source_prototype=f['prototype'],live_definition_sha256=sha(native.encode()),
            retained_definition_sha256=sha(definition.encode()),accepted_recovery_record=f,
            origin='direct retained/recovered pair with matching reviewed C types'))
        used.add(old)
    assert len(entries)==len({e['canonical_name'] for e in entries})==len({e['recovery_name'] for e in entries})
    mapping={e['recovery_name']:e['canonical_name'] for e in entries}
    paths=set()
    for folder in ['cards','functions']:
        paths.update(p for p in (ROOT/folder).rglob('*') if p.is_file() and p.suffix.lower() in {'.c','.h','.cpp'})
    paths.update(p for p in ROOT.iterdir() if p.is_file() and p.suffix.lower() in {'.c','.h','.cpp'})
    for e in entries:
        record=e.pop('accepted_recovery_record')
        e['recovery_prototype']=record['prototype']
        e['accepted_recovery_record_sha256']=sha(json.dumps(record,sort_keys=True).encode())
    outputs={};changed=[]
    for path in sorted(paths):
        source=path.relative_to(ROOT).as_posix();before=read(path);after=project_file(source,before,entries,mapping,sharing)
        # The original public prototypes are obsolete after direct replacement;
        # the recovered declaration now supplies the canonical native contract.
        if after==before:continue
        outputs[source]=after
        changed.append(dict(source=source,predecessor_sha256=sha(before.encode()),canonical_sha256=sha(after.encode())))
    candidate=ROOT/CANDIDATE;candidate.mkdir(exist_ok=True)
    for source,text in outputs.items():
        target=candidate/source;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(text.replace('\n','\r\n').encode('latin1'))
    result=dict(format=1,status='candidate; repository C files untouched',functions=entries,files=changed,
        current_aliases_removed=len(sharing['helpers']),additional_direct_replacements=len(entries)-len(sharing['helpers']),
        inputs={n:sha((ROOT/n).read_bytes()) for n in ['docs/recovery-evidence/recovered-functions.json','docs/recovery-evidence/helper-unification.json','.recovery/type-selection-pairs-preview.json']},
        scope='Canonical native bodies and token-aware reference updates. Three pointer-return const annotations preserve callback C types without adapters. All other signature changes and compiler-worker identity exceptions remain separate work.')
    (ROOT/PLAN).write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(dict(canonical_functions=len(entries),aliases_removed=len(sharing['helpers']),additional_replacements=len(entries)-len(sharing['helpers']),changed_files=len(changed),candidate=str(candidate)),indent=2))

if __name__=='__main__':
    parser=argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply',action='store_true');parser.add_argument('--verify',action='store_true')
    args=parser.parse_args()
    if args.apply:apply()
    elif args.verify:print(json.dumps(verify(),indent=2))
    else:prepare()
