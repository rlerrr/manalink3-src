"""Recover native library reveal/selection logic around the existing EXE viewer."""
import argparse,csv,json,re,shutil,struct
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from elftools.dwarf.locationlists import LocationParser,BaseAddressEntry
from elftools.dwarf.descriptions import describe_DWARF_expr
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import clean,strip_comments,UNRESOLVED,LIBRARY_REVEAL_API_NAMES
from recover_test_consumers import read
from recover_literals import readonly_string,byte_literal
from recover_phase_prompt_cards import debug_evidence
from recover_spell_copy import compile_sources,format_c
from recover_temporary_exile import save_json,sha
from audit_source import function_body
from library_reveal_body import STORAGE,WORKER,TYPE_FRONTEND

HELPERS='26526c0 2654d60 2655580 26555e0 2655740 26557a0'.split()
SOURCE='functions/tutors.c'
HEADER_LABEL='/* October 2022 library reveal and selection APIs. */'
CALLBACK='typedef int (*library_reveal_callback_2022_t)(int player, int card);'
LITERALS={'LITERAL_TITLE':0x275d10c,'LITERAL_SOURCE':0x275d119,'LITERAL_QTY':0x275d12c,
          'LITERAL_CHOSEN':0x275d160,'LITERAL_REST':0x275d188,'LITERAL_BATCH':0x275d1ac}
CRITICAL={
 '2654d98':'668b82ae000000','2654dbe':'66c780ae0000000100','2654dc7':'6689b0c2000000',
 '2654dce':'8b8520ffffff','2654dd4':'c1e002','2654dd7':'e8f4730a00','2654ddc':'29c4',
 '2654dde':'8d442420','2654de2':'898514ffffff','2654f65':'668b3cc534707e00',
 '26551cc':'c744240800000100','2655212':'ff5534','2655229':'83c914',
 '2655513':'8b048500bb7300','265551a':'8b4008','2655525':'8944240c',
 '2655529':'c744240464000000','2655548':'c7442414b0757300',
 '2655567':'c7056441770200000000','265578f':'c744242c00000000',
 '26526fa':'c744240464000000','265277b':'c744240464000000',
 '2652716':'c7442414a8f07800','2652739':'c7056441770200000000'}
RANGES=[('2654f78','2654fb7'),('26550f0','2655124'),('2655175','265525e'),
        ('26553e6','2655418'),('26554b7','26554f3'),('26554fb','265557e'),
        ('2655693','26556e3'),('26556e3','265572f')]
QUIRKS=dict(callback_frontend_passes_null=True,reveal_buffer_bytes=50,reveal_format_bound=100,
            chosen_hand_owner='src_player',rest_tplayer_hand_owner='1 - t_player',
            grave_batch_returns_count=True,single_regular_play_returns_card=True,
            single_attacking_play_returns_count=True,chosen_id_load_unsigned_bytes=2)
CARDS={'23dfc10':('26557a0',2),'220c4d0':('2655580',1),'230f420':('2655740',1)}


def audit_cards(component,manifest,sources,machine_bytes):
    entries={f['address']:f for f in manifest['accepted_functions']}
    assert set(component['functions'])==set(CARDS)
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,(target,count) in CARDS.items():
        detail=component['functions'][a];entry=entries[a];body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name'])
        assert sha(body.encode())==detail['body_sha256']
        native=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(native)==detail['machine_sha256']
        instructions={f'{i.address:x}':i for i in decoder.disasm(native,int(a,16))}
        assert len(detail['sites'])==count and 'test_definition_2022_t test;' in body
        assert entry['library_reveal_interface_2022']==dict(calls={entries[target]['name'][:-5]:entries[target]['name']})
        for site in detail['sites']:
            ins=instructions[site['address']]
            assert site['name']==entries[target]['name'] and site['target']==target and ins.mnemonic=='call' and ins.bytes.hex()==site['bytes'] and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm==int(target,16)
            assert re.search(r'\b'+site['name']+r'\s*\(',body)
    callback=component['turntimber_callback'];target=entries['2306bc0'];entry=entries['230f420']
    assert callback['target']=='2306bc0' and callback['name']==target['name'] and callback['forwarded_by_frontend'] is False
    body=function_body(sources[entry['source']],entry['name']);assert callback['name'] in body
    for site in callback['sites']:
        data=bytes.fromhex(site['bytes']);assert machine_bytes(int(site['address'],16),len(data))==data
        ins=next(decoder.disasm(data,int(site['address'],16)))
        assert ins.mnemonic=='mov' and ins.operands[0].type==X86_OP_MEM and ins.operands[0].mem.disp==40 and ins.operands[1].type==X86_OP_IMM and ins.operands[1].imm==0x2306bc0
    assert len(callback['sites'])==1


def bind_cards(root,apply):
    cache=root/'.recovery';manifest=json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text());ip=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(ip.read_text())
    assert 'library_reveal_cards_2022' not in interfaces
    entries={f['address']:f for f in manifest['accepted_functions']};meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    sources={entries[a]['source']:read(root/entries[a]['source']) for a in CARDS}
    proof=dict(functions={},scope='Four direct library-selection calls in three new card handlers, plus Turntimber\'s callback pointer operand. The native callback frontend discards its fn argument; preserve this behavior until a separate intentional gameplay fix.')
    for a,(target,count) in CARDS.items():
        native=runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16));instructions=list(decoder.disasm(native,int(a,16)))
        sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=target,name=entries[target]['name']) for i in instructions if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==int(target,16)]
        entry=entries[a];proof['functions'][a]=dict(machine_bytes=len(native),machine_sha256=sha(native),sites=sites,body_sha256=sha(function_body(sources[entry['source']],entry['name']).encode()))
        if a=='230f420':proof['turntimber_callback']=dict(target='2306bc0',name=entries['2306bc0']['name'],forwarded_by_frontend=False,
            sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex()) for i in instructions if any(o.type==X86_OP_IMM and o.imm==0x2306bc0 for o in i.operands)])
    audit_cards(proof,manifest,sources,lambda a,n:runtime.pe.get_data(a-0x2000000,n))
    if apply:interfaces['library_reveal_cards_2022']=proof;save_json(ip,interfaces)
    print(json.dumps(dict(applied=apply,bound_card_functions=3,native_library_calls=4,native_callback_pointer_stores=1)))


def cleaned(text,overlap):
    return format_c(clean(text,overlap,use_test_api_2022=True,use_global_tutor_api_2022=True,
        use_library_reveal_api_2022=True,use_extension_flags_2022=True,use_keyword_accessors_2022=True))


def library_debug(debug,meta):
    result=debug_evidence(debug,meta,HELPERS)
    chosen=next(v for v in result['locals']['2654d60'] if v['name']=='chosen_iids')
    # type_name() cannot resolve a reference-valued array bound. Retain its
    # untrusted rendering, then describe the array using the decoded bound DIE.
    chosen['unresolved_decoder_type']=chosen['type'];chosen['type']='int[num_to_pick]'
    chosen['bound_evidence']='storage.vla_upper_bound; DW_FORM_ref4 resolves to an artificial variable, not a constant element count'
    return result


def debug_storage(debug):
    dwarf=debug.dwarf();parser=LocationParser(dwarf.location_lists());result={}
    for cu in dwarf.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!=SOURCE:continue
        def walk(die):
            for child in die.iter_children():
                name=inherited_value(child,'DW_AT_name')
                if child.tag=='DW_TAG_formal_parameter' and name=='fn':
                    sub=type_of(type_of(child));assert sub.tag=='DW_TAG_subroutine_type'
                    result['callback']=dict(result=type_name(type_of(sub)),arguments=[type_name(type_of(p)) for p in sub.iter_children()])
                if child.tag=='DW_TAG_variable' and name=='chosen_iids':
                    array=type_of(child);subrange=next(array.iter_children());attr=subrange.attributes['DW_AT_upper_bound']
                    assert attr.form=='DW_FORM_ref4'
                    bound=subrange.get_DIE_from_attribute('DW_AT_upper_bound')
                    locations=parser.parse_from_attribute(bound.attributes['DW_AT_location'],cu['version'],die=bound)
                    result['vla_upper_bound']=[dict(base=hex(x.base_address)) if isinstance(x,BaseAddressEntry) else
                        dict(start=hex(x.begin_offset),end=hex(x.end_offset),expression=describe_DWARF_expr(x.loc_expr,dwarf.structs),bytes=list(x.loc_expr)) for x in locations]
                if child.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:walk(child)
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_typedef' and value(die,'DW_AT_name')=='card_ptr_t':
                record=type_of(die);result['card_ptr']=dict(bytes=value(record,'DW_AT_byte_size'),
                    members={value(m,'DW_AT_name'):dict(type=type_name(type_of(m)),offset=value(m,'DW_AT_data_member_location')) for m in record.iter_children() if m.tag=='DW_TAG_member' and value(m,'DW_AT_name') in {'id','name','full_name'}})
            if value(die,'DW_AT_low_pc')==0x2654d60:walk(die)
    assert result['callback']==dict(result='int',arguments=['int','int'])
    assert [x['bytes'] for x in result['vla_upper_bound']]==[[145,152,126,6,49,28,159],[113,127,159]]
    return result


def audit_component(component,manifest,sources,machine_bytes,root):
    def require(ok,message):
        if not ok:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('library_reveal_evidence')}
    require(set(entries)==set(HELPERS)==set(component['functions']),'Library reveal helper membership mismatch')
    require(component['native_quirks']==QUIRKS and component['viewer_boundary']==dict(name='show_deck_exe',address='479360',arguments=6,status='existing EXE interface; no dialog implementation recovered'),'Library viewer scope or native behavior mismatch')
    require(component['storage']['callback']==dict(result='int',arguments=['int','int']),'Library callback result/arguments mismatch')
    require(component['storage']['card_ptr']==dict(bytes=152,members={'id':dict(type='uint32_t',offset=0),'full_name':dict(type='char *',offset=4),'name':dict(type='char *',offset=8)}),'Library native card display-name layout mismatch')
    require([x['bytes'] for x in component['storage']['vla_upper_bound']]==[[145,152,126,6,49,28,159],[113,127,159]],'Library chosen-IID VLA bound mismatch')
    require(next(v for v in component['locals']['2654d60'] if v['name']=='chosen_iids')['type']=='int[num_to_pick]','Library VLA must not use the array-bound DIE offset as an element count')
    for name,size,count in [('buffer','char[100]',1),('buf','char[200]',4)]:
        slots=[v for v in component['locals']['2654d60'] if v['name']==name]
        require(len(slots)==count and all(v['type']==size and v['location']=='(DW_OP_fbreg: -220)' for v in slots),'Library overlapping diagnostic/display storage mismatch')
    require(any(v['name']=='buffer' and v['type']=='char[50]' and v['location']=='(DW_OP_fbreg: -70)' for v in component['locals']['26526c0']),'Library reveal native 50-byte buffer mismatch')
    for a,data in CRITICAL.items():require(machine_bytes(int(a,16),len(bytes.fromhex(data))).hex()==data,'Library critical native instruction mismatch')
    require(component['critical_sites']==CRITICAL,'Library critical instruction membership mismatch')
    for block in component['ranges']:
        start,end=int(block['start'],16),int(block['end'],16)
        require(sha(machine_bytes(start,end-start))==block['sha256'],'Library native format/callback/return range mismatch')
    require([(x['start'],x['end']) for x in component['ranges']]==RANGES,'Library native evidence range membership mismatch')
    require(strip_comments(STORAGE).split()==strip_comments(sources[SOURCE][sources[SOURCE].index('typedef union',sources[SOURCE].index('/* Recovered October 2022 library reveal')):sources[SOURCE].index('} library_reveal_text_2022_t;',sources[SOURCE].index('/* Recovered October 2022 library reveal'))+len('} library_reveal_text_2022_t;')]).split(),'Library scratch union source mismatch')
    for a,entry in entries.items():
        require(not entry['static'] and entry['library_reveal_evidence']==component['functions'][a],'Library helper evidence/linkage mismatch')
        require([p['name'] for p in component['parameters'][a]][:2]==(['player','num_cards'] if a=='26526c0' else ['src_player','src_card']),'Library public parameter order mismatch')
        body=function_body(sources[SOURCE].replace('\r\n','\n'),entry['name'])
        require(sha(body.encode())==entry['reconstruction_body_sha256'],'Library helper source body mismatch')
        decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;detail=component['functions'][a]
        instructions=list(decoder.disasm(machine_bytes(int(a,16),detail['machine_bytes']),int(a,16)))
        ledger={x['address'] for x in detail['canonical_calls']}
        extra={f'{x.address:x}' for x in instructions if x.mnemonic=='call' and f'{x.address:x}' not in ledger}
        require(extra==({'2654dd7','2655212'} if a=='2654d60' else set()),'Library native call missing from the callee ledger or the explicit stack-probe/callback evidence')
    worker=function_body(sources[SOURCE],entries['2654d60']['name']);decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    require('cards_ptr[get_id(src_player, src_card)]->name' in worker,'Library title must use the native +8 name field')
    detail=entries['2654d60']['library_reveal_evidence'];native=machine_bytes(0x2654d60,detail['machine_bytes'])
    instructions={f'{i.address:x}':i for i in decoder.disasm(native,0x2654d60)}
    require(set(component['native_literals'])=={f'{a:x}' for a in LITERALS.values()},'Library printf/title literal membership mismatch')
    for a,literal in component['native_literals'].items():
        data=bytes.fromhex(literal['bytes']);require(machine_bytes(int(a,16),len(data)+1)==data+b'\0','Library diagnostic/title literal bytes mismatch')
        require(worker.count(byte_literal(data))==(4 if int(a,16)==LITERALS['LITERAL_SOURCE'] else 1),'Library diagnostic/title source literal usage mismatch')
        require(len(literal['sites'])==(4 if int(a,16)==LITERALS['LITERAL_SOURCE'] else 1),'Library native printf literal use count mismatch')
        for site in literal['sites']:
            ins=instructions[site['address']]
            require(ins.bytes.hex()==site['bytes'] and ins.mnemonic=='mov' and ins.operands[0].type==X86_OP_MEM and ins.operands[1].type==X86_OP_IMM and ins.operands[1].imm==int(a,16),'Library native printf literal materialization mismatch')
    fnbody=function_body(sources[SOURCE],entries['2655740']['name'])
    require(re.search(r'\(library_reveal_callback_2022_t\)0x0',fnbody) is not None,'Library callback frontend must preserve native NULL forwarding')
    for retained in component['retained_helpers']:
        require(sha(function_body(sources[SOURCE].replace('\r\n','\n'),retained['name']).encode())==retained['body_sha256'],'Library original source helper changed')
    require(CALLBACK in read(root/'manalink.h') or component.get('planning') is True,'Library callback typedef missing')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--bind-cards',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    if args.bind_cards:bind_cards(root,args.apply);return
    manifest=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'library_reveal_2022' not in interfaces
    assert not set(HELPERS)&{f['address'] for f in manifest['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes())
    machine_bytes=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    proof=dict(debug_sha256=sha(debug_data),**library_debug(debug,meta),storage=debug_storage(debug),functions={},retained_helpers=[],
        native_literals={},critical_sites=CRITICAL,ranges=[dict(start=a,end=b,sha256=sha(machine_bytes(int(a,16),int(b,16)-int(a,16)))) for a,b in RANGES],
        native_quirks=QUIRKS,viewer_boundary=dict(name='show_deck_exe',address='479360',arguments=6,status='existing EXE interface; no dialog implementation recovered'),
        scope='Six native library reveal/selection helpers, VLA-backed chosen-IID staging, overlapping text scratch, restored diagnostic/title arguments and native callback behavior. Retain old source interfaces; use the existing Magic.exe deck-viewer boundary.',
        integration_pending='Manual earlier consumers, native linkage and gameplay verification remain pending.')
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    natives={a:machine_bytes(int(a,16),int(meta[a]['high_pc'],16)-int(a,16)) for a in HELPERS}
    instructions={a:list(decoder.disasm(data,int(a,16))) for a,data in natives.items()}
    for address in LITERALS.values():
        data=readonly_string(runtime,address);assert data is not None
        sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex()) for i in instructions['2654d60'] if i.mnemonic=='mov' and any(o.type==X86_OP_IMM and o.imm==address for o in i.operands)]
        proof['native_literals'][f'{address:x}']=dict(bytes=data.hex(),sites=sites)
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    source=read(root/SOURCE)
    for name in ['reveal_top_cards_of_library','reveal_top_cards_of_library_and_choose','reveal_top_cards_of_library_and_choose_type']:
        proof['retained_helpers'].append(dict(name=name,body_sha256=sha(function_body(source,name).encode())))
    targets={int(a,16):r['name'] for a,r in meta.items()};targets.update({int(f['address'],16):f['name'] for f in manifest['accepted_functions']})
    targets.update({int(a,16):LIBRARY_REVEAL_API_NAMES[meta[a]['name']] for a in HELPERS})
    targets.update({0x479360:'show_deck_exe',0x479d10:'remove_card_from_deck',0x479d80:'raw_put_iid_on_top_of_deck',0x471d60:'call_sub_437E20_unless_ai_is_speculating',0x2701df0:'sprintf'})
    entries=[];bodies={}
    for a in HELPERS:
        row=meta[a];raw=read(cache/'decompiled-library-reveal'/f'{a}.c');original=strip_comments(raw);text=original;changes=[]
        if a=='2654d60':
            text=WORKER
            for placeholder,address in LITERALS.items():text=text.replace(placeholder,byte_literal(readonly_string(runtime,address)))
        elif a=='26555e0':text=TYPE_FRONTEND
        else:text=text.replace('_func_int_int_int *','library_reveal_callback_2022_t ').replace('EXPANSION_0','0')
        if text!=original:
            changes.append(dict(original=original,replacement=text,occurrences=1,kind='native library VLA/storage/variadic frame and ABI reconstruction',
                machine_sites=[dict(address=site,bytes=data) for site,data in CRITICAL.items()]))
        body=cleaned(text,overlap);assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x|_conflict',body),a
        symbols=[s for n,o,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+o==int(a,16)];assert symbols==[2],(a,symbols)
        calls=[]
        for ins in instructions[a]:
            if ins.mnemonic not in {'call','jmp'} or ins.operands[0].type!=X86_OP_IMM:continue
            target=ins.operands[0].imm
            if target==0x26fc1d0:continue # compiler stack probe, restored by the VLA
            if target not in targets:
                assert ins.mnemonic!='call',(a,f'{target:x}','unknown native callee')
                continue
            name=targets[target]
            # Project any legacy API spelling in the DWARF/EXE map through the same adapters.
            if not re.search(r'\b'+name+r'\s*\(',body):
                probe=cleaned('int probe(void) { '+name+'(); return 0; }',overlap)
                name=re.search(r'\{\s*(\w+)\(',probe)[1]
            assert re.search(r'\b'+name+r'\s*\(',body),(a,f'{target:x}',name)
            calls.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{target:x}',name=name))
        name=LIBRARY_REVEAL_API_NAMES[row['name']];detail=dict(machine_bytes=len(natives[a]),machine_sha256=sha(natives[a]),changes=changes,canonical_calls=calls)
        proof['functions'][a]=detail;bodies[a]=body
        entries.append(dict(address=a,name=name,source=SOURCE,line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',
            raw_sha256=sha(raw.encode()),recovered_literals=[],library_reveal_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    source+='\n\n#ifndef SHANDALAR\n/* Recovered October 2022 library reveal and selection logic. */\n'+format_c(STORAGE)+'\n'
    for entry in entries:
        a=entry['address'];notice=''
        if a=='2655740':notice='\n * Native behavior: this frontend ignores fn and passes NULL to the worker.'
        if a=='26526c0':notice='\n * Native behavior: the 50-byte local is passed with a formatting bound of 100.'
        source+=f'\n/* ManalinkEh 2022: 0x{a}; original {SOURCE}:{entry["line"]}.{notice} */\n'+bodies[a]+'\n'
    source+='#endif\n';header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+CALLBACK+'\n'+'\n'.join(e['prototype'] for e in entries)+'\n#endif\n\n'+header[at:]
    compile_sources(root,cache/'library-reveal-check',{SOURCE:source},header)
    proof['planning']=True;audit_component(proof,{**manifest,'accepted_functions':manifest['accepted_functions']+entries},{SOURCE:source},machine_bytes,root);del proof['planning']
    if args.apply:
        citations=manifest['accepted_functions']+manifest['source_fragments']+[s for f in manifest['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not set(HELPERS)&{c['address'] for c in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};focused={r['address']:r for r in csv.DictReader((cache/'decompiled-library-reveal/export.tsv').open(),delimiter='\t')}
        for a in HELPERS:
            rows[a]=focused[a];shutil.copyfile(cache/'decompiled-library-reveal'/f'{a}.c',cache/'decompiled-abi'/f'{a}.c')
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        (root/SOURCE).write_bytes(source.replace('\n','\r\n').encode('latin1'));(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        manifest['accepted_functions']+=entries;manifest['accepted']+=6;manifest['helper_functions']+=6;manifest['shared_helper_interfaces']+=6;manifest['source_files'][SOURCE]+=6
        manifest['raw_export_entries']=len(rows);manifest['exported_functions']=len({r['name'] for r in rows.values()})
        save_json(mp,manifest);interfaces['library_reveal_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'library-reveal-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_public_helpers=6,restored_printf_title_literals=6,native_literal_sites=9,new_dialog_implementations=0)))


if __name__=='__main__':main()
