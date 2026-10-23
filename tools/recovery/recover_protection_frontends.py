"""Recover four inlined protection-source frontends and Tanazir's creature snapshot."""
import argparse,csv,json,re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image
from promote_recovery import strip_comments,UNRESOLVED
from recover_combat_mask_cards import cleaned
from recover_phase_prompt_cards import debug_evidence
from recover_test_consumers import read
from recover_spell_copy import compile_sources
from recover_literals import recover_strings
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input

HELPERS=['26e09d0','26e0b90','26e0d00','26e0f50']
CARD='23dd3c0'
ADDRESSES=[*HELPERS,CARD]
HEADER_LABEL='/* October 2022 miscellaneous protection frontends. */'
WRAPPER='get_protection_source'
INLINE_PATTERN=r"if \(-1 < attacking_card\) \{\s*get_protection_source_impl\('\\x01',&p,&c\);\s*\}"
INLINE_START={'26e09d0':0x26e0a48,'26e0b90':0x26e0bf8,'26e0d00':0x26e0d68,'26e0f50':0x26e0fb3}
ARRAY_SITES={'23dd569':'c78594fdffff00000000','23dd5a0':'8b8594fdffff','23dd5a6':'89bc8598fdffff',
 '23dd5ad':'40','23dd5ae':'898594fdffff','23dd5c8':'8dbc8594fdffff','23dd5e3':'898594fdffff',
 '23dd614':'8b07','23dd625':'e856003000','23dd62c':'8d8d98fdffff','23dd632':'83ef04','23dd635':'39c8','23dd637':'7597'}
TANAZIR_LOOP='''mc = 0;
    do {
      pcVar2 = in_play(player, iVar1);
      if (pcVar2 != (card_instance_t *)0 && is_what(player, iVar1, 2))
        marked[mc++] = iVar1;
      --iVar1;
    } while (iVar1 != -1);
    if (mc > 0) {
      int *cursor = marked + mc - 1;
      do {
        int toughness = get_toughness(player, card);
        int power = get_power(player, card);
        set_pt_and_abilities_until_eot(player, card, player, *cursor, power, toughness, 0, 0, 8);
        if (cursor == marked)
          break;
        --cursor;
      } while (1);
    }
  }
'''


def transform(a,text,sites):
    changes=[]
    def replace(old,new,kind):
        nonlocal text
        count=text.count(old);assert count,(a,old)
        changes.append(dict(original=old,replacement=new,occurrences=count,kind=kind,machine_sites=sites))
        text=text.replace(old,new)
    if a in HELPERS:
        matches=list(re.finditer(INLINE_PATTERN,text));assert len(matches)==1
        replace(matches[0][0],'get_protection_source(1, &p, &c);','reconstitute native public wrapper guard and private worker call')
    else:
        replace('undefined1 auStack_270 [4];','int mc;\n  int marked[151];','separate 151-int snapshot from 84-byte target lifetime')
        replace('target_definition_t *ptVar4;','','remove generated target-typed snapshot iterator')
        replace('bool bVar5;','','remove generated pointer termination temporary')
        start=text.index('auStack_270 = (undefined1  [4])0x0;');end=text.rindex('  return 0;')
        replace(text[start:end],TANAZIR_LOOP,'native descending collection and reverse snapshot iteration, refreshing power/toughness for every target')
    return text,changes


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('protection_frontend_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions'])
    wrapper=component['retained_wrapper'];assert wrapper['address']=='26e09a0' and wrapper['source']=='functions/abilities.c'
    body=function_body(sources[wrapper['source']].replace('\r\n','\n'),WRAPPER)
    assert sha(body.encode())==wrapper['body_sha256']
    assert 'if (-1 < attacking_card)' in body and 'get_protection_source_impl(can_return_dmg, eff_acc, eff_ac);' in body
    assert machine_bytes(0x26e09a3,7).hex()=='833d9891730000'
    assert machine_bytes(0x26e09b3,11).hex()=='780b0fbec05de942c8ffff'
    for name,type_,offset in [('marked','int[151]',-624),('td','target_definition_t',-624)]:
        assert any(v['name']==name and v['type']==type_ and v['location']==f'(DW_OP_fbreg: {offset})' for v in component['locals'][CARD])
    for a in ADDRESSES:
        assert [p['location'] for p in component['parameters'][a]]==[f'(DW_OP_fbreg: {i*4})' for i in range(4 if a=='26e09d0' else 3)]
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name'])
        assert entry['protection_frontend_evidence']==detail and sha(body.encode())==entry['reconstruction_body_sha256'] and not entry['static']
        raw=read(root/'.recovery/decompiled-abi'/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        sites=([dict(address=f'{i.address:x}',bytes=i.bytes.hex()) for i in decoder.disasm(machine_bytes(INLINE_START[a],26),INLINE_START[a])] if a in HELPERS else [dict(address=a,bytes=b) for a,b in ARRAY_SITES.items()])
        text,changes=transform(a,strip_comments(raw),sites);assert changes==detail['changes']
        assert target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        native=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(native)==detail['machine_sha256']
        instructions={f'{i.address:x}':i for i in decoder.disasm(native,int(a,16))}
        for call in detail['canonical_calls']+detail['callback_references']:
            ins=instructions[call['address']];assert ins.bytes.hex()==call['bytes']
            assert any(o.type==X86_OP_IMM and o.imm==int(call['target'],16) for o in ins.operands)
            assert re.search(r'\b'+call['name']+r'\b',body),(a,call)
        if a in HELPERS:
            for name,offset in [('p',-16),('c',-12)]:
                assert any(v['name']==name and v['type']=='int' and v['location']==f'(DW_OP_fbreg: {offset})' for v in component['locals'][a])
            assert 'get_protection_source(1, &p, &c);' in body and 'get_protection_source_impl' not in body
            ins=list(decoder.disasm(machine_bytes(INLINE_START[a],26),INLINE_START[a]))
            assert [i.bytes.hex() for i in ins[:6]]==['8b1598917300','85d2','7810','8d4dfc','8d55f8','b801000000']
            call=ins[6];assert call.mnemonic=='call' and call.operands[0].imm==0x26dd200
            assert ins[2].operands[0].imm==call.address+call.size
            assert detail['inline_wrapper_call']==dict(address=f'{call.address:x}',bytes=call.bytes.hex(),target='26dd200',source_wrapper='26e09a0')
            assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
        else:
            assert 'int marked[151];' in body and 'marked[mc++] = iVar1;' in body and 'int *cursor = marked + mc - 1;' in body
            assert body.index('get_toughness(player, card)')<body.index('get_power(player, card)')<body.index('set_pt_and_abilities_until_eot(')
            assert 'if (cursor == marked)' in body and '--cursor;' in body


def bind_card(root,apply):
    from bind_delayed_callback_callers import audit_component as audit_callers
    cache=root/'.recovery';m=json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text())
    ip=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(ip.read_text());assert 'protection_frontend_card_callers_2022' not in interfaces
    a,target,name='22d2db0','26e0f50','cannot_be_target_of_abilities';entry=next(f for f in m['accepted_functions'] if f['address']==a)
    row=next(r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc']=='0x'+a)
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());native=runtime.pe.get_data(int(a,16)-0x2000000,int(row['high_pc'],16)-int(a,16))
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=target,name=name) for i in decoder.disasm(native,int(a,16)) if any(o.type==X86_OP_IMM and o.imm==int(target,16) for o in i.operands)]
    source=read(root/entry['source']);proof=dict(functions={a:dict(machine_bytes=len(native),machine_sha256=sha(native),sites=sites,body_sha256=sha(function_body(source,entry['name']).encode()))},scope="Shanna, Sisay's Legacy binds its opponent-only ability-target protection branch to the recovered native frontend.")
    audit_callers(proof,m,{entry['source']:source},lambda a,n:runtime.pe.get_data(a-0x2000000,n),expected={a:target},targets={target:name})
    if apply:interfaces['protection_frontend_card_callers_2022']=proof;save_json(ip,interfaces)
    print(json.dumps(dict(applied=apply,bound_card_handlers=1,native_call_operands=len(sites))))


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--bind-card',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    if args.bind_card:bind_card(root,args.apply);return
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'protection_frontends_2022' not in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(data)
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    proof=dict(debug_sha256=sha(data),**debug_evidence(debug,meta,ADDRESSES),functions={},retained_wrapper=dict(address='26e09a0',source='functions/abilities.c',body_sha256=sha(function_body(read(root/'functions/abilities.c'),WRAPPER).encode())))
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;targets={int(f['address'],16):f for f in m['accepted_functions']}
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    entries=[];sources={}
    for a in ADDRESSES:
        row=meta[a];raw=read(cache/'decompiled-abi'/f'{a}.c')
        sites=([dict(address=f'{i.address:x}',bytes=i.bytes.hex()) for i in decoder.disasm(get(INLINE_START[a],26),INLINE_START[a])] if a in HELPERS else [dict(address=a,bytes=b) for a,b in ARRAY_SITES.items()])
        text,changes=transform(a,strip_comments(raw),sites);text,literals=recover_strings(text,runtime,{});body=cleaned(text,overlap)
        assert not UNRESOLVED.search(body) and not re.search(r'stack0x|\._\d|_conflict',body),a
        native=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,canonical_calls=[],callback_references=[])
        for ins in decoder.disasm(native,int(a,16)):
            for o in ins.operands:
                if o.type!=X86_OP_IMM or o.imm not in targets:continue
                if a in HELPERS and o.imm==0x26dd200:
                    detail['inline_wrapper_call']=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target='26dd200',source_wrapper='26e09a0');continue
                target=targets[o.imm];name=target['name'];assert re.search(r'\b'+name+r'\b',body),(a,name)
                assert not target['static'] or target['source']==row['source']
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{o.imm:x}',name=name)
                (detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail
        entry=dict(address=a,name=row['name'],source=row['source'],line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,protection_frontend_evidence=detail,
            reconstruction='Restore the already recovered guarded public protection-source wrapper, or the DWARF-proven 151-int Tanazir snapshot and reverse iteration; no native behavior repairs.',reconstruction_body_sha256=sha(function_body(body,row['name']).encode()))
        if a in HELPERS:entry['known_native_behavior']='The guarded wrapper leaves p/c untouched when attacking_card is negative, matching the inlined native conditional; no initial values are invented.'
        entries.append(entry)
        retained=[f['prototype'] for f in m['accepted_functions'] if f['source']==row['source'] and f['static'] and re.search(r'\b'+f['name']+r'\b',body)]
        compile_sources(root,cache/'protection-frontends-check',{row['source']:'#include "manalink.h"\n'+'\n'.join(retained)+'\n'+body})
        source=sources.get(row['source'],read(root/row['source']));sources[row['source']]=source+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {row["source"]}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');block=HEADER_LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(f['prototype'] for f in entries if f['address'] in HELPERS)+'\n#endif\n\n';header=header[:at]+block+header[at:]
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=5;m['card_handler_functions']+=1;m['helper_functions']+=4;m['shared_helper_interfaces']+=4
        for entry in entries:m['source_files'][entry['source']]+=1
        save_json(mp,m);interfaces['protection_frontends_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'protection-frontends-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_card_functions=1,new_public_helpers=4)))


if __name__=='__main__':main()
