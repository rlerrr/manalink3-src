"""Recover native spell-copy helpers and their two forced-target card consumers."""
import argparse,csv,json,re,shutil,subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from elftools.dwarf.locationlists import LocationParser,LocationExpr,BaseAddressEntry
from elftools.dwarf.descriptions import describe_DWARF_expr
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import clean,strip_comments,UNRESOLVED
from recover_test_consumers import read,rename,format_c
from audit_source import function_body
from recover_temporary_exile import save_json,sha

HELPERS='2668490 2668690 26686d0 2668720'.split()
CARDS='24558a0 252f1a0'.split()
NAMES={'real_copy_spell':'real_copy_spell_2022','copy_spell':'copy_spell_2022','copy_spell_from_stack':'copy_spell_from_stack_2022'}
HEADER_LABEL='/* October 2022 spell-copy APIs. */'
FLAG_EXPRESSION='(SF_NOT_CAST | SF_PLAYED_FOR_FREE)'
FORCED_EXPRESSION='(SF_NOT_CAST | SF_PLAYED_FOR_FREE | SF_TARGETS_ALREADY_SET)'
CONTROL='26684cb 26684d1 26684d4 26684d7 26684f1 266855c 2668563 2668565 2668568 266856b 266856e 266857c 2668582 26685d8 26685e7 26685ef 26686bd 2668710 2668721 2668741'.split()
LOOP_COMMENT='/* Native 2022 loop has no normal exit or target-index advance once entered. */'


def debug_evidence(debug,meta):
    dw=debug.dwarf();lp=LocationParser(dw.location_lists());parameters={};locals_={};enums={}
    wanted={'SF_NOT_CAST','SF_PLAYED_FOR_FREE','SF_TARGETS_ALREADY_SET','SF2_COPIED_FROM_STACK','EVENT_SPELL_COPIED','TYPE_SPELL','TRIGGER_SPELL_CAST','EVENT_SPA'}
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {meta[a]['source'] for a in HELPERS+CARDS}:continue
        def loc(die):
            at=die.attributes.get('DW_AT_location')
            if at is None:return None
            x=lp.parse_from_attribute(at,cu['version'],die=die)
            if isinstance(x,LocationExpr):return describe_DWARF_expr(x.loc_expr,dw.structs)
            return [dict(base=hex(y.base_address)) if isinstance(y,BaseAddressEntry) else dict(start=hex(y.begin_offset),end=hex(y.end_offset),expression=describe_DWARF_expr(y.loc_expr,dw.structs)) for y in x]
        def variables(die):
            out=[]
            for v in die.iter_children():
                if v.tag=='DW_TAG_variable':out.append(dict(name=inherited_value(v,'DW_AT_name'),type=type_name(type_of(v)),location=loc(v)))
                elif v.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:out+=variables(v)
            return out
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':
                for child in die.iter_children():
                    name=value(child,'DW_AT_name')
                    if name in wanted:
                        number=value(child,'DW_AT_const_value');assert name not in enums or enums[name]==number;enums[name]=number
            pc=value(die,'DW_AT_low_pc');address=f'{pc:x}' if pc else ''
            if address not in HELPERS+CARDS:continue
            parameters[address]=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=loc(p)) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter']
            locals_[address]=variables(die)
    assert set(parameters)==set(HELPERS+CARDS) and set(enums)==wanted
    return dict(parameters=parameters,locals=locals_,enum_values=enums)


def compile_sources(root,scratch,sources,header=None):
    scratch.mkdir(exist_ok=True)
    if header is not None:(scratch/'manalink.h').write_bytes(header.encode('latin1'))
    for source,text in sources.items():
        path=scratch/Path(source).name;path.write_bytes(text.encode('latin1'))
        result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0',
            '-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format',
            '-I',str(scratch),'-I',str(root),str(path)],capture_output=True,text=True);path.with_suffix('.log').write_text(result.stderr)
        if result.returncode:print(result.stderr[-6000:])
        result.check_returncode()


def audit_component(component,manifest,sources,machine_bytes,root):
    def require(ok,message):
        if not ok:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('spell_copy_evidence')}
    require(set(entries)==set(HELPERS)==set(component['functions']) and {a for a,f in entries.items() if f['static']}=={'2668490'},'Spell-copy helper membership/linkage mismatch')
    worker=component['parameters']['2668490']
    require([p['name'] for p in worker]==['player','internal_id','special_flags','special_flags3','copied','forced_target'],'Spell-copy logical parameter order mismatch')
    for p,expr in zip(worker,['DW_OP_reg0 (r0)','DW_OP_reg2 (r2)','DW_OP_reg1 (r1)','DW_OP_fbreg: 0','DW_OP_fbreg: 4','DW_OP_fbreg: 8']):
        require(expr in str(p['location']),'Spell-copy physical register/stack argument mismatch')
    require(any(v['name']=='targ' and v['type']=='target_t' and v['location']=='(DW_OP_fbreg: -16)' for v in component['locals']['2668720']),
            'Forced-copy target local mismatch')
    require([s['address'] for s in component['control_sites']]==CONTROL,'Spell-copy native control membership mismatch')
    for site in component['control_sites']:
        data=bytes.fromhex(site['bytes']);require(machine_bytes(int(site['address'],16),len(data))==data,'Spell-copy native copy/store/control instruction mismatch')
    for address,entry in entries.items():require(entry['spell_copy_evidence']==component['functions'][address],'Spell-copy function evidence mismatch')
    require(component['instance_layout']==dict(bytes=300,info_slot=56,eot_toughness=76,targets=116,target_bytes=8,target_count=19,number_of_targets=54),
            'Spell-copy common instance layout mismatch')
    require(component['enum_values']['EVENT_SPELL_COPIED']==0x5c00 and
            (component['enum_values']['SF2_COPIED_FROM_STACK']&0xffffffff)==0x80000000 and
            component['enum_values']['SF_TARGETS_ALREADY_SET']==0x20000000,'Spell-copy native enum mismatch')
    dispatch=component['dispatch']
    require(dispatch==dict(address='4359b0',call_address='26685e7',call_bytes='e8c4d3dcfd',event=0x5c00),'Spell-copy event dispatch binding mismatch')
    require(re.search(r'_dispatch_event\s*=\s*0x4359b0;',read(root/'manalink.lds')) and
            re.search(r'int\s+dispatch_event\(int player, int card, event_t event\);',read(root/'manalink.h')),'Spell-copy retained EXE dispatch interface mismatch')
    body=function_body(sources[entries['2668490']['source']],entries['2668490']['name'])
    require('EVENT_SPELL_COPIED' in body and 'forced_target' in body and 'SF2_COPIED_FROM_STACK' in body,'Spell-copy event/forced-target/flags projection missing')
    for global_ in component['existing_globals']:
        require(global_['storage']==3 and global_['initial_bytes']=='ffffffff' and machine_bytes(int(global_['address'],16),4).hex()=='ffffffff',
                'Spell-copy retained private global mismatch')
        require(re.search(r'static int '+global_['name']+r' = -1;',sources['functions/play_spell_for_free.c']),'Spell-copy retained global declaration changed')


def audit_cards(component,manifest,sources,machine_bytes):
    def require(ok,message):
        if not ok:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('forced_copy_card_evidence')}
    require(set(entries)==set(CARDS)==set(component['functions']),'Forced-copy card membership mismatch')
    for a,e in entries.items():require(e['forced_copy_card_evidence']==component['functions'][a] and not e['static'],'Forced-copy card evidence/linkage mismatch')
    expected={'24558a0':('marked','int[151]','(DW_OP_fbreg: -624)'), '252f1a0':('marked','int[2][151]','(DW_OP_fbreg: -1228)')}
    for a,(name,type_,location) in expected.items():
        require(any(v['name']==name and v['type']==type_ and v['location']==location for v in component['locals'][a]),'Forced-copy card target snapshot layout mismatch')
    require(any(v['name']=='mc' and v['type']=='int[2]' and v['location']=='(DW_OP_fbreg: -1244)' for v in component['locals']['252f1a0']),
            'Ink-Treader per-player counters mismatch')
    loop=component['zada_native_loop'];require(loop==dict(call_address='2455a49',call_bytes='e8d22c2100',branch_address='2455a4e',branch_bytes='ebe0',destination='2455a30'),
            'Zada native nonterminating loop evidence mismatch')
    require(machine_bytes(0x2455a49,5).hex()==loop['call_bytes'] and machine_bytes(0x2455a4e,2).hex()==loop['branch_bytes'],
            'Zada native unconditional loop instruction mismatch')
    source=sources[entries['24558a0']['source']]
    require(LOOP_COMMENT in source and entries['24558a0'].get('known_native_behavior')=='Copy loop has no normal exit or target advance once reached.',
            'Zada native loop annotation missing')
    body=function_body(source,'card_zada_hedron_grinder')
    require('while (true)' in body,'Zada native unconditional copy loop changed')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--cards',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';directory=cache/'decompiled-spell-copy'
    mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());addresses=CARDS if args.cards else HELPERS
    component='forced_spell_copy_cards_2022' if args.cards else 'spell_copy_2022';assert component not in interfaces
    if args.cards:assert 'spell_copy_2022' in interfaces
    assert not set(addresses)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data)
    proof=dict(debug_sha256=sha(debug_data),**debug_evidence(debug,meta),functions={},
        scope='Native forced-target spell-copy helpers and snapshot-based card consumers; retain native copy flags, target override, event dispatch and source callback state. Earlier source functions remain separate.',
        integration_pending='Earlier manual consumers, native linking and gameplay verification remain pending.')
    if args.cards:
        proof['zada_native_loop']=dict(call_address='2455a49',call_bytes='e8d22c2100',branch_address='2455a4e',branch_bytes='ebe0',destination='2455a30')
    else:
        proof['instance_layout']=dict(bytes=300,info_slot=56,eot_toughness=76,targets=116,target_bytes=8,target_count=19,number_of_targets=54)
        proof['dispatch']=dict(address='4359b0',call_address='26685e7',call_bytes='e8c4d3dcfd',event=0x5c00)
        proof['existing_globals']=[]
        for name,address in [('free_spell_player','2721294'),('free_spell_card','2721298')]:
            symbols=[s for n,o,sec,k,s in debug.symbols if n=='_'+name and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+o==int(address,16)]
            assert symbols==[3],(name,symbols)
            proof['existing_globals'].append(dict(name=name,address=address,storage=3,initial_bytes='ffffffff'))
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']}
    targets.update({int(a,16):NAMES.get(meta[a]['name'],meta[a]['name']) for a in addresses})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    entries=[];sources={};all_instructions={}
    for address in addresses:
        row=meta[address];name=NAMES.get(row['name'],row['name']);raw=read(directory/f'{address}.c');text=strip_comments(raw)
        native=runtime.pe.get_data(int(address,16)-0x2000000,int(row['high_pc'],16)-int(address,16));instructions=list(decoder.disasm(native,int(address,16)))
        by_site={f'{i.address:x}':i for i in instructions};all_instructions.update(by_site);changes=[]
        replacements=[]
        if address=='2668490':
            replacements=[('pcVar3->field_26 = copied->field_26;','pcVar3->info_slot = copied->info_slot;',['26684cb','26684d1']),
                ('pcVar3->field_31 = copied->field_31;','pcVar3->eot_toughness = copied->eot_toughness;',['26684d4','26684d7']),
                ('0x80000000','SF2_COPIED_FROM_STACK',['2668528']),('0x5c00','EVENT_SPELL_COPIED',['26685d8','26685e7'])]
        elif address in ['2668690','26686d0']:
            replacements=[('0xc0000',FLAG_EXPRESSION,['26686bd' if address=='2668690' else '2668710'])]
        elif address=='2668720':replacements=[('0x200c0000',FORCED_EXPRESSION,['2668721'])]
        for original,replacement,sites in replacements:
            assert text.count(original)==1,(address,original);text=text.replace(original,replacement)
            changes.append(dict(original=original,replacement=replacement,occurrences=1,kind='native spell-copy scalar field/flag/event projection',
                machine_sites=[dict(address=a,bytes=by_site[a].bytes.hex()) for a in sites]))
        text,_=rename(text,NAMES)
        body=clean(text,overlap,use_keyword_accessors_2022=True,use_extension_flags_2022=True,use_test_api_2022=True,
            use_target_api_2022=True,use_ability_api_2022=True,use_pump_api_2022=True,use_spell_trigger_api_2022=True)
        if address=='2668490':body='static '+body
        if address=='24558a0':
            token='copy_spell_from_stack_set_target0(';at=body.index(token);body=body[:at]+LOOP_COMMENT+'\n'+body[at:]
        body=format_c(body);assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x',body),address
        symbols=[s for n,o,sec,k,s in debug.functions() if debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+o==int(address,16)];assert symbols==([3] if address=='2668490' else [2])
        calls=[]
        for ins in instructions:
            if ins.mnemonic in {'call','jmp'} and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm in targets:
                target=targets[ins.operands[0].imm];assert re.search(r'\b'+target+r'\s*\(',body),(address,target)
                calls.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{ins.operands[0].imm:x}',name=target))
        detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,canonical_calls=calls);proof['functions'][address]=detail
        entry=dict(address=address,name=name,source=row['source'],line=row['line'],static=address=='2668490',prototype=body[:body.index('{')].strip()+';',
            raw_sha256=sha(raw.encode()),recovered_literals=[],reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode()))
        entry['forced_copy_card_evidence' if args.cards else 'spell_copy_evidence']=detail
        if address=='24558a0':entry['known_native_behavior']='Copy loop has no normal exit or target advance once reached.'
        entries.append(entry);sources.setdefault(row['source'],read(root/row['source']))
        sources[row['source']]+=f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{address}; original {row["source"]}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=None
    if not args.cards:
        proof['control_sites']=[dict(address=a,bytes=all_instructions[a].bytes.hex()) for a in CONTROL]
        header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
        header=header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if not e['static'])+'\n#endif\n\n'+header[at:]
    compile_sources(root,cache/('spell-copy-cards-check' if args.cards else 'spell-copy-helpers-check'),sources,header)
    if args.cards:audit_cards(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,lambda a,n:runtime.pe.get_data(a-0x2000000,n))
    if args.apply:
        citations=m['source_fragments']+m['accepted_functions']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not set(addresses)&{c['address'] for c in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')}
        focused={r['address']:r for r in csv.DictReader((directory/'export.tsv').open(),delimiter='\t')}
        for a in addresses:rows[a]=focused[a];shutil.copyfile(directory/f'{a}.c',cache/'decompiled-abi'/f'{a}.c')
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        if header is not None:(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=len(entries);m['card_handler_functions']+=2 if args.cards else 0;m['helper_functions']+=0 if args.cards else 4
        if not args.cards:m['private_functions']+=1;m['private_helpers']+=1;m['shared_helper_interfaces']+=3
        for entry in entries:m['source_files'][entry['source']]=m['source_files'].get(entry['source'],0)+1
        registrations=list(csv.DictReader((cache/'2022-registrations.csv').open()));names={f['name'] for f in m['accepted_functions']}
        m['registered_handler_symbols']=len({r['symbol'].lstrip('_') for r in registrations if r['symbol'].lstrip('_') in names})
        m['registered_handler_symbols_marked_coded']=len({r['symbol'].lstrip('_') for r in registrations if r['coded']=='True' and r['symbol'].lstrip('_') in names})
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m)
        interfaces[component]=proof;save_json(ip,interfaces)
    save_json(cache/('forced-copy-card-plan.json' if args.cards else 'spell-copy-plan.json'),proof)
    print(json.dumps(dict(applied=args.apply,new_implementations=len(entries),card_functions=2 if args.cards else 0,helpers=0 if args.cards else 4)))


if __name__=='__main__':main()
