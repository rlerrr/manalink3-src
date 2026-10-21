"""Recover four blocked native helpers using concrete field and parameter evidence."""
import argparse,csv,hashlib,json,re,shutil,subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from elftools.dwarf.descriptions import describe_DWARF_expr
from elftools.dwarf.locationlists import LocationParser,LocationExpr,BaseAddressEntry
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import clean,strip_comments,UNRESOLVED
from recover_test_consumers import read,format_c
from audit_source import function_body

ADDRESSES='26d4f30 26a8080 21c7e40 25a22a0'.split()
PRIVATE={'25a22a0'}
SITES='26d4f5c 26d4f6a 26d4f78 26a80e9 26a80ef 26a80f5 26a8104 26a8109 26a810e 21c7ec0 25a22a0 25a22ad 25a22b2 25a246b 25aca41'.split()
REPAIRS={
 '26d4f30':[
  ('*(char *)(t_player * 4 + 0x4ef532)','ENERGY_COUNTERS(t_player)','26d4f5c'),
  ('(&player_counters)[t_player * 4]','POISON_COUNTERS(t_player)','26d4f6a'),
  ('*(char *)(t_player * 4 + 0x4ef531)','EXPERIENCE_COUNTERS(t_player)','26d4f78')],
 '26a8080':[
  ('_player_bits = _player_bits | 0x8000000;',
   'player_bits[0] = (player_bits_t)((uint32_t)player_bits[0] | 0x8000000U);','26a80ef'),
  ('_DAT_004ef1a8 = _DAT_004ef1a8 | 0x8000000;',
   'player_bits[1] = (player_bits_t)((uint32_t)player_bits[1] | 0x8000000U);','26a8109')],
 '21c7e40':[('check_state((int)cVar1,iVar2,0x800000)','check_state((int)cVar1,iVar2,STATE_OUBLIETTED)','21c7ec0')],
 '25a22a0':[]}


def sha(data):return hashlib.sha256(data).hexdigest()


def main():
    ap=argparse.ArgumentParser(__doc__);ap.add_argument('--apply',action='store_true');args=ap.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';directory=cache/'decompiled-scalar-frontier'
    mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'dependency_repairs_2022' not in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);dw=debug.dwarf();lp=LocationParser(dw.location_lists())
    parameters={};locals_={};symbols={a:[] for a in ADDRESSES}
    for name,offset,section,kind,storage in debug.functions():
        if section>0:
            address=f'{debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[section-1].VirtualAddress+offset:x}'
            if address in symbols:symbols[address].append(dict(name=name,storage=storage))
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {meta[a]['source'] for a in ADDRESSES}:continue
        def location(die):
            attr=die.attributes.get('DW_AT_location')
            if attr is None:return None
            loc=lp.parse_from_attribute(attr,cu['version'],die=die)
            if isinstance(loc,LocationExpr):return describe_DWARF_expr(loc.loc_expr,dw.structs)
            return [dict(base=hex(x.base_address)) if isinstance(x,BaseAddressEntry) else dict(start=hex(x.begin_offset),end=hex(x.end_offset),expression=describe_DWARF_expr(x.loc_expr,dw.structs)) for x in loc]
        def variables(die):
            result=[]
            for p in die.iter_children():
                if p.tag=='DW_TAG_variable':result.append(dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p)))
                elif p.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:result+=variables(p)
            return result
        for die in cu.get_top_DIE().iter_children():
            pc=value(die,'DW_AT_low_pc');address=f'{pc:x}' if pc else ''
            if address not in ADDRESSES:continue
            parameters[address]=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p)) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter']
            locals_[address]=variables(die)
    assert [p['name'] for p in parameters['25a22a0']]==['player','amount','card']
    assert [p['location'][0]['expression'] for p in parameters['25a22a0'][:2]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))']
    assert 'DW_OP_GNU_parameter_ref' in parameters['25a22a0'][2]['location'][0]['expression']
    assert any(p['name']=='counter_types' and p['type']=='counter_t[9]' for p in locals_['26d4f30'])
    image=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    natives={a:image.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)) for a in ADDRESSES+['25a2340','25ac8e0']}
    instructions={f'{x.address:x}':x for a,native in natives.items() for x in decoder.disasm(native,int(a,16))}
    controls=[dict(address=a,bytes=instructions[a].bytes.hex(),mnemonic=instructions[a].mnemonic,operands=instructions[a].op_str) for a in SITES]
    overlap=[r.split('\t')[1] for r in (cache/'absolute-interfaces.tsv').read_text().splitlines() if r.split('\t')[2]=='global']
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']};targets.update({int(a,16):meta[a]['name'] for a in ADDRESSES})
    proof=dict(debug_sha256=sha(debug_data),parameters=parameters,locals=locals_,symbols=symbols,control_sites=controls,functions={},
        player_counters=dict(address='4ef530',bytes=8,stride=4,fields=dict(poison_=0,experience_=1,energy_=2)),
        player_bits=dict(address='4ef1a4',bytes=8,stride=4,legend_rule_mask=0x8000000),
        miara_physical=['player','amount'],miara_nonphysical=['card'],
        scope='Native counter-type enumeration, player legend-rule suppression, end-turn bounce and Miara payment worker. Preserve native bounds, mode bits and source ownership; integration and gameplay checks remain pending.')
    sources={};entries=[];header=read(root/'manalink.h')
    for a in ADDRESSES:
        row=meta[a];name=row['name'];source=row['source'];raw=(directory/(a+'.c')).read_text();text=strip_comments(raw);changes=[]
        for original,replacement,site in REPAIRS[a]:
            assert text.count(original)==1,(a,original);text=text.replace(original,replacement)
            changes.append(dict(original=original,replacement=replacement,occurrences=1,kind='verified native field or state operand',machine_sites=[next(c for c in controls if c['address']==site)]))
        body=clean(text,overlap,use_keyword_accessors_2022=True,use_extension_flags_2022=True)
        assert not UNRESOLVED.search(body) and '::' not in body
        if a in PRIVATE:body='static '+body
        body=format_c(body);calls=[]
        for x in decoder.disasm(natives[a],int(a,16)):
            if x.mnemonic in {'call','jmp'} and x.operands[0].type==X86_OP_IMM and x.operands[0].imm in targets:
                target=targets[x.operands[0].imm];assert re.search(r'\b'+target+r'\s*\(',body),(a,target)
                calls.append(dict(address=f'{x.address:x}',bytes=x.bytes.hex(),target=f'{x.operands[0].imm:x}',name=target))
        detail=dict(machine_bytes=len(natives[a]),machine_sha256=sha(natives[a]),changes=changes,canonical_calls=calls);proof['functions'][a]=detail
        prototype=body[:body.index('{')].strip()+';'
        entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=prototype,raw_sha256=sha(raw.encode()),recovered_literals=[],dependency_repair_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
        sources.setdefault(source,read(root/source));sources[source]+=f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    declarations='/* October 2022 counter, legend and delayed-return helpers. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if not e['static'])+'\n#endif\n\n'
    at=header.rfind('#ifdef __cplusplus');header=header[:at]+declarations+header[at:]
    scratch=cache/'dependency-repair-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'))
    for source,text in sources.items():
        path=scratch/Path(source).name;path.write_bytes(text.encode('latin1'))
        result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(path)],capture_output=True,text=True)
        path.with_suffix('.log').write_text(result.stderr)
        if result.returncode:print(result.stderr[-7000:])
        result.check_returncode()
    if args.apply:
        refreshed={r['address'] for r in csv.DictReader((directory/'export.tsv').open(),delimiter='\t')}
        citations=m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not refreshed&{f['address'] for f in m['accepted_functions']} and not refreshed&{f['address'] for f in citations}
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')}
        for r in csv.DictReader((directory/'export.tsv').open(),delimiter='\t'):
            assert r['status']=='ok';rows[r['address']]=r;shutil.copyfile(directory/(r['address']+'.c'),cache/'decompiled-abi'/(r['address']+'.c'))
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        m['accepted_functions']+=entries;m['accepted']=len(m['accepted_functions']);m['helper_functions']+=4
        m['private_functions']+=1;m['private_helpers']+=1;m['shared_helper_interfaces']+=3
        for entry in entries:m['source_files'][entry['source']]=m['source_files'].get(entry['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()})
        mp.write_text(json.dumps(m,indent=2));interfaces['dependency_repairs_2022']=proof;ip.write_text(json.dumps(interfaces,indent=2))
    (cache/'dependency-repair-plan.json').write_text(json.dumps(proof,indent=2));print(json.dumps(dict(applied=args.apply,functions=4,public_helpers=3,private_helpers=1)))


if __name__=='__main__':main()
