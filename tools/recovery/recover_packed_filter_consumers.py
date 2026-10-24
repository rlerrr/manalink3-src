"""Recover eleven handlers and four native helpers using verified packed-field views."""
import argparse,csv,json,re,shutil,struct
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_REG_EBP,X86_REG_ESP
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

DEVIL='229f8c0';AZCANTA='2213cd0';FEROCIDON='2215490';CAVALIER='23955e0';SALVAGE='20c9410';UMORI='228c4f0';VIVIEN='228c7f0';DRUID='25c0110';PAY='245c180';OVERLOAD='245c0c0';MANA='266c5c0'
OVERLOAD_CARDS=['233de70','222f0a0','2231430']
ADDRESSES=[DEVIL,'2375ee0',AZCANTA,FEROCIDON,CAVALIER,SALVAGE,UMORI,VIVIEN,DRUID]+OVERLOAD_CARDS+[PAY,OVERLOAD,MANA]
HELPERS={DEVIL,PAY,OVERLOAD,MANA}
NAMES={n:n+'_2022' for n in ['make_devil','pay_overload','overload','mana_producing_creature_all_one_color']}
LABEL='/* October 2022 Devil token, Overload and creature-mana helpers. */'
LEGACY=[('cards/shadows_over_innistrad.c','make_devil'),('cards/return_to_ravnica.c','pay_overload'),('cards/return_to_ravnica.c','overload'),('functions/produce_mana.c','mana_producing_creature_all_one_color')]
FIXED={AZCANTA:{'2213d68':'c7442414f45d7302','2213d8f':'e8acce4300'},
 CAVALIER:{'2395650':'8d851cffffff','2395659':'8944240c','239566e':'e89d483200','2395677':'8b851cffffff','2395692':'e819f13100'},
 SALVAGE:{'20c9453':'be94987202','20c9458':'f3a5','20c9462':'b911000000','20c946d':'f3ab','20c95c2':'be94987202','20c95c7':'f3a5','20c95d1':'b911000000','20c95dc':'f3ab','20c967d':'be94987202','20c9682':'f3a5','20c968c':'b911000000','20c9691':'f3ab','20c9520':'8d04c0','20c9523':'8d04c511707e00','20c952a':'8944240c'},
 UMORI:{'228c6a3':'be20117202','228c6a8':'b907000000','228c6ad':'f3a5'},
 MANA:{'266c5e8':'8d9076ffffff','266c5ee':'83fa01','266c5f1':'7675'}}


def native_layouts_extra(debug):
    result={}
    for cu in debug.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in {'cards/aether_revolt.c','cards/ixalan.c','cards/ikoria.c'}:continue
        for d in cu.get_top_DIE().iter_children():
            name=value(d,'DW_AT_name')
            if d.tag=='DW_TAG_typedef' and ((source=='cards/aether_revolt.c' and name in {'card_data_t','card_instance_t'}) or (source=='cards/ikoria.c' and name=='token_generation_t')):
                t=type_of(d)
                while t.tag=='DW_TAG_typedef':t=type_of(t)
                members={value(x,'DW_AT_name'):value(x,'DW_AT_data_member_location') for x in t.iter_children() if x.tag=='DW_TAG_member' and value(x,'DW_AT_name') is not None}
                result[name]=dict(bytes=value(t,'DW_AT_byte_size'),members=members)
                if name=='card_instance_t':
                    unions=[x for x in t.iter_children() if x.tag=='DW_TAG_member' and value(x,'DW_AT_name') is None and value(x,'DW_AT_data_member_location')==56]
                    assert len(unions)==1
                    union=type_of(unions[0])
                    assert union.tag=='DW_TAG_union_type' and value(union,'DW_AT_byte_size')==4
                    result[name]['info_slot_union']=dict(offset=56,bytes=4,members={value(x,'DW_AT_name'):value(x,'DW_AT_data_member_location') or 0 for x in union.iter_children() if x.tag=='DW_TAG_member'})
            if source=='cards/ixalan.c' and d.tag=='DW_TAG_enumeration_type':
                for x in d.iter_children():
                    if value(x,'DW_AT_name') in {'PB_CANT_GAIN_LIFE','TRIGGER_COMES_INTO_PLAY'}:result[value(x,'DW_AT_name')]=value(x,'DW_AT_const_value')
    assert result['card_data_t']['bytes']==72 and result['card_data_t']['members']['name']==1
    assert result['card_instance_t']['bytes']==300 and result['card_instance_t']['info_slot_union']==dict(offset=56,bytes=4,members={'info_slot':0,'info_slot_fn':0})
    assert result['token_generation_t']['bytes']==76 and result['token_generation_t']['members']['action_argument']==52
    assert result['PB_CANT_GAIN_LIFE']==0x200000 and result['TRIGGER_COMES_INTO_PLAY']==219
    return result


def field_sites(runtime,meta,a):
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    code=runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16));sites=[]
    for i in decoder.disasm(code,int(a,16)):
        if any(o.type==X86_OP_MEM and ((o.mem.disp in {56,57} and o.mem.base not in {0,X86_REG_EBP,X86_REG_ESP}) or o.mem.disp in {0x4ef1a4,0x4ef1a8,0x7a2d78}) for o in i.operands):sites.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex()))
    return sites


def reconstruct(a,raw,runtime,meta):
    text=strip_comments(raw);changes=[];pairs=[];sites=[dict(address=at,bytes=b) for at,b in FIXED.get(a,{}).items()]+field_sites(runtime,meta,a)
    if a in OVERLOAD_CARDS:pairs=[('((uint)pcVar1->field_26 & 2)','((uint)pcVar1->info_slot & 2)')]
    if a==PAY:
        t=re.search(r'(?m)^  (anon_union_\w+) aVar1;',text);assert t
        pairs=[(t[0],'  int old_info_slot;'),('aVar1 = pcVar2->field_26;','old_info_slot = pcVar2->info_slot;'),('pcVar2->field_26 = aVar1;','pcVar2->info_slot = old_info_slot;')]
    if a==DRUID:
        byte0='((unsigned char *)&pcVar3->info_slot)[0]';byte1='((unsigned char *)&pcVar3->info_slot)[1]'
        pairs=[('*(char *)&pcVar3->field_26',byte0),('*(undefined1 *)&pcVar3->field_26',byte0),('*(undefined1 *)((int)&pcVar3->field_26 + 1)',byte1),('*(char *)((int)&pcVar3->field_26 + 1)',byte1)]
    if a==FEROCIDON:pairs=[('_trigger_condition != 0xdb','trigger_condition != TRIGGER_COMES_INTO_PLAY'),('_player_bits','player_bits[0]'),('_DAT_004ef1a8','player_bits[1]'),('0x200000','PB_CANT_GAIN_LIFE')]
    if a==AZCANTA:pairs=[('0,"Into grave?"','0,(int)"Into grave?"')]
    if a==CAVALIER:pairs=[('  int owner;','  int owner;\n  int position;'),('(int *)&this_test','&position'),('this_test.message._0_4_','position')]
    if a==MANA:pairs=[('event - EVENT_ATTACK_RATING < 2','(unsigned int)event - (unsigned int)EVENT_ATTACK_RATING < 2U')]
    if a==VIVIEN:
        pairs=[('CONCAT22((short)counters[iVar2],1)','(((uint)(ushort)counters[iVar2] << 16) | 1U)')]
        # Word-sized token action payload: low word quantity, high word counter ID.
        sites=field_sites(runtime,meta,a)
        code=runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16));decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
        sites += [dict(address=f'{i.address:x}',bytes=i.bytes.hex()) for i in decoder.disasm(code,int(a,16)) if any(o.type==X86_OP_MEM and o.size==2 for o in i.operands)]
        assert sites
    if a==SALVAGE:
        pairs=[('  undefined4 *puVar5;\n',''),('  char *pcVar6;\n',''),('piVar2[iVar4] * 0x48 + 0x7e7011','cards_data[piVar2[iVar4]].name')]
        pattern=r'(?m)^(?P<indent> +)puVar5 = \(undefined4 \*\)&UNK_02729894;[\s\S]*?\n(?P=indent)new_default_test_definition'
        matches=list(re.finditer(pattern,text));assert len(matches)==3
        for m in matches:
            old=m[0].rsplit('\n',1)[0]+'\n';indent=m['indent'];prompt=readonly_string(runtime,0x2729894)
            assert prompt==b'Select a nonland card to exile.' and len(prompt)==31
            pairs.append((old,indent+'memset(msg, 0, sizeof msg);\n'+indent+'memcpy(msg, '+byte_literal(prompt)+', 32);\n'))
    if a==UMORI:
        start=text.index('    piVar7 = (int *)&_data;');end=text.index('    iVar2 = internal_rand(5);',start)
        values=struct.unpack('<7i',runtime.pe.get_data(0x2721120-0x2000000,28));assert values==(2,4,8,48,64,256,9)
        pairs=[('  byte bVar8;\n',''),('  bVar8 = 0;\n',''),(text[start:end],'    memcpy(types, (const int[]){2, 4, 8, 48, 64, 256, 9}, sizeof types);\n')]
    for old,new in dict(pairs).items():
        count=text.count(old);assert count>0,(a,old)
        change=dict(original=old,replacement=new,occurrences=count,kind='restore native packed field, phase-specific stack slot, fixed initializer or typed boundary',machine_sites=sites)
        if a==SALVAGE and 'puVar5 = (undefined4 *)&UNK_02729894;' in old:
            change['native_literal']=dict(address='2729894',bytes=runtime.pe.get_data(0x2729894-0x2000000,32).hex(),copy_bytes=32);change['literal']='Select a nonland card to exile.'
        changes.append(change);text=text.replace(old,new)
    return text,changes


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('packed_filter_consumer_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert component['native_layouts_extra']==native_layouts_extra(Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()))
    for old in component['retained_bodies']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    for layout in component['unit_layouts'].values():assert layout['test_definition_t']['bytes']==216 and layout['target_definition_t']['bytes']==84
    assert all(any(v['name']==n and v['location']=='(DW_OP_fbreg: -236)' for v in component['locals'][CAVALIER]) for n in ['this_test','position'])
    assert any(v['name']=='types' and v['type']=='int[7]' and v['location']=='(DW_OP_fbreg: -48)' for v in component['locals'][UMORI])
    assert component['umori_types']==list(struct.unpack('<7i',machine_bytes(0x2721120,28)))==[2,4,8,48,64,256,9]
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['packed_filter_consumer_evidence']==detail and not entry['static']
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-packed-filter-consumers' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,{});assert literals==entry['recovered_literals'];text,_=rename(text,NAMES);assert function_body(cleaned(text,overlap),entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==2
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
        if a in HELPERS:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
    assert len(entries)==15 and sum(e['name'].startswith('card_') for e in entries.values())==11


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_layouts_extra(debug);(cache/'export-request-packed-filter-consumers.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared fifteen packed-field/filter consumers.');return
    assert 'packed_filter_consumers_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_layouts_extra=native_layouts_extra(debug),**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{'cards/ixalan.c','cards/ikoria.c','cards/modern_horizons.c','cards/modern_horizons_2.c','cards/aether_revolt.c'}),umori_types=list(struct.unpack('<7i',get(0x2721120,28))),functions={},retained_bodies=[dict(source=s,name=n,body_sha256=sha(function_body(read(root/s),n).encode())) for s,n in LEGACY],
        scope='Recover eleven handlers and four native Devil/Overload/creature-mana helpers together. Preserve DWORD and byte views of info_slot, temporary save/zero/restore, Cavalier phase-specific overlapping stack roles, Umori seven-entry type table, three 32-byte prompt copies plus 68 zero bytes and card-name member address. Use native token/test/target layouts, packed Vivien counter payload and unsigned mana-AI event gate. Keep native cancellation, loop, controller, selection, flag and return behavior; retain legacy helpers and all prior records.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES.get(meta[a]['name'],meta[a]['name']),source=meta[a]['source'],static=False) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-packed-filter-consumers'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);text,literals=recover_strings(text,runtime,{});text,_=rename(text,NAMES);body=cleaned(text,overlap)
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=2)
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,packed_filter_consumer_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in HELPERS)+'\n#endif\n\n'+header[at:];compile_sources(root,cache/'packed-filter-consumers-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-packed-filter-consumers/export.tsv').open(),delimiter='\t'):rows[row['address']]=row;shutil.copyfile(cache/'decompiled-packed-filter-consumers'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=15;m['card_handler_functions']+=11;m['helper_functions']+=4;m['shared_helper_interfaces']+=4
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['packed_filter_consumers_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'packed-filter-consumers-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=11,new_helpers=4)))


if __name__=='__main__':main()
