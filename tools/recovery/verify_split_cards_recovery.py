"""Audit split-DWARF helpers and the first changed-card recovery batch.

Split functions are counted as functions, separately from their discontiguous
machine-code ranges. The ordinary helper triage only inventories low_pc bodies.
"""
import csv,json,re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from elftools.dwarf.locationlists import LocationParser
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value,value
from export_abi import type_name,type_of


def native_ranges(dw,die):
    if 'DW_AT_ranges' in die.attributes:
        base=0;out=[]
        for r in dw.range_lists().get_range_list_at_offset(die.attributes['DW_AT_ranges'].value,cu=die.cu):
            if hasattr(r,'base_address'):base=r.base_address
            elif hasattr(r,'begin_offset'):
                offset=0 if r.is_absolute else base
                out.append((offset+r.begin_offset,offset+r.end_offset))
        return out
    lo=value(die,'DW_AT_low_pc');a=die.attributes.get('DW_AT_high_pc')
    return [(lo,a.value+(0 if a.form=='DW_FORM_addr' else lo))] if lo is not None and a else []


def contract(dw,die):
    parser=LocationParser(dw.location_lists())
    def locations(d):
        a=d.attributes.get('DW_AT_location')
        if not a:return []
        p=parser.parse_from_attribute(a,d.cu['version'],die=d)
        return [p.loc_expr] if hasattr(p,'loc_expr') else [r.loc_expr for r in p if hasattr(r,'loc_expr')]
    return dict(return_type=type_name(type_of(die)),parameters=[dict(name=inherited_value(d,'DW_AT_name'),
        type=type_name(type_of(d)),locations=locations(d),constant=inherited_value(d,'DW_AT_const_value'))
        for d in die.iter_children() if d.tag=='DW_TAG_formal_parameter'])


def walk(die):
    for d in die.iter_children():
        yield d
        yield from walk(d)


def evidence(proof,root=ROOT):
    p=proof.get('native_split_cards_recovery')
    return json.loads((root/p).read_text()) if p else None


def split_coverage(proof,root=ROOT):
    record=evidence(proof,root)
    if not record:return dict(functions=0,code_ranges=0,pending_functions=3)
    return dict(functions=len(record['split_helpers']),code_ranges=sum(len(r['native_ranges']) for r in record['split_helpers']),
        pending_functions=0,names=[r['name'] for r in record['split_helpers']],
        evidence=proof['native_split_cards_recovery'],scope='Separate split-DWARF census; not included in contiguous helper-range totals.')


def verify(proof,image,root=ROOT):
    record=evidence(proof,root)
    if not record:return {}
    assert sha(image.data)==record['dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(debug.data)==record['dbg_sha256']
    dw=debug.dwarf();md=Cs(CS_ARCH_X86,CS_MODE_32)
    inventory=list(csv.DictReader((root/'.recovery/2022-functions.csv').open()))
    split={r['name'] for r in inventory if r['ranges'] and r['declaration']=='False'}
    assert split=={r['name'] for r in record['split_helpers']} and len(split)==5
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256'])
              for e in proof['source_edits']}
    for row in record['split_helpers']+record['cards']+record['support_helpers']:
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        assert inherited_value(d,'DW_AT_name')==row['name']
        assert contract(dw,d)==row['contract'],row['name']
        assert native_ranges(dw,d)==[(int(r['address'],16),int(r['end'],16)) for r in row['native_ranges']]
        t=read(root/row['source']);a,b=span(t,row['name'])
        assert sha(t[a:b].encode())==row['definition_sha256'],row['name']
        if row.get('reviewed_in_this_batch'):
            assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
        for part in row['native_ranges']:
            lo=int(part['address'],16);hi=int(part['end'],16);raw=image.pe.get_data(lo-0x2000000,hi-lo)
            assert sha(raw)==part['bytes_sha256'] and len(raw)==hi-lo
            assert sum(i.size for i in md.disasm(raw,lo))==hi-lo,(row['name'],part['address'])
            assert sha((root/part['raw_export']).read_bytes())==part['raw_export_sha256']
    for row in record['source_dependencies']:
        t=read(root/row['source']);a,b=span(t,row['name']);assert sha(t[a:b].encode())==row['definition_sha256'],row['name']
        for instance in row['inline_instances']:
            d=dw.get_DIE_from_refaddr(int(instance['die_offset'],16))
            assert d.tag=='DW_TAG_inlined_subroutine' and inherited_value(d,'DW_AT_name')==row['name']
            assert native_ranges(dw,d)==[(int(p['address'],16),int(p['end'],16)) for p in instance['ranges']]
            for part in instance['ranges']:
                lo=int(part['address'],16);hi=int(part['end'],16)
                assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==part['bytes_sha256']
    for row in record['enumerators']:
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        assert d.tag=='DW_TAG_enumerator' and value(d,'DW_AT_name')==row['name'] and value(d,'DW_AT_const_value')==row['value']
        m=re.search(r'\b'+row['name']+r'\s*=\s*([^,\n]+)',read(root/'defs.h'));assert m
        assert re.fullmatch(r'[\dxa-fA-F\s<]+',m[1]) and eval(m[1],{'__builtins__':{}},{})==row['value']
    for row in record['retired_predecessors']:
        assert not re.search(r'\b'+row['name']+r'\b',read(root/row['source'])),row['name']
    # Every source/export is byte-pinned; this is manual body review, not a
    # claim of identical generated code or tested whole-program gameplay.
    return dict(split_helpers=split_coverage(proof,root),changed_card_handlers=len(record['cards']),
        support_helpers=len(record['support_helpers']),inline_source_dependencies=len(record['source_dependencies']),
        retired_predecessors=len(record['retired_predecessors']),scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
