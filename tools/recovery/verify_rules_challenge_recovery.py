"""Audit rules/challenge recovery, inline ownership, native storage and deferred UI."""
import ast,csv,json,re,struct
from elftools.dwarf.locationlists import LocationParser
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value,value
from export_abi import type_of


def without_comments(text):
    # Preserve // inside strings such as Faces//%s.jat.
    return re.sub(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*[\s\S]*?\*/|//[^\n]*',
                  lambda m:'' if m[0].startswith(('/',)) else m[0],text)


def integer(expr,names):
    expr=re.sub(r'\((?:int|unsigned int)\)','',expr.strip())
    expr=re.sub(r'(?<=\d)[Uu]\b','',expr)
    for name in re.findall(r'\b[A-Za-z_]\w*\b',expr):
        assert name in names, name
        expr=re.sub(r'\b'+name+r'\b',str(names[name]),expr)
    assert re.fullmatch(r'[\d\s()|<+xXa-fA-F\-]+',expr),expr
    return eval("("+expr+")",{"__builtins__":{}},{})


def verify(proof,image,root=ROOT):
    path=proof.get('native_rules_challenge_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text())
    assert sha(image.data)==record['dll_sha256']
    inventory={r['low_pc']:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc']}
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    def body(row):
        text=read(root/row['source']);a,b=span(text,row['name']);return text[a:b]
    for row in record['helpers']+record['support_helpers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
        assert sha(body(row).encode())==row['definition_sha256'],row['name']
        for part in row['native_ranges']:
            lo=int(part['address'],16);hi=int(part['end'],16);native=inventory[part['address']]
            assert native['name']==row['name'] and native['high_pc']==part['end']
            assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==part['bytes_sha256']
            assert sha((root/part['raw_export']).read_bytes())==part['raw_export_sha256']
    for row in record['source_dependencies']:
        assert sha(body(row).encode())==row['definition_sha256']
        assert any(e['source']==row['source'] and e.get('native_inline_source_dependency',{}).get('function')==row['name']
                   and e['after_sha256']==row['definition_sha256'] for e in proof['source_edits'])
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)==record['dbg_sha256']=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dw=original.dwarf();parser=LocationParser(dw.location_lists())
    def fn(at):return dw.get_DIE_from_refaddr(int(inventory[at]['die_offset'],16))
    def locs(d):
        a=d.attributes.get('DW_AT_location')
        if not a:return []
        p=parser.parse_from_attribute(a,d.cu['version'],die=d)
        return [p.loc_expr] if hasattr(p,'loc_expr') else [r.loc_expr for r in p if hasattr(r,'loc_expr')]
    def abi(at):
        return {inherited_value(d,'DW_AT_name'):dict(first_location=next(iter(locs(d)),None),constant=inherited_value(d,'DW_AT_const_value'))
                for d in fn(at).iter_children() if d.tag=='DW_TAG_formal_parameter'}
    actual={at:abi(at) for at in record['optimized_parameter_evidence']}
    assert actual==record['optimized_parameter_evidence'] and len(actual)==42
    assert actual['0x26ba660']['unlock_level']['first_location']==[80]
    assert actual['0x26ba760']['unlock_level']['first_location']==[145,4]
    assert {n:r['first_location'] for n,r in actual['0x26ba5a0'].items()}==dict(type=[80],type_flag=[82],text=[81])
    for row in record['native_frame_arrays']:
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        assert d.tag=='DW_TAG_variable' and inherited_value(d,'DW_AT_name')==row['name']
        assert d.cu==fn(row['owner']).cu
        t=type_of(d);assert t.tag=='DW_TAG_array_type';dims=[]
        for s in t.iter_children():
            a=s.attributes['DW_AT_upper_bound'];v=dict(form=a.form)
            if a.form=='DW_FORM_ref4':v['expressions']=locs(s.get_DIE_from_attribute('DW_AT_upper_bound'))
            elif a.form=='DW_FORM_exprloc':v['expressions']=[a.value]
            else:
                assert a.form in {'DW_FORM_data1','DW_FORM_data2','DW_FORM_data4'}
                v['extent']=a.value+1
            dims.append(v)
        assert dims==row['dimensions']
        e=type_of(t)
        while e and 'DW_AT_byte_size' not in e.attributes and 'DW_AT_type' in e.attributes:e=type_of(e)
        assert value(e,'DW_AT_byte_size')==row['element_bytes']
    arrays=record['native_frame_arrays'];assert len(arrays)==27
    hand={r['name']:r for r in arrays if r['owner']=='0x26c1820'}
    assert [d['extent'] for d in hand['opts']['dimensions']]==[100,2]
    assert hand['buffer']['dimensions'][0]['expressions']==[[10,183,11,159]]*11  # const2u2999, stack_value: upper bound.
    assert next(r for r in arrays if r['owner']=='0x26c1230')['dimensions'][0]['extent']==32
    for row in record['native_globals']:
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        assert d.tag=='DW_TAG_variable' and inherited_value(d,'DW_AT_name')==row['name']
        assert locs(d)==row['locations'] and value(d,'DW_AT_external')==row['external']
        t=type_of(d);dims=[]
        if t and t.tag=='DW_TAG_array_type':
            dims=[value(s,'DW_AT_upper_bound')+1 for s in t.iter_children() if value(s,'DW_AT_upper_bound') is not None]
        assert dims==row['dimensions']
        if 'initial_value' in row:
            assert int.from_bytes(image.pe.get_data(int(row['address'],16)-0x2000000,4),'little')==row['initial_value']
    assert len(record['native_globals'])==13
    partitions=[dict(symbol=n,address=hex(image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[s-1].VirtualAddress+a),storage=st)
                for n,a,s,k,st in original.symbols if n.startswith(('_is_unlocked_by_challenge','_read_settings'))]
    assert partitions==record['native_compiler_partitions'] and len(partitions)==4
    def walk(d):
        yield d
        for c in d.iter_children():yield from walk(c)
    for row in record['native_inline_dependencies']:
        instances=[dict(die_offset=hex(d.offset),low_pc=value(d,'DW_AT_low_pc'),high_pc=value(d,'DW_AT_high_pc'),ranges=value(d,'DW_AT_ranges'),abstract_origin=value(d,'DW_AT_abstract_origin'))
                   for d in walk(fn(row['owner'])) if d.tag=='DW_TAG_inlined_subroutine' and inherited_value(d,'DW_AT_name')==row['name']]
        assert instances==row['instances'] and instances
    assert {r['name'] for r in record['native_inline_dependencies']}=={'vanguard_check','check_deck_legality','limit_allowed_in_deck'}

    # Verify complete instruction ranges, not just isolated byte patterns.
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    md=Cs(CS_ARCH_X86,CS_MODE_32)
    for row in record['native_instruction_blocks'].values():
        lo=int(row['address'],16);hi=int(row['end'],16);owner=inventory[row['owner']]
        assert int(owner['low_pc'],16)<=lo<hi<=int(owner['high_pc'],16)
        data=image.pe.get_data(lo-0x2000000,hi-lo);assert sha(data)==row['bytes_sha256']
        ins=list(md.disasm(data,lo));assert sum(i.size for i in ins)==hi-lo
        assert [[hex(i.address),i.mnemonic,i.op_str] for i in ins]==row['instructions']
    assert len(record['native_instruction_blocks'])==42
    names={n:int(v) for n,v in re.findall(r'\b(CARD_ID_\w+)\s*=\s*(-?\d+)',read(root/'card_id.h'))}
    challenge=without_comments(read(root/'functions/challenge_mode.c'))
    names.update({n:int(v) for n,v in re.findall(r'\b(CD_\w+)\s*=\s*(-?\d+)',challenge)})
    for row in record['native_static_tables']:
        data=bytes.fromhex(row['native_hex'])
        assert len(data)==row['bytes'] and sha(data)==row['bytes_sha256']
        assert image.pe.get_data(int(row['address'],16)-0x2000000,len(data))==data
        text=without_comments(read(root/row['source']))
        if row.get('function'):
            a,b=span(text,row['function']);text=text[a:b]
        m=re.search(r'\b'+row['name']+r'\s*(?:\[[^\]]*\]\s*)+=\s*\{([\s\S]*?)\}\s*;',text);assert m,row['name']
        if row['name'] in {'unlock_codes','challenge_names'}:
            stride=10 if row['name']=='unlock_codes' else 50
            strings=[ast.literal_eval(s).encode('latin1') for s in re.findall(r'"(?:\\.|[^"\\])*"',m[1])]
            assert all(len(s)<stride for s in strings)
            packed=b''.join(s+b'\0'*(stride-len(s)) for s in strings)
        else:
            values=[integer(s,names) for s in m[1].split(',') if s.strip()]
            packed=b''.join(struct.pack('<I',v&0xffffffff) for v in values)
        packed=packed.ljust(len(data),b'\0');assert packed==data,row['name']
    assert len(record['native_static_tables'])==8
    rules=read(root/'functions/rules_engine.c');defs=read(root/'defs.h')
    for group,text in [('native_graveyard_flags',rules),('native_grave_ability_modes',defs),('native_restored_card_ids',read(root/'card_id.h'))]:
        source_names={}
        for row in record[group]:
            die=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
            assert die.tag=='DW_TAG_enumerator' and value(die,'DW_AT_name')==row['name']
            assert value(die,'DW_AT_const_value')==row['value']
            m=re.search(r'\b'+row['name']+r'\s*=\s*([^,}]+)',without_comments(text));assert m,row['name']
            v=integer(m[1],source_names)
            assert (v&0xffffffff)==(row['value']&0xffffffff),(row['name'],v)
            source_names[row['name']]=v
    assert len(record['native_graveyard_flags'])==32 and len(record['native_grave_ability_modes'])==15
    assert len(record['native_restored_card_ids'])==9
    assert 'unlimited_allowed_in_deck' not in rules
    assert 'counts[id] > limit_allowed_in_deck(id)' in rules
    assert 'static int companion[2][10];' in rules and 'static int ccount[2];' in rules

    # The actual IID chooser is explicitly unsafe/deferred, never credited as recovered.
    boundary=record['deferred_boundaries'];assert len(boundary)==1
    stub=boundary[0];assert not stub['counted_as_recovered']
    expected=dict(source=stub['source'],name=stub['name'],native_address=stub['native_address'],return_value=-1)
    assert expected in proof['additional_source_stubs']
    assert abi('0x26ef7a0')==record['deferred_chooser_abi']
    assert {n:r['first_location'] for n,r in record['deferred_chooser_abi'].items()}==dict(prompt=[145,0],colors=[145,4],types=[145,8])
    from verify_cpp_chooser_recovery import resolved_stub_addresses
    if int(stub['native_address'],16) not in resolved_stub_addresses(proof,root):
        assert sha(body(stub).encode())==stub['definition_sha256']
        assert re.search(r'return\s+-1\s*;',body(stub)) and 'TODO(recovery-ui)' in read(root/stub['source'])
    accepted=json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text())['accepted_functions']
    assert not any(int(r['address'],16)==int(stub['native_address'],16) for r in accepted)
    assert not any(int(p['address'],16)==int(stub['native_address'],16) for r in record['helpers']+record['support_helpers'] for p in r['native_ranges'])
    assert len(record['helpers'])==38 and sum(len(r['native_ranges']) for r in record['helpers'])==40
    assert sum(r['source_changed'] for r in record['helpers'])==11
    assert len(record['support_helpers'])==2 and not any(r['source_changed'] for r in record['support_helpers'])
    assert len(record['source_dependencies'])==2 and all(r['source_changed'] for r in record['source_dependencies'])
    return dict(helper_definitions=38,native_helper_ranges=40,changed_helpers=11,reviewed_support_helpers=2,
                changed_inline_dependencies=2,native_frame_arrays=27,native_static_tables=8,
                native_instruction_blocks=42,deferred_chooser_boundaries=1,
                scope='42 complete native ranges and two concrete inline source dependencies reviewed; deferred IID chooser is excluded from recovered-function credit.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
