"""Prioritize existing helper source using reccmp JSON and strict local code.

No recovery records or C sources are modified. Scores rank review effort;
local-code identity does not prove transitive dependency/gameplay equivalence.
"""
import argparse,ast,collections,csv,difflib,json,re,subprocess,sys
from pathlib import Path
import pefile
from inspect_release import Image
from recover_temporary_exile import sha,save_json
from recover_test_consumers import read
from audit_source import function_body
from promote_recovery import strip_comments
from helper_fingerprints import LocalCode

ROOT=Path(__file__).resolve().parents[2]
CATEGORIES={
 'same_local_code':'Same local machine code after verified address translation; keep existing source and track dependencies.',
 'verified_retained_source':'Visible source with identical complete local instructions, matching DWARF calling/storage contracts and already reviewed direct call/reference targets; retain without another decompilation pass.',
 'callee_binding_only':'Same local instructions; retained callees need canonical 2022 binding/ABI review.',
 'reference_review':'High normalized score; unresolved code/data or indirect references prevent a strict identity claim.',
 'small_diff_review':'At least 90% normalized score; inspect the assembly diff and edit existing source before considering full decompilation.',
 'full_review':'Lower score or failed range/disassembly check; review may still expose compiler-only differences.',
 'mapping_first':'No unambiguous reccmp pair; fix pairing or compiler-part/inline coverage first.',
 'deferred_chooser':'User-deferred chooser UI.',
 'cpp_review':'C++/library helper, outside the initial C source triage.',
 'reviewed_cpp_source':'Complete C++ utility body, authored source and native ABI/storage manually reviewed; dependencies remain separately tracked.',
 'reviewed_compiler_source':'Complete native header specialization/instantiation and corresponding toolchain source reviewed; not authored recovery or strict rebuilt instruction identity.',
 'reconstructed_source':'Complete native body already reconstructed during consolidation; retain its provenance and track callees separately.',
}


def definition(text,name):
    text=strip_comments(text)
    start=re.search(r'(?m)^[ \t]*(?:[A-Za-z_]\w*[ \t\r\n*]+)+'+re.escape(name)+r'\s*\([^;{}]*\)\s*\{',text)
    if start is None:return None
    return function_body(text[start.start():],name)


def strings(body):
    result=set()
    for m in re.finditer(r'"(?:\\.|[^"\\])*"',strip_comments(body or '')):
        try:result.add(ast.literal_eval(m[0]).encode('latin1'))
        except (ValueError,SyntaxError,UnicodeEncodeError):pass
    return result


def source_key(path):return path.replace('\\','/').removeprefix('./')


def reconstructed_sources(proof,native):
    """Recognize explicitly tagged full bodies, never narrow caller witnesses.

    This inventory is deliberately conservative. Untagged source edits and
    partial interface/inline edits remain in the review queue. The canonical
    projection audit separately verifies every native/export hash and replays
    all later edits against the physical source.
    """
    from canonicalize_helpers import span,rename,mask
    mapping={e['recovery_name']:e['canonical_name'] for e in proof.get('functions',[])+proof.get('auxiliary_functions',[])}
    result={}
    tags={'keyword_writer','native_counter_caller','caller_preparation',
          'inlined_owner_recovery','native_source_reconstruction',
          'native_helper_identity','native_frontend_reconstruction'}
    narrow={'native_call_argument_witnesses','native_constant_argument_witnesses',
            'native_counter_argument_block_witness','native_inline_counter_argument_witness'}
    for index,edit in enumerate(proof.get('source_edits',[])):
        if edit.get('interface_only') or narrow.intersection(edit) or not tags.intersection(edit):continue
        after=rename(edit['after'],mapping)
        # A historical source span can include the tail of a preceding macro.
        # Explicit full-body records name their owner; find its real signature
        # with horizontal whitespace in the return type, so a macro's "case"
        # line cannot be mistaken for part of the function declaration.
        reconstruction=edit.get('native_source_reconstruction')
        if isinstance(reconstruction,dict) and reconstruction.get('function'):
            name=reconstruction['function']
            # Mask comments without shifting offsets. Permit one line break
            # before the function name ("static int\nfoo"), while keeping the
            # return-type token list horizontal so earlier macro tails cannot
            # become part of the declaration span.
            head=re.search(r'(?m)^[ \t]*(?:[A-Za-z_]\w*[ \t*]+)*[A-Za-z_]\w*(?=[ \t*\r\n])[ \t*]*(?:\r?\n[ \t*]*)?'
                           +re.escape(name)+r'\s*\([^;{}]*\)\s*\{',mask(after))
            if head is None:continue
            after=after[head.start():]
        match=re.match(r'\s*(?:[A-Za-z_]\w*[\s*]+)+?([A-Za-z_]\w*)\s*\(',after)
        if not match:continue
        name=match[1];body=span(after,name)
        if not body or after[:body[0]].strip() or after[body[1]:].strip():continue
        ranges=edit.get('native_caller_ranges',[])
        if not ranges:continue
        for row in ranges:
            at=int(row['address'],16)
            if at not in native:continue
            # A reviewed unit can have a different historical filename (e.g.
            # odyssey.c in release DWARF versus retained odissey.c). Full-body
            # receipts must name the native unit explicitly in that case.
            native_source=reconstruction.get('native_source',edit['source']) if isinstance(reconstruction,dict) else edit['source']
            assert source_key(native[at]['source'])==source_key(native_source)
            assert int(native[at]['high_pc'],16)==int(row['end'],16)
            result[at]=dict(source=edit['source'],source_function=name,source_edit=index,
                            source_edit_sha256=edit['after_sha256'],native_range=row)
    return result


def retain_local_matches(rows,reviewed_bodies):
    """Close body review from strict equality, never from a reccmp score.

    Each wave depends only on pre-existing body reviews or earlier waves.
    Cycles without an independently reviewed entry remain pending. A target
    body review is not a claim that its own transitive behavior is complete.
    """
    reviewed=set(reviewed_bodies);roots=set(reviewed);waves=[]
    def exact(row):
        counts=row.get('instruction_counts',[])
        fingerprint=row.get('original_fingerprint')
        return (row['category']=='same_local_code' and row.get('source_definition_found')
                and bool(row.get('source_body_sha256'))
                and isinstance(fingerprint,str) and len(fingerprint)==64
                and fingerprint==row.get('rebuilt_fingerprint')
                and len(counts)==2 and counts[0]>0 and counts[0]==counts[1]
                and row.get('storage_contract',{}).get('verified') is True
                and not row.get('review_reasons') and not row.get('binding_repairs'))
    while True:
        ready=sorted((row for row in rows if exact(row)
                      and {int(a,16) for a in row.get('dependencies',[])}<=reviewed),
                     key=lambda r:int(r['address'],16))
        if not ready:break
        wave=len(waves)+1
        for row in ready:
            row['category']='verified_retained_source'
            row['retention_evidence']=dict(method='strict_local_instruction_identity',wave=wave,
                source_body_sha256=row['source_body_sha256'],fingerprint_sha256=row['original_fingerprint'],
                contract_sha256=row['storage_contract']['fingerprint_sha256'],
                reviewed_direct_targets=row.get('dependencies',[]),source_edited=False,
                scope='Complete local body and recorded direct targets only; callees retain their independently tracked dependencies. No global-content or gameplay proof.')
        addresses=[row['address'] for row in ready];waves.append(addresses)
        reviewed.update(int(a,16) for a in addresses)
    # Validate the dependency order independently of the worklist selection.
    checked=set(roots);by_address={row['address']:row for row in rows}
    for number,addresses in enumerate(waves,1):
        for address in addresses:
            row=by_address[address];assert row['retention_evidence']['wave']==number
            assert {int(a,16) for a in row['dependencies']}<=checked,address
            assert row['original_fingerprint']==row['rebuilt_fingerprint']
            assert row['storage_contract']['verified'] is True
            assert not row['review_reasons'] and not row['binding_repairs']
        checked.update(int(a,16) for a in addresses)
    assert checked==reviewed
    for row in rows:
        row['unreviewed_dependencies']=[a for a in row.get('dependencies',[]) if int(a,16) not in reviewed]
    return dict(verified_ranges=sum(map(len,waves)),review_waves=waves,initial_reviewed_bodies=len(roots),
                scope='Current-build instruction identity and direct target body reviews. Recomputed on every run; cycles, unresolved references, indirect flow, stubs and missing source do not close review. Transitive dependencies remain independently tracked.')


def main():
    p=argparse.ArgumentParser(__doc__)
    p.add_argument('--report',type=Path,default=ROOT/'.recovery/reccmp/triage-current.json')
    p.add_argument('--bounds',type=Path,default=ROOT/'.recovery/reccmp/triage-rebuilt-bounds.json')
    p.add_argument('--output',type=Path,default=ROOT/'docs/recovery-evidence/helper-triage.json')
    p.add_argument('--summary',type=Path,default=ROOT/'docs/recovery-helper-triage.md')
    p.add_argument('--refresh',action='store_true',help='Refresh JSON/receipt and reuse or regenerate the hash-pinned rebuilt DWARF cache. Build/setup must already be current.')
    args=p.parse_args();cache=ROOT/'.recovery/reccmp'
    if args.refresh:
        subprocess.run([sys.executable,str(ROOT/'tools/reccmp_project.py'),'compare','--json',str(args.report),'--json-diet','--silent'],cwd=ROOT,check=True)
        sys.path.insert(0,str(ROOT/'tools'))
        from reccmp_project import read_functions
        debug_hash=sha((ROOT/'ManalinkEh.dbg').read_bytes())
        if not args.bounds.exists() or json.loads(args.bounds.read_text()).get('sha256')!=debug_hash:
            save_json(args.bounds,dict(sha256=debug_hash,functions=read_functions(ROOT/'ManalinkEh.dbg',all_functions=True)))
    setup=json.loads((cache/'setup.json').read_text())
    sys.path.insert(0,str(ROOT/'tools'))
    from reccmp_project import naming_state
    assert setup.get('naming_inputs')==naming_state(),'Source name mappings changed; rerun setup and comparison'
    for name,digest in setup['inputs'].items():assert sha(Path(name).read_bytes())==digest,name
    assert sha((ROOT/'config/reccmp/functions.csv').read_bytes())==setup['metadata_sha256']
    receipt=args.report.with_suffix('.inputs.json');receipt_data=json.loads(receipt.read_text())
    assert receipt_data['setup_sha256']==sha((cache/'setup.json').read_bytes()) and receipt_data['report_sha256']==sha(args.report.read_bytes())
    assert receipt_data['inputs']==setup['inputs'] and receipt_data['metadata_sha256']==setup['metadata_sha256']
    for name,digest in receipt_data.get('comparator_source_sha256',{}).items():assert sha(Path(name).read_bytes())==digest,name
    bounds=json.loads(args.bounds.read_text());assert bounds['sha256']==sha((ROOT/'ManalinkEh.dbg').read_bytes())
    manifest_path=ROOT/'docs/recovery-evidence/recovered-functions.json';manifest=json.loads(manifest_path.read_text())
    if 'recovery_ledger_sha256' in receipt_data:assert receipt_data['recovery_ledger_sha256']==sha(manifest_path.read_bytes())
    for header in ['manalink.h','defs.h','Expansions.h']:
        assert (ROOT/header).stat().st_mtime_ns<=(ROOT/'ManalinkEh.dll').stat().st_mtime_ns,('Header newer than DLL; rebuild before triage',header)
    accepted={int(f['address'],16):f for f in manifest['accepted_functions']}
    from canonicalize_helpers import names as canonical_names
    canonical=canonical_names(ROOT)
    unsafe={int(f['native_address'],16) for f in manifest['source_stubs']}
    native_path=ROOT/'.recovery/2022-functions.csv'
    native={int(r['low_pc'],16):r for r in csv.DictReader(native_path.open()) if r['low_pc'] and r['high_pc'] and int(r['high_pc'],16)>int(r['low_pc'],16)}
    data=json.loads(args.report.read_text());assert data['format']==1
    scores={int(r['address'],16):r for r in data['data']}
    assert len(scores)==len(data['data'])==setup['csv_functions']
    for a,f in accepted.items():
        if a in scores:
            expected=canonical.get(f['name'],f['name'])
            actual=scores[a]['name']
            # The matcher appends a stable numeric suffix when a canonical
            # name has multiple isolated emitted ranges.  That is still the
            # expected function identity, not stale recovery metadata.
            assert actual==expected or re.fullmatch(re.escape(expected)+r'\([1-9][0-9]*\)',actual),(hex(a),'stale recovery mapping')
    originals={a:{a} for a in native};current=collections.defaultdict(set)
    for a,r in scores.items():
        if r.get('recomp') and r['recomp']!='various':current[int(r['recomp'],16)].add(a)
    # GCC may split a canonical helper into a public entry plus a private part
    # with the same DWARF name. That prevents a full-body reccmp name match, but
    # its checked public COFF entry still identifies a valid call target.
    # Register identity only: do not fabricate a body score or hide collisions.
    from canonicalize_helpers import load as load_canonical,verify_symbols,verify_native_equivalences
    canonical_data=load_canonical(ROOT);canonical_entry_bindings={};canonical_inlined={}
    unsafe.update(int(f["native_address"],16) for f in
                  (canonical_data or {}).get("additional_source_stubs",[]))
    native_equivalences=verify_native_equivalences(canonical_data,ROOT) if canonical_data else []
    covered_native_helpers={int(e['duplicate_address'],16) for e in native_equivalences}
    reconstructed=reconstructed_sources(canonical_data or {},native)
    from verify_compiler_cpp_recovery import coverage as compiler_cpp_coverage
    compiler_cpp=compiler_cpp_coverage(canonical_data or {},ROOT)
    assert not (set(compiler_cpp)&unsafe),'Deferred UI cannot receive compiler/C++ review credit'
    reviewed_bodies=set(accepted)|set(reconstructed)|covered_native_helpers|set(compiler_cpp)
    for e in native_equivalences:
        originals[int(e['duplicate_address'],16)]={int(e['canonical_address'],16)}
    if canonical_data:
        canonical_debug=Image((ROOT/'ManalinkEh.dbg').read_bytes())
        checked_physical=verify_symbols(canonical_debug,ROOT)
        physical=checked_physical['symbols']
        canonical_inlined=checked_physical['inlined_canonical_functions']
        for e in canonical_data['functions']:
            if e['canonical_name'] in canonical_inlined:
                continue  # Caller-owned inline ranges have no callable helper entry.
            ident=int(e['native_address'],16);symbol=physical[e['canonical_name']]
            address=int(symbol['address'],16);current[address].add(ident)
            canonical_entry_bindings[e['canonical_name']]=dict(native_address=hex(ident),**symbol)
    by_source_name=collections.defaultdict(list);by_at=collections.defaultdict(list)
    for r in bounds['functions']:
        by_source_name[(source_key(r['source']),r['name'])].append(r);by_at[r['address']].append(r)
    original_keys=collections.defaultdict(set)
    for a,r in native.items():original_keys[(source_key(r['source']),r['name'])].add(a)
    legacy=collections.defaultdict(set)
    for key,ids in original_keys.items():
        if len(ids)==1:
            for r in by_source_name.get(key,[]):legacy[r['address']].update(ids)
    orig_pe=pefile.PE(str(ROOT/'.recovery/2022-ManalinkEh.dll'));new_pe=pefile.PE(str(ROOT/'ManalinkEh.dll'))
    # A verified compiler JMP and its retained target implement the same
    # accepted function. Both entry points are valid bindings; a reference to
    # the target does not need another suffixed caller or a source edit.
    from capstone.x86_const import X86_OP_IMM
    for folded in setup.get('folded_recovered_helpers',[]):
        ident=int(folded['native_address'],16);at=int(folded['address'],16);target=int(folded['target'],16)
        assert ident in accepted and accepted[ident]['name']==folded['name']
        assert accepted[ident]['source']==folded['source']==native[ident]['source']
        assert folded['retained_name']==native[ident]['name']
        branch=bytes.fromhex(folded['branch_bytes'])
        assert new_pe.get_data(at-new_pe.OPTIONAL_HEADER.ImageBase,len(branch))==branch
        from capstone import Cs,CS_ARCH_X86,CS_MODE_32
        decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
        if folded.get('representation','thunk')=='coff_alias':
            assert not branch and at==target
            alias_debug=Image((ROOT/'ManalinkEh.dbg').read_bytes())
            actual={alias_debug.pe.OPTIONAL_HEADER.ImageBase+alias_debug.pe.sections[section-1].VirtualAddress+offset
                    for name,offset,section,kind,storage in alias_debug.functions()
                    if name=='_'+folded['name'] and section>0 and kind==32}
            assert actual=={at},('COFF alias changed',folded['name'])
        else:
            assert folded.get('representation','thunk')=='thunk'
            first=list(decoder.disasm(branch,at))
            assert len(first)==1 and first[0].mnemonic=='jmp' and first[0].operands[0].type==X86_OP_IMM and first[0].operands[0].imm==target
        assert sha(new_pe.get_data(target-new_pe.OPTIONAL_HEADER.ImageBase,folded['target_bytes']))==folded['target_sha256']
        assert any(r['address']==target and r['size']==folded['target_bytes'] for r in by_source_name[(source_key(folded['source']),folded['retained_name'])])
        current[at].add(ident);current[target].add(ident)
    original_code=LocalCode(orig_pe,originals,accepted=accepted,unsafe=unsafe)
    rebuilt_code=LocalCode(new_pe,current,legacy,accepted,unsafe)
    # COFF finds genuine compiler thunks that have no separate DWARF body.
    debug=Image((ROOT/'ManalinkEh.dbg').read_bytes());coff=collections.defaultdict(set)
    for n,v,sec,kind,storage in debug.functions():
        if sec>0 and kind==32:coff[n.lstrip('_')].add(debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v)
    texts={};source_hashes={};bodies={}
    def body_for(source,name):
        if source not in texts:
            path=ROOT/source;texts[source]=read(path) if path.is_file() else ''
            if path.is_file():
                assert path.stat().st_mtime_ns<=(ROOT/'ManalinkEh.dll').stat().st_mtime_ns,('Source newer than DLL; rebuild before triage',source)
                source_hashes[source]=sha(path.read_bytes())
        key=(source,name)
        if key not in bodies:bodies[key]=definition(texts[source],name)
        return bodies[key]
    def unique_range(at,source=None,name=None):
        rr=by_at.get(at,[])
        if source is not None:rr=[r for r in rr if source_key(r['source'])==source_key(source)]
        if name is not None:rr=[r for r in rr if r['name']==name or re.fullmatch(re.escape(r['name'])+r'\([1-9][0-9]*\)',name)]
        sizes={r['size'] for r in rr}
        return next(iter(sizes)) if len(sizes)==1 else None
    rows=[]
    for a,r in native.items():
        if a in accepted or a in covered_native_helpers or r['name'].startswith('card_'):continue
        out=dict(address=hex(a),name=r['name'],source=r['source'],native_bytes=int(r['high_pc'],16)-a)
        score=scores.get(a);body=body_for(r['source'],r['name']);out['source_definition_found']=body is not None
        if body is not None:out['source_body_sha256']=sha(body.encode())
        if score is not None:
            out.update(rebuilt_address=score['recomp'],reccmp_score=score['matching'],effective_match=bool(score.get('effective')))
        if a in reconstructed:
            category='reconstructed_source';out['source_reconstruction']=reconstructed[a]
            physical_source=reconstructed[a]['source']
            recovered=body_for(physical_source,reconstructed[a]['source_function'])
            assert recovered is not None,(physical_source,reconstructed[a]['source_function'])
            if physical_source!=r['source']:
                out['source_definition_source']=physical_source
                out['source_definition_found']=True
                out['source_body_sha256']=sha(recovered.encode())
            out['reconstructed_body_sha256']=sha(recovered.encode())
            original=original_code.fingerprint(a,out['native_bytes'],strings(recovered))
            out['dependencies']=[hex(x) for x in sorted(set(original.dependencies))]
            out['unreviewed_dependencies']=[hex(x) for x in sorted(set(original.dependencies)-reviewed_bodies)]
        elif a in compiler_cpp:
            review=compiler_cpp[a];category=review['category']
            out.update(source_definition_found=True,source_definition_source=review['source'],
                       source_function=review['source_function'],source_body_sha256=review['source_body_sha256'],
                       source_review=review['review'])
            original=original_code.fingerprint(a,out['native_bytes'],strings(review['source_body']))
            out['dependencies']=[hex(x) for x in sorted(set(original.dependencies))]
            out['unreviewed_dependencies']=[hex(x) for x in sorted(set(original.dependencies)-reviewed_bodies)]
        elif r['source']=='functions/choose_a_card.cpp':category='deferred_chooser'
        elif r['source'].endswith('.cpp'):category='cpp_review'
        elif score is None or score.get('stub'):category='mapping_first'
        else:
            at=int(score['recomp'],16);size=unique_range(at,r['source'],score['name'])
            if size is None:
                category='mapping_first';out['reason']='No unique complete rebuilt DWARF range in the expected source unit.'
            else:
                old=original_code.fingerprint(a,out['native_bytes'],strings(body));new=rebuilt_code.fingerprint(at,size,strings(body))
                issues=sorted(set(old.issues+new.issues));bindings=new.bindings
                for binding in bindings:
                    target=int(binding['native_target'],16)
                    binding.update(native_name=native[target]['name'],canonical_name=accepted[target]['name'])
                out.update(rebuilt_bytes=size,original_fingerprint=old.sha256,rebuilt_fingerprint=new.sha256,
                           instruction_counts=[len(old.instructions),len(new.instructions)],review_reasons=issues,
                           binding_repairs=bindings,dependencies=[hex(x) for x in sorted(set(old.dependencies+new.dependencies))])
                out['unreviewed_dependencies']=[hex(x) for x in sorted(set(old.dependencies+new.dependencies)-reviewed_bodies)]
                if old.instructions==new.instructions and not issues:category='callee_binding_only' if bindings else 'same_local_code'
                elif score['matching']>=.9 or score.get('effective'):
                    category='reference_review' if issues and score['matching']==1 else 'small_diff_review'
                else:category='full_review'
                if category in {'callee_binding_only','small_diff_review','reference_review'}:
                    diff=difflib.SequenceMatcher(None,[repr(x) for x in old.instructions],[repr(x) for x in new.instructions],autojunk=False)
                    changes=[(op,i,j,k,l) for op,i,j,k,l in diff.get_opcodes() if op!='equal']
                    out['changed_instruction_spans']=[dict(operation=op,original=old.assembly[i:j],rebuilt=new.assembly[k:l]) for op,i,j,k,l in changes[:8]]
                    out['changed_instruction_totals']=[sum(j-i for op,i,j,k,l in changes),sum(l-k for op,i,j,k,l in changes)]
        out['category']=category;rows.append(out)
    dedupe=[];dedupe_screen=collections.Counter()
    for a,f in accepted.items():
        if f['name'] in canonical:
            dedupe_screen['canonical definition already replaces predecessor']+=1;continue
        if '_2022' not in f['name'] or a not in native or f['source'].endswith('.cpp'):continue
        old_name=native[a]['name']
        if old_name==f['name']:continue
        old_body=body_for(f['source'],old_name)
        if old_body is None:dedupe_screen['no anchored retained definition']+=1;continue
        old_ranges=by_source_name.get((source_key(f['source']),old_name),[])
        old_choices=coff.get(old_name,set())
        if len(old_choices)!=1:old_choices={r['address'] for r in old_ranges}
        new_choices=coff.get(f['name'],set())
        if len(old_choices)!=1 or len(new_choices)!=1:dedupe_screen['ambiguous or absent code symbol']+=1;continue
        old_at=next(iter(old_choices));new_at=next(iter(new_choices));old_size=unique_range(old_at,f['source'],old_name)
        if old_size is None:dedupe_screen['retained range unavailable']+=1;continue
        category=None;proof={}
        if old_at==new_at:category='compiler_alias'
        elif unique_range(new_at,f['source'],f['name']) is None:
            first=next(rebuilt_code.decoder.disasm(rebuilt_code.read(new_at,5),new_at,count=1),None)
            from capstone.x86_const import X86_OP_IMM
            if first is not None and first.mnemonic=='jmp' and first.operands[0].type==X86_OP_IMM and first.operands[0].imm==old_at:
                category='compiler_folded_thunk';proof['branch_bytes']=first.bytes.hex()
        else:
            new_size=unique_range(new_at,f['source'],f['name'])
            old_fp=rebuilt_code.fingerprint(old_at,old_size,same_image=True)
            new_fp=rebuilt_code.fingerprint(new_at,new_size,same_image=True)
            if not old_fp.issues and not new_fp.issues and old_fp.instructions==new_fp.instructions:
                category='identical_rebuilt_local_code';proof.update(fingerprint=old_fp.sha256,instructions=len(old_fp.instructions))
        if category:
            dedupe.append(dict(native_address=hex(a),source=f['source'],retained_name=old_name,recovered_name=f['name'],
                               retained_address=hex(old_at),recovered_address=hex(new_at),retained_bytes=old_size,
                               category=category,proof=proof,retained_body_sha256=sha(old_body.encode()),
                               recovered_body_sha256=f.get('reconstruction_body_sha256'),abi_and_caller_review_required=True))
        else:dedupe_screen['different local code or unresolved compiler representation']+=1
    from helper_contracts import annotate as annotate_storage_contracts
    storage_contracts=annotate_storage_contracts(rows,ROOT)
    local_retention=retain_local_matches(rows,reviewed_bodies)
    counts=collections.Counter(r['category'] for r in rows);hist=collections.Counter()
    for r in rows:
        v=r.get('reccmp_score');hist['unpaired' if v is None else '100%' if v==1 else '90-99%' if v>=.9 else '75-90%' if v>=.75 else 'below 75%']+=1
    binding_groups={}
    for row in rows:
        if row['category']!='callee_binding_only':continue
        for site in row['binding_repairs']:
            key=site['native_target'];group=binding_groups.setdefault(key,dict(native_target=key,canonical_name=site['canonical_name'],callers=set(),sites=0))
            group['callers'].add(row['address']);group['sites']+=1
    binding_groups=[{**g,'callers':sorted(g['callers']),'caller_count':len(g['callers'])} for g in binding_groups.values()]
    binding_groups.sort(key=lambda g:(-g['caller_count'],g['native_target']))
    from unify_scalar_helpers import verify_source
    unification=verify_source(ROOT)
    pending_count=len(rows)-counts['reconstructed_source']-counts['verified_retained_source']-counts['reviewed_cpp_source']-counts['reviewed_compiler_source']
    result=dict(status='triage complete; no source or recovery-ledger mutation',pending_helper_ranges=pending_count,
                unledgered_helper_ranges=len(rows),reconstructed_consolidation_helpers=counts['reconstructed_source'],
                verified_retained_helpers=counts['verified_retained_source'],local_retention=local_retention,
                reviewed_cpp_helpers=counts['reviewed_cpp_source'],reviewed_compiler_ranges=counts['reviewed_compiler_source'],
                storage_contracts=storage_contracts,
                helper_unification=unification,canonical_entry_bindings=canonical_entry_bindings,canonical_inlined_helpers=canonical_inlined,native_equivalent_helpers=native_equivalences,verified_compiler_folds=setup.get('folded_recovered_helpers',[]),
                category_counts=dict(counts),category_definitions=CATEGORIES,reccmp_score_histogram=dict(hist),
                local_match_dependency_screen=dict(with_pending_callees=sum(bool(r.get('unreviewed_dependencies')) for r in rows if r['category']=='same_local_code'),
                                                   without_pending_callees=sum(not r.get('unreviewed_dependencies') for r in rows if r['category']=='same_local_code')),
                dedupe_candidates=len(dedupe),dedupe_candidate_categories=dict(collections.Counter(x['category'] for x in dedupe)),
                dedupe_screen=dict(dedupe_screen),binding_target_groups=binding_groups,helpers=sorted(rows,key=lambda r:int(r['address'],16)),duplicates=dedupe,
                inputs={**setup['inputs'],str(native_path):sha(native_path.read_bytes()),str(manifest_path):sha(manifest_path.read_bytes()),
                        str(ROOT/'config/reccmp/functions.csv'):setup['metadata_sha256'],str(args.report):sha(args.report.read_bytes()),
                        str(args.bounds):sha(args.bounds.read_bytes()),str(receipt):sha(receipt.read_bytes()),
                        **{str(ROOT/p):sha((ROOT/p).read_bytes()) for p in ['manalink.h','defs.h','Expansions.h','tools/reccmp_project.py']},
                        str(Path(__file__)):sha(Path(__file__).read_bytes()),str(ROOT/'tools/recovery/helper_fingerprints.py'):sha((ROOT/'tools/recovery/helper_fingerprints.py').read_bytes())},
                source_sha256=source_hashes,
                scope='Current mixed recovery/legacy build versus October 2022; not a pristine historical 2020 comparison. Same-local-code requires exact instruction encoding apart from alignment NOPs, internal branch positions and verified function/source-string address translation. Unknown DLL data/code addresses, indirect control flow, incomplete disassembly and stub dependencies prevent that classification. EXE-owned fixed addresses and numeric/layout constants stay exact. A local match does not prove callees, global contents, gameplay or whole-program equivalence. High-score buckets are review priorities, not proofs of minor semantic changes. Deduplication candidates require ABI/caller, native-binding and provenance migrations before removal.')
    if unification:
        for p in ['docs/recovery-evidence/helper-unification.json','tools/recovery/unify_scalar_helpers.py']:
            result['inputs'][str(ROOT/p)]=sha((ROOT/p).read_bytes())
    for path in ['tools/recovery/helper_contracts.py',storage_contracts['cache']]:
        result['inputs'][str(ROOT/path)]=sha((ROOT/path).read_bytes())
    result['inputs'][storage_contracts['original_debug_path']]=storage_contracts['inputs']['original_debug_sha256']
    for p in ['docs/recovery-evidence/canonical-records.json','tools/recovery/canonicalize_records.py',
              'docs/recovery-evidence/canonical-record-storage.json','tools/recovery/restore_record_storage.py',
              'tools/recovery/qualify_activation_prompts.py','docs/recovery-evidence/canonical-reanimation.json',
              'tools/recovery/canonicalize_reanimation.py','docs/recovery-evidence/canonical-spell-tutor.json',
              'tools/recovery/canonicalize_spell_tutor.py','docs/recovery-evidence/retired-reanimation-worker.json',
              'tools/recovery/retire_reanimation_worker.py','docs/recovery-evidence/zone-input-qualifiers.json',
              'tools/recovery/qualify_zone_inputs.py','docs/recovery-evidence/retired-zone-counter.json','tools/recovery/retire_zone_counter.py',
              'docs/recovery-evidence/canonical-storm.json','tools/recovery/canonicalize_storm.py',
              'docs/recovery-evidence/helper-canonicalization.json','tools/recovery/canonicalize_helpers.py','tools/recovery/verify_inline_helpers.py',
              'docs/recovery-evidence/attachment-predicate-equivalence.json','tools/recovery/verify_helper_equivalences.py']:
        if (ROOT/p).exists():result['inputs'][str(ROOT/p)]=sha((ROOT/p).read_bytes())
    if compiler_cpp:
        from verify_compiler_cpp_recovery import evidence as compiler_cpp_evidence
        reviewed=compiler_cpp_evidence(canonical_data,ROOT)
        for path in [canonical_data['native_compiler_cpp_recovery'],'tools/recovery/verify_compiler_cpp_recovery.py']:
            result['inputs'][str(ROOT/path)]=sha((ROOT/path).read_bytes())
        for row in list(reviewed['header_fragments'].values())+reviewed['header_dependencies']:
            result['inputs'][row['path']]=sha(Path(row['path']).read_bytes())
    args.output.parent.mkdir(parents=True,exist_ok=True);args.summary.parent.mkdir(parents=True,exist_ok=True)
    save_json(args.output,result)
    lines=['# Helper recovery triage','',f'Remaining manual review: **{pending_count:,} native helper ranges**. Separately tracked: **{counts["reconstructed_source"]:,} explicitly reviewed/reconstructed ranges** and **{counts["verified_retained_source"]:,} verified retained ranges**. The classifier does not edit C source or recovery records. Narrow argument/interface edits do not close whole-body review.','',
           '| Review queue | Ranges |','| --- | ---: |']
    lines += [f'| `{k}` | {counts[k]:,} |' for k in CATEGORIES]
    if compiler_cpp:
        lines += ['',f"Separately reviewed: **{counts['reviewed_cpp_source']} C++ utility bodies** (one restored member, six retained definitions) and **{counts['reviewed_compiler_source']} compiler-header ranges**. Complete native bodies, specialized parameters and corresponding source were reviewed. Header copies are supplied by GCC and are excluded from authored/reconstructed-function totals; their review does not assert byte identity or CRT/caller equivalence."]
    if unification and 'canonical_functions' in unification:
        accepted_names=unification.get('accepted_function_names',unification['canonical_functions'])
        auxiliary=unification['canonical_functions']-accepted_names
        lines += ['',f"**{accepted_names} accepted canonical function names plus {auxiliary} auxiliary helper identities** are checked against physical source and compiler output. Initial consolidation removed {unification['aliases_removed']} compatibility aliases/adapters. Historical recovery names remain metadata identities."]
        if canonical_inlined:
            lines += ['',f"{len(canonical_inlined)} private canonical helpers have verified concrete DWARF inline instances instead of callable COFF entries. Their caller-owned ranges do not receive standalone helper scores or fabricated callee addresses."]
    elif unification:
        lines += ['',f"**{unification['shared_implementations']} helper pairs already share one recovered implementation**: {unification.get('alias_pairs',unification['shared_implementations'])} same-address aliases and {unification.get('const_return_adapters',0)} typed const-return tail adapters. Old retained bodies are archived as history. These are separate from the remaining duplicate candidates below.",
                  f"The setup records {len(result['verified_compiler_folds'])} compiler-shared recovered helpers without separate qualified DWARF ranges. Verified JMP targets or same-address COFF aliases count as valid callee bindings; these entries are excluded from full-body reccmp scores."]
    lines += ['',f'**{len(dedupe):,} duplicate implementation candidates** were found among retained/recovered pairs. These require ABI and caller review before canonicalizing names.','',
              f"{counts['verified_retained_source']} existing ranges pass strict complete-body instruction equality, matching DWARF calling/storage contracts, visible source and reviewed direct targets. Their source is retained without another decompilation pass. This category is recomputed from current binaries on every run and is distinct from decompiled source recovery. {storage_contracts['manual_contract_review']} local-code matches still need signature/storage-contract review.",'',
              f"Of the {counts['same_local_code']} remaining local matches, {result['local_match_dependency_screen']['without_pending_callees']} have no pending direct targets but fail another retention prerequisite; {result['local_match_dependency_screen']['with_pending_callees']} still have unreviewed direct targets. Reviewed target bodies keep their own dependency tracking; this is not a transitive gameplay-equivalence claim.",'',
              'Reccmp scores are used to order review. A 100% score alone is insufficient because unknown addresses can be replaced with matching placeholders. The strict comparison keeps numeric constants and structure/stack offsets, checks complete DWARF ranges, and only translates known function identities and exact source-string bytes. Unresolved data and indirect-call dependencies remain review work. Local equality is not transitive semantic or gameplay proof.','',
              'The report compares the current recovery build to October 2022. Recovered bodies, canonical caller contracts and modern headers can create differences from retained source; it is not an isolated 2020 build. A small assembly delta can still have a large gameplay effect; scores never authorize ignoring changed constants or flags.','',
              '## Next workflow','',
              '1. Keep `verified_retained_source` bodies; every refresh rechecks their strict machine identity and direct target review prerequisites. For remaining `same_local_code` bodies, review the pending targets rather than decompiling an identical body again.',
              '2. Review `callee_binding_only` and `small_diff_review` against the existing C source and captured instruction deltas. Make targeted edits where the evidence supports them.',
              '3. Resolve unknown references and ambiguous mappings before interpreting their scores.',
              '4. Reserve full Ghidra decompilation for substantial changes, missing source, and complex workers.',
              '5. Retain native bodies under their intended canonical names, unify callers/prototypes/callbacks/registration bindings with the native contracts, and record predecessor hashes. Compiler sharing is a candidate signal, not permission to delete an arbitrary body. Shared native APIs can resolve many retained-caller bindings together.','',
              '## Shared binding priorities','',
              '| Recovered API | Distinct pending callers | Call/reference sites |','| --- | ---: | ---: |']
    lines += [f"| `{g['canonical_name']}` | {g['caller_count']} | {g['sites']} |" for g in binding_groups[:12]]
    lines += ['',
              '## Refresh','',
              'Run `python tools/recovery/triage_helpers.py --refresh` with a current build/setup to refresh JSON, its input receipt and the DWARF bounds cache, then classify the helpers. Without `--refresh`, it reuses those inputs and rejects stale hashes. Source files newer than the DLL are rejected. Detailed categories, instruction deltas, dependency addresses, dedupe proofs and input/source hashes are in [helper-triage.json](recovery-evidence/helper-triage.json).','']
    args.summary.write_bytes(('\n'.join(lines)).replace('\n','\r\n').encode())
    print(json.dumps({k:result[k] for k in ['pending_helper_ranges','category_counts','reccmp_score_histogram','dedupe_candidates','dedupe_candidate_categories','dedupe_screen']},indent=2))


if __name__=='__main__':main()
