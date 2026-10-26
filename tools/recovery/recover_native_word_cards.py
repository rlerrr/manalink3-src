"""Recover scoped native flag words and indexed engine globals in card handlers."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image,value
from export_abi import type_of,type_name
from promote_recovery import clean,strip_comments,UNRESOLVED
from recover_test_consumers import read,rename
from recover_literals import recover_strings
from recover_spell_copy import compile_sources,format_c
from recover_temporary_exile import save_json,sha
from audit_source import function_body

CONFIG={
 '21d1ff0':[('*(short *)(-player + 0x4ef1f5 + iVar1)','cards_drawn_this_turn[iVar1]',{}),('0xcf','TRIGGER_REPLACE_CARD_DRAW',{'TRIGGER_REPLACE_CARD_DRAW':207})],
 '22d7ab0':[('*(uint *)(&player_bits + player * 4)','player_bits[player]',{}),
            ('(*(byte *)(player * 4 + 0x4ef1a6) & 0x80)','(player_bits[player] & PB2022_CAST_APPROACH_OF_THE_SECOND_SUN2)',{'PB_CAST_APPROACH_OF_THE_SECOND_SUN2':0x800000}),
            ('0x400000','PB2022_CAST_APPROACH_OF_THE_SECOND_SUN1',{'PB_CAST_APPROACH_OF_THE_SECOND_SUN1':0x400000}),('0x800000;','PB2022_CAST_APPROACH_OF_THE_SECOND_SUN2;',{'PB_CAST_APPROACH_OF_THE_SECOND_SUN2':0x800000})],
 '25e6cd0':[('(*(byte *)(player * 4 + 0x4ef1a6) & 8)','(player_bits[player] & PB_COMMANDER_IN_PLAY)',{'PB_COMMANDER_IN_PLAY':0x80000})],
 '261f9f0':[('0x800000','KEYWORD_DEFENDER',{'KEYWORD_DEFENDER':0x800000})],
 '2207060':[('0x800000','KEYWORD_DEFENDER',{'KEYWORD_DEFENDER':0x800000})],
 '21d7080':[('0x800000','KEYWORD_DEFENDER',{'KEYWORD_DEFENDER':0x800000})],
 '20ad0c0':[('0xc00000','(GAA_REGENERATION | GAA_CAN_ONLY_TARGET_OPPONENT)',{'GAA_REGENERATION':0x800000,'GAA_CAN_ONLY_TARGET_OPPONENT':0x400000}),('0x800000','GAA_REGENERATION',{'GAA_REGENERATION':0x800000})],
 '264e100':[('0x800005','(GAA_REGENERATION | GAA_CAN_TARGET | GAA_UNTAPPED)',{'GAA_REGENERATION':0x800000,'GAA_CAN_TARGET':4,'GAA_UNTAPPED':1})],
 '264e5c0':[('td.special = 0x800000','td.special = TARGET_SPECIAL_REGENERATION',{'TARGET_SPECIAL_REGENERATION':0x800000}),('0x800025','(GAA_REGENERATION | GAA_DISCARD | GAA_CAN_TARGET | GAA_UNTAPPED)',{'GAA_REGENERATION':0x800000,'GAA_DISCARD':32,'GAA_CAN_TARGET':4,'GAA_UNTAPPED':1})],
 '23c7b30':[('0x800000','STATE_OUBLIETTED',{'STATE_OUBLIETTED':0x800000})],
 '238e250':[('0x800000','GAA_REGENERATION',{'GAA_REGENERATION':0x800000})],
 '22f87f0':[('0x800000','GAA_REGENERATION',{'GAA_REGENERATION':0x800000})],
 '25e7820':[('0x800000','GAA_REGENERATION',{'GAA_REGENERATION':0x800000})],
 '22b48c0':[('0x800000','KEYWORD_DEFENDER',{'KEYWORD_DEFENDER':0x800000})],
 '22bbcd0':[('0x800000','SF3_CANNOT_PHASE_IN',{'SF3_CANNOT_PHASE_IN':0x800000})],
 '22c1900':[('0x800000','SF3_CANNOT_PHASE_IN',{'SF3_CANNOT_PHASE_IN':0x800000})],
 '2488740':[('0x400000','SF2_CANNOT_BE_SACRIFICED',{'SF2_CANNOT_BE_SACRIFICED':0x400000})]}
CONFIG.update({
 '2377620':[
  ('0x800022','(STATE_OUBLIETTED | STATE_INVISIBLE | STATE_IN_PLAY)',{'STATE_OUBLIETTED':0x800000,'STATE_INVISIBLE':32,'STATE_IN_PLAY':2}),
  ('0x800002','(STATE_OUBLIETTED | STATE_IN_PLAY)',{'STATE_OUBLIETTED':0x800000,'STATE_IN_PLAY':2}),
  ('check_special_flags3(player,card,0x800000)','check_special_flags3(player,card,SF3_CANNOT_PHASE_IN)',{'SF3_CANNOT_PHASE_IN':0x800000}),
  ('0x8000000','SF_PHASED_OUT_INDIRECTLY',{'SF_PHASED_OUT_INDIRECTLY':0x8000000}),
  ('0xc000000','(SF_PHASED_OUT | SF_PHASED_OUT_INDIRECTLY)',{'SF_PHASED_OUT':0x4000000,'SF_PHASED_OUT_INDIRECTLY':0x8000000}),
  ('0x800000','STATE_OUBLIETTED',{'STATE_OUBLIETTED':0x800000})],
 '2377970':[('0x4000000','SF_PHASED_OUT',{'SF_PHASED_OUT':0x4000000})],
 '2377450':[
  ('0x8000000','SF_PHASED_OUT_INDIRECTLY',{'SF_PHASED_OUT_INDIRECTLY':0x8000000}),
  ('0xc000000U','(uint32_t)(SF_PHASED_OUT | SF_PHASED_OUT_INDIRECTLY)',{'SF_PHASED_OUT':0x4000000,'SF_PHASED_OUT_INDIRECTLY':0x8000000}),
  ('0xc000000','(SF_PHASED_OUT | SF_PHASED_OUT_INDIRECTLY)',{'SF_PHASED_OUT':0x4000000,'SF_PHASED_OUT_INDIRECTLY':0x8000000}),
  ('0x800000','STATE_OUBLIETTED',{'STATE_OUBLIETTED':0x800000})],
 '2377a70':[('0x4000000','SF_PHASED_OUT',{'SF_PHASED_OUT':0x4000000})]})
PHASING={'2377620':'phase_in_impl','2377970':'phase_in','2377450':'phase_out_impl','2377a70':'phase_out'}
PHASING_NAMES={name:name+'_2022' for name in PHASING.values()}
DEFS_LABEL='/* October 2022 Approach cast-history bits and phase-in restriction. */'
DEFINITIONS=DEFS_LABEL+'''
#ifndef SHANDALAR
enum
{
  PB2022_CAST_APPROACH_OF_THE_SECOND_SUN1 = 0x400000,
  PB2022_CAST_APPROACH_OF_THE_SECOND_SUN2 = 0x800000,
  SF3_CANNOT_PHASE_IN = 0x800000
};
#endif'''
HEADER_LABEL='/* October 2022 native flag-word helper interfaces. */'
GLOBAL_SITES={
 '21d2015':'b801000000','21d201a':'29f0','21d201c':'6683bc00f4f14e0000',
 '22d7b15':'f604b5a6f14e0080','22d7b78':'8b04b5a4f14e00','22d7b7f':'a900004000',
 '22d7b86':'0d00008000','22d7b8b':'8904b5a4f14e00','22d7bae':'8b04b5a4f14e00',
 '22d7bb5':'a900004000','22d7bc0':'0d00004000','22d7bc5':'8904b5a4f14e00',
 '25e6d58':'f604b5a6f14e0008'}
GLOBALS={'cards_drawn_this_turn':dict(address='4ef1f4',type='uint16_t[2]',bytes=4),
         'player_bits':dict(address='4ef1a4',type='player_bits_t[2]',bytes=8)}


def cleaned(text,overlap):
    return format_c(clean(text,overlap,use_test_api_2022=True,use_target_api_2022=True,
        use_token_api_2022=True,use_token_frontend_api_2022=True,use_extension_flags_2022=True,
        use_keyword_accessors_2022=True,use_ability_api_2022=True,use_pump_api_2022=True,
        use_activated_api_2022=True,use_attack_api_2022=True,use_entry_api_2022=True,
        use_global_tutor_api_2022=True,use_library_reveal_api_2022=True,use_life_api_2022=True,
        use_cannot_lose_api_2022=True,use_spell_api_2022=True,use_sacrifice_api_2022=True))


def audit_component(component,manifest,sources,machine_bytes,root):
    def require(ok,message):
        if not ok:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('native_word_evidence')}
    require(set(entries)==set(CONFIG)==set(component['functions']),'Native flag-word function membership mismatch')
    require(component['globals']==GLOBALS and component['global_sites']==GLOBAL_SITES,'Native draw-count/player-bit global projection mismatch')
    for a,data in GLOBAL_SITES.items():require(machine_bytes(int(a,16),len(bytes.fromhex(data))).hex()==data,'Native indexed engine-word instruction mismatch')
    for a,entry in entries.items():
        detail=component['functions'][a];body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name'])
        require(entry['native_word_evidence']==detail and sha(body.encode())==entry['reconstruction_body_sha256'],'Native flag-word source/evidence mismatch')
        require(entry['static']==(detail['coff_storage_class']==3),'Native flag-word function linkage mismatch')
        if a in PHASING:require(entry['name']==PHASING_NAMES[PHASING[a]],'Native phasing compatibility name mismatch')
        require(len(detail['changes'])==len(CONFIG[a]),'Native flag-word reconstruction membership mismatch')
        for change,(original,replacement,enums) in zip(detail['changes'],CONFIG[a]):
            require((change['original'],change['replacement'])==(original,replacement),'Native flag-word expression reconstruction mismatch')
            for name,number in enums.items():require(component['unit_enums'][entry['source']][name]==number,'Native flag-word per-unit enumerator mismatch')
        decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
        native=machine_bytes(int(a,16),detail['machine_bytes']);require(sha(native)==detail['machine_sha256'],'Native flag-word whole-function hash mismatch')
        instructions={f'{i.address:x}':i for i in decoder.disasm(native,int(a,16))}
        for change in detail['changes']:
            for site in change['machine_sites']:
                ins=instructions[site['address']];require(ins.bytes.hex()==site['bytes'],'Native flag-word operand evidence mismatch')
        for call in detail['canonical_calls']:
            ins=instructions[call['address']];require(ins.bytes.hex()==call['bytes'] and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm==int(call['target'],16) and re.search(r'\b'+call['name']+r'\s*\(',body),'Native flag-word callee binding mismatch')
    require(DEFINITIONS in read(root/'defs.h') or component.get('planning') is True,'Native Approach/phase-in definitions missing')
    for name,digest in component['retained_phasing_bodies'].items():
        from unify_scalar_helpers import retained_source_body
        require(sha(retained_source_body(root,'cards/mirage.c',name,source_view=sources).encode())==digest,'Retained original phasing implementation changed')
    narset=function_body(sources[entries['21d1ff0']['source']],entries['21d1ff0']['name'])
    index=re.search(r'cards_drawn_this_turn\[(\w+)\]',narset)
    require(index is not None and re.search(r'\b'+index[1]+r'\s*=\s*-player\s*\+\s*1;',narset),'Narset native opponent-indexed 16-bit draw-count access missing')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'native_word_cards_2022' not in interfaces
    assert not set(CONFIG)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    wanted={}
    for a,changes in CONFIG.items():wanted.setdefault(meta[a]['source'],{}).update({n:v for o,r,e in changes for n,v in e.items()})
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(data);unit_enums={};global_declarations={}
    for cu in debug.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in wanted:continue
        enums={};decls={}
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':
                for child in die.iter_children():
                    name=value(child,'DW_AT_name')
                    if name in wanted[source]:enums[name]=value(child,'DW_AT_const_value')
            if die.tag=='DW_TAG_variable' and value(die,'DW_AT_name') in GLOBALS:
                name=value(die,'DW_AT_name');decls[name]=dict(type=type_name(type_of(die)),die=hex(die.offset),external=value(die,'DW_AT_external'))
        assert enums==wanted[source],(source,enums,wanted[source]);unit_enums[source]=enums;global_declarations[source]=decls
    assert set(unit_enums)==set(wanted)
    for name,detail in GLOBALS.items():
        assert [s for s in debug.symbols if s[0]=='_'+name]==[('_'+name,int(detail['address'],16),-1,0,2)]
        assert all(d[name]['type']==detail['type'] and d[name]['external'] for d in global_declarations.values())
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());machine_bytes=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    proof=dict(debug_sha256=sha(data),unit_enums=unit_enums,global_declarations=global_declarations,globals=GLOBALS,global_sites=GLOBAL_SITES,functions={},
        retained_phasing_bodies={name:sha(function_body(read(root/'cards/mirage.c'),name).encode()) for name in PHASING.values()},
        scope='Native handlers/helpers with context-specific, caller-DWARF-verified scalar flag names and exact numeric bits, plus complete native phase-in/out workers and wrappers. Restore indexed 16-bit Narset draw counts and Approach/Fierce Guardianship player words; retain old Approach flags and phasing source under their original names.',
        integration_pending='Earlier manual consumers, native linking and gameplay verification remain pending.')
    symbols={int(a,16):[s for n,o,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+o==int(a,16)] for a in CONFIG}
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']};targets.update({int(a,16):PHASING_NAMES.get(meta[a]['name'],meta[a]['name']) for a in CONFIG})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global'];sources={};entries=[];bodies={}
    for a,changes in CONFIG.items():
        row=meta[a];raw=read(cache/('decompiled-phasing' if a in PHASING else 'decompiled-abi')/f'{a}.c');text=strip_comments(raw);detail_changes=[]
        native=machine_bytes(int(a,16),int(row['high_pc'],16)-int(a,16));insns=list(decoder.disasm(native,int(a,16)))
        for original,replacement,enums in changes:
            count=text.count(original);assert count>0,(a,original)
            mask=0
            for number in enums.values():mask|=number
            if original.startswith('0x'):mask=int(re.match(r'0x[0-9a-f]+',original)[0],16)
            if a in {'21d1ff0','22d7ab0','25e6cd0'} and (not enums or original.startswith('(*')):
                sites=[dict(address=s,bytes=b) for s,b in GLOBAL_SITES.items() if int(a,16)<=int(s,16)<int(row['high_pc'],16)]
            else:
                sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex()) for i in insns if i.mnemonic in {'mov','push','or','and','test','cmp'} and any(o.type==X86_OP_IMM and o.imm&0xffffffff==mask for o in i.operands)]
            assert sites,(a,original,mask)
            detail_changes.append(dict(original=original,replacement=replacement,occurrences=count,kind='scoped native flag/indexed-global reconstruction',machine_sites=sites));text=text.replace(original,replacement)
        text,literals=recover_strings(text,runtime,{});text,_=rename(text,PHASING_NAMES);body=cleaned(text,overlap)
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x|_conflict',body),a
        assert len(symbols[int(a,16)])==1 and symbols[int(a,16)][0] in {2,3};private=symbols[int(a,16)]==[3]
        if private:body='static '+body
        calls=[]
        for ins in insns:
            if ins.mnemonic not in {'call','jmp'} or ins.operands[0].type!=X86_OP_IMM or ins.operands[0].imm not in targets:continue
            name=targets[ins.operands[0].imm]
            if not re.search(r'\b'+name+r'\s*\(',body):
                probe=cleaned('int probe(void) { '+name+'(); return 0; }',overlap);name=re.search(r'\{\s*(\w+)\(',probe)[1]
            assert re.search(r'\b'+name+r'\s*\(',body),(a,name)
            calls.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{ins.operands[0].imm:x}',name=name))
        detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=detail_changes,canonical_calls=calls,coff_storage_class=symbols[int(a,16)][0]);proof['functions'][a]=detail;bodies[a]=body
        name=PHASING_NAMES.get(row['name'],row['name'])
        entries.append(dict(address=a,name=name,source=row['source'],line=row['line'],static=private,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),
            recovered_literals=literals,native_word_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    for source in {e['source'] for e in entries}:
        original=read(root/source);selected=[e for e in entries if e['source']==source]
        for e in selected:
            assert not re.search(r'\b'+e['name']+r'\s*\([^;{}]*\)\s*\{',strip_comments(original)),(source,e['name'])
        addition='\n\n#ifndef SHANDALAR\n/* Recovered October 2022 native scalar-word handlers. */\n'+'\n'.join(e['prototype'] for e in selected)+'\n'
        for e in selected:addition+=f'\n/* ManalinkEh 2022: 0x{e["address"]}; original {source}:{e["line"]}. */\n'+bodies[e['address']]+'\n'
        sources[source]=original+addition+'#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');public=[e['prototype'] for e in entries if not e['static'] and not e['name'].startswith('card_')]
    header=header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(public)+'\n#endif\n\n'+header[at:]
    definitions=read(root/'defs.h');assert DEFS_LABEL not in definitions;at=definitions.rfind('\n#endif');definitions=definitions[:at]+'\n\n'+DEFINITIONS+'\n'+definitions[at:]
    # Check the additions with strict format/uninitialized diagnostics. Earlier
    # recovered bodies have existing extra variadic arguments, so do not apply
    # this stricter policy retroactively to their unchanged source prefixes.
    strict_sources={}
    for source in sources:
        selected=[e for e in entries if e['source']==source];text='\n'.join(bodies[e['address']] for e in selected)
        retained=[f['prototype'] for f in m['accepted_functions'] if f['source']==source and f['static'] and re.search(r'\b'+f['name']+r'\b',text)]
        strict_sources[source]='#include "manalink.h"\n'+'\n'.join(retained+[e['prototype'] for e in selected])+'\n'+text
    scratch=cache/'native-word-cards-check';scratch.mkdir(exist_ok=True);(scratch/'defs.h').write_bytes(definitions.encode('latin1'));compile_sources(root,scratch,strict_sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,machine_bytes,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not set(PHASING)&{c['address'] for c in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};focused={r['address']:r for r in csv.DictReader((cache/'decompiled-phasing/export.tsv').open(),delimiter='\t')}
        for a in PHASING:rows[a]=focused[a];shutil.copyfile(cache/'decompiled-phasing'/f'{a}.c',cache/'decompiled-abi'/f'{a}.c')
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()})
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));(root/'defs.h').write_bytes(definitions.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=len(entries)
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['card_handler_functions']=sum(f['name'].startswith('card_') for f in m['accepted_functions']);m['helper_functions']=m['accepted']-m['card_handler_functions']
        m['private_functions']=sum(f['static'] for f in m['accepted_functions']);m['private_helpers']=sum(f['static'] and not f['name'].startswith('card_') for f in m['accepted_functions']);m['shared_helper_interfaces']=m['helper_functions']-m['private_helpers']
        registrations=list(csv.DictReader((cache/'2022-registrations.csv').open()));names={f['name'] for f in m['accepted_functions']}
        m['registered_handler_symbols']=len({r['symbol'].lstrip('_') for r in registrations if r['symbol'].lstrip('_') in names});m['registered_handler_symbols_marked_coded']=len({r['symbol'].lstrip('_') for r in registrations if r['coded']=='True' and r['symbol'].lstrip('_') in names})
        save_json(mp,m);interfaces['native_word_cards_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'native-word-cards-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_implementations=len(entries),card_functions=sum(e['name'].startswith('card_') for e in entries),private_helpers=sum(e['static'] for e in entries),native_word_changes=sum(len(d['changes']) for d in proof['functions'].values()))))


if __name__=='__main__':main()
