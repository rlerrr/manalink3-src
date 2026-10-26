"""Restore typed stack unions proven by overlapping original DWARF locals."""
import argparse,csv,json,re,shutil
from pathlib import Path
from canonicalize_records import verify as verify_records
from unify_scalar_helpers import read,span,mask,sha
from audit_source import function_body

ROOT=Path(__file__).resolve().parents[2]
EVIDENCE='docs/recovery-evidence/canonical-record-storage.json'
# Function: retained scratch declaration, native locals sharing its fbreg slot.
SPECS={
 'card_lord_windgrace':('int dead[10];',{'token_generation_t':'token'}),
 'card_vraska_relic_seeker':('char buffer[50];',{'token_generation_t':'token'}),
 'card_reign_of_terror':('char buffer[100];',{'test_definition_t':'this_test'}),
 'card_fast_furious':('int totals[2];',{'test_definition_t':'this_test'}),
 'card_kayas_guile':('int priority[6];',{'test_definition_t':'this_test','token_generation_t':'token'}),
 'card_mana_cache':('int found[2];',{'test_definition_t':'this_test'}),
 'card_slaughter_the_strong':('char msg[100];',{'test_definition_t':'this_test'}),
}

def project(definition,name):
    declaration,views=SPECS[name];old=re.search(r'\b(\w+)\[',declaration)[1]
    assert definition.count(declaration)==1
    text=definition.replace(declaration,'@CANONICAL_STORAGE@',1)
    tokens=list(re.finditer(r'\b'+old+r'\b',mask(text)))
    for m in reversed(tokens):text=text[:m.start()]+'storage.'+old+text[m.end():]
    for typename,member in views.items():
        text,n=re.subn(r'\('+typename+r'\s*\*\)\s*storage\.'+old+r'\b','&storage.'+member,text)
        assert n>0,(name,typename)
    if name=='card_lord_windgrace':
        original='storage.dead[8] = get_hacked_walk(player, card, 4);'
        assert text.count(original)==1;text=text.replace(original,'storage.token.key_plus = get_hacked_walk(player, card, 4);')
    if name=='card_vraska_relic_seeker':
        original="storage.buffer[0x24] = '\\0';\n      storage.buffer[0x25] = '\\0';\n      storage.buffer[0x26] = '\\x01';\n      storage.buffer[0x27] = '\\0';"
        assert text.count(original)==1;text=text.replace(original,'storage.token.s_key_plus = 0x10000;')
    union='union\n  {\n    '+declaration+'\n'+''.join('    '+t+' '+m+';\n' for t,m in views.items())+'  } storage;'
    return text.replace('@CANONICAL_STORAGE@',union,1)

def audit_sources(sources,root=ROOT,machine_bytes=None):
    path=root/EVIDENCE
    if not path.exists():return dict(functions=0)
    proof=json.loads(path.read_text());assert proof['format']==1 and set(proof['functions'])==set(SPECS)
    debug_path=Path(proof['native_debug_path']);assert sha(debug_path.read_bytes())==proof['native_debug_sha256']
    from inspect_release import Image,value,inherited_value
    from export_abi import type_of,type_name
    dwarf=Image(debug_path.read_bytes()).dwarf()
    for name,e in proof['functions'].items():
        before=e['predecessor_definition'];after=project(before,name)
        assert sha(before.encode())==e['predecessor_sha256']
        assert sha(after.encode())==e['canonical_sha256'] and after==e['canonical_definition']
        actual=read(root/e['source']);b=span(actual,name);assert b and actual[b[0]:b[1]]==after
        assert all(e['locals'][v]['location']==e['scratch_location'] for v in e['locals'])
        declaration,views=SPECS[name];old=re.search(r'\b(\w+)\[',declaration)[1]
        assert set(e['locals'])=={old,*views.values()}
        for v,local in e['locals'].items():
            die=dwarf.get_DIE_from_refaddr(int(local['die'],16))
            assert die.tag=='DW_TAG_variable' and inherited_value(die,'DW_AT_name')==v
            assert type_name(type_of(die))==local['type']
            assert die.attributes['DW_AT_location'].form=='DW_FORM_exprloc'
            assert list(die.attributes['DW_AT_location'].value)==e['scratch_location']
        if machine_bytes is not None:
            assert sha(machine_bytes(int(e['native_address'],16),e['native_bytes']))==e['native_sha256']
        # Only the independently checked representation change is inverted.
        # All earlier raw/decompiler, literal and algorithm proofs still run
        # against their exact input; no recovery record is re-hashed away.
        if e['source'] in sources:
            text=sources[e['source']];b=span(text,name);assert b
            assert text[b[0]:b[1]].replace('\r\n','\n')==after
            sources[e['source']]=text[:b[0]]+before+text[b[1]:]
    return dict(functions=len(proof['functions']),evidence='original DWARF locals share one frame-base offset; full typed unions restore each storage extent',
                live_source='deterministic projection and full definition hashes checked; historical raw/algorithm proofs replay the checked predecessor')

def verify_build(debug,root=ROOT):
    from inspect_release import value,inherited_value
    from export_abi import type_of
    from pack_native_tables import unqual
    path=root/EVIDENCE
    if not path.exists():return {}
    proof=json.loads(path.read_text());found={}
    sizes=verify_records(root)['runtime_bytes']
    sources={e['source'] for e in proof['functions'].values()}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in sources:continue
        for d in cu.iter_DIEs():
            if d.tag!='DW_TAG_variable' or inherited_value(d,'DW_AT_name')!='storage':continue
            parent=d.get_parent()
            while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
            name=inherited_value(parent,'DW_AT_name') if parent is not None else None
            if name not in SPECS:continue
            t=unqual(type_of(d));assert t.tag=='DW_TAG_union_type'
            declaration,views=SPECS[name];old=re.search(r'\b(\w+)\[',declaration)[1]
            fields={value(c,'DW_AT_name'):c for c in t.iter_children() if c.tag=='DW_TAG_member'}
            assert set(fields)=={old,*views.values()}
            assert all(value(c,'DW_AT_data_member_location',0)==0 for c in fields.values())
            expected=max(sizes[typ] for typ in views);assert value(t,'DW_AT_byte_size')==expected
            for typ,v in views.items():assert value(unqual(type_of(fields[v])),'DW_AT_byte_size')==sizes[typ]
            assert name not in found;found[name]=dict(bytes=expected,members=list(fields),all_member_offsets=0)
    assert set(found)==set(SPECS),(set(SPECS)-set(found))
    return found

def main():
    p=argparse.ArgumentParser(__doc__);p.add_argument('--apply',action='store_true');p.add_argument('--verify',action='store_true');args=p.parse_args()
    verify_records()
    from inspect_release import Image,value,inherited_value
    from export_abi import type_of,type_name
    runtime=Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())
    def machine(a,n):return runtime.pe.get_data(a-runtime.pe.OPTIONAL_HEADER.ImageBase,n)
    if args.verify:print(json.dumps(audit_sources({},machine_bytes=machine),indent=2));return
    assert not (ROOT/EVIDENCE).exists(),'Already restored'
    debug_path=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg');debug_data=debug_path.read_bytes();debug=Image(debug_data)
    native={int(r['low_pc'],16):r for r in csv.DictReader((ROOT/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    ledger=json.loads((ROOT/'docs/recovery-evidence/recovered-functions.json').read_text())
    entries={f['name']:f for f in ledger['accepted_functions'] if f['name'] in SPECS};assert set(entries)==set(SPECS)
    locals_by_name={}
    for cu in debug.dwarf().iter_CUs():
        for d in cu.get_top_DIE().iter_children():
            name=inherited_value(d,'DW_AT_name')
            if d.tag!='DW_TAG_subprogram' or name not in SPECS:continue
            found=[]
            def walk(p):
                for c in p.iter_children():
                    if c.tag=='DW_TAG_variable' and 'DW_AT_location' in c.attributes and c.attributes['DW_AT_location'].form=='DW_FORM_exprloc':
                        found.append(dict(name=inherited_value(c,'DW_AT_name'),type=type_name(type_of(c)),location=list(c.attributes['DW_AT_location'].value),die=hex(c.offset)))
                    if c.has_children and c.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:walk(c)
            walk(d);locals_by_name[name]=found
    sources={};proof=dict(format=1,native_debug_path=str(debug_path),native_debug_sha256=sha(debug_data),functions={},
                         scope='Restore compiler-shared stack storage from original DWARF. No algorithm/call/event changes; recovery records remain unchanged.')
    for name,f in entries.items():
        declaration,views=SPECS[name];old=re.search(r'\b(\w+)\[',declaration)[1]
        locals=locals_by_name[name];scratch=next(l for l in locals if l['name']==old)
        expected={old:scratch}
        for typ,v in views.items():
            expected[v]=next(l for l in locals if l['name']==v and l['type']==typ and l['location']==scratch['location'])
        assert scratch['location'][0]==0x91 # DW_OP_fbreg, followed by signed LEB128.
        source=f['source'];text=sources.setdefault(source,read(ROOT/source));b=span(text,name);assert b
        before=text[b[0]:b[1]];after=project(before,name);a=int(f['address'],16);n=int(native[a]['high_pc'],16)-a
        e=dict(source=source,native_address=f['address'],native_bytes=n,native_sha256=sha(machine(a,n)),
               predecessor_definition=before,predecessor_sha256=sha(before.encode()),canonical_definition=after,canonical_sha256=sha(after.encode()),
               predecessor_body_sha256=sha(function_body(before,name).encode()),canonical_body_sha256=sha(function_body(after,name).encode()),
               scratch_location=scratch['location'],locals=expected)
        proof['functions'][name]=e;sources[source]=text[:b[0]]+after+text[b[1]:]
    if args.apply:
        backup=ROOT/'.recovery/canonical-record-storage-before';assert not backup.exists();backup.mkdir()
        for source,text in sources.items():
            target=backup/source;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(ROOT/source,target)
            (ROOT/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        shutil.copy2(ROOT/'docs/recovery-evidence/recovered-functions.json',backup/'recovered-functions.json')
        (ROOT/EVIDENCE).write_text(json.dumps(proof,indent=2)+'\n')
        audit_sources({},machine_bytes=machine)
    print(json.dumps(dict(applied=args.apply,functions=list(proof['functions'])),indent=2))

if __name__=='__main__':main()
