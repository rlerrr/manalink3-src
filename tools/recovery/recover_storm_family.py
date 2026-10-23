"""Recover native storm bookkeeping, its readers, Ral, Tuvasa and Thousand-Year Storm."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from inspect_release import Image,value
from export_abi import type_of,type_name
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read,rename
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input

RULES='functions/rules_engine.c'
HELPERS=['26c7f70','26c81a0','26c81e0','26c8240','26c8280','26c82c0','26c90c0','21d4b10']
CARDS=['21d4b70','2565940','2621080']
ADDRESSES=HELPERS+CARDS
APIS=['increment_storm_count','get_specific_storm_count','get_storm_count','get_stormcreature_count','get_storminstantsorcery_count','get_stormenchantment_count','get_stormcolor_count','count_type_in_hand']
NAMES={n:n+'_2022' for n in APIS}
SUPPORT={'26c3530':('increase_trap_condition_impl',True),'26c8f90':('get_trap_condition',False)}
LEGACY=[(RULES,n) for n in ['increment_storm_count','get_specific_storm_count','get_storm_count','get_stormcreature_count','get_stormcolor_count','increase_trap_condition_impl','get_trap_condition','get_rules_engine_card','keep_storm_count','reset_storm_count']]+[('cards/mercadian_masques.c','count_type_in_hand')]
HEADER_LABEL='/* October 2022 storm bookkeeping and hand-type reader APIs. */'
DEFS_LABEL='/* October 2022 storm mode domain; retain the legacy storm_mode_t. */'
FLAGS={'STORM_BLACK':2,'STORM_BLUE':4,'STORM_GREEN':8,'STORM_RED':16,'STORM_WHITE':32,'STORM_CREATURE':64,'STORM_ENCHANTMENT':128,'STORM_INSTANT_SORCERY':256}
DEFINITIONS=DEFS_LABEL+'\n#ifndef SHANDALAR\ntypedef enum {\n'+''.join(f'  {n.replace("STORM_","STORM2022_",1)} = 0x{v:x},\n' for n,v in FLAGS.items())+'} storm_mode_2022_t;\n#endif\n'
GLOBAL_ADDRESS=0x2721d50
MODE_SITES={'26c7fd8':'f7c640000000','26c7fe4':'f7c680000000','26c7ff0':'f7c600010000'}
TRAP_ARGUMENT_SITES={
    '26c7fc0':'c7042401000000','26c7fc7':'b901000000','26c7fcc':'ba01030000','26c7fd1':'89d8',
    '26c8040':'c7450801000000','26c804a':'89d8','26c804c':'b901000000','26c8052':'ba07040000',
    '26c8060':'c7042401000000','26c8067':'b901000000','26c806c':'ba06040000','26c8071':'89d8',
    '26c8080':'c7042401000000','26c8087':'b901000000','26c808c':'ba05040000','26c8091':'89d8',
    '26c80a8':'c7042401000000','26c80af':'b901000000','26c80b4':'ba04040000','26c80b9':'89d8',
    '26c80d0':'c7042401000000','26c80d7':'b901000000','26c80dc':'ba03040000','26c80e1':'89d8'}


def rules_type(source):
    at=source.index('typedef struct\n{\n  target_t targets[5];');end=source.index('} PACKED rules_engine_globals_t;',at)+len('} PACKED rules_engine_globals_t;')
    return strip_comments(source[at:end])


def layout_checks(layout,counters):
    text='/* October 2022 storm backing-state layout checks. */\n#ifndef SHANDALAR\n'
    text+='STATIC_ASSERT(sizeof(rules_engine_card) == 8, storm_native_card_slots);\nSTATIC_ASSERT(sizeof(rules_engine_globals_t) == 152, storm_native_globals_bytes);\n'
    for name,member in layout['members'].items():text+=f'STATIC_ASSERT(offsetof(rules_engine_globals_t, {name}) == {member["offset"]}, storm_native_offset_{name});\n'
    for name,value_ in counters.items():text+=f'STATIC_ASSERT({name} == {value_}, storm_native_{name});\n'
    text+='STATIC_ASSERT(sizeof(storm_mode_2022_t) == 4, storm_native_mode_bytes);\nSTATIC_ASSERT(STORM_CREATURE == 1, storm_legacy_creature_mode_preserved);\n'
    return text+'#endif\n'


def reconstruct(a,raw):
    text=strip_comments(raw);changes=[]
    if a=='26c7f70':
        for literal,name in [('0x40U','STORM2022_CREATURE'),('0x80U','STORM2022_ENCHANTMENT'),('0x100U','STORM2022_INSTANT_SORCERY')]:
            old='mode & '+literal;assert text.count(old)==1
            new='mode & '+name;changes.append(dict(original=old,replacement=new,occurrences=1,kind='native storm mode domain',machine_sites=[dict(address=a,bytes=b) for a,b in MODE_SITES.items()]));text=text.replace(old,new)
    if a=='26c90c0':
        old='4 < clr - COLOR_BLACK';new='4U < (uint)(clr - COLOR_BLACK)';assert text.count(old)==1
        changes.append(dict(original=old,replacement=new,occurrences=1,kind='explicit native unsigned color-range guard',machine_sites=[dict(address='26c90c9',bytes='83fa04'),dict(address='26c90cc',bytes='760a')]))
        text=text.replace(old,new)
    return text,changes


def state_evidence(debug):
    cu=next(c for c in debug.dwarf().iter_CUs() if value(c.get_top_DIE(),'DW_AT_name')==RULES)
    die=next(d for d in cu.get_top_DIE().iter_children() if d.tag=='DW_TAG_variable' and value(d,'DW_AT_name')=='rules_engine_card')
    typedef=next(d for d in cu.get_top_DIE().iter_children() if d.tag=='DW_TAG_typedef' and value(d,'DW_AT_name')=='rules_engine_globals_t');t=type_of(typedef)
    return dict(location=value(die,'DW_AT_location'),type=type_name(type_of(die)),enum_values={value(d,'DW_AT_name'):value(d,'DW_AT_const_value') for d in cu.iter_DIEs() if d.tag=='DW_TAG_enumerator' and (value(d,'DW_AT_name') or '').startswith('STORM_')},
        counter_values={value(d,'DW_AT_name'):value(d,'DW_AT_const_value') for d in cu.iter_DIEs() if d.tag=='DW_TAG_enumerator' and value(d,'DW_AT_name') in {'COUNTER_VORTEX','COUNTER_BLOOD','COUNTER_CHARGE','COUNTER_DEVOTION'}},
        layout=dict(bytes=value(t,'DW_AT_byte_size'),members={value(d,'DW_AT_name'):dict(offset=value(d,'DW_AT_data_member_location'),type=type_name(type_of(d))) for d in t.iter_children() if d.tag=='DW_TAG_member'}))


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('storm_family_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions'])
    assert component['runtime_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    state=component['state'];assert state['location']==[3,*GLOBAL_ADDRESS.to_bytes(4,'little')] and state['type']=='int[2]' and state['enum_values']==FLAGS
    assert state['layout']['bytes']==152 and state['layout']['members']['bytes_persistent']==dict(offset=112,type='uint8_t[2]') and state['layout']['members']['nybbles_per_turn']==dict(offset=120,type='uint8_t[8]')
    assert machine_bytes(GLOBAL_ADDRESS,8)==bytes.fromhex('ffffffffffffffff') and component['native_global_storage']==3
    rules=sources[RULES].replace('\r\n','\n');assert 'static int rules_engine_card[2] = {-1,-1};' in rules and sha(rules_type(rules).encode())==component['rules_type_sha256']
    assert state['counter_values']=={'COUNTER_BLOOD':19,'COUNTER_DEVOTION':32,'COUNTER_VORTEX':38,'COUNTER_CHARGE':123}
    assert layout_checks(state['layout'],state['counter_values']) in rules
    assert DEFINITIONS in read(root/'defs.h') or component.get('planning')
    for old in component['retained_bodies']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    assert [(x['source'],x['name']) for x in component['retained_bodies']]==LEGACY
    assert component['legacy_trap_scope']==dict(increment_traps=[0x301,0x403,0x404,0x405,0x406,0x407],value=1,add=1,color_read_traps=[0x403,0x404,0x405,0x406,0x407],persistent_byte_offset=113,nibble_byte_offsets=[121,122,123],nibble_max=15,byte_max=255)
    for a,d in component['legacy_bindings'].items():
        assert (d['name'],d['static'])==SUPPORT[a] and d['source']==RULES
        assert sha(machine_bytes(int(a,16),d['machine_bytes']))==d['machine_sha256']
    for a in ADDRESSES:
        params=component['parameters'][a];expected=0 if a=='26c81e0' else 2 if a in {'26c7f70','26c90c0','21d4b10'} else 3 if a in CARDS else 1
        assert len(params)==expected and [p['location'] if isinstance(p['location'],str) else p['location'][0]['expression'] for p in params]==[f'(DW_OP_fbreg: {i*4})' for i in range(expected)]
    assert component['unit_layouts']['cards/war_of_the_spark.c']['target_definition_t']['bytes']==84
    assert any(v['name']=='td' and v['type']=='target_definition_t' and v['location']=='(DW_OP_fbreg: -104)' for v in component['locals']['21d4b70'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global']
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['storm_family_evidence']==detail and not entry['static']
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-storm-family' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,{});assert literals==entry['recovered_literals'];text,_=rename(text,NAMES)
        assert function_body(cleaned(text,overlap),entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==2
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands)
            assert re.search(r'\b'+site['name']+r'\b',body)
        for site in detail['rules_card_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_MEM and GLOBAL_ADDRESS<=o.mem.disp<GLOBAL_ADDRESS+8 for o in ins.operands)
        if a in HELPERS:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
    for a,counter in [('26c81a0','COUNTER_VORTEX'),('26c8240','COUNTER_BLOOD'),('26c8280','COUNTER_CHARGE'),('26c82c0','COUNTER_DEVOTION')]:assert counter in function_body(sources[RULES],entries[a]['name'])
    increment=function_body(rules,NAMES['increment_storm_count']);assert increment.count('increase_trap_condition_impl(')==6 and all(x in increment for x in ['COUNTER_VORTEX','COUNTER_BLOOD','COUNTER_DEVOTION','COUNTER_CHARGE'])
    assert sorted(int(x,16) for x in re.findall(r'increase_trap_condition_impl\(player,\s*(0x[0-9a-f]+),\s*1,\s*1\)',increment))==component['legacy_trap_scope']['increment_traps']
    for at,expected in TRAP_ARGUMENT_SITES.items():assert machine_bytes(int(at,16),len(bytes.fromhex(expected))).hex()==expected
    assert any(r['target']=='21d4b10' for r in component['functions']['21d4b70']['canonical_calls'])
    assert any(r['target']=='26c82c0' for r in component['functions']['2565940']['canonical_calls']) and any(r['target']=='26c8280' for r in component['functions']['2621080']['canonical_calls'])
    assert any(r['target']=='22058a0' for r in component['functions']['21d4b70']['callback_references'])


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'storm_family_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    state=state_evidence(debug);assert state['enum_values']==FLAGS
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),**debug_evidence(debug,meta,ADDRESSES+list(SUPPORT)),state=state,unit_layouts=native_layouts(debug,{'cards/war_of_the_spark.c'}),functions={},legacy_bindings={},
        rules_type_sha256=sha(rules_type(read(root/RULES)).encode()),retained_bodies=[dict(source=s,name=n,body_sha256=sha(function_body(read(root/s),n).encode())) for s,n in LEGACY],
        legacy_trap_scope=dict(increment_traps=[0x301,0x403,0x404,0x405,0x406,0x407],value=1,add=1,color_read_traps=[0x403,0x404,0x405,0x406,0x407],persistent_byte_offset=113,nibble_byte_offsets=[121,122,123],nibble_max=15,byte_max=255),
        scope='Recover the native storm increment and six readers, shared hand-type counter and three card consumers. Preserve the legacy storm and trap APIs, collector/reset routines and private hand counter. Verify the shared two-slot rules-card array and 152-byte backing layout; reuse existing trap support only for the fixed add-one byte/nibble and color-read operations used here. Keep native Blood/Devotion/Charge/Vortex selection, shifted mode bits, silent-counter scope, unsigned color guard and card trigger/copy flow. Legacy event collection/reset integration remains downstream work.')
    symbols=[s for s in debug.symbols if s[0]=='_rules_engine_card' and s[2]>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[s[2]-1].VirtualAddress+s[1]==GLOBAL_ADDRESS];assert len(symbols)==1 and symbols[0][4]==3;proof['native_global_storage']=3
    for a,(name,private) in SUPPORT.items():
        size=int(meta[a]['high_pc'],16)-int(a,16);proof['legacy_bindings'][a]=dict(name=name,source=RULES,static=private,machine_bytes=size,machine_sha256=sha(get(int(a,16),size)))
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES.get(meta[a]['name'],meta[a]['name']),source=meta[a]['source'],static=False) for a in ADDRESSES});targets.update({int(a,16):d for a,d in proof['legacy_bindings'].items()})
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={RULES:read(root/RULES)+'\n\n'+layout_checks(state['layout'],state['counter_values'])};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=NAMES.get(row['name'],row['name']);raw=read(cache/'decompiled-storm-family'/f'{a}.c');text,changes=reconstruct(a,raw);text,literals=recover_strings(text,runtime,{});text,_=rename(text,NAMES);body=cleaned(text,overlap)
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[2],(a,storage)
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],rules_card_references=[],native_coff_storage=2)
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type==X86_OP_IMM and op.imm in targets:
                    target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                    site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
                if op.type==X86_OP_MEM and GLOBAL_ADDRESS<=op.mem.disp<GLOBAL_ADDRESS+8:detail['rules_card_references'].append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex()))
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,storm_family_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
        sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in HELPERS)+'\n#endif\n\n'+header[at:]
    defs=read(root/'defs.h');assert DEFS_LABEL not in defs;at=defs.rfind('#endif');defs=defs[:at]+DEFINITIONS+'\n'+defs[at:]
    scratch=cache/'storm-family-check';scratch.mkdir(exist_ok=True);(scratch/'defs.h').write_bytes(defs.encode('latin1'))
    checked={**sources,'cards/mercadian_masques.c':read(root/'cards/mercadian_masques.c')};checked[RULES]='#define strncasecmp _strnicmp\n'+checked[RULES];compile_sources(root,scratch,checked,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-storm-family/export.tsv').open(),delimiter='\t'):
            rows[row['address']]=row;shutil.copyfile(cache/'decompiled-storm-family'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));(root/'defs.h').write_bytes(defs.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=11;m['card_handler_functions']+=3;m['helper_functions']+=8;m['shared_helper_interfaces']+=8
        for e in entries:m['source_files'][e['source']]+=1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['storm_family_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'storm-family-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=3,new_helpers=8,private_helpers=0)))


if __name__=='__main__':main()
