"""Recover native snow predicates, Zombie Musher, and Hallowed Ground."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read,rename
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input

HELPERS=['23ee460','23eb6c0']
ADDRESSES=HELPERS+['216b6c0','23faff0']
SOURCE='cards/ice_age.c'
NAMES={'control_a_snow_land':'control_a_snow_land_2022'}
HEADER_LABEL='/* October 2022 shared snow-land predicate. */'
RETURN='return (char *)((-(uint)(iVar1 == 0) ^ 0xffffffff) & 0x73964c);'
ERROR_RETURN='return iVar1 ? (char *)EXE_STR(0x73964c) : NULL;'
ERROR_SITE=dict(address='23eb6e9',bytes='254c967300')


def reconstruct(a,raw):
    text=strip_comments(raw);changes=[]
    if a=='23eb6c0':
        assert text.count(RETURN)==1
        changes.append(dict(original=RETURN,replacement=ERROR_RETURN,occurrences=1,
            kind='restore conditional native EXE subtype-error pointer',machine_sites=[ERROR_SITE]))
        text=text.replace(RETURN,ERROR_RETURN)
    return text,changes


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('snow_consumer_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions'])
    assert component['runtime_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert sha(function_body(sources[SOURCE].replace('\r\n','\n'),'control_a_snow_land').encode())==component['retained_legacy_body_sha256']
    assert component['unit_layouts'][SOURCE]['target_definition_t']['bytes']==84
    assert any(v['name']=='td' and v['type']=='target_definition_t' and v['location']=='(DW_OP_fbreg: -104)' for v in component['locals']['23faff0'])
    assert len(component['parameters']['23ee460'])==2 and len(component['parameters']['23eb6c0'])==5
    for a,params in component['parameters'].items():
        assert [p['location'] if isinstance(p['location'],str) else p['location'][0]['expression'] for p in params]==[f'(DW_OP_fbreg: {i*4})' for i in range(len(params))]
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes())
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global']
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['snow_consumer_evidence']==detail
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-snow' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,{});assert literals==entry['recovered_literals'];text,_=rename(text,NAMES)
        assert function_body(cleaned(text,overlap),entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256']
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        assert detail['native_coff_storage']==(3 if a=='23eb6c0' else 2)
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes']
            assert any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands)
            assert re.search(r'\b'+site['name']+r'\b',body)
        if a=='23eb6c0':
            assert ERROR_RETURN.replace('iVar1','result') in body and machine_bytes(0x23eb6e1,14).hex()=='83f80119c083f0ff254c967300c3'
            assert component['engine_error_pointer']==dict(address='73964c',binding='EXE_STR(0x73964c)',operand=ERROR_SITE,scope='Native runtime engine subtype-error buffer; no assumed on-disk literal content.')
        elif a=='23ee460':assert 'must_be_untapped != 1' in body and 'SUBTYPE_SNOW' in body
        elif a=='23faff0':assert 'target_definition_2022_t td;' in body and 'target_definition_t td;' not in body
    assert len(component['functions']['23faff0']['callback_references'])==2
    assert all(s['target']=='23eb6c0' for s in component['functions']['23faff0']['callback_references'])
    assert any(s['target']=='23ee460' for s in component['functions']['216b6c0']['canonical_calls'])
    assert entries['23ee460']['prototype'] in read(root/'manalink.h') or component.get('planning')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'snow_consumers_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes())
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{SOURCE,'cards/coldsnap.c'}),functions={},
        retained_legacy_body_sha256=sha(function_body(read(root/SOURCE),'control_a_snow_land').encode()),
        engine_error_pointer=dict(address='73964c',binding='EXE_STR(0x73964c)',operand=ERROR_SITE,scope='Native runtime engine subtype-error buffer; no assumed on-disk literal content.'),
        scope='Recover the shared native snow-land predicate and private nonsnow validator, plus Zombie Musher and Hallowed Ground. Preserve the legacy static snow helper; bind the native public predicate across compilation units. Restore the callback\'s conditional engine subtype-error pointer without inventing a literal. Preserve native block legality, regeneration mana-cost path, 84-byte target definition and bounce/activation ordering.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES.get(meta[a]['name'],meta[a]['name']),source=meta[a]['source'],static=a=='23eb6c0') for a in ADDRESSES})
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=NAMES.get(row['name'],row['name']);raw=read(cache/'decompiled-snow'/f'{a}.c');text,changes=reconstruct(a,raw);text,literals=recover_strings(text,runtime,{});text,_=rename(text,NAMES);body=cleaned(text,overlap)
        if a=='23eb6c0':body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==([3] if a=='23eb6c0' else [2]),(a,storage)
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail
        entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a=='23eb6c0',prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,snow_consumer_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
        sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+entries[0]['prototype']+'\n#endif\n\n'+header[at:]
    compile_sources(root,cache/'snow-consumers-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-snow/export.tsv').open(),delimiter='\t'):
            rows[row['address']]=row;shutil.copyfile(cache/'decompiled-snow'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=4;m['card_handler_functions']+=2;m['helper_functions']+=2;m['private_functions']+=1;m['private_helpers']+=1;m['shared_helper_interfaces']+=1
        for e in entries:m['source_files'][e['source']]+=1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['snow_consumers_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'snow-consumers-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=2,new_helpers=2,private_helpers=1)))


if __name__=='__main__':main()
