"""Recover the until-upkeep exile worker and expose retained transform callbacks."""
import argparse,csv,hashlib,json,re,shutil,subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from elftools.dwarf.locationlists import LocationParser,LocationExpr,BaseAddressEntry
from elftools.dwarf.descriptions import describe_DWARF_expr
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import clean,strip_comments,UNRESOLVED
from recover_test_consumers import read,format_c
from audit_source import function_body

ADDRESS='2685a40'
EXPOSURES={'20d9140':('return_to_bf_transformed_impl','static void\n','void\n'),
           '20d9fd0':('exile_and_return_to_bf_transformed','static void ','void ')}
HEADER_LABEL='/* October 2022 temporary-exile worker and retained transform callbacks. */'
CONTROL='2685a9f 2685aad 2685ab4 2685ab7 2685ac9 2685ad5 2685ae9 2685b20 2685b23 2685b31 2685b7a 2685b7f 2685b82 2685b89 2685b8b 2685baf 2685bda 2685be0 2685be3 2685bed 2685bf3 2685bf7'.split()
PACKED_ORIGINAL='uVar2._0_2_ = cards_data[card_00].id;\n    uVar2._2_2_ = cards_data[card_00].reserved2;'
PACKED_REPLACEMENT='uVar2 = (uint)cards_data[card_00].id | ((uint)cards_data[card_00].reserved2 << 16);'


def sha(data):return hashlib.sha256(data).hexdigest()


def save_json(path,data):
    temporary=path.with_suffix('.tmp')
    temporary.write_text(json.dumps(data,indent=2))
    assert json.loads(temporary.read_text())==data
    temporary.replace(path)


def audit_component(component,manifest,sources,machine_bytes,root):
    def require(ok,message):
        if not ok:raise ValueError(message)
    entries=[f for f in manifest['accepted_functions'] if f.get('temporary_exile_evidence')]
    require(len(entries)==1 and entries[0]['address']==ADDRESS and not entries[0]['static'],'Temporary exile worker membership/linkage mismatch')
    entry=entries[0];detail=component['function'];require(entry['temporary_exile_evidence']==detail,'Temporary exile worker evidence mismatch')
    require([p['name'] for p in component['parameters']]==['player','card','t_player','t_card','whose_upkeep','discard_cards_equal_to_toughness'] and
            [p['location'] for p in component['parameters']]==[f'(DW_OP_fbreg: {n})' for n in range(0,24,4)],'Temporary exile physical parameters mismatch')
    require(any(v['name']=='csvids' and v['type']=='int[2]' and v['location']=='(DW_OP_fbreg: -28)' for v in component['locals']),
            'Temporary exile meld-pair local mismatch')
    require(component['card_data']==dict(address='7e7010',bytes=72,id_offset=36,reserved2_offset=38,field_bytes=2) and
            component['picture_fields']==dict(id_offset=100,number_offset=102,bytes=2),'Temporary exile packed ID/picture layout mismatch')
    require(component['meld_ids']==[0x3eae,0x3ef4,0x3efb] and component['discard_markers']==dict(disabled=-1,enabled=66),'Temporary exile meld/discard markers mismatch')
    require(len(detail['changes'])==2 and detail['changes'][0]['original']==PACKED_ORIGINAL and detail['changes'][0]['replacement']==PACKED_REPLACEMENT and
            detail['changes'][1]['original']=='(undefined2)uVar2' and detail['changes'][1]['replacement']=='(uint16_t)uVar2','Temporary exile packed projection mismatch')
    require([s['address'] for s in component['control_sites']]==CONTROL,'Temporary exile native control membership mismatch')
    for site in component['control_sites']:
        data=bytes.fromhex(site['bytes']);require(machine_bytes(int(site['address'],16),len(data))==data,'Temporary exile native control/store instruction mismatch')
    require([s['bytes'] for s in component['control_sites'][:2]]==['8d04db','8b04c534707e00'],'Temporary exile native 72-byte indexed packed-ID load mismatch')
    require(detail['function_references']==[dict(address='2685baf',bytes='c744240800f06902',target='269f000',name='remove_until_upkeep_legacy')],
            'Temporary exile return callback binding mismatch')
    body=function_body(sources[entry['source']],entry['name'])
    require('remove_until_upkeep_legacy' in body and 'display_pic_csv_id = (uint16_t)' in body and 'int csvids[2];' in body,
            'Temporary exile callback/picture/meld source projection missing')
    header=read(root/'manalink.h')
    require(set(component['exposures'])==set(EXPOSURES),'Retained transform exposure membership mismatch')
    for address,exposure in component['exposures'].items():
        require(exposure['storage']==2 and exposure['source']=='cards/origins.c','Retained transform public symbol mismatch')
        source=sources[exposure['source']].replace('\r\n','\n')
        require(exposure['new_signature'] in source and exposure['old_signature'] not in source and
                exposure['prototype'] in header,'Retained transform public declaration mismatch')
        require(sha(function_body(source,exposure['name']).encode())==exposure['body_sha256'],
                'Retained transform source body changed')
        require(sha(machine_bytes(int(address,16),exposure['machine_bytes']))==exposure['machine_sha256'],'Retained transform native hash mismatch')
        require(sha(read(root/'.recovery/decompiled-abi'/f'{address}.c').encode())==exposure['raw_sha256'],'Retained transform raw export mismatch')
    require(machine_bytes(0x20d9fd9,8).hex()=='c744240840910d02','Retained blink constructor callback binding mismatch')
    require(re.search(r'_cards_data\s*=\s*0x7E7010;',read(root/'manalink.lds')),'Temporary exile card-data linker binding mismatch')


def audit_transform(component,manifest,sources,machine_bytes):
    def require(ok,message):
        if not ok:raise ValueError(message)
    entries=[f for f in manifest['accepted_functions'] if f.get('transform_cleanup_evidence')]
    require(len(entries)==1 and entries[0]['address']=='22128b0' and entries[0]['static'],'Transform cleanup membership/linkage mismatch')
    detail=component['function'];require(entries[0]['transform_cleanup_evidence']==detail,'Transform cleanup evidence mismatch')
    require(detail['changes']==[dict(original='0x800000',replacement='STATE_OUBLIETTED',occurrences=1,
        kind='native phased-out state argument',machine_sites=[dict(address='221290d',bytes='c744240800008000')])],
        'Transform cleanup native state argument mismatch')
    require(detail['function_references']==[dict(address='221292c',bytes='c744240840910d02',target='20d9140',name='return_to_bf_transformed_impl')],
        'Transform cleanup return callback mismatch')
    for site in detail['function_references']:
        data=bytes.fromhex(site['bytes']);require(machine_bytes(int(site['address'],16),len(data))==data,'Transform cleanup callback instruction mismatch')
    body=function_body(sources[entries[0]['source']],entries[0]['name'])
    require('blink_effect_controller' in body and 'return_to_bf_transformed_impl' in body and 'STATE_OUBLIETTED' in body,
            'Transform cleanup source binding missing')


def recover_transform(root,apply):
    cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'temporary_transform_cleanup_2022' not in interfaces and 'temporary_exile_2022' in interfaces
    address='22128b0';assert address not in {f['address'] for f in m['accepted_functions']}
    row=next(r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc']=='0x'+address)
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());native=runtime.pe.get_data(int(address,16)-0x2000000,int(row['high_pc'],16)-int(address,16))
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;instructions=list(decoder.disasm(native,int(address,16)))
    raw=read(cache/'decompiled-abi'/f'{address}.c');text=strip_comments(raw);assert text.count('0x800000')==1
    text=text.replace('0x800000','STATE_OUBLIETTED')
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    body=format_c('static '+clean(text,overlap,use_keyword_accessors_2022=True,use_extension_flags_2022=True))
    assert not UNRESOLVED.search(body)
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    symbols=[s for n,o,sec,k,s in debug.functions() if debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+o==int(address,16)];assert symbols==[3]
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']};calls=[]
    for ins in instructions:
        if ins.mnemonic in {'call','jmp'} and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm in targets:
            target=targets[ins.operands[0].imm];assert re.search(r'\b'+target+r'\s*\(',body),target
            calls.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{ins.operands[0].imm:x}',name=target))
    detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=[dict(original='0x800000',replacement='STATE_OUBLIETTED',occurrences=1,
        kind='native phased-out state argument',machine_sites=[dict(address='221290d',bytes='c744240800008000')])],canonical_calls=calls,
        function_references=[dict(address='221292c',bytes='c744240840910d02',target='20d9140',name='return_to_bf_transformed_impl')])
    proof=dict(function=detail,scope='Native end-combat transform callback, retaining live/phase guards, controller-preserving blink and legacy cleanup.',
               integration_pending='Native linking, deeper blink/transform comparison and gameplay verification remain pending.')
    entry=dict(address=address,name=row['name'],source=row['source'],line=row['line'],static=True,prototype=body[:body.index('{')].strip()+';',
        raw_sha256=sha(raw.encode()),recovered_literals=[],transform_cleanup_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,row['name']).encode()))
    source=read(root/row['source'])+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{address}; original {row["source"]}:{row["line"]}. */\n'+body+'\n#endif\n'
    scratch=cache/'temporary-exile-check';path=scratch/'ixalan.c';path.write_bytes(source.encode('latin1'))
    result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0',
        '-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format',
        '-I',str(root),str(path)],capture_output=True,text=True);path.with_suffix('.log').write_text(result.stderr)
    if result.returncode:print(result.stderr[-6000:])
    result.check_returncode();audit_transform(proof,{**m,'accepted_functions':m['accepted_functions']+[entry]},{row['source']:source},lambda a,n:runtime.pe.get_data(a-0x2000000,n))
    if apply:
        (root/row['source']).write_bytes(source.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions'].append(entry);m['accepted']+=1;m['helper_functions']+=1;m['private_functions']+=1;m['private_helpers']+=1;m['source_files'][row['source']]+=1
        save_json(mp,m);interfaces['temporary_transform_cleanup_2022']=proof;save_json(ip,interfaces)
    (cache/'temporary-transform-plan.json').write_text(json.dumps(proof,indent=2));print(json.dumps(dict(applied=apply,new_private_helpers=1)))


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--transform',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2]
    if args.transform:return recover_transform(root,args.apply)
    cache=root/'.recovery';directory=cache/'decompiled-temporary-exile'
    mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'temporary_exile_2022' not in interfaces
    assert not {ADDRESS,*EXPOSURES}&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);dw=debug.dwarf();lp=LocationParser(dw.location_lists())
    proof=dict(debug_sha256=sha(debug_data),parameters=[],locals=[],control_sites=[],exposures={},
        card_data=dict(address='7e7010',bytes=72,id_offset=36,reserved2_offset=38,field_bytes=2),picture_fields=dict(id_offset=100,number_offset=102,bytes=2),
        meld_ids=[0x3eae,0x3ef4,0x3efb],discard_markers=dict(disabled=-1,enabled=66),
        scope='Native temporary-exile constructor, including meld pairs, owner and display fields, and discard-on-return marker. Recombine two adjacent unsigned 16-bit card-data fields into the original packed 32-bit value. Expose two retained Origins transform helpers without counting their bodies again.',
        integration_pending='Native linking, deeper blink/transform comparison and gameplay verification remain pending.')
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/functions.c':continue
        def location(die):
            at=die.attributes.get('DW_AT_location')
            if at is None:return None
            loc=lp.parse_from_attribute(at,cu['version'],die=die)
            if isinstance(loc,LocationExpr):return describe_DWARF_expr(loc.loc_expr,dw.structs)
            return [dict(base=hex(x.base_address)) if isinstance(x,BaseAddressEntry) else dict(start=hex(x.begin_offset),end=hex(x.end_offset),expression=describe_DWARF_expr(x.loc_expr,dw.structs)) for x in loc]
        def variables(die):
            result=[]
            for v in die.iter_children():
                if v.tag=='DW_TAG_variable':result.append(dict(name=inherited_value(v,'DW_AT_name'),type=type_name(type_of(v)),location=location(v)))
                elif v.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:result+=variables(v)
            return result
        for die in cu.get_top_DIE().iter_children():
            if value(die,'DW_AT_low_pc')==int(ADDRESS,16):
                proof['parameters']=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p)) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter']
                proof['locals']=variables(die)
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    natives={a:runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)) for a in [ADDRESS,*EXPOSURES]}
    instructions=list(decoder.disasm(natives[ADDRESS],int(ADDRESS,16)));by_site={f'{i.address:x}':i for i in instructions}
    proof['control_sites']=[dict(address=a,bytes=by_site[a].bytes.hex(),instruction=by_site[a].mnemonic+' '+by_site[a].op_str) for a in CONTROL]
    for address in [ADDRESS,*EXPOSURES]:
        symbols=[s for n,o,sec,k,s in debug.functions() if debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+o==int(address,16)];assert symbols==[2]
    raw=read(directory/f'{ADDRESS}.c');text=strip_comments(raw);changes=[]
    for original,replacement,sites in [(PACKED_ORIGINAL,PACKED_REPLACEMENT,['2685aad','2685ab4','2685ab7']),
                                      ('(undefined2)uVar2','(uint16_t)uVar2',['2685be6','2685bf3'])]:
        assert text.count(original)==1;text=text.replace(original,replacement)
        changes.append(dict(original=original,replacement=replacement,occurrences=1,kind='native packed card-ID or picture-width reconstruction',
            machine_sites=[dict(address=a,bytes=by_site[a].bytes.hex()) for a in sites]))
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    body=format_c(clean(text,overlap,use_keyword_accessors_2022=True,use_extension_flags_2022=True))
    assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x',body)
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']};calls=[]
    for ins in instructions:
        if ins.mnemonic in {'call','jmp'} and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm in targets:
            target=targets[ins.operands[0].imm];assert re.search(r'\b'+target+r'\s*\(',body),target
            calls.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{ins.operands[0].imm:x}',name=target))
    detail=dict(machine_bytes=len(natives[ADDRESS]),machine_sha256=sha(natives[ADDRESS]),changes=changes,canonical_calls=calls,
                function_references=[dict(address='2685baf',bytes=by_site['2685baf'].bytes.hex(),target='269f000',name='remove_until_upkeep_legacy')])
    proof['function']=detail;row=meta[ADDRESS]
    entry=dict(address=ADDRESS,name=row['name'],source=row['source'],line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),
        recovered_literals=[],temporary_exile_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,row['name']).encode()))
    sources={row['source']:read(root/row['source']),'cards/origins.c':read(root/'cards/origins.c')}
    sources[row['source']]+=f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{ADDRESS}; original {row["source"]}:{row["line"]}. */\n'+body+'\n#endif\n'
    for address,(name,old_prefix,new_prefix) in EXPOSURES.items():
        old_signature=old_prefix+name+'(int player, int card)';new_signature=new_prefix+name+'(int player, int card)'
        assert sources['cards/origins.c'].count(old_signature)==1
        proof['exposures'][address]=dict(address=address,name=name,source='cards/origins.c',storage=2,old_signature=old_signature,new_signature=new_signature,
            prototype='void '+name+'(int player, int card);',body_sha256=sha(function_body(sources['cards/origins.c'],name).encode()),
            machine_bytes=len(natives[address]),machine_sha256=sha(natives[address]),raw_sha256=sha(read(directory/f'{address}.c').encode()))
        sources['cards/origins.c']=sources['cards/origins.c'].replace(old_signature,new_signature,1)
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+entry['prototype']+'\n'+'\n'.join(e['prototype'] for e in proof['exposures'].values())+'\n#endif\n\n'+header[at:]
    scratch=cache/'temporary-exile-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'))
    for source,text in sources.items():
        path=scratch/Path(source).name;path.write_bytes(text.encode('latin1'))
        result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0',
            '-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format',
            '-I',str(scratch),'-I',str(root),str(path)],capture_output=True,text=True);path.with_suffix('.log').write_text(result.stderr)
        if result.returncode:print(result.stderr[-6000:])
        result.check_returncode()
    if args.apply:
        focused=list(csv.DictReader((directory/'export.tsv').open(),delimiter='\t'));addresses={r['address'] for r in focused}
        citations=m['source_fragments']+m['accepted_functions']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not addresses&{c['address'] for c in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')}
        for r in focused:rows[r['address']]=r;shutil.copyfile(directory/f'{r["address"]}.c',cache/'decompiled-abi'/f'{r["address"]}.c')
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        m['accepted_functions'].append(entry);m['accepted']+=1;m['helper_functions']+=1;m['shared_helper_interfaces']+=1;m['source_files'][row['source']]+=1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()})
        save_json(mp,m);interfaces['temporary_exile_2022']=proof;save_json(ip,interfaces)
    (cache/'temporary-exile-plan.json').write_text(json.dumps(proof,indent=2));print(json.dumps(dict(applied=args.apply,new_helpers=1,exposed_existing_helpers=2)))


if __name__=='__main__':main()
