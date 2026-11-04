"""Join checked spell entry packets to strict, event-specialized continuations.

This is an additional review, not a change to reccmp scores or the strict CFG
reviewer. Unsupported differences remain pending. Calls, data and literal
correspondences retain the assumptions of review_branch_layout.py.
"""
import argparse
import collections
import csv
import json
import re
from pathlib import Path

import capstone
from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_GRP_JUMP
from capstone.x86_const import X86_OP_IMM, X86_OP_REG, X86_OP_MEM
from canonicalize_helpers import read, mask, sha
from inspect_release import Image
from review_spell_entry_dispatch import walk, prove, EVENTS, ALIASES
from review_branch_layout import compare_cfg, literal_value
from review_event_guards import flags_dead

PRESERVED = {'ebx', 'esi', 'edi', 'ebp'}


def jump(md, pc, target):
    return next(md.disasm(b'\xe9' + ((target-pc-5) & 0xffffffff).to_bytes(4, 'little'), pc))


def specialize(code, start, tags, event, public, md, extra_cdecl=frozenset()):
    """Must-analysis: forget event identity on writes and at mixed joins.

    Facts only mean that a full register still holds this invocation's event.
    They do not infer memory contents or propagate values returned by calls.
    Calls retain facts only in the ordinary C callee-saved registers. Each
    removed CMP/branch must have dead flags on its chosen successor, and the
    branch cannot have another direct incoming edge.
    """
    targets = {i.operands[0].imm for i in code.values()
               if i.group(CS_GRP_JUMP) and i.operands[0].type == X86_OP_IMM}

    def decision(i, known):
        if (i.mnemonic != 'cmp' or any(i.prefix) or len(i.operands) != 2
                or i.operands[0].type != X86_OP_REG or i.operands[0].size != 4
                or i.operands[1].type != X86_OP_IMM or i.operands[1].imm not in EVENTS
                or i.reg_name(i.operands[0].reg) not in known):
            return None
        branch = code.get(i.address+i.size)
        if (branch is None or branch.mnemonic not in ('je', 'jne') or any(branch.prefix)
                or branch.operands[0].type != X86_OP_IMM or branch.address in targets):
            return None
        equal = event == i.operands[1].imm
        target = branch.operands[0].imm if equal == (branch.mnemonic == 'je') else branch.address+branch.size
        dead, trace = flags_dead(code, target, public)
        extra_contract = None
        if not dead and trace and extra_cdecl:
            last = code.get(int(trace[-1][0], 0))
            if (last is not None and last.mnemonic == 'call' and not any(last.prefix)
                    and last.operands[0].type == X86_OP_IMM
                    and last.operands[0].imm in extra_cdecl
                    and all(not any(code[int(pc,0)].prefix) for pc,_ in trace)):
                # The existing liveness walker reached this call through only
                # its accepted flag-independent instructions/direct jumps.
                # This separately bound public C call has no EFLAGS input or
                # preservation contract. Never infer that for an unknown call.
                dead = True
                extra_contract = 'separately bound public C-call flags contract'
        if not dead:
            return None
        return dict(cmp=hex(i.address), branch=hex(branch.address), successor=hex(target),
                    register=i.reg_name(i.operands[0].reg), event=event,
                    compared_event=i.operands[1].imm, dead_flags_trace=trace,
                    additional_call_contract=extra_contract)

    facts = {start: frozenset(tags)}
    pending = [start]
    while pending:
        pc = pending.pop()
        assert pc in code, 'Continuation leaves decoded extent'
        i = code[pc]
        assert not any(i.prefix), 'Prefixed continuation instruction'
        known = set(facts[pc])
        choice = decision(i, known)
        if choice:
            successors = [int(choice['successor'], 0)]
        else:
            _, writes = i.regs_access()
            copied = None
            if (i.mnemonic == 'mov' and len(i.operands) == 2
                    and all(o.type == X86_OP_REG and o.size == 4 for o in i.operands)
                    and i.reg_name(i.operands[1].reg) in known):
                copied = i.reg_name(i.operands[0].reg)
            for reg in writes:
                name = i.reg_name(reg)
                known.discard(ALIASES.get(name, (name,))[0])
            if copied:
                known.add(copied)
            if i.mnemonic == 'call':
                known.intersection_update(PRESERVED)
            if i.mnemonic == 'ret':
                assert not i.operands, 'Non-C return'
                successors = []
            elif i.group(CS_GRP_JUMP):
                assert i.operands[0].type == X86_OP_IMM, 'Indirect continuation jump'
                successors = [i.operands[0].imm]
                if i.mnemonic != 'jmp':
                    successors.append(pc+i.size)
            else:
                successors = [pc+i.size]
        for target in successors:
            assert target in code, 'Continuation edge exits function'
            value = frozenset(known) if target not in facts else facts[target] & known
            if facts.get(target) != value:
                facts[target] = value
                pending.append(target)
        assert len(facts) <= 4096, 'Continuation state limit'
    changes = [d for pc, known in facts.items() if (d := decision(code[pc], known))]
    transformed = dict(code)
    for d in changes:
        pc = int(d['cmp'], 0)
        transformed[pc] = jump(md, pc, int(d['successor'], 0))
    return transformed, changes, set(facts)


def controls(md):
    """Small controls for the new must-analysis and flag-liveness boundary."""
    def run(raw, tags=frozenset({'ebx'}), event=0x71):
        code = {i.address: i for i in md.disasm(bytes.fromhex(raw), 0x1000)}
        return specialize(code, 0x1000, tags, event, {}, md)[1]
    # Both branch successors return constants and discard flags at C return.
    tail = '83fb71 7406 b801000000 c3 b802000000 c3'
    assert [r['successor'] for r in run(tail)] == ['0x100b']
    assert [r['successor'] for r in run(tail,event=0x74)] == ['0x1005']
    assert [r['successor'] for r in run(tail.replace('7406','7506'))] == ['0x1005']
    assert not run('89c3 '+tail)  # MOV EBX,EAX loses event identity.
    assert not run('b371 '+tail)  # Even an identical-looking byte write loses it.
    # JE with unknown flags: one predecessor overwrites EBX before the join.
    assert not run('7402 89c3 '+tail)
    # The chosen successor consumes carry before any overwrite.
    assert not run('83fb71 7406 b801000000 c3 83d000 c3')
    # A call cannot preserve the event in a caller-saved register.
    assert not run('e800100000 83f871 7406 b801000000 c3 b802000000 c3', {'eax'})
    assert len(run('e800100000 '+tail)) == 1
    return dict(positive=4, negative=5,
                scope='Event-register writes, partial writes, mixed joins, live flags and call clobbers. Not gameplay tests.')


def additional_call_controls(md):
    # The selected equal edge reaches a call at 0x100b to 0x3000. Unknown calls
    # remain unresolved; an incoming carry consumer still blocks specialization.
    def run(raw, extra):
        code={i.address:i for i in md.disasm(bytes.fromhex(raw),0x1000)}
        return specialize(code,0x1000,{'ebx'},0x71,{},md,extra)[1]
    body='83fb71 7406 b801000000 c3 e8f01f0000 c3'
    assert len(run(body,{0x3000}))==1
    assert not run(body,set())
    assert not run('83fb71 7406 b801000000 c3 83d000 e8ed1f0000 c3',{0x3000})
    return dict(positive=1,negative=2,scope='Only the separately bound C call discards flags; unknown calls and preceding carry reads are rejected.')


def entry_flag_overwrite(code,start):
    dead,trace=flags_dead(code,start,{})
    if not dead or not trace:
        return None
    last=code[int(trace[-1][0],0)]
    # RET is not an overwrite; logical operations leave AF undefined. Require
    # a complete arithmetic overwrite, and refuse prefixes throughout the path.
    if last.mnemonic not in {'sub','add','cmp','neg'}:
        return None
    if any(any(code[int(pc,0)].prefix) for pc,_ in trace):
        return None
    return trace


def entry_flag_controls(md):
    def run(raw):
        code={i.address:i for i in md.disasm(bytes.fromhex(raw),0x1000)}
        return entry_flag_overwrite(code,0x1000)
    assert run('55 89e5 83ec10 c9 c3')
    assert not run('55 89e5 c9 c3')
    assert not run('55 89e5 0f92c0 83ec10 c9 c3')
    assert not run('55 89e5 e8f81f0000 83ec10 c9 c3')
    assert not run('55 89e5 40 c9 c3')
    assert not run('55 89e5 31c0 c9 c3')
    return dict(positive=1,negative=5,scope='Full arithmetic overwrite is required before flags are read or a helper is called; plain return is insufficient.')


def check_function(codes, bases, sizes, targets, public, correspondence, md, extra_cdecl=None):
    extra_cdecl=extra_cdecl or [set(),set()]
    cases = prove(codes, bases, targets)
    suffixes = []
    covered = {}
    for index,case in enumerate(cases):
        if case['outcome'] != 'call':
            continue
        event=case['event']
        states, paths = [], []
        for code, base, target in zip(codes, bases, targets):
            state, path, _ = walk(code, base, event, case['player_equal'], case['card_equal'], target)
            assert state['kind'] == 'call'
            states.append(state)
            paths.append(path)
        assert states[0] == states[1]
        key=repr((event,states,[p[-1] for p in paths]))
        if key in covered:
            suffixes[covered[key]]['entry_case_indices'].append(index)
            continue
        transformed = []
        rewrites = []
        for side in (0, 1):
            tags = {r for r, value in states[side]['preserved'].items() if value == event}
            start = paths[side][-1]
            model, changes, reached = specialize(codes[side], start, tags, event, public[side], md,extra_cdecl[side])
            assert not (reached & set(paths[side][:-1])), 'Continuation re-enters skipped prefix'
            # Strict reviewer retains original extents. Only its entry redirects
            # to the call where the complete entry packets were proved equal.
            model[bases[side]] = jump(md, bases[side], start)
            transformed.append(model)
            rewrites.append(changes)
        proof = compare_cfg(transformed, bases, sizes, correspondence,
                            return_epilogues=True)
        covered[key]=len(suffixes)
        suffixes.append(dict(event=event,entry_case_indices=[index],first_calls=[hex(p[-1]) for p in paths],
                             event_specializations=rewrites,strict_continuation=proof))
    return dict(entry_cases=cases,continuations=suffixes)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--queue', required=True, type=Path)
    parser.add_argument('--build-evidence', required=True, type=Path)
    parser.add_argument('--output', required=True, type=Path)
    parser.add_argument('--limit', type=int, default=100)
    parser.add_argument('--public-call-flags', action='store_true',
                        help='Bind plain public int-return C helpers only after both entry sequences overwrite all arithmetic flags before any use/call.')
    args = parser.parse_args()
    assert args.limit > 0
    queue = json.loads(args.queue.read_bytes())
    reportpath = Path(queue['report'])
    receiptpath = reportpath.with_suffix('.inputs.json')
    receipt = json.loads(receiptpath.read_bytes())
    assert sha(reportpath.read_bytes()) == receipt['report_sha256'] == queue['report_sha256']
    assert sha(receiptpath.read_bytes()) == queue['receipt_sha256']
    for p,h in receipt['inputs'].items():assert sha(Path(p).read_bytes()) == h, p
    metadata = Path(receipt['metadata_snapshot'])
    assert sha(metadata.read_bytes()) == receipt['metadata_sha256']
    build = json.loads(args.build_evidence.read_bytes())['compiled_verification']
    assert build['build'] == 'passed' and build['fixed_entry_slots'] == 15169
    assert sha(Path('ManalinkEh.dll').read_bytes()) == build['dll_sha256']
    assert sha(Path('ManalinkEh.dbg').read_bytes()) == build['debug_sha256']
    bounds_path = Path('.recovery/reccmp/triage-rebuilt-bounds.json')
    bounds = json.loads(bounds_path.read_bytes())
    assert bounds['sha256'] == build['debug_sha256']
    by_address = collections.defaultdict(list)
    for b in bounds['functions']:by_address[b['address']].append(b)
    metadata_rows=list(csv.DictReader(metadata.open()))
    selected = [r for r in queue['functions'] if not r.get('change_site_review')
                and (r.get('source') or '').startswith('cards/') and r['source'].endswith('.c')
                and re.fullmatch(r'card_\w+', r['name'])][:args.limit]
    paths = [Path(p) for p in ['.recovery/2022-ManalinkEh.dll', 'ManalinkEh.dll',
             'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg', 'ManalinkEh.dbg']]
    dependencies = [Path(__file__)] + [Path('tools/recovery/'+n+'.py') for n in
                    ['review_spell_entry_dispatch','review_branch_layout','review_event_guards',
                     'pointer_return_tail','inspect_release','canonicalize_helpers','unify_scalar_helpers']]
    inputpaths = paths + dependencies + [args.queue, reportpath, receiptpath, metadata,
                 args.build_evidence,bounds_path,Path('manalink.h'),Path('defs.h'),
                 Path(capstone.__file__),Path(capstone._cs._name)]
    inputpaths += [Path(r['source']) for r in selected]
    inputs = {str(p):sha(p.read_bytes()) for p in inputpaths}
    assert inputs[str(paths[0])] == '52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert inputs[str(paths[2])] == '7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    images = [Image(p.read_bytes()) for p in paths[:2]]
    symbols = [];public = [];targets = [];tokens = [collections.defaultdict(set),collections.defaultdict(set)]
    for side,p in enumerate(paths[2:]):
        dbg = Image(p.read_bytes());names = collections.defaultdict(set);pub = collections.defaultdict(set)
        for n,o,s,k,t in dbg.symbols:
            if s>0 and t in (2,3) and k in (0,32) and not n.startswith('.'):
                address = 0x2000000+dbg.pe.sections[s-1].VirtualAddress+o
                names[n].add(address)
                if t==2 and k==32:pub[address].add(n.lstrip('_'))
        target = {}
        for n in ['_default_target_definition','_counterspell_target_definition']:
            assert len(names[n])==1
            target[next(iter(names[n]))] = n
        symbols.append(names);public.append(pub);targets.append(target)
    extra_cdecl=[set(),set()]
    for n in symbols[0].keys() & symbols[1].keys():
        if len(symbols[0][n])==len(symbols[1][n])==1:
            for side in (0,1):tokens[side][next(iter(symbols[side][n]))].add('COFF:'+n)
    for side,im in enumerate(images):
        im.pe.parse_data_directories(directories=[1])
        for entry in getattr(im.pe,'DIRECTORY_ENTRY_IMPORT',[]):
            for item in entry.imports:
                tokens[side][item.address].add('import:'+entry.dll.decode().lower()+':'+
                    (item.name.decode() if item.name else '#'+str(item.ordinal)))
    def correspondence(a,b,usage='value'):
        if a==b and not 0x2000000<=a<0x3000000:return 'fixed:'+hex(a)
        common=tokens[0][a]&tokens[1][b]
        if common:return min(common)
        if usage=='value':
            values=[literal_value(im,v,set(range(2,28))) for im,v in zip(images,[a,b])]
            if values[0] is not None and values[0]==values[1]:return 'readonly:'+values[0][0]+':'+sha(values[0][1])
        return None
    md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True
    public_contracts=[];public_sources=[{},{}]
    if args.public_call_flags:
        header=mask(read('manalink.h'))
        declarations={m[1]:m[0] for m in re.finditer(r'(?m)^int\s+([A-Za-z_]\w*)\s*\([^;{}]*\)\s*;',header)
                      if '...' not in m[0] and '__attribute__' not in m[0]}
        by_name=collections.defaultdict(list)
        for row in metadata_rows:
            if (row['source'].endswith('.c') and row['native_name']==row['name']
                    and row['name'] in declarations):by_name[row['name']].append(row)
        for name,entries in sorted(by_name.items()):
            if len(entries)!=1 or any(len(s['_'+name])!=1 for s in symbols):continue
            row=entries[0];addresses=[next(iter(s['_'+name])) for s in symbols]
            if addresses[0]!=int(row['address'],0) or any(name not in p[a] for p,a in zip(public,addresses)):continue
            extents=[b for b in by_address[addresses[1]] if b['name']==name and b['source']==row['source']]
            if not extents or len({b['size'] for b in extents})!=1:continue
            packets=[]
            for side,(address,size) in enumerate(zip(addresses,[int(row['size']),extents[0]['size']])):
                raw=images[side].pe.get_data(address-0x2000000,min(size,256))
                code={i.address:i for i in md.disasm(raw,address)}
                trace=entry_flag_overwrite(code,address)
                if trace is None:break
                last=code[int(trace[-1][0],0)];raw=raw[:last.address+last.size-address]
                packets.append(dict(address=hex(address),size=len(raw),sha256=sha(raw),bytes_hex=raw.hex(),trace=trace))
            if len(packets)!=2:continue
            public_contracts.append(dict(name=name,source=row['source'],declaration=declarations[name],entry_packets=packets,scope='Unique public COFF and source/name/address bounds in both images, plain nonvariadic int-return C declaration, and both entry traces overwrite arithmetic flags before any read/call. Callers must be in another compilation unit. Callee/data behavior and ordinary C call ABI remain correspondence assumptions; this is not a callee implementation proof.'))
            for side,address in enumerate(addresses):
                extra_cdecl[side].add(address);public_sources[side][address]=row['source']
    model_controls=controls(md);call_controls=additional_call_controls(md) if args.public_call_flags else None
    public_controls=entry_flag_controls(md) if args.public_call_flags else None
    reviewed=[];unresolved=[];texts={};masked={};mutation=None
    for row in selected:
        try:
            normalize=lambda s:s.replace('cards/odyssey.c','cards/odissey.c')
            base0=int(row['address'],0);base1=int(row['recomp'],0)
            assert symbols[0]['_'+row['native_name']]=={base0}, 'Native name/entry differs'
            assert symbols[1]['_'+row['name']]=={base1}, 'Current name/entry differs'
            entries=[e for e in by_address[base1] if e['name']==row['name'] and normalize(e['source'])==row['source']]
            assert entries and len({e['size'] for e in entries})==1, 'Ambiguous current extent'
            sizes=[row['native_size'],entries[0]['size']];bases=[base0,base1];ranges=[];codes=[]
            for im,base,size in zip(images,bases,sizes):
                raw=im.pe.get_data(base-0x2000000,size);assert len(raw)==size
                code={i.address:i for i in md.disasm(raw,base)}
                assert sum(i.size for i in code.values())==size, 'Incomplete extent decode'
                ranges.append(dict(address=hex(base),size=size,sha256=sha(raw),bytes_hex=raw.hex()));codes.append(code)
            allowed=[{a for a in extra_cdecl[s] if public_sources[s].get(a)!=row['source']} for s in (0,1)]
            proof=check_function(codes,bases,sizes,targets,public,correspondence,md,allowed)
            source=row['source']
            if source not in texts:texts[source]=read(source);masked[source]=mask(texts[source])
            match=re.search(r'(?m)^\s*int\s+'+re.escape(row['name'])+r'\s*\(\s*int player,\s*int card,\s*event_t event\s*\)\s*\{',masked[source])
            assert match, 'No ordinary three-argument card source definition'
            depth=1
            for end in range(match.end(),len(masked[source])):
                depth+=(masked[source][end]=='{')-(masked[source][end]=='}')
                if depth==0:break
            else:raise AssertionError('Unclosed source body')
            # Use canonical span conventions: leading spaces/newlines before
            # the return type are excluded, matching existing evidence hashes.
            from canonicalize_helpers import span
            extent=span(texts[source],row['name']);assert extent
            body=texts[source][slice(*extent)]
            if mutation is None:
                start=int(proof['continuations'][0]['first_calls'][1],0)
                candidates=[i for i in codes[1].values() if i.address>start and i.mnemonic=='mov'
                            and len(i.operands)==2 and i.operands[0].type==X86_OP_MEM
                            and i.reg_name(i.operands[0].mem.base)=='esp' and i.operands[0].mem.disp==8
                            and i.operands[1].type==X86_OP_IMM and i.imm_size]
                assert candidates, 'No meaningful outgoing argument mutation control'
                i=candidates[0];raw=bytearray.fromhex(ranges[1]['bytes_hex']);raw[i.address-base1+i.imm_offset]^=0x40
                changed={j.address:j for j in md.disasm(raw,base1)}
                try:check_function([codes[0],changed],bases,sizes,targets,public,correspondence,md,allowed)
                except (AssertionError,KeyError) as e:mutation=dict(name=row['name'],instruction=hex(i.address),argument_byte_xor=64,rejected=True,reason=str(e))
                else:raise RuntimeError('Changed outgoing argument was accepted')
            reviewed.append(dict(name=row['name'],address=row['address'],source=source,
                                 source_definition_sha256=sha(body.encode()),ranges=ranges,proof=proof))
        except (AssertionError,KeyError,StopIteration) as e:
            unresolved.append(dict(name=row['name'],address=row['address'],reason=str(e)))
    for p,h in inputs.items():assert sha(Path(p).read_bytes())==h,p
    assert not reviewed or mutation and mutation['rejected']
    result=dict(format=1,inputs=inputs,decoder_version=capstone.__version__,script_sha256=sha(Path(__file__).read_bytes()),
                discovery_report_sha256=receipt['report_sha256'],selected=len(selected),reviewed=reviewed,
                unresolved=unresolved,must_analysis_controls=model_controls,outgoing_argument_negative_control=mutation,
                additional_call_controls=call_controls,
                verified_public_entry_contracts=public_contracts,public_entry_controls=public_controls,
                scope='All 24 entry partitions (five event constants plus their full-width complement, two independent affected-card/controller equalities) have identical first-call argument/private-stack/preserved-register packets or zero returns. Five accepted events then receive strict entry-reachable continuation checks with only must-known event CMP/JE/JNE pairs specialized after proving their flags dead. Partial writes, mixed joins and caller-saved call clobbers forget facts. Callee-saved event registers obey the ordinary C call ABI; stack and engine memory are valid, nonvolatile and not asynchronously mutated. Original strict CFG operation/register/width/address/literal checks and zero-return epilogue rules are reused unchanged. Named callees, mapped globals and read-only literal contents remain correspondence assumptions, not callee/data-content, pointer-identity, fault-order, unwind/longjmp or gameplay proofs. Unsupported differences remain pending; reccmp scores are untouched.')
    if public_contracts:
        result['scope'] += ' Additional public C helper calls are accepted only with pinned native/current entry traces that overwrite arithmetic flags before any use/call, unique public/source identities and a plain nonvariadic int-return declaration. Each selected caller is in another compilation unit. Ordinary C flags-output/ABI assumptions apply; return-only/logical/flag-consuming entries and unknown calls retain rejection. Argument/value and strict operation/register checks are unchanged.'
    args.output.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(dict(selected=len(selected),reviewed=len(reviewed),unresolved=dict(collections.Counter(r['reason'] for r in unresolved)),controls=model_controls,argument_mutation=mutation),indent=2))


if __name__=='__main__':
    main()
