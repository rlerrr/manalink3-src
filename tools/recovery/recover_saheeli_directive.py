"""Recover Saheeli's Directive from complete native ranges, DWARF and VLA call frames."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from elftools.dwarf.descriptions import describe_DWARF_expr
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read
from recover_combat_mask_cards import cleaned
from recover_literals import readonly_string,byte_literal
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input
from saheeli_directive_body import BODY

ADDRESS='2564cc0'
NAME='card_saheelis_directive'
SOURCE='cards/commander_2018.c'
RANGES=[('2564cc0','2564da0','event routing, common generic spell and fallback Improvise'),
    ('2564da0','2564e48','casting, cancellation and signed AI adjustment'),
    ('2564e48','2564edb','deck/X minimum, zero path, Cage gate and reveal'),
    ('2564edb','2564f33','saved stack, count-sized VLA and removal loop'),
    ('2564f33','2564f88','bounded variadic prompt and native test initialization'),
    ('2564f88','256503e','eight-argument selection, ignored creature query and enter-play sequence'),
    ('256503e','2565086','count decrement, left compaction, stack restoration and source destruction'),
    ('2565086','25650c4','cancel path, ascending top insertion and mill')]
SITES={'2564e1c':'8b5038','2564e24':'83fa05','2564e27':'7e06','2564e29':'8d0452','2564e2c':'c1e002',
    '2564e64':'39c2','2564e66':'7e02','2564e6a':'85d2','2564e6c':'0f8497feffff',
    '2564ed3':'85ff','2564ed5':'0f858f010000','2564ee1':'89a5a8feffff','2564ee7':'8d049500000000',
    '2564eee':'e8dd721900','2564ef3':'29c4','2564ef9':'8d7c2420','2564f11':'89049f',
    '2564f39':'8954240c','2564f43':'c744240894417502','2564f4b':'c744240464000000','2564f56':'e8259b1600',
    '2564f7b':'8d4201','2564f7e':'66c745a40300','2564f84':'89459c',
    '2564fcc':'83f8ff','2564fd8':'8995b0feffff','2565012':'c744240800000100',
    '2565044':'4a','2565053':'8b049f','2565056':'89449ffc','2565064':'8ba5a8feffff',
    '2565086':'85d2','256508a':'7e2a','256508c':'8b9dacfeffff'}
FORMAT=dict(symbol='UNK_02754194',address='0x2754194',bytes=b'Select an artifact card with CMC <= %d.'.hex(),raw_occurrences=1,function_end='0x25650c4',materializations=[dict(address='2564f43',bytes=SITES['2564f43'])])


def native_details(debug):
    cu=next(c for c in debug.dwarf().iter_CUs() if value(c.get_top_DIE(),'DW_AT_name')==SOURCE)
    enums={value(x,'DW_AT_name'):value(x,'DW_AT_const_value') for d in cu.get_top_DIE().iter_children() if d.tag=='DW_TAG_enumeration_type' for x in d.iter_children() if value(x,'DW_AT_name') in {'SF_PLAYED_FROM_DECK','GS_X_SPELL','TYPE_CREATURE','TYPE_ARTIFACT','F5_CMC_LESSER_THAN_VALUE'}}
    assert enums==dict(SF_PLAYED_FROM_DECK=65536,GS_X_SPELL=4,TYPE_CREATURE=2,TYPE_ARTIFACT=64,F5_CMC_LESSER_THAN_VALUE=3),enums
    function=next(d for d in cu.get_top_DIE().iter_children() if value(d,'DW_AT_low_pc')==int(ADDRESS,16));found=[]
    def walk(d):
        if d.tag=='DW_TAG_variable' and inherited_value(d,'DW_AT_name')=='deck_copy':found.append(d)
        for child in d.iter_children():walk(child)
    walk(function);assert len(found)==1;t=type_of(found[0]);assert t.tag=='DW_TAG_array_type' and type_name(type_of(t))=='int'
    bound=next(x for x in t.iter_children() if x.tag=='DW_TAG_subrange_type').attributes['DW_AT_upper_bound'];assert bound.form=='DW_FORM_exprloc' and bound.value==[114,127]
    return dict(enums=enums,vla=dict(element_type='int',element_bytes=4,upper_bound_form=bound.form,upper_bound_bytes=bound.value,upper_bound_expression=describe_DWARF_expr(bound.value,debug.dwarf().structs),meaning='EDX - 1; count equals the revealed deck/X minimum',native_alignment=4))


def reconstruct(raw):
    original=strip_comments(raw);change=dict(original=original,replacement=BODY,occurrences=1,kind='Saheeli native VLA and variadic call-frame reconstruction',machine_sites=[dict(address=a,bytes=b) for a,b in SITES.items()])
    return BODY,[change]


def audit_component(component,manifest,sources,machine_bytes,root):
    entry=next(f for f in manifest['accepted_functions'] if f.get('saheeli_directive_evidence'));assert entry['address']==ADDRESS and entry['name']==NAME and not entry['static']
    assert component['runtime_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==component['debug_sha256'] and native_details(debug)==component['native_details']
    assert component['native_details']['vla']['upper_bound_expression']=='(DW_OP_breg2 (r2): -1)'
    assert [p['name'] for p in component['parameters'][ADDRESS]]==['player','card','event']
    for i,p in enumerate(component['parameters'][ADDRESS]):assert all(v['expression']==f'(DW_OP_fbreg: {i*4})' for v in p['location'])
    locals_=component['locals'][ADDRESS];assert any(v['name']=='msg' and v['type']=='char[100]' and v['location']=='(DW_OP_fbreg: -336)' for v in locals_)
    assert any(v['name']=='test' and v['type']=='test_definition_t' and v['location']=='(DW_OP_fbreg: -236)' for v in locals_)
    assert any(v['name']=='deck_copy' and all(x['expression']=='(DW_OP_breg7 (r7): 0)' for x in v['location']) for v in locals_)
    layout=component['unit_layouts'][SOURCE];assert layout['test_definition_t']['bytes']==216 and layout['test_definition_t']['members']['cmc']['offset']==128 and layout['test_definition_t']['members']['cmc_flag']['offset']==136
    detail=component['function'];assert entry['saheeli_directive_evidence']==detail and entry['reconstructed_literal_constants']==[FORMAT]
    body=function_body(sources[SOURCE].replace('\r\n','\n'),NAME);assert sha(body.encode())==entry['reconstruction_body_sha256']
    raw=read(root/'.recovery'/('decompiled-saheeli-directive' if component.get('planning') else 'decompiled-abi')/(ADDRESS+'.c'));assert sha(raw.encode())==entry['raw_sha256']
    text,changes=reconstruct(raw);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,ADDRESS,machine_bytes)==text
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());literal=readonly_string(runtime,int(FORMAT['address'],16));assert literal==bytes.fromhex(FORMAT['bytes'])
    text=text.replace('&'+FORMAT['symbol'],byte_literal(literal));overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];assert function_body(cleaned(text,overlap),NAME)==body
    assert readonly_string(runtime,0x275416c)==b"Cards revealed by Saheeli's Directive"
    assert component['title_operand']==dict(address='2564ea6',bytes='c744240c6c417502') and machine_bytes(0x2564ea6,8).hex()==component['title_operand']['bytes']
    assert sha(machine_bytes(int(ADDRESS,16),detail['machine_bytes']))==detail['machine_sha256'] and detail['machine_bytes']==1028 and detail['native_coff_storage']==2
    assert len(component['ranges'])==len(RANGES)
    for actual,(start,end,role) in zip(component['ranges'],RANGES):
        code=machine_bytes(int(start,16),int(end,16)-int(start,16));assert actual==dict(start=start,end=end,role=role,bytes=len(code),sha256=sha(code))
    assert sum(b['bytes'] for b in component['ranges'])==1028
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;insns={f'{i.address:x}':i for i in decoder.disasm(machine_bytes(int(ADDRESS,16),1028),int(ADDRESS,16))}
    for site in detail['canonical_calls']:
        ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and ins.mnemonic=='call' and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm==int(site['target'],16) and re.search(r'\b'+site['name']+r'\b',body)
    assert len([x for x in detail['canonical_calls'] if x['target']=='20bc670'])==3
    assert len([x for x in detail['canonical_calls'] if x['target']=='26a0250'])==2
    assert 'int deck_copy[max];' in body and 'char msg[100];' in body and 'test_definition_2022_t test;' in body
    assert 'if (max != 0)' in body and 'if (cage == 0)' in body and 'test.cmc = max + 1;' in body and '--amount;' in body
    assert re.search(r'scnprintf\(msg, sizeof\(msg\), "Select an artifact card with CMC <= %d.", max\)',body)
    assert 'if (card_added' not in body and 'is_what(-1, deck_copy[selected], TYPE_CREATURE);' in body
    assert 'for (int i = cage; i < amount; ++i)' in body and body.index('raw_put_iid_on_top_of_deck(')<body.index('mill(player, amount)')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args();root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'saheeli_directive_native_2022' not in interfaces and ADDRESS not in {f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),**debug_evidence(debug,meta,[ADDRESS]),native_details=native_details(debug),unit_layouts=native_layouts(debug,{SOURCE}),ranges=[dict(start=a,end=b,role=role,bytes=int(b,16)-int(a,16),sha256=sha(get(int(a,16),int(b,16)-int(a,16)))) for a,b,role in RANGES],title_operand=dict(address='2564ea6',bytes='c744240c6c417502'),
        scope='Recover Saheeli\'s Directive from all native control-flow ranges, the EDX-1 DWARF VLA bound, stack allocation/restoration, all eight selector arguments and the missing variadic prompt integer. Use the recovered native Improvise frontend and 216-byte test. Preserve signed deck/X minimum, max-zero path skipping explicit destruction, Cage gate/reveal behavior, initial-count prompt/filter, shrinking buffer, ascending restoration then mill, ignored creature query, unguarded add/flag/put sequence, event fallthrough and signed AI adjustment.')
    raw=read(cache/'decompiled-saheeli-directive'/(ADDRESS+'.c'));text,changes=reconstruct(raw);text=text.replace('&'+FORMAT['symbol'],byte_literal(readonly_string(runtime,int(FORMAT['address'],16))))
    overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];body=cleaned(text,overlap);assert not UNRESOLVED.search(strip_comments(body))
    code=get(int(ADDRESS,16),int(meta[ADDRESS]['high_pc'],16)-int(ADDRESS,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],native_coff_storage=2)
    symbols=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(ADDRESS,16)];assert symbols==[2]
    targets={int(f['address'],16):f for f in m['accepted_functions']};decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for ins in decoder.disasm(code,int(ADDRESS,16)):
        if ins.mnemonic!='call' or ins.operands[0].type!=X86_OP_IMM or ins.operands[0].imm not in targets:continue
        target=targets[ins.operands[0].imm];assert not target['static'] and re.search(r'\b'+target['name']+r'\b',body),(target['name'],f'{ins.address:x}')
        detail['canonical_calls'].append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=target['address'],name=target['name']))
    proof['function']=detail;entry=dict(address=ADDRESS,name=NAME,source=SOURCE,line=meta[ADDRESS]['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],reconstructed_literal_constants=[FORMAT],saheeli_directive_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,NAME).encode()))
    source=read(root/SOURCE)+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{ADDRESS}; original {SOURCE}:{meta[ADDRESS]["line"]}. */\n'+body+'\n#endif\n'
    compile_sources(root,cache/'saheeli-directive-check',{SOURCE:source});proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+[entry]},{SOURCE:source},get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert ADDRESS not in {f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};row=next(csv.DictReader((cache/'decompiled-saheeli-directive/export.tsv').open(),delimiter='\t'));assert row['address']==ADDRESS and ADDRESS in rows;rows[ADDRESS]=row;shutil.copyfile(cache/'decompiled-saheeli-directive'/(ADDRESS+'.c'),cache/'decompiled-abi'/(ADDRESS+'.c'))
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        (root/SOURCE).write_bytes(source.replace('\n','\r\n').encode('latin1'));m['accepted_functions'].append(entry);m['accepted']+=1;m['card_handler_functions']+=1;m['source_files'][SOURCE]+=1;save_json(mp,m);interfaces['saheeli_directive_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'saheeli-directive-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=1,new_helpers=0,native_bytes=1028,vla_arrays=1)))


if __name__=='__main__':main()
