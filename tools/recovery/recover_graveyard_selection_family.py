"""Recover native graveyard selection, dynamic arrays and five card handlers."""
import argparse,csv,json,re,shutil,struct
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_REG_EBP,X86_REG_ESP
from inspect_release import Image,value,inherited_value
from export_abi import type_of
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read,rename
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings,readonly_string,byte_literal
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input

BREACH='2359bb0';UNDERWORLD='2361fd0';CHAINER='23b1850';CHAINER_CARD='23b26b0';LILIANA='22063c0';LILIANA_CARD='2209a10';GENESIS='2289940';GYRUS='2562b60';EXILE_COMBAT='2560470'
ADDRESSES=[BREACH,UNDERWORLD,CHAINER,CHAINER_CARD,LILIANA,LILIANA_CARD,GENESIS,EXILE_COMBAT,GYRUS]
HELPERS={BREACH,CHAINER,LILIANA,EXILE_COMBAT};PRIVATE=HELPERS
NAMES={'escape':'escape_part_2022'};LEGACY=[];LABEL='';ALIASES={}
FIXED={BREACH:{'2359c06':'b8b80b0000','2359c11':'e8ba253a00','2359c16':'29c4','2359c18':'8d44241c','2359c2a':'c7442404b80b0000','2359bde':'f604bda7f14e0002','2359c83':'f604bda7f14e0002','2359cd4':'f684c5d9fcffff20'},
 CHAINER:{'23b1968':'8d04c500000000','23b196f':'e85ca83400','23b1974':'29c4','23b1976':'81e2ffffff3f','23b1986':'8d442420','23b1a6e':'c7442408105e7402','23b1b01':'01c6','23b1b03':'8b04b7','23b1b06':'895c2404','23b1b0a':'89442408'},
 LILIANA:{'2206546':'8d148500000000','220654d':'c1e003','2206550':'e87b5c4f00','2206555':'29c4','2206557':'8d442420','2206751':'c744240801000000','2206759':'897c2404','220675d':'891c24','2206760':'e8db244600'},
 GENESIS:{'22899ee':'83f805','22899f7':'8d04b500000000','2289a04':'e8c7274700','2289a09':'29c4','2289a59':'668975de','2289ac0':'89442404','2289b48':'be05000000'},
 GYRUS:{'2562ca5':'e8963d1300','2562cb2':'8944240c','2562caa':'c7442408843f7502','2562d1f':'8d0cc500000000','2562d26':'8d148500000000','2562d2f':'e89c941900','2562d34':'29c4','2562d4b':'f3ab','2562df1':'891c81','2562e03':'85ff','2562e05':'0f84b5000000','2562e9b':'66c785f9feffff0101','2562ea4':'c7850cffffff70045602','2562eae':'c78500ffffff03000000'}}


def native_storage(debug):
    from export_abi import type_name
    arrays={};enums={}
    def walk(d):
        for x in d.iter_children():
            yield x
            if x.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:yield from walk(x)
    for cu in debug.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in {'cards/theros_beyond_death.c','cards/commander_2019.c','cards/m19.c','cards/commander_2018.c','cards/ikoria.c'}:continue
        for d in cu.get_top_DIE().iter_children():
            if d.tag=='DW_TAG_enumeration_type':
                for x in d.iter_children():
                    if value(x,'DW_AT_name') in {'PB_GY_ABILITIES_DISABLED','TRIGGER_COMES_INTO_PLAY','TOKEN_ACTION_ATTACKING'}:enums[value(x,'DW_AT_name')]=value(x,'DW_AT_const_value')
            a=f"{value(d,'DW_AT_low_pc',0):x}"
            if a not in {BREACH,CHAINER,LILIANA,GENESIS,GYRUS}:continue
            unit={}
            for v in walk(d):
                name=inherited_value(v,'DW_AT_name')
                if v.tag!='DW_TAG_variable' or name not in {'playable','added','opts','token'}:continue
                t=type_of(v)
                while t.tag=='DW_TAG_typedef':t=type_of(t)
                if name=='token':assert value(t,'DW_AT_byte_size')==76;unit[name]=dict(bytes=76);continue
                assert t.tag=='DW_TAG_array_type'
                dims=[]
                for child in t.iter_children():
                    if child.tag!='DW_TAG_subrange_type':continue
                    bound=child.attributes.get('DW_AT_upper_bound');dims.append(None if bound is None else dict(form=bound.form,value=bound.value))
                unit[name]=dict(element=type_name(type_of(t)),element_bytes=value(type_of(t),'DW_AT_byte_size'),dimensions=dims)
            arrays[a]=unit
    for a in [CHAINER,LILIANA,GYRUS]:assert arrays[a]['playable']['element']=='int' and arrays[a]['playable']['element_bytes']==4 and arrays[a]['playable']['dimensions'][0]==dict(form='DW_FORM_data1',value=1)
    assert arrays[CHAINER]['playable']['dimensions'][1]['form']=='DW_FORM_ref4'
    assert arrays[LILIANA]['playable']['dimensions'][1]['form']=='DW_FORM_ref4'
    assert arrays[GYRUS]['playable']['dimensions'][1]['form']=='DW_FORM_exprloc'
    assert arrays[GENESIS]['added']['dimensions']==[None] and arrays[GENESIS]['added']['element_bytes']==4
    assert arrays[BREACH]['opts']['dimensions']==[dict(form='DW_FORM_data1',value=99),dict(form='DW_FORM_data1',value=1)]
    assert enums==dict(PB_GY_ABILITIES_DISABLED=33554432,TRIGGER_COMES_INTO_PLAY=219,TOKEN_ACTION_ATTACKING=3)
    return dict(arrays=arrays,enums=enums)


def reconstruct(a,raw,runtime,meta):
    from graveyard_selection_bodies import selection_callback,GENESIS as GENESIS_BODY,GYRUS as GYRUS_BODY
    text=strip_comments(raw);changes=[];pairs=[];sites=[dict(address=at,bytes=b) for at,b in FIXED.get(a,{}).items()]
    if a in {CHAINER,LILIANA,GENESIS,GYRUS}:
        body=selection_callback(raw,a==LILIANA) if a in {CHAINER,LILIANA} else GENESIS_BODY if a==GENESIS else GYRUS_BODY
        changes=[dict(original=text,replacement=body,occurrences=1,kind='native dynamic-stack and concrete call-frame reconstruction',machine_sites=sites,machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=int(meta[a]['high_pc'],16)-int(a,16),machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16))))]
        if a in {CHAINER,LILIANA}:
            literal_at=0x2745e10 if a==CHAINER else 0x2734fef;literal=readonly_string(runtime,literal_at)
            changes[0].update(native_literal=dict(address=f'{literal_at:x}',bytes=(literal+b'\0').hex(),copy_bytes=len(literal)+1),literal=literal.decode())
        return body,changes
    if a==BREACH:
        decoder=Cs(CS_ARCH_X86,CS_MODE_32);code=runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16));calls={i.address+i.size:i for i in decoder.disasm(code,int(a,16)) if i.mnemonic=='call'}
        pairs=[('  char acStack_f10 [2968];\n',''),('  undefined4 uStack_378;\n','')]
        for match in re.finditer(r'(?m)^ +uStack_378 = 0x([0-9a-f]+);\n',text):
            ins=calls[int(match[1],16)];sites.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex()));pairs.append((match[0],''))
        pairs.extend([('else if (event == EVENT_ACTIVATE) {','else if (event == EVENT_ACTIVATE) {\n    char buffer[3000];'),('acStack_f10','buffer'),('(*(byte *)(player * 4 + 0x4ef1a7) & 2)','(player_bits[player] & PB_GY_ABILITIES_DISABLED)'),('(*(byte *)((int)opts[iVar4] + 5) & 0x20)','(opts[iVar4][1] & 0x2000U)')])
    if a==CHAINER_CARD:pairs=[('_trigger_condition == 0xdb','trigger_condition == TRIGGER_COMES_INTO_PLAY')]
    for old,new in dict(pairs).items():
        count=text.count(old);assert count>0,(a,old)
        changes.append(dict(original=old,replacement=new,occurrences=count,kind='restore native conditional buffer, probe artifact or typed flag view',machine_sites=sites));text=text.replace(old,new)
    return text,changes


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('graveyard_selection_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert component['native_storage']==native_storage(Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()))
    for layout in component['unit_layouts'].values():assert layout['test_definition_t']['bytes']==216 and layout['target_definition_t']['bytes']==84
    assert any(v['name']=='opts' and v['type']=='int[100][2]' and v['location']=='(DW_OP_fbreg: -820)' for v in component['locals'][BREACH])
    assert any(v['name']=='cb' and 'DW_OP_const2u: 3000; DW_OP_stack_value' in str(v['location']) for v in component['locals'][BREACH])
    for a in [CHAINER,LILIANA,GYRUS]:assert any(v['name']=='playable' for v in component['locals'][a])
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['graveyard_selection_evidence']==detail and entry['static']==(a in PRIVATE)
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-graveyard-selection-family' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,{});assert literals==entry['recovered_literals'];text,_=rename(text,NAMES);expected=cleaned(text,overlap)
        if a in PRIVATE:expected='static '+expected
        assert function_body(expected,entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    assert len(entries)==9 and sum(e['name'].startswith('card_') for e in entries.values())==5


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(debug);(cache/'export-request-graveyard-selection-family.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared nine graveyard-selection family requests.');return
    assert 'graveyard_selection_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(debug),**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{'cards/m19.c','cards/commander_2019.c','cards/commander_2018.c','cards/ikoria.c'}),functions={},retained_bodies=[dict(source=s,name=n,body_sha256=sha(function_body(read(root/s),n).encode())) for s,n in LEGACY],
        scope='Recover three native private graveyard-selection callbacks, a private end-of-combat exile callback and five card handlers as a batch. Reconstruct three two-row VLA candidate/position tables and the Genesis selection VLA, preserve reloads and call order, native Liliana cast arguments, Gyrus scanned-count gate, formatting argument, token payload and source oddities. Restore Breach 3000-byte activation buffer, 100-by-2 choices and probe artifacts. Preserve all earlier records, globals, fragments, interfaces and dialog stubs; gameplay validation remains downstream.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES.get(meta[a]['name'],meta[a]['name']),source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-graveyard-selection-family'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);text,literals=recover_strings(text,runtime,ALIASES);text,_=rename(text,NAMES);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,graveyard_selection_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');compile_sources(root,cache/'graveyard-selection-family-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in [r for r in csv.DictReader((cache/'decompiled-graveyard-selection-family/export.tsv').open(),delimiter='\t') if r['address'] in ADDRESSES]:rows[row['address']]=row;shutil.copyfile(cache/'decompiled-graveyard-selection-family'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=9;m['card_handler_functions']+=5;m['helper_functions']+=4;m['private_functions']+=4;m['private_helpers']+=4
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['graveyard_selection_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'graveyard-selection-family-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=5,new_helpers=4)))


if __name__=='__main__':main()
