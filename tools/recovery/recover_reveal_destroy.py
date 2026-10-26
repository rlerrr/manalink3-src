"""Recover Creativity, the native reveal VLA, and packed destruction tracking."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from elftools.dwarf.locationlists import LocationParser,LocationExpr,BaseAddressEntry
from elftools.dwarf.descriptions import describe_DWARF_expr
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import REVEAL_DESTROY_API_NAMES,strip_comments,UNRESOLVED
from recover_combat_mask_cards import cleaned
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_test_consumers import read,rename
from recover_spell_copy import compile_sources
from recover_literals import readonly_string,byte_literal
from recover_temporary_exile import save_json,sha
from reveal_destroy_bodies import REVEAL,packed_callback,creativity
from audit_source import function_body,target_validation_input

HELPERS=['2677c50','269f880','269ad40']
CARD='20c3280'
ADDRESSES=HELPERS+[CARD]
NAMES={**REVEAL_DESTROY_API_NAMES,'fx_permanents_destroyed_this_way_accumulate':'fx_permanents_destroyed_this_way_accumulate_2022'}
HEADER_LABEL='/* October 2022 native reveal and destruction-tracking APIs. */'
PACKED_SITES={'2677fac':'66ff464e','2677fd9':'66ff86c4000000','2677fed':'66ff86c6000000',
 '2677dd0':'668b564e','2677dd8':'88484e','2677de3':'88484f','2677de6':'66c7464e0000',
 '2677e43':'668988c4000000','2677e53':'6689b8c6000000'}
REVEAL_SITES={'269ad4c':'f7451400400000','269ad70':'668b98ae000000','269ade9':'8a98af000000',
 '269ae23':'e848af0400','269ae28':'c1e002','269ae2b':'e8a0130600','269ae30':'29c4','269ae32':'8d442418',
 '269aeaf':'09f8','269afda':'c1e710','269afe0':'09f9','269b135':'29c7','269b18e':'21d0','269b197':'c745e000000000'}
CARD_SITES={'20c3800':'8a4336','20c3803':'c1e002','20c3806':'e8c5896300','20c380b':'29c4',
 '20c380f':'8d442420','20c383f':'8944b3fc','20c3873':'8944b3fc','20c3964':'8944b3fc',
 '20c3958':'c704245a610000','20c3867':'c7042461610000','20c39a1':'8ba518ffffff'}
PROMPTS={'269ad40':dict(address='27642a0',sites={'269b0e0':'c744240ca0427602','269b112':'c744240ca0427602'}),
 CARD:dict(address='2729450',sites={'20c3983':'c744240c50947202'})}



def array_bounds(debug):
    result={};dw=debug.dwarf();lp=LocationParser(dw.location_lists())
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {'functions/functions.c','cards/aether_revolt.c'}:continue
        def walk(d,a):
            if d.tag=='DW_TAG_variable' and inherited_value(d,'DW_AT_name') in {'deck_copy','t_array'}:
                name=inherited_value(d,'DW_AT_name');t=type_of(d);assert t.tag=='DW_TAG_array_type' and type_name(type_of(t))=='int'
                sub=next(x for x in t.iter_children() if x.tag=='DW_TAG_subrange_type');attr=sub.attributes['DW_AT_upper_bound']
                if attr.form=='DW_FORM_ref4':
                    bound=sub.get_DIE_from_attribute('DW_AT_upper_bound');assert value(bound,'DW_AT_artificial')
                    loc=lp.parse_from_attribute(bound.attributes['DW_AT_location'],cu['version'],die=bound);assert not isinstance(loc,LocationExpr)
                    result[a]=dict(name=name,upper_bound_form=attr.form,referent=hex(bound.offset),locations=[dict(base=hex(x.base_address)) if isinstance(x,BaseAddressEntry) else dict(start=hex(x.begin_offset),end=hex(x.end_offset),expression=describe_DWARF_expr(x.loc_expr,dw.structs)) for x in loc])
                else:
                    assert attr.form=='DW_FORM_exprloc'
                    result[a]=dict(name=name,upper_bound_form=attr.form,referent=None,locations=[dict(expression=describe_DWARF_expr(attr.value,dw.structs))])
            for c in d.iter_children():walk(c,a)
        for d in cu.get_top_DIE().iter_children():
            at=value(d,'DW_AT_low_pc')
            if at in {0x269ad40,0x20c3280}:walk(d,f'{at:x}')
    assert set(result)=={'269ad40',CARD}
    return result


def reconstruct(a,raw,runtime):
    text=strip_comments(raw);changes=[];prompts=[]
    if a=='2677c50':text,changes=packed_callback(text)
    elif a=='269ad40':changes=[dict(original=text,replacement=REVEAL,occurrences=1,kind='native deck-copy VLA and reveal call-frame reconstruction')];text=REVEAL
    elif a==CARD:text,changes=creativity(text)
    sites=PACKED_SITES if a=='2677c50' else REVEAL_SITES if a=='269ad40' else CARD_SITES if a==CARD else {}
    for change in changes:change['machine_sites']=[dict(address=a,bytes=b) for a,b in sites.items()]
    if a in PROMPTS:
        p=PROMPTS[a];address=int(p['address'],16);data=readonly_string(runtime,address);assert data is not None
        old='&UNK_'+f'{address:08x}';count=text.count(old);assert count==(2 if a=='269ad40' else 1)
        replacement=byte_literal(data);changes.append(dict(original=old,replacement=replacement,occurrences=count,kind='native show-deck prompt operand',machine_sites=[dict(address=a,bytes=b) for a,b in p['sites'].items()]))
        text=text.replace(old,replacement);prompts.append(dict(address=p['address'],bytes=(data+b'\0').hex(),sites=p['sites']))
    return text,changes,prompts


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('reveal_destroy_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions']) and {a for a,f in entries.items() if f['static']}=={'2677c50'}
    assert component['instance_layout']==dict(bytes=300,eot_toughness=76,targets=116,target_bytes=8,packed_player_counts=196)
    assert set(component['array_bounds'])=={'269ad40',CARD} and all(b['locations'] for b in component['array_bounds'].values())
    assert component['array_bounds'][CARD]['upper_bound_form']=='DW_FORM_ref4'
    assert component['array_bounds']['269ad40']['upper_bound_form']=='DW_FORM_exprloc'
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes())
    assert sha(runtime.data)==component['runtime_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert component['array_bounds']['269ad40']['locations']==[dict(expression='(DW_OP_breg0 (r0): -1)')]
    assert all('54; DW_OP_deref_size: 1;' in x['expression'] and 'DW_OP_lit1; DW_OP_minus' in x['expression'] for x in component['array_bounds'][CARD]['locations'])
    for unit in component['unit_layouts'].values():
        assert unit['test_definition_t']['bytes']==216 and unit['target_definition_t']['bytes']==84
        assert unit['test_definition_t']['members']['qty']['offset']==174
    for a,parameters in component['parameters'].items():
        initial=[p['location'] if isinstance(p['location'],str) else p['location'][0]['expression'] for p in parameters]
        assert initial==[f'(DW_OP_fbreg: {4*i})' for i in range({'2677c50':3,'269f880':5,'269ad40':4,CARD:3}[a])]
    assert all(any(v['name']==name and v['location']=='(DW_OP_fbreg: -236)' for v in component['locals'][CARD]) for name in ['td','test'])
    from unify_scalar_helpers import retained_source_body
    for name,digest in component['retained_bodies'].items():assert sha(retained_source_body(root,'functions/functions.c',name).encode())==digest
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name'])
        assert entry['reveal_destroy_evidence']==detail and sha(body.encode())==entry['reconstruction_body_sha256']
        native=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(native)==detail['machine_sha256']
        insns={f'{i.address:x}':i for i in decoder.disasm(native,int(a,16))}
        for change in detail['changes']:
            for site in change['machine_sites']:assert insns[site['address']].bytes.hex()==site['bytes']
        for prompt in detail['prompt_literals']:
            data=bytes.fromhex(prompt['bytes']);assert machine_bytes(int(prompt['address'],16),len(data))==data and data.endswith(b'\0')
            assert byte_literal(data[:-1]) in body
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes']
            assert any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands)
            assert re.search(r'\b'+site['name']+r'\b',body)
        directory='decompiled-aggregate-support' if component.get('planning') and a in HELPERS else 'decompiled-abi'
        raw=read(root/'.recovery'/directory/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        expected,changes,prompts=reconstruct(a,raw,runtime)
        assert changes==detail['changes'] and prompts==detail['prompt_literals']
        assert target_validation_input(strip_comments(raw),detail,a,machine_bytes)==expected
        assert 'stack0x' not in expected
        assert not re.search(r'field_31|\bshort\s*\*|\*\(uint16_t\s*\*',body)
        if a in HELPERS and not entry['static']:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
        if a=='269ad40':
            assert 'int deck_copy[count_deck(player)];' in body and 'remaining = scanned - removed;' in body
            assert '(uint32_t)(int32_t)(int16_t)report' in body and 'packed_result = UINT32_MAX;' in body
            assert body.count('show_deck_2022(')==2 and 'deck_ptr[t_player]' in body
        if a==CARD:
            assert 'int t_array[target_count];' in body and 'target_definition_2022_t td;' in body and 'test_definition_2022_t reveal_test;' in body
            assert '0x615a : 0x6161' in body and body.count('reveal_card_until_you_reveal_test_2022(')==2
    assert len(component['functions']['269f880']['callback_references'])==2
    assert all(r['target']=='2677c50' for r in component['functions']['269f880']['callback_references'])
    assert any(r['target']=='23ed060' for r in component['functions'][CARD]['callback_references'])


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'reveal_destroy_native_2022' not in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(data)
    proof=dict(debug_sha256=sha(data),**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{'functions/functions.c','cards/aether_revolt.c'}),array_bounds=array_bounds(debug),functions={},
        instance_layout=dict(bytes=300,eot_toughness=76,targets=116,target_bytes=8,packed_player_counts=196),retained_bodies={n:sha(function_body(read(root/'functions/functions.c'),n).encode()) for n in NAMES},
        scope='Native destruction tracker/constructor, reveal helper and Indomitable Creativity. Restore two count-sized VLAs and separate target/test lifetimes; represent halfword writes as modular word arithmetic, preserving independent byte sums, native flag combinations, controller/deck mismatches, return packing and original control flow. Retain all three existing support implementations under their legacy names.')
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());proof['runtime_sha256']=sha(runtime.data);get=lambda a,n:runtime.pe.get_data(a-0x2000000,n);decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES[meta[a]['name']],source=meta[a]['source'],static=a=='2677c50') for a in HELPERS})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global'];entries=[];sources={}
    for a in ADDRESSES:
        row=meta[a];directory='decompiled-aggregate-support' if a in HELPERS else 'decompiled-abi';raw=read(cache/directory/f'{a}.c');text,changes,prompts=reconstruct(a,raw,runtime);text,_=rename(text,NAMES);body=cleaned(text,overlap)
        assert not UNRESOLVED.search(body) and not re.search(r'stack0x|\._\d|_conflict',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==([3] if a=='2677c50' else [2]),(a,storage)
        if a=='2677c50':body='static '+body
        native=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,prompt_literals=prompts,canonical_calls=[],callback_references=[])
        for ins in decoder.disasm(native,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==row['source']
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name'])
                (detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;name=NAMES[row['name']] if a in HELPERS else row['name']
        entry=dict(address=a,name=name,source=row['source'],line=row['line'],static=a=='2677c50',prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],reveal_destroy_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode()))
        entries.append(entry);source=sources.get(row['source'],read(root/row['source']));sources[row['source']]=source+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {row["source"]}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in HELPERS and not e['static'])+'\n#endif\n\n'+header[at:]
    # Check the helpers and consumer in one strict candidate unit before writing.
    sample='#include "manalink.h"\nchar *not_indestructible(int,int,int);\n'+'\n'.join(e['prototype'] for e in entries)+'\n'+'\n'.join(sources[e['source']].split(f'/* ManalinkEh 2022: 0x{e["address"]};')[-1].split('*/',1)[1].split('\n#endif',1)[0] for e in entries)
    compile_sources(root,cache/'reveal-destroy-check',{'functions/functions.c':sample},header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(HELPERS)&{f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')}
        for row in csv.DictReader((cache/'decompiled-aggregate-support/export.tsv').open(),delimiter='\t'):
            if row['address'] not in HELPERS:continue
            assert row['address'] not in rows;rows[row['address']]=row;shutil.copyfile(cache/'decompiled-aggregate-support'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=4;m['card_handler_functions']+=1;m['helper_functions']+=3;m['private_functions']+=1;m['private_helpers']+=1;m['shared_helper_interfaces']+=2
        for e in entries:m['source_files'][e['source']]+=1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['reveal_destroy_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'reveal-destroy-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=1,new_helpers=3,private_helpers=1,public_helpers=2)))


if __name__=='__main__':main()
