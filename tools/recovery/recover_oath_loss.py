"""Recover the native Oath life-loss callback using verified original source control flow."""
import argparse
import copy
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from elftools.dwarf.descriptions import describe_DWARF_expr
from elftools.dwarf.locationlists import LocationParser, LocationExpr, BaseAddressEntry
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, strip_comments, UNRESOLVED
from recover_test_consumers import read, rename, format_c
from recover_literals import recover_strings,readonly_string
from audit_source import function_body

ADDRESSES = ['24040a0']
PRIVATE = set()
CONTROL_SITES = '24040a6 24040c5 24040eb 24040f2 24040f9 2404100 2404105 2404114 240413f 240415b 240416a 240418e 24041c7 24041e7 24041fe 2404207 240421e'.split()


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
    assert 'oath_life_loss_2022' not in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in manifest['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    names={a:meta[a]['name']+'_2022' for a in ADDRESSES}
    mapping={meta[a]['name']:n for a,n in names.items()}
    directories={a:cache/'decompiled-oath-loss' for a in ADDRESSES}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug=Image(debug_data);dwarf=debug.dwarf();locations=LocationParser(dwarf.location_lists())
    parameters,locals_,enums={},{},{}
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
            out=[]
            for c in die.iter_children():
                if c.tag=='DW_TAG_variable':
                    typ=type_of(c);item=dict(name=inherited_value(c,'DW_AT_name'),type=type_name(typ),location=location(c))
                    if item['name']=='tgts':item['upper_bound']=[p.attributes['DW_AT_upper_bound'].value for p in typ.iter_children() if 'DW_AT_upper_bound' in p.attributes]
                    out.append(item)
                elif c.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:out.extend(variables(c))
            return out
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':enums.update({value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in die.iter_children() if c.tag=='DW_TAG_enumerator'})
            pc=value(die,'DW_AT_low_pc');address=f'{pc:x}' if pc else ''
            if address not in ADDRESSES:continue
            parameters[address]=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p)) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter']
            locals_[address]=variables(die)
    assert set(parameters)==set(ADDRESSES) and len(parameters['24040a0'])==3
    assert any(v['name']=='td' and v['type']=='target_definition_t' and v['location']=='(DW_OP_fbreg: -104)' for v in locals_['24040a0'])
    targets_array=next(v for v in locals_['24040a0'] if v['name']=='tgts');assert targets_array['upper_bound']==[[145,8,6]]
    original_source=read(root/'cards/ice_age.c')
    template=function_body(original_source,'oath_of_lim_dul_lifeloss_trigger')
    predicate=function_body(original_source,'target_only_permanents_unless_in_hand')
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    natives={a:runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)) for a in ADDRESSES}
    instructions={a:list(decoder.disasm(natives[a],int(a,16))) for a in ADDRESSES}
    by_site={f'{i.address:x}':i for items in instructions.values() for i in items}
    controls=[dict(address=a,bytes=by_site[a].bytes.hex(),instruction=by_site[a].mnemonic+' '+by_site[a].op_str) for a in CONTROL_SITES]
    support=(cache/'decompiled-oath-loss/23ede20.c').read_text()
    prompt=readonly_string(runtime,0x2749380);assert prompt==b'Select a permanent to sacrifice or card to discard.'
    predicate_meta=meta['23ede20'];predicate_code=runtime.pe.get_data(0x23ede20-0x2000000,int(predicate_meta['high_pc'],16)-0x23ede20)
    proof=dict(debug_sha256=sha(debug_data),parameters=parameters,locals=locals_,enum_values={},control_sites=controls,functions={},
        source_template=dict(source='cards/ice_age.c',name='oath_of_lim_dul_lifeloss_trigger',body_sha256=sha(template.encode())),
        existing_predicate=dict(source='cards/ice_age.c',name='target_only_permanents_unless_in_hand',body_sha256=sha(predicate.encode()),address='23ede20',machine_bytes=len(predicate_code),machine_sha256=sha(predicate_code),raw_sha256=sha(support.encode())),
        vla=dict(element_bytes=8,extra_elements=1,upper_bound=[145,8,6]),target_layout=dict(bytes=84,frame_offset=-104,flags=0xc000),
        prompt=dict(address='2749380',bytes=prompt.hex()),
        scope='Native Oath life-loss callback using the original source control flow with the native 84-byte target APIs. Verify the amount-plus-one target VLA and unchanged private predicate; preserve failed-selection index advancement, all-target unmarking and native discard/sacrifice ordering.',
        integration_pending='Earlier manually reconstructed consumers, broader callback/lifecycle integration, native linking and gameplay verification remain pending.')
    targets={int(f['address'],16):f['name'] for f in manifest['accepted_functions']}
    targets.update({int(a,16):n for a,n in names.items()})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    bodies,entries={},[]
    for address,name in names.items():
        raw=(directories[address]/(address+'.c')).read_text();text=strip_comments(raw)
        proof['enum_values'].update({n:enums[n] for n in set(re.findall(r'\b(?:EVENT|AL|COUNTER|RESOLVE_TRIGGER)_\w+\b',text)) if n in enums})
        replacement='void '+template
        changes=[dict(original=text,replacement=replacement,occurrences=1,kind='verified original callback control flow and native VLA/layout reconstruction',machine_sites=controls)]
        text=replacement
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
            supporting_exports=[dict(address='23ede20',raw_sha256=sha(support.encode()),raw_export='decompiled-abi/23ede20.c')],
            oath_life_loss_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    sources={s:read(root/s) for s in {e['source'] for e in entries}}
    for source in sources:
        sources[source]+='\n\n#ifndef SHANDALAR\n/* Recovered October 2022 Oath life-loss callback. */\n'
        for e in entries:
            if e['source']==source:sources[source]+=f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n"+bodies[e['address']]+'\n'
        sources[source]+='#endif\n'
    loss=next(f for f in manifest['accepted_functions'] if f['address']=='268f1b0')
    loss_source=read(root/loss['source']);before=function_body(loss_source,loss['name']);after,count=rename(before,{'oath_of_lim_dul_lifeloss_trigger':'oath_of_lim_dul_lifeloss_trigger_2022'})
    assert count==1 and loss_source.count(before)==1
    loss_native=runtime.pe.get_data(0x268f1b0-0x2000000,0x580)
    calls=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target='24040a0',name='oath_of_lim_dul_lifeloss_trigger_2022') for i in decoder.disasm(loss_native,0x268f1b0) if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==0x24040a0]
    assert len(calls)==1 and calls[0]['address']=='268f5c1'
    proof['loss_binding']=dict(address=loss['address'],source=loss['source'],name=loss['name'],previous_record=copy.deepcopy(loss),previous_body_sha256=sha(before.encode()),body_sha256=sha(after.encode()),calls={'oath_of_lim_dul_lifeloss_trigger':'oath_of_lim_dul_lifeloss_trigger_2022'},canonical_calls=calls)
    sources[loss['source']]=loss_source.replace(before,after)
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+'/* October 2022 Oath life-loss callback API. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if not e['static'])+'\n#endif\n\n'+header[at:]
    scratch=cache/'oath-loss-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'))
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
    if '23ede20' not in existing:rows.append(focused['23ede20'])
    if args.apply:
        citations=manifest['source_fragments']+[s for f in manifest['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not any(c['address'] in names for c in citations)
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        updated=copy.deepcopy(loss['life_loss_evidence']);updated['canonical_calls']=sorted(updated['canonical_calls']+calls,key=lambda c:int(c['address'],16))
        loss['life_loss_evidence']=updated;interfaces['life_loss_2022']['functions']['268f1b0']=updated
        loss['oath_loss_binding_2022']={k:v for k,v in proof['loss_binding'].items() if k!='previous_record'}
        loss['reconstruction_body_sha256']=proof['loss_binding']['body_sha256']
        interfaces['life_loss_2022']['integration_pending']=proof['integration_pending']
        manifest['accepted_functions']+=entries;manifest['accepted']+=1;manifest['helper_functions']+=1
        manifest['shared_helper_interfaces']+=1
        for e in entries:manifest['source_files'][e['source']]=manifest['source_files'].get(e['source'],0)+1
        manifest['raw_export_entries']=len(rows);manifest['exported_functions']=len({r['name'] for r in rows})
        mp.write_text(json.dumps(manifest,indent=2));interfaces['oath_life_loss_2022']=proof;ip.write_text(json.dumps(interfaces,indent=2))
        with export_path.open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');writer.writeheader();writer.writerows(rows)
        for a in names:shutil.copyfile(directories[a]/(a+'.c'),export_path.parent/(a+'.c'))
        shutil.copyfile(cache/'decompiled-oath-loss/23ede20.c',export_path.parent/'23ede20.c')
    (cache/'oath-loss-plan.json').write_text(json.dumps(proof,indent=2))
    print(json.dumps(dict(applied=args.apply,new_helpers=1,private_helpers=0,public_helpers=1,verified_existing_private_predicates=1,coordinated_loss_callback_bindings=1)))


if __name__=='__main__':main()
