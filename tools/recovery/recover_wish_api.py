"""Recover filter-aware Wish helpers with explicitly out-of-scope dialog stubs."""
import argparse,csv,json,re,shutil,subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image
from promote_recovery import clean,strip_comments,UNRESOLVED,WISH_API_NAMES
from recover_test_consumers import read
from recover_spell_copy import compile_sources,format_c
from recover_temporary_exile import save_json,sha
from recover_phase_prompt_cards import debug_evidence
from audit_source import function_body

HELPERS={'268ecf0':'decompiled-wish-support','20590c0':'decompiled-wish'}
STUBS={'26ef820':'choose_a_card_test_ct','26ef860':'choose_a_card_test_ct_allow_cancel'}
STUB_SOURCE='functions/choose_a_card.cpp'
HEADER_LABEL='/* October 2022 filter-aware Wish APIs and out-of-scope dialog boundaries. */'
STUB_PROTO='int {name}(char *prompt, test_definition_2022_t *constraints, color_test_t colors, type_t types);'
STUB_BODY='''extern "C" int {name}(char *prompt, test_definition_2022_t *constraints,
                       color_test_t colors, type_t types)
{{
  (void)prompt;
  (void)constraints;
  (void)colors;
  (void)types;
  return -1;
}}'''
STUB_REASON='User-directed scope exclusion: built-in Wish/card-picker dialog UI and its dialog-specific internals. Entry stubs return no selection (-1); these are placeholders, not recovered native implementations.'
BOUNDARIES={'268ed5c':('e8ff0a0600','26ef860'),'268ed83':('e8980a0600','26ef820')}
CARDS=['239a660','2190ed0','25d04e0','25c7170']


def audit_cards(component,manifest,sources,machine_bytes):
    from bind_delayed_callback_callers import audit_component
    audit_component(component,manifest,sources,machine_bytes,expected={a:'20590c0' for a in CARDS},targets={'20590c0':'generic_wish_effect_2022'})
    entries={f['address']:f for f in manifest['accepted_functions']}
    for a in CARDS:
        entry=entries[a];body=function_body(sources[entry['source']],entry['name'])
        if entry.get('wish_interface_2022')!={'calls':{'generic_wish_effect':'generic_wish_effect_2022'},'dialog_selection_stubbed':True}:
            raise ValueError('Wish card stub-limitation/native interface mismatch')
        if not re.search(r'\bgeneric_wish_effect_2022\(player, card, &test,',body) or 'test_definition_2022_t test;' not in body:
            raise ValueError('Wish card native filter argument missing')


def bind_cards(root,apply):
    cache=root/'.recovery';m=json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text());ip=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(ip.read_text())
    assert 'wish_card_callers_2022' not in interfaces
    entries={f['address']:f for f in m['accepted_functions']};meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    proof=dict(functions={},scope='Four native Wish calls bind 216-byte source filters to the recovered helper. Built-in interactive selection remains stubbed and out of scope.')
    sources={entries[a]['source']:read(root/entries[a]['source']) for a in CARDS}
    for a in CARDS:
        native=runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16));sites=[]
        for ins in decoder.disasm(native,int(a,16)):
            if ins.mnemonic in {'call','jmp'} and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm==0x20590c0:
                sites.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target='20590c0',name='generic_wish_effect_2022'))
        entry=entries[a];proof['functions'][a]=dict(machine_bytes=len(native),machine_sha256=sha(native),sites=sites,body_sha256=sha(function_body(sources[entry['source']],entry['name']).encode()))
    audit_cards(proof,m,sources,lambda a,n:runtime.pe.get_data(a-0x2000000,n))
    if apply:interfaces['wish_card_callers_2022']=proof;save_json(ip,interfaces)
    print(json.dumps(dict(applied=apply,bound_card_functions=4,native_wish_call_sites=4)))


def audit_stubs(component,manifest,root):
    def require(ok,message):
        if not ok:raise ValueError(message)
    source=read(root/STUB_SOURCE)
    require(component['source']==STUB_SOURCE and component['status']=='out-of-scope-stubs' and component['return_value']==-1,'Wish dialog stub scope mismatch')
    require(component['scope_reason']==STUB_REASON and component['counted_as_recovered_implementations'] is False,'Wish dialog stub accounting mismatch')
    require(set(component['functions'])==set(STUBS),'Wish dialog stub membership mismatch')
    require(not set(STUBS)&{f['address'] for f in manifest['accepted_functions']},'Wish dialog stubs were counted as recovered native functions')
    for a,name in STUBS.items():
        detail=component['functions'][a];body=function_body(source,name)
        require(detail['name']==name and sha(body.encode())==detail['stub_body_sha256'],'Wish dialog stub source hash mismatch')
        require(body==function_body(format_c(STUB_BODY.format(name=name)),name),'Wish dialog stub must only ignore arguments and return no selection')
        require([p['name'] for p in component['parameters'][a]]==['prompt','constraints','colors','types'],'Wish dialog boundary parameter order mismatch')
    require(manifest.get('out_of_scope_source_units',{}).get(STUB_SOURCE)==STUB_REASON,'Wish dialog source-unit exclusion missing')
    require(manifest.get('source_stubs')==[dict(source=STUB_SOURCE,name=name,native_address=a,return_value=-1) for a,name in STUBS.items()],
        'Wish dialog separate stub ledger mismatch')


def audit_helpers(component,manifest,sources,machine_bytes,root):
    def require(ok,message):
        if not ok:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('wish_helper_evidence')}
    require(set(entries)==set(HELPERS)==set(component['functions']),'Wish helper membership mismatch')
    require(component['dialog_selection_stubbed'] is True,'Wish helper stub limitation missing')
    for a,entry in entries.items():
        require(not entry['static'] and entry['wish_helper_evidence']==component['functions'][a],'Wish helper evidence/linkage mismatch')
        body=function_body(sources[entry['source']],entry['name'])
        require('test_definition_2022_t *test' in body and entry['name'] in WISH_API_NAMES.values(),'Wish native filter interface missing')
    picker=entries['268ecf0']['wish_helper_evidence']
    require([(x['original'],x['replacement'],x['occurrences']) for x in picker['changes']]==[
        ('color_test_t_conflict','color_test_t',1),('type_t_conflict','type_t',1),('(test_definition_t_conflict *)test','test',2)],'Wish UI boundary type projection mismatch')
    require([p['name'] for p in component['parameters']['268ecf0']]==['player','mode','test'] and
        [p['name'] for p in component['parameters']['20590c0']]==['player','card','test','ai_card'],'Wish native physical parameter roles mismatch')
    for a,(data,target) in BOUNDARIES.items():
        require(machine_bytes(int(a,16),5).hex()==data,'Wish dialog native boundary instruction mismatch')
        require(any(c['address']==a and c['target']==target and c['name']==STUBS[target] for c in picker['canonical_calls']),'Wish dialog boundary binding missing')
    for support in component['retained_helpers']:
        from unify_scalar_helpers import retained_source_body
        require(sha(retained_source_body(root,support['source'],support['name']).encode())==support['body_sha256'],'Wish retained source helper changed')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--bind-cards',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    if args.bind_cards:bind_cards(root,args.apply);return
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'wish_helpers_2022' not in interfaces and 'wish_dialog_stubs_2022' not in interfaces
    assert not (root/STUB_SOURCE).exists() and not set(HELPERS)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes())
    proof=dict(debug_sha256=sha(debug_data),**debug_evidence(debug,meta,list(HELPERS)),functions={},retained_helpers=[],dialog_selection_stubbed=True,
        scope='Native filter-aware Wish/card-list logic and AI fallback are recovered. The built-in C++ chooser dialog is explicitly stubbed at its four-argument public boundaries by user request; do not claim interactive selection as recovered.',
        integration_pending='Dialog integration, native linking and gameplay verification remain pending.')
    stubs=dict(debug_sha256=sha(debug_data),**debug_evidence(debug,meta,list(STUBS)),source=STUB_SOURCE,status='out-of-scope-stubs',return_value=-1,
        scope_reason=STUB_REASON,counted_as_recovered_implementations=False,functions={})
    stub_source='#include "manalink.h"\n\n#ifndef SHANDALAR\n/* TODO(recovery-ui): Built-in Wish/card-picker dialogs are OUT OF SCOPE for\n * decompilation for now, by user request. These boundaries return no selection.\n * They are stubs, not recovered native implementations. Restore the UI before\n * relying on interactive Wish/card-list selection. */\n\n'
    for a,name in STUBS.items():
        body=format_c(STUB_BODY.format(name=name));stub_source+=f'/* Stub for native dialog entry 0x{a}. */\n'+body+'\n\n'
        native=runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16))
        stubs['functions'][a]=dict(name=name,stub_body_sha256=sha(function_body(body,name).encode()),native_boundary_bytes=len(native),native_boundary_sha256=sha(native))
    stub_source+='#endif\n'
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']};targets.update({int(a,16):WISH_API_NAMES[meta[a]['name']] for a in HELPERS});targets.update({int(a,16):n for a,n in STUBS.items()})
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    sources={s:read(root/s) for s in ['functions/functions.c','cards/judgement.c','functions/deck.c']};entries=[]
    for source,name in [('functions/functions.c','new_card_from_list'),('cards/judgement.c','generic_wish_effect'),('functions/deck.c','get_internal_card_id_from_csv_id')]:
        proof['retained_helpers'].append(dict(source=source,name=name,body_sha256=sha(function_body(sources[source],name).encode())))
    for a,directory in HELPERS.items():
        row=meta[a];raw=read(cache/directory/f'{a}.c');text=strip_comments(raw);changes=[]
        if a=='268ecf0':
            for original,replacement in [('color_test_t_conflict','color_test_t'),('type_t_conflict','type_t'),('(test_definition_t_conflict *)test','test')]:
                count=text.count(original);assert count==(2 if original.startswith('(') else 1);text=text.replace(original,replacement)
                changes.append(dict(original=original,replacement=replacement,occurrences=count,kind='representation-preserving typed stub boundary; constraints are not dereferenced by the UI stub',machine_sites=[dict(address=s,bytes=data) for s,(data,t) in BOUNDARIES.items()]))
        body=format_c(clean(text,overlap,use_wish_api_2022=True,use_test_api_2022=True,use_extension_flags_2022=True,use_keyword_accessors_2022=True,use_life_loss_api_2022=True))
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x|_conflict',body)
        native=runtime.pe.get_data(int(a,16)-0x2000000,int(row['high_pc'],16)-int(a,16));calls=[]
        for ins in decoder.disasm(native,int(a,16)):
            if ins.mnemonic in {'call','jmp'} and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm in targets:
                name=targets[ins.operands[0].imm];assert re.search(r'\b'+name+r'\s*\(',body),(a,name)
                calls.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{ins.operands[0].imm:x}',name=name))
        symbols=[s for n,o,sec,k,s in debug.functions() if debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+o==int(a,16)];assert symbols==[2]
        detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,canonical_calls=calls);proof['functions'][a]=detail;name=WISH_API_NAMES[row['name']]
        entries.append(dict(address=a,name=name,source=row['source'],line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),
            recovered_literals=[],wish_helper_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode()),dialog_selection_stubbed=True))
        sources[row['source']]+=f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {row["source"]}:{row["line"]}.\n * Built-in chooser UI is stubbed; see functions/choose_a_card.cpp. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(STUB_PROTO.format(name=name) for name in STUBS.values())+'\n'+'\n'.join(e['prototype'] for e in entries)+'\n#endif\n\n'+header[at:]
    scratch=cache/'wish-api-check';compile_sources(root,scratch,{s:sources[s] for s in ['functions/functions.c','cards/judgement.c']},header)
    target=scratch/'choose_a_card.cpp';target.write_bytes(stub_source.encode('latin1'));result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu++17','-fsyntax-only','-Werror=return-type','-I',str(scratch),'-I',str(root),str(target)],capture_output=True,text=True)
    target.with_suffix('.log').write_text(result.stderr)
    if result.returncode:print(result.stderr[-3000:])
    result.check_returncode();audit_helpers(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,lambda a,n:runtime.pe.get_data(a-0x2000000,n),root)
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not set(HELPERS)&{c['address'] for c in citations} and not m.get('source_stubs')
        for source in ['functions/functions.c','cards/judgement.c']:(root/source).write_bytes(sources[source].replace('\n','\r\n').encode('latin1'))
        (root/STUB_SOURCE).write_bytes(stub_source.replace('\n','\r\n').encode('latin1'));(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')}
        for a,directory in HELPERS.items():
            focused={r['address']:r for r in csv.DictReader((cache/directory/'export.tsv').open(),delimiter='\t')};rows[a]=focused[a];shutil.copyfile(cache/directory/f'{a}.c',cache/'decompiled-abi'/f'{a}.c')
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        m['accepted_functions']+=entries;m['accepted']+=2;m['helper_functions']+=2;m['shared_helper_interfaces']+=2
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['out_of_scope_source_units']={**m.get('out_of_scope_source_units',{}),STUB_SOURCE:STUB_REASON}
        m['source_stubs']=[dict(source=STUB_SOURCE,name=name,native_address=a,return_value=-1) for a,name in STUBS.items()]
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()})
        save_json(mp,m);interfaces['wish_helpers_2022']=proof;interfaces['wish_dialog_stubs_2022']=stubs;save_json(ip,interfaces);audit_stubs(stubs,m,root)
    save_json(cache/'wish-api-plan.json',dict(helpers=proof,stubs=stubs));print(json.dumps(dict(applied=args.apply,new_helpers=2,dialog_stubs=2,stubs_counted_as_recovered=False)))


if __name__=='__main__':main()
