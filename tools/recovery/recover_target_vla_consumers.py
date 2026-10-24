"""Recover dynamic target/hand arrays and native X-payment interfaces."""
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

FORCE='2338530';MASS='25c0b30';GREMLINS='20c75a0';BANE='238a8a0';METZALI='2194e80';AETHER='264c8d0';MULTI='268db60';DOUBLE='268dc30'
DISPLAYS={FORCE:('pcVar8','02740fb0'),MASS:('pcVar5','02757a10'),GREMLINS:('pcVar4','02729450'),BANE:('pcVar2','02743f50')}
ADDRESSES=[MULTI,DOUBLE,FORCE,MASS,GREMLINS,BANE,METZALI,AETHER];HELPERS={MULTI,DOUBLE};PRIVATE=set();ALIASES={}
LEGACY=[('functions/functions.c','charge_mana_for_multi_x'),('functions/functions.c','charge_mana_for_double_x')]
NAMES={n:n+'_2022' for s,n in LEGACY};LABEL='/* October 2022 native multiple-X mana charging. */'
ARRAYS={a:{'t_array'} for a in DISPLAYS};ARRAYS.update({METZALI:{'targ_array'},AETHER:{'original_hand','to_discard'}})


def native_storage(debug):
    arrays={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {'cards/modern_horizons.c','cards/ravnica_allegiance.c','cards/aether_revolt.c','cards/mirage.c','cards/rivals_of_ixalan.c','cards/exodus.c'}:continue
        for d in cu.iter_DIEs():
            if d.tag!='DW_TAG_variable':continue
            name=inherited_value(d,'DW_AT_name');parent=d.get_parent()
            while parent and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
            pc=value(parent,'DW_AT_low_pc') if parent else None;a=f'{pc:x}' if pc else ''
            if a not in ARRAYS or name not in ARRAYS[a]:continue
            t=type_of(d)
            while t.tag=='DW_TAG_typedef':t=type_of(t)
            assert t.tag=='DW_TAG_array_type'
            dims=[dict(form=c.attributes['DW_AT_upper_bound'].form,value=c.attributes['DW_AT_upper_bound'].value) if 'DW_AT_upper_bound' in c.attributes else None for c in t.iter_children() if c.tag=='DW_TAG_subrange_type']
            arrays.setdefault(a,{})[name]=dict(element=type_name(type_of(t)),element_bytes=value(type_of(t),'DW_AT_byte_size'),dimensions=dims)
    assert {a:set(v) for a,v in arrays.items()}==ARRAYS
    for a,records in arrays.items():
        for n,r in records.items():
            assert r['element_bytes']==4 and len(r['dimensions'])==(2 if n=='original_hand' else 1)
            if n=='original_hand':assert r['dimensions'][0]==dict(form='DW_FORM_data1',value=1)
            assert r['dimensions'][-1] is None or r['dimensions'][-1]['form'] in {'DW_FORM_ref4','DW_FORM_exprloc'}
    return dict(arrays=arrays,card_instance_bytes=300,target_pair_offset=116,target_pair_bytes=8)


def outgoing_frames(runtime,meta):
    wanted={0x2650c40:('show_deck',6),0x2650de0:('select_card_from_zone',8),0x26e9c50:('dialog_impl',18),0x44e050:('internal_rand',1)}
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;frames=[]
    for a in ADDRESSES:
        insns=list(decoder.disasm(runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)),int(a,16)))
        for index,ins in enumerate(insns):
            if ins.mnemonic!='call' or ins.operands[0].type!=X86_OP_IMM or ins.operands[0].imm not in wanted:continue
            name,arity=wanted[ins.operands[0].imm];first=insns[max(0,index-48)].address;end=ins.address+ins.size
            frames.append(dict(owner=a,callee=name,physical_argument_words=arity,call=f'{ins.address:x}',start=f'{first:x}',end=f'{end:x}',bytes=runtime.pe.get_data(first-0x2000000,end-first).hex()))
    assert [(n,sum(f['callee']==n for f in frames)) for n in ['show_deck','select_card_from_zone','dialog_impl','internal_rand']]==[('show_deck',4),('select_card_from_zone',1),('dialog_impl',1),('internal_rand',1)]
    return frames


def braced(text,at):
    start=text.index('{',at);depth=1;end=start+1
    while depth:
        if text[end]=='{':depth+=1
        elif text[end]=='}':depth-=1
        end+=1
    return start,end


def reconstruct(a,raw,runtime,meta):
    from target_vla_consumer_bodies import display_body,AETHER_HAND,METZALI_RANDOM,METZALI_DIALOG
    text=strip_comments(raw);changes=[];size=int(meta[a]['high_pc'],16)-int(a,16)
    block=dict(machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))
    def change(old,new,kind):
        nonlocal text
        count=text.count(old);assert count>0,(a,old)
        changes.append(dict(original=old,replacement=new,occurrences=count,kind=kind,**block));text=text.replace(old,new)
    if a in DISPLAYS:
        instance,caption=DISPLAYS[a];at=text.index('check_special_flags2(player,card,0x10000);');at=text.index('if (',at);start,end=braced(text,at)
        change(text[start+1:end-1],display_body(instance,'LIT_'+caption),'restore native target-count-sized display array, player placeholders, live fill count and six-word show_deck frame')
    if a==FORCE:
        for declaration in ['  undefined1 ******ppppppuVar4;\n','  undefined1 ******appppppuStack_6c [2];\n']:change(declaration,'','remove decompiler-only pointer scaffolding replaced by the int VLA')
    if a==MASS:change('*(int *)(pcVar5->color_id + aiStack_74[0] * 8 + -0x3c)','pcVar5->targets[iVar8].player','restore target player view: 64 + (i + 14) * 8 - 60 == 116 + i * 8')
    if a==METZALI:
        at=text.index('iVar1 = target_available(player,card,&td);');at=text.index('if (iVar1 != 0)',at);start,end=braced(text,at)
        change(text[start+1:end-1],METZALI_RANDOM,'restore native availability-sized candidate VLA, -1 fill, current-turn scan and fresh random bound')
        start=text.index('      uStackY_b8 = 0x219511e;');end=text.index('      iVar1 = dialog_impl(player,card,EVENT_ACTIVATE);',start)+len('      iVar1 = dialog_impl(player,card,EVENT_ACTIVATE);')
        change(text[start:end],METZALI_DIALOG,'restore both ability results, native priority paths and all eighteen physical dialog words')
        for declaration in ['  undefined4 *****pppppuVar6;\n','  undefined4 ******ppppppuVar7;\n','  undefined4 *****apppppuStack_6c [2];\n','  byte bVar9;\n','  bVar9 = 0;\n']:change(declaration,'','remove decompiler-only allocation/byte-copy scaffolding')
    if a==AETHER:
        at=text.index('uVar6 = check_special_flags2(player,card,-0x80000000);');at=text.index('if (uVar6 == 0)',at);start,end=braced(text,at)
        change(text[start+1:end-1],AETHER_HAND,'restore two-row original-hand snapshot, independently sized discard array, native compaction extra reads, chooser/cancel/payment/discard frames and order')
    for match in list(re.finditer(r'(?m)^  undefined4 (uStackY_[a-z0-9]+);\n',text)):
        name=match[1];before=text
        after=re.sub(r'(?m)^ *'+re.escape(name)+r' = 0x[0-9a-f]+;\n','',before).replace(match[0],'')
        assert not re.search(r'\b'+re.escape(name)+r'\b',after),a
        change(before,after,'remove unused synthetic return-PC temporary and constant assignments after proving no remaining uses')
    for placeholder in sorted(set(re.findall(r'LIT_([0-9a-f]{8})',text))):
        at=int(placeholder,16);literal=readonly_string(runtime,at);change('LIT_'+placeholder,byte_literal(literal),'restore exact native caption/control/option literal')
        changes[-1].update(native_literal=dict(address=f'{at:x}',bytes=(literal+b'\0').hex(),copy_bytes=len(literal)+1),literal=literal.decode())
    return text,changes


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('target_vla_consumer_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert component['native_storage']==native_storage(Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()))
    for old in component['retained_bodies']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    for layout in component['unit_layouts'].values():assert layout['test_definition_t']['bytes']==216 and layout['target_definition_t']['bytes']==84
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    assert component['outgoing_frames']==outgoing_frames(runtime,meta)
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['target_vla_consumer_evidence']==detail and not entry['static']
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-target-vla' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,ALIASES);assert literals==entry['recovered_literals'];text,_=rename(text,NAMES);assert function_body(cleaned(text,overlap),entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==2
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
        if a in HELPERS:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
    assert len(entries)==8 and sum(e['name'].startswith('card_') for e in entries.values())==6


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(debug);(cache/'export-request-target-vla.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared six target-array consumers and two X-payment helpers.');return
    assert 'target_vla_consumers_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    proof=dict(outgoing_frames=outgoing_frames(runtime,meta),runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(debug),**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{meta[a]['source'] for a in ADDRESSES}),functions={},retained_bodies=[dict(source=s,name=n,body_sha256=sha(function_body(read(root/s),n).encode())) for s,n in LEGACY],
        scope='Recover six target-array consumers and two native multiple-X payment helpers in original source units. Restore seven dynamic integer arrays, including a two-row hand snapshot, the common target-display/player-placeholder view, exact variadic dialog/random/selection frames, target-player field view and native hand compaction/cancel/payment/discard order. Preserve original spell-specific cost and resolution quirks, independent availability/hand counts, extra reads and two legacy payment bodies; gameplay integration remains downstream.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES.get(meta[a]['name'],meta[a]['name']),source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-target-vla'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);text,literals=recover_strings(text,runtime,ALIASES);text,_=rename(text,NAMES);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,target_vla_consumer_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in HELPERS-PRIVATE)+'\n#endif\n\n'+header[at:];compile_sources(root,cache/'target-vla-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-target-vla/export.tsv').open(),delimiter='\t'):rows[row['address']]=row;shutil.copyfile(cache/'decompiled-target-vla'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=8;m['card_handler_functions']+=6;m['helper_functions']+=2;m['shared_helper_interfaces']+=2
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['target_vla_consumers_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'target-vla-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=6,new_helpers=2)))


if __name__=='__main__':main()
