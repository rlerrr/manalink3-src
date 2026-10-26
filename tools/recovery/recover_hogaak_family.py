"""Recover Hogaak Convoke/Delve, its physical worker and native mana text."""
import argparse,csv,json,re,shutil,struct
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_REG_EBP,X86_REG_ESP
from capstone.x86_const import X86_OP_REG,X86_REG_ECX
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

PREDICATE='2335930';PAYMENT='2339600';CAN_CAST='23394f0';CAST='2339e70';CARD='233a1b0';FORMAT='2692630';SELECT='24e0b90'
ADDRESSES=[PREDICATE,CAN_CAST,PAYMENT,CAST,CARD,FORMAT,SELECT]
HELPERS=set(ADDRESSES)-{CARD};PRIVATE={PREDICATE}
NAMES={'has_convoked_mana_for_hogaak':'has_convoked_mana_for_hogaak_impl','mana_into_string':'mana_into_string_2022','will_tap_permanent_for_convoke':'will_tap_permanent_for_convoke_2022'}
LEGACY=[('functions/functions.c','mana_into_string'),('cards/ravnica.c','will_tap_permanent_for_convoke')];LABEL='/* October 2022 Hogaak Convoke/Delve and mana text APIs. */';ALIASES={}
FIXED={PREDICATE:{'2335948':'8d0c9500000000','2335951':'e87a683c00','2335956':'29c4','233595f':'8d0411','2335962':'8d0c8500000000','233596b':'e860683c00','2335970':'29c4','2335983':'f3ab','2335af8':'83f931','2335afb':'7f17'},
 PAYMENT:{'2339762':'e8c98e3500','2339767':'8d8528fdffff','233976d':'8944240c','2339771':'c7442408bd107402','233998a':'8d0c8500000000','2339993':'e838283c00','2339998':'29c4','23399af':'f3ab','2339b06':'8d0492','2339b09':'c1e002','2339b0c':'e8bf263c00','2339b11':'29c4','2339b4b':'83c014','2339b52':'c7855cfcffff32000000'},
 CAST:{'233a01d':'8d90b4000000','233a023':'83c06c','233a028':'8b0a','233a02a':'8b5a04','233a02d':'894a08','233a030':'895a0c','233a033':'83ea08','233a036':'39c2','233a038':'75ee'}}


def native_storage(debug):
    from export_abi import type_name
    arrays={};enums={}
    def walk(d):
        for x in d.iter_children():
            yield x
            if x.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:yield from walk(x)
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='cards/modern_horizons.c':continue
        for d in cu.get_top_DIE().iter_children():
            if d.tag=='DW_TAG_enumeration_type':
                for x in d.iter_children():
                    if value(x,'DW_AT_name') in {'STATE_TAPPED','TYPE_CREATURE'}:enums[value(x,'DW_AT_name')]=value(x,'DW_AT_const_value')
            a=f"{value(d,'DW_AT_low_pc',0):x}"
            if a not in {PREDICATE,PAYMENT}:continue
            unit={}
            for v in walk(d):
                name=inherited_value(v,'DW_AT_name')
                if v.tag!='DW_TAG_variable' or name not in {'tapped','tapped_for_color','clrs','mana','mana2','buffer'}:continue
                t=type_of(v)
                while t.tag=='DW_TAG_typedef':t=type_of(t)
                assert t.tag=='DW_TAG_array_type'
                dims=[]
                for child in t.iter_children():
                    if child.tag!='DW_TAG_subrange_type':continue
                    bound=child.attributes.get('DW_AT_upper_bound');dims.append(None if bound is None else dict(form=bound.form,value=bound.value))
                unit[name]=dict(element=type_name(type_of(t)),element_bytes=value(type_of(t),'DW_AT_byte_size'),dimensions=dims)
            arrays[a]=unit
    for a in [PREDICATE,PAYMENT]:
        assert arrays[a]['tapped']['element']=='int' and arrays[a]['tapped']['element_bytes']==4 and len(arrays[a]['tapped']['dimensions'])==1
        assert arrays[a]['tapped_for_color']['element']=='int' and arrays[a]['tapped_for_color']['element_bytes']==4 and len(arrays[a]['tapped_for_color']['dimensions'])==2
        assert arrays[a]['tapped_for_color']['dimensions'][1]==dict(form='DW_FORM_data1',value=4)
        assert arrays[a]['clrs']['dimensions']==[dict(form='DW_FORM_data1',value=5)]
    assert arrays[PAYMENT]['mana']['dimensions']==[dict(form='DW_FORM_data1',value=5)] and arrays[PAYMENT]['mana2']['dimensions']==[dict(form='DW_FORM_data1',value=9)]
    assert arrays[PAYMENT]['buffer']['dimensions']==[dict(form='DW_FORM_data2',value=499)] and arrays[PAYMENT]['buffer']['element_bytes']==1
    assert enums==dict(STATE_TAPPED=16,TYPE_CREATURE=2)
    return dict(arrays=arrays,enums=enums)


def predicate_cost_frames(runtime):
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    frame={};black=None;proof=[];sites=[]
    for ins in decoder.disasm(runtime.pe.get_data(0x3394f0,0x10c),0x23394f0):
        ops=ins.operands
        if ins.mnemonic=='mov' and len(ops)==2:
            dst,src=ops
            if dst.type==X86_OP_MEM and dst.mem.base==X86_REG_ESP and dst.mem.index==0 and dst.mem.disp in {0,4,8,12} and src.type==X86_OP_IMM:
                frame[dst.mem.disp]=src.imm;sites.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex()))
            if dst.type==X86_OP_REG and dst.reg==X86_REG_ECX and src.type==X86_OP_IMM:
                black=src.imm;sites.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex()))
        if ins.mnemonic=='xor' and len(ops)==2 and all(o.type==X86_OP_REG and o.reg==X86_REG_ECX for o in ops):
            black=0;sites.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex()))
        if ins.mnemonic=='call':
            if ops[0].type==X86_OP_IMM and ops[0].imm==int(PREDICATE,16):
                assert set(frame)=={0,4,8,12} and black is not None
                proof.append(dict(call=f'{ins.address:x}',black=black,stack=[frame[i] for i in [0,4,8,12]],sites=sites+[dict(address=f'{ins.address:x}',bytes=ins.bytes.hex())]))
            frame={};black=None;sites=[]
    assert [(f['black'],f['stack']) for f in proof]==[(2,[0,0,0,0]),(1,[0,1,0,0]),(0,[0,2,0,0])]
    return proof


def reconstruct(a,raw,runtime,meta):
    from hogaak_bodies import PREDICATE as PREDICATE_BODY,PAYMENT as PAYMENT_BODY
    text=strip_comments(raw);changes=[];pairs=[];sites=[dict(address=at,bytes=b) for at,b in FIXED.get(a,{}).items()]
    if a in {PREDICATE,PAYMENT}:
        body=PREDICATE_BODY if a==PREDICATE else PAYMENT_BODY
        changes=[dict(original=text,replacement=body,occurrences=1,kind='native Convoke VLA trial/payment and formatted call-frame reconstruction',machine_sites=sites,machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=int(meta[a]['high_pc'],16)-int(a,16),machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16))))]
        if a==PAYMENT:
            literal_at=0x27410c8;literal=readonly_string(runtime,literal_at);assert literal==b'Select a creature to tap for Convoke.'
            changes[0].update(native_literal=dict(address=f'{literal_at:x}',bytes=(literal+b'\0').hex(),copy_bytes=len(literal)+1),literal=literal.decode())
        return body,changes
    if a==CAST:
        start=text.index('  ptVar11 = pcVar6->targets + 8;');end=text.index('  if (from_grave != 0)',start)
        pairs=[(text[start:end],'  for (int i = 9; i > 0; --i) {\n    pcVar6->targets[i] = pcVar6->targets[i - 1];\n  }\n')]
    for old,new in pairs:
        count=text.count(old);assert count>0,(a,old)
        changes.append(dict(original=old,replacement=new,occurrences=count,kind='restore native reverse copy of nine eight-byte target pairs',machine_sites=sites));text=text.replace(old,new)
    return text,changes


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('hogaak_family_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert component['native_storage']==native_storage(Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()))
    assert component['hybrid_cost_frames']==predicate_cost_frames(runtime)
    from unify_scalar_helpers import retained_source_body
    for old in component['retained_bodies']:assert sha(retained_source_body(root,old['source'],old['name']).encode())==old['body_sha256']
    for layout in component['unit_layouts'].values():assert layout['test_definition_t']['bytes']==216 and layout['target_definition_t']['bytes']==84
    params=component['parameters'][PREDICATE];assert [p['name'] for p in params]==['player','cless','black','blue','green','red','white','card']
    for p,reg in zip(params[:3],[0,2,1]):assert f'DW_OP_reg{reg}' in str(p['location'])
    for p,offset in zip(params[3:7],[0,4,8,12]):assert p['location']==f'(DW_OP_fbreg: {offset})'
    assert 'DW_OP_GNU_parameter_ref' in str(params[-1]['location']) and 'DW_OP_reg' not in str(params[-1]['location']) and 'DW_OP_fbreg' not in str(params[-1]['location'])
    assert sum(s['target']==PREDICATE for f in component['functions'].values() for s in f['canonical_calls'])==5
    assert any(v['name']=='buffer' and v['type']=='char[500]' and v['location']=='(DW_OP_fbreg: -520)' for v in component['locals'][PAYMENT])
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['hogaak_family_evidence']==detail and entry['static']==(a in PRIVATE)
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-hogaak-family' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,{});assert literals==entry['recovered_literals'];text,_=rename(text,NAMES);expected=cleaned(text,overlap)
        if a in PRIVATE:expected='static '+expected
        assert function_body(expected,entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
        if a in HELPERS-PRIVATE:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
    assert len(entries)==7 and sum(e['name'].startswith('card_') for e in entries.values())==1


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(debug);predicate_cost_frames(runtime)
        params=debug_evidence(debug,meta,ADDRESSES)['parameters'][PREDICATE]
        assert [p['name'] for p in params]==['player','cless','black','blue','green','red','white','card']
        for p,reg in zip(params[:3],[0,2,1]):assert f'DW_OP_reg{reg}' in str(p['location'])
        for p,offset in zip(params[3:7],[0,4,8,12]):assert p['location']==f'(DW_OP_fbreg: {offset})'
        assert 'DW_OP_GNU_parameter_ref' in str(params[-1]['location']) and 'DW_OP_reg' not in str(params[-1]['location']) and 'DW_OP_fbreg' not in str(params[-1]['location'])
        (cache/'hogaak-clone-interface.tsv').write_text('\t'.join([PREDICATE,'has_convoked_mana_for_hogaak_impl','int','__regparm3']+['int:'+p['name'] for p in params[:-1]])+'\n')
        (cache/'export-request-hogaak-family.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES))
        print('Prepared seven Hogaak-family requests and the verified physical clone interface.');return
    assert 'hogaak_family_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(debug),hybrid_cost_frames=predicate_cost_frames(runtime),**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{'cards/modern_horizons.c','functions/functions.c'}),functions={},retained_bodies=[dict(source=s,name=n,body_sha256=sha(function_body(read(root/s),n).encode())) for s,n in LEGACY],
        scope='Recover Hogaak card and four Convoke/Delve helpers with native mana text formatting and the shared creature/color selector. Verify seven physical worker arguments and the nonphysical source card parameter, all five caller targets, four typed VLAs, fifty-attempt trial loops, colored/generic choice masks, temporary tapped-state restoration and human/AI native edge cases. Restore formatted payment argument and reverse copy of nine target pairs; preserve Delve hide/show, selection and exile order. Retain legacy formatter, all earlier records, globals, fragments, interfaces and dialog stubs; gameplay validation remains downstream.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES.get(meta[a]['name'],meta[a]['name']),source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-hogaak-family'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);text,literals=recover_strings(text,runtime,ALIASES);text,_=rename(text,NAMES);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,hogaak_family_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in HELPERS-PRIVATE)+'\n#endif\n\n'+header[at:];compile_sources(root,cache/'hogaak-family-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in [r for r in csv.DictReader((cache/'decompiled-hogaak-family/export.tsv').open(),delimiter='\t') if r['address'] in ADDRESSES]:rows[row['address']]=row;shutil.copyfile(cache/'decompiled-hogaak-family'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=7;m['card_handler_functions']+=1;m['helper_functions']+=6;m['private_functions']+=1;m['private_helpers']+=1;m['shared_helper_interfaces']+=5
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['hogaak_family_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'hogaak-family-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=1,new_helpers=6)))


if __name__=='__main__':main()
