"""Recover graveyard, sustained-pump and Suspend-target helper families together."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image,value
from export_abi import type_of
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read,rename
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings,readonly_string,byte_literal
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input

SEEK='2671e30';CALLBACK='2244410';PUMP='2245110';BASE='20e7ea0';TIME='20ea700';BY_ID='26b9770';EYE='209c750';GATEWAY='22d4cb0'
ADDRESSES=[SEEK,'2028540','25bd4d0',CALLBACK,PUMP,'2099ea0','20552e0',BASE,TIME,'23be9f0',EYE,GATEWAY,BY_ID]
HELPERS={SEEK,CALLBACK,PUMP,BASE,TIME,BY_ID};PRIVATE={CALLBACK};NATIVE_PRIVATE={CALLBACK,BASE,TIME}
NAMES={n:n+'_2022' for n in ['seek_grave_to_reanimate_this_turn','kill_legacy_if_source_is_not_in_play_or_untapped','pump_until_source_is_in_play_and_tapped','target_must_be_permanent_or_suspend','target_must_be_permanent_or_suspend_and_have_time_counters','seek_grave_for_id_to_reanimate']}
LABEL='/* October 2022 graveyard, sustained-pump and Suspend-target helpers. */'
LEGACY=[('functions/deadbox.c','seek_grave_to_reanimate_this_turn'),('functions/deck.c','seek_grave_for_id_to_reanimate'),('cards/antiquities.c','pump_until_source_is_in_play_and_tapped'),('cards/antiquities.c','kill_legacy_if_source_is_not_in_play_or_untapped'),('cards/time_spiral.c','target_must_be_permanent_or_suspend'),('cards/time_spiral.c','target_must_be_permanent_or_suspend_and_have_time_counters')]
SITES={
 SEEK:{'2671e3e':'81e6ffffff3f','2671e44':'2500000040','2671e8b':'8d7874','2671e8e':'0504010000','2671ea0':'83c304','2671ea3':'39df','2671ec8':'8b03','2671ee2':'8b03','2671f02':'83c304','2671f16':'e855780400'},
 BASE:{'20e7ee0':'b86c8f7200'},
 '2099ea0':{'2099ece':'8d438e','2099ed1':'83f801','2099ed4':'760b'},
 '20552e0':{'205530e':'8d438e','2055311':'83f801','2055314':'760b'},
 EYE:{'209c75c':'a1a4f14e00','209c761':'8b15a8f14e00','209c767':'80cc02','209c76a':'80ce02','209c76d':'a3a4f14e00','209c772':'8915a8f14e00'},
 GATEWAY:{'22d4cc8':'8d438e','22d4ccb':'83f801','22d4cce':'760b','22d4ce4':'8d95eafeffff','22d4cea':'be1cd57302','22d4cf1':'b820000000','22d4d06':'f3a5','22d4d25':'c7850affffff00000000','22d4d2f':'c7850effffff00000000','22d4d39':'c78512ffffff00000000','22d4d43':'c78516ffffff00000000','22d4d4d':'66c7851affffff0000','22d4e08':'66a11cd57302','22d4e1b':'be1ed57302','22d4e20':'b81e000000'}}


def native_storage(debug):
    result={}
    for cu in debug.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in {'cards/antiquities.c','cards/nemesis.c'}:continue
        for d in cu.get_top_DIE().iter_children():
            if source=='cards/antiquities.c' and d.tag=='DW_TAG_typedef' and value(d,'DW_AT_name')=='pump_ability_t':
                t=type_of(d);result['pump']=dict(bytes=value(t,'DW_AT_byte_size'),members={value(x,'DW_AT_name'):value(x,'DW_AT_data_member_location') for x in t.iter_children() if x.tag=='DW_TAG_member'})
            if source=='cards/antiquities.c' and d.tag=='DW_TAG_typedef' and value(d,'DW_AT_name')=='card_instance_t':
                t=type_of(d)
                while t.tag=='DW_TAG_typedef':t=type_of(t)
                member=next(x for x in t.iter_children() if x.tag=='DW_TAG_member' and value(x,'DW_AT_name')=='targets');array=type_of(member);element=type_of(array)
                while element.tag=='DW_TAG_typedef':element=type_of(element)
                bound=next(x for x in array.iter_children() if x.tag=='DW_TAG_subrange_type')
                result['deadbox_slots']=dict(instance_bytes=value(t,'DW_AT_byte_size'),targets_offset=value(member,'DW_AT_data_member_location'),target_upper_bound=value(bound,'DW_AT_upper_bound'),target_bytes=value(element,'DW_AT_byte_size'),members={value(x,'DW_AT_name'):value(x,'DW_AT_data_member_location') for x in element.iter_children() if x.tag=='DW_TAG_member'})
            if source=='cards/nemesis.c' and d.tag=='DW_TAG_enumeration_type':
                for x in d.iter_children():
                    if value(x,'DW_AT_name')=='PB_HAND_REVEALED':result['hand_revealed']=value(x,'DW_AT_const_value')
    assert result['pump']==dict(bytes=48,members=dict(source_p=0,source_c=4,pow_mod=8,tou_mod=12,key_mod=16,skey_mod=20,paue_flags=24,can_block_additional_creatures=28,eot_removal_method=32,subtype=36,color=40,disabling_mode=44))
    assert result['hand_revealed']==512
    assert result['deadbox_slots']==dict(instance_bytes=300,targets_offset=116,target_upper_bound=18,target_bytes=8,members=dict(player=0,card=4))
    return result


def reconstruct(a,raw,runtime):
    text=strip_comments(raw);changes=[];pairs=[]
    if a==SEEK:
        cell='(dead_slot & 1 ? pcVar2->targets[dead_slot / 2].card : pcVar2->targets[dead_slot / 2].player)'
        pairs=[('target_t *ptVar3;','int dead_slot;'),('ptVar3 = pcVar2->targets;','dead_slot = 0;'),('ptVar3->player',cell),('ptVar3 = (target_t *)&ptVar3->card;','dead_slot = dead_slot + 1;'),('pcVar2->targets + 0x12 == ptVar3','dead_slot == 36'),('pcVar2->targets + 0x12 != ptVar3','dead_slot != 36')]
    if a==BASE:pairs=[('return (char *)0x728f6c;','return (char *)EXE_STR(0x728f6c);')]
    if a in {'2099ea0','20552e0',GATEWAY}:pairs=[('1 < event - EVENT_RESOLVE_ACTIVATION','1U < (unsigned int)event - (unsigned int)EVENT_RESOLVE_ACTIVATION')]
    if a==EYE:pairs=[('_player_bits','player_bits[0]'),('_DAT_004ef1a8','player_bits[1]'),('0x200','PB_HAND_REVEALED')]
    if a==GATEWAY:
        pairs += [('  uint uVar1;\n',''),('  undefined4 *puVar3;\n',''),('  char *pcVar4;\n',''),('  bool bVar5;\n','')]
        start=text.index('      puVar3 = (undefined4 *)&DAT_0273d51c;');end=text.index('      new_default_test_definition',start)
        old=text[start:end];assert "msg[0x31] = '\\0';" in old
        prompt=readonly_string(runtime,0x273d51c);assert prompt==b'Select an historical permanent.' and len(prompt)==31
        replacement='      memset(msg, 0, sizeof msg);\n      memcpy(msg, '+byte_literal(prompt)+', 32);\n'
        pairs.append((old,replacement))
    for old,new in pairs:
        count=text.count(old);expected=4 if a==SEEK and old=='ptVar3->player' else 2 if (a==SEEK and old=='ptVar3 = (target_t *)&ptVar3->card;') or a==EYE else 1
        assert count==expected,(a,old,count)
        change=dict(original=old,replacement=new,occurrences=count,kind='restore native indexed storage, event gate, live EXE pointer or literal initialization',machine_sites=[dict(address=at,bytes=b) for at,b in SITES[a].items()])
        if a==GATEWAY and old.startswith('      puVar3 ='):
            data=runtime.pe.get_data(0x273d51c-0x2000000,32);change['native_literal']=dict(address='273d51c',bytes=data.hex(),copy_bytes=32);change['literal']=prompt.decode('ascii')
        changes.append(change);text=text.replace(old,new)
    return text,changes


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('grave_pump_consumer_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert component['native_storage']==native_storage(Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()))
    from unify_scalar_helpers import retained_source_body
    for old in component['retained_bodies']:assert sha(retained_source_body(root,old['source'],old['name']).encode())==old['body_sha256']
    for layout in component['unit_layouts'].values():assert layout['test_definition_t']['bytes']==216 and layout['target_definition_t']['bytes']==84
    assert any(v['name']=='pump' and v['location']=='(DW_OP_fbreg: -68)' for v in component['locals'][PUMP])
    assert any(v['name']=='msg' and v['type']=='char[50]' and v['location']=='(DW_OP_fbreg: -286)' for v in component['locals'][GATEWAY])
    assert component['shared_predicate_linkage']==dict(native_private=[BASE,TIME],source_public=[BASE,TIME],reason='Qualified predicates are exposed from their original Time Spiral unit to preserve the native Timecrafting callback references across source units; legacy static bodies remain intact.')
    assert re.search(r'(?im)^_player_bits\s*=\s*0x4ef1a4;',read(root/'manalink.lds'))
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['grave_pump_consumer_evidence']==detail and entry['static']==(a in PRIVATE)
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-grave-pump-consumers' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw,runtime);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,{});assert literals==entry['recovered_literals'];text,_=rename(text,NAMES);assert function_body(cleaned(text,overlap),entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in NATIVE_PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
        for at,b in SITES.get(a,{}).items():assert machine_bytes(int(at,16),len(bytes.fromhex(b))).hex()==b
        if a in HELPERS-PRIVATE:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
    assert len(entries)==13 and sum(e['name'].startswith('card_') for e in entries.values())==7


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(debug);(cache/'export-request-grave-pump-consumers.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared thirteen related exports.');return
    assert 'grave_pump_consumers_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(debug),**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{'cards/antiquities.c','cards/dominaria.c','cards/planar_chaos.c'}),functions={},
        retained_bodies=[dict(source=s,name=n,body_sha256=sha(function_body(read(root/s),n).encode())) for s,n in LEGACY],
        shared_predicate_linkage=dict(native_private=[BASE,TIME],source_public=[BASE,TIME],reason='Qualified predicates are exposed from their original Time Spiral unit to preserve the native Timecrafting callback references across source units; legacy static bodies remain intact.'),
        scope='Recover seven handlers and six helpers: current-turn graveyard lookup, sustained pump constructor/lifetime callback, Suspend/time-counter predicates, hand reveal flags and historic-permanent selection. Express the 36 deadbox DWORD slots through 18 typed target pairs; preserve native filters, reloads, loop bounds, reanimation modes and both player gates. Use native 48-byte pump, 84-byte target and 216-byte test domains, unsigned activation gates, EXE-owned error pointer and exact 32-byte prompt plus 18 zero bytes. Earlier source and records remain unchanged.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES.get(meta[a]['name'],meta[a]['name']),source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES})
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-grave-pump-consumers'/f'{a}.c');text,changes=reconstruct(a,raw,runtime);text,literals=recover_strings(text,runtime,{});text,_=rename(text,NAMES);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==([3] if a in NATIVE_PRIVATE else [2])
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,grave_pump_consumer_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
        sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in HELPERS-PRIVATE)+'\n#endif\n\n'+header[at:]
    compile_sources(root,cache/'grave-pump-consumers-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-grave-pump-consumers/export.tsv').open(),delimiter='\t'):
            rows[row['address']]=row;shutil.copyfile(cache/'decompiled-grave-pump-consumers'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=13;m['card_handler_functions']+=7;m['helper_functions']+=6;m['private_functions']+=1;m['private_helpers']+=1;m['shared_helper_interfaces']+=5
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['grave_pump_consumers_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'grave-pump-consumers-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=7,new_helpers=6,private_helpers=1,exposed_native_private_predicates=2)))


if __name__=='__main__':main()
