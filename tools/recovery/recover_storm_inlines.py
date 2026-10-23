"""Recover inline storm collector/reset code without inventing standalone native entries."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from elftools.dwarf.locationlists import LocationParser,LocationExpr
from elftools.dwarf.descriptions import describe_DWARF_expr
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from recover_test_consumers import read
from recover_spell_copy import compile_sources,format_c
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body
from storm_inline_bodies import RESET,KEEP

SOURCE='functions/rules_engine.c'
OWNER='26c9140'
TEMPLATES={'reset_storm_count':RESET,'keep_storm_count':KEEP}
NAMES={n:n+'_2022' for n in TEMPLATES}
ENUMS={'SUBTYPE_WEREWOLF':212,'CARD_ID_PEER_THROUGH_DEPTHS':7344,'CARD_ID_REACH_THROUGH_MISTS':7353,'CARD_ID_RULES_ENGINE':24922,
       'EVENT_CAN_SKIP_TURN':106,'EVENT_END_TRIGGER':16896,'TRIGGER_SPELL_CAST':211,'TYPE_LAND':1,'TYPE_CREATURE':2,'TYPE_ENCHANTMENT':4,'TYPE_SPELL':56,
       'SF_NOT_CAST':262144,'TRAP_NONCREATURES_PLAYED':1032,'TRAP_COT_FLAG':1035,'TRAP_PEER_THROUGH_DEPTHS':1546,'TRAP_REACH_THROUGH_MISTS':1547}
SITES={'26c92c7':'8b4d08','26c92cc':'85c9','26c92ce':'0f95c1','26c92d1':'88cb',
 '26c99a8':'8b7d08','26c99ab':'85ff','26c99ad':'0f843dfaffff','26c99b3':'837d106a','26c99bd':'84db',
 '26ca1a0':'a18c397a00','26ca1a5':'39450c','26ca1b4':'0b1d1c7e7300','26ca1c5':'c744240800000400',
 '26ca1d9':'e8f2fbfaff','26ca1de':'817d1000420000','26ca214':'f7c701000000',
 '26ca239':'bb40000000','26ca240':'80cb80','26ca243':'83e738','26ca24e':'80cf01',
 '26ca267':'3db01c0000','26ca288':'3db91c0000','26ca2a9':'83e03e','26ca2ba':'e8b1dcffff',
 '26c99c8':'8d5d90','26c99e2':'e8b967f9ff','26c99ea':'c745bcd4000000','26c99f1':'e8ca6af9ff',
 '26c9a9e':'83bec000000042','26c9aaa':'c786c000000042000000','26c9ab4':'8986b4000000','26c9abd':'8986b8000000',
 '26c9acf':'3d5a610000','26ca7f8':'c744241026000000','26ca81b':'c744241013000000','26ca83e':'c74424107b000000','26ca85d':'c744241020000000',
 '26ca9a7':'c744241026000000','26ca9c7':'c744241013000000','26ca9e7':'c74424107b000000','26caa07':'c744241020000000',
 '26cadfa':'ba08040000','26cae04':'e82787ffff','26cae70':'ba0b060000','26cae7a':'e8b186ffff','26cae90':'ba0a060000','26cae9a':'e89186ffff',
 '26ca788':'31c9','26ca78c':'c7042400000000','26ca93e':'31c9','26ca947':'c7042400000000',
 '26ca7f1':'8b450c','26ca808':'c7042400000000','26ca9bb':'c7042400000000'}


def inline_evidence(debug,runtime):
    dw=debug.dwarf();cu=next(c for c in dw.iter_CUs() if value(c.get_top_DIE(),'DW_AT_name')==SOURCE);base=value(cu.get_top_DIE(),'DW_AT_low_pc');lp=LocationParser(dw.location_lists())
    def loc(d):
        attr=d.attributes.get('DW_AT_location')
        if not attr:return None
        x=lp.parse_from_attribute(attr,cu['version'],die=d)
        return describe_DWARF_expr(x.loc_expr,dw.structs) if isinstance(x,LocationExpr) else [dict(start=hex(y.begin_offset),end=hex(y.end_offset),expression=describe_DWARF_expr(y.loc_expr,dw.structs)) for y in x if hasattr(y,'loc_expr')]
    found={}
    for d in cu.iter_DIEs():
        if d.tag!='DW_TAG_inlined_subroutine' or inherited_value(d,'DW_AT_name') not in TEMPLATES:continue
        name=inherited_value(d,'DW_AT_name');ranges=[]
        for rr in dw.range_lists().get_range_list_at_offset(value(d,'DW_AT_ranges'),cu=cu):
            assert hasattr(rr,'begin_offset') and not rr.is_absolute
            low,high=base+rr.begin_offset,base+rr.end_offset;code=runtime.pe.get_data(low-0x2000000,high-low)
            ranges.append(dict(start=f'{low:x}',end=f'{high:x}',bytes=high-low,sha256=sha(code)))
        locals_=[]
        def walk(x):
            for v in x.iter_children():
                if v.tag=='DW_TAG_variable':locals_.append(dict(name=inherited_value(v,'DW_AT_name'),type=type_name(type_of(v)),location=loc(v)))
                if v.tag=='DW_TAG_lexical_block':walk(v)
        walk(d)
        found[name]=dict(die=f'{d.offset:x}',entry=f'{value(d,"DW_AT_entry_pc"):x}',ranges_offset=value(d,'DW_AT_ranges'),ranges=ranges,
            parameters=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=loc(p)) for p in d.iter_children() if p.tag=='DW_TAG_formal_parameter'],locals=locals_)
    enum_values={value(d,'DW_AT_name'):value(d,'DW_AT_const_value') for d in cu.iter_DIEs() if d.tag=='DW_TAG_enumerator' and value(d,'DW_AT_name') in ENUMS}
    assert enum_values==ENUMS and set(found)==set(TEMPLATES)
    return dict(cu_base=f'{base:x}',enum_values=enum_values,routines=found)


def audit_component(component,manifest,sources,machine_bytes,root):
    assert component['runtime_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert component['enum_values']==ENUMS and component['cu_base']=='26c1230' and set(component['routines'])==set(TEMPLATES)
    owner=component['owner'];assert owner['address']==OWNER and owner['source']==SOURCE and owner['name']=='card_rules_engine' and owner['machine_bytes']==0x1df7
    assert sha(machine_bytes(int(OWNER,16),owner['machine_bytes']))==owner['machine_sha256']
    raw=read(root/'.recovery'/owner['raw_export']);assert sha(raw.encode())==owner['raw_sha256']
    assert component['instance_layout']==dict(bytes=300,targets=116,werewolf_human_count=180,werewolf_ai_count=184,werewolf_marker=192)
    assert component['unit_layouts'][SOURCE]['target_definition_t']['bytes']==84 and component['unit_layouts'][SOURCE]['target_definition_t']['members']['required_subtype']['offset']==44
    for at,expected in SITES.items():assert machine_bytes(int(at,16),len(bytes.fromhex(expected))).hex()==expected
    source=sources[SOURCE].replace('\r\n','\n');decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    reset_body=function_body(source,NAMES['reset_storm_count']);keep_body=function_body(source,NAMES['keep_storm_count'])
    for name,detail in component['routines'].items():
        expected_meta=('68be050','26c99a8',351528,18,1307,35) if name=='keep_storm_count' else ('68be08e','26c99bd',351704,13,855,26)
        assert (detail['die'],detail['entry'],detail['ranges_offset'],sum(bool(r['bytes']) for r in detail['ranges']),sum(r['bytes'] for r in detail['ranges']),len(detail['calls']))==expected_meta
        fragment=detail['fragment'];assert fragment in manifest['source_fragments'] and fragment['address']==OWNER and fragment['raw_sha256']==owner['raw_sha256']
        assert source.count(fragment['start_marker'])==source.count(fragment['end_marker'])==1
        body=source.split(fragment['start_marker'])[1].split(fragment['end_marker'])[0].strip();expected=format_c(TEMPLATES[name]).strip()
        assert body==expected and sha(body.encode())==fragment['body_sha256']
        assert len(detail['parameters'])==(3 if name=='keep_storm_count' else 2)
        assert {p['name'] for p in detail['parameters']}==({'player','card','event'} if name=='keep_storm_count' else {'player','card'})
        for rr in detail['ranges']:
            low,high=int(rr['start'],16),int(rr['end'],16);assert high-low==rr['bytes'] and int(OWNER,16)<=low<=high<=int(OWNER,16)+owner['machine_bytes']
            assert sha(machine_bytes(low,rr['bytes']))==rr['sha256']
        instructions={f'{i.address:x}':i for i in decoder.disasm(machine_bytes(int(OWNER,16),owner['machine_bytes']),int(OWNER,16))}
        for site in detail['calls']:
            ins=instructions[site['address']];assert ins.bytes.hex()==site['bytes'] and ins.mnemonic=='call' and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm==int(site['target'],16)
            assert re.search(r'\b'+site['name']+r'\s*\(',body+(reset_body if name=='keep_storm_count' else ''))
    assert any(v['name']=='td' and v['type']=='target_definition_t' and v['location']=='(DW_OP_fbreg: -120)' for v in component['routines']['reset_storm_count']['locals'])
    assert reset_body.count('remove_all_counters_from_source(')==8 and reset_body.count('increase_trap_condition_impl(')==10
    assert 'RULES_ENGINE_CSV_ID_2022 = 0x615a' in reset_body and 'TRAP_NONCREATURES_PLAYED_2022 = 0x408' in keep_body
    assert 'else if (event == EVENT_CAN_SKIP_TURN)' in keep_body and 'EVENT_CLEANUP' not in keep_body and 'SF_NOT_CAST' in keep_body
    assert len([s for s in component['routines']['reset_storm_count']['calls'] if s['target']=='26d4c50'])==8
    assert len([s for s in component['routines']['reset_storm_count']['calls'] if s['target']=='26c3530'])==10
    assert component['legacy_dispatch_sha256']==sha(function_body(source,'card_rules_engine').encode())
    assert 'keep_storm_count_2022(' not in function_body(source,'card_rules_engine')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'storm_inline_routines_2022' not in interfaces and 'storm_family_native_2022' in interfaces
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),**inline_evidence(debug,runtime),unit_layouts=native_layouts(debug,{SOURCE}),instance_layout=dict(bytes=300,targets=116,werewolf_human_count=180,werewolf_ai_count=184,werewolf_marker=192),
        legacy_dispatch_sha256=sha(function_body(read(root/SOURCE),'card_rules_engine').encode()),
        scope='Two logical native storm routines are inlined inside the Rules Engine dispatcher. Reconstruct private source helpers without adding standalone native function entries. Preserve human-only guards, copied-spell exclusion and call order, combined type modes, noncreature/spell-specific traps, CAN_SKIP_TURN reset, 84-byte target lifetime, pre-clear Werewolf snapshots and both source-aware four-counter clear paths. Keep the existing dispatcher and legacy routines unchanged; native rules-card ID and trap-domain integration remain pending.')
    raw=read(cache/'decompiled-storm-dispatch'/f'{OWNER}.c');owner_bytes=runtime.pe.get_data(int(OWNER,16)-0x2000000,0x1df7)
    proof['owner']=dict(address=OWNER,name='card_rules_engine',source=SOURCE,machine_bytes=len(owner_bytes),machine_sha256=sha(owner_bytes),raw_export=f'decompiled-storm-dispatch/{OWNER}.c',raw_sha256=sha(raw.encode()))
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets[0x26c3530]=dict(name='increase_trap_condition_impl',source=SOURCE,static=True)
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;instructions=list(decoder.disasm(owner_bytes,int(OWNER,16)));source=read(root/SOURCE);fragments=[]
    for name in ['reset_storm_count','keep_storm_count']:
        detail=proof['routines'][name];body=format_c(TEMPLATES[name]).strip();start=f'/* ManalinkEh 2022 inline {name}: owner 0x{OWNER}; DIE 0x{detail["die"]}. */';end=f'/* End ManalinkEh 2022 inline {name}. */'
        fragment=dict(source=SOURCE,original_function='card_rules_engine::'+name+' [inlined]',address=OWNER,raw_export=proof['owner']['raw_export'],raw_sha256=sha(raw.encode()),start_marker=start,end_marker=end,body_sha256=sha(body.encode()),scope=proof['scope'])
        detail['fragment']=fragment;detail['calls']=[]
        for ins in instructions:
            if ins.mnemonic!='call' or ins.operands[0].type!=X86_OP_IMM or not any(int(rr['start'],16)<=ins.address<int(rr['end'],16) for rr in detail['ranges']):continue
            target=ins.operands[0].imm;entry=targets.get(target)
            if entry is None:
                row=meta[f'{target:x}'];entry=dict(name=row['name'],source=row['source'],static=False)
            assert not entry.get('static') or entry['source']==SOURCE
            detail['calls'].append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{target:x}',name=entry['name']))
        fragments.append(fragment);source+='\n\n#ifndef SHANDALAR\n'+start+'\n'+body+'\n'+end+'\n#endif\n'
    compile_sources(root,cache/'storm-inline-check',{SOURCE:'#define strncasecmp _strnicmp\n'+source})
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n);audit_component(proof,{**m,'source_fragments':m['source_fragments']+fragments},{SOURCE:source},get,root)
    if args.apply:
        pinned=cache/'decompiled-support/26c9140-storm-inline-owner.c';pinned.parent.mkdir(exist_ok=True)
        if pinned.exists():assert sha(read(pinned).encode())==proof['owner']['raw_sha256']
        else:shutil.copyfile(cache/'decompiled-storm-dispatch'/f'{OWNER}.c',pinned)
        proof['owner']['raw_export']='decompiled-support/'+pinned.name
        for fragment in fragments:fragment['raw_export']=proof['owner']['raw_export']
        (root/SOURCE).write_bytes(source.replace('\n','\r\n').encode('latin1'));m['source_fragments']+=fragments;save_json(mp,m);interfaces['storm_inline_routines_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'storm-inline-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_source_fragments=2,new_standalone_native_functions=0,dispatcher_integration='pending')))


if __name__=='__main__':main()
