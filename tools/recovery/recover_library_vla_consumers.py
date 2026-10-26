"""Recover native library-selection VLAs, packed modes and Proliferate helpers."""
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

HAZ='20d0ed0';SUN='2215840';GISHATH='2217170';NIV='21d3830';VOW='22cefa0';ULTIMATUM='2289140';CELEBRATION='21d4080';AUTO='24383e0';MANUAL='2438530';PROLIFERATE='2438700'
ADDRESSES=[AUTO,MANUAL,PROLIFERATE,HAZ,SUN,GISHATH,NIV,VOW,ULTIMATUM,CELEBRATION]
HELPERS={AUTO,MANUAL,PROLIFERATE};PRIVATE=set();ALIASES={}
LEGACY=[('cards/scars_of_mirrodin.c',n) for n in ['proliferate_auto_single_permanent_or_player','proliferate_manual_single_permanent_or_player','proliferate']]
NAMES={n:n+'_2022' for s,n in LEGACY};LABEL='/* October 2022 Proliferate helpers with native target layout. */'
ARRAYS={HAZ:{'haz'},SUN:{'selection_array','selected'},GISHATH:{'deck_copy','msg'},NIV:{'deck_copy','msg'},VOW:{'deck_copy','msg'},ULTIMATUM:{'searched','to_cast'},CELEBRATION:{'choices'}}


def outgoing_frames(runtime,meta):
    callees={0x2650de0:('select_card_from_zone',8),0x26cea80:('scnprintf',None),0x2668ab0:('fake_play_card_in_zone_for_free',4),0x2669320:('play_card_in_exile_for_free',3)}
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;frames=[]
    for a in ADDRESSES:
        code=runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16));insns=list(decoder.disasm(code,int(a,16)))
        for index,ins in enumerate(insns):
            if ins.mnemonic!='call' or ins.operands[0].type!=X86_OP_IMM or ins.operands[0].imm not in callees:continue
            name,arity=callees[ins.operands[0].imm]
            if arity is None:arity=3 if a==GISHATH else 4
            first=insns[max(0,index-40)].address;end=ins.address+ins.size
            frames.append(dict(owner=a,call=f'{ins.address:x}',callee=name,physical_argument_words=arity,start=f'{first:x}',end=f'{end:x}',bytes=runtime.pe.get_data(first-0x2000000,end-first).hex()))
    assert sum(f['callee']=='select_card_from_zone' for f in frames)==7
    assert sum(f['callee']=='scnprintf' for f in frames)==3
    assert sum(f['callee'] in {'fake_play_card_in_zone_for_free','play_card_in_exile_for_free'} for f in frames)==4
    return frames


def native_storage(debug):
    arrays={};tokens={};player_counts={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {'cards/hour_of_devastation.c','cards/ixalan.c','cards/war_of_the_spark.c','cards/dominaria.c','cards/ikoria.c','cards/scars_of_mirrodin.c'}:continue
        for d in cu.iter_DIEs():
            name=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_typedef' and name=='token_generation_t' and value(cu.get_top_DIE(),'DW_AT_name')=='cards/war_of_the_spark.c':
                t=type_of(d)
                while t.tag=='DW_TAG_typedef':t=type_of(t)
                tokens=dict(bytes=value(t,'DW_AT_byte_size'),members={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in t.iter_children() if c.tag=='DW_TAG_member' and value(c,'DW_AT_name') is not None})
            if d.tag=='DW_TAG_enumeration_type':
                for c in d.iter_children():
                    if value(c,'DW_AT_name') in {'PCOUNTER_POISON','PCOUNTER_EXPERIENCE','PCOUNTER_ENERGY','PCOUNTER_invalid'}:player_counts[value(c,'DW_AT_name')]=value(c,'DW_AT_const_value')
            if d.tag!='DW_TAG_variable':continue
            parent=d.get_parent()
            while parent and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
            pc=value(parent,'DW_AT_low_pc') if parent else None;a=f'{pc:x}' if pc else ''
            if a not in ARRAYS or name not in ARRAYS[a]:continue
            t=type_of(d)
            while t.tag=='DW_TAG_typedef':t=type_of(t)
            assert t.tag=='DW_TAG_array_type'
            dims=[dict(form=c.attributes['DW_AT_upper_bound'].form,value=c.attributes['DW_AT_upper_bound'].value) if 'DW_AT_upper_bound' in c.attributes else None for c in t.iter_children() if c.tag=='DW_TAG_subrange_type']
            arrays.setdefault(a,{})[name]=dict(element=type_name(type_of(t)),element_bytes=value(type_of(t),'DW_AT_byte_size'),dimensions=dims)
    assert {a:set(v) for a,v in arrays.items()}==ARRAYS
    for a,n in [(HAZ,'haz'),(GISHATH,'deck_copy'),(NIV,'deck_copy'),(VOW,'deck_copy'),(ULTIMATUM,'to_cast')]:
        record=arrays[a][n];assert record['element_bytes']==4 and len(record['dimensions'])==1 and (record['dimensions']==[None] if a==ULTIMATUM else record['dimensions'][0]['form'] in {'DW_FORM_ref4','DW_FORM_exprloc'})
    assert arrays[SUN]['selection_array']['dimensions'][0]==dict(form='DW_FORM_data1',value=1) and len(arrays[SUN]['selection_array']['dimensions'])==2 and arrays[SUN]['selection_array']['element_bytes']==4
    assert arrays[ULTIMATUM]['searched']['dimensions']==[dict(form='DW_FORM_data1',value=2)]
    assert arrays[CELEBRATION]['choices']['dimensions']==[dict(form='DW_FORM_data1',value=3)]
    assert tokens['bytes']==76 and all(tokens['members'][n]==off for n,off in [('pow',24),('tou',28),('color_forced',40),('no_sleight',41)])
    assert player_counts==dict(PCOUNTER_POISON=0,PCOUNTER_EXPERIENCE=1,PCOUNTER_ENERGY=2,PCOUNTER_invalid=-1)
    return dict(arrays=arrays,token=tokens,player_counter_values=player_counts)


def reconstruct(a,raw,runtime,meta):
    from library_selection_bodies import BODIES
    text=strip_comments(raw);changes=[];size=int(meta[a]['high_pc'],16)-int(a,16)
    block=dict(machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))
    def change(old,new,kind):
        nonlocal text
        count=text.count(old);assert count>0,(a,old)
        changes.append(dict(original=old,replacement=new,occurrences=count,kind=kind,**block));text=text.replace(old,new)
    if a in BODIES:change(text,BODIES[a],'restore native dynamic arrays, row/word strides, outgoing arguments, shrinking queues, original ownership/order/early exits and extra-entry reads')
    if a==AUTO:
        for old,new in [('(&player_counters)[p * 4]','POISON_COUNTERS(p)'),('*(byte *)(p * 4 + 0x4ef531)','EXPERIENCE_COUNTERS(p)'),('*(byte *)(p * 4 + 0x4ef532)','ENERGY_COUNTERS(p)')]:change(old,new,'restore verified four-byte player-counter row and byte offsets')
    if a==CELEBRATION:
        change('aiStack_124 != piStack_138','choices + 4 != piStack_138','restore native four-choice array endpoint')
        for i in range(4):
            old='*(char *)&pcVar1->field_26' if i==0 else f'*(char *)((int)&pcVar1->field_26 + {i})'
            change(old,f'((unsigned char *)&pcVar1->info_slot)[{i}]','restore native packed per-choice byte operation')
        start=text.index('            default_token_definition(');end=text.index('            generate_token((token_generation_t *)choices);',start)+len('            generate_token((token_generation_t *)choices);')
        change(text[start:end],'''            {
            token_generation_t token;
            default_token_definition(player, card, 0x4915, &token);
            token.pow = 2;
            token.tou = 2;
            token.color_forced = 62;
            token.no_sleight = 1;
            generate_token(&token);
            }''','restore separate native 76-byte token phase at the four-choice stack location')
    for placeholder in sorted(set(re.findall(r'LIT_([0-9a-f]{8})',text))):
        at=int(placeholder,16);literal=readonly_string(runtime,at);change('LIT_'+placeholder,byte_literal(literal),'restore exact native prompt literal and copy extent')
        changes[-1].update(native_literal=dict(address=f'{at:x}',bytes=(literal+b'\0').hex(),copy_bytes=len(literal)+1),literal=literal.decode())
    return text,changes


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('library_vla_consumer_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert component['outgoing_frames']==outgoing_frames(runtime,{r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']})
    assert component['native_storage']==native_storage(Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()))
    from unify_scalar_helpers import retained_source_body
    for old in component['retained_bodies']:assert sha(retained_source_body(root,old['source'],old['name'],source_view=sources).encode())==old['body_sha256']
    for layout in component['unit_layouts'].values():assert layout['test_definition_t']['bytes']==216 and layout['target_definition_t']['bytes']==84
    def local(a,n):return next(v for v in component['locals'][a] if v['name']==n)
    assert local(CELEBRATION,'choices')['location']==local(CELEBRATION,'token')['location']=='(DW_OP_fbreg: -312)'
    assert local(ULTIMATUM,'searched')['location']=='(DW_OP_fbreg: -464)'
    assert re.search(r'_player_counters\s*=\s*0x4EF530;',read(root/'manalink.lds'),re.I)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['library_vla_consumer_evidence']==detail and not entry['static']
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-library-vla' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,ALIASES);assert literals==entry['recovered_literals'];text,_=rename(text,NAMES);assert function_body(cleaned(text,overlap),entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==2
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
        if a in HELPERS:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
    assert len(entries)==10 and sum(e['name'].startswith('card_') for e in entries.values())==7


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(debug);(cache/'export-request-library-vla.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared seven library consumers and three Proliferate helpers.');return
    assert 'library_vla_consumers_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    proof=dict(outgoing_frames=outgoing_frames(runtime,meta),runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(debug),**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{meta[a]['source'] for a in ADDRESSES}),functions={},retained_bodies=[dict(source=s,name=n,body_sha256=sha(function_body(read(root/s),n).encode())) for s,n in LEGACY],
        scope='Recover seven library-selection and packed-mode card handlers together with three native Proliferate helpers. Restore six dynamic integer arrays including a two-row table, exact outgoing selection/casting/formatting operands, prompt copies, queue shifts, original ownership and cancellation/return order, packed choice bytes and shared token phase, player counter rows, native extra reads and gate quirks. Preserve three legacy Proliferate bodies and all earlier records; gameplay integration remains downstream.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES.get(meta[a]['name'],meta[a]['name']),source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-library-vla'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);text,literals=recover_strings(text,runtime,ALIASES);text,_=rename(text,NAMES);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,library_vla_consumer_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in HELPERS-PRIVATE)+'\n#endif\n\n'+header[at:];compile_sources(root,cache/'library-vla-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-library-vla/export.tsv').open(),delimiter='\t'):rows[row['address']]=row;shutil.copyfile(cache/'decompiled-library-vla'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=10;m['card_handler_functions']+=7;m['helper_functions']+=3;m['shared_helper_interfaces']+=3
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['library_vla_consumers_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'library-vla-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=7,new_helpers=3)))


if __name__=='__main__':main()
