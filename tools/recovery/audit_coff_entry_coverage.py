"""Cross-check native executable COFF entries outside the ordinary DWARF census.

Reports original/project/compiler scopes separately. It does not modify source,
the recovery ledger, comparison reports, or function matching prerequisites.
"""
import argparse,bisect,collections,csv,json,struct,subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_MEM
from canonicalize_helpers import ROOT,read,span,sha
from inspect_release import Image


def file_owners(image):
    result={};raw=image.data;offset=image.pe.FILE_HEADER.PointerToSymbolTable;index=0;owner=None
    while index<image.pe.FILE_HEADER.NumberOfSymbols:
        record=raw[offset+18*index:offset+18*(index+1)];aux=record[17]
        if record[16]==103:
            owner=raw[offset+18*(index+1):offset+18*(index+1+aux)].split(b'\0',1)[0].decode('latin1')
        else:
            value,section,kind=struct.unpack_from('<IhH',record,8)
            if section>0 and kind==32:
                at=image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[section-1].VirtualAddress+value
                result[(at,record[16])]=owner
        index+=1+aux
    return result


def symbols(image):
    result=collections.defaultdict(set)
    for name,value,section,kind,storage in image.symbols:
        if section>0 and kind==32:
            result[name].add(image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[section-1].VirtualAddress+value)
    return result


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--output',type=Path,default=ROOT/'docs/recovery-evidence/coff-entry-coverage.json');args=parser.parse_args()
    paths=[ROOT/'.recovery/2022-ManalinkEh.dll',Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg'),ROOT/'ManalinkEh.dll',ROOT/'ManalinkEh.dbg']
    native,debug,current,rebuilt=[Image(p.read_bytes()) for p in paths]
    assert sha(native.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert native.functions()==debug.functions()
    metadata=list(csv.DictReader((ROOT/'.recovery/2022-functions.csv').open()))
    intervals=[(int(r['low_pc'],16),int(r['high_pc'],16),r['source'],r['name']) for r in metadata if r['low_pc'] and r['high_pc']]
    proof=json.loads((ROOT/'docs/recovery-evidence/helper-canonicalization.json').read_text())
    split=json.loads((ROOT/proof['native_split_cards_recovery']).read_text())
    for row in split['split_helpers']:
        intervals.extend((int(p['address'],16),int(p['end'],16),row['source'],row['name']) for p in row['native_ranges'])
    intervals.sort();starts=[r[0] for r in intervals];owners=file_owners(debug)
    by_name=collections.defaultdict(list)
    for row in metadata:
        if row['declaration']=='False' or row['name']=='call_card_fn_impl':by_name[row['name']].append(row)
    old_symbols=symbols(debug);new_symbols=symbols(rebuilt)
    native.pe.parse_data_directories(directories=[1]);imports={i.address:(d.dll.decode(),i.name.decode() if i.name else i.ordinal) for d in native.pe.DIRECTORY_ENTRY_IMPORT for i in d.imports}
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    rows=[];sources={}
    for name,value,section,kind,storage in sorted(native.functions(),key=lambda x:(x[2],x[1],x[0])):
        at=native.pe.OPTIONAL_HEADER.ImageBase+native.pe.sections[section-1].VirtualAddress+value
        j=bisect.bisect_right(starts,at);cover=[r for r in intervals[max(0,j-3):j] if r[0]<=at<r[1]]
        row=dict(name=name,address=hex(at),coff_storage=storage,coff_file=owners.get((at,storage)))
        if cover:
            row.update(category='inside_project_dwarf_extent',ranges=[dict(address=hex(a),end=hex(b),source=s,name=n) for a,b,s,n in cover]);rows.append(row);continue
        candidates=by_name.get(name[1:],[]) if name.startswith('_') else []
        if len(candidates)==1 and candidates[0]['source'].startswith(('cards/','functions/')):
            meta=candidates[0];source=meta['source'].replace('cards/odyssey.c','cards/odissey.c');row.update(source=source,die_offset=meta['die_offset'])
            text=read(ROOT/source)
            if name=='_call_card_fn_impl':
                source='functions/call_card_fn.asm';body=read(ROOT/source)
            else:
                bounds=span(text,name[1:]);assert bounds,(name,source);body=text[slice(*bounds)]
            sources[source]=sha((ROOT/source).read_bytes());row.update(source=source,source_definition_sha256=sha(body.encode()))
            seq=[]
            for ins in decoder.disasm(native.pe.get_data(at-0x2000000,160),at):
                seq.append(ins)
                if ins.mnemonic in ('ret','jmp'):break
            assert seq and seq[-1].mnemonic in ('ret','jmp'),name
            length=sum(i.size for i in seq);row.update(native_bytes=length,native_bytes_sha256=sha(native.pe.get_data(at-0x2000000,length)),instructions=[[hex(i.address),i.mnemonic,i.op_str] for i in seq])
            matches=new_symbols.get(name,set());assert len(matches)==1,(name,matches)
            actual=next(iter(matches));row['rebuilt_address']=hex(actual)
            if seq[0].mnemonic=='jmp':
                target=int(seq[0].op_str,16);aliases=[n for n,addrs in old_symbols.items() if addrs=={target} and len(new_symbols.get(n,set()))==1]
                branch=next(decoder.disasm(current.pe.get_data(actual-0x2000000,5),actual,count=1))
                matched=[n for n in aliases if branch.mnemonic=='jmp' and branch.op_str==hex(next(iter(new_symbols[n])))]
                assert matched,(name,branch.op_str,aliases)
                row.update(category='verified_coff_only_tail_forwarder',target_names=matched,rebuilt_instruction=[branch.mnemonic,branch.op_str])
            elif name=='_card_makeshift_mauler':
                # Complete straight-line wrapper: skaabs(p,c,event,1); return0.
                target=next(i for i in seq if i.mnemonic=='call');assert target.op_str=='0x2074030'
                actual_seq=list(decoder.disasm(current.pe.get_data(actual-0x2000000,length),actual));assert len(seq)==len(actual_seq)
                for old,new in zip(seq,actual_seq):
                    if old.mnemonic=='call':assert new.mnemonic=='call' and new.op_str==hex(next(iter(new_symbols['_skaabs'])))
                    else:assert old.bytes==new.bytes,(name,old.mnemonic,new.mnemonic)
                row['category']='verified_coff_only_straight_line_wrapper'
            else:
                assert current.pe.get_data(actual-0x2000000,length)==native.pe.get_data(at-0x2000000,length),name
                row['category']='verified_coff_only_exact_body'
            rows.append(row);continue
        first=next(decoder.disasm(native.pe.get_data(at-0x2000000,6),at,count=1),None)
        if first and first.mnemonic=='jmp' and first.operands[0].type==X86_OP_MEM and not first.operands[0].mem.base and not first.operands[0].mem.index and first.operands[0].mem.disp in imports:
            row.update(category='external_import_thunk',import_identity=list(imports[first.operands[0].mem.disp]))
        elif name.startswith('__Z'):row['category']='cxx_library_candidate'
        else:row['category']='crt_object_candidate'
        rows.append(row)
    mangled=[r for r in rows if r['category']=='cxx_library_candidate']
    result=subprocess.run(['C:/bin/gcc-9.3.0-no-debug/bin/c++filt.exe','-n'],input='\n'.join(r['name'][1:] for r in mangled)+'\n',capture_output=True,text=True,check=True)
    decoded=result.stdout.splitlines();assert len(decoded)==len(mangled)
    for row,name in zip(mangled,decoded):
        row['demangled']=name
        assert any(s in name for s in ['std::','__gnu_cxx','__cxxabiv1','operator new','operator delete','_txnal_']),name
    counts=dict(collections.Counter(r['category'] for r in rows))
    nonfunctions=[]
    roles={'_DllEntry':'DLL entry: covered by native-jump-table verification',
           '_CodeSectionEx':'fixed veneer block: covered by native-jump-table verification',
           '___chkstk_ms':'compiler stack probe: covered by card-runtime binding verification',
           'etext':'end-of-text linker marker',
           '__CTOR_LIST__':'constructor-pointer table, not a function',
           '___CTOR_LIST__':'constructor-pointer table alias, not a function',
           '__DTOR_LIST__':'destructor-pointer table, not a function',
           '___DTOR_LIST__':'destructor-pointer table alias, not a function'}
    for name,value,section,kind,storage in native.symbols:
        if section>0 and kind!=32 and native.pe.sections[section-1].Characteristics&0x20000000 and not name.startswith('.'):
            assert name in roles,(name,kind)
            nonfunctions.append(dict(name=name,address=hex(0x2000000+native.pe.sections[section-1].VirtualAddress+value),kind=kind,role=roles[name]))
    assert {r['name'] for r in nonfunctions}==set(roles)
    result=dict(status='Read-only COFF coverage cross-check; no ordinary triage or recovery-ledger mutation.',coff_function_entries=len(rows),category_counts=counts,project_coff_only_entries=sum(n for k,n in counts.items() if k.startswith('verified_coff_only_')),named_executable_nonfunction_symbols=nonfunctions,entries=rows,inputs={str(p):sha(p.read_bytes()) for p in paths+[ROOT/'.recovery/2022-functions.csv',ROOT/'docs/recovery-evidence/helper-canonicalization.json',Path(__file__)]},source_sha256=sources,scope='COFF entries inside project DWARF extents retain their separately audited body status. COFF-only project entries are checked against native complete terminal bodies and current source/compiled entries. CRT/C++ library candidates and import thunks are separate from authored card/helper source recovery; their implementations are not claimed recovered or equivalent. COFF .file labels are provenance clues, not standalone ownership proof. Interactive UI and gameplay execution remain unverified.')
    args.output.parent.mkdir(parents=True,exist_ok=True);pending=args.output.with_suffix('.pending.json');pending.write_text(json.dumps(result,indent=2)+'\n');pending.replace(args.output)
    print(json.dumps({k:result[k] for k in ['coff_function_entries','category_counts','project_coff_only_entries']},indent=2))


if __name__=='__main__':main()
