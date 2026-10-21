"""Recover native Phyrexian costs, payment state and the Krrik mana-engine blocks."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import struct
import subprocess
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from elftools.dwarf.descriptions import describe_DWARF_expr
from elftools.dwarf.locationlists import LocationParser, LocationExpr, BaseAddressEntry
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, strip_comments, UNRESOLVED
from recover_test_consumers import read, rename, format_c
from recover_literals import recover_strings,readonly_string,byte_literal
from phyrexian_engine_fragments import BLOCKS
from audit_source import function_body

ADDRESSES = '226caa0 226d7a0 226d900 226d990 226da10 226db00'.split()
PRIVATE = {'226caa0'}
ENGINE_ADDRESS='26ae610'
FLAG='charge_mana_called_from_pay_phyrexian_mana'
WORDS='phyrexian_color_words_2022'
CONTROL_SITES='26ae87d 26ae88b 26ae898 26af2fa 26af30a 26af4ca 26af4ef 26af520 26af525 26af4fe 26af7ad 26af7b3 26af7be 26ae6c7 26aea80 26aea8d'.split()


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    parser=argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply',action='store_true')
    args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    mp=root/'docs/recovery-evidence/recovered-functions.json'
    ip=root/'docs/recovery-evidence/interface-recovery.json'
    manifest=json.loads(mp.read_text());interfaces=json.loads(ip.read_text())
    assert 'phyrexian_costs_2022' not in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in manifest['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    names={a:meta[a]['name']+'_2022' for a in ADDRESSES}
    mapping={meta[a]['name']:n for a,n in names.items()}
    directories={a:cache/'decompiled-phyrexian' for a in ADDRESSES}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug=Image(debug_data);dwarf=debug.dwarf();locations=LocationParser(dwarf.location_lists())
    parameters,locals_,enums={},{},{}
    flag_die=None;words_die=None
    for cu in dwarf.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {meta[a]['source'] for a in ADDRESSES}:continue
        def location(die):
            attr=die.attributes.get('DW_AT_location')
            if attr is None:return None
            loc=locations.parse_from_attribute(attr,cu['version'],die=die)
            if isinstance(loc,LocationExpr):return describe_DWARF_expr(loc.loc_expr,dwarf.structs)
            return [dict(base=hex(x.base_address)) if isinstance(x,BaseAddressEntry) else
                    dict(start=hex(x.begin_offset),end=hex(x.end_offset),expression=describe_DWARF_expr(x.loc_expr,dwarf.structs)) for x in loc]
        def variables(die):
            nonlocal words_die
            out=[]
            for c in die.iter_children():
                if c.tag=='DW_TAG_variable':
                    out.append(dict(name=inherited_value(c,'DW_AT_name'),type=type_name(type_of(c)),location=location(c)))
                    if inherited_value(c,'DW_AT_name')=='words':words_die=c
                elif c.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:out.extend(variables(c))
            return out
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_variable' and inherited_value(die,'DW_AT_name')==FLAG and not value(die,'DW_AT_declaration'):flag_die=die
            if die.tag=='DW_TAG_enumeration_type':enums.update({value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in die.iter_children() if c.tag=='DW_TAG_enumerator'})
            pc=value(die,'DW_AT_low_pc');address=f'{pc:x}' if pc else ''
            if address not in ADDRESSES:continue
            parameters[address]=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p)) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter']
            locals_[address]=variables(die)
    assert set(parameters)==set(ADDRESSES) and [len(parameters[a]) for a in ADDRESSES]==[8,8,8,10,8,10]
    private_parameters=parameters['226caa0']
    assert [p['name'] for p in private_parameters]==['player','cless','black','blue','green','red','white','clred_artifact']
    for p,expr in zip(private_parameters[:3],['DW_OP_reg0 (r0)','DW_OP_reg2 (r2)','DW_OP_reg1 (r1)']):assert p['location'][0]['expression']=='('+expr+')'
    assert flag_die is not None and words_die is not None and type_name(type_of(flag_die))=='int' and type_name(type_of(words_die))=='char *[6]'
    assert flag_die.attributes['DW_AT_location'].value==[3,28,64,119,2] and words_die.attributes['DW_AT_location'].value==[3,88,156,115,2]
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    natives={a:runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)) for a in ADDRESSES+[ENGINE_ADDRESS]}
    instructions={a:list(decoder.disasm(natives[a],int(a,16))) for a in ADDRESSES+[ENGINE_ADDRESS]}
    by_site={f'{i.address:x}':i for items in instructions.values() for i in items}
    controls=[dict(address=a,bytes=by_site[a].bytes.hex(),instruction=by_site[a].mnemonic+' '+by_site[a].op_str) for a in CONTROL_SITES]
    table=runtime.pe.get_data(0x2739c58-0x2000000,24)
    word_addresses=struct.unpack('<6I',table);words=[readonly_string(runtime,a) for a in word_addresses]
    assert words==[b'colorless',b'black',b'blue',b'green',b'red',b'white']
    data_symbols=[]
    for name,offset,section,kind,storage in debug.symbols:
        if section<=0:continue
        address=debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[section-1].VirtualAddress+offset
        if address in {0x277401c,0x2739c58}:data_symbols.append(dict(name=name,address=f'{address:x}',storage=storage))
    assert dict(name='_'+FLAG,address='277401c',storage=2) in data_symbols
    assert any(s['address']=='2739c58' and s['storage']==3 for s in data_symbols)
    flag_sites=[];word_sites=[]
    for address,items in instructions.items():
        for instruction in items:
            for operand in instruction.operands:
                if operand.type!=X86_OP_MEM or operand.mem.disp not in {0x277401c,0x2739c58}:continue
                assert operand.size==4
                site=dict(address=f'{instruction.address:x}',bytes=instruction.bytes.hex(),function=address)
                (flag_sites if operand.mem.disp==0x277401c else word_sites).append(site)
    assert len(flag_sites)==3 and len(word_sites)==1
    flag_section=next(sec for sec in runtime.pe.sections if sec.VirtualAddress<=0x77401c<sec.VirtualAddress+sec.Misc_VirtualSize)
    assert flag_section.SizeOfRawData==0
    flag_declaration='int '+FLAG+';'
    word_declaration='static const char *const '+WORDS+'[6] = {'+', '.join(byte_literal(w) for w in words)+'};'
    globals_=[dict(source='cards/new_phyrexia.c',name=FLAG,declaration=flag_declaration,target_address='277401c',zero_initialized=True,linkage='external'),
              dict(source='cards/new_phyrexia.c',name=WORDS,declaration=word_declaration,target_address='2739c58',zero_initialized=False,linkage='static')]
    proof=dict(debug_sha256=sha(debug_data),parameters=parameters,locals=locals_,enum_values={},control_sites=controls,functions={},
        data_symbols=data_symbols,
        payment_flag=dict(address='277401c',bytes=4,zero_fill=True,location=flag_die.attributes['DW_AT_location'].value,declaration=globals_[0],sites=flag_sites),
        color_words=dict(address='2739c58',bytes=table.hex(),location=words_die.attributes['DW_AT_location'].value,words=[dict(address=f'{a:x}',bytes=w.hex()) for a,w in zip(word_addresses,words)],declaration=globals_[1],sites=word_sites),
        scope='Native Phyrexian mana availability/payment, casting and activated costs, AI/life bounds, artifact-only colorless mana and X handling. Recover the payment-mode global plus Krrik payment selection/settlement in the existing engine charger.',
        integration_pending='Earlier manual callers, complete native engine reconstruction/linkage and gameplay validation remain pending.')
    targets={int(f['address'],16):f['name'] for f in manifest['accepted_functions']}
    targets.update({int(a,16):n for a,n in names.items()})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    bodies,entries={},[]
    for address,name in names.items():
        raw=(directories[address]/(address+'.c')).read_text();original=strip_comments(raw);text=original;changes=[]
        if address=='226caa0':
            text=text.replace('charge_phyrexian_mana_impl::words',WORDS)
            assert text!=original
            changes=[dict(original=original,replacement=text,occurrences=1,kind='native private color-word table',machine_sites=[])]
        proof['enum_values'].update({n:enums[n] for n in set(re.findall(r'\b(?:EVENT|AL|COUNTER|RESOLVE_TRIGGER)_\w+\b',text)) if n in enums})
        text,_=rename(text,mapping);text,literals=recover_strings(text,runtime,{})
        body=clean(text,overlap,use_test_api_2022=True,use_target_api_2022=True,use_sacrifice_api_2022=True,
            use_extension_flags_2022=True,use_keyword_accessors_2022=True,use_ability_api_2022=True,
            use_pump_api_2022=True,use_activated_api_2022=True,use_attack_api_2022=True)
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x',body),address
        if address in PRIVATE:body='static '+body
        body=format_c(body);bodies[address]=body
        symbols=[s for n,off,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off==int(address,16)]
        assert symbols==([3] if address in PRIVATE else [2]),(address,symbols)
        calls=[]
        for i in instructions[address]:
            if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm in targets:
                destination=targets[i.operands[0].imm]
                assert re.search(r'\b'+destination+r'\s*\(',body),(address,destination)
                calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{i.operands[0].imm:x}',name=destination))
        detail=dict(machine_bytes=len(natives[address]),machine_sha256=sha(natives[address]),changes=changes,canonical_calls=calls)
        proof['functions'][address]=detail
        entries.append(dict(name=name,address=address,source=meta[address]['source'],line=meta[address]['line'],static=address in PRIVATE,
            prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,
            phyrexian_cost_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    sources={s:read(root/s) for s in {e['source'] for e in entries}}
    for source in sources:
        sources[source]+='\n\n#ifndef SHANDALAR\n/* Recovered October 2022 Phyrexian cost helpers. */\n'
        sources[source]+='\n'.join(g['declaration'] for g in globals_)+'\n'
        for e in entries:
            if e['source']==source:sources[source]+=f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n"+bodies[e['address']]+'\n'
        sources[source]+='#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+'/* October 2022 Phyrexian cost APIs. */\n#ifndef SHANDALAR\n'+'extern int '+FLAG+';\n'+'\n'.join(e['prototype'] for e in entries if not e['static'])+'\n#endif\n\n'+header[at:]
    # Insert only the three separately recorded native engine blocks.
    engine_path='functions/engine.c';engine=read(root/engine_path);previous_engine=function_body(engine,'charge_mana');fragments=[]
    raw_engine=(cache/'decompiled-charge-core'/(ENGINE_ADDRESS+'.c')).read_text()
    for label,code,anchor,indent in BLOCKS:
        start='/* Recovered 2022 Phyrexian-payment fragment: charge_mana '+label+' @ 0x'+ENGINE_ADDRESS+'. */'
        end='/* End recovered 2022 Phyrexian-payment fragment: '+label+'. */'
        payload='#ifndef SHANDALAR\n'+'\n'.join(' '*indent+line if line else '' for line in format_c(code).strip().splitlines())+'\n#endif'
        block=start+'\n'+payload+'\n'+end+'\n'
        at=engine.index(anchor,engine.index('int charge_mana(int player, color_t color, int amount)'))
        engine=engine[:at]+block+engine[at:]
        fragments.append(dict(source=engine_path,original_function='charge_mana',address=ENGINE_ADDRESS,raw_export='decompiled-charge-core/'+ENGINE_ADDRESS+'.c',raw_sha256=sha(raw_engine.encode()),start_marker=start,end_marker=end,body_sha256=sha(payload.encode()),piece=label))
    engine_prompt=readonly_string(runtime,0x2764a2c)
    assert engine_prompt==b' Pay 2 life for how much black mana? (max %d, min %d)'
    proof['engine']=dict(machine_bytes=len(natives[ENGINE_ADDRESS]),machine_sha256=sha(natives[ENGINE_ADDRESS]),previous_body_sha256=sha(previous_engine.encode()),body_sha256=sha(function_body(engine,'charge_mana').encode()),fragments=fragments,
        prompt=dict(address='2764a2c',bytes=engine_prompt.hex()),native_cancel_returns_without_cleanup=True,native_minimum_is_display_only=True,native_settlement_is_not_fizzle_gated=True)
    sources[engine_path]=engine
    scratch=cache/'phyrexian-costs-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'))
    for source,text in sources.items():
        target=scratch/Path(source).name;target.write_bytes(text.encode('latin1'))
        r=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only',
            '-ferror-limit=0','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type',
            '-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(target)],capture_output=True,text=True)
        (scratch/(Path(source).stem+'.log')).write_text(r.stderr)
        if r.returncode:print(r.stderr)
        r.check_returncode()
    export_path=cache/'decompiled-abi/export.tsv';rows=list(csv.DictReader(export_path.open(),delimiter='\t'));existing={r['address'] for r in rows}
    focused={r['address']:r for directory in set(directories.values()) for r in csv.DictReader((directory/'export.tsv').open(),delimiter='\t')}
    rows=[focused[r['address']] if r['address'] in names else r for r in rows];rows.extend(focused[a] for a in names if a not in existing)
    if args.apply:
        citations=manifest['source_fragments']+[s for f in manifest['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not any(c['address'] in names for c in citations)
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        manifest['accepted_functions']+=entries;manifest['accepted']+=6;manifest['helper_functions']+=6
        manifest['private_functions']+=1;manifest['private_helpers']+=1;manifest['shared_helper_interfaces']+=5
        for e in entries:manifest['source_files'][e['source']]=manifest['source_files'].get(e['source'],0)+1
        manifest['source_globals']+=globals_;manifest['source_fragments']+=fragments
        manifest['raw_export_entries']=len(rows);manifest['exported_functions']=len({r['name'] for r in rows})
        mp.write_text(json.dumps(manifest,indent=2));interfaces['phyrexian_costs_2022']=proof;ip.write_text(json.dumps(interfaces,indent=2))
        with export_path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
        for a in names:shutil.copyfile(directories[a]/(a+'.c'),export_path.parent/(a+'.c'))
    (cache/'phyrexian-costs-plan.json').write_text(json.dumps(proof,indent=2))
    print(json.dumps(dict(applied=args.apply,new_helpers=6,private_helpers=1,public_helpers=5,engine_fragments=3)))


if __name__=='__main__':main()
