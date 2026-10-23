"""Recover three reanimation callers from packed-byte, variadic and VLA evidence."""
import argparse,csv,json,re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from elftools.dwarf.locationlists import LocationParser,LocationExpr,BaseAddressEntry
from elftools.dwarf.descriptions import describe_DWARF_expr
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import strip_comments,UNRESOLVED
from recover_combat_mask_cards import cleaned
from recover_phase_prompt_cards import debug_evidence
from recover_test_consumers import read
from recover_spell_copy import compile_sources
from recover_literals import recover_strings
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input
from reanimation_vla_body import EERIE

ADDRESSES=['21ca670','25cd080','2288d90']
PACKED_SITES={'21ca76c':'c7473800000000','21ca9b9':'8b5638','21ca9bc':'01c2','21ca9be':'895638',
 '21ca9e8':'884739','21ca82a':'8a4738','21ca83b':'8a4739','21ca89e':'83f808','21ca900':'83fe09'}
FORMAT_SITES={'25cd2bd':'8b85b4feffff','25cd2c9':'8944240c','25cd2da':'8b85acfeffff',
 '25cd2e3':'89442408','25cd2e7':'c74424041c837502','25cd2f5':'e8f64a1300'}
VLA_SITES={'2288ff8':'8d04b500000000','2288fff':'85f6','2289007':'7ea3','2288fb8':'e813324700',
 '2288fbd':'29c4','2288fbf':'8d44241c','2289055':'e876314700','228905a':'29c4','228905e':'8d54241c',
 '2289068':'8b8c853cfbffff','228906f':'890c82','2289077':'c1ef02','228907a':'89542404',
 '2289087':'e8249a3c00','228908c':'8d04bd00000000','2289093':'e838314700','2289098':'29c4',
 '22890a2':'8d44241c','22890b8':'c744241000000000','22890c0':'8b04b7','22890c7':'8944240c',
 '22890d5':'e886264100','22890e0':'8904b1','22890e3':'c744240801000000','22890f3':'e8a8ec3f00',
 '2289100':'8bb558faffff','2289110':'c744240800000000','2289118':'8b14b7','2289123':'e878ec3f00',
 '2288fd0':'8ba554faffff','2288fed':'e9cefdffff'}
RANGES=[('2288d90','2288e08','event routing'),('2288e08','2288e6e','test setup and graveyard shroud'),
 ('2288e6e','2288fac','selection, temporary Aura validation and chosen rows'),
 ('2288fac','2288ff2','zero-selection sort, restore, kill and basic-spell return'),
 ('2288ff8','2289135','restore, two VLAs, sort, reanimate, set and clear state')]


def vla_bounds(debug):
    dw=debug.dwarf();parser=LocationParser(dw.location_lists());result={}
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='cards/ikoria.c':continue
        def walk(d):
            name=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_variable' and name in {'to_reanimate','reanimated'}:
                t=type_of(d);assert t.tag=='DW_TAG_array_type' and type_name(type_of(t))=='int'
                subranges=[x for x in t.iter_children() if x.tag=='DW_TAG_subrange_type'];assert len(subranges)==1
                s=subranges[0];at=s.attributes['DW_AT_upper_bound'];assert at.form=='DW_FORM_ref4'
                bound=s.get_DIE_from_attribute('DW_AT_upper_bound');assert value(bound,'DW_AT_artificial')
                loc=parser.parse_from_attribute(bound.attributes['DW_AT_location'],cu['version'],die=bound)
                assert not isinstance(loc,LocationExpr)
                result[name]=dict(upper_bound_form=at.form,upper_bound_reference=hex(bound.offset),element_type='int',
                    bound_locations=[dict(base=hex(x.base_address)) if isinstance(x,BaseAddressEntry) else dict(start=hex(x.begin_offset),end=hex(x.end_offset),expression=describe_DWARF_expr(x.loc_expr,dw.structs)) for x in loc])
            for c in d.iter_children():walk(c)
        for d in cu.get_top_DIE().iter_children():
            if value(d,'DW_AT_low_pc')==0x2288d90:walk(d)
    assert set(result)=={'to_reanimate','reanimated'}
    return result


def transform(a,text):
    changes=[]
    def replace(old,new,kind,sites):
        nonlocal text
        count=text.count(old);assert count,(a,old)
        changes.append(dict(original=old,replacement=new,occurrences=count,kind=kind,machine_sites=[dict(address=a,bytes=b) for a,b in sites.items()]))
        text=text.replace(old,new)
    if a=='21ca670':
        replace('(pcVar2->field_26).info_slot_fn','pcVar2->info_slot','native 32-bit integer cost addition',PACKED_SITES)
        replace('*(undefined1 *)((int)&pcVar2->field_26 + 1)','((unsigned char *)&pcVar2->info_slot)[1]','native info-slot second-byte write',PACKED_SITES)
        replace('(uint)*(byte *)((int)&pcVar2->field_26 + 1)','(uint)((unsigned char *)&pcVar2->info_slot)[1]','native zero-extended second-byte read',PACKED_SITES)
        replace('(uint)*(byte *)&pcVar2->field_26','(uint)((unsigned char *)&pcVar2->info_slot)[0]','native zero-extended low-byte damage read',PACKED_SITES)
    elif a=='25cd080':
        replace('sprintf(buffer,"Select up to %d art. or non-Aura ench. with cmc %d or less.",iStack_158);',
            'sprintf(buffer,"Select up to %d art. or non-Aura ench. with cmc %d or less.",iStack_158,iVar4);',
            'restore native ESP+12 second variadic integer argument',FORMAT_SITES)
    else:
        replace(text,EERIE,'native selection and dynamic array call-frame reconstruction',VLA_SITES)
    return text,changes


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('reanimation_tail_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions'])
    assert component['info_slot']==dict(offset=56,bytes=4,damage_byte=0,split_count_byte=1)
    assert set(component['vla_bounds'])=={'to_reanimate','reanimated'}
    for bound in component['vla_bounds'].values():
        assert bound['upper_bound_form']=='DW_FORM_ref4' and bound['element_type']=='int' and bound['bound_locations']
        assert {v['expression'] for v in bound['bound_locations']}=={'(DW_OP_breg6 (r6): -1; DW_OP_stack_value)','(DW_OP_fbreg: -1452; DW_OP_deref; DW_OP_lit1; DW_OP_minus; DW_OP_stack_value)'}
    assert any(v['name']=='chosen' and v['type']=='int[2][151]' and v['location']=='(DW_OP_fbreg: -1228)' for v in component['locals']['2288d90'])
    assert any(v['name']=='test' and v['location']=='(DW_OP_fbreg: -1444)' for v in component['locals']['2288d90'])
    predicate=component['retained_predicate']
    assert predicate['address']=='26c1000' and predicate['source']=='functions/global_checking.c'
    retained=function_body(sources[predicate['source']].replace('\r\n','\n'),'test_not_in_array')
    assert sha(retained.encode())==predicate['body_sha256'] and 'result < 4' in retained
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name'])
        assert entry['reanimation_tail_evidence']==detail and sha(body.encode())==entry['reconstruction_body_sha256'] and not entry['static']
        raw=read(root/'.recovery/decompiled-abi'/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=transform(a,strip_comments(raw));assert changes==detail['changes']
        assert target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        native=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(native)==detail['machine_sha256']
        instructions={f'{i.address:x}':i for i in decoder.disasm(native,int(a,16))}
        for call in detail['canonical_calls']+detail['callback_references']:
            ins=instructions[call['address']];assert ins.bytes.hex()==call['bytes']
            assert any(o.type==X86_OP_IMM and o.imm==int(call['target'],16) for o in ins.operands)
            assert re.search(r'\b'+call['name']+r'\b',body),(a,call)
    command=function_body(sources[entries['21ca670']['source']],'card_command_the_dreadhorde')
    assert 'instance->info_slot = instance->info_slot + iVar4;' in command
    assert '((unsigned char *)&instance->info_slot)[0]' in command and command.count('((unsigned char *)&instance->info_slot)[1]')==2
    assert 'uVar5 < 9' in command and 'uVar5 != 9' in command
    dance=function_body(sources[entries['25cd080']['source']],'card_dance_of_the_manse')
    assert re.search(r'sprintf\(buffer,\s*"Select up to %d art. or non-Aura ench. with cmc %d or less.",\s*iStack_158,\s*iVar4\);',dance)
    callback=next(f for f in manifest['accepted_functions'] if f['address']=='25c8f40');assert callback['static'] and callback['source']==entries['25cd080']['source']
    eerie=function_body(sources[entries['2288d90']['source']],'card_eerie_ultimatum')
    assert 'int chosen[2][151];' in eerie and all(f'int {n}[num_chosen];' in eerie for n in ['to_reanimate','reanimated'])
    assert 'descending_qsort(num_chosen, to_reanimate);' in eerie and 'for (int i = shroud;' in eerie
    assert 'set_special_flags3_2022(player, card, 0x40);' in eerie and 'test.value_for_special_selection_function = (int)chosen[1];' in eerie
    for block in component['vla_ranges']:
        start,end=int(block['start'],16),int(block['end'],16);assert end-start==block['bytes'] and sha(machine_bytes(start,end-start))==block['sha256']
    assert component['vla_ranges']==[dict(start=s,end=e,bytes=int(e,16)-int(s,16),sha256=sha(machine_bytes(int(s,16),int(e,16)-int(s,16))),role=role) for s,e,role in RANGES]
    assert 'Native selection storage is not initialized' in sources[entries['2288d90']['source']]
    assert 'info_slot_fn' not in command


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'reanimation_tail_2022' not in interfaces and 'reanimation_2022' in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(data)
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    proof=dict(debug_sha256=sha(data),**debug_evidence(debug,meta,ADDRESSES),vla_bounds=vla_bounds(debug),functions={},info_slot=dict(offset=56,bytes=4,damage_byte=0,split_count_byte=1),
        vla_ranges=[dict(start=s,end=e,bytes=int(e,16)-int(s,16),sha256=sha(get(int(s,16),int(e,16)-int(s,16))),role=role) for s,e,role in RANGES])
    # The retained selection predicate reads four entries, including uninitialized ones.
    predicate=next(f for f in m['accepted_functions'] if f['address']=='26c1000');proof['retained_predicate']=dict(address='26c1000',source=predicate['source'],body_sha256=sha(function_body(read(root/predicate['source']),predicate['name']).encode()))
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;targets={int(f['address'],16):f for f in m['accepted_functions']}
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    entries=[];sources={}
    for a in ADDRESSES:
        row=meta[a];raw=read(cache/'decompiled-abi'/f'{a}.c');text,changes=transform(a,strip_comments(raw))
        text,literals=recover_strings(text,runtime,{});body=cleaned(text,overlap)
        assert not UNRESOLVED.search(body) and not re.search(r'stack0x|\._\d|_conflict',body),a
        if a=='2288d90':
            body=body.replace('    int chosen[2][151];','    /* Native selection storage is not initialized before the predicate reads it. */\n    int chosen[2][151];')
            body=body.replace('      int to_reanimate[num_chosen];','      /* Native zero-selection path also sorts a zero-length GNU VLA. */\n      int to_reanimate[num_chosen];')
        native=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,canonical_calls=[],callback_references=[])
        for ins in decoder.disasm(native,int(a,16)):
            for o in ins.operands:
                if o.type!=X86_OP_IMM or o.imm not in targets:continue
                target=targets[o.imm];name=target['name'];assert re.search(r'\b'+name+r'\b',body),(a,name)
                assert not target['static'] or target['source']==row['source']
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{o.imm:x}',name=name)
                (detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail
        entry=dict(address=a,name=row['name'],source=row['source'],line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,reanimation_tail_evidence=detail,
            reconstruction='Native info-slot integer/byte accesses, both variadic integer operands or complete selection/VLA call-frame reconstruction; preserve native bounds, selection storage and state-call quirks.',reconstruction_body_sha256=sha(function_body(body,row['name']).encode()))
        if a=='2288d90':entry['known_native_behavior']='The selection predicate reads four slots of initially uninitialized chosen[1]; selection has no explicit 151-entry guard, and failed reanimation results still receive state calls. No gameplay fixes inferred.'
        entries.append(entry)
        retained=[f['prototype'] for f in m['accepted_functions'] if f['source']==row['source'] and f['static'] and re.search(r'\b'+f['name']+r'\b',body)]
        compile_sources(root,cache/'reanimation-tail-check',{row['source']:'#include "manalink.h"\n'+'\n'.join(retained)+'\n'+body})
        source=sources.get(row['source'],read(root/row['source']));sources[row['source']]=source+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {row["source"]}:{row["line"]}. */\n'+body+'\n#endif\n'
    # The retained predicate's owning unit also participates in the body audit.
    sources.setdefault(predicate['source'],read(root/predicate['source']))
    audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root)
    if args.apply:
        for source,text in sources.items():
            if source==predicate['source']:continue
            (root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=3;m['card_handler_functions']+=3
        for entry in entries:m['source_files'][entry['source']]+=1
        save_json(mp,m);interfaces['reanimation_tail_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'reanimation-tail-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_card_functions=3)))


if __name__=='__main__':main()
