"""Recover six alternate-casting cards and four native replacement-filter helpers."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read,rename
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings,readonly_string,byte_literal
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input
from alternate_casting_bodies import BODIES

HELPERS=['2696e30','2696f20','2697040','2697150'];CARDS=['22d9770','25cf1b0','22ceb70','20d5330','228b340','22d0bd0'];ADDRESSES=HELPERS+CARDS
LABEL='/* October 2022 graveyard-replacement helpers with native filter layout. */'
NAMES={'test_definition_t':'test_definition_2022_t','is_humiliated':'is_humiliated_for_ability','new_make_test':'new_make_test_2022','new_make_test_in_play':'new_make_test_in_play_2022','set_special_flags2':'set_special_flags2_2022'}
for n in ['if_a_card_would_be_put_into_graveyard_from_play_exile_it_instead','if_a_card_would_be_put_into_graveyard_from_library_do_something_instead','if_a_card_would_be_put_into_graveyard_from_anywhere_but_library_do_something_instead','if_a_card_would_be_put_into_graveyard_from_anywhere_exile_it_instead']:NAMES[n]=n+'_2022'
LITERALS={0x273d7c9:b'Select a spell to cast.',0x27584a0:b'Select a non-land card with mana value less than or equal to %d to play.',0x273d027:b'Select a card to play.',0x272a17c:b'Select a card to play.',0x273a354:b'Select a permanent card to play.',0x273d24c:b'Select a permanent card to play.',0x273cdb8:b'\x01\x02',0x273ceea:b'\x01\x0c',0x273d151:b'\x01\x03',0x273d29d:b'\x01\x04',0x273cd85:b'\x01\x0e',0x273d294:b'Artifact',0x273d28b:b'Creature',0x273d27f:b'Enchantment',0x273d27a:b'Land',0x273d26d:b'Planeswalker'}


def native_storage(runtime,meta,root):
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;allocations={};get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    for a in CARDS:
        code=get(int(a,16),int(meta[a]['high_pc'],16)-int(a,16));insns=list(decoder.disasm(code,int(a,16)));probes=[i for i,ins in enumerate(insns) if ins.mnemonic=='call' and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm==0x26fc1d0]
        assert len(probes)==1,a
        i=probes[0];prev=insns[i-1];ins=insns[i];after=insns[i+1]
        assert prev.mnemonic=='lea' and '*8' in prev.op_str and after.mnemonic=='sub' and after.op_str=='esp, eax',(a,prev.op_str)
        allocations[a]=dict(rows=2,element_bytes=4,size_source='hand_count[player]' if a in CARDS[:3] else 'count_graveyard(player)',scale_instruction=dict(address=f'{prev.address:x}',bytes=prev.bytes.hex(),instruction=prev.mnemonic+' '+prev.op_str),probe=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target='26fc1d0'),subtract=dict(address=f'{after.address:x}',bytes=after.bytes.hex()))
    for a,text in LITERALS.items():assert readonly_string(runtime,a)==text,(hex(a),text)
    assert get(0x20d5345,3)==bytes.fromhex('83cf0a') and get(0x20d5373,3)==bytes.fromhex('83f808')
    assert get(0x228b79e,9)==bytes.fromhex('83f8020f8eebfbffff')
    assert get(0x22d0cb0,2)==bytes.fromhex('89d0') and get(0x22d0cc8,6)==bytes.fromhex('898510ffffff')
    # Every native outgoing word of the five-option type chooser is retained.
    dialog=get(0x22d10c7,0x22d1199-0x22d10c7)
    assert bytes.fromhex('c744246800000000') in dialog and bytes.fromhex('c744245801000000') in dialog and dialog[-5:]==bytes.fromhex('e8b78a4100')
    return dict(vlas=allocations,filter_bytes=216,literals={f'{a:x}':dict(bytes=(s+b'\0').hex(),literal=byte_literal(s)) for a,s in LITERALS.items()},
        sarcophagus_flags=dict(address='20d5345',bytes='83cf0a',mask=10,legacy_milled_flag=4,native_milled_flag=8,trigger_gate_address='20d5373',trigger_gate_bytes='83f808',trigger_gate_value=8),
        lurrus_companion_active_check=dict(address='228b79e',bytes='83f8020f8eebfbffff',behavior='Native rejects active entries with mana value <= 2; preserve rather than correcting the companion query.'),
        muldrotha_dialog=dict(address='22d10c7',bytes=dialog.hex(),words=27,choices=['Artifact','Creature','Enchantment','Land','Planeswalker'],choice_masks=[64,2,4,1,256],trailing_zero_words=4),
        native_semantics=['Keep duplicated timing-predicate calls and native 99/1 legality branches.',
        'Jodah pays WUBRG before constructing candidates; AI uses only cards it cannot pay for normally.',
        'Lurrus gates humiliation, turn and once-use state only during CAN_ACTIVATE, then marks usage after payment before play.',
        'Muldrotha stores used types in its matching legacy when found; activation does not repeat the turn check or normalize -1.',
        'Sarcophagus retains literal enable mask 0xA and trigger gate 8 despite older header names/values.',
        'Muldrotha preserves all 27 dialog words, consumes type state before play and returns the saved zero on activation paths.'])


def reconstruct(a,raw,runtime,meta):
    text=strip_comments(raw)
    if a in CARDS:
        body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
        return body,[dict(original=text,replacement=body,occurrences=1,kind='restore native two-row VLA, exact event/legality/payment/state rules, native companion query and complete 27-word Muldrotha type chooser',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]
    return text,[]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('alternate_casting_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());units={e['source'] for e in entries.values()};assert component['unit_layouts']==native_layouts(debug,units)
    for old in component['retained_helpers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    for old in component['retained_private_callbacks']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};assert component['native_storage']==native_storage(runtime,meta,root)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for record in component['native_storage']['literals'].values():assert record['literal'] in ''.join(BODIES.values())
    for a,e in entries.items():
        detail=component['functions'][a];assert e['alternate_casting_evidence']==detail and not e['static']
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-alternate-casting' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,{});assert literals==e['recovered_literals'];text,_=rename(text,NAMES);assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==2
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
        assert 'test_definition_t' not in body and 'test_definition_2022_t' in body
        if a in HELPERS:assert e['prototype'] in read(root/'manalink.h') or component.get('planning')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(runtime,meta,root);(cache/'export-request-alternate-casting.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared six alternate-casting cards and four replacement helpers.');return
    assert 'alternate_casting_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    units={meta[a]['source'] for a in ADDRESSES}
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(runtime,meta,root),unit_layouts=native_layouts(debug,units),**debug_evidence(debug,meta,ADDRESSES),functions={},
        retained_helpers=[dict(name=meta[a]['name'],source=meta[a]['source'],body_sha256=sha(function_body(read(root/meta[a]['source']),meta[a]['name']).encode())) for a in HELPERS],
        retained_private_callbacks=[dict(name=n,source=s,body_sha256=sha(function_body(read(root/s),n).encode())) for s,n in [('cards/hour_of_devastation.c','has_cycling'),('cards/dominaria.c','muldrotha_legacy')]],
        scope='Recover As Foretold, Fires of Invention, Jodah, Abandoned Sarcophagus, Lurrus and Muldrotha with their six native two-row VLAs and per-card event/timing/AI/payment/state behavior. Recover four qualified graveyard replacement helpers with 216-byte filters, retaining all older 208-byte APIs. Preserve explicit native quirks, exact prompts, callback identities, flag values, replacement dispatch and the full 27-word Muldrotha chooser. Keep original source ownership; gameplay integration remains downstream.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES.get(meta[a]['name'],meta[a]['name']),source=meta[a]['source'],static=False) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-alternate-casting'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);text,literals=recover_strings(text,runtime,{});text,_=rename(text,NAMES);body=cleaned(text,overlap)
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=2)
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,alternate_casting_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in HELPERS)+'\n#endif\n\n'+header[at:];compile_sources(root,cache/'alternate-casting-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-alternate-casting/export.tsv').open(),delimiter='\t'):rows[row['address']]=row;shutil.copyfile(cache/'decompiled-alternate-casting'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=10;m['card_handler_functions']+=6;m['helper_functions']+=4;m['shared_helper_interfaces']+=4
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['alternate_casting_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'alternate-casting-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=6,new_helpers=4)))


if __name__=='__main__':main()
