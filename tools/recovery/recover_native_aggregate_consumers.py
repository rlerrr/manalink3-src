"""Recover native area damage and three verified target/test storage consumers."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image,value
from export_abi import type_of
from promote_recovery import clean,strip_comments,aggregate_stack_aliases,UNRESOLVED
from recover_stack_storage import rewrite
from recover_combat_mask_cards import cleaned,ENGINE_RAW,ENGINE_NAME
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_test_consumers import read
from recover_spell_copy import compile_sources
from recover_literals import recover_strings
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input

HELPER='26677f0'
CARDS=['228f070','25adb40','2604fb0']
ADDRESSES=[HELPER,*CARDS]
HEADER_LABEL='/* October 2022 area damage with native creature/planeswalker tests. */'
ENGINE_SITES=[dict(address='228f19b',bytes='b8e04b4300'),dict(address='228f1a0',bytes='ffd0')]


def reconstruct(a,raw,prepared):
    text=strip_comments(raw);changes=[];storage=[]
    if a in CARDS:
        intermediate=clean(text);aliases=aggregate_stack_aliases(intermediate)
        assert aliases==[tuple(x) for x in prepared['expected_aliases']]
        rewritten,storage=rewrite(intermediate,aliases,prepared['groups'],prepared['target_sizes'])
        if a=='228f070':
            assert rewritten.count(ENGINE_RAW)==1;rewritten=rewritten.replace(ENGINE_RAW,ENGINE_NAME)
        changes=[dict(original=text,replacement=rewritten,occurrences=1,kind='DWARF-proven shared target/test storage with native EXE boundary',machine_sites=ENGINE_SITES if a=='228f070' else [])]
        text=rewritten
    return text,changes,storage


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('native_aggregate_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions'])
    assert component['helper_test_layout']==dict(bytes=216,not_me_offset=170)
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for unit in component['unit_layouts'].values():assert unit['test_definition_t']['bytes']==216 and unit['target_definition_t']['bytes']==84
    for a,entry in entries.items():
        detail=component['functions'][a];body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name'])
        assert entry['native_aggregate_evidence']==detail and sha(body.encode())==entry['reconstruction_body_sha256'] and not entry['static']
        directory='decompiled-aggregate-support' if component.get('planning') and a==HELPER else 'decompiled-abi'
        raw=read(root/'.recovery'/directory/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes,storage=reconstruct(a,raw,component['prepared_storage'].get(a))
        assert detail['changes']==changes and detail['storage']==storage
        assert target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        native=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(native)==detail['machine_sha256']
        insns={f'{i.address:x}':i for i in decoder.disasm(native,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes']
            assert any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands)
            assert re.search(r'\b'+site['name']+r'\b',body)
        if a in CARDS:
            assert len(storage)==1 and storage[0]['target_extent']==216 and storage[0]['frame_offset']==-236
            assert 'target_definition_2022_t target;' in body and 'test_definition_2022_t test;' in body and 'uint8_t bytes[216];' in body
            for name in ['td',('test' if a=='228f070' else 'this_test')]:
                assert any(v['name']==name and v['location']=='(DW_OP_fbreg: -236)' for v in component['locals'][a])
            assert not aggregate_stack_aliases(body)
        else:
            assert entry['name']=='new_damage_all_2022' and 'test_definition_2022_t *this_test' in entry['prototype']
            assert [p['location'] for p in component['parameters'][a]]==[f'(DW_OP_fbreg: {i*4})' for i in range(6)]
            assert 'mode & 0x20U' in body and 'this_test->not_me' in body and 'if (dmg < 1)' in body
            assert sha(function_body(sources[entry['source']].replace('\r\n','\n'),'new_damage_all').encode())==component['retained_damage_body_sha256']
            assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
    arahbo=function_body(sources[entries['228f070']['source']],'card_arahbo_roar_of_the_world')
    assert ENGINE_NAME+'(player)' in arahbo and 'store_attackers_2022(' in arahbo and 'resolve_declare_attackers_trigger_2022(' in arahbo
    for site in ENGINE_SITES:assert machine_bytes(int(site['address'],16),len(bytes.fromhex(site['bytes']))).hex()==site['bytes']
    plague_refs=component['functions']['25adb40']['callback_references'];assert len(plague_refs)==1
    callback=next(f for f in manifest['accepted_functions'] if f['address']=='25a2630')
    assert callback['static'] and callback['source']==entries['25adb40']['source'] and plague_refs[0]['target']=='25a2630'
    noxious=function_body(sources[entries['2604fb0']['source']],'card_noxious_field')
    assert 'new_damage_all_2022(' in noxious and 'aura_literal_prompt(' in noxious


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'native_aggregate_consumers_2022' not in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(data)
    prepared=json.loads((root/'docs/recovery-evidence/stack-storage.json').read_text())['candidates']
    proof=dict(debug_sha256=sha(data),**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{meta[a]['source'] for a in CARDS}),functions={},prepared_storage={a:prepared[a] for a in CARDS},retained_damage_body_sha256=sha(function_body(read(root/meta[HELPER]['source']),'new_damage_all').encode()),
        scope='Native six-argument area damage plus three target/test consumers with separately checked 216-byte shared union storage. Preserve APNAP rereads, filtering/planeswalker branches, negative damage-card handling and source-aware damage fields. Retain the existing area-damage implementation under its legacy name.')
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!=meta[HELPER]['source']:continue
        for d in cu.get_top_DIE().iter_children():
            if d.tag=='DW_TAG_typedef' and value(d,'DW_AT_name')=='test_definition_t':
                t=type_of(d);proof['helper_test_layout']=dict(bytes=value(t,'DW_AT_byte_size'),not_me_offset=next(value(x,'DW_AT_data_member_location') for x in t.iter_children() if value(x,'DW_AT_name')=='not_me'))
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;targets={int(f['address'],16):f for f in m['accepted_functions']};targets[int(HELPER,16)]=dict(name='new_damage_all_2022',source=meta[HELPER]['source'],static=False)
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    entries=[];sources={}
    for a in ADDRESSES:
        row=meta[a];directory='decompiled-aggregate-support' if a==HELPER else 'decompiled-abi';raw=read(cache/directory/f'{a}.c')
        if a in CARDS:assert prepared[a]['raw_sha256']==sha(raw.encode())
        text,changes,storage=reconstruct(a,raw,prepared.get(a));text,literals=recover_strings(text,runtime,{});body=cleaned(text,overlap)
        assert not UNRESOLVED.search(body) and not re.search(r'stack0x|\._\d|_conflict',body),a
        native=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,storage=storage,canonical_calls=[],callback_references=[])
        for ins in decoder.disasm(native,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name'])
                assert not target['static'] or target['source']==row['source']
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name'])
                (detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;name='new_damage_all_2022' if a==HELPER else row['name']
        entry=dict(address=a,name=name,source=row['source'],line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,native_aggregate_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode()))
        entries.append(entry)
        retained=[f['prototype'] for f in m['accepted_functions'] if f['source']==row['source'] and f['static'] and re.search(r'\b'+f['name']+r'\b',body)]
        # The one new public callee is declared while checking its consumers.
        extra=next((e['prototype'] for e in entries if e['address']==HELPER),'')
        compile_sources(root,cache/'native-aggregate-consumers-check',{row['source']:'#include "manalink.h"\n'+extra+'\n'+'\n'.join(retained)+'\n'+body})
        source=sources.get(row['source'],read(root/row['source']));sources[row['source']]=source+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {row["source"]}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+entries[0]['prototype']+'\n#endif\n\n'+header[at:]
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert HELPER not in {f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};row=next(r for r in csv.DictReader((cache/'decompiled-aggregate-support/export.tsv').open(),delimiter='\t') if r['address']==HELPER)
        assert HELPER not in rows;rows[HELPER]=row;shutil.copyfile(cache/'decompiled-aggregate-support'/f'{HELPER}.c',cache/'decompiled-abi'/f'{HELPER}.c')
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=4;m['card_handler_functions']+=3;m['helper_functions']+=1;m['shared_helper_interfaces']+=1
        for entry in entries:m['source_files'][entry['source']]+=1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['native_aggregate_consumers_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'native-aggregate-consumers-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=3,new_helpers=1)))


if __name__=='__main__':main()
