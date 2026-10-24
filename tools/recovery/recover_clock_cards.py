"""Recover native Windows clock helpers and three clock/redraw consumers."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read,rename
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings,readonly_string,byte_literal
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input

DATE='265cf80';HM='265cfd0';HOUR='265d000';MS='265d020';SLEEP='265d090';PARTY='2305c30';VAMPIRE='25c6aa0';SNOW='2410b40'
ADDRESSES=[DATE,HM,HOUR,MS,SLEEP,PARTY,VAMPIRE,SNOW];HELPERS={DATE,HM,HOUR,MS,SLEEP};PRIVATE=set();ALIASES={};LEGACY=[];NAMES={}
LABEL='/* October 2022 native Windows date, clock and delay helpers. */'


def native_storage(debug):
    records={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/windows.c':continue
        for d in cu.iter_DIEs():
            name=value(d,'DW_AT_name')
            if d.tag=='DW_TAG_structure_type' and name in {'_SYSTEMTIME'}:
                records['SYSTEMTIME']=dict(bytes=value(d,'DW_AT_byte_size'),members={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in d.iter_children() if c.tag=='DW_TAG_member'})
            if d.tag=='DW_TAG_variable' and name=='buf' and inherited_value(d.get_parent(),'DW_AT_name')=='localtime_date':
                t=type_of(d);assert t.tag=='DW_TAG_array_type' and value(type_of(t),'DW_AT_byte_size')==1
                bounds=[value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'];assert bounds==[17]
                records['date_buffer']=dict(bytes=18,location=d.attributes['DW_AT_location'].value,element=type_name(type_of(t)))
    assert records['SYSTEMTIME']==dict(bytes=16,members=dict(wYear=0,wMonth=2,wDayOfWeek=4,wDay=6,wHour=8,wMinute=10,wSecond=12,wMilliseconds=14))
    assert records['date_buffer']['location']==[3,112,68,119,2]
    return records


def native_interfaces(runtime,debug,root):
    runtime.pe.parse_data_directories();wanted={'GetLocalTime','QueryPerformanceFrequency','QueryPerformanceCounter','Sleep'}
    imports={s.name.decode():dict(dll=d.dll.decode(),iat=f'{s.address:x}') for d in runtime.pe.DIRECTORY_ENTRY_IMPORT for s in d.imports if s.name and s.name.decode() in wanted}
    assert imports=={'GetLocalTime':dict(dll='KERNEL32.dll',iat='27b53e8'),'QueryPerformanceCounter':dict(dll='KERNEL32.dll',iat='27b540c'),'QueryPerformanceFrequency':dict(dll='KERNEL32.dll',iat='27b5410'),'Sleep':dict(dll='KERNEL32.dll',iat='27b5420')}
    assert readonly_string(runtime,0x275d6c0)==b'%02d/%02d/%d'
    divider=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and 0x2000000+debug.pe.sections[sec-1].VirtualAddress+v==0x26fc230];assert divider==[dict(name='___divdi3',storage=2)]
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert sha(exe.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    frames=[dict(address=f'{a:x}',bytes=get(a,20).hex(),arguments=[0,255]) for a in [0x2410d04,0x2410f52]]
    assert [f['bytes'] for f in frames]==['c7442404ff000000c7042400000000e8a87102fe','c7442404ff000000c7042400000000e85a6f02fe']
    return dict(imports=imports,divider=dict(address='26fc230',symbols=divider,operation='signed 64-bit quotient; EAX low word returned after wrapped 64-bit times-1000 multiply'),redraw=dict(address='437ec0',arguments=2,frames=frames,exe_prefix=exe.pe.get_data(0x37ec0,0x70).hex(),exe_sha256=sha(exe.data)),date_format=dict(address='275d6c0',bytes=readonly_string(runtime,0x275d6c0).hex()))


def reconstruct(a,raw,runtime,meta):
    text=strip_comments(raw);changes=[];size=int(meta[a]['high_pc'],16)-int(a,16)
    block=dict(machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))
    def change(old,new,kind):
        nonlocal text
        count=text.count(old);assert count>0,(a,old)
        changes.append(dict(original=old,replacement=new,occurrences=count,kind=kind,**block));text=text.replace(old,new)
    if a in {DATE,HM,HOUR}:
        change('  SYSTEMTIME systime;','  SYSTEMTIME systime;\n  _Static_assert(sizeof(SYSTEMTIME) == 16, "native SYSTEMTIME size");\n  _Static_assert(offsetof(SYSTEMTIME, wYear) == 0 && offsetof(SYSTEMTIME, wMonth) == 2 && offsetof(SYSTEMTIME, wDay) == 6 && offsetof(SYSTEMTIME, wHour) == 8 && offsetof(SYSTEMTIME, wMinute) == 10, "native SYSTEMTIME field offsets");','verify Windows clock object size against native sixteen-byte stack allocation')
    if a==DATE:
        change('  SYSTEMTIME systime;','  static char date_buffer[18];\n  SYSTEMTIME systime;','restore persistent native eighteen-byte local buffer, matching DW_OP_addr 0x2774470')
        change('localtime_date::buf','date_buffer','restore function-local static buffer scope and returned lifetime')
    if a==HM:
        change('CONCAT22(systime.wHour,systime.wMinute)','(int)(((uint32_t)systime.wHour << 16) | (uint32_t)systime.wMinute)','restore zero-extended high-hour / low-minute word packing')
    if a==MS:
        start=text.index('  iVar1 = func_0x026fc230(');end=text.index('  return iVar1;',start)
        change(text[start:end],'  int64_t scaled_ticks = (int64_t)((uint64_t)ticks.QuadPart * UINT64_C(1000));\n  iVar1 = (int)(scaled_ticks / frequency.QuadPart);\n','restore native ((ticks*32-ticks)*4+ticks)*8 modulo-64-bit scaling and signed __divdi3 quotient; return low32')
        change('  LARGE_INTEGER ticks;','  LARGE_INTEGER ticks;\n  _Static_assert(sizeof(LARGE_INTEGER) == 8, "native performance-counter size");','verify Windows counter/frequency object size')
    if a==SNOW:
        change('redisplay_all();','EXE_FN(int, 0x437ec0, int, int)(0, 0xff);','restore both native two-word redraw frames instead of the abstract zero-argument name')
    return text,changes


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('clock_card_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['native_storage']==native_storage(debug) and component['native_interfaces']==native_interfaces(runtime,debug,root)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['clock_card_evidence']==detail and not entry['static']
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-clock-cards' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,ALIASES);assert literals==entry['recovered_literals'];text,_=rename(text,NAMES);assert function_body(cleaned(text,overlap),entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==2
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
        if a in HELPERS:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
    assert len(entries)==8 and sum(e['name'].startswith('card_') for e in entries.values())==3


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(debug);(cache/'export-request-clock-cards.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared five clock helpers and three card consumers.');return
    assert 'clock_cards_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(debug),native_interfaces=native_interfaces(runtime,debug,root),**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{'cards/unglued_unhinged.c','cards/unstable.c','cards/promo.c'}),functions={},
        scope='Recover five Windows date/time/sleep helpers and Elvish House Party, Old-Fashioned Vampire and Snow Mercy in original source files. Restore exact SYSTEMTIME field offsets and hour/minute packing, persistent 18-byte date buffer, wrapped 64-bit performance-counter scaling and signed divider, and both two-word redraw frames. Preserve native time boundaries, affected-card omissions, Snow Mercy counter queue/overflow, shake animation, speculation gates and 350ms delay. Add no GUI implementation or gameplay tests; existing source and chooser stubs remain unchanged.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES.get(meta[a]['name'],meta[a]['name']),source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-clock-cards'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);text,literals=recover_strings(text,runtime,ALIASES);text,_=rename(text,NAMES);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,clock_card_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in HELPERS-PRIVATE)+'\n#endif\n\n'+header[at:];compile_sources(root,cache/'clock-cards-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-clock-cards/export.tsv').open(),delimiter='\t'):rows[row['address']]=row;shutil.copyfile(cache/'decompiled-clock-cards'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=8;m['card_handler_functions']+=3;m['helper_functions']+=5;m['shared_helper_interfaces']+=5
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['clock_cards_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'clock-cards-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=3,new_helpers=5)))


if __name__=='__main__':main()
